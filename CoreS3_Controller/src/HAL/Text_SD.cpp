#include "Text_SD.h"

namespace System_SD {
    bool writeBinary(const char* path, uint8_t* data, size_t length)
    {
        File file = SD.open(path, FILE_WRITE);
        if (!file) {
            Serial.printf("Failed to open %s for writing\n", path);
            return false;
        }

        size_t written = file.write(data, length);
        file.close();
        if (written != length) {
            Serial.println("Write Error: Data length mismatch");
            return false;
        }
        return true;
    }

    bool appendText(const char* path, const char* text)
    {
        File file = SD.open(path, FILE_APPEND);
        if (!file) {
            return false;
        }
        
        file.println(text);
        file.close();
        return true;
    }
};