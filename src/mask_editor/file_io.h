#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>
#include <vector>
#include "editor.h"

std::string getNextMaskFilename();
std::string buildMaskPath(const std::string& filename);

std::string saveMask(const EditorMask& mask);
std::string saveMaskAs(const EditorMask& mask, const std::string& baseName);

bool loadMask(EditorMask& mask, const std::string& filename);
std::vector<std::string> getMaskList();

bool deleteMaskFile(const std::string& filename);

#endif
