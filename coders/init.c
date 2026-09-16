/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:32:42 by hrf               #+#    #+#             */
/*   Updated: 2026/09/15 14:32:43 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

static void	init_coder(t_data *data, int i)
{
	data->coders[i].id = i + 1;
	data->coders[i].compiles_count = 0;
	data->coders[i].is_waiting = 0;
	data->coders[i].is_compiling = 0;
	data->coders[i].last_request_time = 0;
	data->coders[i].queue_index = -1;
	data->coders[i].last_compile_start = 0;
	data->coders[i].death_deadline = 0;
	data->coders[i].data = data;
	data->coders[i].left_dongle = &data->dongles[i];
	data->coders[i].right_dongle = &data->dongles[(i + 1) % data->num_coders];
}

static void	init_dongle(t_data *data, int i)
{
	data->dongles[i].id = i;
	data->dongles[i].is_taken = 0;
	data->dongles[i].last_used_time = 0;
	pthread_mutex_init(&data->dongles[i].mutex, NULL);
}

static int	allocate_data(t_data *data)
{
	data->coders = malloc(sizeof(t_coder) * data->num_coders);
	data->dongles = malloc(sizeof(t_dongle) * data->num_coders);
	data->waiting_queue = malloc(sizeof(t_coder *) * data->num_coders);
	if (!data->coders || !data->dongles || !data->waiting_queue)
	{
		free(data->coders);
		free(data->dongles);
		free(data->waiting_queue);
		return (0);
	}
	return (1);
}

int	init(t_data *data)
{
	int	i;

	if (!allocate_data(data))
		return (1);
	memset(data->coders, 0, sizeof(t_coder) * data->num_coders);
	memset(data->dongles, 0, sizeof(t_dongle) * data->num_coders);
	pthread_mutex_init(&data->log_mutex, NULL);
	pthread_mutex_init(&data->sim_mutex, NULL);
	pthread_cond_init(&data->cond_sched, NULL);
	data->queue_size = 0;
	data->queue_capacity = data->num_coders;
	i = 0;
	while (i < data->num_coders)
	{
		init_dongle(data, i);
		init_coder(data, i);
		i++;
	}
	return (0);
}
