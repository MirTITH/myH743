#include "sys_monitor.hpp"
#include "adc.h"

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

static void SysMonitor_Entry(void const *argument)
{
    auto this_monitor = (SysMonitor *)argument;
    uint32_t adc3_value = 0;

    uint32_t start_tick = 0;

    for (;;) {
        start_tick = osKernelSysTick();
        HAL_ADC_Start(&hadc3);
        if (HAL_ADC_PollForConversion(&hadc3, 1000) == HAL_OK) {
            adc3_value = HAL_ADC_GetValue(&hadc3);
        }
        this_monitor->temperature = CalcTemperature(3300, adc3_value, ADC_RESOLUTION_16B);
        this_monitor->loop_time = (osKernelSysTick() - start_tick) * configTICK_RATE_HZ / 1000;
        osDelay(100);
    }
}

SysMonitor::SysMonitor(const char *threadName, osPriority priority, uint32_t stacksize)
{
    thread_def.name = (char *)threadName;
    thread_def.pthread = SysMonitor_Entry;
    thread_def.tpriority = priority;
    thread_def.stacksize = stacksize;

    thread_id = osThreadCreate(&thread_def, this);
}

SysMonitor::~SysMonitor()
{
}
