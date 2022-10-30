/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include "quadspi.h"
#include "cmsis_os.h"
#include "in_handle_mode.h"
#include "high_precision_time.h"

typedef struct
{
    QSPI_HandleTypeDef *pqspi;
    SemaphoreHandle_t mutex;
} SFUD_UserData_t;

static SFUD_UserData_t qspiUserData;

void sfud_log_info(const char *format, ...);

static void qspi_lock(const sfud_spi *spi)
{
    SemaphoreHandle_t mutex = ((SFUD_UserData_t *)spi->user_data)->mutex;
    // 判断是否在中断中
    if (InHandlerMode()) {
        // 在中断中
        // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // xSemaphoreTakeFromISR(sem, &xHigherPriorityTaskWoken);
        // // 判断是否有需要切换的线程。如果有，中断结束后会立即切换线程，提高实时性
        // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        // 在线程中
        xSemaphoreTake(mutex, portMAX_DELAY);
    }
}

static void qspi_unlock(const sfud_spi *spi)
{
    SemaphoreHandle_t mutex = ((SFUD_UserData_t *)spi->user_data)->mutex;
    // 判断是否在中断中
    if (InHandlerMode()) {
        // 在中断中
        // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
        // // 判断是否有需要切换的线程。如果有，中断结束后会立即切换线程，提高实时性
        // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        // 在线程中
        xSemaphoreGive(mutex);
    }
}

/**
 * This function can send or send then receive QSPI data.
 */
sfud_err qspi_send_then_recv(QSPI_HandleTypeDef *pqspi, const void *send_buf, size_t send_length, void *recv_buf, size_t recv_length)
{
    assert_param(send_buf);
    assert_param(recv_buf);
    assert_param(send_length != 0);

    QSPI_CommandTypeDef Cmdhandler;
    unsigned char *ptr = (unsigned char *)send_buf;
    size_t count = 0;
    sfud_err result = SFUD_SUCCESS;

    /* get instruction */
    Cmdhandler.Instruction = ptr[0];
    Cmdhandler.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    count++;

    /* get address */
    if (send_length > 1) {
        if (send_length >= 4) {
            /* address size is 3 Byte */
            Cmdhandler.Address = (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
            Cmdhandler.AddressSize = QSPI_ADDRESS_24_BITS;
            count += 3;
        } else {
            return SFUD_ERR_READ;
        }
        Cmdhandler.AddressMode = QSPI_ADDRESS_1_LINE;
    } else {
        /* no address stage */
        Cmdhandler.Address = 0;
        Cmdhandler.AddressMode = QSPI_ADDRESS_NONE;
        Cmdhandler.AddressSize = 0;
    }

    Cmdhandler.AlternateBytes = 0;
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    Cmdhandler.AlternateBytesSize = 0;

    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

    if (send_buf && recv_buf) {
        /* recv data */
        /* set dummy cycles */
        if (count != send_length) {
            Cmdhandler.DummyCycles = (send_length - count) * 8;
        } else {
            Cmdhandler.DummyCycles = 0;
        }

        /* set recv size */
        Cmdhandler.DataMode = QSPI_DATA_1_LINE;
        Cmdhandler.NbData = recv_length;
        HAL_QSPI_Command(pqspi, &Cmdhandler, 5000);

        if (recv_length != 0) {
            if (HAL_QSPI_Receive(pqspi, recv_buf, 5000) != HAL_OK) {
                sfud_log_info("qspi recv data failed(%d)!", pqspi->ErrorCode);
                pqspi->State = HAL_QSPI_STATE_READY;
                result = SFUD_ERR_READ;
            }
        }

        return result;
    } else {
        /* send data */
        /* set dummy cycles */
        Cmdhandler.DummyCycles = 0;

        /* determine if there is data to send */
        if (send_length - count > 0) {
            Cmdhandler.DataMode = QSPI_DATA_1_LINE;
        } else {
            Cmdhandler.DataMode = QSPI_DATA_NONE;
        }

        /* set send buf and send size */
        Cmdhandler.NbData = send_length - count;
        HAL_QSPI_Command(pqspi, &Cmdhandler, 5000);

        if (send_length - count > 0) {
            if (HAL_QSPI_Transmit(pqspi, (uint8_t *)(ptr + count), 5000) != HAL_OK) {
                sfud_log_info("qspi send data failed(%d)!", pqspi->ErrorCode);
                pqspi->State = HAL_QSPI_STATE_READY;
                result = SFUD_ERR_WRITE;
            }
        }

        return result;
    }
}

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size)
{
    sfud_err result = SFUD_SUCCESS;

    // spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

    /* reset cs pin */
    // if (spi_dev->cs_gpiox != NULL)
    //     HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_RESET);

    if (write_size && read_size) {
        /* read data */
        qspi_send_then_recv(((SFUD_UserData_t *)(spi->user_data))->pqspi, write_buf, write_size, read_buf, read_size);
    } else if (write_size) {
        /* send data */
        qspi_send_then_recv(((SFUD_UserData_t *)(spi->user_data))->pqspi, write_buf, write_size, NULL, 0);
    }

    /* set cs pin */
    // if (spi_dev->cs_gpiox != NULL)
    //     HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_SET);

    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size)
{
    QSPI_HandleTypeDef *pqspi = ((SFUD_UserData_t *)(spi->user_data))->pqspi;
    sfud_err result = SFUD_SUCCESS;
    QSPI_CommandTypeDef Cmdhandler;

    /* set cmd struct */
    Cmdhandler.Instruction = qspi_read_cmd_format->instruction;
    if (qspi_read_cmd_format->instruction_lines == 0) {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_NONE;
    } else if (qspi_read_cmd_format->instruction_lines == 1) {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    } else if (qspi_read_cmd_format->instruction_lines == 2) {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_2_LINES;
    } else if (qspi_read_cmd_format->instruction_lines == 4) {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    }

    Cmdhandler.Address = addr;
    Cmdhandler.AddressSize = QSPI_ADDRESS_24_BITS;
    if (qspi_read_cmd_format->address_lines == 0) {
        Cmdhandler.AddressMode = QSPI_ADDRESS_NONE;
    } else if (qspi_read_cmd_format->address_lines == 1) {
        Cmdhandler.AddressMode = QSPI_ADDRESS_1_LINE;
    } else if (qspi_read_cmd_format->address_lines == 2) {
        Cmdhandler.AddressMode = QSPI_ADDRESS_2_LINES;
    } else if (qspi_read_cmd_format->address_lines == 4) {
        Cmdhandler.AddressMode = QSPI_ADDRESS_4_LINES;
    }

    Cmdhandler.AlternateBytes = 0;
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    Cmdhandler.AlternateBytesSize = 0;

    Cmdhandler.DummyCycles = qspi_read_cmd_format->dummy_cycles;

    Cmdhandler.NbData = read_size;
    if (qspi_read_cmd_format->data_lines == 0) {
        Cmdhandler.DataMode = QSPI_DATA_NONE;
    } else if (qspi_read_cmd_format->data_lines == 1) {
        Cmdhandler.DataMode = QSPI_DATA_1_LINE;
    } else if (qspi_read_cmd_format->data_lines == 2) {
        Cmdhandler.DataMode = QSPI_DATA_2_LINES;
    } else if (qspi_read_cmd_format->data_lines == 4) {
        Cmdhandler.DataMode = QSPI_DATA_4_LINES;
    }

    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    HAL_QSPI_Command(pqspi, &Cmdhandler, 5000);

    if (HAL_QSPI_Receive(pqspi, read_buf, 5000) != HAL_OK) {
        sfud_log_info("qspi recv data failed(%d)!", pqspi->ErrorCode);
        pqspi->State = HAL_QSPI_STATE_READY;
        result = SFUD_ERR_READ;
    }

    return result;
}
#endif /* SFUD_USING_QSPI */

static void retry_delay(void)
{
    HPT_DelayUs(100);
}

sfud_err sfud_spi_port_init(sfud_flash *flash)
{
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */

    switch (flash->index) {
        case SFUD_W25Q64_DEVICE_INDEX: {
            /* set the interfaces and data */
            flash->spi.wr = spi_write_read;
            flash->spi.qspi_read = qspi_read;
            flash->spi.lock = qspi_lock;
            flash->spi.unlock = qspi_unlock;
            flash->spi.user_data = &qspiUserData;

            flash->retry.delay = retry_delay;
            flash->retry.times = 6 * 10000;

            extern QSPI_HandleTypeDef hqspi;
            qspiUserData.mutex = xSemaphoreCreateMutex();
            qspiUserData.pqspi = &hqspi;
            break;
        }
    }
    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}
