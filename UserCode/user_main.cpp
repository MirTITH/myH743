#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include <string>
#include <iostream>
// #include "bsp_driver_sd.h"
#include "freertos_io.h"
#include "stdio_CLI.h"
#include "high_precision_time.h"

// #include <nlohmann/json.hpp>

using namespace std;
// using json = nlohmann::json;

sfud_flash *QspiFlash;

// uint8_t BSP_SD_IsDetected(void)
// {
//     return SD_NOT_PRESENT;
// }

void StartDefaultTask(void const *argument)
{
    (void)argument;

    FreeRTOS_IO_Init();

    QspiFlash = sfud_get_device(SFUD_W25Q64_DEVICE_INDEX);
    sfud_device_init(QspiFlash);

    MX_USB_DEVICE_Init();

    // 等待 USB 初始化完成
    osDelay(500);
    osDelay(2000); // 使得用户有足够的时间打开 USB 串口，防止错过开头的消息。

    CLI_Start();

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(500);
    }
}
