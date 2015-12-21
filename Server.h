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

    Server(unsigned short port_num);

    void run();

    void stop();

    ~Server();
private:
    void handle_link(const SA_in & client_addr);
    void connect_data(const char * remote_client);
    void handle_ls(const char * data_client);
    void handle_cd(const char * data_client);
    void handle_put(const char * data_client);
    void handle_get(const char * data_client);

    bool is_daemon;
    unsigned short port;
    int listen_fd;
    int command_fd;
    int data_fd;
};


#endif //SIMPLEFTP_SERVER_H
