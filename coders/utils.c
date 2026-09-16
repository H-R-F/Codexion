/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:33:24 by hrf               #+#    #+#             */
/*   Updated: 2026/09/15 14:33:26 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (0);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	smart_sleep(long time_to_wait, t_data *data)
{
	long	start;

	start = get_time_in_ms();
	while ((get_time_in_ms() - start) < time_to_wait)
	{
		pthread_mutex_lock(&data->sim_mutex);
		if (data->simulation_stop)
		{
			pthread_mutex_unlock(&data->sim_mutex);
			break ;
		}
		pthread_mutex_unlock(&data->sim_mutex);
		usleep(500);
	}
}

int	ft_atoi(const char *num)
{
	int		i;
	long	number;

	i = 0;
	number = 0;
	if (!num)
		return (-1);
	while (num[i] && (num[i] == ' ' || (num[i] >= 9 && num[i] <= 13)))
		i++;
	if (num[i] == '-')
		return (-1);
	if (num[i] == '+')
		i++;
	while (num[i] && (num[i] >= '0' && num[i] <= '9'))
	{
		number = (number * 10) + (num[i] - '0');
		if (number > 2147483647)
			return (-1);
		i++;
	}
	if (num[i] != '\0')
		return (-1);
	return ((int)number);
}
