/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:32:32 by hrf               #+#    #+#             */
/*   Updated: 2026/09/16 17:48:17 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

static void	lock_dongles(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
	}
	else if (coder->left_dongle->id < coder->right_dongle->id)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		pthread_mutex_lock(&coder->right_dongle->mutex);
	}
	else
	{
		pthread_mutex_lock(&coder->right_dongle->mutex);
		pthread_mutex_lock(&coder->left_dongle->mutex);
	}
}

static int	wait_for_dongles(t_coder *coder)
{
	struct timeval	now;
	struct timespec	timeout;

	while (!can_take_dongles(coder))
	{
		gettimeofday(&now, NULL);
		timeout.tv_sec = now.tv_sec;
		timeout.tv_nsec = (now.tv_usec + 1000) * 1000;
		if (timeout.tv_nsec >= 1000000000)
		{
			timeout.tv_sec++;
			timeout.tv_nsec -= 1000000000;
		}
		pthread_cond_timedwait(
			&coder->data->cond_sched, &coder->data->sim_mutex, &timeout);
		if (coder->data->simulation_stop)
		{
			scheduler_remove(coder);
			coder->is_waiting = 0;
			pthread_mutex_unlock(&coder->data->sim_mutex);
			return (0);
		}
	}
	return (1);
}

int	acquire_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->data->sim_mutex);
	coder->last_request_time = get_time_in_ms();
	coder->is_waiting = 1;
	scheduler_enqueue(coder);
	if (!wait_for_dongles(coder))
		return (0);
	scheduler_remove(coder);
	coder->is_waiting = 0;
	lock_dongles(coder);
	coder->left_dongle->is_taken = 1;
	coder->right_dongle->is_taken = 1;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	if (coder->left_dongle != coder->right_dongle)
		pthread_mutex_unlock(&coder->right_dongle->mutex);
	pthread_mutex_unlock(&coder->data->sim_mutex);
	return (1);
}

int	can_take_dongles(t_coder *coder)
{
	long	now;
	int		can_take;

	if (coder->left_dongle == coder->right_dongle)
		return (0);
	now = get_time_in_ms();
	if (!has_highest_priority(coder))
		return (0);
	lock_dongles(coder);
	can_take = 1;
	if (coder->left_dongle->is_taken || coder->right_dongle->is_taken)
		can_take = 0;
	if (now - coder->left_dongle->last_used_time
		< coder->data->dongle_cooldown)
		can_take = 0;
	if (now - coder->right_dongle->last_used_time
		< coder->data->dongle_cooldown)
		can_take = 0;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	if (coder->left_dongle != coder->right_dongle)
		pthread_mutex_unlock(&coder->right_dongle->mutex);
	return (can_take);
}

void	release_dongles(t_coder *coder)
{
	long	now;

	pthread_mutex_lock(&coder->data->sim_mutex);
	lock_dongles(coder);
	coder->left_dongle->is_taken = 0;
	coder->right_dongle->is_taken = 0;
	now = get_time_in_ms();
	coder->left_dongle->last_used_time = now;
	coder->right_dongle->last_used_time = now;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	if (coder->left_dongle != coder->right_dongle)
		pthread_mutex_unlock(&coder->right_dongle->mutex);
	pthread_cond_broadcast(&coder->data->cond_sched);
	pthread_mutex_unlock(&coder->data->sim_mutex);
}
