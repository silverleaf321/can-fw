/*
 * ledUtil.c
 *
 *  Created on: Jan 18, 2024
 *      Author: greyson
 */

#include "ledUtil.h"
#include "main.h"

int tcycle = 10;
uint8_t brightness = 1;

void writeBit(uint8_t data);
void writeByte(uint8_t data);
void writeOff();
void rpm6000();
void rpm6500();
void rpm7000();
void rpm7500();
void rpm8000();
void rpm8500();
void rpm9000();
void rpm9500();
void rpm10000();
void rpm10500();


void writeLed(uint8_t r, uint8_t g, uint8_t b, bool latch)
{
	writeByte(0b00111010);

	writeByte((uint8_t)(b * brightness / 100));
	writeByte((uint8_t)(g * brightness / 100));
	writeByte((uint8_t)(r * brightness / 100));

	if (latch) {
	  delayMicro(10*tcycle);
	} else {
	  delayMicro(3*tcycle);
	}
}

void writeByte(uint8_t data)
{
  writeBit(data & (1<<7));
  writeBit(data & (1<<6));
  writeBit(data & (1<<5));
  writeBit(data & (1<<4));
  writeBit(data & (1<<3));
  writeBit(data & (1<<2));
  writeBit(data & (1<<1));
  writeBit(data & (1<<0));
}

void writeBit(uint8_t data) {

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);

  if (data)
  {
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
    delayMicro(tcycle);
  }
  else
  {
    delayMicro(tcycle);
  }
}

void cycleBrightness()
{
	brightness = brightness - 1 == 0 ? 10 : brightness - 1;
}

void rpm6000()
{
	writeLed(0,0,255,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void rpm6500()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void rpm7000()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,255,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void rpm7500()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,255,0,false);
	writeLed(255,255,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void rpm8000()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,255,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void rpm8500()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void rpm9000()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,0,0,false);
	writeLed(255,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void rpm9500()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,0,0,false);
	writeLed(255,0,0,false);
	writeLed(170,0,255,false);
	writeLed(0,0,0,true);
}

void rpm10000()
{
	writeLed(0,0,255,false);
	writeLed(0,255,0,false);
	writeLed(0,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,255,0,false);
	writeLed(255,0,0,false);
	writeLed(255,0,0,false);
	writeLed(170,0,255,false);
	writeLed(170,0,255,true);
}

void rpm10500()
{
	writeLed(255,140,0,false);
	writeLed(255,140,0,false);
	writeLed(255,140,0,false);
	writeLed(255,140,0,false);
	writeLed(255,140,0,false);
	writeLed(255,140,0,false);
	writeLed(255,140,0,false);
	writeLed(255,140,0,false);
	writeLed(255,140,0,true);
}

void writeRed()
{
	writeLed(100,0,0,false);
	writeLed(100,0,0,false);
	writeLed(100,0,0,false);
	writeLed(100,0,0,false);
	writeLed(100,0,0,false);
	writeLed(100,0,0,false);
	writeLed(100,0,0,false);
	writeLed(100,0,0,false);
	writeLed(100,0,0,true);
}

void writeGreen()
{
	writeLed(0,100,0,false);
	writeLed(0,100,0,false);
	writeLed(0,100,0,false);
	writeLed(0,100,0,false);
	writeLed(0,100,0,false);
	writeLed(0,100,0,false);
	writeLed(0,100,0,false);
	writeLed(0,100,0,false);
	writeLed(0,100,0,true);
}

void writeBlue()
{
	writeLed(0,0,100,false);
	writeLed(0,0,100,false);
	writeLed(0,0,100,false);
	writeLed(0,0,100,false);
	writeLed(0,0,100,false);
	writeLed(0,0,100,false);
	writeLed(0,0,100,false);
	writeLed(0,0,100,false);
	writeLed(0,0,100,true);
}

void writeOff()
{
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,false);
	writeLed(0,0,0,true);
}

void startupLED() {
	rpm6000();
	delayMilli(400);
	rpm6500();
	delayMilli(300);
	rpm7000();
	delayMilli(250);
	rpm7500();
	delayMilli(200);
	rpm8000();
	delayMilli(150);
	rpm8500();
	delayMilli(100);
	rpm9000();
	delayMilli(50);
	rpm9500();
	delayMilli(50);
	rpm10000();
	delayMilli(1100);
	writeOff();
}

void writeRpm() {
	if (shutdown == 1 && ((getTime() & 0b1000000000000) == 0b1000000000000)) {
		writeRed();
		return;
	}

	if (curScreen == SKIDPAD) {
		if (rpm >= 10500) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 10000) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 9500) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 9000) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 8500) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 8000) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 7500) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 7000) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 6500) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else if (rpm >= 6000) {
			writeLed(0,0,255,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,true);
		} else {
			writeOff();
		}
		return;
	} else if (curScreen == ACCEL) {
		if (rpm >= 9500) {
			if ((getTime() & 0b10000000000) == 0b10000000000) {
				writeLed(0,255,0,false);
				writeLed(255,255,0,false);
				writeLed(255,0,0,false);
				writeLed(170,0,255,false);
				writeLed(170,0,255,false);
				writeLed(170,0,255,false);
				writeLed(255,0,0,false);
				writeLed(255,255,0,false);
				writeLed(0,255,0,true);
			} else {
				writeOff();
			}
		} else if (rpm >= 9000) {
			writeLed(0,255,0,false);
			writeLed(255,255,0,false);
			writeLed(255,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(255,0,0,false);
			writeLed(255,255,0,false);
			writeLed(0,255,0,true);
		} else if (rpm >= 8000) {
			writeLed(0,255,0,false);
			writeLed(255,255,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(255,255,0,false);
			writeLed(0,255,0,true);
		} else if (rpm >= 7000) {
			writeLed(0,255,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,0,0,false);
			writeLed(0,255,0,true);
		} else {
			writeOff();
		}
		return;
	} else {
		if (rpm >= 10500) {
			rpm10500();
		} else if (rpm >= 10000) {
			rpm10000();
		} else if (rpm >= 9500) {
			rpm9500();
		} else if (rpm >= 9000) {
			rpm9000();
		} else if (rpm >= 8500) {
			rpm8500();
		} else if (rpm >= 8000) {
			rpm8000();
		} else if (rpm >= 7500) {
			rpm7500();
		} else if (rpm >= 7000) {
			rpm7000();
		} else if (rpm >= 6500) {
			rpm6500();
		} else if (rpm >= 6000) {
			rpm6000();
		} else {
			writeOff();
		}
		return;
	}
}
