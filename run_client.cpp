//
// Created by 郭嘉丞 on 15/12/19.
//

#include "Client.h"
#include <cstdio>

int main(int argc, const char *argv[])
{
    try
    {
        int port = 1574;
        if(argc == 2)
        {
            sscanf(argv[1], "%d", &port);
        }
        printf("port %d\n", port);
        Client client("localhost", port);
        client.connect();
    }
    catch (std::exception &e)
    {
        printf("error: %s\n", e.what());
    }
    return 0;
}
