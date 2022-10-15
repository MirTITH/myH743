/**
 * @file freertos_io.c
 * @author X. Y.
 * @brief FreeRTOS IO 重定向
 * @version 0.1
 * @date 2022-10-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "freertos_io.h"
#include "string.h"
#include "cmsis_os.h"
#include "in_handle_mode.h"

#define USE_USB -1 // 不要改这行

/****************
 * 以下是待配置内容
 ****************/

#include "usart.h"
#include "usbd_cdc_if.h"

/**
 * IO 流重定向目标
 *
 */

#define IO_STDOUT USE_USB
#define IO_STDERR USE_USB
#define IO_STDIN  USE_USB

/**
 * 缓冲区设置
 *
 */

#define IO_STDIN_BufferSize 128

/****************
 * 以上是待配置内容
 ****************/

#if (defined IO_STDIN) && (IO_STDIN_BufferSize > 0)
#define _IO_USE_STDIN_Buffer
#endif

#ifdef _IO_USE_STDIN_Buffer
static QueueHandle_t StdinQueue = NULL;
#endif

void FreeRTOS_IO_Init()
{
#ifdef _IO_USE_STDIN_Buffer
    StdinQueue = xQueueCreate(IO_STDIN_BufferSize, sizeof(char));
#endif
}

static int FreeRTOS_IO_WriteToSTDOUT(char *pBuffer, int size)
{
#ifdef IO_STDOUT
#if (IO_STDOUT == USE_USB)
    // CDC_Transmit_FS((uint8_t *)pBuffer, (uint16_t)size);
    while (CDC_Transmit_FS((uint8_t *)pBuffer, (uint16_t)size) == USBD_BUSY) {
        taskYIELD();
    }
    return size;
#else

#endif
#endif
}

static int FreeRTOS_IO_WriteToSTDERR(char *pBuffer, int size)
{
#ifdef IO_STDERR
#if (IO_STDERR == USE_USB)
    while (CDC_Transmit_FS((uint8_t *)pBuffer, (uint16_t)size) == USBD_BUSY) {
        taskYIELD();
    }
    return size;
#else

#endif
#endif
}

static int FreeRTOS_IO_ReadFromSTDIN(char *pBuffer, int size)
{
    int received_size = 0;
    if (InHandlerMode()) {
        BaseType_t xTaskWokenByReceive = pdFALSE;
        while (received_size < size) {
            if (xQueueReceiveFromISR(StdinQueue, pBuffer, &xTaskWokenByReceive) == pdFALSE)
                break;
            pBuffer++;
            received_size++;
        }
        if (xTaskWokenByReceive != pdFALSE) {
            taskYIELD();
        }
    } else {
        while (received_size < size) {
            if (xQueueReceive(StdinQueue, pBuffer, portMAX_DELAY) == pdFALSE)
                break;
            pBuffer++;
            received_size++;
        }
    }
    return received_size;
}

void FreeRTOS_IO_RxCallback(char *pBuffer, int size)
{
#ifdef IO_STDIN
#if (IO_STDIN == USE_USB)
#ifdef _IO_USE_STDIN_Buffer
    if (InHandlerMode()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        while (size > 0) {
            if (xQueueSendFromISR(StdinQueue, pBuffer, &xHigherPriorityTaskWoken) == pdFALSE)
                break;
            pBuffer++;
            size--;
        }
        if (xHigherPriorityTaskWoken != pdFALSE) {
            taskYIELD();
        }
    } else {
        while (size > 0) {
            if (xQueueSend(StdinQueue, pBuffer, portMAX_DELAY) == pdFALSE)
                break;
            pBuffer++;
            size--;
        }
    }
#else
#error You must use stdin buffer when using USB for stdin.
#endif
#else

#endif
#endif
}

// 判断是哪个编译器。GCC 会定义 __GNUC__，ARMCCv5 会定义 __ARMCC_VERSION，ARMCCv6 会定义 __GNUC__ 和 __ARMCC_VERSION
#if (defined __GNUC__) && (!defined __ARMCC_VERSION)

// GCC 编译器

#include <errno.h>
#include <sys/unistd.h>

/**
 * @brief Writes data to a file.
 *
 * @param fd File descriptor of file into which data is written.
 * @param pBuffer Data to be written.
 * @param size Number of bytes.
 * @return 成功则返回写入的字节数，否则返回 -1
 */
__attribute__((used)) int _write(int fd, char *pBuffer, int size)
{
    // return IO_Write(fd, (uint8_t *)pBuffer, size);
    switch (fd) {
        case STDOUT_FILENO: // 标准输出流
            return FreeRTOS_IO_WriteToSTDOUT(pBuffer, size);
            break;
        case STDERR_FILENO: // 标准错误流
            return FreeRTOS_IO_WriteToSTDERR(pBuffer, size);
            break;
        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            return -1;
            break;
    }
    return size;
}

/**
 * @brief Reads data from a file
 *
 * @param fd File descriptor referring to the open file.
 * @param pBuffer Storage location for data.
 * @param size Maximum number of bytes to read.
 */
__attribute__((used)) int _read(int fd, char *pBuffer, int size)
{
    if (size <= 0) {
        return 0;
    }

    switch (fd) {
        case STDIN_FILENO:
            return FreeRTOS_IO_ReadFromSTDIN(pBuffer, 1);
            break;
        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            return -1;
            break;
    }
}

#else

// MDK 的 ARMCC 编译器

#include <stdio.h>
#include <rt_sys.h>
#include <rt_misc.h>
#include <time.h>

/**
 *
 * @brief 将字符写入流
 *
 * @param ch 要写入的字符
 * @param stream 流
 * @return 返回写入的字符，但如果出现错误，返回 EOF
 */
int fputc(int ch, FILE *stream)
{
    IORetarget_WriteStr(stdout_huart, (uint8_t *)&ch, 1);
    return ch;
}

int fgetc(FILE *stream)
{
    (void)stream;
    char ch;
#if (defined IORETARGET_STDIN_BUFFER_SIZE) && (IORETARGET_STDIN_BUFFER_SIZE > 0)
    IORetarget_ReadStr(&stdinBufferQueue, &ch, 1);
#else
    ch = IORetarget_ReadChar(stdin_huart);
#endif
    return (int)ch;
}

// ARMCC 默认启用半主机模式，重定向 printf 后需要关闭，防止卡死
// ARMCCv6 和 ARMCCv5 关闭半主机模式的代码不一样，需要判断
#if __ARMCC_VERSION >= 6000000
// ARMCCv6
__asm(".global __use_no_semihosting");
#elif __ARMCC_VERSION >= 5000000
// ARMCCv5
#pragma import(__use_no_semihosting)
#else
#error Unsupported compiler
#endif

// 关闭半主机模式后，需要自己实现一些相关系统函数

const char __stdin_name[] = ":tt";
const char __stdout_name[] = ":tt";
const char __stderr_name[] = ":tt";

FILEHANDLE _sys_open(const char *name, int openmode)
{
    return 1;
}

int _sys_close(FILEHANDLE fh)
{
    return 0;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
    return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
    return -1;
}

void _ttywrch(int ch)
{
}

int _sys_istty(FILEHANDLE fh)
{
    return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    return -1;
}

long _sys_flen(FILEHANDLE fh)
{
    return -1;
}

void _sys_exit(int return_code)
{
    while (1)
        ;
}

clock_t clock(void)
{
    clock_t tmp;
    return tmp;
}

void _clock_init(void)
{
}

time_t time(time_t *timer)
{
    time_t tmp;
    return tmp;
}

int system(const char *string)
{
    return 0;
}

char *getenv(const char *name)
{
    return NULL;
}

void _getenv_init(void)
{
}
#endif
