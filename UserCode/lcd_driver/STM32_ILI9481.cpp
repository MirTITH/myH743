/**
 * @file ESP32_ILI9481.cpp
 * @author X. Y.  
 * @brief 
 * @version 0.1
 * @date 2022-11-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "STM32_ILI9481.hpp"

//========================================================================//
// LCD commands

#define SOFT_RESET                  0x01U
#define SET_COL_ADDRESS             0x2AU
#define SET_PAGE_ADDRESS            0x2BU
#define WRITE_MEMORY_START          0x2CU
#define WRITE_MEMORY_CONTINUE       0x3CU
#define SET_DISPLAY_OFF             0x28U
#define SET_DISPLAY_ON              0x29U
#define SET_ADDRESS_MODE            0x36U
#define EXIT_INVERT_MODE            0x20U
#define ENTER_INVERT_MODE           0x21U
#define ENTER_NORMAL_MODE           0x13U // 0 param
#define EXIT_SLEEP_MODE             0x11U
#define SET_TEAR_OFF                0x34U // 0 param
#define SET_TEAR_ON                 0x35U // 1 param
#define SET_PIXEL_FORMAT            0x3AU
#define SET_TEAR_SCANLINE           0x44U // 2 param
#define FRAME_MEMORY_ACCESS_SETTING 0xB3U // 4 param
#define SET_DISPLAY_MODE            0xB4U // 1 param
#define PANEL_DRIVE_SETTING         0xC0U // 6 param
#define TIMING_SETTING_NORMAL       0xC1U // 3 param
#define TIMING_SETTING_PARTIAL      0xC2U // 3 param
#define FRAME_RATE_CONTROL          0xC5U // 1 param
#define INTERFACE_CONTROL           0xC6U // 1 param
#define POWER_SETTING               0xD0U // 3 param
#define VCOM_CONTROL                0xD1U // 3 param
#define POWER_SETTING_NORMAL        0xD2U // 2 param
#define POWER_SETTING_PARTIAL       0xD3U // 2 param
#define GAMMA_SETTING               0xC8U // 12 param

#define _swap_int16_t(a, b) \
    {                       \
        int16_t t = a;      \
        a = b;              \
        b = t;              \
    } // swaps two 16 bit values

//========================================================================//
// RGB565 color values

#define DISABLED_FG 0xEF3C
#define DISABLED_BG 0xCE59

//========================================================================//
// LCD functions

LCD_ILI9481::LCD_ILI9481()
{
}

//------------------------------------------------------------------------//
// initializes the display with proper reset sequence

void LCD_ILI9481::initializeDisplay()
{
    lcd.Init();
    startDisplay();
}

//------------------------------------------------------------------------//
// starts the display with required configurations

void LCD_ILI9481::startDisplay()
{
    writeCommand(SOFT_RESET);
    delay(100);

    writeCommand(EXIT_SLEEP_MODE); // exit sleep mode
    delay(100);

    writeCommand(POWER_SETTING); // power setting
    writeData8(0x07U);
    writeData8(0x41U);
    writeData8(0x16U);

    writeCommand(VCOM_CONTROL); // VCOM control
    writeData8(0x00U);
    writeData8(0x04U);
    writeData8(0x1fU);

    writeCommand(POWER_SETTING_NORMAL); // power setting for normal mode
    writeData8(0x01U);
    writeData8(0x00U); // Fosc setting

    writeCommand(PANEL_DRIVE_SETTING); // panel driving setting - 6 parameters
    writeData8(0x00);                  // D4: REV; D3: SM; D2: GS
    writeData8(0x3B);                  // D[0-5]: NL - Sets the number of lines to drive the LCD at an interval of 8 lines.
    writeData8(0x00);                  // D[0-6]: SCN - scanning start position
    writeData8(0x00);                  // D0: PTV: Sets the Vcom output in non-display area drive period.
    writeData8(0x02);                  // D4: NDL (non-display area o/p level); D[0-3]: PTS
    writeData8(0x11);                  // D4: PTG, D[0-3]: ISC[4]=0001 (3 frames)

    writeCommand(FRAME_RATE_CONTROL); // frame rate and inversion control - 1 parameter
    /**
     * value: fps
     * 0x00:  125
     * 0x01:  100
     * 0x02:  85 (default)
     * 0x03:  72
     * 0x04:  56
     * 0x05:  50
     * 0x06:  45
     * 0x07:  42
     */
    writeData8(0x05);

    // writeCommand(FRAME_MEMORY_ACCESS_SETTING); //frame memeory access and interface setting
    //   writeData8(0x0U); //extra data is ignored
    //   writeData8(0x0U); //all defaults
    //   writeData8(0x0U);
    //   writeData8(0x0U);

    // writeCommand(SET_TEAR_OFF); //tearing compensation

    // writeCommand(TIMING_SETTING_NORMAL); //display timing for normal mode
    //   writeData8(0x10U); //BC=0 (frame inversion waveform)
    //   writeData8(0x10U); //line period is 16 clocks
    //   writeData8(0x22U); //front and back porch lines = 2

    // writeCommand(GAMMA_SETTING); // gamma setting
    // writeData8(0x00);
    // writeData8(0x01);
    // writeData8(0x47);
    // writeData8(0x60);
    // writeData8(0x04);
    // writeData8(0x16);
    // writeData8(0x03);
    // writeData8(0x67);
    // writeData8(0x67);
    // writeData8(0x06);
    // writeData8(0x0F);
    // writeData8(0x00);

    setRotation(Rotation::landscape);

    writeCommand(SET_PIXEL_FORMAT); // set pixel format - 1 parameter
    writeData8(0x55);               // 0x55: 16-bit per pixel; 0x66: 18-bit per pixel

    writeCommand(ENTER_NORMAL_MODE); // eneter normal mode - no param

    displayOn();
    delay(120);
}

//------------------------------------------------------------------------//
// writes 8 bit data

void LCD_ILI9481::writeData(uint8_t inputData)
{
    lcd.WriteData8(inputData);
    // LCD_IO_WriteData8(inputData);
}

//------------------------------------------------------------------------//
// writes 8 bit command

void LCD_ILI9481::writeCommand(uint8_t inputCommand)
{
    lcd.WriteCmd8(inputCommand);
    // LCD_IO_WriteCmd8(inputCommand);
}

//------------------------------------------------------------------------//
// writes 16 bit data to the 8-bit bus

void LCD_ILI9481::writeData16(uint16_t inputData)
{
    lcd.WriteData16(inputData);
    // LCD_IO_WriteData16(inputData);
}

//------------------------------------------------------------------------//
// writes 8-bit data to 8-bit bus

void LCD_ILI9481::writeData8(uint8_t inputData)
{
    lcd.WriteData8(inputData);
    // LCD_IO_WriteData8(inputData);
}

//------------------------------------------------------------------------//

void LCD_ILI9481::displayOn()
{
    writeCommand(SET_DISPLAY_ON); // set display on
}

//------------------------------------------------------------------------//

void LCD_ILI9481::displayOff()
{
    writeCommand(SET_DISPLAY_OFF); // set display off
}

//------------------------------------------------------------------------//
// sets the number of pixels (or a rectangular window) data to be written to

void LCD_ILI9481::setAddrWindow(int x0, int y0, int x1, int y1)
{
    // x1 = (x1 >= _width) ? _width - 1 : x1;
    // y1 = (y1 >= _height) ? _height - 1 : y1;
    writeCommand(SET_COL_ADDRESS); // set col address - 4 param
    writeData8((x0 & 0xff00) >> 8);
    writeData8(x0 & 0xff);
    writeData8((x1 & 0xff00) >> 8);
    writeData8(x1 & 0xff);

    writeCommand(SET_PAGE_ADDRESS); // set page address - 4 param
    writeData8((y0 & 0xff00) >> 8);
    writeData8(y0 & 0xff);
    writeData8((y1 & 0xff00) >> 8);
    writeData8(y1 & 0xff);

    writeCommand(WRITE_MEMORY_START); // write memory start
    // writeCommand(WRITE_MEMORY_CONTINUE); //write memory continue
}

//------------------------------------------------------------------------//

void LCD_ILI9481::setRotation(Rotation rotation)
{
    writeCommand(SET_ADDRESS_MODE); // set address mode
    switch (rotation) {
        case Rotation::portrait:
            writeData8(0x0A);
            _width = Width;
            _height = Height;
            break;
        case Rotation::landscape:
            writeData8(0x28);
            _width = Height;
            _height = Width;
            break;
        case Rotation::inverse_portrait:
            writeData8(0x09);
            _width = Width;
            _height = Height;
            break;
        case Rotation::inverse_landscape:
            writeData8(0x2B);
            _width = Height;
            _height = Width;
            break;

        default:
            break;
    }
}

//------------------------------------------------------------------------//

void LCD_ILI9481::invertDisplay(bool i)
{
    writeCommand(i ? ENTER_INVERT_MODE : EXIT_INVERT_MODE);
}

//------------------------------------------------------------------------//

void LCD_ILI9481::openWindow(int x0, int y0, int x1, int y1)
{
    setAddrWindow(x0, y0, x0 + x1 - 1, y0 + y1 - 1);
}

//------------------------------------------------------------------------//
// writes a single 16b data to the data lines
void LCD_ILI9481::windowData(uint16_t c)
{
    writeData16(c);
}

//------------------------------------------------------------------------//
// writes an array of data to the data lines

void LCD_ILI9481::windowData(uint16_t *c, int len)
{
    for (int i = 0; i < len; i++) {
        writeData16(c[i]);
    }
}

//------------------------------------------------------------------------//
// sets a single pixel to a color

void LCD_ILI9481::drawPixel(int x, int y, uint16_t color)
{
    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x, y);
    writeData16(color);
}

//------------------------------------------------------------------------//
// fills the entire screen with a color

void LCD_ILI9481::fillScreen(uint16_t color)
{
    setAddrWindow(0, 0, _width - 1, _height - 1);
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            writeData16(color);
        }
    }
}

//------------------------------------------------------------------------//
// fast drawing of horizontal line

void LCD_ILI9481::drawHorizontalLine(int x, int y, int w, uint16_t color)
{
    // Rudimentary clipping
    if ((x >= _width) || (y >= _height))
        return;
    if ((x + w - 1) >= _width)
        w = _width - x;
    setAddrWindow(x, y, x + w - 1, y);

    while (w--) {
        writeData16(color);
    }
}

//------------------------------------------------------------------------//
// fast drawing of vertical line

void LCD_ILI9481::drawVerticalLine(int x, int y, int h, uint16_t color)
{
    if ((x >= _width) || (y >= _height))
        return;
    if ((y + h - 1) >= _height)
        h = _height - y;
    setAddrWindow(x, y, x, y + h - 1);

    while (h--) {
        writeData16(color);
    }
}

//------------------------------------------------------------------------//
// bressenham's line algorithm

void LCD_ILI9481::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

//------------------------------------------------------------------------//
// Draw a rectangle

void LCD_ILI9481::drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    drawHorizontalLine(x, y, w, color);
    drawHorizontalLine(x, y + h - 1, w, color);
    drawVerticalLine(x, y, h, color);
    drawVerticalLine(x + w - 1, y, h, color);
}

//------------------------------------------------------------------------//
// fills a rectanglar area os display with a color

void LCD_ILI9481::fillRectangle(int x, int y, int w, int h, uint16_t color)
{ // fillRectangle(0, 0, 480, 320, 6500);
    if ((x >= _width) || (y >= _height))
        return;
    if ((x + w - 1) >= _width)
        w = _width - x;
    if ((y + h - 1) >= _height)
        h = _height - y;
    setAddrWindow(x, y, x + w - 1, y + h - 1);

    for (y = h * 2; y > 0; y--) {
        for (x = w; x > 0; x--) {
            writeData16(color);
        }
    }
}

//------------------------------------------------------------------------//
// Draw a circle outline

void LCD_ILI9481::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

//------------------------------------------------------------------------//

void LCD_ILI9481::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
        }
    }
}

//------------------------------------------------------------------------//

void LCD_ILI9481::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    drawVerticalLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

//------------------------------------------------------------------------//
// Used to do circles and roundrects

void LCD_ILI9481::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawVerticalLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            drawVerticalLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) {
            drawVerticalLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            drawVerticalLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}

//------------------------------------------------------------------------//
// Draw a rounded rectangle

void LCD_ILI9481::drawRoundRectangle(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    // smarter version
    drawHorizontalLine(x + r, y, w - 2 * r, color);         // Top
    drawHorizontalLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
    drawVerticalLine(x, y + r, h - 2 * r, color);           // Left
    drawVerticalLine(x + w - 1, y + r, h - 2 * r, color);   // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

//------------------------------------------------------------------------//
// Fill a rounded rectangle

void LCD_ILI9481::fillRoundRectangle(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    // smarter version
    fillRectangle(x + r, y, w - 2 * r, h, color);
    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

//------------------------------------------------------------------------//
// Draw a triangle

void LCD_ILI9481::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

//------------------------------------------------------------------------//
// Fill a triangle

void LCD_ILI9481::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1);
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }

    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;
        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;
        drawHorizontalLine(a, y0, b - a + 1, color);
        return;
    }

    int16_t
        dx01 = x1 - x0,
        dy01 = y1 - y0,
        dx02 = x2 - x0,
        dy02 = y2 - y0,
        dx12 = x2 - x1,
        dy12 = y2 - y1;
    int32_t
        sa = 0,
        sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2)
        last = y1; // Include y1 scanline
    else
        last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) _swap_int16_t(a, b);
        drawHorizontalLine(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) _swap_int16_t(a, b);
        drawHorizontalLine(a, y, b - a + 1, color);
    }
}

void LCD_ILI9481::delay(uint32_t delay_ms)
{
    HAL_Delay(delay_ms - 1);
}
