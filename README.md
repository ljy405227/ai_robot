# AI 陪伴机器人 (AI Robot Companion)

<div align="center">
  <img src="https://img.shields.io/badge/platform-ESP32--S3-blue" alt="Platform">
  <img src="https://img.shields.io/badge/framework-Arduino-00979D" alt="Framework">
  <img src="https://img.shields.io/badge/LVGL-8.3.11-green" alt="LVGL">
  <img src="https://img.shields.io/badge/license-MIT-yellow" alt="License">
</div>

基于 **ESP32-S3** 的 AI 智能陪伴机器人，具有音频播放、LVGL 图形界面、WiFi AI 对话、I2S 录音、触摸交互等功能。

**作者：** 欢乐豆  
**邮箱：** 2703867620@qq.com  
**GitHub：** [https://github.com/ljy405227/ai_robot](https://github.com/ljy405227/ai_robot)

---

## 功能特性

- 🎵 **音频播放** — 支持 MP3/FLAC 格式，从 SD 卡 `/music` 目录读取
- 🖥️ **LVGL 图形界面** — 7 种情绪表情动画显示（开心、中性、厌恶、恐惧、愤怒、悲伤、惊讶）
- 🎙️ **I2S 录音** — 通过 INMP441 麦克风录制 WAV 音频并回放
- 🌐 **WiFi AI 对话** — 连接 SiliconFlow API，调用 DeepSeek 大模型进行语音对话
- 💡 **LED 呼吸灯** — 根据交互状态控制背光 PWM 呼吸效果
- 📺 **触摸屏交互** — 电容触摸 + 串口命令双通道控制
- 🔀 **播放控制** — 上一曲/下一曲、暂停/播放、快进/快退、随机/顺序播放、音量调节

## 硬件要求

| 组件 | 型号/规格 |
|------|-----------|
| 主控 | ESP32-S3（4D Systems GEN4-ESP32 R8N16） |
| 显示屏 | ST7789 / 240×280 / SPI 接口 |
| 触摸 | 电容式触摸屏（I2C 地址 0x15） |
| 音频编解码 | ESP32-audioI2S（I2S 接口） |
| 扬声器 | 8Ω / 1W~3W |
| 麦克风 | INMP441（I2S 数字麦克风） |
| SD 卡 | SPI 接口，FAT32 格式 |
| 温湿度传感器 | DHT20 / SHT20（I2C 接口） |
| 温度传感器 | DS18B20（1-Wire 接口） |

## 引脚定义

| 功能 | 引脚 | 说明 |
|------|------|------|
| **显示屏** | | |
| SCLK | GPIO40 | SPI 时钟 |
| MOSI | GPIO41 | SPI 数据 |
| DC | GPIO38 | 数据/命令控制 |
| CS | GPIO39 | 片选 |
| RST | GPIO47 | 复位 |
| BL | GPIO45 | 背光 PWM |
| **触摸** | | |
| SDA | GPIO21 | I2C 数据 |
| SCL | - | I2C 时钟（与显示屏共用） |
| **音频** | | |
| LRC | GPIO42 | I2S 帧时钟 |
| BLCK | GPIO1 | I2S 位时钟 |
| DIN | GPIO2 | I2S 数据输出 |
| **麦克风** | | |
| I2S_WS | GPIO4 | I2S 字选择 |
| I2S_SD | GPIO6 | I2S 数据输入 |
| I2S_SCK | GPIO5 | I2S 串行时钟 |
| **SD 卡** | | |
| CS | GPIO17 | 片选 |
| MOSI | GPIO15 | SPI 数据输入 |
| MISO | GPIO16 | SPI 数据输出 |
| SCK | GPIO7 | SPI 时钟 |
| **串口** | | |
| Serial2 TX | GPIO14 | 第二路串口发送 |
| Serial2 RX | GPIO46 | 第二路串口接收 |
| **LED 背光** | GPIO3 | PWM 通道 0，17kHz |

## 快速开始

### 1. 安装 PlatformIO

```bash
pip install platformio
```

### 2. 克隆项目

```bash
git clone https://github.com/ljy405227/ai_robot.git
cd ai_robot
```

### 3. 准备 SD 卡

在 SD 卡根目录创建 `music` 文件夹，放入 `.mp3` 或 `.flac` 文件：

```
SD卡/
└── music/
    ├── song1.mp3
    ├── song2.flac
    └── ...
```

### 4. 配置 WiFi（可选）

编辑 `src/my_wifi.cpp`，修改 WiFi 名称和密码：

```cpp
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";
```

### 5. 编译上传

使用 VSCode + PlatformIO 扩展打开项目，或命令行：

```bash
pio run -t upload
```

> **注意：** 使用 16MB 闪存分区表，请确保 ESP32-S3 模组闪存大小 ≥ 16MB

## 串口命令

通过 USB 串口（115200 baud）或 UART2（GPIO14/46）发送以下命令：

### 音频控制

| 命令 | USB | UART2 | 功能 |
|------|-----|-------|------|
| 下一曲 | `A` | `a` | 切换到下一首 |
| 上一曲 | `a` | `b` | 切换到上一首 |
| 暂停/播放切换 | `B` | — | 切换暂停/播放状态 |
| 暂停 | `b` | `c` | 暂停播放 |
| 播放 | `P` | `d` | 继续播放 |
| 快进 5s | `C` | `e` | 快进 5 秒 |
| 后退 5s | `D` | `f` | 后退 5 秒 |
| 随机/顺序切换 | `E` | — | 切换播放模式 |
| 设置随机播放 | `e` | `g` | 强制设为随机模式 |
| 设置顺序播放 | `S` | `h` | 强制设为顺序模式 |
| 显示播放列表 | `F` | `i` | 打印接下来 10 首 |
| 音量 + | `G` | `j` | 音量增加 |
| 音量 - | `H` | `k` | 音量减少 |
| 录音开始 | `I` | — | 开始录音 |
| 录音结束 | `J` | — | 结束录音 |

### 表情控制（仅 UART2）

| 命令 | 表情 |
|------|------|
| `C` | 开心 😊 |
| `D` | 中性 😐 |
| `E` | 悲伤 😢 |
| `F` | 愤怒 😠 |
| `G` | 惊讶 😲 |
| `H` | 注意力（LED 呼吸） 💡 |

> USB 串口默认通过 `Serial`（USB CDC），UART2 通过 `Serial2`（GPIO14/46）

## 项目结构

```
ai_robot/
├── platformio.ini                # PlatformIO 构建配置
├── my_partitions.csv             # 16MB 闪存分区表
├── README.md                     # 本文件
│
├── src/                          # # ESP32 固件源码
│   ├── main.cpp                  # 入口：setup() / loop()
│   │
│   ├── my_main.hpp / .cpp        # App 类定义 + 初始化 + LVGL 任务
│   ├── audio_task.cpp            # AudioTask：音频播放 + 命令处理
│   ├── ui_task.cpp               # UITask：串口命令读取
│   ├── ai_task.cpp               # AI_Task：LED 呼吸控制
│   ├── helpers.cpp               # 辅助函数（栈监控、首播初始化等）
│   │
│   ├── command_bus.hpp / .cpp    # 全局命令标志总线
│   ├── app_globals.hpp / .cpp    # 应用全局变量
│   │
│   ├── loudspeaker.hpp / .cpp    # 音频播放（ESP32-audioI2S）
│   ├── my_sd.hpp / .cpp          # SD 卡驱动 + 音乐扫描
│   ├── my_recode.hpp / .cpp      # I2S 录音（WAV 格式）
│   ├── my_wifi.hpp / .cpp        # WiFi + AI API 调用
│   │
│   ├── lv_conf.h                 # LVGL 配置
│   ├── lvgl.h                    # LVGL 主头文件
│   ├── touch.hpp                 # 电容触摸驱动
│   └── lcd_jap.hpp               # 显示屏驱动（LovyanGFX）
│
├── src/lvgl_ui/                  # LVGL 用户界面
│   ├── gui_guider.h / .c         # UI 引导器
│   ├── setup_scr_screen.cpp      # 主界面：7 个情绪表情
│   ├── emotion.hpp               # 表情切换函数声明
│   ├── events_init.hpp / .cpp    # 事件初始化
│   ├── custom.hpp / .cpp         # 自定义初始化钩子
│   └── images/                   # 表情位图（C 数组）
│
├── lib/                          # 第三方库
│   ├── lvgl/                     # LVGL v8.3.11
│   └── LovyanGFX/                # 图形加速库
│
└── 树莓派程序/                   # 树莓派人脸情绪识别
    ├── face_fer/                 # v1：OpenCV DNN + FER
    └── fer_efficientnetb0/       # v2：EfficientNetB0 + PyTorch
```

## 树莓派 — 人脸情绪识别

项目包含基于树莓派的人脸检测与情绪识别程序，通过串口与 ESP32 通信：

### v1（`树莓派程序/face_fer/`）
- **人脸检测：** OpenCV YuNet DNN 模型
- **情绪识别：** ONNX Runtime 7 类分类器
- **模型文件：** `face_detection_yunet_2023mar.onnx` + `fer.onnx`

### v2（`树莓派程序/fer_efficientnetb0/`）
- **模型架构：** EfficientNetB0（PyTorch）
- **训练数据：** RAF-DB 数据集
- **训练脚本：** `train.py`（含标签平滑、余弦退火）
- **导出格式：** PyTorch → ONNX

**情绪类别：** 愤怒、厌恶、恐惧、开心、平和、悲伤、惊讶

## AI API 配置

项目默认使用 [SiliconFlow API](https://siliconflow.cn) 调用 DeepSeek 大模型：

```cpp
// my_wifi.cpp 中配置
const char* apiKey = "your_api_key_here";     // SiliconFlow API Key
// 模型：deepseek-ai/DeepSeek-OCR
// 端点：https://api.siliconflow.cn/v1/chat/completions
```

## 依赖库

| 库 | 版本 | 用途 |
|---|---|---|
| [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) | 2.3.0 | I2S 音频解码/播放 |
| [LovyanGFX](https://github.com/lovyan03/LovyanGFX) | — | 高速图形库（SPI 显示屏） |
| [LVGL](https://lvgl.io) | 8.3.11 | 嵌入式 GUI 框架 |
| [ArduinoJson](https://arduinojson.org) | 7.4.2 | JSON 序列化（AI API） |
| [OneWire](https://github.com/PaulStoffregen/OneWire) | 2.3.8 | 1-Wire 总线 |
| [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) | 4.0.5 | DS18B20 温度传感器 |
| [DFRobot_SHT20](https://github.com/DFRobot/DFRobot_SHT20) | 1.0.0 | SHT20 温湿度传感器 |

## 许可

本项目采用 MIT 许可证。详见 `LICENSE` 文件。

---

**欢乐豆** &copy; 2026 | [GitHub](https://github.com/ljy405227/ai_robot)