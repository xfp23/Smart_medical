#include "http.h"
#include "i2c.h"
#include <cstring>
#include "MAX30102.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

MAX30102_Class max30102;

HTTP_Server http;
extern const char *html_content;
auto &ou = http;
extern "C" void app_main(void)
{
    //  http.OpenHtml(html_content);

    //  http.begin();
    idf_i2c_init();
    max30102.begin(GPIO_NUM_34, &i2c_bus_handle, 400000);
    max30102.setMode();
    // max30102.Read_Temp();
    //  max30102.OpenRedLed(REDLED_ALL);
    //  vTaskDelay(pdMS_TO_TICKS(1000));
    // max30102.setSlotLed(REDLED_ALL);
    // printf("REDLED_ALL\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));

    // max30102.setSlotLed(OFF_ALL);
    // printf("OFF_ALL\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));

    // max30102.setSlotLed(REDLED_1_2);
    // printf("REDLED_1_2\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));
    // max30102.setSlotLed(OFF_1_2);
    // printf("OFF_1_2\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));
     max30102.setSlotLed(REDLED_1_2);
    // printf("REDLED_3_4\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));
    //max30102.setSlotLed(OFF_3_4);
    //printf("OFF_3_4\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));
    // max30102.setSlotLed(REDLED_ALL);
    // printf("REDLED_ALL\n");
    while (true)
    {

        // if (max30102.check())
        // {
        printf(" temp: %f\n", max30102.Read_Temp());
        // }
        // else
        // {
        //     printf("intrrput don't tigger\n");
        // }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
