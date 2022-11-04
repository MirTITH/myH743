#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include <string>
#include <iostream>
#include "freertos_usb_io.h"
#include "stdio_CLI.h"
#include "high_precision_time.h"
#include "ff.h"
#include <nlohmann/json.hpp>
#include "ESP32_ILI9481.h"
#include <stdlib.h>

using namespace std;
using json = nlohmann::json;

// static FATFS fs;

// #define ReadBufferSize 40960
// static TCHAR ReadBuffer[ReadBufferSize];

LCD_ILI9481 LcdClass;

static void USB_Reset()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    /*Configure GPIO pin : PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_Delay(65);
    //先把PA12拉低再拉高，利用D+模拟USB的拔插动作
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_Delay(65);
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    USB_Reset();
    MX_USB_DEVICE_Init();

    FreeRTOS_IO_Init();

    // 等待 USB 初始化完成
    osDelay(500);
    // osDelay(2000); // 使得用户有足够的时间打开 USB 串口，防止错过开头的消息。

    CLI_Start();

    LcdClass.initializeDisplay();

    // LcdClass.drawRectangle(10,20,100,200,0x1234);

    // LcdClass.drawPixel(310,470,0xf800);
    uint8_t r = 0, g = 0, b = 0;
    uint16_t color;

    for (;;) {

        uint32_t start_tick = HPT_GetUs();
        
        for (int i = 0; i < 10; i++) {
            r += 10;
            g += 3;
            b -= 7;
            color = ((r >> (8 - 5)) << (6 + 5)) | ((g >> (8 - 6)) << (5)) | ((b >> (8 - 5)) << (0));
            LcdClass.fillScreen(color);
        }

        uint32_t end_tick = HPT_GetUs();
        cout << 10 * 1e6 / (end_tick - start_tick) << " fps" << endl;
        // LcdClass.fillScreen(0xaafa);
        // LcdClass.fillScreen(0xaaaf);
        // osDelay(500);
    }
}
