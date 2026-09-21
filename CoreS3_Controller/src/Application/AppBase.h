#pragma once

class AppBase {
public :
    virtual ~AppBase() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void stop() = 0;

    virtual void translateUdpCommand(char c) {}
};