#include <vector>
#include "Common/Co_Info.h"
#include "MyUI.h"
#include "Touch_Input.h"



M5Canvas& MyUI::return_sprite() {
    return sprite;
}

void MyUI::begin()
{
    sprite.setPsram(true);//メモリ圧迫回避のため，外付けRAM(PSRAM)を使わせる．
    sprite.setColorDepth(16);
    bool success{sprite.createSprite(M5.Display.width(), M5.Display.height())};
    
    if (!success) {
        M5.Display.fillScreen(TFT_RED);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(2);
        M5.Display.setCursor(10, 10);
        M5.Display.println("Memory Error!");
        M5.Display.println("Check platformio.ini");
        while(1);//安全なフリーズ
    }
}

void MyUI::showMes(const char* message, int x, int y, int size) {
    //sprite.fillScreen(TFT_BLACK);

    //fillscreenとpushは，呼び出す側でやるに統一
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextWrap(true);
    sprite.setTextSize(size);
    sprite.setCursor(x, y);
    sprite.print(message);
    
    //sprite.pushSprite(&M5.Display, 0, 0);
}

//バッテリー
void MyUI::drawStatusBar()
{
    int batLevel{M5.Power.getBatteryLevel()};
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextSize(2);

    sprite.drawRect(290, 1, 20, 8, TFT_WHITE);
    sprite.fillRect(292, 2, 16, 6, TFT_BLACK);
    sprite.fillRect(292, 2, ((16 * batLevel) / 100), 6, (batLevel > 20) ? TFT_GREEN : TFT_RED);

    sprite.setTextDatum(top_right);
    sprite.drawString(String(batLevel) + "%", 282, 8);
    sprite.setTextDatum(top_left);
}

void MyUI::drawIPAddress(const char* IPAdr) {
    showMes(IPAdr, 10, 1, 1);
    sprite.pushSprite(&M5.Display, 0, 0);
}

void MyUI::showErrorMes(const char* message) {
    sprite.fillScreen(TFT_BLACK);
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextSize(2);
    sprite.setTextWrap(true);
    sprite.setCursor(10, 100);
    sprite.print(message);
    sprite.pushSprite(&M5.Display, 0, 0);
    delay(2000);
}

void MyUI::drawHomeScreen()
{
    sprite.fillScreen(TFT_BLACK);
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextDatum(textdatum_t::middle_center);
    sprite.setTextSize(2);

    //計算: 余白＋(箱の長さ / 2) - フォントの大きさ + 余白
    sprite.drawRect(10, 10, 145, 105, TFT_WHITE);
    sprite.drawString("CAMERA", 82, 62);//中心指定

    // 2段目: SD CARD (Y: 65〜110)
    sprite.drawRect(165, 10, 145, 105, TFT_WHITE);
    sprite.drawString("SD CARD", 237, 62);

    // 3段目: SENSOR (Y: 120〜165)
    sprite.drawRect(10, 125, 145, 105, TFT_WHITE);
    sprite.drawString("SENSOR", 82, 177);

    // 4段目: SERVER (Y: 175〜220)
    sprite.drawRect(165, 125, 145, 105, TFT_WHITE);
    sprite.drawString("Controller", 237, 177);

    drawStatusBar();
    sprite.pushSprite(&M5.Display, 0, 0);
}

void MyUI::drawFileMenu(const int& cursorindex, const std::vector<char>& fileNames, const int filecount)
{
    sprite.fillScreen(BLACK);
    sprite.setCursor(0, 0);
    sprite.setTextSize(2);
    sprite.println("--- SD CARD FILES ---");

    int index_per_char{};
    for (int i = 0; i < filecount; ++i) {
        if (i == cursorindex) {
            sprite.setTextColor(YELLOW, BLUE);
        } else {
            sprite.setTextColor(WHITE, BLACK);
        }
        sprite.printf("[%d] %s\n", (i + 1), &fileNames[index_per_char]);
        index_per_char += strlen(&fileNames[index_per_char]) + 1;
    }
    
    sprite.setTextColor(WHITE, BLACK);
    sprite.pushSprite(&M5.Display, 0, 0);
}

void MyUI::drawmyJPG(File* file, const int x, const int y)
{
    sprite.fillScreen(TFT_BLACK);
    sprite.drawJpg(file, x, y);
    sprite.pushSprite(&M5.Display, 0, 0);
    
}

void MyUI::drawMusic()
{
    sprite.fillScreen(TFT_BLACK);
    showMes("Now Playing...", 20, 10, 2);
    const char* fileName = Co_Info::getMusicFileName();
    fileName++; //'/'を除きたい
    sprite.setTextSize(2); 
    int text_w = sprite.textWidth(fileName); 
    int center_x = (320 - text_w) / 2;  //余白
    showMes(fileName, center_x, 110, 2);
    sprite.pushSprite(&M5.Display, 0, 0);
}

void MyUI::drawSensor(float temp, float hum, float pres, const char* lastTimeString)
{
    sprite.fillScreen(BLACK);
    showMes("ENV LOGGER", 10, 10, 2);

    char buf[32];
    snprintf(buf, sizeof(buf), "Tmp: %.1f C", temp);
    showMes(buf, 10, 50, 3);

    snprintf(buf, sizeof(buf), "Hum: %.1f %%\n", hum);
    showMes(buf, 10, 90, 3);

    snprintf(buf, sizeof(buf), "Prs: %.0f hPa\n", pres);
    showMes(buf, 10, 130, 3);

    sprite.setTextSize(2);
    sprite.setTextColor(GREEN);
    sprite.setCursor(10, 200);
    sprite.println(lastTimeString);

    drawStatusBar();

    sprite.pushSprite(&M5.Display, 0, 0);
}

void MyUI::drawController() {
    sprite.fillScreen(TFT_BLACK);
    showMes("Status: Connected to Switch", 10, 50, 1);
    showMes("Controller", 10, 120, 4);
    sprite.pushSprite(&M5.Display, 0, 0);
}

void MyUI::drawReboot() {
    sprite.fillScreen(TFT_BLACK);
    showMes("Returning to Home...", 10, 50, 3);
    sprite.pushSprite(&M5.Display, 0, 0);
}

Mode MyUI::checkMenuSelection()
{
    TouchData touch{Touch_Input::getTouch()};
    if (!touch.validTap || (touch.x <= 0 || touch.y <= 0)) {
        return Mode::HOME;
    }
    //十字の空白はHOME判定．
    if (touch.validTap) {
        if (touch.x <= 155 && touch.y < 115) {
            return Mode::CAMERA;
        } else if (165 <= touch.x && touch.y < 115) {
            return Mode::SD_CARD;
        } else if (touch.x <= 155 && 125 <= touch.y) {
            return Mode::SENSOR;
        } else if (165 <= touch.x && 125 <= touch.y) {
            return Mode::Controller;
        } else {
            return Mode::HOME;
        }
    }
    return Mode::HOME;
}

Camera_Touch MyUI::checkCameraSelection() {
    TouchData touch{Touch_Input::getTouch()};
    if (!touch.validTap) {
        return Camera_Touch::Invalid;
    }
    return touch.x > 160 ? Camera_Touch::Right : Camera_Touch::Left;
}

SD_Action MyUI::checkFileSelection()
{
    TouchData touch = Touch_Input::getTouch();
    if (touch.validTap) {
        if (touch.x <= 0) {//変な値を除く
            return SD_Action::NONE;
        } else if (touch.x < 70) {
            return SD_Action::GO_BACK; 
        } else if (touch.deltaY < -10) {
            return SD_Action::SCROLL_UP;
        } else if (touch.deltaY > 10) {
            return SD_Action::SCROLL_DOWN;
        } else {
            return SD_Action::OPEN_FILE;
        }
    }
    return SD_Action::NONE;
}

SD_Action MyUI::checkSDTouch() {
    TouchData touch = Touch_Input::getTouch();
    if (touch.validTap) {
        return SD_Action::GO_BACK;
    }
    return SD_Action::NONE;
}

Music_Touch MyUI::checkMusicSelection() {
    TouchData touch = Touch_Input::getTouch();
    if (touch.validTap) {
        return Music_Touch::Valid;
    }
    return Music_Touch::Invalid;
}

Sensor_Touch MyUI::checkSensorSelection() {
    TouchData touch = Touch_Input::getTouch();
    if (touch.validTap) {
        return Sensor_Touch::Valid;
    } else {
        return Sensor_Touch::Invalid;
    }
}

Controller_Touch MyUI::checkControllerSelection() {
    TouchData touch{Touch_Input::getTouch()};
    if (touch.validTap) {
        if (touch.x >= 150 && touch.x <= 165) {
            return Controller_Touch::Invalid;
        }
        return Controller_Touch::Valid;
    } else {
        return Controller_Touch::Invalid;
    }
}