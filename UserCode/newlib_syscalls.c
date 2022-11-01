/**
 * @file newlib_syscalls.cpp
 * @author X. Y.
 * @brief Newlib's definitions for OS interface
 * @version 0.1
 * @date 2022-11-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <errno.h>
#include <sys/unistd.h>
#include "freertos_usb_io.h"

/**
 * @brief Writes data to a file.
 *
 * @param fd File descriptor of file into which data is written.
 * @param buf Data to be written.
 * @param cnt Number of bytes.
 * @return 成功则返回写入的字节数，否则返回 -1
 */
int _write(int file, char *ptr, int len)
{
    int result = 0;
    switch (file) {
        case STDOUT_FILENO: // 标准输出流
            result = FreeRTOS_IO_WriteToSTDOUT(ptr, len);
            errno = 0;
            break;
        case STDERR_FILENO: // 标准错误流
            result = FreeRTOS_IO_WriteToSTDERR(ptr, len);
            errno = 0;
            break;
        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            result = -1;
            break;
    }
    return result;
}

/**
 * @brief Reads data from a file
 *
 * @param file File descriptor referring to the open file.
 * @param ptr Storage location for data.
 * @param len Maximum number of bytes to read.
 */
int _read(int file, char *ptr, int len)
{
    if (len <= 0) {
        return 0;
    }

    switch (file) {
        case STDIN_FILENO:
            errno = 0;
            return FreeRTOS_IO_ReadFromSTDIN(ptr, 1);
            break;
        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            return -1;
            break;
    }
}
