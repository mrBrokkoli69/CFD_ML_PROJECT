#include "geometry.h"

Mask createEmptyMask(int nx, int ny) {
    Mask mask;
    mask.nx = nx;
    mask.ny = ny;
    mask.solid.assign(ny, std::vector<bool>(nx, false));
    return mask;
}

void addCylinder(Mask& mask, int cx, int cy, int r) {
    for (int y = 0; y < mask.ny; ++y) {
        for (int x = 0; x < mask.nx; ++x) {
            int dx = x - cx;
            int dy = y - cy;

            if (dx * dx + dy * dy <= r * r) {
                mask.solid[y][x] = true;
            }
        }
    }
}
