#include "my_main.hpp"
#include <Arduino.h>
#include "my_sd.hpp"
#include "loudspeaker.hpp"
#include "my_recode.hpp"
#include "my_wifi.hpp"
#include "lvgl.h"
#include "examples/porting/lv_port_disp_template.hpp"
#include "examples/porting/lv_port_indev_template.hpp"
#include "my_malloc.h"
#include "lvgl_ui/gui_guider.h"
#include "lvgl_ui/custom.hpp"
#include "lvgl_ui/emotion.hpp"
#include "helpers.hpp"
#include "command_bus.hpp"
#include "app_globals.hpp"

void App::begin()
{
    Serial.begin(115200);
    // Serial1.begin(115200, SERIAL_8N1, 10, 9);
    Serial2.begin(115200, SERIAL_8N1, 14, 46);
    delay(100);
    ledcSetup(0, 17000, 8);
    // 将引脚绑定到PWM通道
    ledcAttachPin(3, 0);

    randomSeed(millis());
    sd_init();
    loudspeaker_init();
    mp3_init();
    delay(5000);

    // lvglTicker.attach_ms(50, lvgl_timer_callback);

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    setup_ui(&guider_ui);
    custom_init(&guider_ui);

    switch_emotion_img(&guider_ui, 5);

    // 创建串口互斥锁
    SerialMutex = xSemaphoreCreateMutex();

    // 调整任务优先级（关键：降低AudioTask优先级，让UI有机会执行）
    // 优先级：LVGL(12) > UI(10) > Audio(8) > AI(3) > Recode(2)
    xTaskCreatePinnedToCore(AudioTask, "AudioTask", AUDIO_TASK_STACK, NULL, 13, &audioTaskHandle, 1);
    xTaskCreatePinnedToCore(UITask, "UITask", UI_TASK_STACK, NULL, 10, &uiTaskHandle, 1);
    xTaskCreatePinnedToCore(LVGL_Task, "LVGL_Task", LVGL_TASK_STACK, NULL, 9, &lvglTaskHandle, 0);
    xTaskCreatePinnedToCore(AI_Task, "AI_Task", AI_TASK_STACK, NULL, 3, &aiTaskHandle, 0);

    print_task_stack_info();
}

void App::RecodeTask(void *param) {
  while (1) {
    if (cmdRecode_start)
    {
      // 录音逻辑
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void App::LVGL_Task(void *param)
{
  while (1) {
    lv_timer_handler();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}