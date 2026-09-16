/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:32:07 by hrf               #+#    #+#             */
/*   Updated: 2026/09/16 16:56:11 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

static int	coder_should_stop(t_coder *coder)
{
	int	should_stop;

	pthread_mutex_lock(&coder->data->sim_mutex);
	should_stop = coder->data->simulation_stop;
	if (!should_stop
		&& coder->compiles_count >= coder->data->num_compiles_required)
		should_stop = 1;
	pthread_mutex_unlock(&coder->data->sim_mutex);
	return (should_stop);
}

static void	compile_cycle(t_coder *coder)
{
	print_status(coder, "has taken a dongle");
	if (coder->left_dongle != coder->right_dongle)
		print_status(coder, "has taken a dongle");
	print_status(coder, "is compiling");
	pthread_mutex_lock(&coder->data->sim_mutex);
	coder->last_compile_start = get_time_in_ms();
	coder->death_deadline = coder->last_compile_start
		+ coder->data->time_to_burnout;
	coder->is_compiling = 1;
	pthread_mutex_unlock(&coder->data->sim_mutex);
	smart_sleep(coder->data->time_to_compile, coder->data);
	pthread_mutex_lock(&coder->data->sim_mutex);
	coder->is_compiling = 0;
	pthread_mutex_unlock(&coder->data->sim_mutex);
	release_dongles(coder);
	print_status(coder, "is debugging");
	smart_sleep(coder->data->time_to_debug, coder->data);
	pthread_mutex_lock(&coder->data->sim_mutex);
	if (!coder->data->simulation_stop)
		coder->compiles_count++;
	pthread_mutex_unlock(&coder->data->sim_mutex);
	print_status(coder, "is refactoring");
	smart_sleep(coder->data->time_to_refactor, coder->data);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (coder_should_stop(coder) == 0)
	{
		if (!acquire_dongles(coder))
			break ;
		compile_cycle(coder);
	}
	return (NULL);
}
