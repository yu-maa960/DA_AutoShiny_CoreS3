#include "Touch_Input.h"

TouchData Touch_Input::getTouch() {
    static uint32_t lastTransitionTime = 0;
    static uint32_t touchStartTime = 0;
    auto detail = M5.Touch.getDetail();

    TouchData data{};

    if (millis() - lastTransitionTime < DEBOUNCE_DELAY) {
        return data; 
    }

    data.x = detail.x;
    data.y = detail.y;
    
    data.isPressed = detail.isPressed();
    data.wasReleased = detail.wasReleased(); 

    if (data.isPressed) {
        if (touchStartTime == 0) {
            touchStartTime = millis();
        }
    }

    if (data.wasReleased) {
        uint32_t holdTime = millis() - touchStartTime;
        touchStartTime = 0;
        if (holdTime > TOUCH_TIME) {    //ノイズを除きたい
            lastTransitionTime = millis();//ここで更新させる
            if (data.x <= 5 || data.y <= 5) {
                return data;
            }

            if (detail.base_x <= 5 || detail.base_y <= 5) {
                return data;
            }

            data.deltaX = data.x - detail.base_x;
            data.deltaY = data.y - detail.base_y;
            data.validTap = true;
        }
    }
    
    return data; 
}