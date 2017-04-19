#include <LiquidCrystal_I2C.h>
#include "micromenu.h"
#include "view.h"
#include "menuIdle.h"
#include "pins.h"
#include <Bounce2.h>
#include <Encoder.h>

Bounce buttonEncoder = Bounce();
Bounce buttonRed = Bounce();

Encoder encoder(encode1, encode2);

uint32_t menuLastTimeoutSeconds = 0;
void menuTimeoutReset() {
    menuLastTimeoutSeconds = millis() / 10;
}

int32_t encoderPos = 0;

void setup() {
	Serial.begin(9600);
	lcd.begin();
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0,0);

	buttonEncoder.attach(buttEnc);
	buttonEncoder.interval(100);
	buttonRed.attach(buttRed);  
	buttonRed.interval(100);

	menuNavigate(&Root_Display);
	menuTimeoutReset();
	encoderPos = encoder.read()/4;
}


void onDownPressed(void) {
    if(isIdleMenu()) {
//        modeOccupiedOn();
        menuNavigate(MENU_NEXT);
    } else if(isDialogMenu()){
        menuGetCurrent()->dialogHandler(EVENT_DOWN);
        menuSetUpdated();
    } else {
        menuNavigate(MENU_NEXT);
    }
    menuSetUpdated();
    menuTimeoutReset();
    lcd.backlight();
}

void onUpPressed(void) {
    if(isIdleMenu()) {
//        modeOccupiedOn();
        menuNavigate(MENU_PREVIOUS);
    } else if(isDialogMenu()){
        menuGetCurrent()->dialogHandler(EVENT_UP);
        menuSetUpdated();
    } else {
        menuNavigate(MENU_PREVIOUS);
    }
    menuSetUpdated();
    menuTimeoutReset();
    lcd.backlight();
}


void onEnterPressed(void) {
    if(isDialogMenu()) {
        const MenuItem_t *curMenu = menuGetCurrent();

        if(isValueSettingDialog()) {
//            lcdPutsToAddr(STR_UPDATING[model.curLanguage], ADDR_LINE2);
            uint32_t updateStart = millis();
            curMenu->dialogHandler(EVENT_ENTER);
            while(millis() - updateStart < 500) {
//                CLRWDT();
                //WAIT
            }
        } else {
            curMenu->dialogHandler(EVENT_ENTER);
        }

        const MenuItem_t *nextMenu = menuGetNext();

        if(&NULL_MENU != nextMenu) {
            menuNavigate(nextMenu);
            menuSetNext(&NULL_MENU);
        } else if(&NULL_MENU != curMenu->parent && 0 != curMenu->parent){
            menuNavigate(curMenu->parent);
        }
    } else {
        menuNavigate(MENU_CHILD);
    }
    menuSetUpdated();
    menuTimeoutReset();
    lcd.backlight();
}

void onCancelPressed(void) {
    if(isIdleMenu()) {
        // if(isOccupied()) {
        //     ledOff();
        //     setOccupied(FALSE);
        // } else {
        //     modeOccupiedOn();
        // }
    } else if(isDialogMenu()){
        menuGetCurrent()->dialogHandler(EVENT_CANCEL);
        menuNavigate(MENU_PARENT);
    } else {
        menuNavigate(MENU_PARENT);
    }
    menuSetUpdated();
    menuTimeoutReset();
    lcd.backlight();
}


void loop() {
    if(menuIsUpdated()) {
        menuDisplay(LANGUAGE_DEFAULT);
    }



    if(buttonEncoder.fell()) {
	   	onEnterPressed();
    }

    if(buttonRed.fell()) {
    	onCancelPressed();
    }

    int32_t encoderNew = encoder.read()/4;

    if(encoderNew > encoderPos) {
    	onUpPressed();
		encoderPos = encoderNew;    	
    } else if(encoderNew < encoderPos) {
    	onDownPressed();
		encoderPos = encoderNew;    	
    }


    buttonEncoder.update();
    buttonRed.update();
}
