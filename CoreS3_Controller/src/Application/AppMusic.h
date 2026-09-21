#pragma once

#include "AppBase.h"
#include "Common/App_State.h"


class AppMusic : public AppBase {
private :
    bool exi_intro{false};
    bool firstLoop{true};
    char musicFileName[32]{};
public :
    AppMusic() {};
    void setFileName();
    bool play();
    void setup() override;
    void loop() override;
    void stop() override;
};