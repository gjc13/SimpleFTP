#include "fs_helper.h"
#include <cstdio>
#include <dirent.h>

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

