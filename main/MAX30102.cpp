#include "MAX30102.h"
#include "driver/gpio.h"
#define TIMEOUT 100

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;
#define SCLK_Pin GPIO_NUM_10
#define SDA_Pin GPIO_NUM_11

const uint8_t Reg_data[16] = {
    (uint8_t)FIFO_Configuration, // 寄存器地址
    SAMPLE_AVG_2 | FIFO_ROLLOVER_EN | FIFO_ALMOST_FULL_7,
    (uint8_t)MODE_CONFIG,
    LOW_POWER_MODE_DISENABLE | RESET_DIS | HEARTRATE_SPO2,
    (uint8_t)SPO2_CONFIG,
    SENSOR_SENSITIVITY_MEDIUM | RATE_1X | ADC_18BIT,
    (uint8_t)LED_PULSE_AMPLITUDE_LED1PA,
    LED_CURRENT_3X,
    (uint8_t)LED_PULSE_AMPLITUDE_LED2PA,
    LED_CURRENT_3X,
    (uint8_t)MULTI_LED_MODE_CONTROL_SLOT1_3,
    TURNON_LEDRED_PA_SLOT1_3 | TURNON_LEDIR_PA_SLOT1_3,
    (uint8_t)MULTI_LED_MODE_CONTROL_SLOT2_4,
    TURNON_LEDRED_PA_SLOT2_4 | TURNON_LEDIR_PA_SLOT2_4,
    (uint8_t)DIE_TEMP_CONFIG,
    TEMP_DISABLE};

void Init_i2c()
{
    i2c_master_bus_config_t i2c_master_conf = {
        .i2c_port = 0,
        .sda_io_num = SDA_Pin,
        .scl_io_num = SCLK_Pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .flags = {.enable_internal_pullup = true,},
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_conf,&bus_handle));
    i2c_device_config_t i2c_salve_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MAX30102_ADDR,
        .scl_speed_hz = 100000,
        .flags = {.disable_ack_check = 1 },
    };
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &i2c_salve_conf, &dev_handle));
}


/**
 * @brief 向寄存器写入值
 * 
 * @param data 
 */
void WRDATA(uint8_t reg,uint8_t byte)
{
    uint8_t data[2] = {reg,byte};
    i2c_master_transmit(dev_handle,data,2,TIMEOUT);
}
