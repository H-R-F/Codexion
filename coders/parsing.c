/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:33:11 by hrf               #+#    #+#             */
/*   Updated: 2026/09/16 17:55:09 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "structures.h"

static int	valid_arguments(char **av)
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if (ft_atoi(av[i]) < 0)
			return (0);
		i++;
	}
	if (ft_atoi(av[1]) == 0)
		return (0);
	if (strcmp(av[8], "fifo") != 0 && strcmp(av[8], "edf") != 0)
		return (0);
	return (1);
}

static void	set_arguments(char **av, t_data *data)
{
	data->num_coders = ft_atoi(av[1]);
	data->time_to_burnout = ft_atoi(av[2]);
	data->time_to_compile = ft_atoi(av[3]);
	data->time_to_debug = ft_atoi(av[4]);
	data->time_to_refactor = ft_atoi(av[5]);
	data->num_compiles_required = ft_atoi(av[6]);
	data->dongle_cooldown = ft_atoi(av[7]);
	data->scheduler_type = av[8];
}

int	parsing(int ac, char **av, t_data *data)
{
	if (ac != 9)
	{
		printf("Error: Wrong number of arguments\n");
		return (1);
	}
	if (!valid_arguments(av))
	{
		printf("Error: Invalid argument\n");
		return (1);
	}
	set_arguments(av, data);
	return (0);
}
