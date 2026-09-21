#include "AppHome.h"
#include "Common/App_State.h"
#include "Common/SystemTypes.h"
#include "HAL/MyUI.h"
#include "HAL/My_WiFi.h"
#include "OS/CoreOS.h"

void AppHome::setup() {
    MyUI::drawHomeScreen();
    const char* currentIP = My_WiFi::getIPAdr();
    
    if (currentIP != nullptr && currentIP[0] >= '0' && currentIP[0] <= '9') {
        IPAdr = currentIP;
        MyUI::drawIPAddress(IPAdr);
        IPDrawn = true;
    }
}

void AppHome::loop() {
    if (!IPDrawn) {
        const char* currentIP = My_WiFi::getIPAdr();
    
        if (currentIP != nullptr && currentIP[0] >= '0' && currentIP[0] <= '9') {
            IPAdr = currentIP;
            MyUI::drawIPAddress(IPAdr);
            IPDrawn = true;
        }
    }

    Mode nextMode{MyUI::checkMenuSelection()};
    if (nextMode != Mode::HOME) {
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = nextMode;
        CoreOS::getInstance().post(req);
    }
}

void AppHome::stop() {
    
}

