#include "tasks.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <allegro.h>

#include "baseUtils.h"

struct status	ball[MAX_TASKS];	// balls buffer
struct cbuf		trail[MAX_TASKS];	// trail buffer

int nab = 0;	// number of active balls
int tflag = 0;	// trail flag
int tl = 10;	// actual trail length
float g = G0;	// acceleration of gravity

/**
 * Store position of element i.
 */
void store_trail(int i) {
	int k;
	if (i >= MAX_TASKS) return;
	k = trail[i].top;
	k = (k + 1) % TLEN;
	trail[i].x[k] = ball[i].x;
	trail[i].y[k] = ball[i].y;
	trail[i].top = k;
}

/**
 * Draws trail of element i, for the length of w.
 */
void draw_trail(int i, int w) {
	int j, k;
	int x, y;
	for (j = 0; j < w; j++) {
		k = (trail[i].top + TLEN - j) % TLEN;
		x = XBOX + 1 + trail[i].x[k];
		y = WIN_HEIGHT - FLEV - trail[i].y[k];
		putpixel(screen_buff, x, y, TCOL);
	}
}

void handle_bounce(int i) {
	if (ball[i].y <= ball[i].r) {
		ball[i].y = ball[i].r;
		ball[i].vy = -DUMP * ball[i].vy;
	}
	if (ball[i].y >= HBOX - ball[i].r) {
		ball[i].y = HBOX - ball[i].r;
		ball[i].vy = -DUMP * ball[i].vy;
	}
	if (ball[i].x >= LBOX - ball[i].r) {
		ball[i].x = LBOX - ball[i].r;
		ball[i].vx = -ball[i].vx;
	}
	if (ball[i].x <= ball[i].r) {
		ball[i].x = ball[i].r;
		ball[i].vx = -ball[i].vx;
	}
}

void bump_it() {
	int i;
	for (i = 0; i < nab; i++)
		ball[i].vy = ball[i].vy + 50;
}

/**
 * Draw ball i in graphic coordinates.
 * @param i [description]
 */
void draw_ball(int i) {
	int x, y;
	x = XBOX + 1 + ball[i].x;
	y = WIN_HEIGHT - FLEV - ball[i].y;
	circlefill(screen_buff, x, y, ball[i].r, ball[i].c);
}

void init_ball(int i) {
	ball[i].c = 2 + i % 14; // color in [2,15]
	ball[i].r = frand(RMIN, RMAX);
	ball[i].x = ball[i].r + 10;
	ball[i].y = frand(HMIN, HMAX);
	ball[i].vx = frand(VXMIN, VXMAX);
	ball[i].vy = 0;
	ball[i].v0 = sqrt(2 / g * ball[i].y); // useless
	printf("Init ball %d: x=%f y=%f vx=%d vy=%d v0=%d.\n",
	       i, ball[i].x, ball[i].y, (int)ball[i].vx, (int)ball[i].vy, (int)ball[i].v0);
}

void *balltask(void* arg) {
	int i; // task index
	float dt; // integration interval
	i = get_task_index(arg);
	init_ball(i);
	dt = TSCALE * (float)get_task_period(i) / 1000;
	set_period(i);
	while (!sigterm_tasks) {
		ball[i].vy -= g * dt;
		ball[i].y += ball[i].vy * dt - g * dt * dt / 2;
		ball[i].x += ball[i].vx * dt;
		handle_bounce(i);
		store_trail(i);
		wait_for_period(i);
	}
}

void *display(void* arg) {
	int i, a;
	a = get_task_index(arg);
	set_period(a);
	while (!sigterm_tasks) {
		// clear_to_color(screen_buff, GND);
		rectfill(screen_buff, XBOX + 1, YBOX + 1, RBOX - 1, BBOX - 1, BKG);
		for (i = 0; i < nab; i++) {
			draw_ball(i);
			if (tflag) draw_trail(i, tl);
		}
		/*if (deadline_miss(a))
			show_dmiss(a);*/
		blit(screen_buff, screen, 0, 0, 0, 0, screen_buff->w, screen_buff->h);
		wait_for_period(a);
	}
}

void *interp(void* arg) {
	int a, scan;
	a = get_task_index(arg);
	set_period(a);
	while (!sigterm_tasks) {
		scan = listen_scancode();
		if (scan != 0) printf("Readed keyscan: %d\n", (int)scan);
		switch (scan) {
		case 3: // C key
			for (int j = 0; j < nab; ++j)
				printf("Logging ball %d: x=%f y=%f vx=%d vy=%d v0=%d.\n",
				       j, ball[j].x, ball[j].y, (int)ball[j].vx, (int)ball[j].vy, (int)ball[j].v0);
			break;
		case 24: // X key
			tflag = !tflag;
			printf("tflag setted to %d\n", tflag);
			break;
		case 22: // V key
			bump_it();
			break;

		case KEY_SPACE:
			if (nab < MAX_TASKS) {
				start_task(balltask, PER, DREL, PRI, nab++);
				// printf("Adding ball. Now they are %d.\n", nab);
			}
			break;

		case KEY_UP:
			g = g + 1; // increase gravity
			printf("Gravity increased to %f.\n", g);
			break;
		case KEY_DOWN:
			if (g > 1) g = g - 1; // decrease gravity
			printf("Gravity decreased to %f.\n", g);
			break;

		case KEY_LEFT:
			tl--;
			break;
		case KEY_RIGHT:
			tl++;
			break;

		case KEY_ESC:
			kill_all_task();
			break;

		default: break;
		}
		wait_for_period(a);
	}
}