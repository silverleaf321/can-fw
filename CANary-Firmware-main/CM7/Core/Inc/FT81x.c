/*
 * FT81x.c
 *
 *  Created on: Jan 18, 2024
 *      Author: greyson
 */

#include "FT81x.h"
#include "main.h"
#include <string.h>

#define DISPLAY_WIDTH  800  ///< Width of the display in pixels
#define DISPLAY_HEIGHT 480  ///< Height of the display in pixels

//#define READ  0x000000  ///< Bitmask address reading
#define WRITE 0x800000  ///< Bitmask for address writing

#define DLSTART()                    0xFFFFFF00                                                                                                                            ///< Start display list
#define SWAP()                       0xFFFFFF01
//#define MEMWRITE()                   0xFFFFFF1A
//#define SETBITMAP                    0xFFFFFF43                                                                                                                             ///< Write bytes into memory
#define CLEAR(c, s, t)               ((0x26L << 24) | ((c) << 2) | ((s) << 1) | (t))                                                                                        ///< Clear command
#define BEGIN(p)                     ((0x1FL << 24) | (p))                                                                                                                  ///< Begin primitive drawing
//#define SCISSORSIZE(w, h)            ((0x1CL << 24) | ((w) << 12) | (h))                                                                                                                  ///< Begin primitive drawing
//#define TAG(s)                       ((0x03L << 24) | (s))
#define END()                        (0x21L << 24)                                                                                                                          ///< End primitive drawing
#define END_DL()                     0x00                                                                                                                                   ///< End current display list
//#define CLEAR_COLOR_RGB(r, g, b)     ((0x02L << 24) | ((r) << 16) | ((g) << 8) | (b))                                                                                       ///< Clear with RGB color
#define CLEAR_COLOR(rgb)             ((0x02L << 24) | ((rgb)&0xFFFFFF))                                                                                                     ///< Clear with color
#define COLOR(rgb)                   ((0x04L << 24) | ((rgb)&0xFFFFFF))                                                                                                     ///< Create color
//#define POINT_SIZE(s)                ((0x0DL << 24) | ((s)&0xFFF))                                                                                                          ///< Point size
#define LINE_WIDTH(w)                ((0x0EL << 24) | ((w)&0xFFF))                                                                                                          ///< Line width
//#define VERTEX2II(x, y, h, c)        ((1L << 31) | (((uint32_t)(x)&0xFFF) << 21) | (((uint32_t)(y)&0xFFF) << 12) | ((uint32_t)(h) << 7) | (c))                              ///< Start the operation of graphics primitive at the specified coordinates in pixel precision.
#define VERTEX2F(x, y)               ((1L << 30) | (((uint32_t)(x)&0xFFFF) << 15) | ((uint32_t)(y)&0xFFFF))                                                                 ///< Start the operation of graphics primitives at the specified screen coordinate, in the pixel precision defined by VERTEX_FORMAT.
//#define BITMAP_SOURCE(a)             ((1L << 24) | (a))                                                                                                                     ///< Specify the source address of bitmap data in FT81X graphics memory RAM_G.
//#define BITMAP_LAYOUT(f, s, h)       ((7L << 24) | ((uint32_t)(f) << 19) | (((uint32_t)(s)&0x1FF) << 9) | ((uint32_t)(h)&0x1FF))                                            ///< Specify the source bitmap memory format and layout for the current handle.
//#define BITMAP_SIZE(f, wx, wy, w, h) ((8L << 24) | ((uint32_t)((f)&1) << 20) | ((uint32_t)((wx)&1) << 19) | ((uint32_t)((wy)&1) << 18) | (((w)&0x1FF) << 9) | ((h)&0x1FF))  ///< Specify the screen drawing of bitmaps for the current handle
//#define BGCOLOR()                    0xFFFFFF09                                                                                                                             ///< Set background color
//#define FGCOLOR()                    0xFFFFFF0A                                                                                                                             ///< Set foreground color
//#define PROGRESSBAR()                0xFFFFFF0F                                                                                                                             ///< Draw progressbar
//#define SCROLLBAR()                  0xFFFFFF11                                                                                                                             ///< Draw scrollbar
//#define LOADIDENTITY()               0xFFFFFF26                                                                                                                             ///< Set the current matrix to identity
//#define SETMATRIX()                  0xFFFFFF2A                                                                                                                             ///< Write the current matrix as a bitmap transform
//#define SCALE()                      0xFFFFFF28                                                                                                                             ///< Apply a scale to the current matrix
//#define GRADIENT()                   0xFFFFFF0B                                                                                                                             ///< Draw gradient
#define TEXT()                       0xFFFFFF0C                                                                                                                             ///< Draw text
//#define BUTTON()                     0xFFFFFF0D                                                                                                                             ///< Draw button
//#define GAUGE()                      0xFFFFFF13                                                                                                                             ///< Draw gauge
//#define CLOCK()                      0xFFFFFF14                                                                                                                             ///< Draw clock
//#define SPINNER()                    0xFFFFFF16                                                                                                                             ///< Draw spinner
//#define LOADIMAGE()                  0xFFFFFF24                                                                                                                             ///< Load image data
//#define MEDIAFIFO()                  0xFFFFFF39                                                                                                                             ///< Set up media FIFO in general purpose graphics RAM
#define ROMFONT()                    0xFFFFFF3F                                                                                                                             ///< Load a ROM font into bitmap handle

//#define BITMAPS      1  ///< Bitmap drawing primitive
//#define POINTS       2  ///< Point drawing primitive
#define LINES        3  ///< Line drawing primitive
#define LINE_STRIP   4  ///< Line strip drawing primitive
//#define EDGE_STRIP_R 5  ///< Edge strip right side drawing primitive
//#define EDGE_STRIP_L 6  ///< Edge strip left side drawing primitive
//#define EDGE_STRIP_A 7  ///< Edge strip above drawing primitive
#define EDGE_STRIP_B 8  ///< Edge strip below side drawing primitive
#define RECTS        9  ///< Rectangle drawing primitive

#define DISPLAY_CMD(cmd, params...)                                \
    {                                                              \
        const uint8_t d[] = {params};                              \
        sendCommandToDisplay(cmd, sizeof(d) / sizeof(uint8_t), d); \
    }  ///< Macro to automatically call sendCommandToDisplay

uint16_t cmdWriteAddress = 0;
//
void initFT81x() {
    // reset
    read8(FT81x_REG_ID);

	sendCommand(FT81x_CMD_RST_PULSE);
    HAL_Delay(300);

    // select clock
    sendCommand(FT81x_CMD_CLKEXT);
    HAL_Delay(300);

    // activate
    sendCommand(FT81x_CMD_ACTIVE);

    // wait for boot-up to complete
    HAL_Delay(100);
    while (read8(FT81x_REG_ID) != 0x7C) {
        __asm__ volatile("nop");
    }
    while (read8(FT81x_REG_CPURESET) != 0x00) {
        __asm__ volatile("nop");
    }

    // configure rgb interface
    write16(FT81x_REG_HCYCLE, 928);
    write16(FT81x_REG_HOFFSET, 88);
    write16(FT81x_REG_HSYNC0, 0);
    write16(FT81x_REG_HSYNC1, 48);
    write16(FT81x_REG_HSIZE, DISPLAY_WIDTH);

    write16(FT81x_REG_VCYCLE, 525);
    write16(FT81x_REG_VOFFSET, 32);
    write16(FT81x_REG_VSYNC0, 0);
    write16(FT81x_REG_VSYNC1, 3);
    write16(FT81x_REG_VSIZE, DISPLAY_HEIGHT);

    write8(FT81x_REG_SWIZZLE, 0);
    write8(FT81x_REG_PCLK_POL, 0);
    write8(FT81x_REG_CSPREAD, 0);
    write8(FT81x_REG_DITHER, 1);
    write8(FT81x_REG_ROTATE, 0);

    write16(FT81x_REG_PWM_HZ, 10000);
    write8(FT81x_REG_PWM_DUTY, 128);

    // write first display list
    beginDisplayList();
    clear(0xFFFFFF);
    swapScreen();

    // reset display on GPIO3
    write8(FT81x_REG_GPIO_DIR, 0x80 | read8(FT81x_REG_GPIO_DIR));
    write8(FT81x_REG_GPIO, 0x80 | read8(FT81x_REG_GPIO));

    // enable pixel clock
    write8(FT81x_REG_PCLK, 2);
    HAL_Delay(300);
}

void clear(const uint32_t color) {
    dispCmd(CLEAR_COLOR(color));
    dispCmd(CLEAR(1, 1, 1));
}

//void drawCircle(const int16_t x, const int16_t y, const uint8_t size, const uint32_t color) {
//    dispCmd(COLOR(color));
//    dispCmd(POINT_SIZE(size * 16));
//    dispCmd(BEGIN(POINTS));
//    dispCmd(VERTEX2F(x * 16, y * 16));
//    dispCmd(END());
//}

void drawRect(const int16_t x, const int16_t y, const uint16_t width, const uint16_t height, const uint8_t cornerRadius, const uint32_t color) {
    dispCmd(COLOR(color));
    dispCmd(LINE_WIDTH(cornerRadius * 16));
    dispCmd(BEGIN(RECTS));
    dispCmd(VERTEX2F(x * 16, y * 16));
    dispCmd(VERTEX2F((x + width) * 16, (y + height) * 16));
    dispCmd(END());
}

void drawLine(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2, const uint8_t width, const uint32_t color) {
    dispCmd(COLOR(color));
    dispCmd(LINE_WIDTH(width * 16));
    dispCmd(BEGIN(LINES));
    dispCmd(VERTEX2F(x1 * 16, y1 * 16));
    dispCmd(VERTEX2F(x2 * 16, y2 * 16));
    dispCmd(END());
}

void drawUnfilledRect(const int16_t x1, const int16_t y1, const uint16_t width, const uint16_t height, const uint8_t lineWidth, const uint32_t color) {
    dispCmd(COLOR(color));
    dispCmd(LINE_WIDTH(lineWidth * 16));
    dispCmd(BEGIN(LINES));
    dispCmd(VERTEX2F(x1 * 16, y1 * 16));
    dispCmd(VERTEX2F((x1 + width) * 16, y1 * 16));
    dispCmd(VERTEX2F((x1 + width) * 16, y1 * 16));
    dispCmd(VERTEX2F((x1 + width) * 16, (y1 + height) * 16));
    dispCmd(VERTEX2F((x1 + width) * 16, (y1 + height) * 16));
    dispCmd(VERTEX2F(x1 * 16, (y1 + height) * 16));
    dispCmd(VERTEX2F(x1 * 16, (y1 + height) * 16));
    dispCmd(VERTEX2F(x1 * 16, y1 * 16));
    dispCmd(END());
}

//void beginLineStrip(const uint8_t width, const uint32_t color) {
//    dispCmd(COLOR(color));
//    dispCmd(LINE_WIDTH(width * 16));
//    dispCmd(BEGIN(LINE_STRIP));
//}
//
//void addVertex(const int16_t x, const int16_t y) {
//    dispCmd(VERTEX2F(x * 16, y * 16));
//}
//
//void endLineStrip() {
//    dispCmd(END());
//}
//
//void drawLetter(const int16_t x, const int16_t y, const uint8_t font, const uint32_t color, const uint8_t letter) {
//    uint8_t fontHandle = initBitmapHandleForFont(font);
//    dispCmd(COLOR(color));
//    dispCmd(BEGIN(BITMAPS));
//    dispCmd(VERTEX2II(x, y, fontHandle, letter));
//    dispCmd(END());
//}
//
//void drawBitmap(const uint32_t offset, const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, const uint8_t scale) {
//    dispCmd(COLOR(FT81x_COLOR_RGB(255, 255, 255)));
//    dispCmd(BITMAP_SOURCE(FT81x_RAM_G + offset));
//    dispCmd(BITMAP_LAYOUT(FT81x_BITMAP_LAYOUT_RGB565, (uint32_t)width * 2, (uint32_t)height));  // only supporting one format for now
//    dispCmd(BITMAP_SIZE(FT81x_BITMAP_SIZE_NEAREST, 0, 0, (uint32_t)width * (uint32_t)scale, (uint32_t)height * (uint32_t)scale));
//    dispCmd(BEGIN(BITMAPS));
//    dispCmd(LOADIDENTITY());
//    dispCmd(SCALE());
//    dispCmd((uint32_t)scale * 65536);
//    dispCmd((uint32_t)scale * 65536);
//    dispCmd(SETMATRIX());
//    dispCmd(VERTEX2II(x, y, 0, 0));
//}
//
void drawText(const int16_t x, const int16_t y, const uint8_t font, const uint32_t color, const uint16_t options, const char text[]) {
    dispCmd(COLOR(color));
    dispCmd(TEXT());
    dispCmd(x | ((uint32_t) y << 16));
    dispCmd(font | ((uint32_t) options << 16));
    int len = strlen(text) + 1;
    for (int i = 0; i < (len / 4); i++) {
        dispCmd(text[0+(4*i)] | (text[1+(4*i)] << 8) | (text[2+(4*i)] << 16) | (text[3+(4*i)] << 24));
    }
    if ((len % 4) == 1) {
        dispCmd(text[len-1]);
    } else if ((len % 4) == 2) {
        dispCmd(text[len-2] | text[len-1] << 8);
    } else if ((len % 4) == 3) {
        dispCmd(text[len-3] | text[len-2] << 8 | text[len-1] << 16);
    }
}

//void drawNumber(const int16_t x, const int16_t y, const uint8_t font, const uint32_t color, const uint16_t options, const float num) {
//    dispCmd(COLOR(color));
//    dispCmd(0xffffff2e);
//    dispCmd(x | ((uint32_t) y << 16));
//    dispCmd(font | ((uint32_t) options << 16));
//    dispCmd(num);
//}
//
//void drawButton(const int16_t x, const int16_t y, const int16_t width, const int16_t height, const uint8_t font, const uint32_t textColor, const uint32_t buttonColor, const uint16_t options, const char text[]) {
//    uint8_t fontHandle = initBitmapHandleForFont(font);
//    dispCmd(COLOR(textColor));
//    dispCmd(FGCOLOR());
//    dispCmd(buttonColor);
//    dispCmd(BUTTON());
//    dispCmd(x | ((uint32_t)y << 16));
//    dispCmd(width | ((uint32_t)height << 16));
//    dispCmd(fontHandle | ((uint32_t)options << 16));
//    sendText(text);
//}
//
//void cmd(const uint32_t cmd) {
//    uint16_t cmdWrite = read16(FT81x_REG_CMD_WRITE);
//    uint32_t addr = FT81x_RAM_CMD + cmdWrite;
//    write32(addr, cmd);
//    write16(FT81x_REG_CMD_WRITE, (cmdWrite + 4) % 4096);
//}

void beginDisplayList() {
    waitForCommandBuffer();
    dispCmd(DLSTART());
    dispCmd(CLEAR(1, 1, 1));
}

void swapScreen() {
    dispCmd(END_DL());
    dispCmd(SWAP());
    updateCmdWriteAddress();
}

void waitForCommandBuffer() {
    // Wait for circular buffer to catch up
    while (read16(FT81x_REG_CMD_WRITE) != read16(FT81x_REG_CMD_READ)) {
        __asm__ volatile("nop");
    }
}

void increaseCmdWriteAddress(uint16_t delta) {
    cmdWriteAddress = (cmdWriteAddress + delta) % 4096;
}

inline void updateCmdWriteAddress() { write16(FT81x_REG_CMD_WRITE, cmdWriteAddress); }

//void sendText(const char text[]) {
//    uint32_t data = 0xFFFFFFFF;
//    for (uint8_t i = 0; (data >> 24) != 0; i += 4) {
//        data = 0;
//
//        if (text[i] != 0) {
//            data |= text[i];
//
//            if (text[i + 1] != 0) {
//                data |= text[i + 1] << 8;
//
//                if (text[i + 2] != 0) {
//                    data |= (uint32_t)text[i + 2] << 16;
//
//                    if (text[i + 3] != 0) {
//                        data |= (uint32_t)text[i + 3] << 24;
//                    }
//                }
//            }
//        }
//
//        if ((data >> 24) != 0) {
//            dispCmd(data);
//        } else {
//            dispCmd(data);
//        }
//    }
//
//    if ((data >> 24) != 0) {
//        dispCmd(0);
//    }
//}

uint8_t initBitmapHandleForFont(uint8_t font1, uint8_t font2) {
    if (font1 > 31) {
        dispCmd(ROMFONT());
        dispCmd(font2);
        dispCmd(font1);
        return font2;
    }
    return font1;
}

void dispCmd(const uint32_t cmd) {
    write32(FT81x_RAM_CMD + cmdWriteAddress, cmd);
    increaseCmdWriteAddress(4);
}

void sendCommand(const uint32_t cmd) {
    uint8_t tx[3];
    tx[0] = cmd >> 16;
    tx[1] = cmd >> 8;
    tx[2] = cmd;
	SPI_Transmit(tx, 3);
}

uint8_t read8(const uint32_t address) {
	char tx[5];
	char rx[5];

    tx[0] = address >> 16;
    tx[1] = address >> 8;
    tx[2] = address;
    tx[3] = 0x00;
    tx[4] = 0x00;

    SPI_Transmit_Receive(tx, rx, 5);

    uint8_t result = rx[4];

    return result;
}

uint16_t read16(const uint32_t address) {
	char tx[6];
	char rx[6];

	tx[0] = address >> 16;
	tx[1] = address >> 8;
	tx[2] = address;
	tx[3] = 0x00;
	tx[4] = 0x00;
	tx[5] = 0x00;

	SPI_Transmit_Receive(tx, rx, 6);
    uint16_t result = rx[4];
    result |= (rx[5] << 8);

    return result;
}

uint32_t read32(const uint32_t address) {
	char tx[8];
	char rx[8];

	tx[0] = address >> 16;
	tx[1] = address >> 8;
	tx[2] = address;
	tx[3] = 0x00;
	tx[4] = 0x00;
	tx[5] = 0x00;
	tx[6] = 0x00;
	tx[7] = 0x00;

	SPI_Transmit_Receive(tx, rx, 6);
    uint32_t result = rx[4];
    result |= (rx[5] << 8);
    result |= ((uint32_t)rx[6] << 16);
    result |= ((uint32_t)rx[7] << 24);

    return result;
}

void write8(const uint32_t address, const uint8_t data) {
    uint8_t tx[4];

	uint32_t cmd = address | WRITE;

	tx[0] = cmd >> 16;
	tx[1] = cmd >> 8;
	tx[2] = cmd;
	tx[3] = data;

    SPI_Transmit(tx, 4);

}

void write16(const uint32_t address, const uint16_t data) {
	uint8_t tx[5];

	uint32_t cmd = address | WRITE;

	tx[0] = cmd >> 16;
	tx[1] = cmd >> 8;
	tx[2] = cmd;
	tx[3] = data;
	tx[4] = data >> 8;

	SPI_Transmit(tx, 5);
}

void write32(const uint32_t address, const uint32_t data) {
	uint8_t tx[7];

	uint32_t cmd = address | WRITE;

	tx[0] = cmd >> 16;
	tx[1] = cmd >> 8;
	tx[2] = cmd;
	tx[3] = data;
	tx[4] = data >> 8;
	tx[5] = data >> 16;
	tx[6] = data >> 24;

	SPI_Transmit(tx, 7);
}
