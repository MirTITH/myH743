/**
 * @file STM32_ILI9481.hpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "main.h"
#include "lcd_io_fmc.hpp"

class LCD_ILI9481
{
private:
    const int Width = 320;  // Physical width
    const int Height = 480; // Physical height

    void delay(uint32_t delay_ms);

public:
    LcdIoFmc lcd;
    int _width = Width;   // becasue we can change these later if needed
    int _height = Height; // eg. in setRotation()

    enum class Rotation {
        portrait,
        landscape,
        inverse_portrait,
        inverse_landscape
    };

    //------------------------------------------------------------------------//

    LCD_ILI9481();                                                                                             // for 8-bit interface (total 12 pins)
    void initializeDisplay(void);                                                                              // resets the display
    void startDisplay(void);                                                                                   // update config registers with default values
    void setRotation(Rotation rotation);                                                                       // set rotation of the screen
    void invertDisplay(bool i);                                                                                // inverts colors
    void displayOn(void);                                                                                      // turns on the display
    void displayOff(void);                                                                                     // turns of the display
    void openWindow(int x0, int y0, int x1, int y1);                                                           // opens a custom pixel window with X, Y, width and height
    void windowData(uint16_t);                                                                                 // data values for custom window
    void windowData(uint16_t *c, int len);                                                                     // data values as array, and count for custom window
    void writeData(uint8_t);                                                                                   // writes 8-bit data to bus
    void writeCommand(uint8_t);                                                                                // writes 8-bit command to bus
    void writeData16(uint16_t);                                                                                // writes 16-bit data to 8-bit bus
    void writeData8(uint8_t);                                                                                  // writes any 8-bit data to 8-bit bus
    void setAddrWindow(int x0, int y0, int x1, int y1);                                                        // opens a custom pixel window with X1, Y1, X2, Y2
    void fillScreen(uint16_t);                                                                                 // fills the entire screen with a 16-bit color
    void drawPixel(int x, int y, uint16_t color);                                                              // updates the color of a single pixel
    void drawHorizontalLine(int x, int y, int w, uint16_t color);                                              // draws horizontal straight line
    void drawVerticalLine(int x, int y, int h, uint16_t color);                                                // draws vertical straight line
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);                             // draws line between two points
    void drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);                            // draws a rectangle outline with single pixel thickness
    void fillRectangle(int x, int y, int w, int h, uint16_t color);                                            // fills a rectangle with a 16-bit color
    void drawRoundRectangle(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);            // draws a rounded corner rectangle outline
    void fillRoundRectangle(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);            // fills a rounded corner rectangle with a color
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color); // draws a triangle outline
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color); // fills a triangle with a color
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);                                        // draws a circle outline
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color); // fills a circle with a color
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
};
