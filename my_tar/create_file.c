#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include "my_tar.h"

void *my_memcpy (void *dest, const void *src, size_t len)
{
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

size_t my_strlen(const char *s)
{
    int i;
    for (i=0; s[i]!='\0'; ++i);
    return i;
}

char *my_strcpy (char *dest, const char *src)
{
  return my_memcpy (dest, src, my_strlen (src) + 1);
}


char *my_strcat (char *dest, const char *src)
{
    my_strcpy (dest + my_strlen (dest), src);
    return dest;
}


char get_typeflag(int mode)
{
    switch (mode & S_IFMT) 
    {
        case S_IFREG:
            return REGTYPE;
        case S_IFLNK:
            return SYMTYPE;
        case S_IFCHR:
            return CHRTYPE;
        case S_IFBLK:
            return BLKTYPE;
        case S_IFDIR:
            return DIRTYPE;
        case S_IFIFO:
            return FIFOTYPE;
        default:
            return REGTYPE;
    }
    
}

tar_header * init_tar_header()
{
    tar_header *header = malloc(BLOCKSIZE);
    if (header != NULL) 
    {
        bzero(header, BLOCKSIZE);
    }

    return header;
}

int get_header_size (tar_header * header){
    int size = 0;
    for(unsigned int i = 0; i < sizeof(header->name); i++){
        size += header->name[i];
    }
    for(unsigned int i = 0; i < sizeof(header->mode); i++){
        size += header->mode[i];
    }
    for(unsigned int i = 0; i < sizeof(header->uid); i++){
        size += header->uid[i];
    }
    for(unsigned int i = 0; i < sizeof(header->gid); i++){
        size += header->gid[i];
    }
    for(unsigned int i = 0; i < sizeof(header->size); i++){
        size += header->size[i];
    }
    for(unsigned int i = 0; i < sizeof(header->mtime); i++){
        size += header->mtime[i];
    }
    for(unsigned int i = 0; i < sizeof(header->chksum); i++){
        size += ' '; // spaces in ASCII table 
    }
    for(unsigned int i = 0; i < sizeof(header->typeflag); i++){
        size += header->typeflag;
    }
    for(unsigned int i = 0; i < sizeof(header->linkname); i++){
        size += header->linkname[i];
    }
    for(unsigned int i = 0; i < sizeof(header->magic); i++){
        size += header->magic[i];
    }
    for(unsigned int i = 0; i < sizeof(header->version); i++){
        size += header->version[i];
    }
    for(unsigned int i = 0; i < sizeof(header->uname); i++){
        size += header->uname[i];
    }
    for(unsigned int i = 0; i < sizeof(header->gname); i++){
        size += header->gname[i];
    }
    for(unsigned int i = 0; i < sizeof(header->devmajor); i++){
        size += header->devmajor[i];
    }
    for(unsigned int i = 0; i < sizeof(header->devminor); i++){
        size += header->devminor[i];
    }
    for(unsigned int i = 0; i < sizeof(header->prefix); i++){
        size += header->prefix[i];
    }
    return size;
}


tar_header *get_metadata(char *filename)
{
    struct stat info;
    if (stat(filename, &info) < 0)
    {
        
        if(filename == NULL){
            return NULL;    
        }
        
       printf("tar: %s: Cannot stat: No such file or directory\n", filename);
        return NULL;
    }

    tar_header *header = init_tar_header();

    my_strcpy(header->name, filename);
    my_itoa_base(header->mode, info.st_mode - 32768, 8, OCTAL_BASE); // -100000 octals

    my_itoa_base(header->uid, info.st_uid, 8, OCTAL_BASE); 
    my_itoa_base(header->gid, info.st_gid, 8, OCTAL_BASE);

    my_itoa_base(header->size, info.st_size, 12, OCTAL_BASE);
    my_itoa_base(header->mtime, info.st_mtime, 12, OCTAL_BASE);

    header->typeflag = get_typeflag(info.st_mode);

    my_strcpy(header->magic, TMAGIC);
    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(info.st_gid);
    my_strcpy(header->gname, grp->gr_name);
    pwd = getpwuid(info.st_uid);
    my_strcpy(header->uname, " ");
    my_strcat(header->uname, pwd->pw_name);
    my_strcpy(header->version, " ");

    int header_size = get_header_size(header);

    my_itoa_base(header->chksum, header_size, 7, OCTAL_BASE);
    //my_strcat(header->chksum, " ");

    return header;
}

int get_file_size(int fd)
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


int write_file_content(int fd_archive, char * filename)
{
    int res = 0;

    int fd_file = open(filename, O_RDONLY);
    int file_size = get_file_size(fd_file);

    char content[file_size];
    bzero(content, file_size);

    read(fd_file, content, file_size);
    write(fd_archive, content, file_size);

    close(fd_file);
    return res;
}

void end_of_archive(int fd)
{
    char end[BLOCKSIZE * 2];

    bzero(end, BLOCKSIZE *2);
    write(fd, end, BLOCKSIZE * 2);
    return;
}

int create_archive(int fd_archive, tar_options *options)
{
    int res = 0;
    t_argument * current_arg = options->args;
    tar_header *header;
    while (current_arg) 
    {
        header = get_metadata(current_arg->filename);
        
        if (header!= NULL)
        {   
            if (options->create == true || options->append == true ){
                write(fd_archive, header, BLOCKSIZE); //write header to archive
                write_file_content(fd_archive, current_arg->filename); // write text to archive
            }
        }
        else 
        {
            res += 1;
        }
        free(header);
        current_arg = current_arg->next;
    }
    end_of_archive(fd_archive);
    return res;
}