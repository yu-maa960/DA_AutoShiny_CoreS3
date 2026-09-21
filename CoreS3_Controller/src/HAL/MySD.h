#pragma once

#include <vector>
#include "Common/File_info.h"
class MySD {
public:
    MySD() = delete;
    static bool init(int& fileCount, std::vector<char>& fileNames);
    static const char* extract_FileAdress(const std::vector<char>& fileNames, int cursorIndex);
    static FileType judge_FileType(const char* type);
    static FileType get_FileType(const char* fileHead);
    static File getFile(const char* path);
};