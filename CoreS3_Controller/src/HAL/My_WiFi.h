#pragma once
#include <M5Unified.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Common/Secret.h"

class My_WiFi {
    static constexpr const char* SSID{Secrets::SSID};   //入力
    static constexpr const char* PASS{Secrets::PASS}; //入力
    static constexpr const int UdpPort{12345};  //PC用プログラムと揃える
    inline static char IPAdr[16]{};
    inline static WiFiUDP udp{};
public:
    My_WiFi() = delete;
    static void WiFiMonitorTask(void* param);
    static void setup();
    static void UDPConnection();
    static const char* getIPAdr();
};