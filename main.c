#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>


typedef struct
{
	int		id;
	int		nb_coders;
	int		burnout_time;
	int		compile_time;
	int		debug_time;
	int		refactor_time;
	int		nb_compiles;
	int		dongle_cooldown;
	struct timeval		*dongles;
	pthread_mutex_t		*lock;
	char	*scheduler;
	struct timeval		tv;
	int			*burned;
} coder;


coder	*make_coders()
{
	coder	*coders;
	struct timeval	*dongles;
	pthread_mutex_t	*lock;
	int	nb[2];
	int	*burned;

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
		dongles[nb[1]].tv_sec -= 1;
		nb[1]++;
	}
	return coders;
}


int	time_since(struct timeval tv)
{
	struct timeval new;
	int seconds;
	int mili;

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
	return seconds + mili;
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


int	skip_time(int nb, coder *new, struct timeval tv)
{
	struct timeval	timer;

	gettimeofday(&timer, NULL);
	while (nb > time_since(timer))
	{
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

	cooldown = new -> dongle_cooldown;
	index1 = new -> id - 1;
	index2 = new -> id % new -> nb_coders;
	if (time_since(new -> dongles[index1]) < cooldown)
		return (0);
	if (time_since(new -> dongles[index2]) < cooldown)
		return (0);
	return (1);
}


void	*coder_routine(void *idk)
{
	coder	*new;
	struct timeval tv;
	struct timeval timer;
	int	compiles;
	int	previous;

	gettimeofday(&tv, NULL);

	new = (coder *)idk;
	compiles = 0;
	gettimeofday(&tv, NULL);
	while (compiles < new -> nb_compiles)
	{
		previous = compiles;
		while (previous == compiles)
		{
			pthread_mutex_lock(new -> lock);
			if (dongles_ready(new))
			{
				printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
				printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
				printf("%d %d is compiling\n", time_since(new -> tv), new -> id);
				compiles++;
			}
			pthread_mutex_unlock(new -> lock);
			if (previous == compiles)
				usleep(1000);
			if (check_burnout(new, tv))
				return (NULL);
		}
		gettimeofday(&timer, NULL);
		while (time_since(timer) < new -> compile_time)
		{
			gettimeofday(&(new -> dongles[new -> id - 1]), NULL);
			gettimeofday(&(new -> dongles[new -> id % (new -> nb_coders)]), NULL);
			usleep(1000);
			if (new -> burned[0])
				return (NULL);
		}
		gettimeofday(&tv, NULL);
		printf("%d %d is debugging\n", time_since(new -> tv), new -> id);
		skip_time(new -> debug_time, new, tv);
		printf("%d %d is refactoring\n", time_since(new -> tv), new -> id);
		skip_time(new -> debug_time, new, tv);
	}
	printf("%d %d is finished\n", time_since(new -> tv), new -> id);
	return (NULL);
}


int	main()
{
	pthread_t	*threads;
	coder		*coders;
	int		i;

	coders = make_coders();
	threads = malloc(coders[0].nb_coders * sizeof(pthread_t));
	i = 0;
	while (i < coders[0].nb_coders)
	{
		pthread_create(&threads[i], NULL, coder_routine, &coders[i]);
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
