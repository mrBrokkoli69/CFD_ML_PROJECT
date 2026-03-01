#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>
#include "editor.h"


void saveMask(const Mask& mask, const std::string& filename);
void loadMask(Mask& mask, const std::string& filename);




#endif
