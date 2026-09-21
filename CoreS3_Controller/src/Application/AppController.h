#pragma once
#include "AppBase.h"
#include "HAL/S_Controller.h"

void udpListenerTask(void *pvParameters);

class AppController : public AppBase {
    SwitchGamePad MySwitchPad{};
    static constexpr uint32_t S_INTERVAL = 50; //スティックキーの入力間隔
    static constexpr uint32_t B_INTERVAL = 150; //ボタン入力間隔
    static constexpr uint32_t INTERVAL = 150;   //行動間隔
    static constexpr uint32_t UI_WAIT = 2500;    //画面転換間隔

public:
    AppController() {}
    bool waitManager(Sword_DA_Command expectedMode, uint32_t interval = INTERVAL);
    bool waitManager(uint32_t interval = INTERVAL);
    void translateUdpCommand(char c) override;
    void setup() override;
    void loop() override;
    void stop() override;
};