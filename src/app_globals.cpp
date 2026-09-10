#include "app_globals.hpp"

// ===== LVGL UI 结构体 =====
lv_ui guider_ui;

// ===== 应用状态标志 =====
uint8_t       zhuyili_flag       = 0;
unsigned long lastTimePrint      = 0;
bool          isAudioPlaying     = false;
bool          isAudioLoading     = false;
bool          isFirstPlayInited  = false;

// ===== FreeRTOS 任务句柄 =====
TaskHandle_t recodeTaskHandle = NULL;
TaskHandle_t audioTaskHandle  = NULL;
TaskHandle_t uiTaskHandle     = NULL;
TaskHandle_t lvglTaskHandle   = NULL;
TaskHandle_t aiTaskHandle     = NULL;

// ===== 串口互斥锁 =====
SemaphoreHandle_t SerialMutex  = NULL;

// ===== 音乐模式 =====
uint8_t music_mode = 0;