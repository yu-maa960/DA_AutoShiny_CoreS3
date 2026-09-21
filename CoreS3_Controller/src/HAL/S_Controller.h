#pragma once
#include <USB.h>
#include <USBHID.h>
#include "Common/S_Command.h"

struct SwitchAction {
    uint16_t btnMask{};
    uint8_t lx{};
    uint8_t ly{};
};

enum SwitchButton : uint16_t {
    BTN_Y       = 1 << 0,  // 0000 0000 0000 0001
    BTN_B       = 1 << 1,  // 0000 0000 0000 0010
    BTN_A       = 1 << 2,  // 0000 0000 0000 0100
    BTN_X       = 1 << 3,  // 0000 0000 0000 1000
    BTN_L       = 1 << 4,  // 0000 0000 0001 0000
    BTN_R       = 1 << 5,  // 0000 0000 0010 0000
    BTN_ZL      = 1 << 6,  // 0000 0000 0100 0000
    BTN_ZR      = 1 << 7,  // 0000 0000 1000 0000
    BTN_MINUS   = 1 << 8,  // 0000 0001 0000 0000
    BTN_PLUS    = 1 << 9,  // 0000 0010 0000 0000
    BTN_LCLICK  = 1 << 10, // 0000 0100 0000 0000
    BTN_RCLICK  = 1 << 11, // 0000 1000 0000 0000
    BTN_HOME    = 1 << 12, // 0001 0000 0000 0000
    BTN_CAPTURE = 1 << 13  // 0010 0000 0000 0000
};

class SwitchGamePad : public USBHIDDevice {
    uint16_t buttons{};
    static constexpr uint32_t VID{0x0f0d};
    static constexpr uint32_t PID{0x0092};
    inline static USBHID myHID{};
    static constexpr uint8_t switch_report_descriptor[]{
        //ゲームパッド宣言
        0x05, 0x01,        
        0x09, 0x05,       
        0xA1, 0x01,       
        0x85, 0x01,        // Report ID (1)
        //ボタン関連
        0x15, 0x00,        //   Logical Minimum (0)
        0x25, 0x01,        //   Logical Maximum (1)
        0x35, 0x00,        //   Physical Minimum (0)
        0x45, 0x01,        //   Physical Maximum (1)
        0x75, 0x01,        //   Report Size (1)
        0x95, 0x10,        //   Report Count (16)
        0x05, 0x09,        //   Usage Page (Button)
        0x19, 0x01,        //   Usage Minimum (0x01)
        0x29, 0x10,        //   Usage Maximum (0x10)
        0x81, 0x02,        //   Input
        //十字キー
        0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
        0x09, 0x39,        //   Usage (Hat switch)
        0x15, 0x00,        //   Logical Minimum (0)
        0x25, 0x07,        //   Logical Maximum (7)
        0x35, 0x00,        //   Physical Minimum (0)
        0x46, 0x3B, 0x01,  //   Physical Maximum (315)
        0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
        0x75, 0x04,        //   Report Size (4)
        0x95, 0x01,        //   Report Count (1)
        0x81, 0x02,        //   Input
        //十字キーをパッディング
        0x95, 0x01,        //   Report Count (1)
        0x75, 0x04,        //   Report Size (4) -> 空白の4ビット
        0x81, 0x03,        //   Input
        //アナログスティック
        0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
        0x09, 0x30,        // Usage (X) -> 左スティック左右
        0x09, 0x31,        // Usage (Y) -> 左スティック上下
        0x09, 0x32,        // Usage (Z) -> 右スティック左右
        0x09, 0x35,        // Usage (Rx) -> 右スティック上下
        0x15, 0x00,        //   Logical Minimum (0)
        0x26, 0xFF, 0x00,  //   Logical Maximum (255)
        0x75, 0x08,        //   Report Size (8)
        0x95, 0x04,        //   Report Count (4)
        0x81, 0x02,        //   Input

        0xC0               // End Collection（設計図の終わり)
    };

    // S_CommandファイルのS_Controller_Commandの並び順と一致させる．
    static constexpr SwitchAction actionDictionary[] = {
        {0, 128, 0},     // 0: UP    (lyを0にする)
        {0, 128, 255},   // 1: DOWN  (lyを255にする)
        {0, 255, 128},   // 2: RIGHT (lxを255にする)
        {0, 0, 128},     // 3: LEFT  (lxを0にする)
        {0, 128, 128},   // 十字キーリセット
        {SwitchButton::BTN_A, 128, 128}, // 4: A   (BTN_Aのビットを立てる)
        {SwitchButton::BTN_B, 128, 128}, // 5: B
        {SwitchButton::BTN_X, 128, 128}, // 6: X
        {SwitchButton::BTN_Y, 128, 128}  // 7: Y
    };
    
public: 
    SwitchGamePad() {}
    bool init();
    bool confirm_Connection();
    void fina();
    uint16_t _onGetDescriptor(uint8_t* dst) override;
    static uint32_t getVID();
    static uint32_t getPID();
    void keyInput(S_Controller_Command key);
    void pressButton(S_Controller_Command btn);
    void releaseButton(S_Controller_Command btn);
    void releaseAll();
};

struct __attribute__((packed)) SwitchDataReport {
    uint16_t btn{0};      // 送るボタン16個（2バイト）
    uint8_t  hat{8};          // 十字キー（1バイト：初期値8＝中心）
    uint8_t  lx{128};           // 左スティックX（1バイト：初期値128＝中心）
    uint8_t  ly{128};           // 左スティックY（1バイト：初期値128＝中心）
    uint8_t  rx{128};           // 右スティックX（1バイト：初期値128＝中心）
    uint8_t  ry{128};           // 右スティックY（1バイト：初期値128＝中心）
    uint8_t  padding{0};
};

