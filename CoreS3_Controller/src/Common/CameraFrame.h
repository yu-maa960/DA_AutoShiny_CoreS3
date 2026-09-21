#pragma once
#include <stddef.h>
#include <stdint.h>

struct CameraFrame {
    uint8_t* data = nullptr;
    size_t size{};
    bool isValid = false;
};