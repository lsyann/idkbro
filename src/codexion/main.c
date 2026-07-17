#include "header.h"

void	free_all(pthread_t *threads, t_coder *coders)
{
	free(threads);
	free(coders[0].dongles);
	free(coders[0].burned);
	free(coders[0].lock);
	free(coders);
}

int	main(int argc, char **argv)
{
	pthread_t	*threads;
	t_coder		*coders;
	int			i;

	if (argc != 9)
	{
		printf("Invalid number of arguments\n");
		return (0);
	}
	coders = make_coders(argv);
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
	free_all(threads, coders);
	return (0);
}
