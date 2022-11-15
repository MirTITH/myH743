#pragma once
#include "main.h"
#include "high_precision_time.h"
#include "cpp_adc.hpp"

class Joystick
{
public:
    float x = 0; // x 方向值 [-1 1]
    float y = 0; // y 方向值 [-1 1]

    Joystick(CPP_ADC &adc_x, uint8_t x_data_index, CPP_ADC &adc_y, uint8_t y_data_index)
        : adc_x(adc_x), x_data_index(x_data_index), adc_y(adc_y), y_data_index(y_data_index){};

    void Init()
    {
        adc_x.StartDMA();
        adc_y.StartDMA();
    }

    void Update()
    {
        x = -2 * adc_x.GetChannelVoltage(x_data_index) / adc_x.GetVref() + 1;
        y = 2 * adc_y.GetChannelVoltage(y_data_index) / adc_y.GetVref() - 1;
    }

private:
    CPP_ADC &adc_x;
    uint8_t x_data_index;
    CPP_ADC &adc_y;
    uint8_t y_data_index;
};

extern Joystick LeftJoystick;
extern Joystick RightJoystick;

static inline void Joystick_Init()
{
    LeftJoystick.Init();
    RightJoystick.Init();
}
