//
// Created by 郭嘉丞 on 15/12/19.
//

#include "Client.h"

Client::Client(const char *hostname, int port)
        : server_name(hostname), server_port(port),
          command_fd(-1)
{
}


void Client::connect()
{
    command_fd = connect_sock_fd(server_name, server_port);
}

Client::~Client()
{
    if(command_fd != -1) close(command_fd);
}
