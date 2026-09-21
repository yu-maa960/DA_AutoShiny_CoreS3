#include <cstring>
#include "Co_Info.h"

void Co_Info::setMusicFile(const char* fileName) {
    strncpy(selectedMusicFile, fileName, sizeof(selectedMusicFile) - 1);
    selectedMusicFile[sizeof(selectedMusicFile) - 1] = '\0';
}

const char* Co_Info::getMusicFileName() {
    return selectedMusicFile;
}