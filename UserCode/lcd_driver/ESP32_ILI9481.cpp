
//========================================================================//
//                                                                        //
//  ## ESP32-ILI9481-LCD-Library ##                                       //
//  ILI9481 320 x 480 LCD driver and graphics library for ESP32 boards    //
//                                                                        //
//  Filename : ESP32_ILI9481.cpp                                          //
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
//  File last modified : +05:30 12:43:23 PM, 12-05-2018, Saturday         //
//                                                                        //
//========================================================================//

#include "ESP32_ILI9481.h"

//========================================================================//
//parallel data pins for ILI9481 display (recommended)
//below pins are  for DOIT DevKit board version 1
//pin numbers are of digital pins
//note : pins for ESP32 Devkit and Node32S are different

// #define CS_PIN_LCD  26
// #define RST_PIN_LCD 12
// #define DC_PIN_LCD  14
// #define WR_PIN_LCD  27
//
// #define PD0   15    //DB8  - D15
// #define PD1   2     //DB9  - D2
// #define PD2   0     //DB10 - D0
// #define PD3   4     //DB11 - D4
// #define PD4   16    //DB12 - D16
// #define PD5   17    //DB13 - D17
// #define PD6   21    //DB14 - D21
// #define PD7   22    //DB15 - D22

//========================================================================//
//LCD functions

LCD_ILI9481::LCD_ILI9481 (uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h,
                          uint8_t i, uint8_t j, uint8_t k, uint8_t l) {
  _PD0 = a;
  _PD1 = b;
  _PD2 = c;
  _PD3 = d;
  _PD4 = e;
  _PD5 = f;
  _PD6 = g;
  _PD7 = h;
  _CS_PIN_LCD = i;
  _RST_PIN_LCD = j;
  _DC_PIN_LCD = k;
  _WR_PIN_LCD = l;
  _bus_width = 8;
}

//------------------------------------------------------------------------//
//initializes the display with proper reset sequence

void LCD_ILI9481::initializeDisplay() {
  //the following statement won't work on the rev 0 of ESP32 chip

  // GPIO.out_w1ts = (PORT_MASK << _DC_PIN_LCD); //DC high
  // GPIO.out_w1ts = (PORT_MASK << _CS_PIN_LCD); //CS high
  // GPIO.out_w1ts = (PORT_MASK << _WR_PIN_LCD); //WR high
  // GPIO.out_w1ts = (PORT_MASK << _RST_PIN_LCD); //RST high
  // delay(100);
  // GPIO.out_w1tc = (PORT_MASK << _DC_PIN_LCD); //RST low
  // delay(100);
  // GPIO.out_w1ts = (PORT_MASK << _RST_PIN_LCD); //RST high
  // delay(100);
  // GPIO.out_w1tc = (PORT_MASK << _CS_PIN_LCD); //CS low

  LCD_IO_Init();
  // GPIO.out |= (PORT_MASK << _DC_PIN_LCD); //DC high
  // GPIO.out |= (PORT_MASK << _CS_PIN_LCD); //CS high
  // GPIO.out |= (PORT_MASK << _WR_PIN_LCD); //WR high
  // GPIO.out |= (PORT_MASK << _RST_PIN_LCD); //RST high
  // delay(100);
  // GPIO.out &= (~(PORT_MASK << _DC_PIN_LCD)); //RST low
  // delay(100);
  // GPIO.out |= (PORT_MASK << _RST_PIN_LCD); //RST high
  // delay(100);
  // GPIO.out &= (~(PORT_MASK << _CS_PIN_LCD)); //CS low

  startDisplay();
  // Serial.println("LCD Initialized.");
}

//------------------------------------------------------------------------//
//starts the display with required configurations

void LCD_ILI9481::startDisplay() {
  writeCommand(EXIT_SLEEP_MODE); //exit sleep mode
    delay(80);

  writeCommand(POWER_SETTING); //power setting
    writeData8(0x07U);
    writeData8(0x42U);
    writeData8(0x18U);

  writeCommand(VCOM_CONTROL); //VCOM control
    writeData8(0x00U);
    writeData8(0x07U);
    writeData8(0x10U);

  writeCommand(POWER_SETTING_NORMAL); //power setting for normal mode
    writeData8(0x01U);
    writeData8(0x02U); //Fosc setting

  writeCommand(PANEL_DRIVE_SETTING); //panel driving setting - 5 parameters
    writeData8(0x10U); //REV=1, SM=0, GS=0 - grayscale inversion enabled : will invert colors
    //writeData8(0x00); //REV=0, SM=0, GS=0 - no color inversion

    writeData8(0x3BU); //NL[5] - max lines
    writeData8(0x00U); //SCN - scanning start position
    writeData8(0x0U); //PTV=1 (halts vcom)
    writeData8(0x02U); //NDL (non-display area o/p level), PTS[3]

    writeData8(0x1U); //PTG=0 (interval scan), ISC[3]=0001 (3 frames)
    // writeData8(0x11); //PTG=1 (interval scan), ISC[3]=0001 (3 frames)
    // writeData8(0x1F); //PTG=1 (interval scan), ISC[3]=1111 (31 frames)
    // writeData8(0x01); //PTG=0 (normal scan), ISC[3]=0002 (3 frames)

  writeCommand(FRAME_RATE_CONTROL); //frame rate and inversion control - 1 parameter
    //writeData8(0x03); //72FPS (default) - this had backlight flickering
    writeData8(0x00U); //125 FPS (max) - this has no flickering

  writeCommand(FRAME_MEMORY_ACCESS_SETTING); //frame memeory access and interface setting
    writeData8(0x0U); //extra data is ignored
    writeData8(0x0U); //all defaults
    writeData8(0x0U);
    writeData8(0x0U);

  writeCommand(SET_TEAR_OFF); //tearing compensation
    // writeData8(0x0);

  writeCommand(TIMING_SETTING_NORMAL); //display timing for normal mode
    writeData8(0x10U); //BC=0 (frame inversion waveform)
    writeData8(0x10U); //line period is 16 clocks
    writeData8(0x22U); //front and back porch lines = 2

  writeCommand(GAMMA_SETTING); //gamma setting
    writeData8(0x00U);
    writeData8(0x32U);
    writeData8(0x36U);
    writeData8(0x45U);
    writeData8(0x06U);
    writeData8(0x16U);
    writeData8(0x37U);
    writeData8(0x75U);
    writeData8(0x77U);
    writeData8(0x54U);
    writeData8(0x0CU);
    writeData8(0x00U);

  writeCommand(SET_ADDRESS_MODE); //set address mode - 1 parameter
    writeData8(0x0AU); //BGR, horizontal flip

  writeCommand(ENTER_NORMAL_MODE); //eneter normal mode - no param

  writeCommand(SET_PIXEL_FORMAT); //set pixel format - 1 parameter
    writeData8(0x55U); //16-bit per pixel
    // writeData(0x66); //18-bit per pixel

  setAddrWindow(0, 0, _width, _height);
  //writeCommand(0x2A); //set column address
    //writeData8(0x00);
    //writeData8(0x00);
    //writeData8(0x01);
    //writeData8(0x3F);

  //writeCommand(0x2B); //set page address
    //writeData8(0x00);
    //writeData8(0x00);
    //writeData8(0x01);
    //writeData8(0xDF);
    delay(120);

  writeCommand(SET_DISPLAY_ON); //set display on
}

//------------------------------------------------------------------------//
//writes 8 bit data

void LCD_ILI9481::writeData (uint8_t inputData) {
  LCD_IO_WriteData8(inputData);
}

//------------------------------------------------------------------------//
//writes 8 bit command

void LCD_ILI9481::writeCommand (uint8_t inputCommand) {
  LCD_IO_WriteCmd8(inputCommand);
}

//------------------------------------------------------------------------//
//writes 16 bit data to the 8-bit bus

void LCD_ILI9481::writeData16 (uint16_t inputData) {
  LCD_IO_WriteData16(inputData);
}

//------------------------------------------------------------------------//
//writes 8-bit data to 8-bit bus

void LCD_ILI9481::writeData8 (uint8_t inputData) {
  LCD_IO_WriteData8(inputData);
}

//------------------------------------------------------------------------//
//writes 8-bit data to individual pins
//LSB is ANDed with 1, and writes HIGH is the ouput is 1, LOW otherwise
//then the data is shifted right to do this for all 8 bits

// void LCD_ILI9481::writeToPins (uint32_t inputData) {
//   ((inputData >> 0) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD0)) : (GPIO.out_w1tc = (PORT_MASK << _PD0));
//   ((inputData >> 1) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD1)) : (GPIO.out_w1tc = (PORT_MASK << _PD1));
//   ((inputData >> 2) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD2)) : (GPIO.out_w1tc = (PORT_MASK << _PD2));
//   ((inputData >> 3) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD3)) : (GPIO.out_w1tc = (PORT_MASK << _PD3));
//   ((inputData >> 4) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD4)) : (GPIO.out_w1tc = (PORT_MASK << _PD4));
//   ((inputData >> 5) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD5)) : (GPIO.out_w1tc = (PORT_MASK << _PD5));
//   ((inputData >> 6) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD6)) : (GPIO.out_w1tc = (PORT_MASK << _PD6));
//   ((inputData >> 7) & 0x01) ? (GPIO.out_w1ts = (PORT_MASK << _PD7)) : (GPIO.out_w1tc = (PORT_MASK << _PD7));


//   // ((inputData >> 0) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD0)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD0));
//   // ((inputData >> 1) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD1)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD1));
//   // ((inputData >> 2) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD2)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD2));
//   // ((inputData >> 3) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD3)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD3));
//   // ((inputData >> 4) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD4)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD4));
//   // ((inputData >> 5) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD5)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD5));
//   // ((inputData >> 6) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD6)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD6));
//   // ((inputData >> 7) & 0x01) ? (GPIO.out_w1ts |= ((uint32_t)1 << _PD7)) : (GPIO.out_w1tc |= ((uint32_t)1 << _PD7));

//   GPIO.out_w1tc |= (PORT_MASK << _WR_PIN_LCD); //WR low
//   // delayMicroseconds(100);
//   GPIO.out_w1ts |= (PORT_MASK << _WR_PIN_LCD); //WR high

//   // Serial.print("F : ");
//   // Serial.println(millis());
// }

//------------------------------------------------------------------------//

void LCD_ILI9481::displayOn () {
    writeCommand (SET_DISPLAY_ON); //set display on
}

//------------------------------------------------------------------------//

void LCD_ILI9481::displayOff () {
    writeCommand (SET_DISPLAY_OFF); //set display off
}

//------------------------------------------------------------------------//
//sets the number of pixels (or a rectangular window) data to be written to

void LCD_ILI9481::setAddrWindow (int x0, int y0, int x1, int y1) {
  x1 = (x1 >= _width) ? _width -1 : x1;
  y1 = (y1 >= _height) ? _height -1 : y1;
    writeCommand(SET_COL_ADDRESS); //set col address - 4 param
      writeData16(x0);
      writeData16(x1);

    writeCommand(SET_PAGE_ADDRESS); //set page address - 4 param
      writeData16(y0);
      writeData16(y1);

    writeCommand(WRITE_MEMORY_START); //write memory start
    //writeCommand(WRITE_MEMORY_CONTINUE); //write memory continue
}

//------------------------------------------------------------------------//

void LCD_ILI9481::setRotation (int m) {
  writeCommand (SET_ADDRESS_MODE); //set address mode
  int rotation = m % 4; // can't be higher than 3
  switch (rotation) {
    case 0:
      writeData8(0x0A);
      _width  = Width;
      _height = Height;
      break;
    case 1:
      writeData8(0x28);
      _width  = Height;
      _height = Width;
      break;
    case 2:
      writeData8(0x09);
      _width  = Width;
      _height = Height;
      break;
    case 3:
      writeData8(0x2B);
      _width  = Height;
      _height = Width;
      break;
  }
}

//------------------------------------------------------------------------//

void LCD_ILI9481::invertDisplay (bool i) {
  writeCommand(i ? ENTER_INVERT_MODE : EXIT_INVERT_MODE);
}

//------------------------------------------------------------------------//

void LCD_ILI9481::openWindow (int x0, int y0, int x1, int y1) {
  setAddrWindow(x0, y0, x0 + x1 - 1, y0 + y1 - 1);
}

//------------------------------------------------------------------------//
//writes a single 16b data to the data lines
void LCD_ILI9481::windowData (uint16_t c) {
    writeData16(c);
}

//------------------------------------------------------------------------//
//writes an array of data to the data lines

void LCD_ILI9481::windowData (uint16_t *c, int len) {
    for (int i = 0; i < len; i++) {
        writeData16(c[i]);
    }
}

//------------------------------------------------------------------------//
//sets a single pixel to a color

void LCD_ILI9481::drawPixel (int x, int y, uint16_t color) {
  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
  setAddrWindow(x,y,x,y);
  writeData16(color);
}

//------------------------------------------------------------------------//
//fills the entire screen with a color

void LCD_ILI9481::fillScreen (uint16_t color) {
  //fillRectangle(0, 0,  _width, _height, color);
  setAddrWindow(0, 0, _width, _height);
  for(int i=0; i<_height; i++) {
    for(int j=0; j<_width; j++) {
      writeData16(color);
    }
  }
}

//------------------------------------------------------------------------//
//fast drawing of horizontal line

void LCD_ILI9481::drawHorizontalLine (int x, int y, int w, uint16_t color) {
  // Rudimentary clipping
  if((x >= _width) || (y >= _height))
    return;
  if((x+w-1) >= _width)
    w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  while (w--) {
    writeData16(color);
  }
}

//------------------------------------------------------------------------//
//fast drawing of vertical line

void LCD_ILI9481::drawVerticalLine (int x, int y, int h, uint16_t color) {
  if((x >= _width) || (y >= _height))
    return;
  if((y+h-1) >= _height)
    h = _height-y;
  setAddrWindow(x, y, x, y+h-1);

  while (h--) {
    writeData16(color);
  }
}


//------------------------------------------------------------------------//
//bressenham's line algorithm

void LCD_ILI9481::drawLine (int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
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

    for (; x0<=x1; x0++) {
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

void LCD_ILI9481::drawRectangle (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawHorizontalLine(x, y, w, color);
  drawHorizontalLine(x, y+h-1, w, color);
  drawVerticalLine(x, y, h, color);
  drawVerticalLine(x+w-1, y, h, color);
}

//------------------------------------------------------------------------//
//fills a rectanglar area os display with a color

void LCD_ILI9481::fillRectangle (int x, int y, int w, int h, uint16_t color) { //fillRectangle(0, 0, 480, 320, 6500);
  if((x >= _width) || (y >= _height))
    return;
  if((x + w - 1) >= _width)
    w = _width  - x;
  if((y + h - 1) >= _height)
    h = _height - y;
  setAddrWindow(x, y, x+w-1, y+h-1);

  for(y=h*2; y>0; y--) {
    for(x=w; x>0; x--) {
      writeData16(color);
    }
  }
}

//------------------------------------------------------------------------//
// Draw a circle outline

void LCD_ILI9481::drawCircle (int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0  , y0+r, color);
  drawPixel(x0  , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
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

void LCD_ILI9481::drawCircleHelper (int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x<y) {
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

void LCD_ILI9481::fillCircle (int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  drawVerticalLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

//------------------------------------------------------------------------//
// Used to do circles and roundrects

void LCD_ILI9481::fillCircleHelper (int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
    int16_t f  = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawVerticalLine(x0+x, y0-y, 2*y+1+delta, color);
            drawVerticalLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            drawVerticalLine(x0-x, y0-y, 2*y+1+delta, color);
            drawVerticalLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

//------------------------------------------------------------------------//
// Draw a rounded rectangle

void LCD_ILI9481::drawRoundRectangle (int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  drawHorizontalLine(x+r, y, w-2*r, color); // Top
  drawHorizontalLine(x+r, y+h-1, w-2*r, color); // Bottom
  drawVerticalLine(x, y+r, h-2*r, color); // Left
  drawVerticalLine(x+w-1, y+r, h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r, y+h-r-1, r, 8, color);
}

//------------------------------------------------------------------------//
// Fill a rounded rectangle

void LCD_ILI9481::fillRoundRectangle (int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  fillRectangle(x+r, y, w-2*r, h, color);
  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

//------------------------------------------------------------------------//
// Draw a triangle

void LCD_ILI9481::drawTriangle (int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

//------------------------------------------------------------------------//
// Fill a triangle

void LCD_ILI9481::fillTriangle (int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawHorizontalLine(a, y0, b-a+1, color);
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
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        drawHorizontalLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        drawHorizontalLine(a, y, b-a+1, color);
    }
}

void LCD_ILI9481::delay(uint32_t delay_ms)
{
  HAL_Delay(delay_ms);
}
