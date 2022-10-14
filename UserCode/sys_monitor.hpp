#pragma once
#include "cmsis_os.h"
#include "main.h"

class SysMonitor
{
public:
    float temperature;
    SysMonitor(const char *threadName = "SysMonitor", osPriority priority = osPriorityNormal, uint32_t stacksize = 256);
    ~SysMonitor();
private:
    osThreadDef_t thread_def;
    osThreadId thread_id;
};
