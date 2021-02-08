#include "my_tar.h"

int header_mtime_compare(char *archive_content, tar_header *header, int initial_size){
    int archive_size = initial_size;
    tar_header *header_saved;
    int index = 0;
    int result = 0;
    while (index < archive_size) {
        header_saved = (tar_header*)&archive_content[index];
        if (is_header(header_saved) )
        {
            if(my_strcmp(header->name, header_saved->name) == 0){
                if(my_strcmp(header->mtime, header_saved->mtime) == 1){
                    result = 1;
                }
                else {
                    return 0;
                }
            }
        }
        index = get_next_block(archive_content, archive_size, index);
    }
    return result;
}

int header_check(char *archive_content, tar_header *header, int archive_size){
    tar_header *header_saved;
    int index = 0;
    int counter = 0;
    while (index < archive_size) {
        header_saved = (tar_header*)&archive_content[index];
        if (is_header(header_saved) ){
            if(my_strcmp(header->name, header_saved->name) == 0){
                return 0;
            }
            else {
                counter += 1;
            }
        }
        index = get_next_block(archive_content, archive_size, index);
    }
    return counter;
}


int append_by_time(int fd_archive, tar_options * options) // -u flag
{
    int res = 0;
    t_argument * current_arg = options->args;
    tar_header *header;
    
    int archive_size = get_archive_size(fd_archive);
    char *archive_content = get_archive_content(fd_archive, archive_size);

    while (current_arg) 
    {
        header = get_metadata(current_arg->filename);
        if (header!= NULL)
        {   
            //if the archive does not exist or the file does not exist in the archive
            if (archive_size == 0 || header_check(archive_content, header, archive_size) != 0){
                write(fd_archive, header, BLOCKSIZE);
                write_file_content(fd_archive, current_arg->filename);
            }
            //new entries are added only if they have a modification date newer
            else {
                if(header_check(archive_content, header, archive_size) == 0){
                    if(header_mtime_compare(archive_content, header, archive_size) > 0){
                        write(fd_archive, header, BLOCKSIZE);
                        write_file_content(fd_archive, current_arg->filename);  
                    }
                }
            }
        }
        else 
        {
            res += 1;
        }
        current_arg = current_arg->next;
        free(header);
    }
    free(archive_content);
    end_of_archive(fd_archive);
    return res;
}