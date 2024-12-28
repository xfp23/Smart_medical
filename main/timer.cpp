#include "timer.h"
#include "esp_log.h"
Timer_Class::Timer_Class() 
{
    cbs = new gptimer_event_callbacks_t();  // 或者其他方式初始化 cbs
}

Timer_Class::~Timer_Class()
{
    delete cbs;  // 释放内存
}


/**
 * @brief 初始化定时器
 * 
 * @param hz 定时器目标频率
 * @param intr 中断优先级
 * @param alarm 报警值
 * @param reload 重载值
 * @note 默认1ms触发
 */
void Timer_Class::begin(uint32_t hz ,uint8_t intr ,uint32_t alarm ,uint32_t reload )
{
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = hz,
        .intr_priority = intr,
        .flags = {.intr_shared = false},
    };

    // 初始化定时器
    esp_err_t err = gptimer_new_timer(&config, &gptimer_handle);
    if (err != ESP_OK) {
        // 处理定时器创建失败
        printf("Failed to create GPTimer\n");
        return;
    }

    // 配置报警
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = alarm,
        .reload_count = reload,
        .flags = {.auto_reload_on_alarm = true},
    };

    // 注册回调函数（确保定时器初始化完成）
    if (gptimer_handle != NULL) {
        gptimer_register_event_callbacks(gptimer_handle, cbs, NULL);  // 确保定时器处于初始化状态
        gptimer_set_alarm_action(gptimer_handle, &alarm_config);
        gptimer_enable(gptimer_handle); // 启用定时器
        gptimer_start(gptimer_handle);  // 启动定时器
    } else {
        // 处理 gptimer_handle 为空的情况
    }
}

void Timer_Class::RegisterAlarm(Timer_Alarm fun)
{
    if (cbs == NULL) {
        // 确保 cbs 被正确初始化
        cbs = new gptimer_event_callbacks_t();
    }
    cbs->on_alarm = fun;
    if (gptimer_handle != NULL) {
        gptimer_register_event_callbacks(gptimer_handle, cbs, NULL);
    }
}



void Timer_Class::start()
{
    gptimer_start(gptimer_handle);
}

void Timer_Class::stop()
{
    gptimer_stop(gptimer_handle);
}