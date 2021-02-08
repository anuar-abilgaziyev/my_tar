#include <bits/pthreadtypes-arch.h>
#include "my_tar.h"


int main(int ac, char** av)
{
    int res = 0;
    int fd_archive;
    tar_options *options = get_opt(ac,av);

    if(options == NULL)
    {
        printf("Error parsing options");
        return 1;
    }

    fd_archive = open_archive_file(options); 
    if (fd_archive < 0) 
    {
        printf("tar: %s: Cannot open: No such file or directory", options->archive_name);
        delete_options(options);
        return 2;
    }
    if (options->create == true || options->append == true) 
    {
        res += create_archive(fd_archive, options);
    }
    else if (options->append_by_time == true){
        append_by_time(fd_archive, options);
    }
    else if (options->extract == true || options->show == true) {
        res += extract_archive(fd_archive, options);
    }
    close(fd_archive);
    delete_options(options);
    return res;
}