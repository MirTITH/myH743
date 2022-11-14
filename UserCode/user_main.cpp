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
#include "STM32_ILI9481.hpp"
#include <stdlib.h>

using namespace std;
using json = nlohmann::json;

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

void TestThread(void const *argument)
{
    (void)argument;

    int intensity = 0;
    int delta_intensity = 1;

    for (;;) {
        intensity += delta_intensity;
        if (intensity >= 1000) {
            delta_intensity = -1;
        }

        if (intensity <= 0) {
            delta_intensity = 1;
        }

        LcdClass.lcd.SetBacklight(intensity);

        osDelay(1);
    }
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    USB_Reset();
    MX_USB_DEVICE_Init();

    FreeRTOS_IO_Init();

    // 等待 USB 初始化完成
    osDelay(500);
    osDelay(2000); // 使得用户有足够的时间打开 USB 串口，防止错过开头的消息。

    CLI_Start();

    LcdClass.initializeDisplay();

    osThreadDef_t test_thread_def = {
        .name = (char *)"test_thread",
        .pthread = TestThread,
        .tpriority = osPriorityNormal,
        .instances = 0,
        .stacksize = 512,
        .buffer = NULL,
        .controlblock = NULL};

    osThreadCreate(&test_thread_def, NULL);

    for (;;) {
        int leftA = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
        int leftB = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1);
        int leftButton = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);

        cout << leftA << ' ' << leftB << ' ' << leftButton;
        cout << endl;

        osDelay(1);
    }
}
