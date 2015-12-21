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
#include <errno.h>
#include <cstring>
#include <string>
#include "definitions.h"
#include "fs_helper.h"
#include <cctype>
#include <fcntl.h>

using std::string;

static char help_text[DATA_SIZE] =
"commands:\n\
help\n\
pwd\n\
cd\n\
ls\n\
get\n\
put\n\
quit\n";
static char errmsg[200];


Server::Server()
        : is_daemon(true), port(21),
          listen_fd(-1), command_fd(-1), data_fd(-1)
{
}


Server::Server(unsigned short port_num)
        : is_daemon(false), port(port_num),
          listen_fd(-1), command_fd(-1), data_fd(-1)
{
}

void Server::run()
{
    listen_fd = listen_sock_fd(port);
    while (true)
    {
        SA_in client_addr;
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
    const char * remote_client = gethostbyaddr(&client_addr.sin_addr,
                                 sizeof(client_addr.sin_addr),
                                 AF_INET)->h_name;
    connect_data(remote_client);
    int command;
    char buf[DATA_SIZE + 1];
    char * data_client = buf;
    shutdown(listen_fd, SHUT_RDWR);
    listen_fd = -1;
    while(true)
    {
        robust_readn(command_fd, &command, sizeof(command));
        int reply = REQ_OK;
        int data_length = 0;
        string ls_string;
        switch(command)
        {
        case FTP_CD:
        case FTP_LS:
        case FTP_PUT:
        case FTP_GET:
            if(robust_readn(data_fd, buf, DATA_SIZE) == -1)
            {
                reply = REQ_ERR;
                robust_writen(command_fd, &reply, sizeof(reply));
            }
            buf[DATA_SIZE] = 0;
            while(*data_client != 0 && isspace(*data_client)) data_client++; 
            break;
        default:
            break;
        }
        switch(command)
        {
        case FTP_HELP:
            printf("help\n");
            robust_writen(command_fd, &reply, sizeof(reply));
            robust_writen(data_fd, help_text, DATA_SIZE);
            break;
        case FTP_LS:
            printf("ls\n");
            handle_ls(data_client);
            break;
        case FTP_PWD:
            printf("pwd\n");
            getcwd(buf, DATA_SIZE);
            robust_writen(command_fd, &reply, sizeof(reply));
            robust_writen(data_fd, buf, DATA_SIZE);
            break;
        case FTP_CD:
            printf("cd\n");
            handle_cd(data_client);
            break;
        case FTP_PUT:
            printf("put\n");
            handle_put(data_client);
            break;
        case FTP_GET:
            printf("get\n");
            handle_get(data_client);
            break;
        case FTP_EXIT:
            printf("quit\n");
            printf("%s quitted\n", remote_client);
            return;
        default:
            reply = REQ_DENY;
            robust_writen(command_fd, &reply, sizeof(reply));
        }
    }
}

void Server::handle_ls(const char * data_client)
{
    char buf[DATA_SIZE];
    buf[0] = 0;
    const char * dir;
    int reply = REQ_OK;
    sscanf(data_client, "%s", buf);
    dir = buf[0] ? buf : ".";
    string ls_string;
    try
    {
        ls_string = get_dir_content(dir);
    }
    catch(FSException & e)
    {
        reply = REQ_UNFOUND;
        robust_writen(command_fd, &reply, sizeof(reply));
        return;
    }
    robust_writen(command_fd, &reply, sizeof(reply));
    int length = ls_string.size() + 1;
    robust_writen(data_fd, &length, sizeof(length));
    robust_writen(data_fd, ls_string.c_str(), ls_string.size() + 1);
}

void Server::handle_cd(const char * data_client)
{
    char buf[DATA_SIZE];
    int reply = REQ_OK;
    if(chdir(data_client) == -1)
    {
        switch(errno)
        {
        case EACCES:
            reply = REQ_DENY;
            break;
        case ENOENT:
            reply = REQ_UNFOUND;
            break;
        default:
            reply = REQ_ERR;
        }
        robust_writen(command_fd, &reply, sizeof(reply));
        return;
    }
    robust_writen(command_fd, &reply, sizeof(reply));
    getcwd(buf, DATA_SIZE);
    robust_writen(data_fd, buf, DATA_SIZE);
}

void Server::handle_put(const char * data_client)
{
    int reply = REQ_OK;
    try
    {
        int file_fd = open(data_client, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        char buf[DATA_SIZE];
        if(file_fd < 0)
        {
            throw FSException("cannot open file");
        }
        robust_writen(command_fd, &reply, sizeof(reply));
        recv_from_socket(data_fd, file_fd);
        close(file_fd);
        return;
    }
    catch(FSException & e)
    {
        fprintf(stderr, "%s\n", e.what());
        switch(errno)
        {
        case EACCES:
            reply = REQ_DENY;
            break;
        case ENOENT:
            reply = REQ_UNFOUND;
            break;
        default:
            reply = REQ_ERR;
        }
    }
    robust_writen(command_fd, &reply, sizeof(reply));
    return;
}

void Server::handle_get(const char * data_client)
{
    int reply = REQ_OK;
    try
    {
        int file_fd = open(data_client, O_RDONLY);
        char buf[DATA_SIZE];
        if(file_fd < 0)
        {
            throw FSException("cannot open file");
        }
        robust_writen(command_fd, &reply, sizeof(reply));
        send_to_socket(data_fd, file_fd);
        close(file_fd);
        return;
    }
    catch(FSException & e)
    {
        fprintf(stderr, "%s d\n", e.what());
        switch(errno)
        {
        case EACCES:
            reply = REQ_DENY;
            break;
        case ENOENT:
            reply = REQ_UNFOUND;
            break;
        default:
            reply = REQ_ERR;
            break;
        }
    }
    printf("send reply %d\n", reply);
    robust_writen(command_fd, &reply, sizeof(reply));
    return;
}

void Server::connect_data(const char * remote_client)
{
    unsigned short data_port;
    if(robust_readn(command_fd, &data_port, sizeof(data_port)) != sizeof(data_port))
    {
        sprintf(errmsg, "Error get data link port from client %s", remote_client);
        throw SockException(errmsg);
    }
    unsigned wait_time = 1;
    int retry_count = 0;
    sleep(1);
    while(true)
    {
        try
        {
            data_fd = connect_sock_fd(remote_client, data_port);
            if(data_fd > 0) break;
        }
        catch(SockException & e)
        {
            retry_count++;
            if(retry_count == NUM_RETRY)
            {
                sprintf(errmsg, "Error set data link port to client %s", remote_client);
                throw SockException(errmsg);
            }
            wait_time *= 2;
            sleep(wait_time);
        }
    }
    printf("connected to client %s:%d\n", remote_client, data_port);
}

Server::~Server()
{
    stop();
}

void Server::stop()
{
    if (listen_fd != -1) shutdown(command_fd, SHUT_RDWR);
    if (command_fd != -1) shutdown(command_fd, SHUT_RDWR);
    if (data_fd != -1) shutdown(command_fd, SHUT_RDWR);
}
