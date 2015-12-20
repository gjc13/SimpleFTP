//
// Created by 郭嘉丞 on 15/12/19.
//

#include <cstdio>
#include "Server.h"


int main(int argc, const char * argv[])
{
    try
    {
        int port = 1574;
        if(argc == 2)
        {
            sscanf(argv[1], "%d\n", &port);
        }
        printf("Server running on port %d\n", port);
        Server server(port);
        server.run();
    }
    catch(std::exception & e)
    {
        printf("%s\n", e.what());
    }
    return 0;
}
