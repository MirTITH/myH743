/**
 * @file sys_monitor.cpp
 * @author X. Y.
 * @brief 系统监视器
 * @version 0.2
 * @date 2022-10-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sys_monitor.hpp"
#include "cmsis_os.h"
#include "nameof.hpp"
#include <string>

#define NameAndValue(outstream, x) (outstream << NAMEOF(x) << ":" << x)

/**
 * @brief 计算内部温度传感器的温度
 *
 * @param vref            Analog reference voltage (unit: mV)
 * @param adcValue        ADC conversion data of internal temperature sensor (unit: digital value)
 * @param adcResolutuion  ADC resolution at which internal temperature sensor voltage has been measured.
 *                            This parameter can be one of the following values:
 *                            @arg @ref ADC_RESOLUTION_16B
 *                            @arg @ref ADC_RESOLUTION_14B
 *                            @arg @ref ADC_RESOLUTION_12B
 *                            @arg @ref ADC_RESOLUTION_10B
 *                            @arg @ref ADC_RESOLUTION_8B
 * @return 温度（摄氏度）
 *
 * @note Calculation formula:
 *           Temperature = ((TS_ADC_DATA - TS_CAL1)
 *                           * (TS_CAL2_TEMP - TS_CAL1_TEMP))
 *                         / (TS_CAL2 - TS_CAL1) + TS_CAL1_TEMP
 *           with TS_ADC_DATA = temperature sensor raw data measured by ADC
 *                Avg_Slope = (TS_CAL2 - TS_CAL1)
 *                            / (TS_CAL2_TEMP - TS_CAL1_TEMP)
 *                TS_CAL1   = equivalent TS_ADC_DATA at temperature
 *                            TEMP_DEGC_CAL1 (calibrated in factory)
 *                TS_CAL2   = equivalent TS_ADC_DATA at temperature
 *                            TEMP_DEGC_CAL2 (calibrated in factory)
 */
static float CalcTemperature(uint16_t vref, uint32_t adcValue, uint32_t adcResolutuion)
{
    float adcValue16Bit = __LL_ADC_CONVERT_DATA_RESOLUTION(adcValue, adcResolutuion, LL_ADC_RESOLUTION_16B);
    return (adcValue16Bit * vref / TEMPSENSOR_CAL_VREFANALOG - (*TEMPSENSOR_CAL1_ADDR)) *
               (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) /
               (*TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR) +
           TEMPSENSOR_CAL1_TEMP;
}

/**
 * @brief 将 ADC 的值转换为电压
 *
 * @param vref Analog reference voltage (unit: mV)
 * @param adcValue ADC conversion data of internal temperature sensor (unit: digital value)
 * @param adcResolutuion ADC resolution at which internal temperature sensor voltage has been measured.
 * @return 电压 (V)
 */
static float CalcVoltage(uint16_t vref, uint32_t adcValue, uint32_t adcResolutuion)
{
    float adcValue16Bit = __LL_ADC_CONVERT_DATA_RESOLUTION(adcValue, adcResolutuion, LL_ADC_RESOLUTION_16B);
    return adcValue16Bit * vref / (1000 * (0xffff));
}

SysMonitor::SysMonitor(ADC_HandleTypeDef *hadc, uint16_t circuit_vref)
{
    this->hadc = hadc;
    this->circuit_vref = circuit_vref;
    this->Number_Of_Conversion = hadc->Init.NbrOfConversion;
    this->info_index = 0;

    // ADC 校准
    HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
    // 开始中断转换
    HAL_ADC_Start_IT(hadc);
}

void SysMonitor::conv_callback(ADC_HandleTypeDef *current_hadc)
{
    if (current_hadc == hadc) {
        uint32_t adc_value = HAL_ADC_GetValue(hadc);
        switch (info_index) {
            case 0:
                info.temperature = CalcTemperature(circuit_vref, adc_value, hadc->Init.Resolution);
                break;
            case 1:
                // 芯片内部有 1/4 分压，所以这里要乘以 4 才是实际电压
                info.vbat = CalcVoltage(circuit_vref, adc_value, hadc->Init.Resolution) * 4;
                break;
            case 2:
                info.vrefint = CalcVoltage(circuit_vref, adc_value, hadc->Init.Resolution);
                break;
            default:
                break;
        }
        info_index++;
        if (info_index >= Number_Of_Conversion) {
            info_index = 0;
        }

        // 连续模式只需要开启一次中断转换即可,非连续转换模式每次都要重新开启
        if (this->hadc->Init.ContinuousConvMode == FunctionalState::DISABLE) {
            HAL_ADC_Start_IT(hadc);
        }
    }
}

SysMonitor::Info SysMonitor::GetInfo() const
{
    portENTER_CRITICAL();
    Info temp = info;
    portEXIT_CRITICAL();
    return temp;
}

std::ostream &operator<<(std::ostream &out, SysMonitor &A)
{
    NameAndValue(out, A.info.temperature) << std::endl;
    NameAndValue(out, A.info.vbat) << std::endl;
    NameAndValue(out, A.info.vrefint) << std::endl;
    return out;
}
