#include "AppFactory.h"
#include "Application/AppCamera.h"
#include "Application/AppController.h"
#include "Application/AppHome.h"
#include "Application/AppMusic.h"
#include "Application/AppSD.h"
#include "Application/AppSensor.h"

AppBase* AppFactory::create(Mode mode) {
    switch (mode) {
        case Mode::HOME: {
            return new AppHome();
        }
        case Mode::SENSOR: {
            return new AppSensor();
        }
        case Mode::CAMERA: {
            return new AppCamera();
        }
        case Mode::SD_CARD: {
            return new AppSD();
        }
        case Mode::MUSIC: {
            return new AppMusic();
        }
        case Mode::Controller: {
            return new AppController();
        }
    }
    return nullptr;
}