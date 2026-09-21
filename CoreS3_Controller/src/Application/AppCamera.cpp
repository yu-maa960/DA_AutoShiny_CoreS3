#include "AppCamera.h"
#include "Common/App_State.h"
#include "Common/CameraFrame.h"
#include "Common/SystemTypes.h"
#include "HAL/Camera.h"
#include "HAL/MyUI.h"
#include "HAL/Text_SD.h"
#include "OS/CoreOS.h"

void AppCamera::setup()
{
    if (!Camera::init()) {
        MyUI::showErrorMes("Failed to initiialize Camera");
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = Mode::HOME;
        CoreOS::getInstance().post(req);
    }
}

void AppCamera::loop()
{
    CameraFrame photo{};
    Camera::drawPicture(photo);
    if (photo.isValid) {
        M5.Display.pushImage(0, 0, 320, 240, reinterpret_cast<uint16_t*>(photo.data));
        Camera_Touch result{MyUI::checkCameraSelection()};
        switch (result) {
            case Camera_Touch::Right: {
                char fileName[32];
                auto dt = M5.Rtc.getDateTime();
                sprintf(fileName, "/photo_%04d%02d%02d_%02d%02d%02d.jpg", 
                        dt.date.year, 
                        dt.date.month, 
                        dt.date.date, 
                        dt.time.hours, 
                        dt.time.minutes, 
                        dt.time.seconds);

                Camera::capture(photo);            
                if (!System_SD::writeBinary(fileName, photo.data, photo.size)) {
                    MyUI::showErrorMes("Failed to save photo");
                }
                break;
            }
            case Camera_Touch::Left: {
                M5.Display.waitDisplay();

                OSRequest req{};
                req.type = RequestType::APP_INITIALIZE;
                req.state = Mode::HOME;
                CoreOS::getInstance().post(req);
                stop();
                break;
            }
            case Camera_Touch::Invalid: {
                break;
            }
        }
        CoreOS::getInstance().freePSRAM(photo.data);
        photo.data = nullptr;

    } else {
        MyUI::showErrorMes("Failed to get Buffer. Returing to Home...");
        stop();
    }
}

void AppCamera::stop()
{
    if (Camera::fina()) {
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = Mode::HOME;
        CoreOS::getInstance().post(req);
    } else {
        MyUI::showErrorMes("Failed to recover Wire1. Rebooting...");
        CoreOS::getInstance().reboot();
    }
}