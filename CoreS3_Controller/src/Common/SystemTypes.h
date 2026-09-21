#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include "App_State.h"

enum class RequestType : uint8_t {
    NONE = 0,
    APP_INITIALIZE,
    THREAD_INIT,
    WIFI_CONNECTED,
    UDP_COMMAND,
};

struct OSRequest {  //初期化は必ず
    RequestType type{RequestType::NONE};    //必須
    Mode state{Mode::NONE}; //App用
    const char* message = nullptr;  //UDP通信でのPCからの受信
    void* bufferPtr = nullptr;  //メモリ用
    int size = 0;
    TaskFunction_t taskFunc = nullptr;  //スレッド用
    char command{};
};