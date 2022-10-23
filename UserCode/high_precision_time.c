/**
 * @file micro_second.c
 * @author X. Y.
 * @brief 高精度时间库
 * @version 0.1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "high_precision_time.h"
#include "main.h"

#define USE_FREERTOS

#ifdef USE_FREERTOS
#include "cmsis_os.h"
#endif

uint32_t HPT_GetUs()
{
#ifdef USE_FREERTOS
    return osKernelSysTick() * (1000000 / configTICK_RATE_HZ) + (SysTick->LOAD - SysTick->VAL) / (configCPU_CLOCK_HZ / (1000 * configTICK_RATE_HZ));
#else
    return uwTick * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / (1000 * 1000 / uwTickFreq));
#endif
}

void HPT_DelayUs(uint32_t us)
{
#ifdef USE_FREERTOS
    uint32_t beforeTaskDelay = HPT_GetUs();
    if (us >= 1000000 / configTICK_RATE_HZ) {
        vTaskDelay(us / 1000);
    }
    while ((HPT_GetUs()) < us + beforeTaskDelay) {
    }
#else
    uint32_t usStart = HPT_GetUs();
    while ((HPT_GetUs() - usStart) < us) {
    }
#endif // USE_FREERTOS
}

uint64_t HPT_GetNs()
{
#ifdef USE_FREERTOS
    return (uint64_t)osKernelSysTick() * (1000000000 / configTICK_RATE_HZ) + (uint64_t)(SysTick->LOAD - SysTick->VAL) * (1000000 * configTICK_RATE_HZ) / configCPU_CLOCK_HZ;
#else
    return (uint64_t)uwTick * 1000000 + (uint64_t)(SysTick->LOAD - SysTick->VAL) * (1000000 * 1000 / uwTickFreq) / SystemCoreClock;
#endif
}
