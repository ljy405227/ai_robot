#include "helpers.hpp"
#include "app_globals.hpp"
#include "command_bus.hpp"
#include "my_sd.hpp"
#include "loudspeaker.hpp"

void print_task_stack_info() {
  xSemaphoreTake(SerialMutex, portMAX_DELAY);

  Serial.println("\n===== 任务栈使用情况 ======");

  if (audioTaskHandle != NULL) {
    UBaseType_t audio_free = uxTaskGetStackHighWaterMark(audioTaskHandle);
    Serial.printf("AudioTask | 总栈：%d字(%dKB) | 剩余：%d字(%dKB) | 已用：%d字(%dKB)\n",
                  AUDIO_TASK_STACK, AUDIO_TASK_STACK*4/1024,
                  audio_free, audio_free*4/1024,
                  AUDIO_TASK_STACK - audio_free, (AUDIO_TASK_STACK - audio_free)*4/1024);
  } else {
    Serial.println("AudioTask | 任务未创建");
  }

  if (uiTaskHandle != NULL) {
    UBaseType_t ui_free = uxTaskGetStackHighWaterMark(uiTaskHandle);
    Serial.printf("UITask    | 总栈：%d字(%dKB)  | 剩余：%d字(%dKB) | 已用：%d字(%dKB)\n",
                  UI_TASK_STACK, UI_TASK_STACK*4/1024,
                  ui_free, ui_free*4/1024,
                  UI_TASK_STACK - ui_free, (UI_TASK_STACK - ui_free)*4/1024);
  } else {
    Serial.println("UITask    | 任务未创建");
  }

  if (lvglTaskHandle != NULL) {
    UBaseType_t lvgl_free = uxTaskGetStackHighWaterMark(lvglTaskHandle);
    Serial.printf("LVGL_Task | 总栈：%d字(%dKB) | 剩余：%d字(%dKB) | 已用：%d字(%dKB)\n",
                  LVGL_TASK_STACK, LVGL_TASK_STACK*4/1024,
                  lvgl_free, lvgl_free*4/1024,
                  LVGL_TASK_STACK - lvgl_free, (LVGL_TASK_STACK - lvgl_free)*4/1024);
  } else {
    Serial.println("LVGL_Task | 任务未创建");
  }

  if (recodeTaskHandle != NULL) {
    UBaseType_t recode_free = uxTaskGetStackHighWaterMark(recodeTaskHandle);
    Serial.printf("RecodeTask| 总栈：%d字(%dKB) | 剩余：%d字(%dKB) | 已用：%d字(%dKB)\n",
                  RECODE_TASK_STACK, RECODE_TASK_STACK*4/1024,
                  recode_free, recode_free*4/1024,
                  RECODE_TASK_STACK - recode_free, (RECODE_TASK_STACK - recode_free)*4/1024);
  } else {
    Serial.println("RecodeTask| 任务未创建（注释）");
  }

  if (aiTaskHandle != NULL) {
    UBaseType_t ai_free = uxTaskGetStackHighWaterMark(aiTaskHandle);
    Serial.printf("AI_Task   | 总栈：%d字(%dKB) | 剩余：%d字(%dKB) | 已用：%d字(%dKB)\n",
                  AI_TASK_STACK, AI_TASK_STACK*4/1024,
                  ai_free, ai_free*4/1024,
                  AI_TASK_STACK - ai_free, (AI_TASK_STACK - ai_free)*4/1024);
  } else {
    Serial.println("AI_Task   | 任务未创建");
  }

  Serial.println("===========================\n");

  xSemaphoreGive(SerialMutex);
}


bool waitAudioLoadComplete() {
  unsigned long startTime = millis();
  isAudioLoading = true;

  while (millis() - startTime < AUDIO_LOAD_TIMEOUT) {
    if (audio.getAudioFileDuration() > 0) {
      isAudioLoading = false;
      xSemaphoreTake(SerialMutex, portMAX_DELAY);
      Serial.println("✅ 音频加载完成");
      xSemaphoreGive(SerialMutex);
      return true;
    }
    audio.loop();
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  isAudioLoading = false;
  xSemaphoreTake(SerialMutex, portMAX_DELAY);
  Serial.println("❌ 音频加载超时");
  xSemaphoreGive(SerialMutex);
  return false;
}

void initFirstPlay() {
  // 如果为空
  if (playlist.empty()) {
    xSemaphoreTake(SerialMutex, portMAX_DELAY);
    Serial.println("⚠️ 播放列表为空，跳过首次播放初始化");
    xSemaphoreGive(SerialMutex);
    isFirstPlayInited = true;
    return;
  }

  currentIndex = 0;

  xSemaphoreTake(SerialMutex, portMAX_DELAY);
  Serial.printf("▶ 开机首次播放: %s\n", playlist[currentIndex].c_str());
  xSemaphoreGive(SerialMutex);

  playCurrentSong();

  isFirstPlayInited = true;
}