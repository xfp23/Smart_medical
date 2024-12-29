#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "esp_task_wdt.h"

#define ON (1)
#define OFF (0)

typedef struct
{
    uint8_t system_1ms : 1;
    uint8_t system_10ms : 1;
    uint8_t system_100ms : 1;
    uint8_t system_500ms : 1;
    uint8_t system_1000ms : 1;
    uint8_t Receive_bits : 3; // 保留
} System_timeflag_t;

typedef struct {
    float Ambient_temp; // 外界环境温度
    float Body_temp; // 人体体温
    float Heart_Rate; // 人体心率
    float Spo2;       // 人体血氧
}System_sensor_t;

class System_Class
{
public:
    volatile static System_timeflag_t timeFlag;
    void begin();
    void DealWith_oled();     // 处理oled
    void DealWith_max3010x(); // 处理MAX30102
    void DealWith_NTC();      // 处理NTC
    // void DealWith_http();
    volatile System_sensor_t Sensor = {0,0,0,0};
private:
    
    
};


extern System_Class device;
#endif