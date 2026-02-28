#include <ncurses.h>
#include "editor.h"

void drawMask(const Mask& mask, int cursorY, int cursorX) {
	 for (int y = 0; y < mask.HEIGHT; y++) {
       	 for (int x = 0; x < mask.WIDTH; x++) {
		if (mask.isSolid(y,x)) {
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
	mouseinterval(0);
	raw();
	keypad(stdscr, TRUE);
	noecho();

	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

	int cursorX = mask.WIDTH / 2 ;
	int cursorY = mask.HEIGHT / 2 ;



	int ch;
	while((true))
        {
		drawMask(mask, cursorY, cursorX);
		
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
			case ' ':
				mask.setSolid(cursorY, cursorX, !(mask.isSolid(cursorY, cursorX)));
				break;
			case KEY_MOUSE:
			{
   				 MEVENT event;
   				 if (getmouse(&event) == OK) {
          			 	   int maskX = event.x - 1;  // отступ слева
   					   int maskY = event.y - 1;  // отступ сверху
       					    
     					   // Проверяем, что координаты внутри маски
       					 if (maskX >= 0 && maskX < mask.WIDTH && 
         				   maskY >= 0 && maskY < mask.HEIGHT) {
					 	if(event.bstate & (BUTTON1_CLICKED | BUTTON1_PRESSED)) {
							mask.setSolid(cursorY, cursorX, true);
						}
						else if(event.bstate & (BUTTON3_CLICKED | BUTTON3_PRESSED)) 	{
							mask.setSolid(cursorY, cursorX, false);
						}				
                                
     		   }
 		   }
 			   break;
			}
		 }

	}
	endwin();
	return 0;	
}
