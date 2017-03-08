/**
 * -----------------------------------------------------------------------
 * General popruse utility, aimed to handle time,
 * base task commands and Allegro library.
 * -----------------------------------------------------------------------
 */
#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <allegro.h>

//-----------------------------------------------------
// PUBLIC VARIABLES
//-----------------------------------------------------
pthread_t		tid[NT];
pthread_attr_t		att[NT];
struct task_param	tp[NT];
struct sched_param	sp;
//-----------------------------------------------------
int sigterm_tasks = 0;
//-----------------------------------------------------
BITMAP *screen_buff;
//-----------------------------------------------------
// PRIVATE VARIABLES
//-----------------------------------------------------
static int n_active_tasks = 0;		// active task counter


//-----------------------------------------------------
// TIME HANDLING FUNCTIONS
//-----------------------------------------------------

/**
 * Copy a time value in other variable.
 * @param td destination time variable
 * @param ts source time variable
 */
void time_copy(struct timespec *td, struct timespec ts) {
	td->tv_sec = ts.tv_sec;
	td->tv_nsec = ts.tv_nsec;
}

/**
 * Add tot milliseconds to a given time.
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
 * 		is bigger, -1 if it's lower, 0 if equals to t2.
 * @param t1  first time
 * @param t2  second time
 */
int time_cmp(struct timespec t1, struct timespec t2) {
	if (t1.tv_sec > t2.tv_sec) return 1;
	if (t1.tv_sec < t2.tv_sec) return -1;
	if (t1.tv_nsec > t2.tv_nsec) return 1;
	if (t1.tv_nsec < t2.tv_nsec) return -1;
	return 0;
}

//-----------------------------------------------------
// TASK HANDLING FUNCTIONS
//-----------------------------------------------------

/**
 * Computes next activation time and absolute
 * 		deadline of a task identified by his index.
 */
void set_period(int index) {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(tp[index].at), t);
	time_copy(&(tp[index].dl), t);
	time_add_ms(&(tp[index].at), tp[index].period);
	time_add_ms(&(tp[index].dl), tp[index].deadline);
}

/**
 * Suspends the calling task (identified by his index) until the next
 * 		activation and, when awaken, updates activation time and deadline.
 */
void wait_for_period(int index) {
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(tp[index].at), NULL);
	time_add_ms(&(tp[index].at), tp[index].period);
	time_add_ms(&(tp[index].dl), tp[index].period);
}

/**
 * If the task (identified by his index) is still in execution when re‐activated,
 * 		it increments the value of dmiss and returns 1, otherwise returns 0.
 */
int deadline_miss(int index) {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (time_cmp(now, tp[index].dl) > 0) {
		tp[index].dmiss++;
		return 1;
	}
	return 0;
}

/**
 * Get the int value passed as argument to a task.
 * @param arg  is a casted void task_param structure
 */
int get_task_index(void *arg) {
	struct task_param *tp;
	tp = (struct task_param *)arg;
	return tp->index;
}

int get_task_period(int index) {
	return tp[index].period;
}

/**
 * Create a new FIFO syncronous task with desired attributes and function.
 * @param  task_fun pointer to function that the task will execute
 * @param  period   repetition time, in milliseconds
 * @param  deadline max relative time from start to end of task
 * @param  priority 0-255 value reguarding importance of this task
 * @return          index of just created task
 */
int create_and_start_task(void *task_fun, int period, int deadline, int priority) {
	struct sched_param sched_par;

	tp[n_active_tasks].index = n_active_tasks;
	tp[n_active_tasks].period = period;
	tp[n_active_tasks].deadline = deadline;
	tp[n_active_tasks].priority = priority;
	tp[n_active_tasks].dmiss = 0;

	pthread_attr_init(&att[n_active_tasks]);
	pthread_attr_setinheritsched(&att[n_active_tasks], PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&att[n_active_tasks], SCHED_FIFO);
	sched_par.sched_priority = tp[n_active_tasks].priority;
	pthread_attr_setschedparam(&att[n_active_tasks], &sched_par);

	pthread_create(&tid[n_active_tasks], &att[n_active_tasks], task_fun, &tp[n_active_tasks]);

	return n_active_tasks++;
}

/**
 * Blocking function that waits all given thread to terminate,
 * 		then print num of deadline miss for each task.
 */
void kill_all_task() {
	sigterm_tasks = 1;
	while (n_active_tasks > 0) {
		n_active_tasks--;
		pthread_join(tid[n_active_tasks], NULL);
		printf("Task %d terminated. Missed %d deadlines.\n", tp[n_active_tasks].index, tp[n_active_tasks].dmiss);
	}
}

//-----------------------------------------------------
// ALLEGRO FUNCTIONS
//-----------------------------------------------------

/**
 * Blocking function that wait for an input from keyboard
 * 		and returns its scancode and ascii.
 * @param scan  scancode of the pressed key
 * @param ascii ascii code of the pressed key
 */
void get_keycodes(char *scan, char *ascii) {
	int k;
	k = readkey(); // block until a key is pressed
	*ascii = k; // get ascii code
	*scan = k >> 8; // get scan code
}

/**
 * Non-blocking function that check if a key is pressed and,
 * 		if true, return the corresponding scancode.
 */
char listen_scancode() {
	if (keypressed())
		return readkey() >> 8;
	return 0;
}

/**
 * Reads a string from the keyboard and displays the echo in
 * 		graphic mode at position (x,y), color c and background b.
 * @param str complete string written by used (until enter)
 * @param x   x position for the grafical output
 * @param y   y position for the grafical output
 * @param c   color for the printed text
 * @param b   backgroud for the printed text
 */
void get_string(char *str, int x, int y, int c, int b) {
	char ascii, scan, s[2];
	int i = 0;
	do {
		get_keycodes(&scan, &ascii);
		if (scan != KEY_ENTER) {
			s[0] = ascii; // put ascii in s for echoing
			s[1] = '\0';
			textout_ex(screen_buff, font, s, x, y, c, b); // echo
			x = x + 8;
			str[i++] = ascii; // insert character in string
		}
	} while (scan != KEY_ENTER && scan != KEY_ESC);
	str[i] = '\0';
}

/**
 * Enable mouse integration, handled by hardware, with custom icon.
 */
void activate_mouse() {
	BITMAP* mouse_bmp = load_bitmap("mouse.bmp", NULL);
	install_mouse();
	enable_hardware_cursor();
	if (mouse_bmp != NULL) {
		set_mouse_sprite(mouse_bmp);
		set_mouse_sprite_focus(0, 0);
		// position_mouse(WIN_WIDTH / 2, WIN_HEIGHT / 2);
	}
	else
		printf("Mouse bitmap not found. I'll leave your default icon...\n");
	show_mouse(screen);
}

/**
 * Job with the only purpose to refresh screen,
 * 		transferring screen_buff in Allegro screen.
 */
void *task_graphic_refresh(void *arg) {
	int index;
	index = get_task_index(arg);

	set_period(index);
	while (!sigterm_tasks) {
		blit(screen_buff, screen, 0, 0, 0, 0, screen_buff->w, screen_buff->h);
		wait_for_period(index);
	}
}

/**
 * Base commands to start Allegro, with a task that will refresh screen.
 */
void init_graphic() {
	allegro_init();
	set_color_depth(COLOR_DEPTH);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIN_WIDTH, WIN_HEIGHT, 0, 0);

	install_keyboard();
	activate_mouse();

	// create a buffer, where all task can paint
	screen_buff = create_bitmap(SCREEN_W, SCREEN_H);
	clear_bitmap(screen_buff);
	clear_to_color(screen_buff, GND);

	// starts a task that refresh graphic at 25fps, lowest priority possible
	create_and_start_task(task_graphic_refresh, 40, 40, 1);
}