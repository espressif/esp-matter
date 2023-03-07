/******************************************************************************

 @file  two_btn_menu.h

 @brief This file contains macros, type definitions, and function prototypes
        for two-button menu implementation.

 Group: WCS BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef TWO_BTN_MENU_H
#define TWO_BTN_MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <hal_types.h>
#include <ti/display/Display.h>
#include <icall.h>

/******************************************************************************
 * DEFINITIONS
 */

/* First row for the application messages. Note that application SHOULD NOT
   use the rows before TBM_ROW_APP */
#define TBM_ROW_APP        tbmRowItemLast

/*
 * Initialization macros for a menu object consisting of item entries
 */

/* Beginning of the menu object declaration and initialization
     W: This menu object
     X: Title to display
     Y: # of item entries. Maximum # of items is 15.
     Z: Pointer to upper menu object.
        If none or not desired, NULL should be given.
*/
#define MENU_OBJ(W, X, Y, Z) { \
W.pTitle=(uint8*)X;\
W.attrib.numItem=Y;\
W.attrib.bActive=TBM_ITEM_ALL; W.pUpper=Z; \
W.itemEntry = ICall_malloc(Y * sizeof(tbmItemEntry_t));}

/* Submenu item entry.
     W: This menu object
     N: Serial number of object
     X: Pointer to submenu object
*/
#define MENU_ITEM_SUBMENU(W, N, X) {if (W.itemEntry){W.itemEntry[N].pDesc=NULL; W.itemEntry[N].item.pSubMenu=(X);}}

/* Action item entry.
     W: This menu object
     N: Serial number of object
     X: Action description.
     Y: Pointer to action function
*/
#define MENU_ITEM_ACTION(W, N, X, Y) {if (W.itemEntry){W.itemEntry[N].pDesc=(uint8*)(X); W.itemEntry[N].item.pfnAction=(Y);}}

/* Multiple Actions items entry.
     W: This menu object
     N: number of actions to add.
     X: Action description.
     Y: Pointer to action function
*/
#define MENU_ITEM_MULTIPLE_ACTIONS(W, N, X, Y) {uint8 i; for(i=0; i<N; i++){MENU_ITEM_ACTION(W, i, X, Y);}}

/* End of the menu object declaration and initialization */
#define MENU_OBJ_END

/*
 * Menu item setting macros for already existing menu object instances
 */

/* Set the title of a menu object.
     X: Pointer to menu object
     Y: Pointer to the title string.
*/
#define TBM_SET_TITLE(X, Y) (X)->pTitle=((uint8 *)Y)

/* Set the number of items in a menu object.
     X: Pointer to menu object
     Y: Number of items
*/
#define TBM_SET_NUM_ITEM(X, Y) (X)->attrib.numItem=(Y)

/* Set the object of a submenu item.
     X: Pointer to menu object
     Y: Item index
     Z: Pointer to the submenu object.
*/
#define TBM_SET_SUBMENU_OBJ(X, Y, Z) (X)->itemEntry[Y].item.pSubMenu=(Z)

/* Set an action description. Only applicable to an action item.
     X: Pointer to menu object
     Y: Item index
     Z: Pointer to the action description string.
*/
#define TBM_SET_ACTION_DESC(X, Y, Z) (X)->itemEntry[Y].pDesc=Z

/* Set an action function. Only applicable to an action item.
     X: Pointer to menu object
     Y: Item index
     Z: Function pointer of the action.
*/
#define TBM_SET_ACTION_FUNC(X, Y, Z) (X)->itemEntry[Y].item.pfnAction=(Z)

/* Set both the description and the function of an action item.
     W: Pointer to menu object
     X: Item index
     Y: Item description
     Z: Pointer to the action description string.
*/
#define TBM_SET_ACTION_ITEM(W, X, Y, Z) TBM_SET_ACTION_DESC(W, X, Y); \
TBM_SET_ACTION_FUNC(W, X, Z)

/* Get the number of items in a menu object.
     X: Pointer to menu object
*/
#define TBM_GET_NUM_ITEM(X) ((X)->attrib.numItem)

/* Get the pointer to a submenu item.
     X: Pointer to menu object
     Y: Index of the submenu item
*/
#define TBM_GET_SUBMENU_OBJ(X, Y) ((X)->itemEntry[Y].item.pSubmenu)

/* Get the pointer to an action description.
     X: Pointer to menu object
     Y: Index of the action item
*/
#define TBM_GET_ACTION_DESC(X, Y) ((X)->itemEntry[Y].pDesc)

/* Get the pointer to an action function.
     X: Pointer to menu object
     Y: Index of the action item
*/
#define TBM_GET_ACTION_FUNC(X, Y) ((X)->itemEntry[Y].item.pfnAction)

/* Check if the item is a submenu item
     X: Pointer to menu object
     Y: Index of the item
*/
#define TBM_IS_SUBMENU(X, Y) ((X)->itemEntry[Y].pDesc == NULL)

/* Check if the item is an action item
     X: Pointer to menu object
     Y: Index of the item
*/
#define TBM_IS_ACTION(X, Y) ((X)->itemEntry[Y].pDesc != NULL)

/* Check if a menu item is active.
     X: Pointer to menu object
     Y: Index of the item
*/
#define TBM_IS_ITEM_ACTIVE(X, Y) (((X)->attrib.bActive & (1 << (Y))) != 0)

/* Indicates no more active item from there */
#define TBM_NO_ITEM      0xFFFFFFFF

/* Item bitmap for enabling/disabling */
#define TBM_ITEM(X)      (1 << (X))
#define TBM_ITEM_ALL     0xFFFFFFFF
#define TBM_ITEM_NONE    0     /* Used when nothing is selected */

/******************************************************************************
 * TYPEDEFS
 */

/* Type definition for final action functions */
typedef bool (*pfnAction_t)(uint8 index);

typedef struct tbmMenuObj_s tbmMenuObj_t;

/* Type definition for a submenu/action item entry */
typedef struct {
  uint8* pDesc;                 /* action description. NULL for submenu */
  union {
    tbmMenuObj_t* pSubMenu;     /* submenu */
    pfnAction_t   pfnAction;    /* function for action */
  } item;
} tbmItemEntry_t;

/* Type definition for a menu object */
struct tbmMenuObj_s {
  uint8* pTitle;                /* Title of this menu */
  struct {
	uint32 bActive ;         /* status bitmap of items */
	uint32 numItem ;         /* # of item entries */
  } attrib;
  tbmMenuObj_t*  pUpper;        /* upper menu */
  tbmItemEntry_t *itemEntry;   /* item entries */
};

/* Type definition for menu switching notificaiton callback */
typedef void (*pfnMenuSwitchCB_t)(tbmMenuObj_t* pMenuObjCurr,
                                  tbmMenuObj_t* pMenuObjNext);

/******************************************************************************
 * EXTERNAL VARIABLES
 */

extern uint8 tbmRowItemLast;

/******************************************************************************
 * API FUNCTIONS
 */

/**
 * @fn     tbm_initTwoBtnMenu
 * @brief  Initialize Two-Button Menu module.
 *
 * @param   hDisp           - TIRTOS Display Driver handle. Should have been
 *                            opened by the time this function is called.
 * @param   pMenuMain       - Pointer to Main Menu object
 * @param   numItemRow      - Max # of rows to display items in a menu object
 * @param   pfnMenuSwitchCB - Callback to be notified that the menu is switching
 *                            to another. If nothing needs to be specially done
 *                            upon menu switching, the application can put NULL.
 *
 * @return  true if successful, false otherwise
 */
bool tbm_initTwoBtnMenu(Display_Handle hDisp, tbmMenuObj_t* pMenuMain,
                        uint8 numItemRow, pfnMenuSwitchCB_t pfnMenuSwitchCB);

/**
 * @fn     tbm_setItemStatus
 * @brief  Enable or disable individual items and refresh the display
 *         if necessary
 *
 * @param   pMenuObj - Pointer to the new menu object whose items' status
 *                     to be set.
 * @param   enables  - Bitmap of the items to be enabled. If no item to enable,
 *                     this shall be TBM_ITEM_NONE.
 * @param   disables - Bitmap of the items to be disabled. If no item to disable,
 *                     this shall be TBM_ITEM_NONE.
 * @return  None
 */
void tbm_setItemStatus(tbmMenuObj_t* pMenuObj, uint32 enables, uint32 disables);

/**
 * @fn     tbm_goTo
 * @brief  Switch to a certain menu
 *
 * @param   pMenuObj - Pointer to the new menu object to switch to
 *
 * @return  None
 */
void tbm_goTo(tbmMenuObj_t* pMenuObj);

/**
 * @fn     tbm_buttonLeft
 * @brief  Perform a task supposed to be done when the left button is pressed.
 *         "Move to the next item" will be done in this implementation.
 *
 * @param   None
 *
 * @return  None
 */
bool tbm_buttonLeft(void);

/**
 * @fn     tbm_buttonRight
 * @brief  Perform a task supposed to be done when the right button is pressed.
 *         "Select current item" will be done in this implementation.
 *
 * @param   None
 *
 * @return  None
 */
bool tbm_buttonRight(void);

#ifdef __cplusplus
}
#endif

#endif /* TWO_BTN_MENU_H */

