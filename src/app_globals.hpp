#ifndef APP_GLOBALS_HPP
#define APP_GLOBALS_HPP

#include <Arduino.h>
#include "lvgl_ui/gui_guider.h"

// ===== LVGL UI 结构体 =====
extern lv_ui guider_ui;

// ===== 应用状态标志 =====
extern uint8_t       zhuyili_flag;            // 注意力（LED 呼吸）标志
extern unsigned long lastTimePrint;           // 上次打印播放时间
extern bool          isAudioPlaying;          // 是否正在播放音频
extern bool          isAudioLoading;          // 是否正在加载音频
extern bool          isFirstPlayInited;       // 首次播放是否已初始化
#define AUDIO_LOAD_TIMEOUT 5000               // 音频加载超时（ms）

// ===== FreeRTOS 任务句柄 =====
extern TaskHandle_t recodeTaskHandle;
extern TaskHandle_t audioTaskHandle;
extern TaskHandle_t uiTaskHandle;
extern TaskHandle_t lvglTaskHandle;
extern TaskHandle_t aiTaskHandle;

// ===== 任务栈大小 =====
#define AUDIO_TASK_STACK   8192
#define UI_TASK_STACK      2048
#define LVGL_TASK_STACK    4096
#define RECODE_TASK_STACK  3072
#define AI_TASK_STACK      2048

// ===== 串口互斥锁 =====
extern SemaphoreHandle_t SerialMutex;

// ===== 音乐模式 =====
extern uint8_t music_mode;

#endif // APP_GLOBALS_HPP