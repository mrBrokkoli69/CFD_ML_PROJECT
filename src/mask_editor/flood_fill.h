#ifndef FLOOD_FILL_H
#define FLOOD_FILL_H

#include "editor.h"


void floodFill(Mask& mask, int startY, int startX, bool targetValue, bool newValue);


void clearMask(Mask& mask);


#endif
