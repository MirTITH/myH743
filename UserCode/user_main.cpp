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
#include "sfud_test.h"

// #include <nlohmann/json.hpp>

using namespace std;
// using json = nlohmann::json;

sfud_flash *QspiFlash;

static FATFS fs;
static FIL json_file;
#define ReadBufferSize 81920

uint8_t ReadBuffer[ReadBufferSize];

void StartDefaultTask(void const *argument)
{
    (void)argument;

    MX_USB_DEVICE_Init();
    FreeRTOS_IO_Init();

    // 等待 USB 初始化完成
    osDelay(500);
    osDelay(2000); // 使得用户有足够的时间打开 USB 串口，防止错过开头的消息。

    sfud_init();

    QspiFlash = sfud_get_device(SFUD_W25Q64_DEVICE_INDEX);
    sfud_qspi_fast_read_enable(QspiFlash, 4);

    CLI_Start();

    f_mount(&fs, "0", 1);

    f_open(&json_file, "settings.json", FA_READ);

    UINT bytes_read;

    f_read(&json_file, ReadBuffer, ReadBufferSize, &bytes_read);

    for (uint i = 0; i < bytes_read; i++) {
        cout << ReadBuffer[i];
    }

    cout << endl;

    f_close(&json_file);

    sfud_ReadTest(QspiFlash, 0, ReadBufferSize, ReadBuffer);

    auto start_tick = HPT_GetUs();
    memcpy(ReadBuffer, ReadBuffer + ReadBufferSize / 2, ReadBufferSize / 2);
    auto end_tick = HPT_GetUs();

    cout << "memcpy time: " << end_tick - start_tick << "us. Speed: " << (double)(1e6 / 1024) * (ReadBufferSize / 2) / (end_tick - start_tick) << "KB/s" << endl;

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(500);
    }
}
