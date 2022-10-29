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

// #include <nlohmann/json.hpp>

using namespace std;
// using json = nlohmann::json;

#define ReadBufferSize 40960
uint8_t ReadBuffer[ReadBufferSize];

sfud_flash *QspiFlash;

// uint8_t BSP_SD_IsDetected(void)
// {
//     return SD_NOT_PRESENT;
// }

void StartDefaultTask(void const *argument)
{
    (void)argument;

    MX_USB_DEVICE_Init();
    FreeRTOS_IO_Init();

    // 等待 USB 初始化完成
    osDelay(500);
    osDelay(2000); // 使得用户有足够的时间打开 USB 串口，防止错过开头的消息。

    QspiFlash = sfud_get_device(SFUD_W25Q64_DEVICE_INDEX);
    sfud_device_init(QspiFlash);

    CLI_Start();

    FATFS fs;

    FRESULT res;

    res = f_mount(&fs, "0", 1);

    if (res == FR_OK) {
        cout << "f_mount OK!" << endl;
    } else {
        cout << "f_mount failed: " << res << endl;
    }

    res = f_mkfs("0", FM_FAT, 0, ReadBuffer, ReadBufferSize / 4);

    if (res == FR_OK) {
        cout << "f_mkfs OK!" << endl;
    } else {
        cout << "f_mkfs failed: " << res << endl;
    }

    FIL file;
    res = f_open(&file, "/settings.json", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

    if (res == FR_OK) {
        cout << "f_open OK!" << endl;
    } else {
        cout << "f_open failed: " << res << endl;
    }

    UINT bytes_written;
    res = f_write(&file, "Hello file\n", sizeof("Hello file\n"), &bytes_written);
    if (res == FR_OK) {
        cout << "f_write OK!" << endl;
    } else {
        cout << "f_write failed: " << res << endl;
    }

    UINT bytes_read;
    res = f_read(&file, ReadBuffer, ReadBufferSize, &bytes_read);

    if (res == FR_OK) {
        cout << "f_read OK!" << endl;
    } else {
        cout << "f_read failed: " << res << endl;
    }

    cout << ReadBuffer << endl;

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(500);
    }
}
