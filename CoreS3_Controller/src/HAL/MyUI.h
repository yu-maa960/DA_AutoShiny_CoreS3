#pragma once

#include <M5Unified.h>
#include <SD.h>
#include <vector>
#include "Common/App_State.h"

class MyUI {
private :
    inline static M5Canvas sprite{};
public :
    MyUI() = delete;    
    static void begin(); 
    static M5Canvas& return_sprite();

    static void showMes(const char* message, int x, int y, int size);
    static void drawStatusBar();
    static void drawIPAddress(const char* IPAdr);

    static void showErrorMes(const char* message);
    static void drawHomeScreen();
    static void drawFileMenu(const int& cursorindex, const std::vector<char>& fileNames, const int filecount);
    static void drawmyJPG(File* file, const int x, const int y);
    static void drawMusic();
    static void drawSensor(float temp, float hum, float pres, const char* lastTimeString);
    static void drawController();
    static void drawReboot();

    static Mode checkMenuSelection();
    static Camera_Touch checkCameraSelection();
    static SD_Action checkFileSelection();
    static SD_Action checkSDTouch();
    static Music_Touch checkMusicSelection();
    static Sensor_Touch checkSensorSelection();
    static Controller_Touch checkControllerSelection();
};