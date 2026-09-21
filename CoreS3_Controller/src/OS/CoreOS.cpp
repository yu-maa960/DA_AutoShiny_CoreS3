#include <esp_heap_caps.h>
#include <SD.h>
#include <USB.h>
#include "AppFactory.h"
#include "CoreOS.h"
#include "HAL/My_WiFi.h"
#include "HAL/MyUI.h"

CoreOS& CoreOS::getInstance() {
    static CoreOS instance; //一回だけインスタンスを生成する．以降はインスタンスを返すだけの関数
    return instance;
}

//各Appがqueueを知らなくてもリクエストを送れるようにする
bool CoreOS::post(const OSRequest& req) {
    return getInstance().queue.sendRequest(req);
}

bool CoreOS::udpPost(const OSRequest& req) {
    return getInstance().udpQueue.sendRequest(req);
}

//起動
bool CoreOS::boot() {
    auto cfg = M5.config();
    M5.begin(cfg);

    MyUI::begin();

    if (!queue.init()) {
        return false;
    }

    if (!udpQueue.init()) {
        return false;
    }

    //デフォルトだとカメラのピンと衝突するので，ここで必ず指定
    SPI.begin(36, 35, 37, 4);
    if (!SD.begin(GPIO_NUM_4, SPI, 25000000)) {
        MyUI::showErrorMes("SD Failed");
    }

    My_WiFi::setup();
    launchApp(Mode::HOME);
    return true;
}

//常に監視し，アプリを動かす
void CoreOS::run() {
    M5.update();

    OSRequest req{};
    if (queue.receiveRequest(req)) {
        switch(req.type) {
            case RequestType::APP_INITIALIZE: {
                launchApp(req.state);
                break;
            }
            case RequestType::THREAD_INIT: {
                xTaskCreatePinnedToCore(req.taskFunc, req.message, req.size, NULL, 1, NULL, 0);
                break;
            }
            case RequestType::WIFI_CONNECTED: {
                break;
            }
            //ここにアプリケーション変化を作る．
            default: {
                break;
            }
        }
    }

    if (udpQueue.receiveRequest(req)) {
        if (req.type == RequestType::UDP_COMMAND) {
            if (currentApp != nullptr) {
                currentApp->translateUdpCommand(req.command);
            }
        }
    }

    if (currentApp != nullptr) {
        currentApp->loop();
    }
}

void CoreOS::reboot() {
    MyUI::drawReboot();
    delay(2000);
    ESP.restart();
}

//アプリ終了⇒生成，メモリを確保するのでOSで行う．
void CoreOS::launchApp(Mode App) {
    if (currentApp != nullptr) {
        currentApp->stop();
        delete currentApp;
        currentApp = nullptr;
    }

    currentApp = AppFactory::create(App);

    if (currentApp != nullptr) {
        currentApp->setup();
    } else {
        MyUI::showErrorMes("Memory Error");
        currentApp = AppFactory::create(Mode::HOME);
        currentApp->setup();
    }
}

//switchコントローラー用の待機中にudp用queue確認関数
void CoreOS::checkUdpInterrupt() {
    OSRequest req{};
    M5.update();
    int processCount = 0;
    while (udpQueue.receiveRequest(req) && processCount < 5) {
        if (req.type == RequestType::UDP_COMMAND) {
            if (currentApp != nullptr) {
                currentApp->translateUdpCommand(req.command);
            }
        }
        processCount++;
    }
}

void CoreOS::setDisplayEcoMode(bool enable) {
    if (enable) {             
        M5.Display.setBrightness(50);        
    } else {           
        M5.Display.setBrightness(200);        
    }
}

void CoreOS::setCpuEcoMode(bool enable) {
    if (enable) {
        setCpuFrequencyMhz(80);                    
    } else {
        setCpuFrequencyMhz(240);                    
    }
}

//メモリを確保する依頼処理．すぐに確保させるためQueueを通さない
void* CoreOS::allocatePSRAM(size_t size) {
    void* ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);

    if (ptr == nullptr) {
        MyUI::showErrorMes("Lack of Memory");
    }
    
    return ptr;
}

