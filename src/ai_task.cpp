#include "my_main.hpp"
#include "app_globals.hpp"

void App::AI_Task(void *param)
{
  while (1)
  {
    if(zhuyili_flag != 0)
    {
      zhuyili_flag++;
      if (zhuyili_flag == 2)
      {
        zhuyili_flag = 0;
      }

      for (int i = 0; i <= 10; i++) {
        ledcWrite(0, i);
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      // 下降
      for (int i = 10; i >= 0; i--) {
          ledcWrite(0, i);
          vTaskDelay(pdMS_TO_TICKS(100));
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));

  }
}