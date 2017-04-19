#ifndef MICROMENU_H
#define	MICROMENU_H

#include <stdint.h>
#include "menuconfig.h"

/** Type define for a menu item. Menu items should be initialized via the helper
 *  macro \ref MENU_ITEM(), not created from this type directly in user-code.
 */
typedef struct MenuItem {
    const struct MenuItem * const next;
    const struct MenuItem * const previous;
    const struct MenuItem * const parent;
    const struct MenuItem * const child;

    void (* const getLine1)(void);
    void (* const getLine2)(void);
    const char * const *line1;
    const char * const *line2;
    void (* const dialogHandler)(uint8_t event);
} MenuItem_t;

/** Creates a new menu item entry with the specified links and callbacks.
 *
 *  \param[in] Name      Name of the menu entry, must be unique.
 *  \param[in] Next      Name of the next linked menu item, or \ref NULL_MENU if no menu link.
 *  \param[in] Previous  Name of the previous linked menu item, or \ref NULL_MENU if no menu link.
 *  \param[in] Parent    Name of the parent linked menu item, or \ref NULL_MENU if no menu link.
 *  \param[in] Child     Name of the child linked menu item, or \ref NULL_MENU if no menu link.
 *  \param[in] SelectFunc  Function callback to execute when the menu item is selected, or \c NULL for no callback.
 *  \param[in] EnterFunc   Function callback to execute when the menu item is entered, or \c NULL for no callback.
 */
#define MENU_ITEM(Name, Next, Previous, Parent, Child, GetLine1Func, GetLine2Func, line1, line2) \
        const extern MenuItem_t MENU_ITEM_STORAGE Next;     \
        const extern MenuItem_t MENU_ITEM_STORAGE Previous; \
        const extern MenuItem_t MENU_ITEM_STORAGE Parent;   \
        const extern MenuItem_t MENU_ITEM_STORAGE Child;  \
        const MenuItem_t MENU_ITEM_STORAGE Name = {&Next, &Previous, &Parent, &Child, GetLine1Func, GetLine2Func, line1, line2, NULL}

#define MENU_DIALOG(Name, Parent, GetLine1Func, GetLine2Func, line1, line2, DialogHandlerFunc) \
        const extern MenuItem_t MENU_ITEM_STORAGE Parent;   \
        const MenuItem_t MENU_ITEM_STORAGE Name = {&NULL_MENU, &NULL_MENU, &Parent, &NULL_MENU, GetLine1Func, GetLine2Func, line1, line2, DialogHandlerFunc}

/** Relative navigational menu entry for \ref Menu_Navigate(), to move to the menu parent. */
#define MENU_PARENT         MENU_ITEM_READ_POINTER(menuGetCurrent()->parent)

/** Relative navigational menu entry for \ref Menu_Navigate(), to move to the menu child. */
#define MENU_CHILD          MENU_ITEM_READ_POINTER(menuGetCurrent()->child)

/** Relative navigational menu entry for \ref Menu_Navigate(), to move to the next linked menu item. */
#define MENU_NEXT           MENU_ITEM_READ_POINTER(menuGetCurrent()->next)

/** Relative navigational menu entry for \ref Menu_Navigate(), to move to the previous linked menu item. */
#define MENU_PREVIOUS       MENU_ITEM_READ_POINTER(menuGetCurrent()->previous)

/** Null menu entry, used in \ref MENU_ITEM() definitions where no menu link is to be made. */
extern const MenuItem_t NULL_MENU;

/** Retrieves the currently selected meny item.
 *
 *  \return Pointer to the currently selected meny item.
 */
const MenuItem_t* menuGetCurrent(void);
const MenuItem_t* menuGetNext(void);
void menuSetNext(const MenuItem_t*);
/** Navigates to an absolute or relative menu entry.
 *
 * \param[in] NewMenu  Pointer to the absolute menu item to select, or one of \ref MENU_PARENT,
 *                     \ref MENU_CHILD, \ref MENU_NEXT or \ref MENU_PREVIOUS for relative navigation.
 */
void menuNavigate(const MenuItem_t* NewMenu);

extern uint8_t menuUpdated;
#define menuIsUpdated() (menuUpdated)
#define menuSetUpdated() menuUpdated = 1
#define menuDisplayed() menuUpdated = 0

const MenuItem_t NULL_MENU = {0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t menuUpdated;

/** \internal
 *  Pointer to the currently selected menu item.
 */
static const MenuItem_t* CurrentMenuItem = (MenuItem_t*) &NULL_MENU;

static const MenuItem_t* NextMenuItem = (MenuItem_t*) &NULL_MENU;


const MenuItem_t* menuGetCurrent(void) {
    return CurrentMenuItem;
}

const MenuItem_t* menuGetNext(void) {
    return NextMenuItem;
}

void menuSetNext(const MenuItem_t* next) {
    NextMenuItem = next;
}

void menuNavigate(const MenuItem_t* NewMenu) {
    if ((NewMenu == &NULL_MENU) || (NewMenu == NULL))
        return;

    CurrentMenuItem = NewMenu;
        if(CurrentMenuItem->dialogHandler) {
            CurrentMenuItem->dialogHandler(7);
        }
        menuUpdated = 1;
}


#endif	/* MICROMENU_H */

