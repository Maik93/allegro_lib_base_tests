#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <pthread.h>
#include <allegro.h>

//-----------------------------------------------------
// GRAPHICS CONSTANTS
//-----------------------------------------------------
#define WIN_WIDTH	800	// width of graphic window
#define WIN_HEIGHT	600	// height of graphic window
#define CHAR_WIDTH	8	// horizontal pixels occupied by a char
#define CHAR_HEIGHT	10	// vertical pixels occupied by a char
//-----------------------------------------------------
#define COLOR_DEPTH	8
#define BLACK	0
#define WHITE	15
#define GREEN	10
#define GREY	8
#define RED		4
#define GND		19
//-----------------------------------------------------
// TASKS CONSTANTS
//-----------------------------------------------------
#define MAX_TASKS	20			// max number of user tasks
#define MAX_THREADS	MAX_TASKS+2	// max thread number (tasks + interp + graphic)
//-----------------------------------------------------
// OTHER CONSTANTS
//-----------------------------------------------------

//-----------------------------------------------------
// STRUCT
//-----------------------------------------------------
struct task_param {
	int		index;			// task index, the easyest way to find him
	// long	wcet;			// in microseconds
	int		period;			// relative (ms)
	int		deadline;		// relative (ms)
	int		priority;		// in [0-99]
	int		dmiss;			// num of misses
	struct	timespec at;	// next activ. time
	struct	timespec dl;	// abs. deadline
	int		counts;			// number of times this task is runned
};

//-----------------------------------------------------
// TASK VARIABLES
//-----------------------------------------------------
extern pthread_t			tid[MAX_THREADS];
extern pthread_attr_t		att[MAX_THREADS];
extern struct task_param	tp[MAX_THREADS];
extern struct sched_param	sp;
//-----------------------------------------------------
extern int		sigterm_tasks;	// [0-1] signal for all task to terminate
//-----------------------------------------------------
// GRAPHIC VARIABLES
//-----------------------------------------------------
extern BITMAP	*screen_buff;	// double buffer integration

//-----------------------------------------------------
// BASE UTILITY FUNCTIONS
//-----------------------------------------------------
float frand(float xmi, float xma);
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
pthread_t start_task(void *task_fun, int period, int deadline, int priority, int index);
void kill_all_task();

//-----------------------------------------------------
// ALLEGRO FUNCTIONS
//-----------------------------------------------------
void get_keycodes(char *scan, char *ascii);
int listen_scancode();
void get_string(char *str, int x, int y, int c, int b);
void activate_mouse();
void init(int enable_mouse);

#endif