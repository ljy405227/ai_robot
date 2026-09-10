#include "my_main.hpp"
#include "command_bus.hpp"
#include "app_globals.hpp"
#include "my_sd.hpp"
#include "loudspeaker.hpp"

// ===== Audio 任务 =====
void App::AudioTask(void *param) {
  while (1) {

      unsigned long loopStart = millis();

      if (!isFirstPlayInited) {
        initFirstPlay();
        vTaskDelay(pdMS_TO_TICKS(10));
        continue;
      }

      // 音频核心处理（必须优先执行，无延迟）
      if (!isAudioLoading) {
        audio.loop();
        if (millis() - loopStart > 50) {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("⚠️ audio.loop() 执行超时，可能卡住了！");
          xSemaphoreGive(SerialMutex);
        }
      } else {
        audio.loop();
      }

      // 非核心逻辑：打印播放时间（每秒一次，不影响实时性）
      if (!isPaused && millis() - lastTimePrint >= 1000 && sd_get_flag == 1)
      {
        lastTimePrint = millis();

        play_time = audio.getAudioCurrentTime();
        uint32_t total = audio.getAudioFileDuration();

        static uint32_t last_play_time = 0;
        if (play_time == last_play_time && !isPaused && !isAudioLoading) {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("⚠️ 音频播放进度卡住，尝试恢复播放！");
          xSemaphoreGive(SerialMutex);
        }
        last_play_time = play_time;

        uint32_t remain = (total > play_time) ? (total - play_time) : 0;

        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.printf(
          "⏱ %02d:%02d / %02d:%02d  (剩余 %02d:%02d)\n",
          play_time / 60, play_time % 60,
          total / 60, total % 60,
          remain / 60, remain % 60
        );
        xSemaphoreGive(SerialMutex);
      }

      // 下一曲命令处理逻辑
      if (cmdNext || needNextSong)
      {
        cmdNext = false;
        needNextSong = false;
        isMusic = true;

        bool wasPaused = isPaused;
        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.printf("📝 切换前状态：%s\n", wasPaused ? "暂停" : "播放");
        xSemaphoreGive(SerialMutex);

        if (isRandomPlay) {
          currentIndex = random(playlist.size());
        } else {
          currentIndex++;
          if (currentIndex >= playlist.size()) currentIndex = 0;
        }

        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.printf("▶ 正在播放: %s\n", playlist[currentIndex].c_str());
        xSemaphoreGive(SerialMutex);

        playCurrentSong();

        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.println(isRandomPlay ? "🔀 随机播放模式" : "➡ 顺序播放模式");
        xSemaphoreGive(SerialMutex);
      }

      // ========== 新增：上一曲命令处理逻辑 ==========
      if (cmdPrev) {
        cmdPrev = false;
        isMusic = true;

        bool wasPaused = isPaused;
        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.printf("📝 切换前状态：%s\n", wasPaused ? "暂停" : "播放");
        xSemaphoreGive(SerialMutex);

        if (isRandomPlay) {
          // 随机播放模式下，随机选曲（和下一曲逻辑一致）
          currentIndex = random(playlist.size());
        } else {
          // 顺序播放模式下，索引减1，处理边界（小于0时回到最后一位）
          currentIndex--;
          if (currentIndex < 0) {
            currentIndex = playlist.size() - 1;
          }
        }

        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.printf("◀ 正在播放: %s\n", playlist[currentIndex].c_str());
        xSemaphoreGive(SerialMutex);

        playCurrentSong();

        if (waitAudioLoadComplete()) {
          if (wasPaused) {
            audio.pauseResume();
            isPaused = true;
            isAudioPlaying = false;
            xSemaphoreTake(SerialMutex, portMAX_DELAY);
            Serial.println("⏸ 恢复暂停状态");
            xSemaphoreGive(SerialMutex);
          } else {
            isPaused = false;
            isAudioPlaying = true;
          }
        } else {
          isPaused = true;
          isAudioPlaying = false;
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("❌ 音频加载失败，设置为暂停状态");
          xSemaphoreGive(SerialMutex);
        }

        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.println(isRandomPlay ? "🔀 随机播放模式" : "⬅ 顺序播放模式");
        xSemaphoreGive(SerialMutex);
      }
      // ========== 上一曲逻辑结束 ==========

      // 原有暂停/播放切换逻辑（保留，也可以删除）
      if (cmdPauseToggle) {
        cmdPauseToggle = false;
        isMusic = true;
        audio.pauseResume();
        isPaused = !isPaused;

        isAudioPlaying = !isPaused;
        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.println(isPaused ? "⏸ 已暂停" : "▶ 继续播放");
        xSemaphoreGive(SerialMutex);
      }

      // ========== 新增：独立暂停命令 ==========
      if (cmdPause) {
        cmdPause = false;
        isMusic = true;
        if (!isPaused) { // 只在未暂停时执行
          audio.pauseResume();
          isPaused = true;
          isAudioPlaying = false;
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("⏸ 已暂停");
          xSemaphoreGive(SerialMutex);
        } else {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("ℹ️ 已经处于暂停状态");
          xSemaphoreGive(SerialMutex);
        }
      }

      // ========== 新增：独立播放命令 ==========
      if (cmdPlay) {
        cmdPlay = false;
        isMusic = true;
        if (isPaused) { // 只在暂停时执行
          audio.pauseResume();
          isPaused = false;
          isAudioPlaying = true;
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("▶ 继续播放");
          xSemaphoreGive(SerialMutex);
        } else {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("ℹ️ 已经处于播放状态");
          xSemaphoreGive(SerialMutex);
        }
      }

      if (cmdForward) {
        cmdForward = false;
        if (!isAudioLoading) {
          isMusic = true;
          audio.setAudioPlayPosition(audio.getAudioCurrentTime() + 5);
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("⏩ 快进 5 秒");
          xSemaphoreGive(SerialMutex);
        } else {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("⚠️ 音频加载中，暂不响应快进");
          xSemaphoreGive(SerialMutex);
        }
      }

      if (cmdBackward) {
        cmdBackward = false;
        if (!isAudioLoading) {
          isMusic = true;
          int pos = audio.getAudioCurrentTime() - 5;
          if (pos < 0) pos = 0;
          audio.setAudioPlayPosition(pos);
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("⏪ 后退 5 秒");
          xSemaphoreGive(SerialMutex);
        } else {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("⚠️ 音频加载中，暂不响应后退");
          xSemaphoreGive(SerialMutex);
        }
      }

      // 原有随机/顺序切换逻辑（保留，也可以删除）
      if (cmdToggleRandom) {
        cmdToggleRandom = false;
        isRandomPlay = !isRandomPlay;
        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.println(isRandomPlay ? "🔀 随机播放" : "➡ 顺序播放");
        xSemaphoreGive(SerialMutex);
      }

      // ========== 新增：独立设置随机播放 ==========
      if (cmdSetRandom) {
        cmdSetRandom = false;
        if (!isRandomPlay) { // 只在非随机模式时切换
          isRandomPlay = true;
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("🔀 已设置为随机播放模式");
          xSemaphoreGive(SerialMutex);
        } else {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("ℹ️ 已经处于随机播放模式");
          xSemaphoreGive(SerialMutex);
        }
      }

      // ========== 新增：独立设置顺序播放 ==========
      if (cmdSetSequence) {
        cmdSetSequence = false;
        if (isRandomPlay) { // 只在随机模式时切换
          isRandomPlay = false;
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("➡ 已设置为顺序播放模式");
          xSemaphoreGive(SerialMutex);
        } else {
          xSemaphoreTake(SerialMutex, portMAX_DELAY);
          Serial.println("ℹ️ 已经处于顺序播放模式");
          xSemaphoreGive(SerialMutex);
        }
      }

      if (cmdShowList) {
        cmdShowList = false;
        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.println("接下来播放：");

        if (isRandomPlay) {
          for (int i = 0; i < 10 && playlist.size() > 1; i++) {
            int idx = random(playlist.size());
            Serial.printf("  • %s\n", playlist[idx].c_str());
          }
        } else {
          for (int i = 1; i <= 10 && i < playlist.size(); i++) {
            int idx = (currentIndex + i) % playlist.size();
            Serial.printf("  • %s\n", playlist[idx].c_str());
          }
        }
        xSemaphoreGive(SerialMutex);
      }

      if (cmdVolUp) {
        cmdVolUp = false;
        if (volume < 21) volume++;
        audio.setVolume(volume);
        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.printf("🔊 音量: %d\n", volume);
        xSemaphoreGive(SerialMutex);
      }

      if (cmdVolDown) {
        cmdVolDown = false;
        if (volume > 0) volume--;
        audio.setVolume(volume);
        xSemaphoreTake(SerialMutex, portMAX_DELAY);
        Serial.printf("🔉 音量: %d\n", volume);
        xSemaphoreGive(SerialMutex);
      }

    // 关键恢复：AudioTask延迟改回2ms，保证音频实时性
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}