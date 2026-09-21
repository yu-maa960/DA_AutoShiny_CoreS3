#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <USB.h>
#include <USBHID.h>
#include "AppController.h"
#include "Common/SystemTypes.h"
#include "Common/S_Command.h"
#include "HAL/My_WiFi.h"
#include "HAL/MyUI.h"
#include "OS/CoreOS.h"

Sword_DA_Command currentBtnMode{Sword_DA_Command::A};
bool isUdpTaskRunning = false;
uint32_t lastTime{};    //UDP受け取った時間

//スレッド．FreeRTOSのタスク関数．
void udpListenerTask(void *pvParameters) {
    while(isUdpTaskRunning) {
        My_WiFi::UDPConnection();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

//currentModeが変わったら即時に終了する関数
bool AppController::waitManager(Sword_DA_Command expectedMode, uint32_t interval) {
    uint32_t start{millis()};
    while (millis() - start < interval) {
        M5.update();
        CoreOS::getInstance().checkUdpInterrupt();
        if (Sword_DA_Command::NONE != expectedMode) {    //NONEは待機するだけの時
            if (currentBtnMode != expectedMode) {
                return false;
            }
        }
        delay(1);
    }
    return true;
}

//interval分待機する関数
bool AppController::waitManager(uint32_t interval) {
    return waitManager(Sword_DA_Command::NONE, interval);
}

void AppController::translateUdpCommand(char c) {
    switch (c) {
        case 'C': {
            static uint32_t lastCTime = 0;
            if (millis() - lastCTime > 2000) {  //一度受け取ったら2秒間は破棄
                currentBtnMode = Sword_DA_Command::CHOOSE;
                lastCTime = millis();
            }
            break;
        }
        case 'F': {
            currentBtnMode = Sword_DA_Command::FINISH;
            break;
        }
        case 'P': {
            currentBtnMode = Sword_DA_Command::PAUSE;
            lastTime = millis();    //更新
            break;
        }
        case 'S': {
            static uint32_t lastCTime = 0;
            if (millis() - lastCTime > 20000) {  //queue圧迫回避
                currentBtnMode = Sword_DA_Command::SHINY;
                lastCTime = millis();
            }
            break;
        }
        default: {
            currentBtnMode = Sword_DA_Command::NONE;
            break;
        }
    }
}


void AppController::setup() {
    MySwitchPad.init();

    uint32_t startWait = millis();
    while (!MySwitchPad.confirm_Connection()) {
        if (millis() - startWait > 1000) {
            MyUI::showErrorMes("Not Connected or Recognized. Rebooting...");
            stop();
        }
        delay(1); // 1ミリ秒ずつ休みながら待つ
    }
    CoreOS::getInstance().setDisplayEcoMode(true);

    MyUI::drawController();

    isUdpTaskRunning = true;
    
    OSRequest req{};
    req.type = RequestType::THREAD_INIT;
    req.message = "UDP_TASK";
    req.taskFunc = udpListenerTask;
    req.size = 4096;
    CoreOS::getInstance().post(req);
}

void AppController::loop() {
    Sword_DA_Command runningMode{currentBtnMode};
    switch(runningMode) {
        case Sword_DA_Command::A: {
            if (!waitManager(runningMode, INTERVAL)) {
                break;
            };
            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager(runningMode);
            break;
        }

        case Sword_DA_Command::CHOOSE: {
            waitManager();

            MySwitchPad.keyInput(S_Controller_Command::DOWN);
            waitManager(S_INTERVAL);
            MySwitchPad.keyInput(S_Controller_Command::RESET);
            waitManager();

            currentBtnMode = Sword_DA_Command::A;
            break;
        }

        case Sword_DA_Command::FINISH: {
            waitManager(UI_WAIT);
            
            MySwitchPad.keyInput(S_Controller_Command::UP);
            waitManager(S_INTERVAL);
            MySwitchPad.keyInput(S_Controller_Command::RESET);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.keyInput(S_Controller_Command::DOWN);
            waitManager(S_INTERVAL);
            MySwitchPad.keyInput(S_Controller_Command::RESET);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);

            waitManager(UI_WAIT);

            for (int i = 0; i < 5; ++i) {   //余分に
                MySwitchPad.keyInput(S_Controller_Command::UP);
                waitManager(S_INTERVAL);
                MySwitchPad.keyInput(S_Controller_Command::RESET);
                if (!waitManager(runningMode)) {    //SHINY(色違い)感知したら即抜け
                    return;
                }
            }

            MySwitchPad.pressButton(S_Controller_Command::B);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::B);
            
            waitManager(UI_WAIT);

            MySwitchPad.pressButton(S_Controller_Command::B);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::B);
            waitManager();

            //以降A4回
            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            waitManager(UI_WAIT);
            currentBtnMode = Sword_DA_Command::A;
            break;
        }
        case Sword_DA_Command::PAUSE: {
            if (millis() - lastTime >= 1000) {
                currentBtnMode = Sword_DA_Command::A;
            }
            break;
        } case Sword_DA_Command::SHINY: {
            waitManager(UI_WAIT);
            MySwitchPad.pressButton(S_Controller_Command::B);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::B);
            waitManager(UI_WAIT);

            //以降7回(+1)
            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            waitManager(UI_WAIT);

            MySwitchPad.pressButton(S_Controller_Command::A);
            waitManager(B_INTERVAL);
            MySwitchPad.releaseButton(S_Controller_Command::A);
            waitManager();

            currentBtnMode = Sword_DA_Command::A;
            break;
        }
        default: {
            break;
        }
    }
    if (MyUI::checkControllerSelection() == Controller_Touch::Valid) {
        stop();
    }
}

void AppController::stop() {
    isUdpTaskRunning = false;
    MySwitchPad.fina();
    CoreOS::getInstance().setDisplayEcoMode(false);
    CoreOS::getInstance().reboot();
}