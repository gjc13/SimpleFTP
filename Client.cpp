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
    char input[2 * DATA_SIZE + 1];
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
        else if(!strcmp(command, "ls"))
        {
            handle_ls(input + strlen(command));
        }
        fflush(stdin);
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

int Client::handle_ls(const char * command_str)
{
    char buf[DATA_SIZE];
    int ftp_command = FTP_LS;
    int reply;
    robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
    robust_writen(data_fd, (void *)command_str, DATA_SIZE);
    robust_readn(command_fd, &reply, sizeof(reply));
    if(reply == REQ_OK)
    {
        int to_read;
        robust_readn(data_fd, &to_read, sizeof(to_read));
        robust_readn(data_fd, &buf, to_read);
        printf("%s", buf);
    }
    else if(reply == REQ_UNFOUND)
    {
        printf("no such directory\n");
        return -1;
    }
    else
    {
        err_common(reply);
        return -1;
    }
    printf("\n");
    return 0;
}

void Client::err_common(int reply)
{
    switch(reply)
    {
    case REQ_UNFOUND:
        printf("no such resource\n");
        break;
    case REQ_DENY:
        printf("access denied\n");
        break;
    case REQ_ERR:
        printf("internal error in server\n");
        break;
    default:
        break;
    }
}

Client::~Client()
{
    if(command_fd != -1) close(command_fd);
    if(data_fd != -1) close(data_fd);
}
