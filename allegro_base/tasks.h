#ifndef TASKS_H
#define TASKS_H

#include <stdio.h>
#include <pthread.h>
#include <allegro.h>
#include "utils.h"

//-----------------------------------------------------
// HELLO TASK CONSTANTS
//-----------------------------------------------------
#define XBASE	WIN_WIDTH / 2 - 100	// X start for the message
#define YBASE	20 + CHAR_HEIGHT + 5// Y level for the first task
#define YINC	CHAR_HEIGHT + 3		// Y increment for the other tasks
#define PER		30					// base period
#define PINC	20					// period increment
//-----------------------------------------------------

void *task_hello(void* arg);
void *task_base(void *arg);

#endif