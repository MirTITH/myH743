
//========================================================================//
//                                                                        //
//  ## ESP32-ILI9481-LCD-Library ##                                       //
//  ILI9481 320 x 480 LCD driver and graphics library for ESP32 boards    //
//                                                                        //
//  Filename : ESP32_ILI9481.h                                            //
//  Description : Part of ILI9481 TFT LCD library.                        //
//  Library version : 2.7                                                 //
//  Author : Vishnu M Aiea                                                //
//  Source : https://github.com/vishnumaiea/ESP32-ILI9481-LCD-Library     //
//  Author's website : www.vishnumaiea.in                                 //
//  Initial release : IST 11:51 PM 07-04-2018, Saturday                   //
//  License : MIT                                                         //
//  Additional source license/s :                                         //
//    1. BSD license @ Adafruit Industries                                //
//       https://github.com/adafruit/Adafruit-GFX-Library                 //
//                                                                        //
//  File last modified : +05:30 12:43:40 PM, 12-05-2018, Saturday         //
//                                                                        //
//========================================================================//

#include "main.h"
#include "lcd_io_gpio16.h"

class LCD_ILI9481 {
  private:
    const int Width = 320; // Physical width
    const int Height = 480; // Physical height

    void delay(uint32_t delay_ms);

  public:
    int _width = Width; //becasue we can change these later if needed
    int _height = Height; //eg. in setRotation()

    enum class Rotation {
      portrait,
      landscape,
      inverse_portrait,
      inverse_landscape
    };

    //------------------------------------------------------------------------//

    LCD_ILI9481 (); //for 8-bit interface (total 12 pins)
    void initializeDisplay (void); //resets the display
    void startDisplay (void); //update config registers with default values
    void setRotation (Rotation); //set rotation of the screen
    void invertDisplay (bool); //inverts colors
    void displayOn (void); //turns on the display
    void displayOff (void); //turns of the display
    void openWindow (int, int, int, int); //opens a custom pixel window with X, Y, width and height
    void windowData (uint16_t); //data values for custom window
    void windowData (uint16_t*, int); //data values as array, and count for custom window
    void writeData (uint8_t); //writes 8-bit data to bus
    void writeCommand (uint8_t); //writes 8-bit command to bus
    void writeData16 (uint16_t); //writes 16-bit data to 8-bit bus
    void writeData8 (uint8_t); //writes any 8-bit data to 8-bit bus
    void setAddrWindow (int, int, int, int); //opens a custom pixel window with X1, Y1, X2, Y2
    void fillScreen (uint16_t); //fills the entire screen with a 16-bit color
    void drawPixel (int, int, uint16_t); //updates the color of a single pixel
    void drawHorizontalLine (int, int, int, uint16_t); //draws horizontal straight line
    void drawVerticalLine (int, int, int, uint16_t); //draws vertical straight line
    void drawLine (int16_t, int16_t, int16_t, int16_t, uint16_t); //draws line between two points
    void drawRectangle (int16_t, int16_t, int16_t,int16_t, uint16_t); //draws a rectangle outline with single pixel thickness
    void fillRectangle (int, int, int, int, uint16_t); //fills a rectangle with a 16-bit color
    void drawRoundRectangle (int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t); //draws a rounded corner rectangle outline
    void fillRoundRectangle (int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t); //fills a rounded corner rectangle with a color
    void drawTriangle (int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t); //draws a triangle outline
    void fillTriangle (int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t); //fills a triangle with a color
    void drawCircle (int16_t, int16_t, int16_t, uint16_t); //draws a circle outline
    void drawCircleHelper (int16_t, int16_t, int16_t, uint8_t, uint16_t);
    void fillCircle (int16_t, int16_t, int16_t, uint16_t); //fills a circle with a color
    void fillCircleHelper (int16_t, int16_t, int16_t, uint8_t, int16_t, uint16_t);
};
