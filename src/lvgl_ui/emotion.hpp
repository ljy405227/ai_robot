#ifndef LVGL_EMOTION_HPP
#define LVGL_EMOTION_HPP

#include "gui_guider.h"

/**
 * @brief 切换表情图片（1~7 分别对应 7 种情绪）
 * @param ui  LVGL UI 结构体指针
 * @param num 表情编号：1=开心, 2=中性, 3=厌恶, 4=恐惧, 5=愤怒, 6=悲伤, 7=惊讶
 */
void switch_emotion_img(lv_ui *ui, int num);

#endif // LVGL_EMOTION_HPP