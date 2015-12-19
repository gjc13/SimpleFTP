//
// Created by 郭嘉丞 on 15/12/19.
//

#ifndef SIMPLEFTP_SERVER_H
#define SIMPLEFTP_SERVER_H

#include "net_helper.h"

class Server
{
public:
    Server();

    Server(int port_num);

    void run();

    void stop();

private:
    void handle_link(const SA_in & client_addr);

    ~Server();

    bool is_daemon;
    int port;
    int listen_fd;
    int command_fd;
    int data_fd;
};


#endif //SIMPLEFTP_SERVER_H
