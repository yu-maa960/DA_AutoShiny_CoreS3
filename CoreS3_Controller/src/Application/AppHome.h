#pragma once
#include "AppBase.h"

class AppHome : public AppBase {
    const char* IPAdr{};
    bool IPDrawn = false;
public:
    AppHome(){};
    void setup() override;
    void loop() override;
    void stop() override;
};