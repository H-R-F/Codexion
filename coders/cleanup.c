/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:33:30 by hrf               #+#    #+#             */
/*   Updated: 2026/09/15 14:33:31 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

void	cleanup(t_data *data)
{
	int	i;

	if (!data)
		return ;
	if (data->dongles)
	{
		i = 0;
		while (i < data->num_coders)
		{
			pthread_mutex_destroy(&data->dongles[i].mutex);
			i++;
		}
		free(data->dongles);
	}
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->sim_mutex);
	pthread_cond_destroy(&data->cond_sched);
	if (data->coders)
		free(data->coders);
	if (data->waiting_queue)
		free(data->waiting_queue);
}
