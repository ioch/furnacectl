#ifndef VIEW_H
#define	VIEW_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <LiquidCrystal_I2C.h>

#include "micromenu.h"

#define LANGUAGE_DEFAULT 0

#define EVENT_UP        1
#define EVENT_DOWN      2
#define EVENT_LEFT      3
#define EVENT_RIGHT     4
#define EVENT_ENTER     5
#define EVENT_CANCEL    6
#define EVENT_SETUP     7

char lcdBuf[17];
LiquidCrystal_I2C lcd(0x27,20,4);


void writeTempTo(char* buf, int temp);

uint8_t isValueSettingDialog();
uint8_t isFrequentUpdateMenu();
// void menuCursorControl(const MenuItem_t *curMenu);
void menuDisplay(uint8_t language);
// void writeTimeTo(char* buf, uint8_t hours, uint8_t minutes, uint8_t format);
// void writeConvertedTempTo(char* buf, int temp, uint8_t scale);
uint8_t isDialogMenu();
uint8_t isIdleMenu();
uint8_t isControlBlockingMenu();

extern uint8_t cursorPos;

extern int8_t optDialogValue;
extern int8_t optDialogOptionsMax;
extern int numDialogValue;
extern int numDialogMin;
extern int numDialogMax;

void menuGetNumDialogTemp();
void menuGetNumDialogValue();

extern const char * const * optDialogOptions;
void menuGetOptDialogValue();

void numDialogDefault(uint8_t event);
void optDialogDefault(uint8_t event);

void writeTempTo(char* buf, int temp) {
    sprintf(buf, "%dC", temp);
}


const char * const STR_EMPTY = "";

void menuDisplay(uint8_t language) {
    const MenuItem_t *curMenu = menuGetCurrent();
    char line1buf[17];
    char line2buf[17];

    if(NULL != curMenu->line1) {
        strcpy(lcdBuf, curMenu->line1[language]);
    } else if(NULL != curMenu->getLine1) {
        curMenu->getLine1();
    } else {
    	strcpy(lcdBuf, STR_EMPTY);
    }
    strcpy(line1buf, lcdBuf);

    if(NULL != curMenu->line2) {
        strcpy(lcdBuf, curMenu->line2[language]);
    } else if(NULL != curMenu->getLine2) {
        curMenu->getLine2();
    } else {
    	strcpy(lcdBuf, STR_EMPTY);
    }
    strcpy(line2buf, lcdBuf);

    // lcdWriteCmd(CLEAR_DISPLAY);
    // lcdPutsToAddr(line1buf, ADDR_LINE1);
    // lcdPutsToAddr(line2buf, ADDR_LINE2);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1buf);
    lcd.setCursor(0, 1);
    lcd.print(line2buf);

    // menuCursorControl(curMenu);

    menuDisplayed();
}



uint8_t isFrequentUpdateMenu() {
    const MenuItem_t *curMenu = menuGetCurrent();
    return 0;
    // return curMenu == &Root_Temp ||
    //        curMenu == &Install_Sensors_Room_Display ||
    //        curMenu == &Install_Sensors_Floor_Display ||
    //        curMenu == &Install_Sensors_Limit_Display ||
    //        curMenu == &Install_Gfi_Measure_Display;
}

uint8_t isControlBlockingMenu() {
    const MenuItem_t *curMenu = menuGetCurrent();
    return 0;
    // return curMenu == &Install_Gfi_Measure_Display ||
    //        curMenu == &Install_Gfi_Test_Display;
}

uint8_t isValueSettingDialog() {
    const MenuItem_t *curMenu = menuGetCurrent();
    return 0;
    // return curMenu != &Schedule_Weekday_Dialog &&
    //         curMenu != &Schedule_Slot_Dialog &&
    //         curMenu != &Schedule_SlotStart_Dialog &&
    //         curMenu != &Schedule_SlotEnd_Dialog &&
    //         curMenu != &Install_Gfi_Measure_Display &&
    //         curMenu != &Install_Gfi_Test_Display;
}

uint8_t isDialogMenu() {
    const MenuItem_t* curMenu = menuGetCurrent();
    return 0!= curMenu->dialogHandler;
}


uint8_t cursorPos=0;

int numDialogValue = 0;
int numDialogMin = 0;
int numDialogMax = 0;

int8_t optDialogValue = 0;
int8_t optDialogOptionsMax = 0;

void menuGetNumDialogTemp() {
    writeTempTo(lcdBuf, numDialogValue);
}

void menuGetNumDialogValue() {
    sprintf(lcdBuf, "%d", numDialogValue);
//    itoa(lcdBuf, numDialogValue, 10);
}


const char * const * optDialogOptions;

void menuGetOptDialogValue() {
    strcpy(lcdBuf, optDialogOptions[optDialogValue]);
}

void numDialogDefault(uint8_t event) {
    switch(event) {
        case EVENT_UP:
            numDialogValue++;
            if(numDialogValue > numDialogMax) {
                numDialogValue = numDialogMin;
            }
            break;
        case EVENT_DOWN:
            numDialogValue--;
            if(numDialogValue < numDialogMin) {
                numDialogValue = numDialogMax;
            }
    }
}

void optDialogDefault(uint8_t event) {
    switch(event){
        case EVENT_UP:
            optDialogValue++;
            if(optDialogValue > optDialogOptionsMax) {
                optDialogValue = 0;
            }
            break;
        case EVENT_DOWN:
            optDialogValue--;
            if(optDialogValue < 0) {
                optDialogValue = optDialogOptionsMax;
            }
    }
}

#endif	/* VIEW_H */