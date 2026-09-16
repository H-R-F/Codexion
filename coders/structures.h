/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrf <hrf@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:30:00 by hrf               #+#    #+#             */
/*   Updated: 2026/09/16 17:48:17 by hrf              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_data	t_data;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	long			last_used_time;
	int				is_taken;
}	t_dongle;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	long		last_compile_start;
	long		death_deadline;
	int			compiles_count;
	int			is_waiting;
	int			is_compiling;
	long		last_request_time;
	int			queue_index;
	t_dongle	*left_dongle;
	t_dongle	*right_dongle;
	t_data		*data;
}	t_coder;

struct s_data
{
	int				num_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				num_compiles_required;
	long			dongle_cooldown;
	char			*scheduler_type;
	long			start_time;
	int				simulation_stop;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	sim_mutex;
	pthread_cond_t	cond_sched;
	t_coder			**waiting_queue;
	int				queue_size;
	int				queue_capacity;
	t_coder			*coders;
	t_dongle		*dongles;
};

long	get_time_in_ms(void);
int		ft_atoi(const char *nptr);
int		parsing(int ac, char **av, t_data *data);
int		init(t_data *data);
void	cleanup(t_data *data);
void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);
void	smart_sleep(long time_to_wait, t_data *data);
int		acquire_dongles(t_coder *coder);
int		can_take_dongles(t_coder *coder);
void	release_dongles(t_coder *coder);
void	heap_fix(t_data *data, int index);
int		has_highest_priority(t_coder *coder);
void	scheduler_enqueue(t_coder *coder);
void	scheduler_remove(t_coder *coder);
void	print_status(t_coder *coder, char *status);

#endif
