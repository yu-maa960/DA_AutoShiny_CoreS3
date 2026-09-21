#pragma once
#include <M5UnitENV.h>
#include "Common/sensor_Data.h"

class Sensor {
    //センサ: Unit ENV-Ⅲ用
    inline static SHT3X sht3x{};
    inline static QMP6988 qmp{};
public:
    Sensor() = delete;
    static bool init();
    static sensor_Data sendData();
    static void fina();
};