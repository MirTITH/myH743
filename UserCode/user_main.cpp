#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include <string>
#include <iostream>
#include "sys_monitor.hpp"
#include "bsp_driver_sd.h"
#include "freertos_io.h"
#include "stdio_CLI.h"

using namespace std;

uint8_t BSP_SD_IsDetected(void)
{
    return SD_NOT_PRESENT;
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    FreeRTOS_IO_Init();

    MX_USB_DEVICE_Init();

    // 等待 USB 初始化完成
    osDelay(500);

    CLI_Start();

    auto &sys_monitor = SysMonitor::GetInstance();

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        cout << sys_monitor << endl;

        osDelay(500);
    }
}
