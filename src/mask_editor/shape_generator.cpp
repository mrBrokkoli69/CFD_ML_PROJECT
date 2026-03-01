#include "shape_generator.h"


#include "shape_generator.h"


void drawCircle(Mask& mask, int cx, int cy, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;  // параметр решения (ошибка)

    while (y >= x) {
        // Рисуем 8 симметричных точек
        // 1-й октант (0° - 45°)
        if (cy + x >= 0 && cy + x < mask.HEIGHT && cx + y >= 0 && cx + y < mask.WIDTH)
            mask.setSolid(cy + x, cx + y, true);

        // 2-й октант (45° - 90°)
        if (cy + y >= 0 && cy + y < mask.HEIGHT && cx + x >= 0 && cx + x < mask.WIDTH)
            mask.setSolid(cy + y, cx + x, true);

        // 3-й октант (90° - 135°)
        if (cy + y >= 0 && cy + y < mask.HEIGHT && cx - x >= 0 && cx - x < mask.WIDTH)
            mask.setSolid(cy + y, cx - x, true);

        // 4-й октант (135° - 180°)
        if (cy + x >= 0 && cy + x < mask.HEIGHT && cx - y >= 0 && cx - y < mask.WIDTH)
            mask.setSolid(cy + x, cx - y, true);

        // 5-й октант (180° - 225°)
        if (cy - x >= 0 && cy - x < mask.HEIGHT && cx - y >= 0 && cx - y < mask.WIDTH)
            mask.setSolid(cy - x, cx - y, true);

        // 6-й октант (225° - 270°)
        if (cy - y >= 0 && cy - y < mask.HEIGHT && cx - x >= 0 && cx - x < mask.WIDTH)
            mask.setSolid(cy - y, cx - x, true);

        // 7-й октант (270° - 315°)
        if (cy - y >= 0 && cy - y < mask.HEIGHT && cx + x >= 0 && cx + x < mask.WIDTH)
            mask.setSolid(cy - y, cx + x, true);

        // 8-й октант (315° - 360°)
        if (cy - x >= 0 && cy - x < mask.HEIGHT && cx + y >= 0 && cx + y < mask.WIDTH)
            mask.setSolid(cy - x, cx + y, true);

        x++;

        // Обновляем параметр решения
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

void drawRectangle(Mask& mask, int x1, int y1, int x2, int y2) {
    // пока пусто
}

void drawEllipse(Mask& mask, int cx, int cy, int rx, int ry) {
    // пока пусто
}
