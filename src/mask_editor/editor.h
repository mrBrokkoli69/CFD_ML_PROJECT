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

	bool isSolid(int x, int y) const {
		if (((x<0) || (x>= WIDTH)) || ((y<0) || (y >= HEIGHT)) ) {
			return true;
		}
		else {
			return cells[y][x];
		}
	
	}

	void setSolid(int x, int y, bool solid) {
 	   if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        	cells[y][x] = solid;
   		 }
	}


};



#endif
