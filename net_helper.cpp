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
#include <unistd.h>
#include <errno.h>
#include <cstdio>

int connect_sock_fd(const char *hostname, unsigned short port)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        throw SockException("create socket failed");
    }
    struct hostent *hp = gethostbyname(hostname);
    if (hp == nullptr)
    {
        throw SockException("Unkown host");
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

int listen_sock_fd(unsigned short & port)
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
        port = ntohs(addr.sin_port);
    }

    if (listen(sock_fd, 1024) < 0)
    {
        throw SockException("listen failed");
    }
    return sock_fd;
}

int robust_readn(int fd, void * buf, unsigned n)
{
    int num_left = n;
    char * bufp = (char *)buf;
    while(num_left > 0)
    {
        int nowread = read(fd, bufp, num_left);
        if(nowread < 0) return -1;
        if(nowread == 0) break; //EOF
        num_left -= nowread;
        bufp += nowread;
    }
    return int(n - num_left);
}

int robust_writen(int fd, const void * buf, unsigned n)
{
    int num_left = n;
    char * bufp = (char *)buf;
    while(num_left > 0)
    {
        int nowread = write(fd, bufp, num_left);
        if(nowread < 0) return -1;
        num_left -= nowread;
        bufp += nowread;
    }
    return int(n - num_left);
}
