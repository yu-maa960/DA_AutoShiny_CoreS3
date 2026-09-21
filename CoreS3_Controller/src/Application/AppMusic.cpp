#include <cstring>
#include <SD.h>
#include "AppMusic.h"
#include "Common/Co_Info.h"
#include "Common/SystemTypes.h"
#include "HAL/Music.h"
#include "HAL/MyUI.h"
#include "OS/CoreOS.h"

void AppMusic::setFileName() {
    const char* fileName_from_Co_Info{Co_Info::getMusicFileName()};
    if (fileName_from_Co_Info) {
        strncpy(musicFileName, fileName_from_Co_Info, sizeof(musicFileName) - 1);
        musicFileName[sizeof(musicFileName) - 1] = '\0';
    }
}

void AppMusic::setup()
{
    setFileName();
    Music::init();
    MyUI::drawMusic();
}

bool AppMusic::play()
{
    if (!Music::memory_for_music(musicFileName)) {
        MyUI::showErrorMes("Wave file not found");
        MyUI::showErrorMes("Out of Memory");
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = Mode::SD_CARD;
        CoreOS::getInstance().post(req);
        return false;
    }
    Music::play();
    return true;    
}

void AppMusic::loop()
{
    if (firstLoop) {
        firstLoop = false;
        play();
    }

    Music::update();

    if(MyUI::checkMusicSelection() == Music_Touch::Valid) {
        stop();
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = Mode::SD_CARD;
        CoreOS::getInstance().post(req);
    }
}


void AppMusic::stop() 
{
    Music::fina();
}