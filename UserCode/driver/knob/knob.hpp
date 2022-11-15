#pragma once

#include "main.h"

class Knob
{
public:
    Knob(GPIO_TypeDef *A_GPIO_Port, uint16_t A_GPIO_Pin,
         GPIO_TypeDef *B_GPIO_Port, uint16_t B_GPIO_Pin,
         GPIO_TypeDef *Button_GPIO_Port, uint16_t Button_GPIO_Pin)
        : A_GPIO_Port(A_GPIO_Port), A_GPIO_Pin(A_GPIO_Pin),
          B_GPIO_Port(B_GPIO_Port), B_GPIO_Pin(B_GPIO_Pin),
          Button_GPIO_Port(Button_GPIO_Port), Button_GPIO_Pin(Button_GPIO_Pin){};

    /**
     * @brief 在 A 上升沿的回调函数中调用此函数
     *
     */
    void ARiseCallback();

    /**
     * @brief 按钮是否按下
     *
     * @return true 按钮按下
     * @return false 按钮未按下
     */
    bool IsButtonDown();

    /**
     * @brief 获得旋转计数
     *
     * @return int 旋转一个齿加减1，逆时针为正
     */
    int GetRotationalCount()
    {
        return rot_count;
    }

private:
    GPIO_TypeDef *const A_GPIO_Port;
    const uint16_t A_GPIO_Pin;

    GPIO_TypeDef *const B_GPIO_Port;
    const uint16_t B_GPIO_Pin;
    GPIO_TypeDef *const Button_GPIO_Port;
    const uint16_t Button_GPIO_Pin;

    volatile uint32_t last_callback_time = 0;
    uint32_t jitter_elimination_time = 1000; // 消抖时间 (us)

    volatile int32_t rot_count = 0;
};

extern Knob LeftKnob;
extern Knob RightKnob;
