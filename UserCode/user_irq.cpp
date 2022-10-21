#include "main.h"
#include "sys_monitor.hpp"

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    SysMonitor::GetInstance().conv_callback(hadc);
}
