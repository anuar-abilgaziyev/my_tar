#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_tar.h"

static tar_options *init_tar_options(void)
{
    tar_options * options = malloc(sizeof(tar_options));

    if(options != NULL)
    {
        options->create = false;
        options->extract = false;
        options->append = false;
        options->append_by_time = false;
        options->show = false;
        options->args = init_argument();
        options->archive_name = NULL;
    }
    return options;
}

static tar_options * handle_options(tar_options *options, char *argument, bool *is_archive_name)
{
    int index = 0;

    while(argument[index]){
        if (argument[index] == 'c') 
        {
            options->create = true;
        }
        else if (argument[index] == 'r') 
        {
            options->append = true;
        }
        else if (argument[index] == 'u') 
        {
            options->append_by_time = true;
        }
        else if (argument[index] == 't') 
        {
            options->show = true;
        }
        else if (argument[index] == 'x') 
        {
            options->extract = true;
        }
        else if (argument[index] == 'f') 
        {
            *is_archive_name = true;
        }
        index++;
    }
    return options;
}

static bool validate_options(tar_options *options)
{
    if((options->create? 1:0) + (options->extract? 1:0) + (options->append? 1:0) + 
      (options->append_by_time? 1:0) + (options->show? 1:0) > 1){
        printf("tar: Must specify one of -c, -r, -t, -u, -x\n");
        return false;
    }
    else if ((options->create? 1:0) + (options->extract? 1:0) + (options->append? 1:0) + 
      (options->append_by_time? 1:0) + (options->show? 1:0) == 0)
    {
        printf("tar: Can't specify both modes\n");
        return false;        
    }
    if (options->archive_name == NULL)
    {
        printf("tar: Must specify a filename\n");
        return false;
    }
    return true;
}

tar_options *get_opt(int ac, char **av)
{
    if (ac<2)
    {
        return NULL;
    }

    tar_options *options = init_tar_options();
    int runner = 1;
    bool is_archive_name = false;

    while (av[runner])
    {
        if (av[runner][0] == '-') 
        {
            handle_options(options, av[runner], &is_archive_name);
        }
        else if (is_archive_name == true)
        {
            options->archive_name = strdup(av[runner]);
            is_archive_name = false;
        }
        else 
        {
            add_to_list(&(options)->args, av[runner]);
        }
        runner += 1;
    }

    if (validate_options(options) != true)
    {
        delete_options(options);
        return  NULL;
    }
    
    return options;
}

void delete_options(tar_options *options)
{
    if (options->archive_name != NULL) 
    {
        free(options->archive_name);
    }

    delete_argument_list(&(options)->args);
    free(options);
    return;
}