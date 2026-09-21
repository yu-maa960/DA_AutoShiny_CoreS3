#pragma once

//アプリケーションの状態
enum class Mode{
    NONE,
    HOME,
    CAMERA,
    SD_CARD,
    SENSOR,
    Controller,
    MUSIC,
    MUSIC_NOW,
};

//カメラ
enum class Camera_Touch {
    Invalid,
    Left,
    Right
};

//センサー
enum class Sensor_Touch {
    Invalid,      
    Valid
};

//コントローラー
enum class Controller_Touch {
    Invalid,      
    Valid
};

//SDカード
enum class SD_State {
    LIST,
    VIEWER,
};

//SDカードでのアクション
enum class SD_Action {
    NONE,
    SCROLL_UP,
    SCROLL_DOWN,
    OPEN_FILE,
    GO_BACK,
};

//music
enum class Music_Touch {
    Invalid,
    Valid
};