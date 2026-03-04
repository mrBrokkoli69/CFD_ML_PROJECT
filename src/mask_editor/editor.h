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
    
    bool isSolid(int y, int x) const {
        if (y < 0 || y >= HEIGHT || x < 0 || x >= WIDTH) {
            return true;
        }
        return cells[y][x];
    }
    
    void setSolid(int y, int x, bool solid) {
        if (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH) {
            cells[y][x] = solid;
        }
    }
};

void drawMask(const Mask& mask, int cursorY, int cursorX);

int inputInt(const char* prompt);

#endif
