#include "header.h"

void	lesslines(char **arg, t_coder *idk)
{
	idk->burnout_time = atoi(arg[2]);
	idk->compile_time = atoi(arg[3]);
	idk->debug_time = atoi(arg[4]);
	idk->refactor_time = atoi(arg[5]);
	idk->nb_compiles = atoi(arg[6]);
	idk->dongle_cooldown = atoi(arg[7]);
	idk->scheduler = arg[8];
	gettimeofday(&idk->tv, NULL);
	gettimeofday(&idk->last_compile, NULL);
	idk->last_compile.tv_sec -= 1000;
}

int	*somanylines(t_coder **coder, pthread_mutex_t **lock,
	struct timeval **dongles, int nb)
{
	int	*idk;

	(void)coder;
	(void)lock;
	(void)dongles;
	(void)nb;
	*coder = malloc(nb * sizeof(t_coder));
	*lock = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(*lock, NULL);
	*dongles = malloc(nb * sizeof(struct timeval));
	idk = malloc(3 * sizeof(int));
	idk[0] = nb;
	idk[2] = 0;
	return (idk);
}

t_coder	*make_coders(char **arg)
{
	t_coder			*coders;
	struct timeval	*dongles;
	pthread_mutex_t	*lock;
	int				*nb;
	t_list			*lst;

	nb = somanylines(&coders, &lock, &dongles, atoi(arg[1]));
	lst = NULL;
	nb[1] = 0;
	while (nb[1] < nb[0])
	{
		lesslines(arg, &coders[nb[1]]);
		coders[nb[1]].id = nb[1] + 1;
		coders[nb[1]].nb_coders = nb[0];
		coders[nb[1]].dongles = dongles;
		coders[nb[1]].lst = lst;
		coders[nb[1]].lock = lock;
		coders[nb[1]].burned = nb;
		gettimeofday(&dongles[nb[1]], NULL);
		dongles[nb[1]++].tv_sec -= 1000;
	}
	return (coders);
}

int	schedule(t_coder *new, int previous, int *compiles, struct timeval tv)
{
	if (strcmp(new -> scheduler, "fifo") == 0 && fifo(new, 1))
	{
		skip_time(new -> compile_time, new, tv, 1);
		*compiles += 1;
		return (1);
	}
	if (strcmp(new -> scheduler, "edf") == 0 && edf(new))
	{
		skip_time(new -> compile_time, new, tv, 1);
		*compiles += 1;
		return (1);
	}
	if (check_burnout(new, tv) && previous == *compiles)
		return (2);
	return (0);
}

void	*coder_routine(void *idk)
{
	struct timeval	tv;
	int				compiles;
	int				previous;
	t_coder			*new;

	new = (t_coder *)idk;
	gettimeofday(&tv, NULL);
	compiles = 0;
	while (compiles < new -> nb_compiles)
	{
		previous = compiles;
		while (previous == compiles)
			if (schedule(new, previous, &compiles, tv) == 2)
				return (NULL);
		gettimeofday(&tv, NULL);
		printf("%d %d is debugging\n", time_since(new -> tv), new -> id);
		if (skip_time(new -> debug_time, new, tv, 0))
			return (NULL);
		printf("%d %d is refactoring\n", time_since(new -> tv), new -> id);
		if (skip_time(new -> refactor_time, new, tv, 0))
			return (NULL);
	}
	printf("%d %d is finished\n",
		time_since(new -> tv), new -> id);
	return (NULL);
}
