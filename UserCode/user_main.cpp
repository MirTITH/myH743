#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include <string>
#include <iostream>
// #include "bsp_driver_sd.h"
#include "freertos_io.h"
#include "stdio_CLI.h"
#include "high_precision_time.h"
#include "ff.h"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

static FATFS fs;

// #define ReadBufferSize 1024
// static TCHAR ReadBuffer[ReadBufferSize];

void StartDefaultTask(void const *argument)
{
    (void)argument;

    MX_USB_DEVICE_Init();
    FreeRTOS_IO_Init();

    // 等待 USB 初始化完成
    osDelay(500);
    osDelay(2000); // 使得用户有足够的时间打开 USB 串口，防止错过开头的消息。

    f_mount(&fs, "0", 1);

    CLI_Start();

    auto json_file = new FIL;
    f_open(json_file, "settings.json", FA_READ);

    f_close(json_file);
    delete json_file;

    for (;;) {
        // cout << ReadBuffer << endl;
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(500);
    }
}
