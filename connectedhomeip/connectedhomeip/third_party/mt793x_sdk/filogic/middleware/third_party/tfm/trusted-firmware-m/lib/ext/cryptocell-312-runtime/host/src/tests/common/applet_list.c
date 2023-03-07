/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "applet_list.h"
#include "sep_test_agent.h"

/*
* @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
uint8_t getLoadedApplets(IN_PARAM int fd , OUT_PARAM uint8_t *pBuff)
{

    uint32_t      mem_shared_adr = 0;
    uint32_t      max_polling = MAX_POLLING_COUNT;
    uint8_t       stat= 0;
    uint8_t       lock_state = APP_BUFF_LOCKED;
    /*mmap RAM for shared buffer status loaded/unloaded applets*/
    mem_shared_adr = (unsigned int)mmap(0, APP_LOADING_SHARED_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, APP_LOADING_START_SHARED_ADDR);
    if (mem_shared_adr == 0 || mem_shared_adr == 0xFFFFFFFF) {
        fprintf(stderr, "Error:setAppletLoadState: mem mmap failed (%s)\n", strerror(errno));
        return 1;
    }
    TRY_ACQUIRE_LOCK(mem_shared_adr,stat,max_polling,lock_state);
    if (stat) {
        return 1; /* failed to acquire lock */
    }
    memcpy(pBuff,(uint8_t*)mem_shared_adr,MAX_APPLET_SLOT_NUM);
    RELEASE_LOCK(mem_shared_adr);
    return 0;
}
/*
* @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
uint8_t clearAllApplets(IN_PARAM int fd)
{

    uint32_t      mem_shared_adr = 0;
    uint32_t      max_polling = MAX_POLLING_COUNT;
    uint8_t       stat= 0;
    uint8_t       lock_state = APP_BUFF_LOCKED;
    /*mmap RAM for shared buffer status loaded/unloaded applets*/
    mem_shared_adr = (unsigned int)mmap(0, APP_LOADING_SHARED_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, APP_LOADING_START_SHARED_ADDR);
    if (mem_shared_adr == 0 || mem_shared_adr == 0xFFFFFFFF) {
        fprintf(stderr, "Error:clearAllApplets: mem mmap failed (%s)\n", strerror(errno));
        return 1;
    }
    TRY_ACQUIRE_LOCK((uint8_t *)mem_shared_adr,stat,max_polling,lock_state);
    if (stat) {
        return 1; /* failed to acquire lock */
    }
    memset((uint8_t*)mem_shared_adr,APP_UNLOADED, MAX_APPLET_SLOT_NUM);
    RELEASE_LOCK(mem_shared_adr);
    return 0;
}
/*
* @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
uint8_t loadApplet(IN_PARAM int fd , uint8_t slotId)
{

    uint32_t      mem_shared_adr = 0;
    uint32_t      max_polling = MAX_POLLING_COUNT;
    uint8_t       stat= 0;
    uint8_t       lock_state = APP_BUFF_LOCKED;

    mem_shared_adr = (unsigned int)mmap(0, APP_LOADING_SHARED_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, APP_LOADING_START_SHARED_ADDR);
    if (mem_shared_adr == 0 || mem_shared_adr == 0xFFFFFFFF) {
        fprintf(stderr, "Error:loadApplet: mem mmap failed (%s)\n", strerror(errno));
        return 1;
    }
    TRY_ACQUIRE_LOCK((uint8_t *)mem_shared_adr,stat,max_polling,lock_state);
    if (stat) {
        return 1; /* failed to acquire lock */
    }
    memset(((uint8_t *)mem_shared_adr)+slotId, APP_LOADED, 1);
    RELEASE_LOCK(mem_shared_adr);
    return 0;
}
/*
* @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
uint8_t unloadApplet(IN_PARAM int fd , OUT_PARAM uint8_t *pBuff,uint8_t slotId)
{

    uint32_t      mem_shared_adr = 0;
    uint32_t      max_polling = MAX_POLLING_COUNT;
    uint8_t       stat= 0;
    uint8_t       lock_state = APP_BUFF_LOCKED;

    mem_shared_adr = (unsigned int)mmap(0, APP_LOADING_SHARED_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, APP_LOADING_START_SHARED_ADDR);
    if (mem_shared_adr == 0 || mem_shared_adr == 0xFFFFFFFF) {
        fprintf(stderr, "Error:unloadApplet: mem mmap failed (%s)\n", strerror(errno));
        return 1;
    }
    TRY_ACQUIRE_LOCK((uint8_t *)mem_shared_adr,stat,max_polling,lock_state);
    if (stat) {
        return 1; /* failed to acquire lock */
    }
    memset(((uint8_t *)mem_shared_adr)+slotId, APP_UNLOADED, 1);
    RELEASE_LOCK(mem_shared_adr);
    return 0;
}
/*
* @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
void appList_buildList(OUT_PARAM int8_t *appList,
               IN_PARAM  uint8_t *loadedAppsList,
               IN_PARAM  uint8_t appletListNum,
               IN_PARAM  uint8_t currentSlot,
               IN_PARAM  bool  isLastPriv)
{
    uint8_t       slotsNum = 0;
    uint8_t       nextSlot = 0;
    bool        isSramInList = false;

    if ((NULL == appList) ||
        (NULL == loadedAppsList)) {
        return;
    }

    memset(appList, (-1), MAX_APPLET_SLOT_NUM);
    appList[NEXT_SLOT_INDEX] = 0;

    if ((appletListNum >= MAX_APPLET_SLOT_NUM) ||
        (appletListNum <= 1) ||
        (currentSlot >= MAX_APPLET_SLOT_NUM)) {
        return;
    }

    // Update sramFlag according to slot number
    isSramInList |= IS_SRAM_SLOT(currentSlot);
    while (slotsNum < (appletListNum-1)) {
        GET_NEXT_SLOT(currentSlot, nextSlot, loadedAppsList);
        // non-priv slot must not appear in the list after SRAM slot,
        // if last slot must be priv check that, otherwise just check last is not SRAM since SRAM can only forward command to other applet
        if (IS_NEXT_SLOT_LEGAL(isSramInList, nextSlot)) {
            if ((isLastPriv)) {
                if (IS_LAST_PRIV_SLOT(nextSlot, slotsNum, appletListNum)) {
                    isSramInList |= IS_SRAM_SLOT(currentSlot);
                    appList[slotsNum] = nextSlot;
                    slotsNum++;
                    currentSlot = nextSlot;
                }
            } else {
                if (IS_NOT_LAST_SRAM_SLOT(nextSlot, slotsNum, appletListNum)) {
                    isSramInList |= IS_SRAM_SLOT(currentSlot);
                    appList[slotsNum] = nextSlot;
                    slotsNum++;
                    currentSlot = nextSlot;
                }
            }
        }
    }

}
