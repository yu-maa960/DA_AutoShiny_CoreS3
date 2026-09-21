#include "AppSensor.h"
#include "Common/App_State.h"
#include "Common/sensor_Data.h"
#include "Common/SystemTypes.h"
#include "HAL/Sensor.h"
#include "HAL/Text_SD.h"
#include "HAL/MyUI.h"
#include "OS/CoreOS.h"
void AppSensor::setup() {
    if (!Sensor::init()) {
        MyUI::showErrorMes("Failed to initiialize Sensor");
        init_success = false;
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = Mode::HOME;
        CoreOS::getInstance().post(req);
        return;
    }
    CoreOS::getInstance().setDisplayEcoMode(true);
    CoreOS::getInstance().setCpuEcoMode(true);
    LastTime = millis();
}

void AppSensor::loop() {
    if (!init_success) {
        return;
    }
    Sensor_Touch result{MyUI::checkSensorSelection()};
    if (result == Sensor_Touch::Valid) {
        stop();
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = Mode::HOME;
        CoreOS::getInstance().post(req);
        return;
    }

    if ((millis() - LastTime) >= LOG_INTERVAL || firstDrawing) {
        firstDrawing = false;
        sensor_Data data{Sensor::sendData()};
        if (data.flag) {

            temp = data.temp;
            hum = data.hum;
            pres = data.pres / 100.0f;

            auto dt = M5.Rtc.getDateTime();

            char logEntry[64]{};
            snprintf(logEntry, sizeof(logEntry), "%04d/%02d/%02d %02d:%02d:%02d, %.2f, %.2f, %.2f\n",
                    dt.date.year, dt.date.month, dt.date.date,
                    dt.time.hours, dt.time.minutes, dt.time.seconds,
                    temp, hum, pres);

            snprintf(lastTimeString, sizeof(lastTimeString), "Last: %02d/%02d %02d:%02d:%02d",
                    dt.date.month, dt.date.date, dt.time.hours, dt.time.minutes, dt.time.seconds);

            bool result{System_SD::appendText(outputFileName, logEntry)};
            if (!result) {
                MyUI::showErrorMes("Failed to append texts");
            }
            LastTime = millis();
        } else {
            MyUI::showErrorMes("Read Error");
        }
        MyUI::drawSensor(temp, hum, pres, lastTimeString);
    }
}

void AppSensor::stop()
{
    Sensor::fina(); 
    CoreOS::getInstance().setDisplayEcoMode(false);
    CoreOS::getInstance().setCpuEcoMode(false);
}