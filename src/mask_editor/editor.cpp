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
	mvaddch(cursorY + 1, cursorX + 1, 'X');    
	refresh();

}

int main() {
	Mask mask;

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	
	int cursorX = mask.WIDTH / 2 ;
	int cursorY = mask.HEIGHT / 2 ;



	int ch;
	while((true))
        {
		drawMask(mask, cursorX, cursorY);
		
		move(mask.HEIGHT + 2, 0) ;	
		printw("Mask editor v1.0\n");
		printw("Press q to quit");
		refresh();
		

		ch = getch();
		if(ch == 'q') {
			break;
		}
		
		switch(ch) {
			case KEY_UP:
				if(cursorY>0) cursorY--;
				break;
			case KEY_DOWN:
				if(cursorY < mask.HEIGHT - 1) cursorY++;
				break;
			case KEY_LEFT:
				if(cursorX > 0) cursorX--;
				break;
			case KEY_RIGHT:
				if(cursorX < mask.WIDTH - 1) cursorX++;
				break;
			case 'w':
				if(cursorY>0) cursorY--;
				break;
			case 's':
				if(cursorY < mask.HEIGHT - 1) cursorY++;
				break;
			case 'a':
				if(cursorX > 0) cursorX--;
				break;
			case 'd':
				if(cursorX < mask.WIDTH - 1) cursorX++;
				break;

		}

	}

	endwin();
	return 0;	
}
