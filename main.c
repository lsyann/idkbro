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
} coder;


coder	*make_coders()
{
	coder	*coders;
	struct timeval	*dongles;
	pthread_mutex_t	*lock;
	int	nb;
	int	i;

	nb = 3;
	lock = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(lock, NULL);
	coders = malloc(nb * sizeof(coder));
	dongles = malloc(nb * sizeof(struct timeval));
	i = 0;
	while (i < nb)
	{
		coders[i].id = i + 1;
		coders[i].nb_coders = nb;
		coders[i].burnout_time = 1000;
		coders[i].compile_time = 200;
		coders[i].debug_time = 200;
		coders[i].refactor_time = 200;
		coders[i].nb_compiles = 3;
		coders[i].dongle_cooldown = 100;
		coders[i].dongles = dongles;
		coders[i].scheduler = "fifo";
		coders[i].lock = lock;
		gettimeofday(&coders[i].tv, NULL);
		gettimeofday(&dongles[i], NULL);
		dongles[i].tv_sec -= 1;
		i++;
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
	printf("thread nb: %d\n", new -> id);
	while (compiles < new -> nb_compiles)
	{
		previous = compiles;
		while (previous == compiles)
		{
			pthread_mutex_lock(new -> lock);
			if (time_since(new -> dongles[new -> id - 1]) > new -> dongle_cooldown && time_since(new -> dongles[new -> id % new -> nb_coders]) > new -> dongle_cooldown)
			{
				printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
				printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
				printf("%d %d is compiling\n", time_since(new -> tv), new -> id);
				compiles++;
			}
			pthread_mutex_unlock(new -> lock);
			if (previous == compiles)
				usleep(1000);
			if (time_since(tv) > new -> burnout_time)
			{
				printf("%d %d died\n", time_since(new -> tv), new -> id);
				return (NULL);
			}
		}
		gettimeofday(&timer, NULL);
		while (time_since(timer) < new -> compile_time)
		{
			gettimeofday(&(new -> dongles[new -> id - 1]), NULL);
			gettimeofday(&(new -> dongles[new -> id % (new -> nb_coders)]), NULL);
			usleep(1000);
		}
		gettimeofday(&tv, NULL);
		gettimeofday(&timer, NULL);
		printf("%d %d is debugging\n", time_since(new -> tv), new -> id);
		while (time_since(timer) < new -> debug_time)
		{
			if (time_since(tv) > new -> burnout_time)
			{
				printf("%d %d died\n", time_since(new -> tv), new -> id);
				return (NULL);
			}
			usleep(1000);
		}
		gettimeofday(&timer, NULL);
		printf("%d %d is refactoring\n", time_since(new -> tv), new -> id);
		while (time_since(timer) < new -> refactor_time)
		{
			
			if (time_since(tv) > new -> burnout_time)
			{
				printf("%d %d died\n", time_since(new -> tv), new -> id);
				return (NULL);
			}
			usleep(1000);
		}
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
