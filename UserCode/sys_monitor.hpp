#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float core_temp;
} SysMonitorInfo_t;

extern SysMonitorInfo_t SysMonitorInfo;

void SysMonitor_Start();

#ifdef __cplusplus
}
#endif
