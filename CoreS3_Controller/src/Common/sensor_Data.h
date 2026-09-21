#pragma once

struct sensor_Data {
    float temp{}; 
    float hum{};
    float pres{};
    bool flag = false;
};