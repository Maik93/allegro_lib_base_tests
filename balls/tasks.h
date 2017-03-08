#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H
//-----------------------------------------------------
// BALL TASK CONSTANTS
//-----------------------------------------------------
#define PER		20	// task period in ms
#define DREL	PER	// relative deadline in ms
#define PRI		60	// task priority
//-----------------------------------------------------
// BALL CONSTANTS
//-----------------------------------------------------
#define RMIN	5				// min radius
#define RMAX	15				// man radius
#define G0		9.8				// acceleration of gravity
#define HMAX	WIN_HEIGHT - YBOX -RMAX -10 // max initial height
#define HMIN	HMAX - 150		// min initial height
#define VXMIN	20				// min initial hor. speed
#define VXMAX	10				// max initial hor. speed
#define DUMP	.9				// dumping coefficient
#define TSCALE	10				// time scale factor
#define TLEN	30				// circular buffer length
//-----------------------------------------------------
#define BKG		20				// background color
#define MCOL	14				// menu color
#define NCOL	7				// numbers color
#define TCOL	GREEN			// trail color
//-----------------------------------------------------
#define XBOX	5				// left box X coordinate
#define YBOX	5				// upper box Y coordinate
#define RBOX	795				// right box X coordinate
#define BBOX	595				// bottom box Y coordinate
#define LBOX	RBOX - XBOX -1	// X length of the ball box (RBOX - XBOX -1)
#define HBOX	BBOX - YBOX - 1	// Y height of the ball box
#define FLEV	5				// floor Y level (in world)
//-----------------------------------------------------

//-----------------------------------------------------
// STRUCT
//-----------------------------------------------------
struct status {		// ball structure
	int c;			// color [1,15]
	float r;		// radius (m)
	float x;		// x coordinate (m)
	float y;		// y coordinate (m)
	float vx;		// horizontal velocity (m/s)
	float vy;		// vertical velocity (m/s)
	float v0;		// jumping velocity (m/s)
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
// void draw_ball(int i);
// void handle_bounce(int i);
// void init_ball(int i);
// void *balltask(void* arg);
//-----------------------------------------------------
void *display(void* arg);
void *interp(void* arg);

#endif