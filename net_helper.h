//
// Created by 郭嘉丞 on 15/12/19.
//

#ifndef SIMPLEFTP_NET_HELPER_H
#define SIMPLEFTP_NET_HELPER_H

#include <exception>
#include <netinet/in.h>

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_in;

class SockException : public std::exception
{
public:
    SockException(const char *msg)
        :message(msg)
    { }

    const char *what() const _NOEXCEPT override
    {
        return message;
    }

private:
    const char *message;
};

int connect_sock_fd(const char *hostname, unsigned short port);
int listen_sock_fd(unsigned short & port);
int robust_readn(int fd, void * buf, unsigned n);
int robust_writen(int fd, const void * buf, unsigned n);

#endif //SIMPLEFTP_NET_HELPER_H


