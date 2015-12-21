#include "fs_helper.h"
#include <cstdio>
#include <dirent.h>
#include "definitions.h"
#include <fcntl.h>
#include "net_helper.h"

using std::string;

std::string get_dir_content(const char * dir_name)
{
    string ls_string;
    struct dirent * file = nullptr;
    DIR * dir = opendir(dir_name);
    if(dir == nullptr)
    {
        throw FSException("directory doesn't exist");
    }
    while((file = readdir(dir)) != nullptr)
    {
        switch(file->d_type)
        {
        case DT_DIR:
            ls_string.append("\x1b[34m");
            break;
        case DT_LNK:
            ls_string.append("\x1b[31m");
            break;
        }
        ls_string.append(file->d_name);
        ls_string.push_back(' ');
        ls_string.append("\x1b[0m");
    }
    return ls_string;
}

void send_to_socket(int sock_fd, int file_fd)
{
    char buf[DATA_SIZE];
    while(true)
    {
        int num_read = read(file_fd, buf, DATA_SIZE);
        robust_writen(sock_fd, &num_read, sizeof(num_read));
        if(num_read > 0)
        {
            robust_writen(sock_fd, buf, num_read);
        }
        if(num_read < DATA_SIZE) break;
    }
}

void recv_from_socket(int sock_fd, int file_fd)
{
    char buf[DATA_SIZE];
    while(true)
    {
        int num_to_write; 
        robust_readn(sock_fd, &num_to_write, sizeof(num_to_write));
        if(num_to_write > 0)
        {
            robust_readn(sock_fd, buf, num_to_write);
            write(file_fd, buf, num_to_write);
        }
        if(num_to_write < DATA_SIZE) break;
    }
}

