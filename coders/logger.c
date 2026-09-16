/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:33:35 by hrf               #+#    #+#             */
/*   Updated: 2026/09/15 14:33:36 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

void	print_status(t_coder *coder, char *status)
{
	long	timestamp;

	pthread_mutex_lock(&coder->data->sim_mutex);
	pthread_mutex_lock(&coder->data->log_mutex);
	if (!coder->data->simulation_stop)
	{
		timestamp = get_time_in_ms() - coder->data->start_time;
		printf("%ld %d %s\n", timestamp, coder->id, status);
	}
	pthread_mutex_unlock(&coder->data->log_mutex);
	pthread_mutex_unlock(&coder->data->sim_mutex);
}
