#include <stdio.h>
#include <string.h>


int is_int(char *nb)
{
        int i;

        i = 0;
        while (nb[i])
        {
            if (nb[i] < '0' || nb[i] > '9')
                return (0);
            i++;
        }
        return (1);
}


int	config(int argc, char **argv)
{
	int	i;

	if (argc != 9)
	{
		printf("Invalid number of arguments\n");
		return (1);
	}
	i = 1;
	while (i < 7)
	{
		if (!is_int(argv[i]))
		{
			printf("Invalid param: %s\n", argv[i]);
			return (1);
		}
		i++;
	}
	if (strcmp("fifo", argv[8]) != 0 && strcmp("edf", argv[8]) != 0)
	{
		printf("Invalid scheduler: %s\n", argv[8]);
		return (1);
	}
	return (0);
}
