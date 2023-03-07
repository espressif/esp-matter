/******************************************************************************

 @file  two_btn_menu.c

 @brief This file contains implementations of two-button menu module.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */
#include <ti/display/Display.h>
#include <bcomdef.h>
#include "two_btn_menu.h"

/******************************************************************************
 * DEFINITIONS
 */
/* Row to display Title at */
#define TBM_ROW_TITLE      0
/* Row to display 'Next item' at */
#define TBM_ROW_NEXT       1
/* First row to display items at */
#define TBM_ROW_ITEM_FIRST 2

/*********************************************************************
 * LOCAL VARIABLES
 */
static tbmMenuObj_t*  ptbmMenuObj = NULL;
static uint32          tbmItemCurrent = 0;
static pfnMenuSwitchCB_t pfn_tbmMenuSwitch;
#ifndef Display_DISABLE_ALL
static Display_Handle tbmDispHandle;
static const uint8 strUpper[] = "+Upper Menu";
#endif /* !Display_DISABLE_ALL */

/*********************************************************************
 * PUBLIC VARIABLES
 */
uint8 tbmRowItemLast;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
#ifndef Display_DISABLE_ALL
void tbm_displayItemPage(void);
void tbm_goTo(tbmMenuObj_t* pMenuObj);
#endif /* !Display_DISABLE_ALL */

/*********************************************************************
 * PUBLIC FUNCTIONS
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
                        uint8 numItemRow, pfnMenuSwitchCB_t pfnMenuSwitchCB)
{
  if (pMenuMain == NULL || hDisp == NULL || numItemRow == 0)
  {
    return false;
  }

  if (pMenuMain->attrib.numItem == 0)
  {
    return false;
  }

#ifndef Display_DISABLE_ALL
  tbmDispHandle = hDisp;
#endif /* !Display_DISABLE_ALL */
  tbmRowItemLast = TBM_ROW_ITEM_FIRST + numItemRow;
  pfn_tbmMenuSwitch = pfnMenuSwitchCB;

  /* Enter pMenuMain menu context */
  tbm_goTo(pMenuMain);

  return true;
}

#if defined(TBM_ACTIVE_ITEMS_ONLY)
/* Find the first active item from the current position */
uint32 tbm_findActiveItem(uint32 itemStart)
{
  uint32 itemCurrent = itemStart; /* Save current focus */

  /* Bypass disabled items */
  while (!TBM_IS_ITEM_ACTIVE(ptbmMenuObj, itemCurrent) &&
         itemCurrent < ptbmMenuObj->attrib.numItem)
  {
    itemCurrent++;
  }

  if (itemCurrent == ptbmMenuObj->attrib.numItem)
  {
    if (ptbmMenuObj->pUpper == NULL)
    {
      itemCurrent = TBM_NO_ITEM;
    }
  }
  else if (itemCurrent > ptbmMenuObj->attrib.numItem)
  {
    itemCurrent = TBM_NO_ITEM;
  }

  return itemCurrent;
}
#endif /* TBM_ACTIVE_ITEMS_ONLY */

/* Get the number of items to display including upper menu in the current menu */
uint32 tbm_getNumDisplayItem(void)
{
  uint32  numDisplayItem = 0;
#if defined(TBM_ACTIVE_ITEMS_ONLY)
  uint32  numItem = ptbmMenuObj->attrib.numItem;
  uint32 bActive = ptbmMenuObj->attrib.bActive;
#endif /* TBM_ACTIVE_ITEMS_ONLY */

  if (ptbmMenuObj->pUpper != NULL)
  {
    numDisplayItem++;
  }

#if defined(TBM_ACTIVE_ITEMS_ONLY)
  do
  {
    numDisplayItem += (bActive & 0x01);
    bActive >>= 1;
  } while (--numItem);
#else /* !TBM_ACTIVE_ITEMS_ONLY */
  numDisplayItem += ptbmMenuObj->attrib.numItem;
#endif /* TBM_ACTIVE_ITEMS_ONLY */

  return numDisplayItem;
}

/* Go to a menu */
void tbm_goTo(tbmMenuObj_t* pMenuObj)
{
  /* If a callback is registered, call it */
  if (pfn_tbmMenuSwitch)
  {
    pfn_tbmMenuSwitch(ptbmMenuObj, pMenuObj);
  }

  /* Go down into the submenu */
  ptbmMenuObj = pMenuObj;

#if defined(TBM_ACTIVE_ITEMS_ONLY)
  /* Put the focus on the first active item */
  tbmItemCurrent = tbm_findActiveItem(0);
#else /* !TBM_ACTIVE_ITEMS_ONLY */
  /* Put the focus on the first item */
  tbmItemCurrent = 0;
#endif /* TBM_ACTIVE_ITEMS_ONLY */

#ifndef Display_DISABLE_ALL
  /* Display the title prefixed with '*' */
  Display_printf(tbmDispHandle, TBM_ROW_TITLE, 0, "*%s", ptbmMenuObj->pTitle);

  tbm_displayItemPage();
#endif /* !Display_DISABLE_ALL */
}

/* Change individual item's status and refresh the display if necessary */
void tbm_setItemStatus(tbmMenuObj_t* pMenuObj, uint32 enables, uint32 disables)
{
  if (enables != TBM_ITEM_NONE)
  {
    pMenuObj->attrib.bActive |= enables;
  }

  if (disables != TBM_ITEM_NONE)
  {
    pMenuObj->attrib.bActive &= ~disables;
  }

  /* If current item, refresh the display */
  if (pMenuObj == ptbmMenuObj)
  {
    tbm_goTo(pMenuObj);
  }
}

/* Left Button Task */
bool tbm_buttonLeft(void)
{
  if (tbmItemCurrent == ptbmMenuObj->attrib.numItem)
  {
#if defined(TBM_ACTIVE_ITEMS_ONLY)
    /* If current item is "upper", find the first active item */
    tbmItemCurrent = tbm_findActiveItem(0);
#else /* !TBM_ACTIVE_ITEMS_ONLY */
    /* If current item is "upper", move focus onto the first item */
    tbmItemCurrent = 0;
#endif /* TBM_ACTIVE_ITEMS_ONLY */
  }
#if defined(TBM_ACTIVE_ITEMS_ONLY)
  else if (tbmItemCurrent != TBM_NO_ITEM)
  {
    uint32 itemTemp;

    /* If current item is not "upper", find the next active item */
    itemTemp = tbm_findActiveItem(tbmItemCurrent + 1);

    /* if there is no more active item, find one from the beginning */
    if (itemTemp == TBM_NO_ITEM)
    {
      itemTemp = tbm_findActiveItem(0);
    }

    tbmItemCurrent = itemTemp;
  }
#else /* !TBM_ACTIVE_ITEMS_ONLY */
  else
  {
    /* If current item is not "upper",
       move focus onto either the next item or the first item
       depending on existence of the "upper" */
    tbmItemCurrent++;
    if (ptbmMenuObj->pUpper == NULL &&
        tbmItemCurrent == ptbmMenuObj->attrib.numItem) {
      tbmItemCurrent = 0;
    }
  }
#endif /* TBM_ACTIVE_ITEMS_ONLY */

#ifndef Display_DISABLE_ALL
  tbm_displayItemPage();
#endif /* !Display_DISABLE_ALL */

  return true;
}

/* Right Button Task */
bool tbm_buttonRight(void)
{
  bool status = true;

  if (tbmItemCurrent == ptbmMenuObj->attrib.numItem)
  {
    /* We are now at "Back to Upper". Go there. */
    tbm_goTo(ptbmMenuObj->pUpper);
  }
  /* Focused item cannot be a disabled one */
#if defined(TBM_ACTIVE_ITEMS_ONLY)
  else if (tbmItemCurrent != TBM_NO_ITEM)
  {
#else /* !TBM_ACTIVE_ITEMS_ONLY */
  else if (TBM_IS_ITEM_ACTIVE(ptbmMenuObj, tbmItemCurrent))
  {
#endif /* TBM_ACTIVE_ITEMS_ONLY */
    /* Go down into the submenu or take the action only if it is active */
    if (TBM_IS_SUBMENU(ptbmMenuObj, tbmItemCurrent))
    {
      /* Go down into the submenu */
      tbm_goTo(ptbmMenuObj->itemEntry[tbmItemCurrent].item.pSubMenu);
    }
    else
    {
      /* Take the action */
      status = ptbmMenuObj->itemEntry[tbmItemCurrent].item.pfnAction(tbmItemCurrent);
    }
  }
#if !defined(TBM_ACTIVE_ITEMS_ONLY)
  else
  {
    status = false;
  }
#endif /* !TBM_ACTIVE_ITEMS_ONLY */

  return status;
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#ifndef Display_DISABLE_ALL
/* Display a single item */
void tbm_displayItem(uint32 itemIndex, uint8 row)
{
  uint8* pDesc;
  uint8* pPrefix = (void*)"";
  uint8* pPostfix =(void*)"";

  if (itemIndex == ptbmMenuObj->attrib.numItem)
  {
    /* If Upper Menu exists, the description will be "Back to Upper" */
    pDesc = (uint8*) strUpper;
  }
  else
  {
    if (TBM_IS_SUBMENU(ptbmMenuObj, itemIndex))
    {
      /* If this is a submenu, pull the title from the submenu object */
      pDesc = ptbmMenuObj->itemEntry[itemIndex].item.pSubMenu->pTitle;
    }
    else
    {
      /* If this is an action, pull the description from the item entry */
      pDesc = ptbmMenuObj->itemEntry[itemIndex].pDesc;
    }

  #if !defined(TBM_ACTIVE_ITEMS_ONLY)
    if (!TBM_IS_ITEM_ACTIVE(ptbmMenuObj, itemIndex))
    {
      /* If this is an inactive item, should be prefixed with 'x' */
      pPrefix = (uint8 *)"x";
    }
    else
  #endif /* TBM_ACTIVE_ITEMS_ONLY */
    {
      if (TBM_IS_SUBMENU(ptbmMenuObj, itemIndex))
      {
        /* If this is an active submenu, '+' should prefix the text. */
        pPrefix =(void*) "+";
      }
      else
      {
        /* If this is an active action, a space should prefix the text. */
        pPrefix = (void*)" ";
      }
    }
  }

  if (row == TBM_ROW_ITEM_FIRST)
  {
    /* If the current item, postfix with " >" */
    pPostfix = (void*)" >";
  }

  /* Display the description with the prefix and the postfix */
  Display_printf(tbmDispHandle, row, 1, "%s%s%s", pPrefix, pDesc, pPostfix);
}

/* Display one page of the item list */
void tbm_displayItemPage(void)
{
  uint8 i;
  uint32 itemToDisplay;
  #if !defined(TBM_ACTIVE_ITEMS_ONLY)
  uint32 itemEnd = tbm_getNumDisplayItem();
  #endif /* !TBM_ACTIVE_ITEMS_ONLY */

  /* Display "< Next Item" if there are more than 1 item */
  if (tbm_getNumDisplayItem() > 1)
  {
    Display_printf(tbmDispHandle, TBM_ROW_NEXT, 0, "< Next Item");
  }
  else
  {
    Display_clearLine(tbmDispHandle, TBM_ROW_NEXT);
  }

  /* Display from the first item row */
  i = TBM_ROW_ITEM_FIRST;

  #if defined(TBM_ACTIVE_ITEMS_ONLY)
  if (tbmItemCurrent != TBM_NO_ITEM)
  {
  #endif /* TBM_ACTIVE_ITEMS_ONLY */
    /* Display from the current item */
    itemToDisplay = tbmItemCurrent;

    /* Display items as many as possible in the designated area */
    do
    {
      tbm_displayItem(itemToDisplay, i++);
  #if defined(TBM_ACTIVE_ITEMS_ONLY)
      /* Find next active item */
      itemToDisplay = tbm_findActiveItem(itemToDisplay + 1);
    } while (itemToDisplay != TBM_NO_ITEM && i < tbmRowItemLast);
  #else /* !TBM_ACTIVE_ITEMS_ONLY */
      /* Find next item */
      itemToDisplay++;
    } while (itemToDisplay < itemEnd && i < tbmRowItemLast);
  #endif /* TBM_ACTIVE_ITEMS_ONLY */
  #if defined(TBM_ACTIVE_ITEMS_ONLY)
  }
  #endif /* TBM_ACTIVE_ITEMS_ONLY */

  /* Clear unused area */
  if (i < tbmRowItemLast)
  {
    Display_clearLines(tbmDispHandle, i, tbmRowItemLast - 1);
  }
}
#endif /* !Display_DISABLE_ALL */

