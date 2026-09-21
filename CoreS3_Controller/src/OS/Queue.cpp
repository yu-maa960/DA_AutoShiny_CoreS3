#include "Queue.h"

bool OSQueue::init() {
    queue = xQueueCreate(MAX_MESSAGES, sizeof(OSRequest));
    if (queue == nullptr) {
        return false;
    }
    return true;
}

bool OSQueue::sendRequest(const OSRequest& req) {
    if (queue == nullptr) {
        return false;
    }

    if (xQueueSend(queue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
        M5.Display.println("Warning: OS Queue is FULL!");
        return false;
    }

    return true;
}

bool OSQueue::receiveRequest(OSRequest& req) {
    if (queue == nullptr) {
        return false;
    }

    if (xQueueReceive(queue, &req, 0) != pdPASS) {
        return false;
    }
    return true;
}