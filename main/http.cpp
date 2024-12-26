// http.cpp 文件
#include "esp_log.h"
#include "nvs_flash.h"
#include "http.h"
#include <iostream>
using namespace std;

#define TAG "HTTP_SERVER"

const char HTTP_Server::error_404[20] = "ESP32 Http//:: 404";

static esp_err_t data_post_handler(httpd_req_t *req) {
    // 接收数据
    char content[100];
    int ret = httpd_req_recv(req, content, sizeof(content));
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    content[ret] = '\0'; // 确保字符串以 '\0' 结尾
    ESP_LOGI(TAG, "Received POST data: %s", content);

    // 响应客户端
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"success\"}");

    return ESP_OK;
}

HTTP_Server::HTTP_Server()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

HTTP_Server::~HTTP_Server()
{
    if (HTML != nullptr)
    {
        free((void*)HTML);
    }
    httpd_stop(server);
}

void HTTP_Server::begin(const char *ssid, const char *password, uint8_t connect_max)
{
    esp_netif_init();
    esp_event_loop_create_default();

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    if (ap_netif == NULL) {
        ESP_LOGE(TAG, "Failed to create Wi-Fi AP network interface");
        return;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config;
    memcpy(wifi_config.ap.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.ap.password, password, strlen(password));
    wifi_config.ap.ssid_len = strlen(ssid);
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;  // 使用WPA2加密
    wifi_config.ap.max_connection = connect_max;

    if (strlen(password) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN; // 如果密码为空，使用开放网络
    }

    wifi_config.ap.channel = 6; 

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);

    esp_err_t start_err = esp_wifi_start();
    if (start_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(start_err));
        return;
    }

    ESP_LOGI(TAG, "Wi-Fi AP started. SSID: %s, Password: %s", ssid, password);

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;

    uri.uri = "/";
    uri.handler = HTTP_Server::static_http_handle;
    uri.method = HTTP_GET;
    uri.user_ctx = (void*)this; // 将 this 指针传递给 user_ctx

    esp_err_t httpd_start_err = httpd_start(&server, &config);
    if (httpd_start_err != ESP_OK) {
        ESP_LOGE(TAG, "Error starting HTTP server: %s", esp_err_to_name(httpd_start_err));
        return;
    }

    httpd_register_uri_handler(server, &uri);
}


// 静态函数，用于包装成员函数的调用
esp_err_t HTTP_Server::static_http_handle(httpd_req_t *req)
{
    // 通过 req->user_ctx 获取当前实例
    HTTP_Server* instance = (HTTP_Server*)req->user_ctx;
    return instance->http_handle(req);
}

void HTTP_Server::  OpenHtml(const char *load)
{
    Html_size = strlen(load);
    ESP_LOGI(TAG, "Start copy HTML size: %d", Html_size);

    HTML = (char*) calloc(Html_size + 20, sizeof(char));
    if (HTML == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed for HTML");
        return;
    }

    memcpy((void *)HTML, (void*)load, Html_size + 1);
    ESP_LOGI(TAG, "Copy done");
}

esp_err_t HTTP_Server::http_handle(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Sending HTML content");

    // 假设这些是你动态插入的健康数据
    float temp = 22.5;         // 环境温度
    float bodyTemp = 37.0;     // 体温
    float heartRate = 75.0;    // 心率
    float oxygen = 98.0;       // 血氧饱和度

    // 计算最终 HTML 内容的大小，增加空间以适应动态数据
    size_t buffer_size = Html_size + 100;  // 留出额外空间
    char *buffer = (char*)malloc(buffer_size);  // 动态分配内存

    if (buffer == nullptr) {
        ESP_LOGE(TAG, "Memory allocation failed for buffer");
        return ESP_FAIL;
    }

    // 使用 snprintf 格式化 HTML 内容并插入动态值
    int written = snprintf(buffer, buffer_size, this->HTML, temp, bodyTemp, heartRate, oxygen);

    // 检查格式化是否成功，防止缓冲区溢出
    if (written < 0 || written >= buffer_size) {
        ESP_LOGE(TAG, "Error formatting HTML or buffer overflow");
        free(buffer);
        return ESP_FAIL;
    }

    // 发送格式化后的 HTML 内容
    httpd_resp_send(req, buffer, written);

    // 发送完后释放内存
    free(buffer);
    return ESP_OK;
}


