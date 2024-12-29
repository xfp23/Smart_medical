#ifndef NTC_H
#define NTC_H

/**
 * @file NTC.h
 * @author xfp23
 * @brief NTC 热敏电阻驱动库
 * @version 0.1
 * @date 2024-12-29
 * 
 * @copyright Copyright (c) 2024
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */


#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"
#include <stdint.h>

#define Vref  3.3            					  // ADC 基准电压
#define  ADC_MAX (4095)        	   // ADC 最大值（12 位）
#define  R_fixed  (10000)    		      // 分压电路中的固定电阻，单位：欧姆
#define  B_value (3435)       		   // 热敏电阻的 B 值，单位：K
#define  R25  (10000)           		  // 热敏电阻在 25℃ 时的阻值，单位：欧姆
#define  T0  (273.15f + 25)     		   // 25℃ 转换为开尔文温标

class NTC_Class {

public:
NTC_Class();
~NTC_Class();
void begin(adc_unit_t adc_id,adc_channel_t adc_channel);
float Read_Temp(); // 读取温度
uint16_t Read_ADC(); // 读取AD值
private:
adc_channel_t __Channel;
adc_oneshot_unit_handle_t *adc_handle;
float calculateADC(uint16_t adc_value); // 根据ADC计算出温度
};

#endif
