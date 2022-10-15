#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include <string>
#include <iostream>
#include "sys_monitor.hpp"
#include "bsp_driver_sd.h"
#include "freertos_io.h"

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

    auto &sys_monitor = SysMonitor::GetInstance();

    string str;

    for (;;) {
        cin >> str;
        cout << "received:" << str << endl;
        // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        // cout << sys_monitor << endl;
        // cout << sys_monitor.GetInfo().temperature << ",";
        // cout << sys_monitor.GetInfo().vbat << ",";
        // cout << sys_monitor.GetInfo().vrefint;
        // cout << endl;
        // osDelay(500);
    }
}
