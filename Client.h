//
// Created by 郭嘉丞 on 15/12/19.
//

#ifndef SIMPLEFTP_CLIENT_H
#define SIMPLEFTP_CLIENT_H

#include "net_helper.h"
#include <vector>

class Client
{

public:
    Client(const char * hostname, unsigned short port);
    void connect();
    ~Client();
private:
    const char * server_name;
    int handle_help();
    unsigned short server_port;
    int command_fd;
    int data_fd;
};


#endif //SIMPLEFTP_CLIENT_H
