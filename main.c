#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

typedef struct
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
	struct timeval	*dongles;
	pthread_mutex_t	*lock;
	char			*scheduler;
	struct timeval	tv;
} coder;

coder	*make_coders(void)
{
	coder			*coders;
	struct timeval	*dongles;
	pthread_mutex_t	*lock;
	int				nb[2];
	int				*burned;

	nb[0] = 2;
	lock = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(lock, NULL);
	coders = malloc(nb[0] * sizeof(coder));
	dongles = malloc(nb[0] * sizeof(struct timeval));
	burned = malloc(sizeof(int));
	burned[0] = 0;
	nb[1] = 0;
	while (nb[1] < nb[0])
	{
		coders[nb[1]].id = nb[1] + 1;
		coders[nb[1]].nb_coders = nb[0];
		coders[nb[1]].burnout_time = 1000;
		coders[nb[1]].compile_time = 200;
		coders[nb[1]].debug_time = 200;
		coders[nb[1]].refactor_time = 200;
		coders[nb[1]].nb_compiles = 3;
		coders[nb[1]].dongle_cooldown = 100;
		coders[nb[1]].dongles = dongles;
		coders[nb[1]].scheduler = "fifo";
		coders[nb[1]].lock = lock;
		coders[nb[1]].burned = burned;
		gettimeofday(&coders[nb[1]].tv, NULL);
		gettimeofday(&dongles[nb[1]], NULL);
		dongles[nb[1]++].tv_sec -= 1;
	}
	return (coders);
}

int	time_since(struct timeval tv)
{
	struct timeval	new;
	int				seconds;
	int				mili;

	gettimeofday(&new, NULL);
	seconds = (new.tv_sec - tv.tv_sec) * 1000;
	mili = new.tv_usec - tv.tv_usec;
	if (mili >= 0)
		mili = (mili + 500) / 1000;
	else
		mili = (mili - 500) / 1000;
	if (mili < 0)
	{
		mili = 1000 + mili;
		seconds -= 1000;
	}
	return (seconds + mili);
}

int	check_burnout(coder *new, struct timeval tv)
{
	if (time_since(tv) > new -> burnout_time)
	{
		new -> burned[0] = 1;
		printf("%d %d burned out\n", time_since(new -> tv), new -> id);
		return (1);
	}
	if (new -> burned[0])
		return (1);
	return (0);
}

int	skip_time(int nb, coder *new, struct timeval tv, int compile)
{
	struct timeval	timer;
	int				idk;

	idk = new -> id % new -> nb_coders;
	gettimeofday(&timer, NULL);
	while (nb > time_since(timer))
	{
		if (compile)
		{
			gettimeofday(&(new -> dongles[new -> id - 1]), NULL);
			gettimeofday(&(new -> dongles[idk]), NULL);
		}
		if (check_burnout(new, tv))
			return (1);
		usleep(1000);
	}
	return (0);
}

int	dongles_ready(coder *new)
{
	int	index1;
	int	index2;
	int	cooldown;
	int	idk;

	pthread_mutex_lock(new -> lock);
	idk = 0;
	cooldown = new -> dongle_cooldown;
	index1 = new -> id - 1;
	index2 = new -> id % new -> nb_coders;
	if (time_since(new -> dongles[index1]) < cooldown)
		idk = 1;
	if (time_since(new -> dongles[index2]) < cooldown)
		idk = 1;
	pthread_mutex_unlock(new -> lock);
	if (idk)
	{
		usleep(1000);
		return (0);
	}
	printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
	printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
	printf("%d %d is compiling\n", time_since(new -> tv), new -> id);
	return (1);
}

void	fifo(coder *new)
{
	struct timeval	tv;
	int				compiles;
	int				previous;

	gettimeofday(&tv, NULL);
	compiles = 0;
	while (compiles < new -> nb_compiles)
	{
		previous = compiles;
		while (previous == compiles)
		{
			if (dongles_ready(new))
				compiles++;
			if (check_burnout(new, tv))
				return ;
		}
		if (skip_time(new -> compile_time, new, tv, 1))
			return ;
		gettimeofday(&tv, NULL);
		printf("%d %d is debugging\n", time_since(new -> tv), new -> id);
		if (skip_time(new -> debug_time, new, tv, 0))
			return ;
		printf("%d %d is refactoring\n", time_since(new -> tv), new -> id);
		if (skip_time(new -> refactor_time, new, tv, 0))
			return ;
	}
	printf("%d %d is finished\n", time_since(new -> tv), new -> id);
	return ;
}

void	*choice(void *idk)
{
	coder	*new;

	new = (coder *)idk;
	if (strcmp(new -> scheduler, "fifo") == 0)
		fifo(new);
	return (NULL);
}

int	main(void)
{
	pthread_t	*threads;
	coder		*coders;
	int			i;

	coders = make_coders();
	threads = malloc(coders[0].nb_coders * sizeof(pthread_t));
	i = 0;
	while (i < coders[0].nb_coders)
	{
		pthread_create(&threads[i], NULL, choice, &coders[i]);
		i++;
	}
	i = 0;
	while (i < coders[0].nb_coders)
		pthread_join(threads[i++], NULL);
	pthread_mutex_destroy(coders[0].lock);
	free(threads);
	free(coders[0].dongles);
	free(coders);
	return (0);
}
