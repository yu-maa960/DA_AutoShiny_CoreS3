#pragma once
#include <cstdint>
#include "AppBase.h"

class AppSensor : public AppBase {
private :
    uint32_t LastTime{};
    float temp{}, hum{}, pres{};
    char lastTimeString[64]{"Waiting for data..."};
    bool init_success = true;
    bool firstDrawing = true;
    static constexpr uint32_t LOG_INTERVAL = 60000;
    static constexpr const char* outputFileName = "/sensor_log.txt";

public :
    AppSensor() {};
    void setup() override;
    void loop() override;
    void stop() override;
};