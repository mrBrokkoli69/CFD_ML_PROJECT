#include <ncurses.h>
#include "editor.h"


int main() {
	Mask mask;
	mask.setSolid(10,5,true);
	if (mask.isSolid(10,5)) {}
	mask.setSolid(10,5,false);	



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
