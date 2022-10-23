/**
 * @file micro_second.h
 * @author X. Y.
 * @brief 高精度时间库
 * @version 0.1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t HPT_GetUs();
uint64_t HPT_GetNs();
void HPT_DelayUs(uint32_t us);

#ifdef __cplusplus
}
#endif
