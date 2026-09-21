#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Common/SystemTypes.h"
#include "My_WiFi.h"
#include "OS/CoreOS.h"

void My_WiFi::WiFiMonitorTask(void* param) {
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    IPAddress ip{WiFi.localIP()};

    char tempBuf[16]{};
    snprintf(tempBuf, sizeof(tempBuf), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    strncpy(IPAdr, tempBuf, sizeof(IPAdr));
    udp.begin(UdpPort);

    OSRequest req{};
    req.type = RequestType::WIFI_CONNECTED;
    CoreOS::getInstance().post(req);

    vTaskDelete(NULL);
}

void My_WiFi::setup() {
    WiFi.begin(SSID, PASS);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    WiFi.setSleep(false);   //PCからのコマンドを即時に受け取る

    OSRequest req{};
    req.type = RequestType::THREAD_INIT;
    req.message = "WiFi_Con";
    req.taskFunc = WiFiMonitorTask;
    req.size = 4096;
    CoreOS::getInstance().post(req);
}

void My_WiFi::UDPConnection() {
    int packetSize{udp.parsePacket()};
    if (packetSize) {   //-1をカット
        char receive{static_cast<char>(udp.read())};
        OSRequest req{};
        req.type = RequestType::UDP_COMMAND;
        req.command = receive;
        CoreOS::getInstance().udpPost(req);
    }
}

const char* My_WiFi::getIPAdr() {
    return IPAdr;
}