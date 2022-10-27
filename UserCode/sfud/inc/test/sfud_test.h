#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sfud.h"

/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
void sfud_demo(const sfud_flash *flash, uint32_t addr, size_t size, uint8_t *data);

#ifdef __cplusplus
}
#endif
