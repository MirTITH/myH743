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
#include "nameof.hpp"
#include <string>
#include "cmsis_os.h"

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

SysMonitor::Info SysMonitor::GetInfo()
{
    UpdateInfo();
    return info;
}

std::ostream &operator<<(std::ostream &out, SysMonitor &A)
{
    if (A.is_inited) {
        A.UpdateInfo();
        NameAndValue(out, A.info.temperature) << std::endl;
        NameAndValue(out, A.info.vbat) << std::endl;
        NameAndValue(out, A.info.vrefint) << std::endl;
    } else {
        out << "SysMonitor not initialized" << std::endl;
    }

    return out;
}

void SysMonitor::UpdateInfo()
{
    portENTER_CRITICAL();
    auto temperature_data = cpp_adc.GetChannelData(temperature_data_index);
    info.temperature = CalcTemperature(cpp_adc.GetVref() * 1000, temperature_data, cpp_adc.GetResolution());
    // 芯片内部有 1/4 分压，所以这里要乘以 4 才是实际电压
    info.vbat = cpp_adc.GetChannelVoltage(vbat_data_index) * 4;
    info.vrefint = cpp_adc.GetChannelVoltage(vrefint_data_index);
    portEXIT_CRITICAL();
}
