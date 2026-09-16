/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 00:00:00 by hrf               #+#    #+#             */
/*   Updated: 2026/09/16 17:55:26 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

static int	higher_priority(t_coder *first, t_coder *second)
{
	if (strcmp(first->data->scheduler_type, "edf") == 0)
	{
		if (first->death_deadline != second->death_deadline)
			return (first->death_deadline < second->death_deadline);
	}
	else if (first->last_request_time != second->last_request_time)
		return (first->last_request_time < second->last_request_time);
	return (first->id < second->id);
}

static void	heap_swap(t_data *data, int first, int second)
{
	t_coder	*tmp;

	tmp = data->waiting_queue[first];
	data->waiting_queue[first] = data->waiting_queue[second];
	data->waiting_queue[second] = tmp;
	data->waiting_queue[first]->queue_index = first;
	data->waiting_queue[second]->queue_index = second;
}

static int	heap_best_child(t_data *data, int index)
{
	int	child;
	int	best;

	child = index * 2 + 1;
	if (child >= data->queue_size)
		return (index);
	best = child;
	if (child + 1 < data->queue_size && higher_priority(
			data->waiting_queue[child + 1], data->waiting_queue[child]))
		best = child + 1;
	if (!higher_priority(data->waiting_queue[best], data->waiting_queue[index]))
		return (index);
	return (best);
}

void	heap_fix(t_data *data, int index)
{
	int		parent;
	int		best;

	while (1)
	{
		best = index;
		parent = (index - 1) / 2;
		if (index > 0 && higher_priority(data->waiting_queue[index],
				data->waiting_queue[parent]))
			best = parent;
		if (best == index)
			best = heap_best_child(data, index);
		if (best == index)
			break ;
		heap_swap(data, index, best);
		index = best;
	}
}
