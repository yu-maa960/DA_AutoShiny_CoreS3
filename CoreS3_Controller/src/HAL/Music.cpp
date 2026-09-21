#include <cstring>
#include <M5Unified.h>
#include <SD.h>
#include "Music.h"
#include "OS/CoreOS.h"

void Music::init() {
    M5.Speaker.setVolume(40);
    M5.Speaker.begin();
    delay(10);
}

//前提: ルートディレクトリ直下にサブフォルダが存在しないこと（通常ファイルのみを想定）
bool Music::memory_for_music(const char* musicFileName) {
    File file = SD.open(musicFileName);
    if (!file) {
        return false;
    }

    loopFileSize = file.size();
    loopBuffer = reinterpret_cast<uint8_t*>(CoreOS::getInstance().allocatePSRAM(loopFileSize));
    if (loopBuffer == nullptr) {
        file.close();
        return false;
    }
    file.read(loopBuffer, loopFileSize);
    file.close();
    
    while (*musicFileName == '/') {//   '/'を除きたい
        musicFileName++;
    }
    strncpy(loopName, musicFileName, sizeof(loopName) - 1);
    loopName[sizeof(loopName) - 1] = '\0';

    char introPath[32]{};
    snprintf(introPath, sizeof(introPath), "/_intro_%s", loopName);

    File intro = SD.open(introPath);
    if (intro) {
        exi_intro = true;
    }
    return true;
}

void Music::play() {
    if (exi_intro) {
        char introPath[32]{};
        snprintf(introPath, sizeof(introPath), "/_intro_%s", loopName);

        File intro = SD.open(introPath);
        introFileSize = intro.size();
        introBuffer = reinterpret_cast<uint8_t*>(CoreOS::getInstance().allocatePSRAM(introFileSize));
        if (introBuffer == nullptr) {
            exi_intro = false;
        } else {
            intro.read(introBuffer, introFileSize);
        }
        intro.close();
    }

    if (exi_intro && introBuffer != nullptr) {
        M5.Speaker.playWav(introBuffer, introFileSize, 1, 0, false);
        isPlayingIntro = true;
    } else if (loopBuffer != nullptr) {
        M5.Speaker.playWav(loopBuffer, loopFileSize, ~0u, false);
    }
}

void Music::update() {
    if (isPlayingIntro && !M5.Speaker.isPlaying(0)) {
        isPlayingIntro = false;

        if (loopBuffer != nullptr) {
            M5.Speaker.playWav(loopBuffer, loopFileSize, ~0u, 0, false);
        }
    }
}

void Music::fina() {
    M5.Speaker.stop();
    delay(20); 

    if (introBuffer != nullptr) {
        CoreOS::getInstance().freePSRAM(introBuffer);
        introBuffer = nullptr;
    }

    if (loopBuffer != nullptr) {
        CoreOS::getInstance().freePSRAM(loopBuffer);
        loopBuffer = nullptr;
    }
    exi_intro = false;
}
