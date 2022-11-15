#pragma once
#include "main.h"
#include "adc.h"
#include "high_precision_time.h"

class Joystick
{
public:
    float x = 0; // x 方向值 [-1 1]
    float y = 0; // y 方向值 [-1 1]
    uint32_t last_callback_us = 0;
    uint32_t callback_period_us = 0;

    Joystick(ADC_HandleTypeDef *hadc, uint16_t circuit_vref = 3300)
        : hadc(hadc), circuit_vref(circuit_vref){};

    void Init()
    {
        // ADC 校准
        HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
        // 开始中断转换
        HAL_ADC_Start_IT(hadc);
    }

    /**
     * @brief ADC 转换完成的回调函数
     *
     * @param current_hadc 当前转换完成的 adc
     */
    void conv_callback(ADC_HandleTypeDef *current_hadc)
    {
        if (current_hadc == hadc) {
            uint32_t now_us = HPT_GetUs();
            callback_period_us = now_us - last_callback_us;
            last_callback_us = now_us;
            uint32_t adc_value = HAL_ADC_GetValue(hadc);
            switch (data_index) {
                case 0:
                    x = -2000 * CalcVoltage(circuit_vref, adc_value, hadc->Init.Resolution) / circuit_vref + 1;
                    break;
                case 1:
                    y = 2000 * CalcVoltage(circuit_vref, adc_value, hadc->Init.Resolution) / circuit_vref - 1;
                    break;
                default:
                    break;
            }
            data_index++;
            if (data_index >= number_of_conversion) {
                data_index = 0;
            }

            // 连续模式只需要开启一次中断转换即可,非连续转换模式每次都要重新开启
            if (this->hadc->Init.ContinuousConvMode == FunctionalState::DISABLE) {
                HAL_ADC_Start_IT(hadc);
            }
        }
    }

private:
    ADC_HandleTypeDef *hadc;          // 对应的 ADC
    uint16_t circuit_vref;            // 电路上设计的参考电压值 (mV)
    uint8_t number_of_conversion = 2; // 扫描转换的数量
    uint8_t data_index = 0;           // 当前要接收的数据

    /**
     * @brief 将 ADC 的值转换为电压
     *
     * @param vref Analog reference voltage (unit: mV)
     * @param adcValue ADC conversion data of internal temperature sensor (unit: digital value)
     * @param adcResolutuion ADC resolution at which internal temperature sensor voltage has been measured.
     * @return 电压 (V)
     */
    float
    CalcVoltage(uint16_t vref, uint32_t adcValue, uint32_t adcResolutuion)
    {
        float adcValue16Bit = __LL_ADC_CONVERT_DATA_RESOLUTION(adcValue, adcResolutuion, LL_ADC_RESOLUTION_16B);
        return adcValue16Bit * vref / (1000 * (0xffff));
    }
};

extern Joystick LeftJoystick;
extern Joystick RightJoystick;

static inline void Joystick_Init()
{
    LeftJoystick.Init();
    RightJoystick.Init();
}
