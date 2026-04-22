#include <ncurses.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "editor.h"
#include "shape_generator.h"
#include "flood_fill.h"
#include "file_io.h"

struct RunParams {
	double tau = 0.6;
	double uMax = 0.05;
	double rho0 = 1.0;
	int maxSteps = 3000;
	int coutInterval = 1000;
	int vtkInterval = 100;
	double characteristicLength = 20.0;
};




void drawMask(const Mask& mask, int cursorY, int cursorX) {
	for (int y = 0; y < mask.HEIGHT; y++) {
		for (int x = 0; x < mask.WIDTH; x++) {
			if (mask.isSolid(y, x)) {
				mvaddch(y + 1, x + 1, '#');
			} else {
				mvaddch(y + 1, x + 1, '.');
			}
		}
	}

	mvaddch(cursorY + 1, cursorX + 1, 'X');
	refresh();
}

int inputInt(const char* prompt) {
	char buffer[16];

	echo();
	mvprintw(LINES - 2, 0, "%s: ", prompt);
	clrtoeol();
	getstr(buffer);
	noecho();

	return atoi(buffer);
}

std::string inputString(const char* prompt) {
	char buffer[256];

	echo();
	mvprintw(LINES - 2, 0, "%s: ", prompt);
	clrtoeol();
	getstr(buffer);
	noecho();

	return std::string(buffer);
}

double inputDouble(const char* prompt, double defaultValue) {
	char buffer[54];
	echo();
	mvprintw(LINES - 2, 0, "%s [%.4f]: ", prompt, defaultValue);
	clrtoeol();
	getstr(buffer);
	noecho();

	if (buffer[0] == '\0') {
		return defaultValue;

	}

	return atof(buffer);


}



void clearMessageArea(const Mask& mask) {
	for (int i = 0; i < 15; ++i) {
		move(mask.HEIGHT + 5 + i, 0);
		clrtoeol();
	}
}

std::string selectMaskFromList(const Mask& mask) {
	std::vector<std::string> masks = getMaskList();

	clearMessageArea(mask);

	if (masks.empty()) {
		mvprintw(mask.HEIGHT + 5, 0, "No saved masks");
		return "";
	}

	move(mask.HEIGHT + 6, 0);
	printw("=== Available masks ===");

	int count = 0;
	for (const auto& name : masks) {
		if (count >= 10) {
			break;
		}

		move(mask.HEIGHT + 7 + count, 0);
		printw("%d: %s", count + 1, name.c_str());
		count++;
	}

	move(mask.HEIGHT + 18, 0);
	printw("Enter number (1-%d) or 0 to cancel: ", count);
	refresh();

	echo();

	char input[16];
	move(mask.HEIGHT + 18, 35);
	clrtoeol();
	getstr(input);

	noecho();

	int choice = atoi(input);
	if (choice <= 0 || choice > count) {
		mvprintw(mask.HEIGHT + 5, 0, "Cancelled");
		return "";
	}

	return masks[choice - 1];
}

RunParams inputRunParams(const Mask& mask) {
	clearMessageArea(mask);

	RunParams params;
	params.tau = inputDouble("tau", params.tau);
	params.uMax = inputDouble("uMax", params.uMax);
	params.rho0 = inputDouble("rho0", params.rho0);
	params.maxSteps = inputInt("maxSteps");
	if (params.maxSteps <= 0) params.maxSteps = 5000;

	params.coutInterval = inputInt("coutInterval");
	if (params.coutInterval <= 0) params.coutInterval = 1000;

	params.vtkInterval = inputInt("vtkInterval (0 = off)");
	if (params.vtkInterval < 0) params.vtkInterval = 0;

	params.characteristicLength = inputDouble("characteristicLength", params.characteristicLength);

	clearMessageArea(mask);
	return params;
}






int main() {
	Mask mask;

	initscr();
	mouseinterval(0);
	raw();
	keypad(stdscr, TRUE);
	noecho();
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

	int cursorX = mask.WIDTH / 2;
	int cursorY = mask.HEIGHT / 2;

	int ch;
	while (true) {
		drawMask(mask, cursorY, cursorX);

		move(mask.HEIGHT + 2, 0);
		clrtoeol();
		printw("Mask editor v1.0");
		move(mask.HEIGHT + 3, 0);
		clrtoeol();
		printw("Press q to quit");
		move(mask.HEIGHT + 4, 0);
		clrtoeol();
		printw("f - flood fill | c - clear all | p - save mask | l - load mask | r - run solver | x - delete mask | C - draw Circle | R - draw Rectangle | E - draw Ellipse | N - draw NASA form");
		refresh();

		ch = getch();
		if (ch == 'q') {
			break;
		}

		switch (ch) {
			case KEY_UP:
			case 'w':
				if (cursorY > 0) cursorY--;
				break;

			case KEY_DOWN:
			case 's':
				if (cursorY < mask.HEIGHT - 1) cursorY++;
				break;

			case KEY_LEFT:
			case 'a':
				if (cursorX > 0) cursorX--;
				break;

			case KEY_RIGHT:
			case 'd':
				if (cursorX < mask.WIDTH - 1) cursorX++;
				break;

			case ' ':
				mask.setSolid(cursorY, cursorX, !mask.isSolid(cursorY, cursorX));
				break;

			case 'C': {
					  int cx = inputInt("Circle center X");
					  int cy = inputInt("Circle center Y");
					  int r = inputInt("Circle radius");
					  drawCircle(mask, cx, cy, r);
					  break;
				  }

			case KEY_MOUSE: {
						MEVENT event;
						if (getmouse(&event) == OK) {
							int maskX = event.x - 1;
							int maskY = event.y - 1;

							if (maskX >= 0 && maskX < mask.WIDTH &&
									maskY >= 0 && maskY < mask.HEIGHT) {
								cursorX = maskX;
								cursorY = maskY;

								if (event.bstate & (BUTTON1_CLICKED | BUTTON1_PRESSED)) {
									mask.setSolid(cursorY, cursorX, true);
								} else if (event.bstate & (BUTTON3_CLICKED | BUTTON3_PRESSED)) {
									mask.setSolid(cursorY, cursorX, false);
								}
							}
						}
						break;
					}

			case 'f': {
					  bool targetValue = mask.isSolid(cursorY, cursorX);
					  bool newValue = !targetValue;
					  floodFill(mask, cursorY, cursorX, targetValue, newValue);
					  break;
				  }

			case 'c':
				  clearMask(mask);
				  break;

			case 'p': {
					  std::string userName = inputString("Mask name (empty = auto)");
					  std::string filename;

					  if (userName.empty()) {
						  filename = saveMask(mask);
					  } else {
						  filename = saveMaskAs(mask, userName);
					  }

					  clearMessageArea(mask);
					  if (!filename.empty()) {
						  mvprintw(mask.HEIGHT + 5, 0, "Saved to %s", filename.c_str());
					  } else {
						  mvprintw(mask.HEIGHT + 5, 0, "Save failed!");
					  }
					  break;
				  }

			case 'l': {
					  std::string selectedName = selectMaskFromList(mask);
					  if (selectedName.empty()) {
						  break;
					  }

					  std::string fullPath = buildMaskPath(selectedName);
					  loadMask(mask, fullPath);

					  clearMessageArea(mask);
					  mvprintw(mask.HEIGHT + 5, 0, "Loaded: %s", selectedName.c_str());
					  break;
				  }

			case 'x': {
					  std::string selectedName = selectMaskFromList(mask);
					  if (selectedName.empty()) {
						  break;
					  }

					  clearMessageArea(mask);
					  if (deleteMaskFile(selectedName)) {
						  mvprintw(mask.HEIGHT + 5, 0, "Deleted: %s", selectedName.c_str());
					  } else {
						  mvprintw(mask.HEIGHT + 5, 0, "Delete failed: %s", selectedName.c_str());
					  }
					  break;
				  }

			case 'r': {
					  std::string selectedName = selectMaskFromList(mask);
					  if (selectedName.empty()) {
						  break;
					  }

					  RunParams params = inputRunParams(mask);

					  std::string fullPath = buildMaskPath(selectedName);

					  std::string command =
						  "./lbm_test \"" + fullPath + "\" " +
						  std::to_string(params.tau) + " " +
						  std::to_string(params.uMax) + " " +
						  std::to_string(params.rho0) + " " +
						  std::to_string(params.maxSteps) + " " +
						  std::to_string(params.coutInterval) + " " +
						  std::to_string(params.vtkInterval) + " " +
						  std::to_string(params.characteristicLength);

					  clearMessageArea(mask);
					  mvprintw(mask.HEIGHT + 5, 0, "Running solver on: %s", selectedName.c_str());
					  refresh();

					  endwin();
					  int status = std::system(command.c_str());

					  initscr();
					  mouseinterval(0);
					  raw();
					  keypad(stdscr, TRUE);
					  noecho();
					  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

					  clearMessageArea(mask);
					  if (status == 0) {
						  mvprintw(mask.HEIGHT + 5, 0, "Solver finished for: %s", selectedName.c_str());
					  } else {
						  mvprintw(mask.HEIGHT + 5, 0, "Solver failed for: %s", selectedName.c_str());
					  }
					  break;
				  }

			case 'R': {
					  int x1 = inputInt("Rectangle x1");
					  int y1 = inputInt("Rectangle y1");
					  int x2 = inputInt("Rectangle x2");
					  int y2 = inputInt("Rectangle y2");
					  drawRectangle(mask, x1, y1, x2, y2);
					  break;
				  }

			case 'E': {
					  int cx = inputInt("Ellipse center X");
					  int cy = inputInt("Ellipse center Y");
					  int rx = inputInt("Ellipse radius X");
					  int ry = inputInt("Ellipse radius Y");
					  drawEllipse(mask, cx, cy, rx, ry);
					  break;
				  }

			case 'N': {
					  int cx = inputInt("NACA center X");
					  int cy = inputInt("NACA center Y");
					  int chord = inputInt("NACA chord length");
					  int digits = inputInt("NACA digits (0012 or 2412)");

					  if (digits == 12) {
						  drawNACA0012(mask, cx, cy, chord);
					  } else if (digits == 2412) {
						  drawNACA2412(mask, cx, cy, chord);
					  } else {
						  mvprintw(LINES - 3, 0, "Unknown NACA profile: %d", digits);
						  getch();
					  }
					  break;
				  }
		}
	}

	endwin();
	return 0;
}
