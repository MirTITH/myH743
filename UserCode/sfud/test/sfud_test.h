#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sfud.h"

/**
 * SFUD write and read test. Warning: this test will write data to the flash. 
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
void sfud_WriteReadTest(const sfud_flash *flash, uint32_t addr, size_t size, uint8_t *data);

/**
 * SFUD read test. 
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
void sfud_ReadTest(const sfud_flash *flash, uint32_t addr, size_t size, uint8_t *data);

#ifdef __cplusplus
}
#endif
