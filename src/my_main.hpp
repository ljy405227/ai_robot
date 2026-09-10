#pragma once
#include <Arduino.h>
#include "app_globals.hpp"

class App {
public:
    void begin();

private:
    static void RecodeTask(void *param);
    static void AudioTask(void *param);
    static void UITask(void *param);
    static void LVGL_Task(void *param);
    static void AI_Task(void *param);
};