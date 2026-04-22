#ifndef SHAPE_GENERATOR_H
#define SHAPE_GENERATOR_H

#include "editor.h"

void drawCircle(EditorMask& mask, int cx, int cy, int r);
void drawRectangle(EditorMask& mask, int x1, int y1, int x2, int y2);
void drawEllipse(EditorMask& mask, int cx, int cy, int rx, int ry);
void drawNACA0012(EditorMask& mask, int cx, int cy, int chord);
void drawNACA2412(EditorMask& mask, int cx, int cy, int chord);

#endif
