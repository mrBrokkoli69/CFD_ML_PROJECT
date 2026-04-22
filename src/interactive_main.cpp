#include "mask_editor/editor.h"

#include <cstring>
#include <iostream>

// объявление solver entry point
int runSolverMode(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::strcmp(argv[1], "--solver") == 0) {
        return runSolverMode(argc - 1, argv + 1);
    }

    return runEditorApp();
}
