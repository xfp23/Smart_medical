#include "System.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


extern "C" void app_main(void)
{
    device.begin();
    esp_task_wdt_add(NULL);
    while (true)
    {
        esp_task_wdt_reset();
        if (device.timeFlag.system_1ms == ON)
        {
            device.timeFlag.system_1ms = OFF;
        }
        if (device.timeFlag.system_10ms == ON)
        {
            device.timeFlag.system_10ms = OFF;
            device.DealWith_NTC();
        }
        if (device.timeFlag.system_100ms == ON)
        {
            device.timeFlag.system_100ms = OFF;
            device.DealWith_max3010x();
        }
        if (device.timeFlag.system_500ms == ON)
        {
            device.timeFlag.system_500ms = OFF;
             device.DealWith_oled();
        }
        if (device.timeFlag.system_1000ms == ON)
        {
            device.timeFlag.system_1000ms = OFF;
           
        }
    }
}
