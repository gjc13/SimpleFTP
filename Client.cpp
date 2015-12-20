//
// Created by 郭嘉丞 on 15/12/19.
//

#include "Client.h"
#include <cstdio>
#include <cstring>
#include "definitions.h"

Client::Client(const char *hostname, unsigned short port)
        : server_name(hostname), server_port(port),
          command_fd(-1), data_fd(-1)
{
}


void Client::connect()
{
    command_fd = connect_sock_fd(server_name, server_port);
    unsigned short port = 0;
    int data_listen_fd = listen_sock_fd(port);
    robust_writen(command_fd, &port, sizeof(port));
    SA_in server_addr;
    unsigned length = sizeof(server_addr);
    data_fd = accept(data_listen_fd, (SA *) &server_addr, &length);
    if(data_fd == -1)
    {
        throw SockException("Error in accept server data connection");
    }
    printf("Connected to server %s:%d\n", server_name, server_port);
    char input[DATA_SIZE + 1];
    while(true)
    {
        printf("%s>", server_name);
        fflush(stdout);
        fgets(input, 1000, stdin); 
        input[DATA_SIZE] = 0;
        char command[DATA_SIZE + 1];
        int numRead = sscanf(input, "%s", command);
        if(!strcmp(command, "help"))
        {
            handle_help();
        }
    }
    close(data_listen_fd);
}

int Client::handle_help()
{
    char help_text[DATA_SIZE];
    int command = FTP_HELP;
    robust_writen(command_fd, &command, sizeof(command));
    if(robust_readn(data_fd, help_text, DATA_SIZE) >= 0)
    {
        printf("%s\n", help_text);
        return 0;
    }
    return -1;
}

Client::~Client()
{
    if(command_fd != -1) close(command_fd);
    if(data_fd != -1) close(data_fd);
}
