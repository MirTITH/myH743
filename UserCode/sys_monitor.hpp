#pragma once
#include "cmsis_os.h"
#include "main.h"

class SysMonitor
{
public:
    float temperature; // 内核温度（摄氏度）
    uint16_t loop_time; // 进行一轮采集花费的总时间（毫秒）
    SysMonitor(const char *threadName = "SysMonitor", osPriority priority = osPriorityNormal, uint32_t stacksize = 256);
    ~SysMonitor();
private:
    osThreadDef_t thread_def;
    osThreadId thread_id;
};
