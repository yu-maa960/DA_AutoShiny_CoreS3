#include <SD.h>
#include "AppSD.h"
#include "Common/Co_Info.h"
#include "Common/SystemTypes.h"
#include "HAL/MySD.h"
#include "HAL/MyUI.h"
#include "OS/CoreOS.h"


void AppSD::setup()
{
    fileNames.reserve(1600);//32文字*50個分を確保．
    if (!MySD::init(fileCount, fileNames)) {
        MyUI::showErrorMes("Failed to access SD card");
        init_success = false;
        OSRequest req{};
        req.type = RequestType::APP_INITIALIZE;
        req.state = Mode::HOME;
        CoreOS::getInstance().post(req);
        return;
    }
    MyUI::drawFileMenu(cursorIndex, fileNames, fileCount);
}

void AppSD::loop()
{
    if (!init_success) {
        return;
    }
    switch (SDState) {
        case SD_State::LIST: {
            if (fileNames.empty()) {
                MyUI::showErrorMes("File not found");
            }
            SDAction = MyUI::checkFileSelection();
            switch (SDAction) {
                case SD_Action::NONE: {
                    break;
                }
                case SD_Action::GO_BACK: {
                    stop();
                    cursorIndex = 0;//上に戻す
                    OSRequest req{};
                    req.type = RequestType::APP_INITIALIZE;
                    req.state = Mode::HOME;
                    CoreOS::getInstance().post(req);
                    return;
                }
                case SD_Action::SCROLL_UP: { 
                    if (cursorIndex == 0) {
                        cursorIndex = fileCount - 1; 
                    } else {
                        cursorIndex--;
                    }
                    MyUI::drawFileMenu(cursorIndex, fileNames, fileCount);
                    break;
                }
                case SD_Action::SCROLL_DOWN: {
                    cursorIndex++;
                    if (cursorIndex >= fileCount) {
                        cursorIndex = 0;
                    }
                    MyUI::drawFileMenu(cursorIndex, fileNames, fileCount);
                    break;
                }
                case SD_Action::OPEN_FILE: {
                    //coursorIndexからファイルの先頭ポインタがほしい．
                    const char* selectedFile = MySD::extract_FileAdress(fileNames, cursorIndex);//開きたいファイルの先頭アドレスを返す
                    FileType ext = MySD::get_FileType(selectedFile);//拡張子を取り出す
                    if (ext == FileType::UNKNOWN) {
                        MyUI::showErrorMes("Undefined File type");
                        SDState = SD_State::LIST;
                        break;
                    }
                    snprintf(fileName, sizeof(fileName), "%s%s", "/", selectedFile);//'/'を付けて，openできるようにする．
                    switch (ext) {
                        case FileType::JPG: {
                            SDState = SD_State::VIEWER;
                            break;
                        }
                        case FileType::WAV: {
                            Co_Info::setMusicFile(fileName);
                            OSRequest req{};
                            req.type = RequestType::APP_INITIALIZE;
                            req.state = Mode::MUSIC;
                            CoreOS::getInstance().post(req);
                            break;
                        }
                        //拡張子ごとの動作を足す
                    }
                }
            }
            
            break;
        }

        case SD_State::VIEWER: {
            if (!isImageDrawn) {
                File imageFile{MySD::getFile(fileName)};
                if (!imageFile) {
                    MyUI::showErrorMes("Image not found!");
                    SDState = SD_State::LIST;
                    return;
                }
                MyUI::drawmyJPG(&imageFile, 0, 0);
                imageFile.close();
                isImageDrawn = true;
            }
            
            if (SD_Action::GO_BACK == MyUI::checkSDTouch()) {
                SDState = SD_State::LIST;
                isImageDrawn = false;
                MyUI::drawFileMenu(cursorIndex, fileNames, fileCount);
            }
            break;
        }
    }
}           
                    
    

void AppSD::stop()
{
    
}