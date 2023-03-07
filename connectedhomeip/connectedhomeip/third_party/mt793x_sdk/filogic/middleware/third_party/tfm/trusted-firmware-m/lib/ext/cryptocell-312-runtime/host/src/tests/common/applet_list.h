/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* \file applet_list.h
 * Definition of building list of applets for applet to applet tests
 */

#ifndef __APPLET_LIST_H__
#define __APPLET_LIST_H__

#include <stdbool.h>
#include "cc_pal_types.h"
#include "cc_pal_types_plat.h"

#define IN_PARAM
#define OUT_PARAM
#define IN_OUT_PARAM
#define APP_IS_LOADED 1

enum {
     AppSuccess,
     AppFail
};

#define GET_NEXT_SLOT(currentSlot, nextSlot, loadedAppsList) {\
    do {\
        if (IS_SRAM_SLOT(currentSlot)) {\
               nextSlot = rand() % (1 + MAX_PRIVILEGED_APPLET_ID);\
        } else {\
               nextSlot = rand() % MAX_APPLET_SLOT_NUM;\
        }\
        if (APP_IS_LOADED == loadedAppsList[nextSlot]) break;\
    } while (1);\
}


// if sram slot is in the list no-priv slot mustn't follow
#define IS_NEXT_SLOT_LEGAL(isSramInList, slot) ((IS_NON_PRIV_SLOT(slot) && (isSramInList))?0:1)

// if last slot in the list, it must be priviledge , if not last in list it's legal
#define IS_LAST_PRIV_SLOT(slot, index, numSlots) ((index == numSlots-2)?IS_PRIV_SLOT(slot):1)

// if last slot in the list is SARM
#define IS_NOT_LAST_SRAM_SLOT(slot, index, numSlots) ((index == numSlots-2)?(!(IS_SRAM_SLOT(slot))):1)


extern void appList_buildList(OUT_PARAM int8_t *appList,
            IN_PARAM  uint8_t *loadedAppsList,
            IN_PARAM  uint8_t appletListNum,
            IN_PARAM  uint8_t currentSlot,
            IN_PARAM  bool  isLastPriv);

extern  uint8_t getLoadedApplets(IN_PARAM int fd , OUT_PARAM uint8_t *pBuff);
extern  uint8_t clearAllApplets(IN_PARAM int fd);
extern  uint8_t loadApplet(IN_PARAM int fd , uint8_t slotId);
extern  uint8_t unloadApplet(IN_PARAM int fd , OUT_PARAM uint8_t *pBuff,uint8_t slotId);

#endif // __APPLET_LIST_H__
