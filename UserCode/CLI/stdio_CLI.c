/**
 * @file stdio_CLI.c
 * @author X. Y.
 * @brief FreeRTOS 命令行，使用标准输入输出流
 * @version 0.1
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "stdio_CLI.h"
#include "cmsis_os.h"
#include "FreeRTOS_CLI.h"
#include <stdio.h>
#include <string.h>

extern void vRegisterSampleCLICommands(void);

// 输入缓冲
static char inputBuffer[configCOMMAND_INT_MAX_INPUT_SIZE] = {0};

// 欢迎信息
const char helloStr[] = "\
FreeRTOS command server.\r\n\
Type Help to view a list of registered commands.\r\n\
Love from WTRobot.\r\n\
";

inline static void CLI_GetStr(char *buffer, int size)
{
    fgets(buffer, size, stdin);

    size_t buffer_length = strlen(buffer);

    if (buffer[buffer_length - 1] == '\n' || buffer[buffer_length - 1] == '\r') {
        buffer[buffer_length - 1] = '\0';
        if (buffer[buffer_length - 2] == '\n' || buffer[buffer_length - 2] == '\r') {
            buffer[buffer_length - 2] = '\0';
        }
    }
}

inline static void CLI_PutStr(const char *buffer)
{
    puts(buffer);
}

static void CLI_ThreadEntry(void const *argument)
{
    (void)argument;
    char *outputBuffer = FreeRTOS_CLIGetOutputBuffer();
    BaseType_t xReturned;
    vRegisterCustomCLICommands();
    vRegisterSampleCLICommands();
    CLI_PutStr(helloStr);

    for (;;) {
        CLI_GetStr(inputBuffer, configCOMMAND_INT_MAX_INPUT_SIZE);
        CLI_PutStr(inputBuffer);

        do {
            /* Get the next output string from the command interpreter. */
            xReturned = FreeRTOS_CLIProcessCommand(inputBuffer, outputBuffer, configCOMMAND_INT_MAX_OUTPUT_SIZE);

            /* Write the generated string to the UART. */
            CLI_PutStr(outputBuffer);
            outputBuffer[0] = '\0';
        } while (xReturned != pdFALSE);
    }
}

void CLI_Start(void)
{
    osThreadDef_t cliThreadDef = {
        .name = (char *)"CLI",
        .pthread = CLI_ThreadEntry,
        .tpriority = osPriorityBelowNormal,
        .instances = 0,
        .stacksize = 512,
        .buffer = NULL,
        .controlblock = NULL};

    osThreadCreate(&cliThreadDef, NULL);
}
