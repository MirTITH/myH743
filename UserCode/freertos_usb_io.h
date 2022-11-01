#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void FreeRTOS_IO_Init();
void FreeRTOS_IO_RxCallback(char *pBuffer, int size);
void FreeRTOS_IO_TxCpltCallback();

int FreeRTOS_IO_WriteToSTDOUT(const char *pBuffer, int size);
int FreeRTOS_IO_WriteToSTDERR(const char *pBuffer, int size);
int FreeRTOS_IO_ReadFromSTDIN(char *pBuffer, int size);

#ifdef __cplusplus
}
#endif
