/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:33:06 by hrf               #+#    #+#             */
/*   Updated: 2026/09/16 16:24:06 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

static int	all_coders_completed(t_data *data)
{
	int	completed_coders;
	int	j;

	completed_coders = 0;
	j = 0;
	while (j < data->num_coders)
	{
		if (data->coders[j].compiles_count >= data->num_compiles_required)
			completed_coders++;
		j++;
	}
	return (completed_coders == data->num_coders);
}

static int	check_burnout(t_data *data)
{
	int		i;
	long	now;

	i = 0;
	while (i < data->num_coders)
	{
		now = get_time_in_ms();
		if (!data->coders[i].is_compiling
			&& now > data->coders[i].death_deadline)
		{
			data->simulation_stop = 1;
			pthread_mutex_lock(&data->log_mutex);
			printf("%ld %d has burned out\n", now - data->start_time,
				data->coders[i].id);
			pthread_mutex_unlock(&data->log_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	stop_monitor(t_data *data)
{
	pthread_cond_broadcast(&data->cond_sched);
	pthread_mutex_unlock(&data->sim_mutex);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (1)
	{
		pthread_mutex_lock(&data->sim_mutex);
		if (data->simulation_stop)
		{
			pthread_mutex_unlock(&data->sim_mutex);
			return (NULL);
		}
		if (data->num_compiles_required != -1 && all_coders_completed(data))
		{
			data->simulation_stop = 1;
			stop_monitor(data);
			return (NULL);
		}
		if (check_burnout(data))
		{
			stop_monitor(data);
			return (NULL);
		}
		pthread_mutex_unlock(&data->sim_mutex);
		usleep(1000);
	}
}
