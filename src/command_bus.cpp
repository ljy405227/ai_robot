#include "command_bus.hpp"

// ========== 音频播放控制标志 ==========
volatile uint32_t play_time        = 0;
volatile bool     needNextSong     = false;
volatile bool     cmdNext          = false;
volatile bool     cmdPauseToggle   = false;
volatile bool     cmdForward       = false;
volatile bool     cmdBackward      = false;
volatile bool     cmdToggleRandom  = false;
volatile bool     cmdShowList      = false;
volatile bool     cmdVolUp         = false;
volatile bool     cmdVolDown       = false;
volatile bool     isPaused         = true;
volatile bool     isRandomPlay     = false;
volatile bool     isMusic          = false;
volatile bool     cmdPrev          = false;
int               volume           = 10;
volatile bool     cmdPause         = false;
volatile bool     cmdPlay          = false;
volatile bool     cmdSetRandom     = false;
volatile bool     cmdSetSequence   = false;

// ========== 录音控制标志 ==========
volatile bool cmdRecode_start      = false;
volatile bool cmdRecode_end        = false;

// 注：原先 loudspeaker.cpp 中的 cmdScmdSetSequenceop 是拼写错误且未被任何代码引用，已移除