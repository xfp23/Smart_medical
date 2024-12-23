#include "http.h"



HTTP_Server http;
extern const char* html_content;
extern "C"  void app_main(void) {
     http.OpenHtml(html_content);
     http.begin();

    while (true) {
        vTaskDelay(100);
    }
    
}
