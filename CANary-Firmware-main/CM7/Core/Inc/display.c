/*
 * display.c
 *
 *  Created on: Jan 18, 2024
 *      Author: greyson
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
uint32_t white = 0xFFFFFF;
uint32_t black = 0x000000;
uint32_t curTime;
uint8_t blinkActive;

void smallWarning(int warningIndex) {
  drawRect(160, 400, 480, 80, 1, 0xFF0000);
  drawLine(160,400,640,400,1,0x000000);
  drawLine(160,400,160,480,1,0x000000);
  drawLine(640,400,640,480,1,0x000000);
  drawText(400, 440, 30, 0x000000, FT81x_OPT_CENTER, "WARNING");
  //const char* str = warningList[warningIndex]->name.c_str();
  //drawText(240, 250, 29, 0x000000, FT81x_OPT_CENTER, str);
}

void bigWarning(int warningIndex) {
  clear(0xFF0000);
  initBitmapHandleForFont(33, 2);
  drawText(400, 180, 2, 0x000000, FT81x_OPT_CENTER, "WARNING:");
  //std::string warnName = warningList[warningIndex]->name;
  //int len = warnName.length();
  //char* char_array = new char[len + 1];
  //strcpy(char_array, warnName.c_str());
  //drawText(240, 190, 31, 0x000000, FT81x_OPT_CENTER, char_array);
}

void bottomScreenSection(uint32_t color, uint32_t lineColor) {
  /* FRAME: */
  drawLine(0,370,800,370,2,lineColor);
  drawLine(160,370,160,480,2,lineColor);
  drawLine(640,370,640,480,2,lineColor);

  /* TODO BFR LOGO: */
  drawText(400, 420, 30, 0x000000, FT81x_OPT_CENTER, "GOB EARS!");

}

//void screen1(int warnType, int warningIndex, int seconds) {
//  uint32_t lineColor;
//
//  beginDisplayList();
//  if (warnType == 2 && warningIndex >= 0 && warningIndex < 9) {
//    bigWarning(warningIndex);
//  } else {
//    clear(0xFFFFFF);
//
////    if (warningIndex == 7) {
////      lineColor = 0xFF0000;
////    } else {
////      lineColor = 0x000000;
////    }
////
////    if ((seconds / 200) % 2 == 0) {
////      if (warningIndex == 7) {
////        arrowColor = 0xFF0000;
////      } else {
////        arrowColor = 0x000000;
////      }
////
////      if (warningIndex == 0 || warningIndex == 2) {
////        tempTextColor = 0xFF0000;
////      } else {
////        tempTextColor = 0x000000;
////      }
////    } else {
////      arrowColor = 0x000000;
////      tempTextColor = 0x000000;
////    }
//
//    lineColor = shutdown == 0 ? 0x000000 : 0xFF0000;
//
//    /* FRAME: */
//    drawLine(0,180,800,180,2,lineColor);
//    drawLine(400,0,400,180,2,lineColor);
//    drawLine(300,180,300,370,2,lineColor);
//    drawLine(500,180,500,370,2,lineColor);
//
//    bottomScreenSection(0xFFFFFF, lineColor);
//
//    /* RPM */
//    initBitmapHandleForFont(33);
//    char rpmVal[6];
//    rpm = rpm - (rpm % 100);
//    sprintf(rpmVal, "%i", rpm);
//    drawText(240, 50, 1, 0x000000, FT81x_OPT_RIGHTX, rpmVal);
//    initBitmapHandleForFont(32);
//    drawText(380,60,1,0x000000,FT81x_OPT_RIGHTX, "RPM");
//
//    /* Fuel Pressure */
//    //will either be 100% of value or 0%, typical 60psi
////    initBitmapHandleForFont(33);
////    char tmpFuelPressure[6];
////    sprintf(tmpFuelPressure, "%i", fuelPressure);
////    tmpFuelPressure[3] = tmpFuelPressure[2];
////    tmpFuelPressure[2] = tmpFuelPressure[1];
////    tmpFuelPressure[1] = '.';
////    tmpFuelPressure[4] = '\0';
////    drawText(240, 350, 1, fuelPressure >= 0 ? 0x000000 : 0xFF0000, FT81x_OPT_RIGHTX, tmpFuelPressure);
////    drawText(380,370,31,fuelPressure >= 0 ? 0x000000 : 0xFF0000,FT81x_OPT_RIGHTX, "kPa");
//
//    /* Shifting Pressure */
//    //6k max, precision to 100. X.XkPa
////    initBitmapHandleForFont(33);
////    char tmpShiftingPressure[6];
////    sprintf(tmpShiftingPressure, "%i", shiftingPressure);
////    tmpShiftingPressure[2] = tmpShiftingPressure[1];
////    tmpShiftingPressure[1] = '.';
////    tmpShiftingPressure[3] = '\0';
////    drawText(600, 350, 1, shiftingPressure >= 0 ? 0x000000 : 0xFF0000, FT81x_OPT_RIGHTX, tmpShiftingPressure);
////    drawText(780,370,31,shiftingPressure >= 0 ? 0x000000 : 0xFF0000,FT81x_OPT_RIGHTX, "kPa");
//
//    /* WTEMP */
//    initBitmapHandleForFont(33);
//    char tmpVal[6];
//    sprintf(tmpVal, "%i", wtemp - 40);
//    drawText(600, 50, 1, wtemp-40 >= 100 ? 0xFF0000 : 0x000000, FT81x_OPT_RIGHTX, tmpVal);
//    drawText(780,70,31,wtemp-40 >= 100 ? 0xFF0000 : 0x000000,FT81x_OPT_RIGHTX, "WTEMP");
//
//    /* LAMBDA */
//    initBitmapHandleForFont(34);
//    char mixVal[6];
//    if (mix == 10000) {
//    	mix = 1000;
////    	sprintf(mixVal, "%i", mix);
////    	mixVal[3] = mixVal[2];
////		mixVal[2] = '.';
////		mixVal[4] = '\0';
//    }
//	sprintf(mixVal, "%f", mix*0.01);
//	mixVal[4] = '\0';
//    drawText(190, 220, 1, 0x000000, FT81x_OPT_RIGHTX, mixVal);
//    initBitmapHandleForFont(32);
//    drawText(280,260,31,0x000000,FT81x_OPT_RIGHTX, "MIX");
//
//    /* GEAR */
//    initBitmapHandleForFont(34);
//    char gearVal[6];
//    sprintf(gearVal, "%i", gear);
//    drawText(400, 280, 1, 0x000000, FT81x_OPT_CENTER, gearVal);
//
//    /* VOLTAGE */
//    initBitmapHandleForFont(34);
//    char voltageVal[6];
//    sprintf(voltageVal, "%f", voltage*0.1);
//    voltageVal[4] = '\0';
//    drawText(700, 220, 1, 0x000000, FT81x_OPT_RIGHTX, voltageVal);
//    drawText(780,220,1,0x000000,FT81x_OPT_RIGHTX, "V");
//
//    if (warnType == 1 && warningIndex >= 0 && warningIndex < 9) {
//      smallWarning(warningIndex);
//    }
//  }
//  swapScreen();
//}
//
//void screen2(int warnType, int warningIndex, int seconds) {
//
//	blinkActive = (curTime & 0b1000000000000) == 0b1000000000000 ? 1 : 0;
//
//	beginDisplayList();
//
//	clear(black);
//
//	/* RPM */
//	initBitmapHandleForFont(32, 1);
//	initBitmapHandleForFont(33, 2);
//	initBitmapHandleForFont(34, 3);
//	char rpmVal[6];
//	rpm = rpm - (rpm % 100);
//	sprintf(rpmVal, "%i", rpm);
//	drawText(140, 150, 3, yellow, FT81x_OPT_CENTER, rpmVal);
//	drawText(140,230,2,yellow,FT81x_OPT_CENTER, "RPM");
//
//	/* GEAR */
//	switch (gear) {
//		case 0:
//			drawLine(320, 300, 320, 80, 8, green);
//			drawLine(320, 80, 480, 300, 8, green);
//			drawLine(480, 300, 480, 80, 8, green);
//			break;
//
//		case 1:
//			drawLine(400, 300, 400, 80, 8, green);
//			drawLine(400, 80, 350, 110, 8, green);
//			drawLine(340, 300, 460, 300, 8, green);
//			break;
//
//		case 2:
//			drawLine(320, 80, 480, 80, 8, green);
//			drawLine(480, 80, 480, 190, 8, green);
//			drawLine(320, 190, 480, 190, 8, green);
//			drawLine(320, 190, 320, 300, 8, green);
//			drawLine(320, 300, 480, 300, 8, green);
//			break;
//
//		case 3:
//			drawLine(320, 80, 480, 80, 8, green);
//			drawLine(480, 80, 480, 300, 8, green);
//			drawLine(320, 190, 480, 190, 8, green);
//			drawLine(320, 300, 480, 300, 8, green);
//			break;
//
//		case 4:
//			drawLine(420, 300, 420, 80, 8, green);
//			drawLine(420, 80, 320, 230, 8, green);
//			drawLine(320, 230, 450, 230, 8, green);
//			break;
//
//		case 5:
//			drawLine(320, 80, 480, 80, 8, green);
//			drawLine(320, 80, 320, 190, 8, green);
//			drawLine(320, 190, 480, 190, 8, green);
//			drawLine(480, 190, 480, 300, 8, green);
//			drawLine(320, 300, 480, 300, 8, green);
//			break;
//	}
//
//	/* WTEMP */
//	char tmpVal[6];
//	sprintf(tmpVal, "%i", wtemp - 40);
//	if (wtemp >= 145 && blinkActive) {
//		drawRect(560, 50, 220, 80, 1, red);
//	}
//	drawText(710, 50, 2, lightBlue, FT81x_OPT_RIGHTX, tmpVal);
//	drawText(770,50,2,lightBlue,FT81x_OPT_RIGHTX, "C");
//
//	/* LAMBDA */
//	char mixVal[6];
//	sprintf(mixVal, "%f", mix*0.001);
//	mixVal[4] = '\0';
//	drawText(770, 140, 2, green, FT81x_OPT_RIGHTX, mixVal);
//
//	/* VOLTAGE */
//	char voltageVal[6];
//	sprintf(voltageVal, "%f", voltage*0.1);
//	voltageVal[4] = '\0';
//	if (voltage < 120 && blinkActive) {
//		drawRect(560, 230, 220, 80, 1, red);
//	}
//	drawText(710, 230, 2, yellow, FT81x_OPT_RIGHTX, voltageVal);
//	drawText(770,230,2,yellow,FT81x_OPT_RIGHTX, "V");
//
//	/* Shifting Pressure */
//    char tmpShiftingPressure[6];
//    float scaledPressure;
//    scaledPressure = ((shiftingPressure*0.00122070312)-0.5)*1250*0.001;
//    scaledPressure = scaledPressure >= 5.0 ? 0.0 : scaledPressure;
//    sprintf(tmpShiftingPressure, "%f", scaledPressure);
//    tmpShiftingPressure[3] = 'K';
//    tmpShiftingPressure[4] = '\0';
//    if (scaledPressure == 0.0) {
//    	tmpShiftingPressure[0] = 'N';
//    	tmpShiftingPressure[1] = 'a';
//    	tmpShiftingPressure[2] = 'N';
//    	tmpShiftingPressure[3] = '\0';
//    }
//    if (scaledPressure <= 1.5 && scaledPressure > 1.0) {
//		drawRect(10, 340, 270, 120, 1, yellow);
//    } else if (scaledPressure <= 1.0 && blinkActive == 1) {
//    	drawRect(10, 340, 270, 120, 1, red);
//    }
//	drawRect(20,350,scaledPressure/5*250,40,1,lilac);
//	drawUnfilledRect(20,350,250,40,2,lilac);
//    drawText(130,405,31,lilac,FT81x_OPT_RIGHTX, "SHIFT");
//    drawText(270,395,1,lilac,FT81x_OPT_RIGHTX, tmpShiftingPressure);
//
//	/* Fuel Used */
//    char tmpFuelUsed[6];
//    sprintf(tmpFuelUsed, "%f", fuelUsed >= 280 ? 2.8 : fuelUsed*0.01);
//    tmpFuelUsed[3] = 'L';
//    tmpFuelUsed[4] = '\0';
//    if (fuelUsed >= 240 && fuelUsed < 270) {
//		drawRect(510, 340, 270, 120, 1, yellow);
//    } else if (fuelUsed >= 270 && blinkActive == 1) {
//    	drawRect(510, 340, 270, 120, 1, red);
//    }
//	drawRect(520,350,(fuelUsed >= 280 ? 1 : (fuelUsed/280))*250,40,1,lightBlue);
//	drawUnfilledRect(520,350,250,40,2,lightBlue);
//    drawText(615,405,31,lightBlue,FT81x_OPT_RIGHTX, "FUEL");
//    drawText(770,395,1,lightBlue,FT81x_OPT_RIGHTX, tmpFuelUsed);
//
//    //SD Warning
//    if (!(sdInitialized && sdPresent)) {
//    	drawText(400, 370, 1, red, FT81x_OPT_CENTER, "NO SD");
//    }
//
//	drawText(400, 450, 31, white, FT81x_OPT_CENTER, "AUTOX");
//
//	swapScreen();
//}


void screenWelcome() {
  beginDisplayList();
  clear(0x003262);
  initBitmapHandleForFont(33, 2);
  drawText(400, 240, 2, 0xFDB515, FT81x_OPT_CENTER, "GOB EARS!\0");
  swapScreen();
}

void screenRed() {
  beginDisplayList();
  clear(0xFF0000);
  initBitmapHandleForFont(33, 2);
  drawText(400, 240, 2, 0xFFFFFF, FT81x_OPT_CENTER, "GOB EARS!\0");
  swapScreen();
}

void screenGreen() {
  beginDisplayList();
  clear(0x00FF00);
  initBitmapHandleForFont(33, 2);
  drawText(400, 240, 2, 0xFFFFFF, FT81x_OPT_CENTER, "GOB EARS!\0");
  swapScreen();
}

void screenBlue() {
  beginDisplayList();
  clear(0x0000FF);
  initBitmapHandleForFont(33, 2);
  drawText(400, 240, 2, 0xFFFFFF, FT81x_OPT_CENTER, "GOB EARS!\0");
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

void displayScreen(int index) {
	 //curTime = getTime();
	 switch (index) {
	 	 case 1:
	 		 //screen1(0,0,0);
	 		 break;
	 	 case 2:
	 		 //screen2(0,0,0);
	 		 break;
	 	 case 3:
	 		 screenBlue();
	 		 break;
	 	 case 99:
	 		 screenWelcome();
	 		 break;
	 	 default:
	 		 break;
	  }
}
