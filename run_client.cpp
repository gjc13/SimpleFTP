//
// Created by 郭嘉丞 on 15/12/19.
//

#include "Client.h"
#include <cstdio>

int main(int argc, const char *argv[])
{
    try
    {
        Client client("127.0.0.1", 1574);
        client.connect();
    }
    catch (std::exception &e)
    {
        printf("error: %s\n", e.what());
    }
    return 0;
}
