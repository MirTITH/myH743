#include "knob.hpp"
#include "high_precision_time.h"

Knob LeftKnob(LKnobA_GPIO_Port, LKnobA_Pin, LKnobB_GPIO_Port, LKnobB_Pin, LKnobButton_GPIO_Port, LKnobButton_Pin);
Knob RightKnob(RKnobA_GPIO_Port, RKnobA_Pin, RKnobB_GPIO_Port, RKnobB_Pin, RKnobButton_GPIO_Port, RKnobButton_Pin);

void Knob::ARiseCallback()
{
    HPT_DelayUs(500); // 延时 500 us 后再检测 A 是否为高电平，防止在 A 的下降沿由于抖动触发上升沿中断
    if (HAL_GPIO_ReadPin(A_GPIO_Port, A_GPIO_Pin) == GPIO_PIN_SET) {
        if (HPT_GetUs() - last_callback_time > jitter_elimination_time) {
            if (HAL_GPIO_ReadPin(B_GPIO_Port, B_GPIO_Pin) == GPIO_PIN_RESET) {
                rot_count++;
            } else {
                rot_count--;
            }
            last_callback_time = HPT_GetUs();
        }
    }
}

bool Knob::IsButtonDown()
{
    switch (HAL_GPIO_ReadPin(Button_GPIO_Port, Button_GPIO_Pin)) {
        case GPIO_PIN_RESET:
            return true;
            break;
        case GPIO_PIN_SET:
            return false;
            break;

        default:
            return false;
            break;
    }
}
