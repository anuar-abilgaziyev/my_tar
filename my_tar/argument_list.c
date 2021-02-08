#include "my_tar.h"

// add an argument to linked list
void add_to_list(t_argument **a, char *b)
{
    t_argument * new_node;
    new_node = (t_argument *) malloc(sizeof(t_argument));

    new_node->filename = b;
    new_node->next = *a;
    *a = new_node;

}

// initialize linked list
t_argument *init_argument()
{
    t_argument *args = (t_argument*)malloc(sizeof(t_argument));
    
    if (args != NULL)
    {
        args->filename = NULL;
        args->next = NULL;
    }
    return args;
}

// delete linked list
void delete_argument_list(t_argument **a)
{
    t_argument* current = *a;
    t_argument* next; 
    
    while(current != NULL)
    {
        next = current->next; 
        free(current); 
        current = next; 
    }
    free(next);
    *a = NULL; 
    //printf("SUCCESSFULLY DELETED ALL NODES OF LINKED LIST\n");
}


