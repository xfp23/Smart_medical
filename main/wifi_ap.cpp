#include "wifi_ap.h"

void wifi_init() {
    nvs_flash_init();  // 初始化NVS
    esp_netif_init();  // 初始化网络接口
}