#include <allegro.h>
#include "utils.h"
#include "tasks.h"

#define RADIUS	50	// disc radius
#define MAX_HELLO	5	// max number of hello tasks

int main(int argc, char const *argv[]) {
	int x = 100, y = 400, i = 0;
	char str[20], str2[20];
	float pi = 3.14159;

	init_graphic();

	circlefill(screen_buff, x, y, RADIUS, RED);

	sprintf(str, "pi = %5.2f", pi);
	textout_ex(screen_buff, font, str, 5, 8, GREEN, GND);

	textout_centre_ex(screen_buff, font, "centered text", WIN_WIDTH / 2, WIN_HEIGHT / 2, GREEN, GND);

	// Here's a graphical input text
	/*textout_ex(screen_buff, font, "Hi, write something: ", 10, 30, WHITE, 0); // prompt
	get_string(str, 10 + 21 * CHAR_WIDTH, 30, WHITE, 0); // read data with echo (21 prompt characters)
	// sscanf(str, "%f", &x); // convert string into float
	sprintf(str2, "You wrote: %s", str);
	textout_ex(screen_buff, font, str2, 10, 30 + CHAR_HEIGHT, WHITE, 0); // prompt*/

	sprintf(str, "Press SPACE to create a task");
	textout_centre_ex(screen_buff, font, str, WIN_WIDTH / 2, 20, GREEN, GND);

	do {
		// draws pixels with left_key of mouse
		if (mouse_b & 1) {
			x = mouse_x;
			y = mouse_y;
			putpixel(screen_buff, x, y, RED);
		}
		// every space pressed adds a new task
		if (listen_scancode() == KEY_SPACE && i < MAX_HELLO) {
			// task_create(i, task_hello, PER + i * PINC, PER + i * PINC, 50, ACT);
			create_and_start_task(task_hello, PER + i * PINC, PER + i * PINC, 20);
			printf("Space pressed. Spawning task %d, period %d.\n", i+1, PER + i * PINC);
			i++;
		}
	} while (!key[KEY_ESC]);

	kill_all_task();
	allegro_exit();
	return 0;
}