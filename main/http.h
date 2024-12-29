#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_netif.h"
class HTTP_Server
{

public:
    HTTP_Server();
    ~HTTP_Server();
// //void begin(const char *ssid,const char *passward); // STA模式

void begin(const char *ssid = "ESP32_AP",const char * password = "123456789",uint8_t connect_max = 4); // AP模式
//// void OpenHtml(const char *load,size_t html_size); // 打开html
char * getIP(); // 获取IP地址
void OpenHtml(const char * load);
// void update();
private:
uint8_t connect_num = 4; // 支持的连接数量
char *HTML;
size_t Html_size = 0;
httpd_handle_t server = nullptr;
static const char error_404[20];
virtual esp_err_t http_handle(httpd_req_t *req);
httpd_uri_t uri;
static esp_err_t static_http_handle(httpd_req_t *req);
// esp_err_t HTTP_Server::get_health_data_handler(httpd_req_t *req);
// esp_err_t (*handle)(httpd_req_t *req) = http_handle;
// httpd_req_t *Req;
};

#endif // !HTTP_H