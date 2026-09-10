#include "lvgl.h"
#include <stdio.h>
#include "events_init.hpp"
#include "../my_sd.hpp"

// void update_music_icons(int idx, bool paused);
// void music_btn_event_cb(lv_event_t *e)
// {
//     if (!e) return;
//     int idx = (int)lv_event_get_user_data(e);

//     // 点击同一首：暂停 / 播放切换
//     if (idx == currentIndex) {
//         isPaused = !isPaused;
//         audio.pauseResume();
//         update_music_icons(idx, isPaused);
//         return;
//     }

//     // 点击新歌：切换播放
//     currentIndex = idx;
//     isPaused = false;
//     playCurrentSong();
//     update_music_icons(currentIndex, false);
// }


void events_init(lv_ui *ui)
{

}
