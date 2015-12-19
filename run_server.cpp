//
// Created by 郭嘉丞 on 15/12/19.
//

#include <cstdio>
#include "Server.h"

bool is_stopped = false;

int main(int argc, const char * argv[])
{
    Server server(1574);
    server.run();
    return 0;
}
