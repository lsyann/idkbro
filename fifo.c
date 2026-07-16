#include "header.h"

int	time_since(struct timeval tv)
{
	struct timeval	new;
	int				seconds;
	int				mili;

	gettimeofday(&new, NULL);
	seconds = (new.tv_sec - tv.tv_sec) * 1000;
	mili = (new.tv_usec - tv.tv_usec) / 1000;
	if (mili < 0)
	{
		mili = 1000 + mili;
		seconds -= 1000;
	}
	return (seconds + mili);
}

int	check_burnout(t_coder *new, struct timeval tv)
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

int	skip_time(int nb, t_coder *new, struct timeval tv, int compile)
{
	struct timeval	timer;
	int				idk;

	idk = new -> id % new -> nb_coders;
	gettimeofday(&timer, NULL);
	while (nb + 1 > time_since(timer))
	{
		if (compile)
		{
			gettimeofday(&(new -> dongles[new -> id - 1]), NULL);
			gettimeofday(&(new -> dongles[idk]), NULL);
		}
		if (compile == 0 && check_burnout(new, tv))
			return (1);
		usleep(1000);
	}
	gettimeofday(&new->last_compile, NULL);
	return (0);
}

int	fifo(t_coder *new)
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
