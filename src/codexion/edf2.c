#include "header.h"

int	cond(t_coder *new, t_list *next)
{
	if ((((t_coder *)next->data)->id) - 1 == new -> id - 1
		|| (((t_coder *)next->data)->id) % new->nb_coders == new -> id - 1
		|| (((t_coder *)next->data)->id) - 1 == new -> id % new->nb_coders
		|| ((((t_coder *)next->data)->id) % new->nb_coders
			== new -> id % new->nb_coders)
		|| (((t_coder *)next->data)->id == new->id && fifo(new, 0)))
		return (1);
	return (0);
}
