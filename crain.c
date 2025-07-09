#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
void usleep(long);
#endif

static void sleep_seconds(float seconds) {
	if (seconds < 0) {
		return;
	}

#ifdef _WIN32
	Sleep((DWORD)(seconds * 1000));
#else
	usleep((long)(seconds * 1000000));
#endif
}

static float get_time() {
	static clock_t start_time = 0;
	if (start_time == 0)
		start_time = clock();

	return (float)(clock() - start_time) / CLOCKS_PER_SEC;
}

typedef struct {
	uint64_t x, y, height;
	uint64_t frame_threshold;
	bool decay;
} Droplet;

typedef struct {
	Droplet* data;
	size_t size;
	size_t max_size;
	size_t length;
} Droplets;

void add_droplet(Droplets* droplets) {
	Droplet droplet;
	size_t i;

	droplet.x = rand() % getmaxx(stdscr);
	droplet.y = 0;
	droplet.height = rand() % 3 + 1;
	droplet.frame_threshold = rand() % 3 + 1;
	droplet.decay = false;

	for (i = 0; i < droplets->length; i++) {
		if (droplets->data[i].decay) {
			memcpy(&droplets->data[i], &droplet, sizeof(Droplet));
			return;
		}
	}

	if (droplets->length < 200) {
		droplets->data = realloc(droplets->data, droplets->size + sizeof(Droplet));
		droplets->data[droplets->length] = droplet;

		droplets->size += sizeof(Droplet);
		droplets->length++;
	}
}

static void crain_init_color(short color) {
	static short i = 1;
	init_pair(i, color, 0);
	i++;
}

int main() {
	Droplets droplets;
	uint64_t frames = 0;

	setlocale(LC_ALL, "");

	initscr();
	curs_set(0);
	nodelay(stdscr, TRUE);
	noecho();
	start_color();

	/*
	crain_init_color(39);
	crain_init_color(33);
	*/
	crain_init_color(27);
	crain_init_color(21);
	crain_init_color(20);
	crain_init_color(19);
	crain_init_color(18);
	crain_init_color(17);

	droplets.length = 0;
	droplets.size = 0;
	droplets.data = NULL;
	add_droplet(&droplets);

	for (;;) {
		size_t i, j, color;
		int k;

		clear();
		for (i = 0; i < droplets.length; i++) {
			Droplet* droplet = &droplets.data[i];
			color = droplet->frame_threshold;
			attron(COLOR_PAIR(color));
			for (j = droplet->y; j < droplet->height + droplet->y; j++) {
				if (j >= getmaxy(stdscr) + 10) {
					droplet->decay = true;
				}
				mvprintw(j, droplet->x, "│");
			}
			attroff(COLOR_PAIR(color));
			if (frames % droplet->frame_threshold == 0) {
				droplet->y++;
			}
		}
		refresh();
		k = getch();
		if (k == 'q')
			break;
		if (frames % 2 == 0)
			add_droplet(&droplets);
		sleep_seconds(0.003f);
		frames++;
	}

	endwin();

	free(droplets.data);
	return 0;
}
