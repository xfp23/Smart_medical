#include "MAX30102.h"
#include "driver/gpio.h"



const uint8_t MAX30102_Class::__Config[20] = {
    (uint8_t)INTERRUPT_ENABLE_1, // 中断使能寄存器1
    INTER_A_FULL_DISABLE | INTER_ALC_OVF_DISABLE | INTER_PPG_RDY_DISABLE,
    (uint8_t)INTERRUPT_ENABLE_2, // 中断使能寄存器2
    INTER_TEMP_RDY_EN,
    (uint8_t)FIFO_Configuration, // 寄存器地址
    SAMPLE_AVG_2 | FIFO_ROLLOVER_EN | FIFO_ALMOST_FULL_7,
    (uint8_t)MODE_CONFIG,
    LOW_POWER_MODE_DISENABLE | RESET_DIS | HEARTRATE_SPO2,
    SPO2_CONFIG,
    SENSOR_SENSITIVITY_MEDIUM | RATE_5X | ADC_18BIT,
    (uint8_t)LED_PULSE_AMPLITUDE_LED1PA,
    LED_CURRENT_8X,
    (uint8_t)LED_PULSE_AMPLITUDE_LED2PA,
    LED_CURRENT_8X,
    (uint8_t)MULTI_LED_MODE_CONTROL_SLOT1_2,
    TURNOFF_LED_SLOT1_3 | TURNOFF_LED_SLOT2_4 /* |TURNON_LEDIR_PA_SLOT1_3*/,
    (uint8_t)MULTI_LED_MODE_CONTROL_SLOT3_4,
    TURNOFF_LED_SLOT1_3 | TURNOFF_LED_SLOT2_4 /*| TURNON_LEDIR_PA_SLOT2_4*/,
    (uint8_t)DIE_TEMP_CONFIG,
    TEMP_DISABLE};

volatile bool MAX30102_Class::ReadFlag = false;

MAX30102_Class::MAX30102_Class()
{
    dev_handle = new i2c_master_dev_handle_t();
}

MAX30102_Class::~MAX30102_Class()
{
    i2c_master_bus_rm_device(*dev_handle); // 移除I2C设备
    delete dev_handle;
}

void MAX30102_Class::Init_i2c()
{

    i2c_device_config_t i2c_salve_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MAX30102_ADDR >> 1,
        .scl_speed_hz = __I2C_Speed,
        .scl_wait_us = 500000,
        .flags = {.disable_ack_check = false},
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &i2c_salve_conf, dev_handle));
    // printf("i2c init success\n");
}

void MAX30102_Class::begin(gpio_num_t intr, i2c_master_bus_handle_t *i2c_bus, uint32_t i2c_speed )
{
    __INTR = intr;
    bus_handle = i2c_bus;
    __I2C_Speed = i2c_speed;
    Init_i2c();
    for (int i = 0; i < 20; i += 2)
    {
        i2c_master_transmit(*dev_handle, MAX30102_Class::__Config + i, 2, TIMEOUT);
    }
    // i2c_master_transmit(*dev_handle,MAX30102_Class::__Config,16,TIMEOUT); // 写入寄存器
    init_intr();
}


void  MAX30102_Class::begin(i2c_master_bus_handle_t * i2c_bus,uint32_t i2c_speed)
{
    bus_handle = i2c_bus;
    __I2C_Speed = i2c_speed;
    Init_i2c();
    for (int i = 0; i < 20; i += 2)
    {
        i2c_master_transmit(*dev_handle, MAX30102_Class::__Config + i, 2, TIMEOUT);
    }
}
// /**
//  * @brief 向寄存器写入值
//  *
//  * @param data
//  */
// void MAX30102_Class::WRDATA(uint8_t reg, uint8_t byte)
// {
//     uint8_t data[2] = {reg, byte};
//     i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
// }

/**
 * @brief 读取心率
 *
 * @return float
 */
float MAX30102_Class::Read_HeartRate()
{
    // 测心率读取后三个字节
    uint32_t ADC_Value = 0;
    float HeartRate = 0.0f;
    ReadFlag = false;
    uint8_t Register = FIFO_Data_Register;
    uint8_t data[6] = {0};

    i2c_master_transmit_receive(*dev_handle,&Register,1,data,6,TIMEOUT);
    printf("HeratRate data: %x %x %x\n",data[3],data[4],data[5]);
    ADC_Value = (uint32_t)data[3] << 16 | (uint16_t)data[4] << 8 | data[5];
    ADC_Value &= 0x3FFFF; // 清空无效位
    HeartRate = calculate_heart_rate(ADC_Value);
    printf("HeartRate %.2f\n",HeartRate);
    return HeartRate;
}

float MAX30102_Class::Read_Spo2()
{
    uint32_t ADC_Value = 0;
    float Spo2 = 0.0f; // 血氧
    ReadFlag = false;
    uint8_t data[6] = {0};
    uint8_t Register = FIFO_Data_Register;
    i2c_master_transmit_receive(*dev_handle,&Register,1,data,6,TIMEOUT);
     printf("Spo2 data: %x %x %x\n",data[0],data[1],data[2]);
    ADC_Value = (uint32_t)data[0] << 16 | (uint16_t)data[1] << 8 | data[2];
    ADC_Value &= 0x3FFFF;
    Spo2 =calculate_spo2(ADC_Value);
    printf("Spo2 : %.2f\n",Spo2);
    return Spo2;
}

float MAX30102_Class::Read_Temp()
{
    uint8_t WriteData[2] = {DIE_TEMP_CONFIG, TEMP_EN};
    uint8_t ReadData[2] = {DIE_TEMP_INTEGER, DIE_TEMP_FRACTION};
    uint8_t Tempint = 0;
    uint8_t Tempfloat = 0;
    float result = 0.0f;
    // ReadFlag = false;
    // if (!__TempFlag)
    // {
    //     uint8_t WriteData[2] = {DIE_TEMP_CONFIG, TEMP_EN};
    //     __TempFlag = true;
    //     i2c_master_transmit(*dev_handle, WriteData, 2, TIMEOUT);
    // }
    // i2c_master_transmit(*dev_handle,&ReadData[1],1,TIMEOUT);
    // i2c_master_receive(*dev_handle,&Tempint,1,TIMEOUT);
    // i2c_master_transmit(*dev_handle,&ReadData[0],1,TIMEOUT);
    // i2c_master_receive(*dev_handle,&Tempfloat,1,TIMEOUT);
    i2c_master_transmit(*dev_handle,WriteData,2,TIMEOUT);
    i2c_master_transmit_receive(*dev_handle, &ReadData[1], 1, &Tempint, 1, TIMEOUT);
    i2c_master_transmit_receive(*dev_handle, &ReadData[2], 1, &Tempfloat, 1, TIMEOUT);
    Tempfloat &= 0x07; // 清空无效位
    printf("Temp int data : %x\n",Tempint);
    printf("Temp float data : %x\n",Tempfloat);
    if (Tempint > 0x80)
    {
        result = 0x100 - Tempint;
    }
    else
    {
        result += Tempint;
    }
    // printf("temp int : %d\n",Tempint);
    result += Tempfloat * 0.0625;
    printf("Temp : %.2f\n",result);
    return result;
}

/**
 * @brief 读取心率和血氧
 *
 */
void MAX30102_Class::Read_HeartRAte_Spo2(float &heartRate, float &oxygen)
{
    uint8_t Register = FIFO_Data_Register;  // 寄存器
    uint8_t data[6] = {0};
    uint32_t heart_ADC = 0, oxygen_ADC = 0;
    
    // 读取数据
    i2c_master_transmit_receive(*dev_handle, &Register, 1, data, 6, TIMEOUT);
    
    // 获取ADC值
    heart_ADC = (uint32_t)data[0] << 16 | (uint16_t)data[1] << 8 | data[2];
    oxygen_ADC = (uint32_t)data[3] << 16 | (uint16_t)data[4] << 8 | data[5];
    
    // 限制ADC范围
    heart_ADC &= 0x3FFFF;
    oxygen_ADC &= 0x3FFFF;
    heartRate = calculate_heart_rate(heart_ADC);
    oxygen =  calculate_spo2(oxygen_ADC);
    
}

float MAX30102_Class::calculate_heart_rate(uint32_t heart_ADC) {
    // 假设我们有一个信号周期的估计值，这通常通过快速傅里叶变换 (FFT) 或其他方法得到
    // 这里只做简化处理，假设心率信号的周期为固定值或通过采样周期推算

    // 模拟从心跳信号中提取的 AC 信号部分（假设已知处理）
    float heart_rate_frequency = (float) heart_ADC / 100000.0;  // 模拟的频率计算

    // 计算心率，假设采样频率为50Hz，信号频率为 heart_rate_frequency
    float heart_rate = heart_rate_frequency * 60; // 转化为每分钟的次数 (BPM)

    return heart_rate;
}

// 计算血氧的函数
float MAX30102_Class::calculate_spo2(uint32_t adc_value) {
    const uint32_t max_adc_value = 262143;  // 18-bit ADC最大值
    float percentage = (static_cast<float>(adc_value) / max_adc_value) * 100;
    return percentage;
}

void MAX30102_Class::init_intr()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << __INTR,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE, // 下降沿触发
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0); // 使用默认标志
    gpio_isr_handler_add(__INTR, intr_callback, (void *)__INTR);
}

void MAX30102_Class::intr_callback(void *arg)
{
    ReadFlag = true;
}

bool MAX30102_Class::check()
{
    if (ReadFlag == true)
    {
        return true;
    }
    return false;
}

/**
 * @brief 设置时间槽的灯光
 * @param 配置的时间槽 Slot_Led_t
 *
 */
void MAX30102_Class::setSlotLed(Slot_Led_t channel)
{
    uint8_t data[4] = {
        (uint8_t)MULTI_LED_MODE_CONTROL_SLOT1_2,
        (uint8_t)OFF_ALL,
        (uint8_t)MULTI_LED_MODE_CONTROL_SLOT3_4,
        (uint8_t)OFF_ALL,
    };
    switch (channel)
    {
    case OFF_ALL:
        i2c_master_transmit(*dev_handle, data, 4, TIMEOUT);
        break;
    case OFF_1_2:
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;

    case OFF_3_4:
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;
    case REDLED_1_2:
        data[1] = (uint8_t)REDLED_1_2;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;
    case REDLED_3_4:
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;

    case IRLED_1_2:
        data[1] = (uint8_t)IRLED_1_2;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;

    case IRLED_3_4:
        data[3] = (uint8_t)IRLED_3_4;
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;
    case IRLED_ALL:
        data[1] = (uint8_t)IRLED_1_2;
        data[3] = (uint8_t)IRLED_1_2;
        i2c_master_transmit(*dev_handle, data, 4, TIMEOUT);
        break;

    case REDLED_ALL:
        data[1] = (uint8_t)REDLED_1_2;
        data[3] = (uint8_t)REDLED_1_2;
        i2c_master_transmit(*dev_handle, data, 4, TIMEOUT);
        break;
    case RED_1_IR_2:
        data[1] = (uint8_t)RED_1_IR_2;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;
    case IR1_RED2:
        data[1] = (uint8_t)IR1_RED2;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;

    case RED_3_IR_4:
        data[3] = (uint8_t)RED_1_IR_2;
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;

    case RED_4_IR_3:
        data[3] = (uint8_t)IR1_RED2;
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;

    case ONLY_RED_1:
        data[1] = (uint8_t)ONLY_RED_1;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;

    case ONLY_RED_2:
        data[1] = (uint8_t)ONLY_RED_2;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;

    case ONLY_RED_3:
        data[3] = (uint8_t)ONLY_RED_1;
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;

    case ONLY_RED_4:
        data[3] = (uint8_t)ONLY_RED_2;
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;

    case ONLY_IR_1:
        data[1] = (uint8_t)ONLY_IR_1;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;

    case ONLY_IR_2:
        data[1] = (uint8_t)ONLY_IR_2;
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;

    case ONLY_IR_3:
        data[3] = (uint8_t)ONLY_IR_1;
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);
        break;

    case ONLY_IR_4:
        data[3] = (uint8_t)ONLY_IR_2;
        i2c_master_transmit(*dev_handle, data + 2, 2, TIMEOUT);

        break;
    }
}

/**
 * @brief 设置工作模式
 *
 */
void MAX30102_Class::setMode(Max3010x_Mode_t Mode)
{
    uint8_t data[2] = {
        MODE_CONFIG,
       0};
    switch (Mode)
    {
    case ONLY_SPO2:
        data[1] = LOW_POWER_MODE_DISENABLE | RESET_DIS | SPO2_MODE;
        break;
    case ONLY_HEART:
        data[1] = LOW_POWER_MODE_DISENABLE | RESET_DIS | HEARTRATE_REDONLY;
        break;

    case SPO2_HEART:
        data[1] = LOW_POWER_MODE_DISENABLE | RESET_DIS | HEARTRATE_SPO2;
        break;
    default:
        break;
    }

    i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
}


