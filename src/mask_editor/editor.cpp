#include <ncurses.h>
#include "editor.h"
#include "shape_generator.h"
#include "flood_fill.h"
#include "file_io.h"



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


int inputInt(const char* prompt) {
	char buffer[16];

	echo();
	mvprintw(LINES - 2 , 0 , "%s: ", prompt);

	clrtoeol();
	getstr(buffer);
	noecho();

	return atoi(buffer);
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
		printw("Press q to quit\n");
		printw("f - flood fill | c - clear all| p - save mask | l - load mask | C - draw Circle | R - draw Rectangle | E - draw Ellipse | N - draw NASA form ");
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
			case 'C':  
				{
					int cx = inputInt("Circle center X");
					int cy = inputInt("Circle center Y");
					int r = inputInt("Circle radius");

					drawCircle(mask, cx, cy, r);
					break;
				}
			case KEY_MOUSE:
				{
					MEVENT event;
					if (getmouse(&event) == OK) { 
						int maskX = cursorX -1;
						int maskY = cursorY - 1;					
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

			case 'f':{
					 // Определяем, что заливаем: целевое значение = текущее под курсором
					 bool targetValue = mask.isSolid(cursorY, cursorX);
					 // Новое значение — противоположное
					 bool newValue = !targetValue;

					 // Вызываем заливку, начиная с позиции курсора
					 floodFill(mask, cursorY, cursorX, targetValue, newValue);
					 break;}

			case 'c':
				 clearMask(mask);
				 break;


			case 'p':
				 saveMask(mask,"mask.dat");
				 mvprintw(mask.HEIGHT +5 ,0,"Mask saved to mask.dat");
				 break;
			case 'l':
				 loadMask(mask,"mask.dat");
				 mvprintw(mask.HEIGHT +5 ,0,"Mask loaded from mask.dat");
				 break;
			case 'R':  // заглавная R (Shift+r)
				 {
					 int x1 = inputInt("Rectangle x1");
					 int y1 = inputInt("Rectangle y1");
					 int x2 = inputInt("Rectangle x2");
					 int y2 = inputInt("Rectangle y2");

					 drawRectangle(mask, x1, y1, x2, y2);
					 break;
				 }

			case 'E':  // заглавная E (Shift+e)
				 {
					 int cx = inputInt("Ellipse center X");
					 int cy = inputInt("Ellipse center Y");
					 int rx = inputInt("Ellipse radius X");
					 int ry = inputInt("Ellipse radius Y");

					 drawEllipse(mask, cx, cy, rx, ry);
					 break;
				 }
			case 'N':  // заглавная N (Shift+n)
				 {
					 int cx = inputInt("NACA center X");
					 int cy = inputInt("NACA center Y");
					 int chord = inputInt("NACA chord length");
					 int digits = inputInt("NACA digits (0012 or 2412)");

					 if (digits == 12 || digits == 12) {  // для 0012 (ведущие нули теряются, поэтому 12)
						 drawNACA0012(mask, cx, cy, chord);
					 } else if (digits == 2412) {
						 drawNACA2412(mask, cx, cy, chord);
					 } else {
						 // Если ввели что-то другое — покажем сообщение об ошибке
						 mvprintw(LINES - 3, 0, "Unknown NACA profile: %d", digits);
						 getch();  // ждём нажатия, чтобы прочитали сообщение
					 }
					 break;
				 }
		}
	}
	endwin();
	return 0;	
}
