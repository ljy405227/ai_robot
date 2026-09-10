#ifndef LV_PORT_DISP_TEMPL_H
#define LV_PORT_DISP_TEMPL_H



/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "../../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#include "lcd_jap.hpp"  // 日本鬼子的LCD父类
extern LGFX_EmmaDisplay my_Lcd;


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/* Initialize low level display driver */
void lv_port_disp_init(void);

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void);

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void);

/**********************
 *      MACROS
 **********************/


#endif /*Disable/Enable content*/
