#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include <string>
#include <iostream>
#include "bsp_driver_sd.h"
#include "freertos_io.h"
#include "stdio_CLI.h"
#include "high_precision_time.h"
// #include "w25qxx.h"
#include "sfud.h"
// #include <nlohmann/json.hpp>

using namespace std;
// using json = nlohmann::json;

// #define SFUD_DEMO_TEST_BUFFER_SIZE 128
// static uint8_t test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];

#define valueArray_Size 10000

static uint32_t valueArray[valueArray_Size];

int temp;

uint8_t BSP_SD_IsDetected(void)
{
    return SD_NOT_PRESENT;
}

/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
void sfud_demo(uint32_t addr, size_t size, uint8_t *data)
{
    sfud_err result = SFUD_SUCCESS;
    extern sfud_flash *sfud_dev;
    const sfud_flash *flash = sfud_get_device(SFUD_W25Q64_DEVICE_INDEX);
    size_t i;
    /* prepare write data */
    for (i = 0; i < size; i++) {
        data[i] = i;
    }
    /* erase test */
    result = sfud_erase(flash, addr, size);
    if (result == SFUD_SUCCESS) {
        printf("Erase the %s flash data finish. Start from 0x%08X, size is %zu.\r\n", flash->name, addr, size);
    } else {
        printf("Erase the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* write test */
    result = sfud_write(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Write the %s flash data finish. Start from 0x%08X, size is %zu.\r\n", flash->name, addr, size);
    } else {
        printf("Write the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* read test */
    result = sfud_read(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Read the %s flash data success. Start from 0x%08X, size is %zu. The data is:\r\n", flash->name, addr, size);
        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++) {
            if (i % 16 == 0) {
                printf("[%08X] ", addr + i);
            }
            printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1) {
                printf("\r\n");
            }
        }
        printf("\r\n");
    } else {
        printf("Read the %s flash data failed.\r\n", flash->name);
    }
    /* data check */
    for (i = 0; i < size; i++) {
        if (data[i] != i % 256) {
            printf("Read and check write data has an error. Write the %s flash data failed.\r\n", flash->name);
            break;
        }
    }
    if (i == size) {
        printf("The %s flash test is success.\r\n", flash->name);
    }
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    FreeRTOS_IO_Init();
    MX_USB_DEVICE_Init();

    // 等待 USB 初始化完成
    osDelay(500);

    CLI_Start();

    osDelay(2000);

    for (;;) {
        // vTaskSuspendAll();

        // for (int i = 0; i < valueArray_Size; i++) {
        //     for (int j = 0; j < 1e3; j++) {
        //         temp++;
        //     }
        // }
        uint64_t tickStart = HPT_GetNs();
        // osDelay(2);
        HPT_DelayUs(20);
        uint64_t tickEnd = HPT_GetNs();
        // xTaskResumeAll();
        cout << tickEnd << ',';
        cout << tickStart << ',';
        cout << tickEnd - tickStart << endl;

        // cout << test_buf << endl;
        // for (int i = 0; i < valueArray_Size; i++) {
        //     cout << valueArray[i] << endl;
        // }

        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(500);
    }
}
