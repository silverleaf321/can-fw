/*
 * display.h
 *
 *  Created on: Jan 18, 2024
 *      Author: greyson
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <FT81x.h>
#include "stm32h7xx_hal.h"

/*!
    @brief Handles which screen is displayed
    @param index Number of screen
*/
void displayScreen();
void screenBrightnessChange();

#endif /* INC_DISPLAY_H_ */
