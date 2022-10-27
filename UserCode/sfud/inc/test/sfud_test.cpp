#include "sfud_test.h"
#include "high_precision_time.h"
#include <iostream>

using namespace std;

void sfud_demo(const sfud_flash *flash, uint32_t addr, size_t size, uint8_t *data)
{
    uint32_t tickStart, tickEnd;
    sfud_err result = SFUD_SUCCESS;
    size_t i;
    /* prepare write data */
    for (i = 0; i < size; i++) {
        data[i] = i;
    }
    /* erase test */
    tickStart = HPT_GetUs();
    result = sfud_erase(flash, addr, size);
    tickEnd = HPT_GetUs();
    if (result == SFUD_SUCCESS) {
        printf("Erase the %s flash data finish. Start from 0x%08lX, size is %u.\r\n", flash->name, addr, size);
        cout << "time: " << tickEnd - tickStart << "us.\tSpeed: " << (double)(1e6 / 1024) * size / (tickEnd - tickStart) << "KB/s" << endl;
    } else {
        printf("Erase the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* write test */
    tickStart = HPT_GetUs();
    result = sfud_write(flash, addr, size, data);
    tickEnd = HPT_GetUs();
    if (result == SFUD_SUCCESS) {
        printf("Write the %s flash data finish. Start from 0x%08lX, size is %u.\r\n", flash->name, addr, size);
        cout << "time: " << tickEnd - tickStart << "us.\tSpeed: " << (double)(1e6 / 1024) * size / (tickEnd - tickStart) << "KB/s" << endl;
    } else {
        printf("Write the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* read test */
    tickStart = HPT_GetUs();
    result = sfud_read(flash, addr, size, data);
    tickEnd = HPT_GetUs();
    if (result == SFUD_SUCCESS) {
        printf("Read the %s flash data success. Start from 0x%08lX, size is %u. The data is:\r\n", flash->name, addr, size);
        // printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        // for (i = 0; i < size; i++) {
        //     if (i % 16 == 0) {
        //         printf("[%08lX] ", addr + i);
        //     }
        //     printf("%02X ", data[i]);
        //     if (((i + 1) % 16 == 0) || i == size - 1) {
        //         printf("\r\n");
        //     }
        // }
        printf("\r\n");
        cout << "time: " << tickEnd - tickStart << "us.\tSpeed: " << (double)(1e6 / 1024) * size / (tickEnd - tickStart) << "KB/s" << endl;
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