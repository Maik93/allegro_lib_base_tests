#include <allegro.h>
#include "utils.h"
#include "tasks.h"

// #define	RADIUS	50	// disc radius
#define	NT		10	// max thread number

int main(int argc, char const *argv[]) {
	struct sched_param	sp;
	struct task_param	tp[NT];
	pthread_attr_t		att[NT];
	pthread_t			tid[NT];
	int i;

	for (i = 0; i < NT; i++) {
		tp[i].arg = i;
		tp[i].period = 100;
		tp[i].deadline = 80;
		tp[i].priority = 20;
		tp[i].dmiss = 0;

		start_new_task(&tid[i], &att[i], task, &tp[i], &sp);
	}

	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_ms(&t, 3 * 1000);
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	sigterm_tasks = 1;

	wait_for_all_task(tid, tp, NT);
	return 0;
}


// int main(int argc, char const *argv[]) {
// int x = 100, y = 100, col = 4; // RED color
// allegro_init();
// install_keyboard();
// set_color_depth(8); // VGA mode (8 bits)
// set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);
// clear_to_color(screen, 0); // black background
// circlefill(screen, x, y, RADIUS, col);
// readkey(); // wait for any key
// allegro_exit();
// return 0;
// }