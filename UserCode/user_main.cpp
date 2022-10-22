#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include <string>
#include <iostream>
#include "bsp_driver_sd.h"
#include "freertos_io.h"
#include "stdio_CLI.h"
#include "w25qxx.h"
// #include <nlohmann/json.hpp>

using namespace std;
// using json = nlohmann::json;

uint8_t buff[5] = {0x01, 0x22, 0xff, 0x23, 0x00};

uint8_t BSP_SD_IsDetected(void)
{
    return SD_NOT_PRESENT;
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    // FreeRTOS_IO_Init();
    MX_USB_DEVICE_Init();

    // 等待 USB 初始化完成
    osDelay(500);

    // CLI_Start();

    for (;;) {
        // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        osDelay(500);
    }
}
