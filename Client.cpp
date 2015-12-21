//
// Created by 郭嘉丞 on 15/12/19.
//

#include "Client.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include "definitions.h"
#include "fs_helper.h"
#include <cctype>

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
        clear_input(input);
        char command[DATA_SIZE + 1];
        int numRead = sscanf(input, "%s", command);
        int ftp_command;
        char * data_client = input + strlen(command);
        if(!strcmp(command, "ls") || !strcmp(command, "cd") ||
                !strcmp(command, "get") || !strcmp(command, "put"))
        {
            while(*data_client != 0 && isspace(*data_client)) data_client++; 
        }
        if(!strcmp(command, "help"))
        {
            ftp_command = FTP_HELP;
            robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
            handle_show();
        }
        else if(!strcmp(command, "ls"))
        {
            ftp_command = FTP_LS;
            robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
            robust_writen(data_fd, data_client, DATA_SIZE);
            handle_ls();
        }
        else if(!strcmp(command, "pwd"))
        {
            ftp_command = FTP_PWD;
            robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
            handle_show();
        }
        else if(!strcmp(command, "cd"))
        {
            ftp_command = FTP_CD;
            robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
            robust_writen(data_fd, data_client, DATA_SIZE);
            printf("cd: ");
            handle_show();
        }
        else if(!strcmp(command, "get"))
        {
            ftp_command = FTP_GET;
            robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
            robust_writen(data_fd, data_client, DATA_SIZE);
            handle_get(input + strlen(command));
        }
        else if(!strcmp(command, "put"))
        {
            handle_put(data_client);
        }
        else if(!strcmp(command, "quit"))
        {
            ftp_command = FTP_EXIT;
            robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
            break;
        }
        fflush(stdin);
    }
    shutdown(data_listen_fd, SHUT_RDWR);
}

int Client::handle_show()
{
    char help_text[DATA_SIZE];
    int command = FTP_HELP;
    int reply;
    robust_readn(command_fd, &reply, sizeof(reply));
    if(reply != REQ_OK)
    {
        err_common(reply);
        return -1;
    }
    else if(robust_readn(data_fd, help_text, DATA_SIZE) >= 0)
    {
        printf("%s\n", help_text);
        return 0;
    }
    return -1;
}

int Client::handle_ls()
{
    int reply;
    robust_readn(command_fd, &reply, sizeof(reply));
    if(reply == REQ_OK)
    {
        int to_read;
        robust_readn(data_fd, &to_read, sizeof(to_read));
        char * buf = new char[to_read + 1];
        robust_readn(data_fd, buf, to_read);
        buf[to_read] = 0;
        printf("%s\n", buf);
        //delete [] buf;
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
    return 0;
}

int Client::handle_get(const char * filename)
{
    char buf[DATA_SIZE];
    while(isspace(*filename)) filename++; 
    try
    {
        int reply = REQ_ERR;
        robust_readn(command_fd, &reply, sizeof(reply));
        if(reply != REQ_OK)
        {
            err_common(reply);
            return -1;
        }
        int file_fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if(file_fd < 0)
        {
            throw FSException("cannot open file");
        }
        recv_from_socket(data_fd, file_fd);
        close(file_fd);
        return 0;
    }
    catch(FSException & e)
    {
        while(true)
        {
            int num_to_read;
            robust_readn(data_fd, &num_to_read, sizeof(num_to_read));
            if(num_to_read > 0)
            {
                robust_readn(data_fd, buf, num_to_read);
            }
            if(num_to_read < DATA_SIZE) break;
        }
        fprintf(stderr, "%s\n", e.what());
    }
    return -1;
}

int Client::handle_put(const char * filename)
{

    char buf[DATA_SIZE];
    while(isspace(*filename)) filename++; 
    int reply;
    try
    {
        int file_fd = open(filename, O_RDONLY);
        char buf[DATA_SIZE];
        if(file_fd < 0)
        {
            throw FSException("cannot open file");
        }
        int ftp_command = FTP_PUT;
        robust_writen(command_fd, &ftp_command, sizeof(ftp_command));
        robust_writen(data_fd, filename, DATA_SIZE);
        robust_readn(command_fd, &reply, sizeof(reply));
        if(reply != REQ_OK)
        {
            err_common(reply);
            return -1;
        }
        send_to_socket(data_fd, file_fd);
        close(file_fd);
        return 0;
    }
    catch(FSException & e)
    {
        fprintf(stderr, "%s\n", e.what());
    }
    return -1;
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

void Client::clear_input(char * input)
{
    while(*input)
    {
        if(*input == '\n') *input = 0;
        input++;
    }
}

Client::~Client()
{
    if(command_fd != -1) shutdown(command_fd, SHUT_RDWR);
    if(data_fd != -1) shutdown(data_fd, SHUT_RDWR);
}
