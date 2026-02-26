#ifndef EDITOR_H
#define EDITOR_H

struct Mask {
	static const int WIDTH = 64;
	static const int HEIGHT = 32;

	bool cells[HEIGHT][WIDTH];

	Mask() {
	for (int y = 0; y < HEIGHT; y++) {
        	for (int x = 0; x < WIDTH; x++) {
            		cells[y][x] = false;
        }
    }
}



};



#endif
