/**
 * @brief
 *
 */

#include "main.h"
#include "cmsis_os.h"

#define RunTimeCounterSpeedFactor 10 // RunTimeCounter 相比于 osKernelSysTick 的速度倍率

/**
 * @brief 定时器初始化函数。（因为使用 SysTick，不需要初始化额外的定时器）
 *
 */
// void configureTimerForRunTimeStats(void)
// {
// }

/**
 * @brief Get the run time counter value
 *
 * @return unsigned long
 */
unsigned long getRunTimeCounterValue(void)
{
    return osKernelSysTick() * RunTimeCounterSpeedFactor + (SysTick->LOAD - SysTick->VAL) / (configCPU_CLOCK_HZ / (RunTimeCounterSpeedFactor * configTICK_RATE_HZ));
}
