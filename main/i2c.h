#ifndef I2C_H
#define I2C_H

/**
 * @file i2c.htmlinclude 
 * @author xfp23 (github.com/xfp23)
 * @brief 初始化i2c总线
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "driver/i2c_master.h"
#ifdef __cplusplus
extern "C" {
#endif

#define __IDF_I2C 0x2002
 
void idf_i2c_init(void);

extern i2c_master_bus_handle_t i2c_bus_handle;
#ifdef __cplusplus
}
#endif
#endif