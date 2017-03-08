#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H
//-----------------------------------------------------
// FLY TASK CONSTANTS
//-----------------------------------------------------
#define PER		20	// task period in ms
#define DREL	PER	// relative deadline in ms
#define PRI		60	// task priority
//-----------------------------------------------------
// FLY CONSTANTS
//-----------------------------------------------------
#define FL		15				// length of the fly
#define FW		6				// width of the fly
#define XCEN	FL/3			// x center for the fly arrow
#define YCEN	FW/2			// y center for the fly arrow
// #define RMIN	5				// min radius
// #define RMAX	10				// man radius
#define G0		9.8				// acceleration of gravity
// #define HMAX	WIN_HEIGHT - YBOX -RMAX -10 // max initial height
// #define HMIN	HMAX - 150		// min initial height
#define VMIN	10				// min initial hor. speed
#define VMAX	20				// max initial hor. speed
#define PI		3.14
#define AMIN	0				// min initial hor. speed
#define AMAX	2*PI			// max initial hor. speed
#define DELTA_A	10*PI/180		// max alpha variation per step (rad)
#define DELTA_V	20				// max vel variation per step (m/s)
// #define DUMP	.9				// dumping coefficient
#define TSCALE	10				// time scale factor
#define TLEN	30				// circular buffer length
//-----------------------------------------------------
#define BKG		20				// background color box
// #define MCOL	14				// menu color
// #define NCOL	7				// numbers color
#define TCOL	GREEN			// trail color
//-----------------------------------------------------
#define XBOX	5				// left box X coordinate
#define YBOX	5				// upper box Y coordinate
#define RBOX	795				// right box X coordinate
#define BBOX	595				// bottom box Y coordinate
#define LBOX	RBOX - XBOX -1	// X length of the fly box (RBOX - XBOX -1)
#define HBOX	BBOX - YBOX - 1	// Y height of the fly box
#define FLEV	5				// floor Y level (in world)
//-----------------------------------------------------

//-----------------------------------------------------
// STRUCT
//-----------------------------------------------------
struct status {		// fly structure
	int c;			// color [1-15]
	float r;		// radius (m)
	float x;		// x coordinate (m)
	float y;		// y coordinate (m)
	float v;		// velocity (m/s)
	float alpha;	// orientation angle (rads)
};
struct cbuf {		// circular buffer structure
	int top;		// index of the current element
	int x[TLEN];	// array of x coordinates
	int y[TLEN];	// array of y coordinates
};

//-----------------------------------------------------
// PRIVATES
// void store_trail(int i);
// void draw_trail(int i, int w);
// void draw_fly(int i);
// void handle_bounce(int i);
// void init_fly(int i);
// void *flytask(void* arg);
//-----------------------------------------------------
void *display(void* arg);
void *interp(void* arg);

#endif