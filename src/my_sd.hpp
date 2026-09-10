#ifndef MY_SD_HPP
#define MY_SD_HPP

#include <Arduino.h>
#include "SD.h"
#include "SPI.h"
#include <vector>

#define SD_CS    17
#define SD_MOSI  15
#define SD_MISO  16
#define SD_SCK   7

extern std::vector<String> playlist;
extern volatile int currentIndex;

extern volatile uint8_t sd_get_flag;  // sd获取标志

void sd_init(void);
void mp3_init(void);
void scanSDForMP3(void);
#endif