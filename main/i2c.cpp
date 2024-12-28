#include "i2c.h"
#include "esp_log.h"

i2c_master_bus_handle_t  i2c_bus_handle;
void idf_i2c_init()
{
        i2c_master_bus_config_t i2c_master_conf = {
        .i2c_port = 0,
        .sda_io_num = GPIO_NUM_26,
        .scl_io_num = GPIO_NUM_27,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .flags = {.enable_internal_pullup = true,},
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_conf,&i2c_bus_handle));
    // printf("idf i2c bus init\n");
}