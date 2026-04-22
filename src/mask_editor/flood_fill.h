#ifndef FLOOD_FILL_H
#define FLOOD_FILL_H

#include "editor.h"

void floodFill(EditorMask& mask, int startY, int startX, bool targetValue, bool newValue);
void clearMask(EditorMask& mask);

#endif
