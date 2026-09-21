#pragma once
#include "AppBase.h"

class AppCamera : public AppBase {
public :
    AppCamera() {};
    void setup() override;
    void loop() override;
    void stop() override;
};