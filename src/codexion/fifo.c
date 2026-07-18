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
		new -> burned[2] = 1;
		printf("%d %d burned out\n", time_since(new -> tv), new -> id);
		return (1);
	}
	if (new -> burned[2])
		return (1);
	return (0);
}

int	skip_time(int nb, t_coder *new, struct timeval tv, int compile)
{
	struct timeval	timer;
	int				idk[2];

	gettimeofday(&timer, NULL);
	idk[0] = timer.tv_usec + nb * 1000;
	idk[1] = 1;
	if (idk[0] + nb * 1000 > 1000000)
		idk[0] -= 1000000;
	while (idk[1] > 0)
	{
		gettimeofday(&timer, NULL);
		idk[1] = idk[0] - timer.tv_usec;
		if (idk[0] < nb * 1000 && timer.tv_usec > idk[0])
			idk[1] = 1000000 - timer.tv_usec + idk[0];
		if (compile == 0 && check_burnout(new, tv))
			return (1);
		if (compile)
		{
			gettimeofday(&(new -> dongles[new -> id - 1]), NULL);
			gettimeofday(&(new -> dongles[new->id % new->nb_coders]), NULL);
		}
		if (idk[1] > 2000)
			usleep(500);
	}
	return (gettimeofday(&new->last_compile, NULL), 0);
}

int	fifo(t_coder *new, int lock)
{
	int	index1;
	int	index2;
	int	cooldown;
	int	idk;

	if (lock)
		pthread_mutex_lock(new -> lock);
	idk = 0;
	cooldown = new -> dongle_cooldown;
	index1 = new -> id - 1;
	index2 = new -> id % new -> nb_coders;
	if (time_since(new -> dongles[index1]) < cooldown)
		idk = 1;
	if (time_since(new -> dongles[index2]) < cooldown)
		idk = 1;
	if (lock)
		pthread_mutex_unlock(new -> lock);
	if (idk)
		return (usleep(1000), 0);
	printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
	printf("%d %d has taken a dongle\n", time_since(new -> tv), new -> id);
	printf("%d %d is compiling\n", time_since(new -> tv), new -> id);
	return (1);
}
