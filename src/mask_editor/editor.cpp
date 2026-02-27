#include <ncurses.h>
#include "editor.h"

void drawMask(const Mask& mask, int cursorX, int cursorY) {
	 for (int y = 0; y < mask.HEIGHT; y++) {
       	 for (int x = 0; x < mask.WIDTH; x++) {
		if (mask.isSolid(x,y)) {
			mvaddch(y+1,x+1,'#');
		}
		else {
			mvaddch(y+1,x+1,'.');
		}
        }
    }
    
    refresh();

}

int main() {
	Mask mask;

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	
	mask.setSolid(5,6,true);
	mask.setSolid(5,7,true);
	mask.setSolid(5,8,true);

	drawMask(mask,0,0);

	printw("\nMask Editor v1.0\n");
	printw("Press q to quit");
	refresh();
	int ch;
	while((ch = getch()) != 'q')
        { }
	endwin();
	return 0;	
}
