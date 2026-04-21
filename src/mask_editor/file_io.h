#ifndef FILE_IO_H
#define FILE_IO_H
#pragma once

#include "editor.h"

#include <string>
#include <vector>

std::string getNextMaskFilename();
std::string buildMaskPath(const std::string& filename);

std::string saveMask(const Mask& mask);
std::string saveMaskAs(const Mask& mask, const std::string& baseName);

bool loadMask(Mask& mask, const std::string& filename);
std::vector<std::string> getMaskList();

bool deleteMaskFile(const std::string& filename);

#endif
