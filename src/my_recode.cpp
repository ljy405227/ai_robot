#include "my_recode.hpp"
#include "command_bus.hpp"
#include "loudspeaker.hpp"

const char* folderName = "/record";      // 文件夹名称
const char* filename = "/record/record.wav"; // 文件路径

// ----------------- I2S 配置 -----------------
i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = SAMPLE_RATE,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,       // INMP441 输出左声道
  .communication_format = I2S_COMM_FORMAT_STAND_I2S, // 标准 I2S
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 8,
  .dma_buf_len = 1024,
  .use_apll = true,
  .tx_desc_auto_clear = false,
  .fixed_mclk = 0
};

// ----------------- I2S 引脚配置 -----------------
i2s_pin_config_t pin_config = {
  .bck_io_num = I2S_SCK,
  .ws_io_num = I2S_WS,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = I2S_SD
};

// ----------------- WAV 头写入函数 -----------------
void writeWavHeader(File &file, uint32_t dataLength, uint16_t channels, uint32_t sampleRate, uint16_t bitsPerSample) {
  file.seek(0); // 回到文件开头覆盖

  file.write((const uint8_t *)"RIFF", 4);
  uint32_t chunkSize = 36 + dataLength;
  file.write((uint8_t*)&chunkSize, 4);
  file.write((const uint8_t *)"WAVE", 4);
  file.write((const uint8_t *)"fmt ", 4);
  uint32_t subChunk1Size = 16;
  file.write((uint8_t*)&subChunk1Size, 4);
  uint16_t audioFormat = 1; // PCM
  file.write((uint8_t*)&audioFormat, 2);
  file.write((uint8_t*)&channels, 2);
  file.write((uint8_t*)&sampleRate, 4);
  uint32_t byteRate = sampleRate * channels * bitsPerSample / 8;
  file.write((uint8_t*)&byteRate, 4);
  uint16_t blockAlign = channels * bitsPerSample / 8;
  file.write((uint8_t*)&blockAlign, 2);
  file.write((uint8_t*)&bitsPerSample, 2);
  file.write((const uint8_t *)"data", 4);
  file.write((uint8_t*)&dataLength, 4);
}

void recode_init(void)
{
  i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_1, &pin_config);
  Serial.println("I2S 初始化完成");


  // ----------------- 创建 WAV 文件并写占位头 -----------------
  if (!SD.exists(folderName)) {
    if (SD.mkdir(folderName)) {
      Serial.println("文件夹创建成功!");
    } else {
      Serial.println("文件夹创建失败!");
      while (1);
    }
  }

  // 创建文件
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("创建文件失败!");
    while (1);
  } else {
    Serial.println("文件创建成功!");
  }

  uint8_t wavHeader[44] = {0};
  file.write(wavHeader, 44); // 占位 44 字节 WAV 头

  // ----------------- 开始录音 -----------------
  int16_t buffer[I2S_READ_LEN];
  size_t bytesRead;
  uint32_t totalBytes = 0;
  uint32_t totalSamples = SAMPLE_RATE * RECORD_TIME;

  Serial.println("开始录音...");
  while (totalBytes / 2 < totalSamples) {
    i2s_read(I2S_NUM_1, buffer, I2S_READ_LEN * sizeof(int16_t), &bytesRead, portMAX_DELAY);
    file.write((uint8_t*)buffer, bytesRead);
    totalBytes += bytesRead;
  }

  // ----------------- 录音完成，写入 WAV 头 -----------------
  writeWavHeader(file, totalBytes, 1, SAMPLE_RATE, 16);
  file.close();

  Serial.println("录音完成！文件保存为 record.wav");
  // ----------------- 播放录音 -----------------
  Serial.println("开始播放录音...");
  audio.connecttoFS(SD, filename);
  isMusic = false;
  while (audio.isRunning()) {
      // 等待播放完成
      audio.loop();
  }
  Serial.println("播放结束");
}