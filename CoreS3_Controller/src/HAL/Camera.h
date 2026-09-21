#pragma once

#include <esp_camera.h>
#include "Common/CameraFrame.h"


class Camera {
    inline static camera_config_t config{};
    //Gc0308のピンマップ
    static constexpr int SIOC  = -1;
    static constexpr int SIOD  = -1;//Wire1を使うため11,12を指定しない．
    static constexpr int XCLK  = -1;//ESP32のPinはつかわない．
    static constexpr int VSYNC = 46;
    static constexpr int HREF  = 38;
    static constexpr int PCLK  = 45;
    static constexpr int D0    = 39;
    static constexpr int D1    = 40;
    static constexpr int D2    = 41;
    static constexpr int D3    = 42;
    static constexpr int D4    = 15;
    static constexpr int D5    = 16;
    static constexpr int D6    = 48;
    static constexpr int D7    = 47;
    static constexpr int RESET = -1;
    static constexpr int PWDN  = -1;
public:
    Camera() = delete;
    static bool init();
    static void drawPicture(CameraFrame& outFrame);
    static void capture(CameraFrame& outFrame);
    static bool fina(); 
};