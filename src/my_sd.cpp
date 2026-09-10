#include "my_sd.hpp"
#include "loudspeaker.hpp"
#include "command_bus.hpp"

static SPIClass* sd_spi = new SPIClass(FSPI);

// ===== 播放列表 =====
std::vector<String> playlist;
volatile int currentIndex = 0;

volatile uint8_t sd_get_flag = 0;         // sd卡获取标志



void sd_init(void)
{
  int maxRetries = 5;      // 最大重试次数
  int retryCount = 0;      // 当前尝试次数
  sd_spi->begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  while (retryCount < maxRetries)
  {
    if (SD.begin(SD_CS, *sd_spi, 2000000)) {
        Serial.println("SD卡初始化成功");
        sd_get_flag = 1;
        break;
    } else {
        retryCount++;
        Serial.print("SD卡初始化失败，重试第 ");
        Serial.print(retryCount);
        Serial.println(" 次...");
        delay(100);
    }
  }

  if (!sd_get_flag) {
      Serial.println("SD卡连续初始化失败，请检查是否插卡或SD卡是否损坏");
      sd_get_flag = 0;
  }

}


void mp3_init(void)
{
  if (sd_get_flag)
  {
    scanSDForMP3();
    if (playlist.empty())
    {
      Serial.println("未找到 MP3");
      sd_get_flag = 0;
    }
    playCurrentSong();
    audio.pauseResume();
    isPaused = true;
  }
}

// ===== 扫描 SD 根目录 MP3 =====
void scanSDForMP3(void) {
  File musicDir = SD.open("/music");
  if (!musicDir) {
    Serial.println("打开music文件夹失败");
    return;
  }
  else
  {
    Serial.println("扫描 SD 卡中的 MP3 文件...");
  }

  File file;
  while ((file = musicDir.openNextFile())) {
    if (!file.isDirectory()) {
      String name = file.name();
      name.toLowerCase();
      if (name.endsWith(".flac" ) || name.endsWith(".mp3")) {
        playlist.push_back(String("/music/") + file.name());
        Serial.print("🎵 发现 FLAC: ");
        Serial.println(file.name());
      }
    }
    file.close();
  }
  musicDir.close();

  Serial.print("FLAC 总数: ");
  Serial.println(playlist.size());
}