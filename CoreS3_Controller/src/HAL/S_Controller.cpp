#include "S_Controller.h"

uint32_t SwitchGamePad::getVID() {
    return VID;
}

uint32_t SwitchGamePad::getPID() {
    return PID;
}

bool SwitchGamePad::init() {
    USB.productName("HORIPAD S");
    USB.manufacturerName("HORI CO.,LTD.");
    
    USB.VID(SwitchGamePad::getVID());
    USB.PID(SwitchGamePad::getPID());

    USBHID::addDevice(this, this->_onGetDescriptor(nullptr)); //USBHIDにHIDであることを初期登録

    myHID.begin();
    USB.begin();

    return true;
}

bool SwitchGamePad::confirm_Connection() {
    if (myHID.ready()) {
        return true;
    } else {
        return false;
    }
}

void SwitchGamePad::fina() {
    releaseAll();
    SwitchDataReport emptyReport{};
    myHID.SendReport(0, &emptyReport, sizeof(emptyReport));
}

uint16_t SwitchGamePad::_onGetDescriptor(uint8_t* dst) {
    if (dst != nullptr) {
        memcpy(dst, switch_report_descriptor, sizeof(switch_report_descriptor));
    }
    return static_cast<uint16_t>(sizeof(switch_report_descriptor));
}

void SwitchGamePad::keyInput(S_Controller_Command key) {
    SwitchAction action = actionDictionary[static_cast<int>(key)];
    SwitchDataReport currentReport{};
    currentReport.lx = action.lx;
    currentReport.ly = action.ly;
    myHID.SendReport(0, &currentReport, sizeof(currentReport));
}

//ボタン処理
void SwitchGamePad::pressButton(S_Controller_Command btn) {
    SwitchAction input = actionDictionary[static_cast<int>(btn)];
    SwitchDataReport report{};
    buttons |= input.btnMask;
    report.btn = buttons;
    myHID.SendReport(0, &report, sizeof(report));
}

void SwitchGamePad::releaseButton(S_Controller_Command btn) {
    SwitchAction off = actionDictionary[static_cast<int>(btn)];
    SwitchDataReport report{};
    buttons &= ~off.btnMask;
    report.btn = buttons;
    myHID.SendReport(0, &report, sizeof(report));
}

void SwitchGamePad::releaseAll() {
    buttons = 0;
}
