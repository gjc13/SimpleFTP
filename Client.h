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
    void clear_input(char * input);
    int handle_show();
    int handle_ls();
    int handle_get(const char * filename);
    int handle_put(const char * filename);
    void err_common(int reply);
    unsigned short server_port;
    int command_fd;
    int data_fd;
};


#endif //SIMPLEFTP_CLIENT_H
