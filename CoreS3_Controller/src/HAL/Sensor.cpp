#include <M5Unified.h>
#include "Sensor.h"

bool Sensor::init() {
    M5.Power.setExtOutput(true);//外部に電源
    delay(100);//少し待つことで，qmp未反応を防止．
    Wire.begin(2, 1);

    if (!sht3x.begin(&Wire, SHT3X_I2C_ADDR, 2, 1, 400000U)) {//I2C通信における，SHT3Xだけを呼び出している
        return false;
    }
    if (!qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 2, 1, 400000U)) {
        return false;
    }

    return true;
}

sensor_Data Sensor::sendData() {
    sensor_Data data{};
    if(sht3x.update() && qmp.update()) {
        data.temp = sht3x.cTemp;
        data.hum = sht3x.humidity;
        data.pres = qmp.pressure / 100;
        data.flag = true;
    }
    return data;
}

void Sensor::fina() {
    M5.Power.setExtOutput(false);
    Wire.end();

    delay(50);//一瞬の遷移を防ぐ．
}