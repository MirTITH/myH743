/**
 * @file freertos_usb_io.c
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2022-11-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "freertos_usb_io.h"
#include "string.h"
#include "cmsis_os.h"
#include "in_handle_mode.h"

#define USE_USB -1 // 不要改这行

/****************
 * 以下是待配置内容
 ****************/

#include "usbd_cdc_if.h"

// IO 流重定向目标

#define IO_STDOUT USE_USB
#define IO_STDERR USE_USB
#define IO_STDIN  USE_USB

// 缓冲区设置

#define IO_STDIN_BufferSize 128

// 超时时间设置

static TickType_t MAX_TX_BLOCK_TICK = 10;            // 发送的最大阻塞时间 (ms)
static TickType_t MAX_RX_BLOCK_TICK = portMAX_DELAY; // 接收的最大阻塞时间 (ms)
static TickType_t MAX_CALLBACK_BLOCK_TICK = 10;      // 回调函数的最大阻塞时间（在中断中调用一定不会阻塞） (ms)

/****************
 * 以上是待配置内容
 ****************/

static SemaphoreHandle_t OutputSem;

#ifdef IO_STDIN
static QueueHandle_t StdinQueue = NULL;
#endif

void FreeRTOS_IO_Init()
{
    OutputSem = xSemaphoreCreateBinary();
    xSemaphoreGive(OutputSem);
#ifdef IO_STDIN
    StdinQueue = xQueueCreate(IO_STDIN_BufferSize, sizeof(char));
#endif
}

int FreeRTOS_IO_WriteToSTDOUT(const char *pBuffer, int size)
{
#ifdef IO_STDOUT
#if (IO_STDOUT == USE_USB)
    // int sent_size = 0;
    int sent_size = size; // 欺骗标准库，让它以为成功发送了。否则需要处理异常机制

    // 判断是否在中断中
    if (InHandlerMode()) {
        // 在中断中
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // 从中断中获取信号量，如果成功获取，则说明可以发送；
        // 如果不能获取，则说明发送处于 busy 状态。由于中断要求尽可能短地执行，所以放弃此次发送
        if (xSemaphoreTakeFromISR(OutputSem, &xHigherPriorityTaskWoken) == pdTRUE) {
            if (CDC_Transmit_FS((uint8_t *)pBuffer, (uint16_t)size) == USBD_OK) {
                // 发送成功
                sent_size = size;
            } else {
                // 发送失败，不会进入回调函数，所以要在这里释放信号量，防止卡死
                xSemaphoreGiveFromISR(OutputSem, &xHigherPriorityTaskWoken);
            }
        }

        // 判断是否有需要切换的线程。如果有，中断结束后会立即切换线程，提高实时性
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        // 在线程中
        if (xSemaphoreTake(OutputSem, MAX_TX_BLOCK_TICK) == pdTRUE) {
            if (CDC_Transmit_FS((uint8_t *)pBuffer, (uint16_t)size) == USBD_OK) {
                sent_size = size;
            } else {
                xSemaphoreGive(OutputSem);
            }
        }
    }

    return sent_size;
#else

#endif
#endif
}

int FreeRTOS_IO_WriteToSTDERR(const char *pBuffer, int size)
{
#ifdef IO_STDERR
#if (IO_STDERR == USE_USB)
    // int sent_size = 0;
    int sent_size = size; // 欺骗标准库，让它以为成功发送了。否则需要处理异常机制

    // 判断是否在中断中
    if (InHandlerMode()) {
        // 在中断中
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // 从中断中获取信号量，如果成功获取，则说明可以发送；
        // 如果不能获取，则说明发送处于 busy 状态。由于中断要求尽可能短地执行，所以放弃此次发送
        if (xSemaphoreTakeFromISR(OutputSem, &xHigherPriorityTaskWoken) == pdTRUE) {
            if (CDC_Transmit_FS((uint8_t *)pBuffer, (uint16_t)size) == USBD_OK) {
                // 发送成功
                sent_size = size;
            } else {
                // 发送失败，不会进入回调函数，所以要在这里释放信号量，防止卡死
                xSemaphoreGiveFromISR(OutputSem, &xHigherPriorityTaskWoken);
            }
        }

        // 判断是否有需要切换的线程。如果有，中断结束后会立即切换线程，提高实时性
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        // 在线程中
        if (xSemaphoreTake(OutputSem, MAX_TX_BLOCK_TICK) == pdTRUE) {
            if (CDC_Transmit_FS((uint8_t *)pBuffer, (uint16_t)size) == USBD_OK) {
                sent_size = size;
            } else {
                xSemaphoreGive(OutputSem);
            }
        }
    }

    return sent_size;
#else

#endif
#endif
}

int FreeRTOS_IO_ReadFromSTDIN(char *pBuffer, int size)
{
#ifdef IO_STDIN
    int received_size = 0;
    if (InHandlerMode()) {
        BaseType_t xTaskWokenByReceive = pdFALSE;
        while (received_size < size) {
            if (xQueueReceiveFromISR(StdinQueue, pBuffer, &xTaskWokenByReceive) == pdFALSE)
                break;
            pBuffer++;
            received_size++;
        }
        portYIELD_FROM_ISR(xTaskWokenByReceive);
    } else {
        while (received_size < size) {
            if (xQueueReceive(StdinQueue, pBuffer, MAX_RX_BLOCK_TICK) == pdFALSE)
                break;
            pBuffer++;
            received_size++;
        }
    }
    return received_size;
#else
    return 0;
#endif
}

void FreeRTOS_IO_TxCpltCallback()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (InHandlerMode()) {
        xSemaphoreGiveFromISR(OutputSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xSemaphoreGive(OutputSem);
    }
}

void FreeRTOS_IO_RxCallback(char *pBuffer, int size)
{
#ifdef IO_STDIN
    if (InHandlerMode()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        while (size > 0) {
            if (xQueueSendFromISR(StdinQueue, pBuffer, &xHigherPriorityTaskWoken) == pdFALSE)
                break;
            pBuffer++;
            size--;
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        while (size > 0) {
            if (xQueueSend(StdinQueue, pBuffer, MAX_CALLBACK_BLOCK_TICK) == pdFALSE)
                break;
            pBuffer++;
            size--;
        }
    }
#endif
}
