#ifndef __FS_HELPER_H__
#define __FS_HELPER_H__

#include <string>
#include <exception>

class FSException : public std::exception
{
public:
    FSException(const char *msg)
        :message(msg)
    { }

    const char *what() const _NOEXCEPT override
    {
        return message;
    }

private:
    const char *message;
};


std::string get_dir_content(const char * dir_name);
void send_to_socket(int sock_fd, int file_fd);
void recv_from_socket(int sock_fd, int file_fd);

#endif
