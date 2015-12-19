//
// Created by 郭嘉丞 on 15/12/19.
//

#ifndef SIMPLEFTP_CLIENT_H
#define SIMPLEFTP_CLIENT_H

#include "net_helper.h"

class Client
{

public:
    Client(const char * hostname, int port);
    void connect();
    ~Client();
private:
    const char * server_name;
    int server_port;
    int command_fd;
};


#endif //SIMPLEFTP_CLIENT_H
