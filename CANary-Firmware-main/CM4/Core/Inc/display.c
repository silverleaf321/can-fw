/*
 * display.c
 *
 *  Created on: Jan 18, 2024
 *      Author: greyson naugle
 */

#include "display.h"
#include <stdio.h>
#include "main.h"

uint8_t screenBrightness = 128;
uint32_t lightBlue = 0x00FFFF;
uint32_t lilac = 0xD5A6E3;
uint32_t yellow = 0xFFFF00;
uint32_t red = 0xFF0000;
uint32_t green = 0x00FF00;
uint32_t pink = 0xFF00FF;
uint32_t white = 0xFFFFFF;
uint32_t black = 0x000000;
uint16_t curTime;
uint8_t blinkActive;
uint8_t shiftActive;
float rpmSoundScale = 1.17;

void autox() {

	blinkActive = (curTime & 0b1000000000000) == 0b1000000000000 ? 1 : 0;

	beginDisplayList();

	clear(black);

	/* RPM */
	initBitmapHandleForFont(32, 1);
	initBitmapHandleForFont(33, 2);
	initBitmapHandleForFont(34, 3);

	/* Log Number */
	char logNumVal[6];
	sprintf(logNumVal, "%i", logNumber);
	drawText(100, 30, 1, pink, FT81x_OPT_CENTER, logNumVal);
	drawText(30,30,1,pink,FT81x_OPT_CENTER, "#");

	/* Map State */
	char mapStateVal[6];
	sprintf(mapStateVal, "%i", ECUMapState);
	drawText(750, 30, 1, pink, FT81x_OPT_CENTER, mapStateVal);
	drawText(670,30,1,pink,FT81x_OPT_CENTER, "Map:");

	/* RPM */
	char rpmVal[6];
	rpm = rpm - (rpm % 100);
	sprintf(rpmVal, "%i", rpm);
	drawText(140, 150, 3, yellow, FT81x_OPT_CENTER, rpmVal);
	drawText(140,230,2,yellow,FT81x_OPT_CENTER, "RPM");

	/* GEAR */
	switch (gear) {
		case 0:
			drawLine(320, 300, 320, 80, 8, pink);
			drawLine(320, 80, 480, 300, 8, pink);
			drawLine(480, 300, 480, 80, 8, pink);
			break;

		case 1:
			drawLine(400, 300, 400, 80, 8, pink);
			drawLine(400, 80, 350, 110, 8, pink);
			drawLine(340, 300, 460, 300, 8, pink);
			break;

		case 2:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(480, 80, 480, 190, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(320, 190, 320, 300, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;

		case 3:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(480, 80, 480, 300, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;

		case 4:
			drawLine(420, 300, 420, 80, 8, pink);
			drawLine(420, 80, 320, 230, 8, pink);
			drawLine(320, 230, 450, 230, 8, pink);
			break;

		case 5:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(320, 80, 320, 190, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(480, 190, 480, 300, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;
	}

	/* WTEMP */
	char tmpVal[6];
	sprintf(tmpVal, "%i", wtemp - 40);
	if (wtemp >= 145 && blinkActive) {
		drawRect(560, 50, 220, 80, 1, red);
	}
	drawText(710, 50, 2, lightBlue, FT81x_OPT_RIGHTX, tmpVal);
	drawText(770,50,2,lightBlue,FT81x_OPT_RIGHTX, "C");

	/* LAMBDA */
	char mixVal[6];
	sprintf(mixVal, "%f", mix*0.001);
	mixVal[4] = '\0';
	drawText(770, 140, 2, green, FT81x_OPT_RIGHTX, mixVal);

	/* VOLTAGE */
	char voltageVal[6];
	sprintf(voltageVal, "%f", voltage*0.1);
	voltageVal[4] = '\0';
	if (voltage < 120 && blinkActive) {
		drawRect(560, 230, 220, 80, 1, red);
	}
	drawText(710, 230, 2, yellow, FT81x_OPT_RIGHTX, voltageVal);
	drawText(770,230,2,yellow,FT81x_OPT_RIGHTX, "V");

	/* Shifting Pressure */
    char tmpShiftingPressure[6];
    float scaledPressure;
    scaledPressure = ((shiftingPressure*0.00122070312)-0.5)*1250*0.001;
    scaledPressure = scaledPressure >= 5.0 ? 0.0 : scaledPressure;
    scaledPressure = scaledPressure <= 0.0 ? 0.0 : scaledPressure;
    sprintf(tmpShiftingPressure, "%f", scaledPressure);
    tmpShiftingPressure[3] = 'K';
    tmpShiftingPressure[4] = '\0';
    if (scaledPressure == 0.0) {
    	tmpShiftingPressure[0] = 'N';
    	tmpShiftingPressure[1] = 'a';
    	tmpShiftingPressure[2] = 'N';
    	tmpShiftingPressure[3] = '\0';
    }
    if (scaledPressure <= 1.5 && scaledPressure > 1.0) {
		drawRect(10, 340, 270, 120, 1, yellow);
    } else if (scaledPressure <= 1.0 && blinkActive == 1) {
    	drawRect(10, 340, 270, 120, 1, red);
    }
	drawRect(20,350,scaledPressure/5*250,40,1,lilac);
	drawUnfilledRect(20,350,250,40,2,lilac);
    drawText(130,405,31,lilac,FT81x_OPT_RIGHTX, "SHIFT");
    drawText(270,395,1,lilac,FT81x_OPT_RIGHTX, tmpShiftingPressure);

	/* Fuel Used */
//    char tmpFuelLeft[6];
//    sprintf(tmpFuelLeft, "%f", (530 - fuelUsed) <= 0 ? 0.0 : (530-fuelUsed)*0.01);
//    tmpFuelLeft[3] = 'L';
//    tmpFuelLeft[4] = '\0';
//    if (fuelUsed >= 330 && fuelUsed < 430) {
//		drawRect(510, 340, 270, 120, 1, yellow);
//    } else if (fuelUsed >= 430 && blinkActive == 1) {
//    	drawRect(510, 340, 270, 120, 1, red);
//    }
//	//drawRect(520,350,(fuelUsed >= 530 ? 0 : ((530.0-(1.0*fuelUsed))/530.0))*250,40,1,lightBlue);
//	drawUnfilledRect(520,350,250,40,2,lightBlue);
//    drawText(615,405,31,lightBlue,FT81x_OPT_RIGHTX, "FUEL");
//    drawText(770,395,1,lightBlue,FT81x_OPT_RIGHTX, tmpFuelLeft);

    /* Low Side Shifting Pressure */
    char shiftingLowPressureVal[6];
    float scaledLowPressure;
    scaledLowPressure = (((shiftingLowPressure*0.00122070312)-0.5)/5.0) * 200;
    scaledLowPressure = scaledLowPressure >= 200.0 ? 0.0 : scaledLowPressure;
    scaledLowPressure = scaledLowPressure <= 0.0 ? 0.0 : scaledLowPressure;
    sprintf(shiftingLowPressureVal, "%f", scaledLowPressure);
    shiftingLowPressureVal[3] = '\0';
    if (scaledLowPressure == 0.0) {
    	shiftingLowPressureVal[0] = 'N';
    	shiftingLowPressureVal[1] = 'a';
    	shiftingLowPressureVal[2] = 'N';
    	shiftingLowPressureVal[3] = '\0';
    }
    drawText(650, 340, 31, lightBlue,FT81x_OPT_RIGHTX, "low shift");
    drawText(770, 340, 1, lightBlue,FT81x_OPT_RIGHTX, shiftingLowPressureVal);


    /* Fuel Pressure */
    char fuelPressureVal[6];
    sprintf(fuelPressureVal, "%i", fuelPressure / 70);
        //draw fuelPressure
    drawUnfilledRect(1000 ,750 ,250 ,40 ,2,lightBlue);
    drawText(615, 405, 31, lightBlue,FT81x_OPT_RIGHTX, "psi");
    drawText(770, 395, 1, lightBlue,FT81x_OPT_RIGHTX, fuelPressureVal);

    //SD Warning
	if (!(sdInitialized && sdPresent)) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "NO SD");
	} else if (loggingStatus == 0) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "LOG OFF");
	}

	drawText(400, 450, 31, white, FT81x_OPT_CENTER, "AUTOX");

	swapScreen();
}

void skidpad() {

	blinkActive = (curTime & 0b1000000000000) == 0b1000000000000 ? 1 : 0;

	beginDisplayList();

	clear(black);

	initBitmapHandleForFont(32, 1);
	initBitmapHandleForFont(33, 2);
	initBitmapHandleForFont(34, 3);

	/* Log Number */
	char logNumVal[6];
	sprintf(logNumVal, "%i", logNumber);
	drawText(100, 30, 1, pink, FT81x_OPT_CENTER, logNumVal);
	drawText(30,30,1,pink,FT81x_OPT_CENTER, "#");

	/* RPM */
	char rpmVal[6];
	rpm = rpm - (rpm % 100);
	sprintf(rpmVal, "%i", rpm);
	drawText(140, 150, 3, yellow, FT81x_OPT_CENTER, rpmVal);
	drawText(140,230,2,yellow,FT81x_OPT_CENTER, "RPM");

	/* GEAR */
	switch (gear) {
		case 0:
			drawLine(320, 300, 320, 80, 8, pink);
			drawLine(320, 80, 480, 300, 8, pink);
			drawLine(480, 300, 480, 80, 8, pink);
			break;

		case 1:
			drawLine(400, 300, 400, 80, 8, pink);
			drawLine(400, 80, 350, 110, 8, pink);
			drawLine(340, 300, 460, 300, 8, pink);
			break;

		case 2:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(480, 80, 480, 190, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(320, 190, 320, 300, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;

		case 3:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(480, 80, 480, 300, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;

		case 4:
			drawLine(420, 300, 420, 80, 8, pink);
			drawLine(420, 80, 320, 230, 8, pink);
			drawLine(320, 230, 450, 230, 8, pink);
			break;

		case 5:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(320, 80, 320, 190, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(480, 190, 480, 300, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;
	}

	/* WTEMP */
	char tmpVal[6];
	sprintf(tmpVal, "%i", wtemp - 40);
	if (wtemp >= 145 && blinkActive) {
		drawRect(560, 50, 220, 80, 1, red);
	}
	drawText(710, 50, 2, lightBlue, FT81x_OPT_RIGHTX, tmpVal);
	drawText(770,50,2,lightBlue,FT81x_OPT_RIGHTX, "C");

	/* LAMBDA */
	char mixVal[6];
	sprintf(mixVal, "%f", mix*0.001);
	mixVal[4] = '\0';
	drawText(770, 140, 2, green, FT81x_OPT_RIGHTX, mixVal);

	/* VOLTAGE */
	char voltageVal[6];
	sprintf(voltageVal, "%f", voltage*0.1);
	voltageVal[4] = '\0';
	if (voltage < 120 && blinkActive) {
		drawRect(560, 230, 220, 80, 1, red);
	}
	drawText(710, 230, 2, yellow, FT81x_OPT_RIGHTX, voltageVal);
	drawText(770,230,2,yellow,FT81x_OPT_RIGHTX, "V");

	/* Shifting Pressure */
    char tmpShiftingPressure[6];
    float scaledPressure;
    scaledPressure = ((shiftingPressure*0.00122070312)-0.5)*1250*0.001;
    scaledPressure = scaledPressure >= 5.0 ? 0.0 : scaledPressure;
    scaledPressure = scaledPressure <= 0.0 ? 0.0 : scaledPressure;
    sprintf(tmpShiftingPressure, "%f", scaledPressure);
    tmpShiftingPressure[3] = 'K';
    tmpShiftingPressure[4] = '\0';
    if (scaledPressure == 0.0) {
    	tmpShiftingPressure[0] = 'N';
    	tmpShiftingPressure[1] = 'a';
    	tmpShiftingPressure[2] = 'N';
    	tmpShiftingPressure[3] = '\0';
    }
    if (scaledPressure <= 1.5 && scaledPressure > 1.0) {
		drawRect(10, 340, 270, 120, 1, yellow);
    } else if (scaledPressure <= 1.0 && blinkActive == 1) {
    	drawRect(10, 340, 270, 120, 1, red);
    }
	drawRect(20,350,scaledPressure/5*250,40,1,lilac);
	drawUnfilledRect(20,350,250,40,2,lilac);
    drawText(130,405,31,lilac,FT81x_OPT_RIGHTX, "SHIFT");
    drawText(270,395,1,lilac,FT81x_OPT_RIGHTX, tmpShiftingPressure);

    /* Fuel Used */
//	char tmpFuelLeft[6];
//	sprintf(tmpFuelLeft, "%f", (530 - fuelUsed) <= 0 ? 0.0 : (530-fuelUsed)*0.01);
//	tmpFuelLeft[3] = 'L';
//	tmpFuelLeft[4] = '\0';
//	if (fuelUsed >= 330 && fuelUsed < 430) {
//		drawRect(510, 340, 270, 120, 1, yellow);
//	} else if (fuelUsed >= 430 && blinkActive == 1) {
//		drawRect(510, 340, 270, 120, 1, red);
//	}
//	//drawRect(520,350,(fuelUsed >= 530 ? 0 : ((530.0-fuelUsed)/530.0))*250,40,1,lightBlue);
//	drawUnfilledRect(520,350,250,40,2,lightBlue);
//	drawText(615,405,31,lightBlue,FT81x_OPT_RIGHTX, "FUEL");
//	drawText(770,395,1,lightBlue,FT81x_OPT_RIGHTX, tmpFuelLeft);
    /* Fuel Pressure */
//    char fuelPressureVal[6];
//    sprintf(fuelPressureVal, "%f", fuelPressure / 70);
//        //draw fuelPressure
//    drawUnfilledRect(1000 ,750 ,250 ,40 ,2,lightBlue);
//    drawText(615, 405, 31, lightBlue,FT81x_OPT_RIGHTX, "kPa");
//    drawText(770, 395, 1, lightBlue,FT81x_OPT_RIGHTX, fuelPressureVal);

    //SD Warning
    if (!(sdInitialized && sdPresent)) {
    	drawText(400, 370, 1, red, FT81x_OPT_CENTER, "NO SD");
    } else if (loggingStatus == 0) {
    	drawText(400, 370, 1, red, FT81x_OPT_CENTER, "LOG OFF");
    }

	drawText(400, 450, 31, white, FT81x_OPT_CENTER, "SKIDPAD");

	swapScreen();
}

void sound() {

	blinkActive = (curTime & 0b1000000000000) == 0b1000000000000 ? 1 : 0;

	beginDisplayList();

	clear(black);

	initBitmapHandleForFont(32, 1);
	initBitmapHandleForFont(33, 2);
	initBitmapHandleForFont(34, 3);

	/* Log Number */
	char logNumVal[6];
	sprintf(logNumVal, "%i", logNumber);
	drawText(100, 30, 1, pink, FT81x_OPT_CENTER, logNumVal);
	drawText(30,30,1,pink,FT81x_OPT_CENTER, "#");

	/* Map State */
	char mapStateVal[6];
	sprintf(mapStateVal, "%i", ECUMapState);
	drawText(750, 30, 1, pink, FT81x_OPT_CENTER, mapStateVal);
	drawText(670,30,1,pink,FT81x_OPT_CENTER, "Map:");

	/* RPM */
	char rpmVal[6];
	rpm = (rpm * rpmSoundScale) / 1;
	sprintf(rpmVal, "%i", rpm);
	drawText(140, 150, 3, yellow, FT81x_OPT_CENTER, rpmVal);
	drawText(140,230,2,yellow,FT81x_OPT_CENTER, "RPM");

	/* GEAR */
	switch (gear) {
		case 0:
			drawLine(320, 300, 320, 80, 8, pink);
			drawLine(320, 80, 480, 300, 8, pink);
			drawLine(480, 300, 480, 80, 8, pink);
			break;

		case 1:
			drawLine(400, 300, 400, 80, 8, pink);
			drawLine(400, 80, 350, 110, 8, pink);
			drawLine(340, 300, 460, 300, 8, pink);
			break;

		case 2:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(480, 80, 480, 190, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(320, 190, 320, 300, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;

		case 3:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(480, 80, 480, 300, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;

		case 4:
			drawLine(420, 300, 420, 80, 8, pink);
			drawLine(420, 80, 320, 230, 8, pink);
			drawLine(320, 230, 450, 230, 8, pink);
			break;

		case 5:
			drawLine(320, 80, 480, 80, 8, pink);
			drawLine(320, 80, 320, 190, 8, pink);
			drawLine(320, 190, 480, 190, 8, pink);
			drawLine(480, 190, 480, 300, 8, pink);
			drawLine(320, 300, 480, 300, 8, pink);
			break;
	}

	/* WTEMP */
	char tmpVal[6];
	sprintf(tmpVal, "%i", wtemp - 40);
	if (wtemp >= 145 && blinkActive) {
		drawRect(560, 50, 220, 80, 1, red);
	}
	drawText(710, 50, 2, lightBlue, FT81x_OPT_RIGHTX, tmpVal);
	drawText(770,50,2,lightBlue,FT81x_OPT_RIGHTX, "C");

	/* LAMBDA */
	char mixVal[6];
	sprintf(mixVal, "%f", mix*0.001);
	mixVal[4] = '\0';
	drawText(770, 140, 2, green, FT81x_OPT_RIGHTX, mixVal);

	/* VOLTAGE */
	char voltageVal[6];
	sprintf(voltageVal, "%f", voltage*0.1);
	voltageVal[4] = '\0';
	if (voltage < 120 && blinkActive) {
		drawRect(560, 230, 220, 80, 1, red);
	}
	drawText(710, 230, 2, yellow, FT81x_OPT_RIGHTX, voltageVal);
	drawText(770,230,2,yellow,FT81x_OPT_RIGHTX, "V");

	/* Shifting Pressure */
    char tmpShiftingPressure[6];
    float scaledPressure;
    scaledPressure = ((shiftingPressure*0.00122070312)-0.5)*1250*0.001;
    scaledPressure = scaledPressure >= 5.0 ? 0.0 : scaledPressure;
    scaledPressure = scaledPressure <= 0.0 ? 0.0 : scaledPressure;
    sprintf(tmpShiftingPressure, "%f", scaledPressure);
    tmpShiftingPressure[3] = 'K';
    tmpShiftingPressure[4] = '\0';
    if (scaledPressure == 0.0) {
    	tmpShiftingPressure[0] = 'N';
    	tmpShiftingPressure[1] = 'a';
    	tmpShiftingPressure[2] = 'N';
    	tmpShiftingPressure[3] = '\0';
    }
    if (scaledPressure <= 1.5 && scaledPressure > 1.0) {
		drawRect(10, 340, 270, 120, 1, yellow);
    } else if (scaledPressure <= 1.0 && blinkActive == 1) {
    	drawRect(10, 340, 270, 120, 1, red);
    }
	drawRect(20,350,scaledPressure/5*250,40,1,lilac);
	drawUnfilledRect(20,350,250,40,2,lilac);
    drawText(130,405,31,lilac,FT81x_OPT_RIGHTX, "SHIFT");
    drawText(270,395,1,lilac,FT81x_OPT_RIGHTX, tmpShiftingPressure);

    /* Fuel Used */
	char tmpFuelLeft[6];
	sprintf(tmpFuelLeft, "%f", (530 - fuelUsed) <= 0 ? 0.0 : (530-fuelUsed)*0.01);
	tmpFuelLeft[3] = 'L';
	tmpFuelLeft[4] = '\0';
	if (fuelUsed >= 330 && fuelUsed < 430) {
		drawRect(510, 340, 270, 120, 1, yellow);
	} else if (fuelUsed >= 430 && blinkActive == 1) {
		drawRect(510, 340, 270, 120, 1, red);
	}
	//drawRect(520,350,(fuelUsed >= 530 ? 0 : ((530.0-fuelUsed)/530.0))*250,40,1,lightBlue);
	drawUnfilledRect(520,350,250,40,2,lightBlue);
	drawText(615,405,31,lightBlue,FT81x_OPT_RIGHTX, "FUEL");
	drawText(770,395,1,lightBlue,FT81x_OPT_RIGHTX, tmpFuelLeft);

    //SD Warning
	if (!(sdInitialized && sdPresent)) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "NO SD");
	} else if (loggingStatus == 0) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "LOG OFF");
	}

	drawText(400, 450, 31, white, FT81x_OPT_CENTER, "AUTOCROSS");

	swapScreen();
}

void accel() {

	blinkActive = (curTime & 0b1000000000000) == 0b1000000000000 ? 1 : 0;
	// Set whether we need to shift or not
	if (rpm >= 9500) {
		if ((curTime & 0b100000000000) == 0b100000000000) {
			shiftActive = 1;
		} else {
			shiftActive = 0;
		}
	} else {
		shiftActive = 0;
	}
	//does the above
	//shiftActive = rpm >= 9500 ? ((curTime & 0b100000000000) == 0b100000000000 ? 1 : 0) : 0;

	beginDisplayList();

	clear(shiftActive == 1 ? red : black);

	initBitmapHandleForFont(32, 1);
	initBitmapHandleForFont(33, 2);
	initBitmapHandleForFont(34, 3);

	/* Log Number */
	char logNumVal[6];
	sprintf(logNumVal, "%i", logNumber);
	drawText(100, 30, 1, pink, FT81x_OPT_CENTER, logNumVal);
	drawText(30,30,1,pink,FT81x_OPT_CENTER, "#");

	/* RPM */
	char rpmVal[6];
	rpm = rpm - (rpm % 100);
	sprintf(rpmVal, "%i", rpm);
	drawText(140, 150, 3, shiftActive == 1 ? black : yellow, FT81x_OPT_CENTER, rpmVal);
	drawText(140,230,2,shiftActive == 1 ? black : yellow,FT81x_OPT_CENTER, "RPM");

	/* GEAR */
	switch (gear) {
		case 0:
			drawLine(320, 300, 320, 80, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 80, 480, 300, 8, shiftActive == 1 ? black : pink);
			drawLine(480, 300, 480, 80, 8, shiftActive == 1 ? black : pink);
			break;

		case 1:
			drawLine(400, 300, 400, 80, 8, shiftActive == 1 ? black : pink);
			drawLine(400, 80, 350, 110, 8, shiftActive == 1 ? black : pink);
			drawLine(340, 300, 460, 300, 8, shiftActive == 1 ? black : pink);
			break;

		case 2:
			drawLine(320, 80, 480, 80, 8, shiftActive == 1 ? black : pink);
			drawLine(480, 80, 480, 190, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 190, 480, 190, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 190, 320, 300, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 300, 480, 300, 8, shiftActive == 1 ? black : pink);
			break;

		case 3:
			drawLine(320, 80, 480, 80, 8, shiftActive == 1 ? black : pink);
			drawLine(480, 80, 480, 300, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 190, 480, 190, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 300, 480, 300, 8, shiftActive == 1 ? black : pink);
			break;

		case 4:
			drawLine(420, 300, 420, 80, 8, shiftActive == 1 ? black : pink);
			drawLine(420, 80, 320, 230, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 230, 450, 230, 8, shiftActive == 1 ? black : pink);
			break;

		case 5:
			drawLine(320, 80, 480, 80, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 80, 320, 190, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 190, 480, 190, 8, shiftActive == 1 ? black : pink);
			drawLine(480, 190, 480, 300, 8, shiftActive == 1 ? black : pink);
			drawLine(320, 300, 480, 300, 8, shiftActive == 1 ? black : pink);
			break;
	}
	/* WHEELSPEED */
	char wheelspeedFLVal[6];
	sprintf(wheelspeedFLVal, "%i", wheelspeedFL);
	char wheelspeedFRVal[6];
	sprintf(wheelspeedFRVal, "%i", wheelspeedFR);
	char wheelspeedRLVal[6];
	sprintf(wheelspeedRLVal, "%i", wheelspeedRL);
	char wheelspeedRRVal[6];
	sprintf(wheelspeedRRVal, "%i", wheelspeedRR);
	drawText(700 , 50 , 1, green, FT81x_OPT_RIGHTX, wheelspeedFLVal);
	drawText(760 , 50 , 20, green, FT81x_OPT_RIGHTX, "km/h FL");
	drawText(700 , 140 , 1, green, FT81x_OPT_RIGHTX, wheelspeedFRVal);
	drawText(760 , 140 , 20, green, FT81x_OPT_RIGHTX, "km/h FR");
	drawText(700 , 230 , 1, green, FT81x_OPT_RIGHTX, wheelspeedRLVal);
	drawText(760 , 230 , 20, green, FT81x_OPT_RIGHTX, "km/h RL");
	drawText(700 , 320 , 1, green, FT81x_OPT_RIGHTX, wheelspeedRRVal);
	drawText(760 , 320 , 20, green, FT81x_OPT_RIGHTX, "km/h RR");

	//SD Warning
	if (!(sdInitialized && sdPresent)) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "NO SD");
	} else if (loggingStatus == 0) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "LOG OFF");
	}

	drawText(400, 450, 31, white, FT81x_OPT_CENTER, "ACCEL");

	swapScreen();
}

void diag() {

	blinkActive = (curTime & 0b1000000000000) == 0b1000000000000 ? 1 : 0;

	beginDisplayList();

	clear(black);

	initBitmapHandleForFont(32, 1);
	initBitmapHandleForFont(33, 2);
	initBitmapHandleForFont(34, 3);

	// Log Number
	char logNumVal[6];
	sprintf(logNumVal, "%i", logNumber);
	drawText(100, 30, 1, pink, FT81x_OPT_CENTER, logNumVal);
	drawText(30,30,1,pink,FT81x_OPT_CENTER, "#");

	// RPM
	char rpmVal[6];
	rpm = rpm - (rpm % 100);
	sprintf(rpmVal, "%i", rpm);
	drawText(140, 150, 3, shiftActive == 1 ? black : yellow, FT81x_OPT_CENTER, rpmVal);
	drawText(140,230,2,shiftActive == 1 ? black : yellow,FT81x_OPT_CENTER, "RPM");

	// Front Brake Pressure
	char fbpVal[6];


	// SD Warning
	if (!(sdInitialized && sdPresent)) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "NO SD");
	} else if (loggingStatus == 0) {
		drawText(400, 370, 1, red, FT81x_OPT_CENTER, "LOG OFF");
	}

	drawText(400, 450, 31, white, FT81x_OPT_CENTER, "DIAG");

	swapScreen();
}


void screenWelcome() {
  beginDisplayList();
  clear(0x003262);
  initBitmapHandleForFont(33, 2);
  drawText(400, 240, 2, 0xFDB515, FT81x_OPT_CENTER, "GOB EARS!\0");
  swapScreen();
}

void screenBrightnessChange() {
	if (screenBrightness == 128) {
		screenBrightness = 80;
	} else if (screenBrightness == 80) {
		screenBrightness = 60;
	} else if (screenBrightness == 60) {
		screenBrightness = 40;
	} else if (screenBrightness == 40) {
		screenBrightness = 20;
	} else if (screenBrightness == 20) {
		screenBrightness = 128;
	}
	write8(FT81x_REG_PWM_DUTY, screenBrightness);
}

void displayScreen() {
	 curTime = getTime();
	 switch (curScreen) {
	 	 case AUTOX:
	 		 autox();
	 		 break;
	 	 case SKIDPAD:
	 		 skidpad();
	 		 break;
	 	 case ACCEL:
	 		 accel();
	 		 break;
	 	 case SOUND:
	 		 sound();
			 break;
	 	 case 99:
	 		 screenWelcome();
	 		 break;
	 	 default:
	 		 break;
	  }
}
