#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.hpp"
#include "widgets_init.h"
#include "custom.hpp"
#include "../lv_font_source_han_sans_normal_16.c"   // 引入字体
#include <vector>
#include "../my_sd.hpp"
// #include "../my_main.hpp"  // 重构后不再需要

// 声明字体
LV_FONT_DECLARE(lv_font_source_han_sans_normal_16);


// 图片切换函数：输入 1~7 显示对应表情，其他值全部隐藏
void switch_emotion_img(lv_ui *ui, int num)
{

    lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_img_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_img_3, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_img_4, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_img_5, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_img_6, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_img_7, LV_OBJ_FLAG_HIDDEN);

    // 根据序号显示对应图片
    switch(num)
    {
        case 1: lv_obj_clear_flag(ui->screen_img_1, LV_OBJ_FLAG_HIDDEN); break; // 开心
        case 2: lv_obj_clear_flag(ui->screen_img_2, LV_OBJ_FLAG_HIDDEN); break; // 中性
        case 3: lv_obj_clear_flag(ui->screen_img_3, LV_OBJ_FLAG_HIDDEN); break; // 厌恶
        case 4: lv_obj_clear_flag(ui->screen_img_4, LV_OBJ_FLAG_HIDDEN); break; // 恐惧
        case 5: lv_obj_clear_flag(ui->screen_img_5, LV_OBJ_FLAG_HIDDEN); break; // 愤怒
        case 6: lv_obj_clear_flag(ui->screen_img_6, LV_OBJ_FLAG_HIDDEN); break; // 悲伤
        case 7: lv_obj_clear_flag(ui->screen_img_7, LV_OBJ_FLAG_HIDDEN); break; // 惊讶
        default: break;
    }
}

void setup_scr_screen(lv_ui *ui)
{
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 280, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_1
    ui->screen_img_1 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_1, &_1happy_1_alpha_280x128);
    lv_img_set_pivot(ui->screen_img_1, 50,50);
    lv_img_set_angle(ui->screen_img_1, 0);
    lv_obj_set_pos(ui->screen_img_1, 0, 37);
    lv_obj_set_size(ui->screen_img_1, 280, 128);

    //Write style for screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_2
    ui->screen_img_2 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_2, &_1neutral_1_alpha_277x162);
    lv_img_set_pivot(ui->screen_img_2, 50,50);
    lv_img_set_angle(ui->screen_img_2, 0);
    lv_obj_set_pos(ui->screen_img_2, 0, 37);
    lv_obj_set_size(ui->screen_img_2, 277, 162);
    lv_obj_add_flag(ui->screen_img_2, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_3
    ui->screen_img_3 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_3, &_1disgust_2_alpha_277x172);
    lv_img_set_pivot(ui->screen_img_3, 50,50);
    lv_img_set_angle(ui->screen_img_3, 0);
    lv_obj_set_pos(ui->screen_img_3, 0, 37);
    lv_obj_set_size(ui->screen_img_3, 277, 172);
    lv_obj_add_flag(ui->screen_img_3, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_4
    ui->screen_img_4 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_4, &_1fear_1_alpha_273x163);
    lv_img_set_pivot(ui->screen_img_4, 50,50);
    lv_img_set_angle(ui->screen_img_4, 0);
    lv_obj_set_pos(ui->screen_img_4, 4, 37);
    lv_obj_set_size(ui->screen_img_4, 273, 163);
    lv_obj_add_flag(ui->screen_img_4, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_5
    ui->screen_img_5 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_5, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_5, &_1anger_7_alpha_274x160);
    lv_img_set_pivot(ui->screen_img_5, 50,50);
    lv_img_set_angle(ui->screen_img_5, 0);
    lv_obj_set_pos(ui->screen_img_5, 0, 37);
    lv_obj_set_size(ui->screen_img_5, 274, 160);
    lv_obj_add_flag(ui->screen_img_5, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_img_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_6
    ui->screen_img_6 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_6, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_6, &_1sadness_11_alpha_275x148);
    lv_img_set_pivot(ui->screen_img_6, 50,50);
    lv_img_set_angle(ui->screen_img_6, 0);
    lv_obj_set_pos(ui->screen_img_6, 4, 46);
    lv_obj_set_size(ui->screen_img_6, 275, 148);
    lv_obj_add_flag(ui->screen_img_6, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_img_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_7
    ui->screen_img_7 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_7, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_7, &_1surprise_1_alpha_278x162);
    lv_img_set_pivot(ui->screen_img_7, 50,50);
    lv_img_set_angle(ui->screen_img_7, 0);
    lv_obj_set_pos(ui->screen_img_7, 0, 43);
    lv_obj_set_size(ui->screen_img_7, 278, 162);
    lv_obj_add_flag(ui->screen_img_7, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_img_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen.

    //Update current screen layout.
    lv_obj_update_layout(ui->screen);
}
