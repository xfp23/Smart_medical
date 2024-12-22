#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_netif.h"

#define WIFI_SSID "ESP32_AP"
#define WIFI_PASS "123456789"
#define MAX_STA_CONN 4

static const char *TAG = "HTTP_SERVER";

// HTTP GET 请求处理函数
esp_err_t hello_get_handler(httpd_req_t *req) {
    const char *resp_str = "<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"    <meta charset=\"UTF-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"    <title>Health Information</title>"
"    <style>"
"        body {"
"            font-family: Arial, sans-serif;"
"            background-color: #f5f5f5;"
"            margin: 0;"
"            padding: 0;"
"        }"
"        nav {"
"            background-color: #4CAF50;"
"            overflow: hidden;"
"        }"
"        nav a {"
"            float: left;"
"            display: block;"
"            color: white;"
"            padding: 14px 20px;"
"            text-align: center;"
"            text-decoration: none;"
"        }"
"        nav a:hover {"
"            background-color: #45a049;"
"        }"
"        .dropdown {"
"            float: left;"
"            overflow: hidden;"
"        }"
"        .dropdown .dropbtn {"
"            background-color: #4CAF50;"
"            color: white;"
"            padding: 14px 20px;"
"            font-size: 16px;"
"            border: none;"
"            cursor: pointer;"
"        }"
"        .dropdown:hover .dropbtn {"
"            background-color: #45a049;"
"        }"
"        .dropdown-content {"
"            display: none;"
"            position: absolute;"
"            background-color: #f9f9f9;"
"            min-width: 160px;"
"            box-shadow: 0px 8px 16px rgba(0, 0, 0, 0.2);"
"            z-index: 1;"
"        }"
"        .dropdown-content a {"
"            color: black;"
"            padding: 12px 16px;"
"            text-decoration: none;"
"            display: block;"
"        }"
"        .dropdown-content a:hover {"
"            background-color: #ddd;"
"        }"
"        .dropdown:hover .dropdown-content {"
"            display: block;"
"        }"
"        .content {"
"            padding: 20px;"
"            margin-top: 20px;"
"            background-color: white;"
"            border-radius: 5px;"
"            box-shadow: 0px 4px 8px rgb(245, 3, 3);"
"        }"
"        .content h2 {"
"            color: #333;"
"        }"
"        .data {"
"            font-size: 18px;"
"            color: #555;"
"        }"
"    </style>"
"</head>"
"<body>"
"    <!-- Navigation Bar -->"
"    <nav>"
"        <a href=\"#\">Home</a>"
"        <div class=\"dropdown\">"
"            <button class=\"dropbtn\">Health Info</button>"
"            <div class=\"dropdown-content\">"
"                <a href=\"#\" onclick=\"showContent('temperature')\">Temperature</a>"
"                <a href=\"#\" onclick=\"showContent('oxygen')\">Blood Oxygen</a>"
"                <a href=\"#\" onclick=\"showContent('heartbeat')\">Heart Rate</a>"
"            </div>"
"        </div>"
"    </nav>"
"    <!-- Content Section -->"
"    <div class=\"content\" id=\"content\">"
"        <h2>Select a category to view the information</h2>"
"        <div class=\"data\" id=\"data\">Please select from the menu above.</div>"
"    </div>"
"    <script>"
"        // Simulated data for each category"
"        const data = {"
"            temperature: \"36.5°C\","
"            oxygen: \"98%\","
"            heartbeat: \"72 bpm\""
"        };"
"        // Function to update content based on selection"
"        function showContent(category) {"
"            const contentElement = document.getElementById('content');"
"            const dataElement = document.getElementById('data');"
"            contentElement.innerHTML = `<h2>${category.charAt(0).toUpperCase() + category.slice(1)} Information</h2>`;"
"            dataElement.innerHTML = `Current ${category.charAt(0).toUpperCase() + category.slice(1)}: ${data[category]}`;"
"        }"
"    </script>"
"</body>"
"</html>";

    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

// 创建 HTTP 服务器
httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;

    // 注册处理函数
    httpd_uri_t hello_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = hello_get_handler,
        .user_ctx  = NULL
    };

    if (httpd_start(&server, &config) == ESP_OK) {
 //       ESP_LOGI(TAG, "Starting HTTP Server on /");
        // 注册 URI
        httpd_register_uri_handler(server, &hello_uri);
        return server;
    }

    //ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

// 停止 HTTP 服务器
void stop_webserver(httpd_handle_t server) {
    httpd_stop(server);
}

// Wi-Fi 连接设置
void wifi_init_softap(void) {
    esp_netif_init();
    esp_event_loop_create_default();

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
             .password = WIFI_PASS,
            .ssid_len = strlen(WIFI_SSID),
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = MAX_STA_CONN,
           
        },
    };

    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();

  //  ESP_LOGI(TAG, "AP SSID:%s password:%s", WIFI_SSID, WIFI_PASS);
}

// 初始化系统
extern "C"  void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化Wi-Fi热点
    wifi_init_softap();

    // 启动 HTTP 服务器
    httpd_handle_t server = start_webserver();

    // 等待设备连接，或者停止服务器
    while (true) {
        vTaskDelay(100);
    }

    stop_webserver(server);
}
