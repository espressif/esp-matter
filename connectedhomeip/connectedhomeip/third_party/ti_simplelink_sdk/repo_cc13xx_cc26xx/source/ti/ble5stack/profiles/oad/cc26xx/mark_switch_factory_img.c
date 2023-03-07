/******************************************************************************

 @file  mark_switch_factory_img.c

 @brief This file marks own image header to switch to factory image.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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

#include <driverlib/flash.h>
#include <common/cc26xx/oad/oad_image_header.h>
#include "onboard.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */
// The current image's header is initialized in oad_image_header_app.c
extern const imgHdr_t _imgHdr;
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn     markSwitchFactoryImg
 *
 * @brief   This function invalidate the the CRC status of it's own image 
 *          header to mark it to switch to factory image
 *
 * @return  None 
 */
void markSwitchFactoryImg(void)
{
    /* Read image header */
    uint32_t imgStartAddr = _imgHdr.imgPayload.startAddr;

    uint8_t crcStat = CRC_INVALID;
    
    uint8_t state = VIMSModeGet(VIMS_BASE);

    // Check VIMS state
    if (state != VIMS_MODE_DISABLED)
    {
      // Invalidate cache
      VIMSModeSet(VIMS_BASE, VIMS_MODE_DISABLED);

      // Wait for disabling to be complete
      while (VIMSModeGet(VIMS_BASE) != VIMS_MODE_DISABLED);
    }

 
    FlashProgram(&crcStat, (imgStartAddr + CRC_STAT_OFFSET),
                                     sizeof(_imgHdr.fixedHdr.crcStat));
    SystemReset();

    return;
}


/*********************************************************************
*********************************************************************/
