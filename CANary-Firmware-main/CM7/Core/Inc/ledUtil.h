/*
 * ledUtil.h
 *
 *  Created on: Jan 18, 2024
 *      Author: greyson
 */

#ifndef INC_LEDUTIL_H_
#define INC_LEDUTIL_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32h7xx_hal.h"

/**
 * @brief Writes all LEDs to red
 * @retval None
 */
void writeRed();

/**
 * @brief Writes all LEDs to blue
 * @retval None
 */
void writeBlue();

/**
 * @brief Writes all LEDs to green
 * @retval None
 */
void writeGreen();

/**
 * @brief Write to an LED given RGB and latch
 * @param r, g, b, latch
 * @retval None
 */
void writeLed(uint8_t r, uint8_t g, uint8_t b, bool latch);

/**
 * @brief Sets brightness to a value between 0-1
 * @param val
 * @retval None
 */
void cycleBrightness();

void writeTime(uint8_t time);
void writeRpm();
void startupLED();

#endif /* INC_LEDUTIL_H_ */
