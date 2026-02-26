#include <ncurses.h>

int main() {
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	printw("Mask Editor v1.0\n");
	printw("Press q to quit");
	refresh();
	int ch;
	while((ch = getch()) != 'q')
        { }
	endwin();
	return 0;	
}
