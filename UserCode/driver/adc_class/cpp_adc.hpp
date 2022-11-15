#pragma once
#include "main.h"
#include "adc.h"
#include "high_precision_time.h"

class CPP_ADC
{
public:
    enum class States {
        Stop,
        RunningDMA,
        RunningIT
    };

    uint32_t callback_period_us = 0; // ConvCpltCallback 调用的周期 (us)，通过此值可以获得 ADC 转换速率

    /**
     * @brief Construct a new cpp_adc object
     *
     * @param hadc
     * @param vref 参考电压 (mV)
     */
    CPP_ADC(ADC_HandleTypeDef *hadc, uint16_t vref = 3300)
        : hadc(hadc), vref(vref){};

    ~CPP_ADC()
    {
        if (data != nullptr) {
            delete[] data;
        }
    }

    // ADC 校准
    HAL_StatusTypeDef Calibrate()
    {
        if (now_state == States::Stop) {
            return HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
        } else {
            return HAL_ERROR;
        }
    }

    HAL_StatusTypeDef Stop()
    {
        HAL_StatusTypeDef result = HAL_OK;
        switch (now_state) {
            case States::Stop:
                break;
            case States::RunningDMA:
                result = StopDma();
                break;
            case States::RunningIT:
                result = StopIT();
                break;

            default:
                break;
        }
        now_state = States::Stop;
        return result;
    }

    HAL_StatusTypeDef StartIT()
    {
        if (now_state == States::Stop) {
            if (is_calibrated == false) {
                Calibrate();
            }
            NumberOfConversion = hadc->Init.NbrOfConversion;
            NewDataArray(NumberOfConversion);

            HAL_StatusTypeDef result = HAL_ADC_Start_IT(hadc);
            if (result == HAL_OK) {
                now_state = States::RunningIT;
            }

            return result;
        }
        return HAL_BUSY;
    }

    HAL_StatusTypeDef StartDMA()
    {
        if (now_state == States::Stop) {
            if (is_calibrated == false) {
                Calibrate();
            }

            NumberOfConversion = hadc->Init.NbrOfConversion;
            NewDataArray(NumberOfConversion);

            HAL_StatusTypeDef result = HAL_ADC_Start_DMA(hadc, data, NumberOfConversion);
            if (result == HAL_OK) {
                now_state = States::RunningDMA;
            }

            return result;
        }
        return HAL_BUSY;
    }

    uint32_t GetChannelData(int index)
    {
        if (index < NumberOfConversion && index >= 0) {
            return data[index];
        } else {
            return 0;
        }
    }

    uint32_t GetResolution() const
    {
        return hadc->Init.Resolution;
    }

    /**
     * @brief Get Vref (V)
     *
     */
    float GetVref() const
    {
        return vref / 1000.0f;
    }

    States GetState() const
    {
        return now_state;
    }

    /**
     * @brief 将 ADC 的值转换为电压
     *
     * @param vref Analog reference voltage (unit: mV)
     * @param adcValue ADC conversion data of internal temperature sensor (unit: digital value)
     * @param adcResolutuion ADC resolution at which internal temperature sensor voltage has been measured.
     * @return 电压 (V)
     */
    float GetChannelVoltage(int index)
    {
        if (index < NumberOfConversion && index >= 0) {
            // 转换到 16 bit 对应的值
            float adcValue16Bit = __LL_ADC_CONVERT_DATA_RESOLUTION(data[index], hadc->Init.Resolution, LL_ADC_RESOLUTION_16B);
            return adcValue16Bit * vref / (1000 * (0xffff));
        } else {
            return 0;
        }
    }

    /**
     * @brief ADC 转换完成的回调函数。
     * 如果使用中断连续转换模式，则必须选择 End of single of conversion.
     * 如果使用 DMA 模式，建议选择 End of sequence of conversion.（DMA 模式不勾选 ADC 中断也能进回调函数
     * @note 请在 void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) 中调用该函数
     * @param current_hadc
     */
    void ConvCpltCallback(ADC_HandleTypeDef *current_hadc)
    {
        if (current_hadc->Instance == hadc->Instance) {
            // 判断 ADC 是直接读取模式还是 DMA 模式。
            if (hadc->Init.ConversionDataManagement == ADC_CONVERSIONDATA_DR) {
                // 直接读取模式，需要 CPU 读取
                data[data_index] = HAL_ADC_GetValue(hadc);
                data_index++;
                if (data_index >= NumberOfConversion) {
                    data_index = 0;
                }
            } else {
                // DMA 模式，不需要使用 CPU 读取，但需要无效化数据对应的缓存，使得下次读取直接从内存里读。
                SCB_InvalidateDCache_by_Addr(data, sizeof(data));
            }

            // 统计数据
            uint32_t now_us = HPT_GetUs();
            callback_period_us = now_us - last_callback_us;
            last_callback_us = now_us;
        }
    }

private:
    States now_state = States::Stop;
    bool is_calibrated = false;
    ADC_HandleTypeDef *hadc;
    const uint16_t vref; // 参考电压 (mV)
    int NumberOfConversion = 0;
    uint32_t *data = nullptr; // 存放 ADC 转换出的数据。因为 ADC 最多 20 个通道，所以这里直接分配 20。
    int data_index = 0;

    uint32_t last_callback_us = 0;

    void NewDataArray(int size)
    {
        if (data != nullptr) {
            delete[] data;
        }

        data = new uint32_t[size]();
    }

    HAL_StatusTypeDef StopIT()
    {
        return HAL_ADC_Stop_IT(hadc);
    }

    HAL_StatusTypeDef StopDma()
    {
        return HAL_ADC_Stop_DMA(hadc);
    }
};

/*************************************
 * 创建 CPP_ADC 对象
 *************************************/
extern CPP_ADC CppAdc1;
extern CPP_ADC CppAdc2;
extern CPP_ADC CppAdc3;
