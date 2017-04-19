#include "micromenu.h"
#include "model.h"
#define STATE_OPTIONS_MAX 2

const extern MenuItem_t Root_Setpoint;
const extern MenuItem_t Root_Display;
const extern MenuItem_t Root_Weekday;
const extern MenuItem_t Root_Temp;

const extern MenuItem_t Menu_State_Dialog;
const extern MenuItem_t Menu_SetSetpoint_Dialog;

const char * const STR_TIME[]={"Menu 1"};
const char * const STR_WEEKDAY[]={"Menu 2"};
const char * const STR_TEMP[]={"Menu 3"};
const char * const STR_SETPOINT[]={"SETPOINT"};
const char * const STR_GRIB[]={"Grib"};
const char * const STR_SET_MODE[]={"Set mode:"};
const char * const STR_SET_SETPOINT[]={"Setpoint:"};

//MENU_ITEM(Name, Next, Previous, Parent, Child, GetLine1Func, GetLine2Func, line1, line2)

const char * const STR_STATE[]={"IDLE", "AUTO", "MANUAL"};

void menuGetState() {
	sprintf(lcdBuf, "%s", STR_STATE[model.state]);
}

void menuGetNumbers() {
	sprintf(lcdBuf, "Temp: %d Set: %d", model.tempCurr, model.tempSetpoint);
}

void stateDialogHandler(uint8_t event) {
    switch(event) {
        case EVENT_SETUP:
            optDialogOptions = states[LANGUAGE_DEFAULT];
            optDialogValue = model.state;
            optDialogOptionsMax = STATE_OPTIONS_MAX;
            break;
        case EVENT_ENTER:
            model.state = optDialogValue;
            //modelSaveByte(model.curLanguage, offsetof(Model_t, curLanguage));
            break;
        default:
            optDialogDefault(event);
    }
}

void setpointDialogHandler(uint8_t event) {
    switch(event) {
        case EVENT_SETUP:
            numDialogValue = model.tempSetpoint;
            numDialogMin = 20;
            numDialogMax = 300;
            break;
        case EVENT_ENTER:
            model.tempSetpoint = numDialogValue;
            //modelSaveWord(model.minTemp, offsetof(Model_t, minTemp));
            break;
        default:
            numDialogDefault(event);
    }
}

void menuGetSetpoint() {
    writeTempTo(lcdBuf, model.tempSetpoint);
}

//MENU_ITEM(Name, Next, Previous, Parent, Child, GetLine1Func, GetLine2Func, line1, line2)

MENU_ITEM(Root_Display, Root_Weekday, Root_Weekday, NULL_MENU, Menu_State_Dialog, menuGetState, menuGetNumbers, NULL, NULL);
MENU_ITEM(Root_Weekday, Root_Display, Root_Display, NULL_MENU, Menu_SetSetpoint_Dialog, NULL, menuGetSetpoint, STR_SETPOINT, NULL);
// MENU_ITEM(Root_Temp, Root_Setpoint, Root_Weekday, NULL_MENU, NULL_MENU, NULL, NULL, STR_TEMP, NULL);
// MENU_ITEM(Root_Setpoint, Root_Display, Root_Temp, NULL_MENU, NULL_MENU, NULL, NULL, STR_SETPOINT, NULL);

//MENU_DIALOG(Name, Parent, GetLine1Func, GetLine2Func, line1, line2, DialogHandlerFunc)
MENU_DIALOG(Menu_State_Dialog, Root_Display, NULL, menuGetOptDialogValue, STR_SET_MODE, NULL, stateDialogHandler);
MENU_DIALOG(Menu_SetSetpoint_Dialog, Root_Weekday, NULL, menuGetNumDialogValue, STR_SET_SETPOINT, NULL, setpointDialogHandler);

uint8_t isIdleMenu() {
    const MenuItem_t *curMenu = menuGetCurrent();
    return 0;
    // return curMenu == &Root_Setpoint ||
    //        curMenu == &Root_Temp ||
    //        curMenu == &Root_Display ||
    //        curMenu == &Root_Weekday;
}
