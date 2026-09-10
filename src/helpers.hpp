#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <Arduino.h>

// ===== 打印所有任务的栈使用情况 =====
void print_task_stack_info();

// ===== 等待音频加载完成（超时机制） =====
bool waitAudioLoadComplete();

// ===== 首次播放初始化 =====
void initFirstPlay();

#endif // HELPERS_HPP