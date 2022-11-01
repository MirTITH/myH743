#pragma once
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "sfud.h"

void StartDefaultTask(void const *argument);

uint8_t BSP_SD_IsDetected(void);

#ifdef __cplusplus
}
#endif
