#include "my_main.hpp"




App app;
void setup()
{
    // my_mem_init_all();
    app.begin();
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}
