#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <pthread.h>

struct task_param {
	int		arg;			// task argument
	long	wcet;			// in microseconds
	int		period;			// in milliseconds
	int		deadline;		// relative (ms)
	int		priority;		// in [0,99]
	int		dmiss;			// no. of misses
	struct	timespec at;	// next activ. time
	struct	timespec dl;	// abs. deadline
};

extern int sigterm_tasks;

void time_copy(struct timespec *td, struct timespec ts);
void time_add_ms(struct timespec *t, int ms);
int time_cmp(struct timespec t1, struct timespec t2);
void set_period(struct task_param *tp);
void wait_for_period(struct task_param *tp);
int deadline_miss(struct task_param *tp);
void start_new_task(pthread_t *tid, pthread_attr_t *attr, void *task_fun,
                   struct task_param *task_par, struct sched_param *sched_par);
void wait_for_all_task(pthread_t *tid, struct task_param *tp, int n_tasks);

#endif