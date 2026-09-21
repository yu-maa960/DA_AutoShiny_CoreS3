#pragma once
#include <Arduino.h>
#include <SD.h>

namespace System_SD {
    bool writeBinary(const char* path, uint8_t* data, size_t length);
    bool appendText(const char* path, const char* text);
};
