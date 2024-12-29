
/**
 * @file file System.cpp
 * @brief 项目主逻辑
 *
 */

/**
 * @project include
 *
 */
#include "System.h"
#include "timer.h"
#include "MAX30102.h"
#include "oled.h"
#include "http.h"
#include "NTC.h"

/**
 * @brief 外部引用
 *
 */
extern const char *html_content;

/**
 * @brief 实例声明
 *
 */
System_Class device;
Timer_Class sys_timer;
MAX30102_Class max30102;
HTTP_Server http;
NTC_Class NTC;

volatile System_timeflag_t System_Class::timeFlag = {0};

bool SystemTimerCallback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    static uint8_t system_count1ms = 0;
    static uint8_t system_count10ms = 0;
    static uint8_t system_count100ms = 0;
    static uint16_t system_count500ms = 0;
    static uint16_t system_count1000ms = 0;

    if (++system_count1ms > 1)
    {
        System_Class::timeFlag.system_1ms = ON;
        system_count1ms = 0;
    }
    if (++system_count10ms > 10)
    {
        System_Class::timeFlag.system_10ms = ON;
        system_count10ms = 0;
    }
    if (++system_count100ms > 100)
    {
        System_Class::timeFlag.system_100ms = ON;
        system_count100ms = 0;
    }
    if (++system_count500ms > 500)
    {
        System_Class::timeFlag.system_500ms = ON;
        system_count500ms = 0;
    }
    if (++system_count1000ms > 1000)
    {
        System_Class::timeFlag.system_1000ms = ON;
        system_count1000ms = 0;
    }
    return true;
}

/**
 * @brief 系统初始化
 *
 */
void System_Class::begin()
{
    idf_i2c_init();
    http.begin("Smart-Medical", "123456789", 4);
    http.OpenHtml(html_content);
    max30102.begin(&i2c_bus_handle);
    max30102.setMode(SPO2_HEART);
    max30102.setSlotLed(IR1_RED2);
    max30102.setSlotLed(RED_3_IR_4);
    OLED_Init();
    // OLED_ON();
    NTC.begin(ADC_UNIT_2, ADC_CHANNEL_0); // gpio4
    sys_timer.RegisterAlarm(SystemTimerCallback);
    sys_timer.begin();
}

void System_Class::DealWith_max3010x()
{
    max30102.Read_HeartRAte_Spo2((float &)Sensor.Heart_Rate, (float &)Sensor.Spo2);
    Sensor.Body_temp = max30102.Read_Temp();
}

void System_Class::DealWith_NTC()
{
    Sensor.Ambient_temp = NTC.Read_Temp();
}

void System_Class::DealWith_oled()
{
    static bool fill_screen = false;
    char Ambient[] = {"Amb T : %2.2f"};
    char Ambient_buffer[sizeof(Ambient) + 5] = {0};
    char Heart[] =   {"HR    : %3.2f"};
    char Heart_buffer[sizeof(Heart) + 6] = {0};
    char Spo2[] =    {"Spo2  : %3.2f"};
    char Spo2_buffer[sizeof(Spo2) + 6] = {0};
    char Body_T[] =  {"BODY T: %2.2f"};
    char Body_T_buffer[sizeof(Body_T) + 5] = {0};
    char IP[18] =    {"IP: "};
    char *net = http.getIP();
    strcat(IP,net);
    if (!fill_screen)
    {
        OLED_ON();
        OLED_Fill(0x00);
        free(net);
        Display_Str(0,7,IP);
        fill_screen = true;
    }
    snprintf(Ambient_buffer, strlen(Ambient), Ambient, Sensor.Ambient_temp);
    Display_Str(0, 0, Ambient_buffer);
    snprintf(Heart_buffer, strlen(Heart), Heart, Sensor.Heart_Rate);
    Display_Str(0, 4, Heart_buffer);
    snprintf(Spo2_buffer, strlen(Spo2), Spo2, Sensor.Spo2);
    Display_Str(0, 3, Spo2_buffer);
    snprintf(Body_T_buffer, strlen(Body_T), Body_T, Sensor.Body_temp);
    Display_Str(0, 1, Body_T_buffer);
}

// void  System_Class::DealWith_http()
// {
//     http.update();
// }