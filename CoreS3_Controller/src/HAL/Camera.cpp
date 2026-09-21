#include <M5Unified.h>
#include "Camera.h"
#include "OS/CoreOS.h"

bool Camera::init() {
    M5.Power.setExtOutput(true);

    config.pin_d0       = D0;
    config.pin_d1       = D1;
    config.pin_d2       = D2;
    config.pin_d3       = D3;
    config.pin_d4       = D4;
    config.pin_d5       = D5;
    config.pin_d6       = D6;
    config.pin_d7       = D7;
    config.pin_xclk     = XCLK;
    config.pin_pclk     = PCLK;
    config.pin_vsync    = VSYNC;
    config.pin_href     = HREF;
    config.pin_sccb_sda = SIOD;
    config.pin_sccb_scl = SIOC;
    config.pin_pwdn     = PWDN;
    config.pin_reset    = RESET;
    config.xclk_freq_hz = 20000000;

    config.sccb_i2c_port = 1;//Wire1を使う
    config.pixel_format = PIXFORMAT_RGB565; // ディスプレイと同じ形式で色を取得
    config.frame_size   = FRAMESIZE_QVGA;   // 320x240
    config.jpeg_quality = 12;
    config.fb_count     = 2;                // ダブルバッファ
    config.fb_location  = CAMERA_FB_IN_PSRAM;
    config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;

    Wire1.begin(12, 11, 400000);

    esp_err_t err = esp_camera_init(&config);
    if (err == ESP_OK) {
        return true;
    } else {
        fina();
        return false;
    }
}

void Camera::drawPicture(CameraFrame& outFrame) {
    camera_fb_t* fb{esp_camera_fb_get()};
    if (!fb) {
        return;
    }
    outFrame.data = reinterpret_cast<uint8_t*>(CoreOS::getInstance().allocatePSRAM(fb->len));
    if (outFrame.data != nullptr) {
        memcpy(outFrame.data, fb->buf, fb->len);
        outFrame.size = fb->len;
        outFrame.isValid = true;
    }
    esp_camera_fb_return(fb);
    fb = nullptr;
}

void Camera::capture(CameraFrame& outFrame) {
    uint8_t* jpeg_buf{};
    size_t jpeg_len{};

    camera_fb_t fake_fb{};
    fake_fb.buf = outFrame.data;
    fake_fb.len = outFrame.size;
    fake_fb.width = 320;
    fake_fb.height = 240;
    fake_fb.format = PIXFORMAT_RGB565;

    bool converted{frame2jpg(&fake_fb, 80, &jpeg_buf, &jpeg_len)};

    if (converted) {
        uint8_t* final_jpeg_buf = reinterpret_cast<uint8_t*>(CoreOS::getInstance().allocatePSRAM(jpeg_len));
        
        if (final_jpeg_buf != nullptr) {
            memcpy(final_jpeg_buf, jpeg_buf, jpeg_len);

            CoreOS::getInstance().freePSRAM(outFrame.data);
            outFrame.data = final_jpeg_buf;
            outFrame.size = jpeg_len;
        }
        free(jpeg_buf);
        jpeg_buf = nullptr;
    }
}

bool Camera::fina() {
    esp_camera_deinit();
    M5.Power.setExtOutput(false);
    delay(100); 

    if (Wire1.begin(12, 11, 400000)) {//通信路戻す
        return true;
    } else {
        return false;
    }
}