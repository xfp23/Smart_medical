#include "NTC.h"
#include <math.h>
NTC_Class::NTC_Class()
{
    adc_handle = new adc_oneshot_unit_handle_t;
}

NTC_Class::~NTC_Class()
{
    if (adc_handle != nullptr)
    {
        delete adc_handle;
    }
    ESP_ERROR_CHECK(adc_oneshot_del_unit(*adc_handle));
}

void NTC_Class::begin(adc_unit_t adc_id, adc_channel_t adc_channel)
{
    __Channel = adc_channel;
    adc_oneshot_unit_init_cfg_t adc_cfg = {
        .unit_id = adc_id,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_cfg, adc_handle));
    adc_oneshot_chan_cfg_t adc_chan_cfg = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc_handle, __Channel, &adc_chan_cfg));
}

float NTC_Class::calculateADC(uint16_t adc_value)
{
    float Vout = ((float)adc_value / (float)ADC_MAX) * (float)Vref;
    float R_ntc = (float)R_fixed * (Vref / Vout - 1);
    float T = 1 / ((log(R_ntc / (float)R25) / (float)B_value) + (1 / (float)T0));
    return T - 273.15; // 转换为摄氏度
}


uint16_t NTC_Class::Read_ADC()
{
uint16_t result;
esp_err_t ret = adc_oneshot_read(*adc_handle,__Channel,(int *)&result);
if (ret != ESP_OK) {
    // printf("ADC read failed: %s\n", esp_err_to_name(ret));
    result = 0; // 返回一个默认值
}
// printf("ADC Value : %d Vol : %.2f\n",result,result *3.3/4096);
   return (uint16_t)result;
}

float NTC_Class::Read_Temp()
{
    float Temp = calculateADC(Read_ADC());
    return Temp;
}