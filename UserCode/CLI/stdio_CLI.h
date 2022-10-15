/**
 * @file stdio_CLI.h
 * @author X. Y.
 * @brief FreeRTOS 命令行，使用标准输入输出流
 * @version 0.1
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"

#define configCOMMAND_INT_MAX_OUTPUT_SIZE 512
#define configCOMMAND_INT_MAX_INPUT_SIZE 128

void CLI_Start(void);

#ifdef __cplusplus
}
#endif
