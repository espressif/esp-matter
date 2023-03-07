/******************************************************************************

 @file  bim_util.h

 @brief This file contains the oad user application
        definitions and prototypes.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

#ifndef BIM_UTIL_H
#define BIM_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/aon_event.h)
#include DeviceFamily_constructPath(driverlib/osc.h)
#include DeviceFamily_constructPath(driverlib/setup.h)
#include DeviceFamily_constructPath(driverlib/pwr_ctrl.h)

#include "common/cc26xx/oad/oad_image_header.h"

/*******************************************************************************
*  EXTERNAL VARIABLES
*/

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn          jumpToPrgEntry
 *
 * @brief       This function jumps the execution to program entry to execute
 *              application
 *
 * @param       prgEntry - address of application entry.
 *
 * @return      None.
 */
extern void jumpToPrgEntry(uint32_t prgEntry);

/*******************************************************************************
 * @fn          imgIDCheck
 *
 * @brief       This function check if the buffer contains OAD image
 *              identification value.
 *
 * @param       imgHdr - pointer to image identification header buffer.
 *
 * @return      None.
 */
extern bool imgIDCheck(imgFixedHdr_t *imgHdr);

/*******************************************************************************
 * @fn     setLowPowerMode
 *
 * @brief  This function sets the device to the lowest power mode, and does not
 *         return.
 */
extern void setLowPowerMode(void);

#ifndef BIM_ONCHIP
/*******************************************************************************
 * @fn          metadataIDCheck
 *
 * @brief       This function check if the buffer contains external flash OAD
 *              metadata identification value.
 *
 * @param       imgHdr - pointer to flash metadata header buffer.
 *
 * @return      None.
 */
extern bool metadataIDCheck(imgFixedHdr_t *imgHdr);


#ifdef BIM_ONCHIP
extern bool evenBitCount(uint32_t value);
#endif //#ifdef BIM_ONCHIP
#endif //#ifndef BIM_ONCHIP


#ifdef BIM_ONCHIP
/*******************************************************************************
 * @fn     evenBitCount
 *
 * @brief  Finds if the value field has even number of bits set to 1's.
 *
 * @param  value  - input field on which to calculate the even number of 1's.
 *
 * @return True when the value field has even number of 1's, otherwise returns
 *         False.
 */
extern bool evenBitCount(uint32_t value);
#endif //#ifdef BIM_ONCHIP
/*******************************************************************************
*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* BIM_UTIL_H */
