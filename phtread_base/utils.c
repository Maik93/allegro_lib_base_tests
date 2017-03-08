/**
 * General popruse utility, aimed to handle time and base task commands.
 */
#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

int sigterm_tasks = 0;		// [0-1] signal for all task to terminate

/**
 * Copy a time value in other variable
 * @param td destination time variable
 * @param ts source time variable
 */
void time_copy(struct timespec *td, struct timespec ts) {
	td->tv_sec = ts.tv_sec;
	td->tv_nsec = ts.tv_nsec;
}

/**
 * Add tot milliseconds to a given time
 * @param t  time that will be incremented
 * @param ms milliseconds to add to given time
 */
void time_add_ms(struct timespec *t, int ms) {
	t->tv_sec += ms / 1000;
	t->tv_nsec += (ms % 1000) * 1000000;
	if (t->tv_nsec > 1000000000) {
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
}

/**
 * Compares two given times and returns 1 if first
 * is bigger, -1 if it's lower, 0 if equals to t2.
 * @param t1:	first time
 * @param t2:	second time
 */
int time_cmp(struct timespec t1, struct timespec t2) {
	if (t1.tv_sec > t2.tv_sec) return 1;
	if (t1.tv_sec < t2.tv_sec) return -1;
	if (t1.tv_nsec > t2.tv_nsec) return 1;
	if (t1.tv_nsec < t2.tv_nsec) return -1;
	return 0;
}

/**
 * Computes next activation time and absolute deadline
 * for the given task from current time.
 * @param tp:	task parameter pointer
 */
void set_period(struct task_param *tp) {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(tp->at), t);
	time_copy(&(tp->dl), t);
	time_add_ms(&(tp->at), tp->period);
	time_add_ms(&(tp->dl), tp->deadline);
}

/**
 * Suspends the calling thread until the next activation and,
 * when awaken, updates activation time and deadline.
 * @param tp:	task parameter pointer
 */
void wait_for_period(struct task_param *tp) {
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(tp->at), NULL);
	time_add_ms(&(tp->at), tp->period);
	time_add_ms(&(tp->dl), tp->period);
}

/**
 * If the thread is still in execution when re‐activated, it increments
 * the value of dmiss and returns 1, otherwise returns 0.
 * @param tp:	task parameter pointer
 */
int deadline_miss(struct task_param *tp) {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (time_cmp(now, tp->dl) > 0) {
		tp->dmiss++;
		return 1;
	}
	return 0;
}

/**
 * Create a new FIFO task with desired attributes and function
 * @param tid       return id of the created task
 * @param attr      thread attributes
 * @param task_fun  pointer to function that the task will execute
 * @param task_par  parameters that define the wanted task (args, period, deadline...)
 * @param sched_par scheduler parameters
 */
void start_new_task(pthread_t *tid, pthread_attr_t *attr, void *task_fun,
                    struct task_param *task_par, struct sched_param *sched_par) {
	pthread_attr_init(attr);
	pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(attr, SCHED_FIFO);
	sched_par->sched_priority = task_par->priority;
	pthread_attr_setschedparam(attr, sched_par);
	pthread_create(tid, attr, task_fun, task_par);
}

/**
 * Blocking function that waits all given thread to terminate;
 * then print num of deadline miss for each task.
 * @param tid		array of thread IDs
 * @param tp		task parameter pointer
 * @param n_tasks	number of threads to wait (dimension of array tid)
 */
void wait_for_all_task(pthread_t *tid, struct task_param *tp, int n_tasks) {
	int i;
	for (i = 0; i < n_tasks; i++) {
		pthread_join(tid[i], NULL);
		printf("Task %d terminated. Missed %d deadlines.\n", tid[i], tp->dmiss);
	}
}