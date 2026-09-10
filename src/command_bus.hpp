#ifndef COMMAND_BUS_HPP
#define COMMAND_BUS_HPP

#include <Arduino.h>
#include <cstdint>

// ========== 音频播放控制标志（原先在 loudspeaker.hpp 中） ==========
extern volatile uint32_t play_time;          // 音乐播放时间（秒）
extern volatile bool     needNextSong;       // 一曲结束自动播放下一曲
extern volatile bool     cmdNext;            // 手动下一曲
extern volatile bool     cmdPauseToggle;     // 暂停/继续（切换）
extern volatile bool     cmdForward;         // 快进 5 秒
extern volatile bool     cmdBackward;        // 后退 5 秒
extern volatile bool     cmdToggleRandom;    // 切换随机/顺序播放
extern volatile bool     cmdShowList;        // 显示播放列表
extern volatile bool     cmdVolUp;           // 音量 +
extern volatile bool     cmdVolDown;         // 音量 -
extern volatile bool     isPaused;           // 是否已暂停
extern volatile bool     isRandomPlay;       // 是否随机播放
extern volatile bool     isMusic;            // 是否在播放音乐
extern volatile bool     cmdPrev;            // 上一曲命令标志
extern volatile bool     cmdPause;           // 暂停（独立命令）
extern volatile bool     cmdPlay;            // 播放（独立命令）
extern volatile bool     cmdSetRandom;       // 设置随机播放
extern volatile bool     cmdSetSequence;     // 设置顺序播放
extern int               volume;             // 音量

// ========== 录音控制标志（原先在 my_recode.hpp 中） ==========
extern volatile bool     cmdRecode_start;    // 录音开始
extern volatile bool     cmdRecode_end;      // 录音结束

#endif // COMMAND_BUS_HPP