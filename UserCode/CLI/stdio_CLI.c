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
#include "custom_cli_command.h"
#include "cmsis_os.h"
#include "FreeRTOS_CLI.h"
#include <stdio.h>
#include <string.h>
#include "ff.h"

extern void vRegisterSampleCLICommands(void);

// 输入缓冲
static char inputBuffer[configCOMMAND_INT_MAX_INPUT_SIZE] = {0};

// 欢迎信息
const char helloStr[] = "\
FreeRTOS command server.\n\
Type Help to view a list of registered commands.\n\
❤ from WTRobot.\n";

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

inline static void CLI_PutStrAndNewLine(const char *buffer)
{
    puts(buffer);
}

inline static void CLI_PutStr(const char *buffer)
{
    printf("%s", buffer);
    fflush(stdout);
}

static void CLI_EchoNewLine(char *buffer, uint buffer_size)
{
    printf("\n");
    f_getcwd(buffer, buffer_size);
    const char END_STR[] = ">";
    strncat(buffer, END_STR, buffer_size - strnlen(buffer, buffer_size) - 1);
    printf("%s", buffer);
    fflush(stdout);
}

static void CLI_ThreadEntry(void const *argument)
{
    (void)argument;
    char *outputBuffer = FreeRTOS_CLIGetOutputBuffer();
    BaseType_t xReturned;
    vRegisterCustomCLICommands();
    vRegisterSampleCLICommands();
    CLI_PutStrAndNewLine(helloStr);
    CLI_EchoNewLine(outputBuffer, configCOMMAND_INT_MAX_OUTPUT_SIZE);
    outputBuffer[0] = '\0';

    for (;;) {
        CLI_GetStr(inputBuffer, configCOMMAND_INT_MAX_INPUT_SIZE);
        CLI_PutStrAndNewLine(inputBuffer);

        do {
            /* Get the next output string from the command interpreter. */
            xReturned = FreeRTOS_CLIProcessCommand(inputBuffer, outputBuffer, configCOMMAND_INT_MAX_OUTPUT_SIZE);

            /* Write the generated string to the UART. */
            CLI_PutStr(outputBuffer);
            outputBuffer[0] = '\0';
        } while (xReturned != pdFALSE);

        CLI_EchoNewLine(outputBuffer, configCOMMAND_INT_MAX_OUTPUT_SIZE);
        outputBuffer[0] = '\0';
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
