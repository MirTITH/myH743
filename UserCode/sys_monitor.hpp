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
#include "cpp_adc.hpp"
#include <ostream>

class SysMonitor
{
public:
    bool is_inited = false;
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

    void Init()
    {
        if (is_inited == false) {
            cpp_adc.StartDMA();
        }
        is_inited = true;
    }

    Info GetInfo();

    void UpdateInfo();

    friend std::ostream &operator<<(std::ostream &out, SysMonitor &A);

private:
    Info info;

    CPP_ADC &cpp_adc = CppAdc3;
    int temperature_data_index = 1;
    int vbat_data_index = 0;
    int vrefint_data_index = 2;

    /**
     * @brief 在这里修改相关参数
     *
     * @param hadc 对应的 ADC
     * @param circuit_vref 电路上设计的参考电压值 (mV)
     */
    SysMonitor(){};
    ~SysMonitor() = default;
    SysMonitor(const SysMonitor &) = delete;
    SysMonitor &operator=(const SysMonitor &) = delete;
};
