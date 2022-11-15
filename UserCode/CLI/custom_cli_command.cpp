/**
 * @file custom_cli_command.cpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2022-10-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "custom_cli_command.h"
#include "cmsis_os.h"
#include "FreeRTOS_CLI.h"
#include "sys_monitor.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include "ff.h"

using namespace std;

/**
 * 不可重入，请勿多线程调用
 */
static FRESULT PrintItems(const char *path)
{
    FRESULT res;
    static DIR dir;
    static FILINFO fno;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                  /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break; /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {
                /* It is a directory */
                printf("%s/\n", fno.fname);
            } else {
                /* It is a file. */
                printf("%s\n", fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}

static BaseType_t CMD_ll(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    (void)pcWriteBuffer;
    (void)xWriteBufferLen;
    (void)pcCommandString;
    PrintItems(".");
    return pdFALSE;
}

/**
 * 不可重入，请勿多线程调用
 */
static BaseType_t CMD_cat(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t xParameterStringLength;
    auto parameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

    static FIL *file_struct = nullptr;

    if (file_struct == nullptr) {
        file_struct = new FIL;
        auto res = f_open(file_struct, parameter, FA_READ);
        if (res != FR_OK) {
            strncpy(pcWriteBuffer, "Failed to open ", xWriteBufferLen);
            strncat(pcWriteBuffer, parameter, xWriteBufferLen - strnlen(pcWriteBuffer, xWriteBufferLen) - 2);
            strcat(pcWriteBuffer, "\n");
            delete file_struct;
            file_struct = nullptr;
            return pdFALSE;
        }
    }

    auto returned_str = f_gets(pcWriteBuffer, xWriteBufferLen, file_struct);
    if (returned_str != nullptr) {
        return pdTRUE;
    } else {
        f_close(file_struct);
        delete file_struct;
        file_struct = nullptr;
    }

    return pdFALSE;
}

static BaseType_t CMD_cd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t xParameterStringLength;
    auto parameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

    auto res = f_chdir(parameter);
    if (res != FR_OK) {
        strncpy(pcWriteBuffer, "No such directory.", xWriteBufferLen);
    }
    return pdFALSE;
}

static BaseType_t SysMonitorCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    (void)pcWriteBuffer;
    (void)xWriteBufferLen;
    (void)pcCommandString;

    if (SysMonitor::GetInstance().is_inited == false) {
        cout << "Starting SysMonitor..." << endl;
        SysMonitor::GetInstance().Init();
        osDelay(1000);
    }

    cout << SysMonitor::GetInstance();
    return pdFALSE;
}

#include "lcd_io_fmc.hpp"

static BaseType_t CMD_lcd_ad_mode(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    (void)pcWriteBuffer;
    (void)xWriteBufferLen;

    BaseType_t xParameterStringLength;
    auto parameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

    if (parameter != nullptr) {
        LcdIoFmc lcd;
        uint16_t data = atoi(parameter);

        printf("Writing %x\n", data);

        lcd.WriteCmd8(0x36U);
        lcd.WriteData8(data);
    }

    return pdFALSE;
}

void vRegisterCustomCLICommands()
{
    static const CLI_Command_Definition_t sys_monitor = {
        "sys_monitor",
        "sys_monitor: Show system info\n\n",
        SysMonitorCommand,
        0};
    FreeRTOS_CLIRegisterCommand(&sys_monitor);

    static const CLI_Command_Definition_t cd = {
        "cd",
        "cd: Change directory\n\n",
        CMD_cd,
        -1};
    FreeRTOS_CLIRegisterCommand(&cd);

    static const CLI_Command_Definition_t ll = {
        "ll",
        "ll: List all items in the current directory.\n\n",
        CMD_ll,
        0};
    FreeRTOS_CLIRegisterCommand(&ll);

    static const CLI_Command_Definition_t cat = {
        "cat",
        "cat: List all items in the current directory.\n\n",
        CMD_cat,
        -1};
    FreeRTOS_CLIRegisterCommand(&cat);

    static const CLI_Command_Definition_t lcd_ad_mode = {
        "lcd_ad_mode",
        "lcd_ad_mode: SET LCD ADDRESS MODE.\n\n",
        CMD_lcd_ad_mode,
        1};
    FreeRTOS_CLIRegisterCommand(&lcd_ad_mode);
}