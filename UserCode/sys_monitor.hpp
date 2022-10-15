/**
 * @file sys_monitor.hpp
 * @author X. Y.
 * @brief 系统监视器
 * @version 0.2
 * @date 2022-10-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "main.h"
#include "adc.h"
#include <ostream>

class SysMonitor
{
public:
    struct Info {
        float temperature; // 内核温度 (℃)
        float vbat;        // 电池电压 (V)
        float vrefint;     // 内部参考电压 (V)
    };

    // 获取单例
    static SysMonitor &GetInstance()
    {
        // 需要 c++11 及以上标准以保证线程安全
        static SysMonitor instance;
        return instance;
    }

    /**
     * @brief ADC 转换完成的回调函数
     *
     * @param current_hadc 当前转换完成的 adc
     */
    void conv_callback(ADC_HandleTypeDef *current_hadc);

    Info GetInfo() const;

    friend std::ostream & operator<<(std::ostream &out, SysMonitor &A);

private:
    ADC_HandleTypeDef *hadc;  // 对应的 ADC
    uint16_t circuit_vref;    // 电路上设计的参考电压值 (mV)
    int Number_Of_Conversion; // 扫描转换的数量

    Info info;
    uint8_t info_index; // 当前要接收的信息 id

    /**
     * @brief 在这里修改相关参数
     *
     * @param hadc 对应的 ADC
     * @param circuit_vref 电路上设计的参考电压值 (mV)
     */
    SysMonitor(ADC_HandleTypeDef *hadc = &hadc3, uint16_t circuit_vref = 3300);
    ~SysMonitor() = default;
    SysMonitor(const SysMonitor &) = delete;
    SysMonitor &operator=(const SysMonitor &) = delete;
};
