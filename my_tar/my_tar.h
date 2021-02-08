#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <utime.h>
#include <time.h>
/*

*/
#define OCTAL_BASE 8
#define DEC_BASE 10

#define BLOCKSIZE 512

typedef struct posix_header
{
    char name[100]; //name of the file
    char mode[8]; //mode
    char uid[8];
    char gid[8];
    char size[12]; //size
    char mtime[12]; //modification time
    char chksum[8]; //total bytes of header
    char typeflag; //typeflag
    char linkname[100]; //linkname of the file
    char magic[6];
    char version[2]; //version
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];

} tar_header;



#define TMAGIC "ustar"
#define TMAGLEN 6
#define TVERSION "00"
#define TVERSLEN 2


#define REGTYPE '0'
#define AREGTYPE '\0'
#define LNKTYPE '1'
#define SYMTYPE '2'
#define CHRTYPE '3'
#define BLKTYPE '4'
#define DIRTYPE '5'
#define FIFOTYPE '6'
#define CONTTYPE '7'
/*

*/

typedef struct s_opt
{
    bool create;
    bool append;
    bool append_by_time;
    bool extract;
    bool show;
    char *archive_name;
    struct s_argument *args;
}tar_options;


typedef struct s_argument
{
    char *filename;
    struct s_argument *next;
}t_argument;

//File_manipulation.c
int open_archive_file(tar_options *options);

//get_opt.c
tar_options *get_opt(int, char**);
void delete_options(tar_options *);

//argument_list.c
void add_to_list(t_argument **, char *);
t_argument *init_argument();
void delete_argument_list(t_argument **);

//Conversion.c
char *my_itoa_base(char *, unsigned int, int, int);
long oct_to_dec(long);
long dec_to_oct(long);
long my_atol(const char *str, int size);

//create_file.c
void *my_memcpy (void *dest, const void *src, size_t len);
size_t my_strlen(const char *s);
char *my_strcpy (char *dest, const char *src);
char *my_strcat (char *dest, const char *src);
int get_header_size(tar_header * header);
int create_archive(int fd_archive, tar_options *options);
tar_header *get_metadata(char *filename);
int write_file_content(int fd_archive, char * filename);
void end_of_archive(int fd);

//extract_archive.c
int extract_archive(int fd_archive, tar_options * options);
int my_strcmp (const char * s1, const char * s2);
int get_archive_size(int);
char *get_archive_content(int fd_archive, int);
bool is_header(tar_header *header);
int get_next_block(char *archive_content, int archive_size, int index);


// append_by_time.c
int append_by_time(int fd_archive, tar_options * options);
