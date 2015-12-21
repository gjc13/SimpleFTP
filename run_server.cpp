//
// Created by 郭嘉丞 on 15/12/19.
//

#include <cstdio>
#include "Server.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

void sigchild_handler(int sig)
{
    while(waitpid(-1, 0, WNOHANG) > 0);
    return;
}



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
        signal(SIGCHLD, sigchild_handler);
        Server server(port);
        server.run();
    }
    catch(std::exception & e)
    {
        printf("%s\n", e.what());
    }
    return 0;
}
