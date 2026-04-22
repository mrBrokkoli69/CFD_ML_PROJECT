#include "shape_generator.h"
#include <math.h>

void drawCircle(EditorMask& mask, int cx, int cy, int r) {
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

void drawRectangle(EditorMask& mask, int x1, int y1, int x2, int y2) {
    // Упорядочиваем координаты (чтобы x1 <= x2, y1 <= y2)
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    
    // Проходим по всем Y и X внутри прямоугольника
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            mask.setSolid(y, x, true);  // порядок (y, x) как в Mask
        }
    }
}

void drawEllipse(EditorMask& mask, int cx, int cy, int rx, int ry) {
    int x = 0;
    int y = ry;
    
    // Начальные параметры решения для первой части
    long long rx2 = (long long)rx * rx;
    long long ry2 = (long long)ry * ry;
    long long twoRx2 = 2 * rx2;
    long long twoRy2 = 2 * ry2;
    
    long long p;
    long long px = 0;
    long long py = twoRx2 * y;
    
    // Часть 1: наклон < 1
    p = (long long)(ry2 - rx2 * ry + (0.25 * rx2));
    while (px < py) {
        // Рисуем 4 симметричные точки
        if (cy + y >= 0 && cy + y < mask.HEIGHT && cx + x >= 0 && cx + x < mask.WIDTH)
            mask.setSolid(cy + y, cx + x, true);
        if (cy + y >= 0 && cy + y < mask.HEIGHT && cx - x >= 0 && cx - x < mask.WIDTH)
            mask.setSolid(cy + y, cx - x, true);
        if (cy - y >= 0 && cy - y < mask.HEIGHT && cx + x >= 0 && cx + x < mask.WIDTH)
            mask.setSolid(cy - y, cx + x, true);
        if (cy - y >= 0 && cy - y < mask.HEIGHT && cx - x >= 0 && cx - x < mask.WIDTH)
            mask.setSolid(cy - y, cx - x, true);
        
        x++;
        px += twoRy2;
        
        if (p < 0) {
            p += ry2 + px;
        } else {
            y--;
            py -= twoRx2;
            p += ry2 + px - py;
        }
    }
    
    // Часть 2: наклон > 1
    p = (long long)(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
    while (y >= 0) {
        // Рисуем 4 симметричные точки
        if (cy + y >= 0 && cy + y < mask.HEIGHT && cx + x >= 0 && cx + x < mask.WIDTH)
            mask.setSolid(cy + y, cx + x, true);
        if (cy + y >= 0 && cy + y < mask.HEIGHT && cx - x >= 0 && cx - x < mask.WIDTH)
            mask.setSolid(cy + y, cx - x, true);
        if (cy - y >= 0 && cy - y < mask.HEIGHT && cx + x >= 0 && cx + x < mask.WIDTH)
            mask.setSolid(cy - y, cx + x, true);
        if (cy - y >= 0 && cy - y < mask.HEIGHT && cx - x >= 0 && cx - x < mask.WIDTH)
            mask.setSolid(cy - y, cx - x, true);
        
        y--;
        py -= twoRx2;
        
        if (p > 0) {
            p += rx2 - py;
        } else {
            x++;
            px += twoRy2;
            p += rx2 - py + px;
        }
    }
}



void drawNACA0012(EditorMask& mask, int cx, int cy, int chord) {
    // chord — длина хорды в пикселях (сколько точек по X)
    for (int x = 0; x <= chord; x++) {
        // Нормализованная координата от 0 до 1
        double xn = (double)x / chord;

        // Формула толщины для NACA 0012
        // t = 0.12 (12% толщины)
        double yt = 0.6 * (0.2969 * sqrt(xn)
                         - 0.1260 * xn
                         - 0.3516 * xn * xn
                         + 0.2843 * xn * xn * xn
                         - 0.1015 * xn * xn * xn * xn);

        // Переводим толщину в пиксели
        int thickness = (int)(yt * chord);

        // Верхняя и нижняя поверхности
        int yUpper = cy - thickness;
        int yLower = cy + thickness;

        // Рисуем вертикальную линию в этой точке хорды
        for (int y = yUpper; y <= yLower; y++) {
            if (x + cx >= 0 && x + cx < mask.WIDTH &&
                y >= 0 && y < mask.HEIGHT) {
                mask.setSolid(y, x + cx, true);
            }
        }
    }
}


void drawNACA2412(EditorMask& mask, int cx, int cy, int chord) {
    // Параметры NACA 2412
    double m = 0.02;  // максимальная кривизна (2%)
    double p = 0.4;   // положение макс. кривизны (40% от носка)
    double t = 0.12;  // максимальная толщина (12%)
    
    for (int x = 0; x <= chord; x++) {
        // Нормализованная координата от 0 до 1
        double xn = (double)x / chord;
        
        // 1. Толщина (та же, что у NACA 0012)
        double yt = (t/0.2) * (0.2969 * sqrt(xn) 
                              - 0.1260 * xn 
                              - 0.3516 * xn * xn 
                              + 0.2843 * xn * xn * xn 
                              - 0.1015 * xn * xn * xn * xn);
        
        // 2. Средняя линия (кривизна)
        double yc;
        double dyc_dx;  // производная для угла наклона
        
        if (xn <= p) {
            // Передняя часть (до точки макс. кривизны)
            yc = (m / (p * p)) * (2 * p * xn - xn * xn);
            dyc_dx = (2 * m / (p * p)) * (p - xn);
        } else {
            // Задняя часть (после точки макс. кривизны)
            yc = (m / ((1 - p) * (1 - p))) * ((1 - 2 * p) + 2 * p * xn - xn * xn);
            dyc_dx = (2 * m / ((1 - p) * (1 - p))) * (p - xn);
        }
        
        // 3. Угол наклона средней линии
        double theta = atan(dyc_dx);
        
        // 4. Координаты верхней и нижней поверхности
        int xPos = cx + x;
        
        // Верхняя поверхность
        int yUpper = cy - (int)(yc * chord) - (int)(yt * chord * cos(theta));
        // Нижняя поверхность
        int yLower = cy - (int)(yc * chord) + (int)(yt * chord * cos(theta));
        
        // Рисуем вертикальную линию между верхней и нижней поверхностями
        if (xPos >= 0 && xPos < mask.WIDTH) {
            for (int y = yUpper; y <= yLower; y++) {
                if (y >= 0 && y < mask.HEIGHT) {
                    mask.setSolid(y, xPos, true);
                }
            }
        }
    }
}


