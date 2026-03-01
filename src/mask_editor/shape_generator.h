
#ifndef SHAPE_GENERATOR_H
#define SHAPE_GENERATOR_H

#include "editor.h"

void drawCircle(Mask& mask, int cx, int cy, int r);

void drawRectangle(Mask& mask, int x1, int y1, int x2, int y2);

void drawEllipse(Mask& mask, int cx, int cy, int rx, int ry);

void drawNACA0012(Mask& mask, int cx, int cy, int chord);


#endif
