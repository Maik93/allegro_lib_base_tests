#include "tasks.h"

#include <stdio.h>
#include <pthread.h>
#include "utils.h"

void *task(void *arg) {
	struct task_param *tp;
	int index = -1;
	// <local state variables>

	tp = (struct task_param *)arg;
	index = tp->arg;

	set_period(tp);
	while (!sigterm_tasks) {
		printf("Hi, here's task %d!\n", index);
		// <thread body>

		if (deadline_miss(tp))
			printf("OMFG HERE'S TASK %d, I'VE MISSED A DEADLINE!\n", index);
		wait_for_period(tp);
	}
}