#include <stdio.h>
#include <stdlib.h>

typedef struct
{
        int                             id;
        int                             nb_coders;
        int                             burnout_time;
        int                             compile_time;
        int                             debug_time;
        int                             refactor_time;
        int                             nb_compiles;
        int                             dongle_cooldown;
        int                             *burned;
        struct timeval  *dongles;
        pthread_mutex_t *lock;
        char                    *scheduler;
        struct timeval  tv;
}       coder;

typedef struct  s_list
{
        int	data;
        struct s_list   *next;
}	t_list;

int main()
{
	t_list *first;

	first = malloc(sizeof(t_list));
	first -> data = 1;
	t_list *second;
	second = malloc(sizeof(t_list));
	second->data = 2;
	first->next = second;
	second->next = NULL;
	while (first)
	{
		printf("%d\n", first->data);
		first = first->next;
	}
}
