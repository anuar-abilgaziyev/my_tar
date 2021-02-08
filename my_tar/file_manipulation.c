#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "my_tar.h"

//opens archive file based on flag options
int open_archive_file(tar_options *options)
{
    int fd_archive;
    int flags;
    const mode_t DEFAULT_CREAT_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    if(options->create)
    {
        flags = O_CREAT | O_WRONLY | O_TRUNC;
    }
    else if (options->append) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    }
    else if (options->append_by_time) {
        flags = O_RDWR | O_CREAT | O_APPEND;
    }
    else if (options->extract == true || options->show == true)
    {
        flags = O_RDONLY;
    }
    fd_archive = open(options->archive_name, flags, DEFAULT_CREAT_MODE);
    return fd_archive;
}