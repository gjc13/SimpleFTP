//
// Created by 郭嘉丞 on 15/12/19.
//

#include "Server.h"
#include <netdb.h>
#include <cstdio>
#include "net_helper.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>

extern bool is_stopped;

Server::Server()
        : is_daemon(true), port(21),
          listen_fd(-1), command_fd(-1), data_fd(-1)
{
}


Server::Server(int port_num)
        : is_daemon(false), port(port_num),
          listen_fd(-1), command_fd(-1), data_fd(-1)
{
}

void Server::run()
{
    listen_fd = listen_sock_fd(port);
    while (!is_stopped)
    {
        struct sockaddr_in client_addr;
        unsigned length = sizeof(client_addr);
        command_fd = accept(listen_fd, (SA *) &client_addr, &length);
        if (command_fd == -1)
        {
            throw SockException("Error in accept client");
        }
        pid_t pid = fork();
        if (pid == -1)
        {
            throw SockException("Fork subprocess failed");
        }
        if (pid == 0)
        {
            is_daemon = false;
            handle_link(client_addr);
            break;
        }
    }
}

void Server::handle_link(const SA_in &client_addr)
{
    printf("%s\n", gethostbyaddr(&client_addr.sin_addr,
                                 sizeof(client_addr.sin_addr),
                                 AF_INET)->h_name);
}

Server::~Server()
{
    stop();
}

void Server::stop()
{
    if (is_daemon)
    {
        if (listen_fd != -1) close(listen_fd);
    }
    else
    {
        if (command_fd != -1) close(command_fd);
        if (data_fd != -1) close(data_fd);
    }
}
