/**
 * @file freertos_io.h
 * @author X. Y.
 * @brief FreeRTOS IO 重定向
 * @version 0.1
 * @date 2022-10-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void FreeRTOS_IO_Init();
void FreeRTOS_IO_RxCallback(char *pBuffer, int size);
void FreeRTOS_IO_TxCpltCallback();

#ifdef __cplusplus
}
#endif
