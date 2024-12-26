#include "MAX30102.h"
#include "driver/gpio.h"

// i2c_master_bus_handle_t bus_handle;
// i2c_master_dev_handle_t dev_handle;
// #define SCLK_Pin GPIO_NUM_4
// #define SDA_Pin GPIO_NUM_0

 const uint8_t  MAX30102_Class::__Config[20] = {
    (uint8_t)INTERRUPT_ENABLE_1, // 中断使能寄存器1
    INTER_A_FULL_DISABLE | INTER_ALC_OVF_DISABLE | INTER_PPG_RDY_DISABLE,
    (uint8_t)INTERRUPT_ENABLE_2, // 中断使能寄存器2
    INTER_TEMP_RDY_EN,
    (uint8_t)FIFO_Configuration, // 寄存器地址
    SAMPLE_AVG_2 | FIFO_ROLLOVER_EN | FIFO_ALMOST_FULL_7,
    (uint8_t)MODE_CONFIG,
    LOW_POWER_MODE_DISENABLE | RESET_DIS | HEARTRATE_SPO2,
    SPO2_CONFIG,
    SENSOR_SENSITIVITY_MEDIUM | RATE_1X | ADC_18BIT,
    (uint8_t)LED_PULSE_AMPLITUDE_LED1PA,
    LED_CURRENT_3X,
    (uint8_t)LED_PULSE_AMPLITUDE_LED2PA,
    LED_CURRENT_3X,
    (uint8_t)MULTI_LED_MODE_CONTROL_SLOT1_3,
    TURNOFF_LED_NOPA_SLOT1_3/* |TURNON_LEDIR_PA_SLOT1_3*/ ,
    (uint8_t)MULTI_LED_MODE_CONTROL_SLOT2_4,
    TURNOFF_LED_NOPA_SLOT2_4 /*| TURNON_LEDIR_PA_SLOT2_4*/,
    (uint8_t)DIE_TEMP_CONFIG,
    TEMP_DISABLE};

volatile bool MAX30102_Class::ReadFlag = false;

MAX30102_Class::MAX30102_Class()
{
    dev_handle = new i2c_master_dev_handle_t();

}

MAX30102_Class::~MAX30102_Class()
{
    delete dev_handle;
}

 void MAX30102_Class::Init_i2c()
{

    i2c_device_config_t i2c_salve_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MAX30102_ADDR,
        .scl_speed_hz = __I2C_Speed,
        .scl_wait_us = 500000,
        .flags = {.disable_ack_check = true },
    };
ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &i2c_salve_conf, dev_handle));
printf("i2c init success\n");
}


void MAX30102_Class::begin(gpio_num_t intr,i2c_master_bus_handle_t *i2c_bus,uint32_t i2c_speed = DEF_Speed)
{
    __INTR = intr;
    bus_handle = i2c_bus;
    __I2C_Speed = i2c_speed;
    Init_i2c();
    for(int i = 0; i < 20; i+= 2)
    {
        i2c_master_transmit(*dev_handle,MAX30102_Class::__Config + i,2,TIMEOUT);
    }
   //i2c_master_transmit(*dev_handle,MAX30102_Class::__Config,16,TIMEOUT); // 写入寄存器
   init_intr();
}

/**
 * @brief 向寄存器写入值
 * 
 * @param data 
 */
void MAX30102_Class::WRDATA(uint8_t reg,uint8_t byte)
{
    uint8_t data[2] = {reg,byte};
    i2c_master_transmit(*dev_handle,data,2,TIMEOUT);
}

/**
 * @brief 读取心率
 * 
 * @return uint16_t 
 */
uint16_t MAX30102_Class::Read_HeartRate()
{

    ReadFlag = false;
    return 0;
}

uint16_t MAX30102_Class::Read_Spo2()
{
    uint16_t data = 0;
    ReadFlag = false;

    return data;
}

float MAX30102_Class::Read_Temp()
{
    
    uint8_t ReadData[2] = {DIE_TEMP_INTEGER,DIE_TEMP_FRACTION};
    uint8_t Tempint = 0;
    uint8_t Tempfloat = 0;
    float  result = 0.0f;
    ReadFlag = false;
    if(!__TempFlag)
    {
        uint8_t WriteData[2] = {DIE_TEMP_CONFIG,TEMP_EN};
        __TempFlag = true;
        i2c_master_transmit(*dev_handle,WriteData,2,TIMEOUT);
    }
    // i2c_master_transmit(*dev_handle,&ReadData[1],1,TIMEOUT);
    // i2c_master_receive(*dev_handle,&Tempint,1,TIMEOUT);
    // i2c_master_transmit(*dev_handle,&ReadData[0],1,TIMEOUT);
    // i2c_master_receive(*dev_handle,&Tempfloat,1,TIMEOUT);
    i2c_master_transmit_receive(*dev_handle,&ReadData[1],1,&Tempint,1,TIMEOUT);
    i2c_master_transmit_receive(*dev_handle,&ReadData[2],1,&Tempfloat,1,TIMEOUT);

    if (Tempint > 0x80)
    {
        result = 0x100 - Tempint;
 
    }
    else
    {
        result += Tempint;
    }
    // printf("temp int : %d\n",Tempint);
    result += Tempfloat*0.0625;
    return result;
}

/**
 * @brief 读取心率和血氧
 * 
 */
void MAX30102_Class::Read_HeartRAte_Spo2(float &heartRate,auto &oxygen)
{

}

void MAX30102_Class::init_intr()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << __INTR,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE ,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE, // 下降沿触发
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0); // 使用默认标志
    gpio_isr_handler_add(__INTR,intr_callback,(void *)__INTR);
}


void MAX30102_Class::intr_callback(void *arg)
{
    ReadFlag = true;
}

bool MAX30102_Class::check()
{
    if(ReadFlag == true)
    {
        return true;
    }
    return false;
}

/**
 * @brief 打开红光
 * @param 
 * 
 */
void MAX30102_Class::OpenRedLed(Slot_RedLed_t channel)
{
    uint8_t data[] = {
                      MULTI_LED_MODE_CONTROL_SLOT1_3, TURNON_LEDRED_PA_SLOT1_3,
                      MULTI_LED_MODE_CONTROL_SLOT2_4, TURNON_LEDRED_PA_SLOT2_4,
                      };
    switch (channel)
    {
    case REDLED_1:
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        break;
    case REDLED_2:
        i2c_master_transmit(*dev_handle, &data[2], 2, TIMEOUT);
        break;
    case REDLED_ALL:
        // i2c_master_transmit(*dev_handle, data, 4, TIMEOUT);
        i2c_master_transmit(*dev_handle, data, 2, TIMEOUT);
        i2c_master_transmit(*dev_handle, &data[2], 2, TIMEOUT);
        break;
    default:
        break;
    }
}

void MAX30102_Class::CloseRedLed()
{

}

void MAX30102_Class::OpenIRLed()
{

}

void MAX30102_Class::CloseIRLed()
{

}