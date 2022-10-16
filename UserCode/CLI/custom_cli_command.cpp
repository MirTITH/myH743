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

using namespace std;

static BaseType_t SysMonitorCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    (void)pcWriteBuffer;
    (void)xWriteBufferLen;
    (void)pcCommandString;

    if (SysMonitor::GetInstance().GetInfo().temperature == 0) {
        cout << "Starting SysMonitor..." << endl;
        // 等待第一次转换完成
        osDelay(1000);
    }

    cout << SysMonitor::GetInstance();
    return pdFALSE;
}

void vRegisterCustomCLICommands()
{
    static const CLI_Command_Definition_t xTaskStats = {
        "sys_monitor",
        "sys_monitor: Show system info\r\n\r\n",
        SysMonitorCommand,
        0};
    FreeRTOS_CLIRegisterCommand(&xTaskStats);
}