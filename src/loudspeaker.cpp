#include "loudspeaker.hpp"
#include "my_sd.hpp"
#include "command_bus.hpp"

Audio audio;

void loudspeaker_init(void)
{
  audio.setPinout(LOUDSPEAKE_BLCK, LOUDSPEAKE_LRC,LOUDSPEAKE_DIN);
  audio.setVolume(volume);
  Serial.println("扬声器初始化完成");
}

// ===== 播放当前歌曲 =====
void playCurrentSong(void) {
  if (playlist.empty())
  {
    Serial.println("播放列表为空!");
    return;
  }
  isPaused = false;
  Serial.println("=================================");
  Serial.print("▶ 正在播放: ");
  Serial.println(playlist[currentIndex]);
  Serial.println(isRandomPlay ? "🔀 随机播放模式" : "➡ 顺序播放模式");
  audio.connecttoFS(SD, playlist[currentIndex].c_str());
}

// ===== EOF 回调 =====
void audio_eof_mp3(const char *info) {
  Serial.println("进入一次回调函数");
  if (isMusic)
  {
    needNextSong = true;
  }
}