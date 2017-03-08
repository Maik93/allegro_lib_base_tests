#include "tasks.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <allegro.h>

#include "baseUtils.h"

struct status	fly[MAX_TASKS];	// fly buffer
struct cbuf		trail[MAX_TASKS];	// trail buffer

int naf = 0;	// number of active flies
int tflag = 1;	// trail flag
int tl = 15;	// actual trail length
float g = G0;	// acceleration of gravity

/**
 * Store position of element i.
 */
void store_trail(int i) {
	int k;
	if (i >= MAX_TASKS) return;
	k = trail[i].top;
	k = (k + 1) % TLEN;
	trail[i].x[k] = fly[i].x;
	trail[i].y[k] = fly[i].y;
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
	int left, right, top, bottom;

	left = (fly[i].x <= XBOX + fly[i].r);
	right = (fly[i].x >= RBOX - fly[i].r);
	top = (fly[i].y <= YBOX + fly[i].r);
	bottom = (fly[i].y >= BBOX - fly[i].r);
	// printf("%d %d %d %d.\n", left, right, top, bottom);

	if (left) fly[i].x = XBOX + fly[i].r;
	if (right) fly[i].x = RBOX - fly[i].r;
	if (top) fly[i].y = YBOX + fly[i].r;
	if (bottom) fly[i].y = BBOX - fly[i].r;

	if (left || right) fly[i].alpha = PI - fly[i].alpha;
	if (top || bottom) fly[i].alpha = - fly[i].alpha;
}

void init_fly(int i) {
	fly[i].c = 2 + i % 14; // color in [2,15]
	// fly[i].r = frand(RMIN, RMAX);
	fly[i].r = FL;
	fly[i].x = frand(15, RBOX - 15);
	fly[i].y = frand(15, BBOX - 15);
	fly[i].v = frand(VMIN, VMAX);
	fly[i].alpha = frand(0, 2 * PI);
	/*printf("Init fly %d: x=%f y=%f vx=%d vy=%d v0=%d.\n",
	       i, fly[i].x, fly[i].y, (int)fly[i].vx, (int)fly[i].vy, (int)fly[i].v0);*/
}

/**
 * Draw fly i in graphic coordinates.
 * @param i [description]
 */
void draw_fly(int i) {
	// dot flies
	/*int x, y;
	x = XBOX + 1 + fly[i].x;
	y = WIN_HEIGHT - FLEV - fly[i].y;
	circlefill(screen_buff, x, y, fly[i].r, fly[i].c);*/

	// arrow flies
	float p1x, p1y, p2x, p2y, p3x, p3y; // world coord.
	float ca, sa;

	ca = cos(fly[i].alpha);
	sa = sin(fly[i].alpha);

	p1x = fly[i].x + FL * ca; // nose point
	p1y = fly[i].y + FL * sa;
	p2x = fly[i].x - FW * sa; // left wing
	p2y = fly[i].y + FW * ca;
	p3x = fly[i].x + FW * sa; // right wing
	p3y = fly[i].y - FW * ca;

	/*triangle(screen_buff,
	         XCEN + p1x / scale, YMAX - YCEN - p1y / scale, // nose point
	         XCEN + p2x / scale, YMAX - YCEN - p2y / scale, // left wing
	         XCEN + p3x / scale, YMAX - YCEN - p3y / scale, // right wing
	         fly[i].c);*/

	triangle(screen_buff,
	         p1x + XCEN, WIN_HEIGHT - YCEN - p1y, // nose point
	         p2x + XCEN, WIN_HEIGHT - YCEN - p2y, // left wing
	         p3x + XCEN, WIN_HEIGHT - YCEN - p3y, // right wing
	         fly[i].c);
}

void *flytask(void* arg) {
	int i; // task index
	float dt, da, dv;
	i = get_task_index(arg);

	init_fly(i);
	dt = TSCALE * (float)get_task_period(i) / 1000;

	set_period(i);
	while (!sigterm_tasks) {
		da = frand(-DELTA_A, DELTA_A);
		// dv = frand(-DELTA_V, DELTA_V);

		fly[i].alpha += da;
		fly[i].v += dv;
		fly[i].x += fly[i].v * cos(fly[i].alpha) * dt;
		fly[i].y += fly[i].v * sin(fly[i].alpha) * dt;

		handle_bounce(i);
		store_trail(i);

		wait_for_period(i);
	}
}

void *display(void* arg) {
	int i, a;
	char str[20];

	a = get_task_index(arg);
	set_period(a);
	while (!sigterm_tasks) {
		clear_to_color(screen_buff, GND);
		rectfill(screen_buff, XBOX + 1, YBOX + 1, RBOX - 1, BBOX - 1, BKG);

		for (i = 0; i < naf; i++) {
			draw_fly(i);
			if (tflag) draw_trail(i, tl);
		}

		if (naf == MAX_TASKS) {
			sprintf(str, "MAX_TASKS reached");
			textout_ex(screen_buff, font, str, WIN_WIDTH - 17 * CHAR_WIDTH - 20, WIN_HEIGHT - 20, RED, -1);
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
			for (int j = 0; j < naf; ++j)
				printf("Logging fly %d: x=%f y=%f v=%d va=%d.\n",
				       j, fly[j].x, fly[j].y, (int)fly[j].v, (int)fly[j].alpha);
			break;
		case 24: // X key
			tflag = !tflag;
			printf("tflag setted to %d\n", tflag);
			break;

		case KEY_SPACE:
			if (naf < MAX_TASKS) {
				start_task(flytask, PER, DREL, PRI, naf++);
				// printf("Adding fly. Now they are %d.\n", naf);
			}
			break;

		case KEY_UP:
			// TODO
			break;
		case KEY_DOWN:
			// TODO
			break;

		case KEY_LEFT:
			if (tl > 10) tl--;
			break;
		case KEY_RIGHT:
			if (tl < TLEN) tl++;
			break;

		case KEY_ESC:
			kill_all_task();
			break;

		default: break;
		}
		wait_for_period(a);
	}
}