#pragma once

#include <vector>
#include "AppBase.h"
#include "Common/App_State.h"


class AppSD : public AppBase {
private :
    inline static int cursorIndex{0};//UXのために保持
    int fileCount{0};
    bool init_success = true;
    bool isImageDrawn{false};
    char fileName[32]{};
    std::vector<char> fileNames{};
    SD_State SDState{SD_State::LIST};
    SD_Action SDAction = SD_Action::NONE;
public :
    AppSD() {};
    void setup() override;
    void loop() override;
    void stop() override;
};