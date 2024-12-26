#ifndef MAX30102_H
#define MAX30102_H

/**
 * @file MAX30102.h
 * @author xfp23 (@github.com/xfp23)
 * @brief MAX30102 传感器驱动头文件
 * @version 0.1
 * @date 2024-12-26
 *
 * @copyright Copyright (c) 2024
 *
 * @note 本文件包含了 MAX30102 传感器的初始化、配置和数据读取等功能接口定义。
 *       提供了与 MAX30102 进行 I2C 通信的必要功能，支持心率监测和血氧饱和度等功能。
 */

#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include <stdint.h>
/**
 * 中断1寄存器映射： B7: A_FULL B6:PPG_RDY   B5:ALC_OVF
 * A_FULL :在 SpO2 和 HR 模式下，当 FIFO 写指针剩余一定数量的可用空间时，会触发此中断。
 * 触发数量可以通过 FIFO_A_FULL[3:0] 寄存器设置。通过读取中断状态清除中断1 个寄存器（0x00）。
 *
 * PPG_RDY : 在 SpO2 和 HR 模式下，当数据 FIFO 中存在新样本时，会触发该中断。中断被清除
 * 读取中断状态 1 寄存器 (0x00)，或读取 FIFO_DATA 寄存器
 *
 * ALC_OVF 当 SpO2/HR 光电二极管的环境光消除功能达到最大值时，会触发此中断
 * 限制，因此环境光会影响 ADC 的输出。通过读取中断来清除中断
 * 状态 1 寄存器 (0x00)。
 *
 * 上电时或掉电情况后，当电源电压 VDD 从欠压锁定以下转变时
 * (UVLO) 电压高于 UVLO 电压，将触发上电就绪中断以发出模块已上电的信号
 * 并准备收集数据
 *
 */

/**
 * 中断寄存器2映射： B1 ： DIE_TEMP_RDY
 * 当内部芯片温度转换完成时，会触发此中断，以便处理器可以读取温度
 * 数据寄存器。通过读取中断状态 2 寄存器 (0x01) 或 TFRAC 寄存器来清除中断
 * （0x20）。
 */

/**
 * 中断寄存器描述：
 * 除电源就绪外，每个硬件中断源都可以在软件寄存器中禁用
MAX30102 IC。上电就绪中断无法禁用，因为模块的数字状态在
掉电条件（低电源电压），默认条件是所有中断都被禁用。还有，它是
对于系统来说重要的是要知道发生了掉电情况，并且模块内的数据被重置为
结果。
 * 为了正常操作，未使用的位应始终设置为零。
 *
 */

/**
 * @brief 读取的数据
 * 18位的ADC值，有效位0~17
 *
 */
// #define MAX30102_ADDR 0xAE
// 每当触发中断时，MAX30102将低电平有效中断引脚拉至低电平状态，直到中断被触发。 中断被清除后，引脚将恢复高电平已清除。
typedef enum
{
  INTERRUPT_STATUS_1 = 0X00,             // 中断状态寄存器1，B7: A_FULL, B6: PPG_RDY, B5: ALC_OVF
                                         // 触发条件包括FIFO满、PPG数据准备好、环境光消除溢出
  INTERRUPT_STATUS_2 = 0X01,             // 中断状态寄存器2，B1: DIE_TEMP_RDY
                                         // 触发条件为芯片内部温度转换完成
  INTERRUPT_ENABLE_1 = 0X02,             // 中断使能寄存器1，用来启用或禁用特定的中断源
  INTERRUPT_ENABLE_2 = 0X03,             // 中断使能寄存器2，用来启用或禁用温度中断
  FIFO_WRITE_POINTER = 0X04,             // FIFO写指针寄存器，指示下一次数据写入的位置
  OVERFLOW_COUNTER = 0X05,               // FIFO溢出计数器，当FIFO溢出时增加
  FIFO_READ_POINTER = 0X06,              // FIFO读指针寄存器，指示下一次数据读取的位置
  FIFO_Data_Register = 0X07,             // FIFO数据寄存器，存储最新的传感器数据（如PPG数据）
  FIFO_Configuration = 0X08,             // FIFO配置寄存器，控制FIFO的行为（如FIFO满时的中断条件等）
  MODE_CONFIG = 0x09,                    // 工作模式配置寄存器，设置传感器的工作模式（如心率模式、SpO2模式等）
  SPO2_CONFIG = 0x0A,                    // SpO2配置寄存器，用于设置SpO2测量的灵敏度和采样率
  LED_PULSE_AMPLITUDE_LED1PA = 0X0C,     // LED1的脉冲幅度（LED驱动电流的强度）
  LED_PULSE_AMPLITUDE_LED2PA = 0X0D,     // LED2的脉冲幅度（LED驱动电流的强度）
  MULTI_LED_MODE_CONTROL_SLOT1_3 = 0x11, // 多LED模式控制寄存器1和2，用于控制LED的开启/关闭
  // MULTI_LED_MODE_CONTROL_SLOT2 = 0x11, // 多LED模式控制寄存器2，用于控制LED的开启/关闭
  MULTI_LED_MODE_CONTROL_SLOT2_4 = 0x12, // 多LED模式控制寄存器3和4，用于控制LED的开启/关闭
  // MULTI_LED_MODE_CONTROL_SLOT4 = 0x12, // 多LED模式控制寄存器4，用于控制LED的开启/关闭
  DIE_TEMP_INTEGER = 0x1F,  // 温度寄存器整数部分，用于读取芯片的温度
  DIE_TEMP_FRACTION = 0X20, // 温度寄存器小数部分，用于读取芯片的温度
  DIE_TEMP_CONFIG = 0X21,   // 温度配置寄存器，用于启用或禁用温度传感器
  REVISION_ID = 0XFE,       // 修订ID寄存器，用于获取芯片的修订版本
  PART_ID = 0XFF,           // 部件ID寄存器，用于获取芯片的型号和ID
} MAX30102_RegAddr_t;

/**
 * @brief 以下是配置寄存器(FIFO_Configuration)的位配置
 *
 */

// 采样平均设置
#define SAMPLE_AVG_0 (uint8_t)(0x00 << 5)
#define SAMPLE_AVG_2 (uint8_t)(0x01 << 5)
#define SAMPLE_AVG_4 (uint8_t)(0x02 << 5)
#define SAMPLE_AVG_8 (uint8_t)(0x03 << 5)
#define SAMPLE_AVG_16 (uint8_t)(0x04 << 5)
#define SAMPLE_AVG_32 (uint8_t)(0x05 << 5)

// 设置是否用新值覆盖旧值
#define FIFO_ROLLOVER_EN (uint8_t)(0x01 << 4)
#define FIFO_ROLLOVER_DISENABLE (uint8_t)(0x00 << 4)

// 设置剩余队列空间数量触发中断
#define FIFO_ALMOST_FULL_0 0x0  // 0 empty, 32 unread data samples in FIFO
#define FIFO_ALMOST_FULL_1 0x1  // 1 empty, 31 unread data samples in FIFO
#define FIFO_ALMOST_FULL_2 0x2  // 2 empty, 30 unread data samples in FIFO
#define FIFO_ALMOST_FULL_3 0x3  // 3 empty, 29 unread data samples in FIFO
#define FIFO_ALMOST_FULL_4 0x4  // 4 empty, 28 unread data samples in FIFO
#define FIFO_ALMOST_FULL_5 0x5  // 5 empty, 27 unread data samples in FIFO
#define FIFO_ALMOST_FULL_6 0x6  // 6 empty, 26 unread data samples in FIFO
#define FIFO_ALMOST_FULL_7 0x7  // 7 empty, 25 unread data samples in FIFO
#define FIFO_ALMOST_FULL_8 0x8  // 8 empty, 24 unread data samples in FIFO
#define FIFO_ALMOST_FULL_9 0x9  // 9 empty, 23 unread data samples in FIFO
#define FIFO_ALMOST_FULL_10 0xA // 10 empty, 22 unread data samples in FIFO
#define FIFO_ALMOST_FULL_11 0xB // 11 empty, 21 unread data samples in FIFO
#define FIFO_ALMOST_FULL_12 0xC // 12 empty, 20 unread data samples in FIFO
#define FIFO_ALMOST_FULL_13 0xD // 13 empty, 19 unread data samples in FIFO
#define FIFO_ALMOST_FULL_14 0xE // 14 empty, 18 unread data samples in FIFO
#define FIFO_ALMOST_FULL_15 0xF // 15 empty, 17 unread data samples in FIFO

/**
 * @brief 以下是寄存器(MODE_CONFIG)(0x09)的配置
 *
 */

// 低功耗模式
#define LOW_POWER_MODE_EN (uint8_t)(0x01 << 7)
#define LOW_POWER_MODE_DISENABLE (uint8_t)(0x00 << 7)

//  设备复位 设置 RESET 位不会触发 PWR_RDY 中断事件。
#define RESET_EN (uint8_t)(0x01 << 6)
#define RESET_DIS (uint8_t)(0x00 << 6)

// 设置工作状态
#define NO_USE 0x00            // 不使用
#define HEARTRATE_REDONLY 0x02 // 只通过红光测心率
#define SPO2_MODE 0x03         // 通过红光和红外光测血氧
#define HEARTRATE_SPO2 0x07    // 同时监测心率和血氧饱和度

/**
 * @brief SpO2 Configuration (0x0A)
 *
 */

// SpO2 Configuration (0x0A) 设置传感器 ADC 满量程范围（灵敏度）
#define SENSOR_SENSITIVITY_HIGH (uint8_t)(0x00 << 5)        // 高灵敏度 (LSB: 7.81 pA, Full Scale: 2048 nA)
#define SENSOR_SENSITIVITY_MEDIUM_HIGH (uint8_t)(0x01 << 5) // 中等高灵敏度 (LSB: 15.63 pA, Full Scale: 4096 nA)
#define SENSOR_SENSITIVITY_MEDIUM (uint8_t)(0x02 << 5)      // 中等灵敏度 (LSB: 31.25 pA, Full Scale: 8192 nA)
#define SENSOR_SENSITIVITY_LOW (uint8_t)(0x03 << 5)         // 低灵敏度 (LSB: 62.5 pA, Full Scale: 16384 nA)

// 以下是采样周期的设置 采样速度
#define RATE_1X (uint8_t)(0x00 << 2) // 每秒50
#define RATE_2X (uint8_t)(0x01 << 2) // 每秒100
#define RATE_3X (uint8_t)(0x02 << 2) // 每秒200
#define RATE_4X (uint8_t)(0x03 << 2) // 每秒400
#define RATE_5X (uint8_t)(0x04 << 2) // 每秒800
#define RATE_6X (uint8_t)(0x05 << 2) // 每秒1000
#define RATE_7X (uint8_t)(0x06 << 2) // 每秒1600
#define RATE_8X (uint8_t)(0x07 << 2) // 每秒3200

// ADC 宽度设置

#define ADC_15BIT 0x00
#define ADC_16BIT 0X01
#define ADC_17BIT 0X02
#define ADC_18BIT 0X03D

/**
 * @brief LED Pulse Amplitude (0x0C–0x0D)
 *
 */

// 设置LED的驱动电流 两个LED通用
#define LED_CURRENT_1X 0x00 // 0MA
#define LED_CURRENT_2X 0x01 // 0.2MA
#define LED_CURRENT_3X 0x02 // 0.4MA
#define LED_CURRENT_4X 0x0F // 3.0MA
#define LED_CURRENT_5X 0X1F // 6.2MA
#define LED_CURRENT_6X 0X3F // 12.6MA
#define LED_CURRENT_7X 0X7F // 25.4MA
#define LED_CURRENT_8X 0XFF // 51.0MA 满值

/**
 * @brief Multi-LED Mode Control Registers (0x11–0x12) 2个寄存器通用
 *
 */

// 这是设置SLOT1和SLOT3的
#define NONE_LEN_NOPA_SLOT1_3 0x00    // 不开灯也不设置电流强度
#define TURNON_LEDRED_PA_SLOT1_3 0x01 // 开启红光并设置电流强度
#define TURNON_LEDIR_PA_SLOT1_3 0X02  // 开启红外光并设置电流强度
#define TURNOFF_LED_NOPA_SLOT1_3 0x03 // 关灯

// 这是设置SLOT2和SLOT4的
#define NONE_LEN_NOPA_SLOT2_4 (uint8_t)(0x00 << 4)    // 不开灯也不设置电流强度
#define TURNON_LEDRED_PA_SLOT2_4 (uint8_t)(0x01 << 4) // 开启红光并设置电流强度
#define TURNON_LEDIR_PA_SLOT2_4 (uint8_t)(0X02 << 4)  // 开启红外光并设置电流强度
#define TURNOFF_LED_NOPA_SLOT2_4 (uint8_t)(0x03 << 4) // 关灯

/**
 * @brief Temperature Data (0x1F–0x21)
 *
 */

#define TEMP_EN 0x01      // 启用温度转换
#define TEMP_DISABLE 0x00 // 禁用温度转换 注意 当每完成一次温度转换的时候，传感器会自动清除此位

/**
 * 温度对应表在数据手册22页
 * 温度计算方法： 读取
 * DIE_TEMP_INTEGER = 0x1F,
 * DIE_TEMP_FRACTION =  0X20, 这两个寄存器来获取整数和小数部分，
 * 整数增量 : 1
 * 小数增量 : 0.0625
 * 小数部分除以16 + 整数部分
 * 
 */

/**
 * @brief INTERRUPT_ENABLE_1 (0x02) 中断使能寄存器1配置
 * 
 */
#define INTER_A_FULL_EN (uint8_t)(0x01 << 7) // A_FULL_EN 中断使能
#define INTER_A_FULL_DISABLE (uint8_t)(0x01 << 7) // A_FULL_EN 禁用中断
#define INTER_PPG_RDY_EN (uint8_t)(0X01 << 6) // PPG_RDY_EN 中断使能
#define INTER_PPG_RDY_DISABLE (uint8_t)(0X01 << 6) // PPG_RDY_EN 禁用中断
#define INTER_ALC_OVF_EN (uint8_t) (0X01 << 5) // ALC_OVF_EN 中断使能
#define INTER_ALC_OVF_DISABLE (uint8_t) (0X01 << 5) // ALC_OVF_EN 禁用中断

/**
 * @brief INTERRUPT_ENABLE_2 (0x03) 中断使能寄存器2配置
 * 
 */
#define INTER_TEMP_RDY_EN 0x01 // TEMP_RDY_EN 中断使能
#define INTER_TEMP_RDY_DISABLE 0x00 // TEMP_RDY_EN 禁用中断

typedef enum {
  REDLED_1,
  REDLED_2,
  REDLED_ALL,
}Slot_RedLed_t;
class MAX30102_Class
{
public:
typedef enum {
  INTR_DISABLE, // 禁用中断
  INTR_ENABLE, // 使能中断
}MAX30102_intr_t;


  MAX30102_Class();
  ~MAX30102_Class();
  /**
   * @brief 初始化MAX30102
   *
   * @param sda  MAX30102 SDA
   * @param sclk MAX30102 SCLK
   * @param intr MAX30102 中断
   * @param i2c_bus MAX30102 I2C总线
   * @param i2c_speed MAX30102 I2C速度
   */
  void begin(gpio_num_t intr, i2c_master_bus_handle_t *i2c_bus, uint32_t i2c_speed);
  uint16_t Read_HeartRate(); // 读取心率
  uint16_t Read_Spo2();      // 读取血氧
  bool check();              // 检测是否有数据可读
  void Read_HeartRAte_Spo2(float &heartRate,auto &oxygen);
  float Read_Temp();
  void OpenRedLed(Slot_RedLed_t channel);             // 打开红光
  void CloseRedLed();            // 关闭红光
  void OpenIRLed();               // 打开红外
  void CloseIRLed();               // 关闭红外
private:
  static const uint8_t MAX30102_ADDR = 0X57; // I2C地址
  int TIMEOUT = 100;                    // I2C超时
  static const uint32_t DEF_Speed = 100000;  // 100KHz
  static const uint8_t __Config[20];
  i2c_master_bus_handle_t *bus_handle = nullptr;
  i2c_master_dev_handle_t *dev_handle = nullptr;
  volatile gpio_num_t __INTR; // 中断引脚
  static volatile bool ReadFlag;
  volatile bool __TempFlag = false; // 温度传感器标志
  volatile uint32_t __I2C_Speed = DEF_Speed;
  virtual void Init_i2c(void); // 初始化I2C
  void WRDATA(uint8_t reg, uint8_t byte);
  void init_intr();
  static void intr_callback(void * arg); // 中断回调函数

};
#endif