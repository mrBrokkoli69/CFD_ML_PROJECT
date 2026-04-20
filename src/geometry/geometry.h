#pragma once

#include "../lbm/lbm_core.h"

Mask createEmptyMask(int nx, int ny);
void addCylinder(Mask& mask, int cx, int cy, int r);
