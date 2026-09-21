#pragma once
#include <M5Unified.h>

struct TouchData {
    int x{};
    int y{};
    int deltaX{};
    int deltaY{};

    bool isPressed{false};
    bool wasReleased{false};
    bool validTap{false};
};

class Touch_Input {
private:
    static constexpr uint32_t DEBOUNCE_DELAY{300};
    static constexpr uint32_t TOUCH_TIME{50};
public:
    Touch_Input() = delete;
    static TouchData getTouch();
};