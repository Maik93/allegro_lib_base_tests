#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <pthread.h>
#include <allegro.h>

//-----------------------------------------------------
// GRAPHICS CONSTANTS
//-----------------------------------------------------
#define WIN_WIDTH		800	// width of graphic window
#define WIN_HEIGHT		600	// height of graphic window
#define CHAR_WIDTH	8	// horizontal pixels occupied by a char
#define CHAR_HEIGHT	10	// vertical pixels occupied by a char
//-----------------------------------------------------
#define COLOR_DEPTH	8
// #define BLACK	makecol(0, 0, 0)
// #define WHITE	makecol(255, 255, 255)
// #define GREEN	makecol(0, 227, 16)
// #define GREY		makecol(64, 64, 64)
// #define RED		makecol(221, 19, 19)
#define BLACK	0
#define WHITE	15
#define GREEN	10
#define GREY		8
#define RED		4
#define GND		8
//-----------------------------------------------------
// TASKS CONSTANTS
//-----------------------------------------------------
#define NT	10				// max thread number

//-----------------------------------------------------
// STRUCT
//-----------------------------------------------------
struct task_param {
	int		index;			// task index, the easyest way to find him
	long	wcet;			// in microseconds
	int		period;			// in milliseconds
	int		deadline;		// relative (ms)
	int		priority;		// in [0,99]
	int		dmiss;			// no. of misses
	struct	timespec at;	// next activ. time
	struct	timespec dl;	// abs. deadline
};

//-----------------------------------------------------
// TASK VARIABLES
//-----------------------------------------------------
extern pthread_t			tid[NT];
extern pthread_attr_t		att[NT];
extern struct task_param	tp[NT];
extern struct sched_param	sp;
//-----------------------------------------------------
extern int		sigterm_tasks;	// [0-1] signal for all task to terminate
//-----------------------------------------------------
// GRAPHIC VARIABLES
//-----------------------------------------------------
extern BITMAP	*screen_buff;	// double buffer integration

//-----------------------------------------------------
// TIME HANDLING FUNCTIONS
//-----------------------------------------------------
void time_copy(struct timespec *td, struct timespec ts);
void time_add_ms(struct timespec *t, int ms);
int time_cmp(struct timespec t1, struct timespec t2);

//-----------------------------------------------------
// TASK HANDLING FUNCTIONS
//-----------------------------------------------------
void set_period(int index);
void wait_for_period(int index);
int deadline_miss(int index);
int get_task_index(void *arg);
int get_task_period(int index);
int create_and_start_task(void *task_fun, int period, int deadline, int priority);
void kill_all_task();

//-----------------------------------------------------
// ALLEGRO FUNCTIONS
//-----------------------------------------------------
void get_keycodes(char *scan, char *ascii);
char listen_scancode();
void get_string(char *str, int x, int y, int c, int b);
void activate_mouse();
// void *task_graphic_refresh(void *arg); it should be private
void init_graphic();

#endif