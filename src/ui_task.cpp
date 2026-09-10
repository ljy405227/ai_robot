#include "my_main.hpp"
#include "command_bus.hpp"
#include "app_globals.hpp"
#include "lvgl_ui/emotion.hpp"

// ===== 串口 UI 任务 =====
void App::UITask(void *param) {
  while (1) {
    if (Serial.available())
    {
      xSemaphoreTake(SerialMutex, portMAX_DELAY);
      char cmd = Serial.read();
      xSemaphoreGive(SerialMutex);
      switch (cmd)
      {
        case 'A': cmdNext = true; break;          // 下一曲
        case 'a': cmdPrev = true; break;          // 上一曲（小写a）
        case 'B': cmdPauseToggle = true; break;   // 暂停/播放切换（保留）
        case 'b': cmdPause = true; break;         // 暂停（小写b）
        case 'P': cmdPlay = true; break;          // 播放（大写P）
        case 'C': cmdForward = true; break;       // 快进
        case 'D': cmdBackward = true; break;      // 后退
        case 'E': cmdToggleRandom = true; break;  // 随机/顺序切换（保留）
        case 'e': cmdSetRandom = true; break;     // 设置随机播放（小写e）
        case 'S': cmdSetSequence = true; break;   // 设置顺序播放（大写S）
        case 'F': cmdShowList = true; break;      // 显示播放列表
        case 'G': cmdVolUp = true; break;         // 音量+
        case 'H': cmdVolDown = true; break;       // 音量-
        case 'I': cmdRecode_start = true; break;  // 录音开始
        case 'J': cmdRecode_end = true; break;    // 录音结束
      }
    }
    if (Serial2.available()) {
      // xSemaphoreTake(SerialMutex, portMAX_DELAY);
      char cmd = Serial2.read();
      // xSemaphoreGive(SerialMutex);

      switch (cmd) {
        case 'a': cmdNext = true; break;          // 下一曲
        case 'b': cmdPrev = true; break;          // 上一曲（小写a）
        // case 'B': cmdPauseToggle = true; break;   // 暂停/播放切换（保留）
        case 'c': cmdPause = true; break;         // 暂停（小写b）
        case 'd': cmdPlay = true; break;          // 播放（大写P）
        case 'e': cmdForward = true; break;       // 快进
        case 'f': cmdBackward = true; break;      // 后退
        // case 'E': cmdToggleRandom = true; break;  // 随机/顺序切换（保留）
        case 'g': cmdSetRandom = true; break;     // 设置随机播放（小写e）
        case 'h': cmdSetSequence = true; break;   // 设置顺序播放（大写S）
        case 'i': cmdShowList = true; break;      // 显示播放列表
        case 'j': cmdVolUp = true; break;         // 音量+
        case 'k': cmdVolDown = true; break;       // 音量-
        // case 'I': cmdRecode_start = true; break;  // 录音开始
        // case 'J': cmdRecode_end = true; break;    // 录音结束

        case 'C': switch_emotion_img(&guider_ui, 2); break;
        case 'D': switch_emotion_img(&guider_ui, 1); break;
        case 'E': switch_emotion_img(&guider_ui, 6); break;
        case 'F': switch_emotion_img(&guider_ui, 5); break;
        case 'G': switch_emotion_img(&guider_ui, 7); break;
        case 'H': zhuyili_flag = 1; break;
      }
    }
    // 上升

    // UI任务延迟保持10ms，提升响应速度
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}