#ifndef TIMER_H
#define TIMER_H

/**
 * @file timer.h
 * @author xfp23 @github.com/xfp23
 * @brief esp 定时器驱动
 * @version 0.1
 * @date 2024-12-28
 * @note 编译此驱动库请包含 esp_timer组件
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "esp_timer.h"
#include "driver/gptimer_types.h"
#include "driver/gptimer.h"

typedef bool (*Timer_Alarm)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);
class Timer_Class
{

public:
    Timer_Class();
    ~Timer_Class();
    void begin(uint32_t hz = 40000000,uint8_t intr = 2,uint32_t alarm  = 40000,uint32_t reload = 0);
    void RegisterAlarm(Timer_Alarm fun); // 注册定时器回调函数
    void start(); // 启动定时器
    void stop(); // 停止定时器

private:
    Timer_Alarm Timer_Alarm_Fun = NULL;
    gptimer_handle_t gptimer_handle = NULL; // 定时器句柄
    gptimer_event_callbacks_t *cbs;
};

#endif