#include "sys_monitor.hpp"
#include "cmsis_os.h"
#include "main.h"
#include "adc.h"

SysMonitorInfo_t SysMonitorInfo;

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
 */
static float CalcTemperature(uint16_t vref, uint32_t adcValue, uint32_t adcResolutuion)
{
    float adcValue16Bit = __LL_ADC_CONVERT_DATA_RESOLUTION(adcValue, adcResolutuion, LL_ADC_RESOLUTION_16B);
    return (adcValue16Bit * vref / TEMPSENSOR_CAL_VREFANALOG - (*TEMPSENSOR_CAL1_ADDR)) *
               (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) /
               (*TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR) +
           TEMPSENSOR_CAL1_TEMP;
}

void SysMonitor_Entry(void const *argument)
{
    (void)argument;

    uint32_t adc3_value = 0;

    for (;;) {
        HAL_ADC_Start(&hadc3);
        if (HAL_ADC_PollForConversion(&hadc3, 100) == HAL_OK) {
            adc3_value = HAL_ADC_GetValue(&hadc3);
        }
        SysMonitorInfo.core_temp = CalcTemperature(3300, adc3_value, ADC_RESOLUTION_16B);

        std::cout << "\tHAL:" << __HAL_ADC_CALC_TEMPERATURE(3300, adc3_value, ADC_RESOLUTION_16B) << std::endl;

        osDelay(100);
    }
}

void SysMonitor_Start()
{
    osThreadDef_t sysMonitorThreadDef = {
        .name = (char *)"SysMonitor",
        .pthread = SysMonitor_Entry,
        .tpriority = osPriorityNormal,
        .stacksize = 256};

    osThreadCreate(&sysMonitorThreadDef, NULL);
}
