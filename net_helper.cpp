//
// Created by 郭嘉丞 on 15/12/19.
//

#include "net_helper.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

int connect_sock_fd(const char *hostname, int port)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        throw SockException("create socket failed");
    }
    struct hostent *hp = gethostbyname(hostname);
    if (hp == nullptr)
    {
        throw new SockException("Unkown host");
    }
    SA_in addr;
    memset((void *) &addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, hp->h_addr_list[0], &addr.sin_addr);
    if (connect(sock_fd, (SA *) &addr, sizeof(addr)) < 0)
    {
        throw SockException("Connect failed");
    }
    return sock_fd;
}

int listen_sock_fd(int & port)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        throw SockException("create socket failed");
    }
    SA_in addr;
    memset((void *) &addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock_fd, (SA *) &addr, sizeof(addr)) < 0)
    {
        throw SockException("bind failed");
    }
    if(port == 0)
    {
        unsigned length = sizeof(addr);
        if(getsockname(sock_fd, (SA *) &addr, &length) < 0)
        {
            throw SockException("Get socket port for random select failed!");
        }
        port = addr.sin_port;
    }
    return sock_fd;
}

