#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>
#include "editor.h"


std::string saveMask(const Mask& mask); 
void loadMask(Mask& mask, const std::string& filename);
std::string getNextMaskFilename();



#endif
