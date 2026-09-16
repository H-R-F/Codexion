/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:34:00 by hrf               #+#    #+#             */
/*   Updated: 2026/09/16 17:02:49 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

void	scheduler_enqueue(t_coder *coder)
{
	t_data	*data;

	data = coder->data;
	if (coder->queue_index >= 0)
		return ;
	data->waiting_queue[data->queue_size] = coder;
	coder->queue_index = data->queue_size++;
	heap_fix(data, coder->queue_index);
}

void	scheduler_remove(t_coder *coder)
{
	t_data	*data;
	int		index;

	data = coder->data;
	index = coder->queue_index;
	if (index < 0)
		return ;
	data->queue_size--;
	coder->queue_index = -1;
	if (index == data->queue_size)
		return ;
	data->waiting_queue[index] = data->waiting_queue[data->queue_size];
	data->waiting_queue[index]->queue_index = index;
	heap_fix(data, index);
}

int	has_highest_priority(t_coder *coder)
{
	return (coder->data->queue_size > 0
		&& coder->data->waiting_queue[0] == coder);
}
