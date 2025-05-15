/*
 * display.h
 *
 *  Created on: Jan 18, 2024
 *      Author: greyson
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <FT81x.h>

//#define BUTTONMENUX 110
//#define BUTTONMENUY 197
//#define BUTTONMENUXSIZE 260
//#define BUTTONMENUYSIZE 75
//#define BUTTONMENUSIZEFONT 22
//#define BUTTONMENUTEXTCOLOR 50
//#define BUTTONTEXTMENUX (BUTTONMENUX + (BUTTONMENUXSIZE / 2))
//#define BUTTONTEXTMENUY (BUTTONMENUY + (BUTTONMENUYSIZE / 2))
//
//#define BUTTON1X 10
//#define BUTTON1Y 10
//
//#define BUTTON2X 245
//#define BUTTON2Y 10
//
//#define BUTTON3X 10
//#define BUTTON3Y 141
//
//#define BUTTON4X 245
//#define BUTTON4Y 141
//
//#define BUTTONXSIZE 215
//#define BUTTONYSIZE 121
//#define BUTTONYSIZEFONT 23
//#define BUTTONTEXTCOLOR 0x000000
//
//#define BUTTONTEXT1X (BUTTON1X + (BUTTONXSIZE / 2))
//#define BUTTONTEXT1Y (BUTTON1Y + (BUTTONYSIZE / 2))
//#define BUTTONTEXT2X (BUTTON2X + (BUTTONXSIZE / 2))
//#define BUTTONTEXT2Y (BUTTON2Y + (BUTTONYSIZE / 2))
//#define BUTTONTEXT3X (BUTTON3X + (BUTTONXSIZE / 2))
//#define BUTTONTEXT3Y (BUTTON3Y + (BUTTONYSIZE / 2))
//#define BUTTONTEXT4X (BUTTON4X + (BUTTONXSIZE / 2))
//#define BUTTONTEXT4Y (BUTTON4Y + (BUTTONYSIZE / 2))

/*!
    @brief Handles which screen is displayed
    @param index Number of screen
*/
void displayScreen(int index);
void screenBrightnessChange();

#endif /* INC_DISPLAY_H_ */
