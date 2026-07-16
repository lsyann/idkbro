#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

typedef struct s_list
{
	void			*data;
	struct s_list	*next;
}	t_list;

typedef struct s_coder
{
	int				id;
	int				nb_coders;
	int				burnout_time;
	int				compile_time;
	int				debug_time;
	int				refactor_time;
	int				nb_compiles;
	int				dongle_cooldown;
	int				*burned;
	t_list			*lst;
	struct timeval	*dongles;
	pthread_mutex_t	*lock;
	char			*scheduler;
	struct timeval	tv;
	struct timeval	last_compile;
}	t_coder;

int		edf(t_coder *new);
int		is_ready(t_list *first, t_coder *new);
void	add_to_lst(t_coder *new);
int		is_in(t_list *lst, t_coder *idk);
int		time_since(struct timeval tv);
int		fifo(t_coder *new);
int		fifo(t_coder *new);
int		skip_time(int nb, t_coder *new, struct timeval tv, int compile);
int		check_burnout(t_coder *new, struct timeval tv);
int		time_since(struct timeval tv);
