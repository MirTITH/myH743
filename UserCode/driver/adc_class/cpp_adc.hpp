#pragma once
#include "main.h"
#include "adc.h"
#include "high_precision_time.h"

class CPP_ADC
{
public:
    uint32_t data[16];

    CPP_ADC(ADC_HandleTypeDef *hadc)
        : hadc(hadc){};

    HAL_StatusTypeDef StartIT()
    {
        NumberOfConversion = hadc->Init.NbrOfConversion;
        // ADC 校准
        HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
        return HAL_ADC_Start_IT(hadc);
    }

    HAL_StatusTypeDef StopIT()
    {
        return HAL_ADC_Stop_IT(hadc);
    }

    HAL_StatusTypeDef StartDma()
    {
        NumberOfConversion = hadc->Init.NbrOfConversion;
        // ADC 校准
        HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
        return HAL_ADC_Start_DMA(hadc, data, NumberOfConversion);
    }

    HAL_StatusTypeDef StopDma()
    {
        return HAL_ADC_Stop_DMA(hadc);
    }

    void ConvCpltCallback(ADC_HandleTypeDef *current_hadc)
    {
        if (current_hadc == hadc) {
            uint32_t now_us = HPT_GetUs();
            callback_period_us = now_us - last_callback_us;
            last_callback_us = now_us;

            data[data_index] = HAL_ADC_GetValue(hadc);
            data_index++;
            if (data_index >= NumberOfConversion) {
                data_index = 0;
            }

            // 连续模式只需要开启一次中断转换即可,非连续转换模式每次都要重新开启
            // if (this->hadc->Init.ContinuousConvMode == FunctionalState::DISABLE) {
            //     HAL_ADC_Start_IT(hadc);
            // }
        }
    }

private:
    ADC_HandleTypeDef *hadc;
    int NumberOfConversion;
    int data_index = 0;
    uint32_t last_callback_us = 0;
    uint32_t callback_period_us = 0;
};

extern CPP_ADC CppAdc1;
extern CPP_ADC CppAdc2;