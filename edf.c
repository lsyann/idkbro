#include "header.h"

int	is_in(t_list *lst, t_coder *idk)
{
	t_list	*first;

	if (lst == NULL)
		return (0);
	first = lst;
	while (first)
	{
		if (first -> data == idk)
			return (1);
		first = first -> next;
	}
	return (0);
}

int	toomanylines(t_coder *new, t_list *new_node, t_list *first)
{
	if (time_since(((t_coder *)first->data)->last_compile) > (
			time_since(((t_coder *)new->lst->data)->last_compile)))
	{
		new_node -> next = first -> next;
		first -> next = new_node;
		return (1);
	}
	if (!first -> next)
	{
		first -> next = new_node;
		return (1);
	}
	return (0);
}

void	add_to_lst(t_coder *new)
{
	t_list	*first;
	t_list	*new_node;

	new_node = malloc(sizeof(t_list));
	new_node -> data = (void *)new;
	new_node -> next = NULL;
	pthread_mutex_lock(new->lock);
	if (!new -> lst)
	{
		new -> lst = new_node;
		new_node = NULL;
	}
	first = new -> lst;
	while (first -> next && new_node)
	{
		if (toomanylines(new, new_node, first))
			break ;
	}
	pthread_mutex_unlock(new -> lock);
}

int	is_ready(t_list *first, t_coder *new)
{
	if (((t_coder *)first -> data)-> id == new -> id)
	{
		if (fifo(new))
		{
			pthread_mutex_lock(new -> lock);
			new -> lst = new -> lst -> next;
			pthread_mutex_unlock(new -> lock);
			return (1);
		}
	}
	else
		usleep(1000);
	return (0);
}

int	edf(t_coder *new)
{
	if (is_in(new -> lst, new) == 0)
		add_to_lst(new);
	if (is_ready(new -> lst, new))
		return (1);
	return (0);
}
