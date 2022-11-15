#include "main.h"
#include "sys_monitor.hpp"
#include "knob.hpp"
#include "joystick.hpp"

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    SysMonitor::GetInstance().conv_callback(hadc);
    LeftJoystick.conv_callback(hadc);
    RightJoystick.conv_callback(hadc);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case LKnobA_Pin:
            LeftKnob.ARiseCallback();
            break;
        case RKnobA_Pin:
            RightKnob.ARiseCallback();
            break;

        default:
            break;
    }
}
