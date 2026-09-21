#pragma once
#include <M5Unified.h>
#include "Common/App_State.h"
#include "Application/AppBase.h"
#include "Common/SystemTypes.h"
#include "Queue.h"

class CoreOS {
private:
    OSQueue queue;
    OSQueue udpQueue;
    AppBase* currentApp = nullptr;

    CoreOS() {}
    ~CoreOS() {}

public:
    static CoreOS& getInstance();

    static bool post(const OSRequest& req);
    static bool udpPost(const OSRequest& req);

    bool boot();
    void run();
    void reboot();

    void launchApp(Mode App);
    void checkUdpInterrupt();
    void setDisplayEcoMode(bool enable);
    void setCpuEcoMode(bool enable);

    //動的メモリ確保，Queue介さずすぐに確保
    void* allocatePSRAM(size_t size);

    template <typename T>
    void freePSRAM(T*& ptr);
};

#include "CoreOS.inl"