/**
 * @brief
 *
 */

#include "main.h"
#include "cmsis_os.h"

#define RunTimeCounterSpeedFactor 10

void configureTimerForRunTimeStats(void)
{
}

unsigned long getRunTimeCounterValue(void)
{
    return osKernelSysTick() * RunTimeCounterSpeedFactor + (SysTick->LOAD - SysTick->VAL) / (configCPU_CLOCK_HZ / (RunTimeCounterSpeedFactor * configTICK_RATE_HZ));
}
