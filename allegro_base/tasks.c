#include "tasks.h"

#include <stdio.h>
#include <pthread.h>
#include <allegro.h>
#include "utils.h"

void *task_hello(void* arg) {
	int i, k = 0;		// task and character index
	int x, y;			// graphical position of current char
	char ch_buf[2];		// temp buffer for 1 character string
	char mes[20];		// string containing entire task message

	i = get_task_index(arg);

	sprintf(mes, "I'M TASK %d, T = %d milliseconds", i, get_task_period(i));

	set_period(i);
	while (!sigterm_tasks) {
		x = XBASE + k * 8;
		y = YBASE + i * YINC;
		sprintf(ch_buf, "%c", mes[k]);
		textout_ex(screen_buff, font, ch_buf, x, y, 2 + i, GND);

		k = k + 1;
		// if eol reached, clean the entire row and restarts from 0
		if (mes[k] == '\0') {
			k = 0;
			textout_ex(screen_buff, font, mes, XBASE, y, GND, GND);
		}
		wait_for_period(i);
	}
}

void *task_base(void *arg) {
	int index;
	index = get_task_index(arg);

	// <local state variables>

	set_period(index);
	while (!sigterm_tasks) {
		printf("Hi, here's task %d!\n", index);
		// <thread body>

		if (deadline_miss(index))
			printf("OMG HERE'S TASK %d, I'VE MISSED A DEADLINE!\n", index);
		wait_for_period(index);
	}
}