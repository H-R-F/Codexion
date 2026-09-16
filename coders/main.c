/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:32:59 by hrf               #+#    #+#             */
/*   Updated: 2026/09/15 15:04:56 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

static int	start_coders(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		data->coders[i].last_compile_start = data->start_time;
		data->coders[i].death_deadline = data->start_time
			+ data->time_to_burnout;
		if (pthread_create(&data->coders[i].thread, NULL, &coder_routine,
				&data->coders[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}

static void	wait_for_threads(t_data *data, pthread_t monitor_thread)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		pthread_join(data->coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
}

int	main(int argc, char *argv[])
{
	t_data		data;
	pthread_t	monitor_thread;

	if (parsing(argc, argv, &data) != 0)
		return (1);
	if (init(&data) != 0)
		return (1);
	data.start_time = get_time_in_ms();
	data.simulation_stop = 0;
	if (start_coders(&data) != 0)
		return (cleanup(&data), 1);
	if (pthread_create(&monitor_thread, NULL, &monitor_routine, &data) != 0)
		return (cleanup(&data), 1);
	wait_for_threads(&data, monitor_thread);
	cleanup(&data);
	return (0);
}
