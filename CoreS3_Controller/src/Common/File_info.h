#pragma once

#include <vector>

enum class FileType {
    UNKNOWN,
    JPG,
    WAV,
};

struct ExtensionPair {
    const char* key;
    FileType value;
};

static constexpr ExtensionPair extMap[] =
    {
        {".jpg", FileType::JPG},
        {".wav", FileType::WAV},
    };
