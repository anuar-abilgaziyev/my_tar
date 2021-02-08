#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "my_tar.h"


int my_strcmp (const char * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s1 == 0)
            return 0;
    return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

int get_archive_size(int fd)
{
    int res = 0;
    int reader = 0;
    char buf[BLOCKSIZE];
    while ((reader = read(fd, buf, BLOCKSIZE)) > 0 ) 
    {
        res += reader;
    }

    
    if (res % BLOCKSIZE != 0)
    {
        res += BLOCKSIZE - (res % BLOCKSIZE);
    }
    lseek(fd, 0, SEEK_SET);
    return res;
}

char *get_archive_content(int fd_archive, int archive_size){
    char* content = malloc(archive_size*sizeof(char*));
    if(fd_archive == -1){
        free(content);
        return NULL;
    }
    char c;
    int index = 0;
    while (index < archive_size) {
        read(fd_archive, &c, sizeof(c));
        content[index] = c;
        index += 1;
    }
    return content;
}

//checks by checksum comparison if it is header
bool is_header(tar_header *header){
    char header_check[8];
    my_itoa_base(header_check, get_header_size(header), 7, OCTAL_BASE);
    //my_strcat(header_check, " ");

    if (my_strcmp(header_check, header->chksum) != 0) {
        return false;
    }
    return true;
}

//creates file and writes text content
void create_file(tar_header *header, char *archive_content, int index){
    
    int fd_to_write = open(header->name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    int i = 0;
    char c;
    while (archive_content[index + BLOCKSIZE + i] != '\0') {
        c = archive_content[index + BLOCKSIZE + i];
        write(fd_to_write, &c, sizeof(c));
        i++;
    }
close(fd_to_write);
}

//goes to the next block in archive content
int get_next_block(char *archive_content, int archive_size, int index){
    int i = index + 1;
    while (i<archive_size) {
        if (archive_content[i] != '\0') {
            if (archive_content[i-1] == '\0'){
                break;
            }
        }
        i++;
    }
    return i;
}



int extract_archive(int fd_archive, tar_options * options)
{
   
    int res = 0;
    int archive_size = get_archive_size(fd_archive);
    char *archive_content = get_archive_content(fd_archive, archive_size);
    tar_header *header;
    int index = 0;
    while (index < archive_size) {
        header = (tar_header*)&archive_content[index];
        if (is_header(header)) //checks by checksum comparison if it is header
        {
            if(options->show == true){
                printf("%s\n", header->name);
            }
            if (options->extract == true) {
                if(header->typeflag == REGTYPE){
                    create_file(header, archive_content, index);
                }
            }
            
            
        }
        index = get_next_block(archive_content, archive_size, index);
    }
    free(archive_content);
    return res;
}
