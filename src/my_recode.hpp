#ifndef MY_RECODE_HPP
#define MY_RECODE_HPP

#include <Arduino.h>
#include "SD.h"
#include "SPI.h"
#include <vector>

#define I2S_WS     4
#define I2S_SD     6
#define I2S_SCK    5

// ===== 录音参数 =====
#define SAMPLE_RATE   16000     // 16kHz
#define I2S_READ_LEN  1024      // 每次读取样本数
#define RECORD_TIME   20         // 录音时长（秒）

extern volatile bool cmdRecode_start;
extern volatile bool cmdRecode_end;
extern const char* folderName;
extern const char* filename;

void writeWavHeader(File &file, uint32_t dataLength, uint16_t channels, uint32_t sampleRate, uint16_t bitsPerSample);
void recode_init(void);

#endif