#pragma once

#include "main.h"

class LcdIoFmc
{
private:
    /**
     * LCD_BASE_ADDRESS 要根据 Chip Select 修改为对应的值
     * NE1 0x60000000
     * NE2 0x64000000
     * NE3 0x68000000
     * NE4 0x6C000000
     * REGSELECT_BIT: for example: A18 pin -> 18
     */
    const uint32_t LCD_BASE = 0x60000000;

    // 背光控制引脚
    GPIO_TypeDef *const BL_GPIOX = GPIOC;
    const uint16_t BL_GPIO_PIN = GPIO_PIN_6;
    const bool BL_ON_LEVEL = true;

    // RESET 引脚
    GPIO_TypeDef *const RESET_GPIOX = LcdReset_GPIO_Port;
    const uint16_t RESET_GPIO_PIN = LcdReset_Pin;

    volatile uint16_t *const LCD_COMMAND_ADDRESS = (uint16_t *)LCD_BASE;
    volatile uint16_t *const LCD_DATA_ADDRESS = (uint16_t *)(LCD_BASE + (0x4000000 - 2));

public:
    LcdIoFmc(){};
    ~LcdIoFmc(){};
    void Init();
    void HardReset();
    void WriteCmd8(uint8_t cmd);
    void WriteCmd16(uint16_t cmd);
    void WriteData8(uint8_t data);
    void WriteData16(uint16_t data);
    void SetBacklight(uint32_t lightness);
};
