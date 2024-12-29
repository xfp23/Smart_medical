/**
 * @file OLED12864.h
 * @author 早上坏 (star32349@outlook.com)
 * @brief 
 * @version 1.0
 * @date 2024-05-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * @file oled.h
 * @author xfp23 (@github.com/xfp23)
 * @brief 将原作者的STM32库移植到ESP-IDF
 * @version 0.1
 * @date 2024-12-29
 * 
 * @copyright Copyright (c) 2024 xfp23
 */
#ifndef __OLED12864_H
#define __OLED12864_H

// #include "main.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define OLED_ADD 0x78  /*!< OLED的I2C地址（禁止修改）*/
#define OLED_COM 0x00  /*!< OLED 指令（禁止修改）*/
#define OLED_DAT 0x40  /*!< OLED 数据（禁止修改）*/
#define OLED_WIDTH 128 /*!< OLED像素宽度 */
#define OLED_HEIGHT 64 /*!< OLED像素高度 */



void OLED_Init(void);
void WriteCmd(uint8_t IIC_Command);
void WriteDat(uint8_t IIC_Data);
void HAL_I2C_WriteByte(uint8_t addr, uint8_t data);
void SetStart(uint8_t x, uint8_t y);

void OLED_Fill(uint8_t Fill_Data);
void OLED_ON(void);
void OLED_OFF(void);
void Display_Char(uint8_t x, uint8_t y, char c);
void Display_Str(uint8_t x, uint8_t y, char *str);
void Display_IMG(const uint8_t (*img)[128],uint8_t mode);
void OLED_IMGtest(void);
void OLED_CHARtest(void);


#ifdef __cplusplus
}
#endif
#endif