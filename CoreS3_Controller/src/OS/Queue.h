#pragma once
#include <M5Unified.h>
#include <freertos/FreeRTOS.h>
#include <FreeRTOS/queue.h>
#include "Common/SystemTypes.h"

class OSQueue {
private:
    QueueHandle_t queue;
    static const uint8_t MAX_MESSAGES = 100;

public:
    OSQueue() : queue(nullptr) {}

    bool init();
    bool sendRequest(const OSRequest& req);
    bool receiveRequest(OSRequest& req);
};