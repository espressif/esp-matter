/******************************************************************************

 @file  hall_profile.h

 This file contains the Hall profile definitions and prototypes.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

#ifndef HALLPROFILE_H
#define HALLPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "sensor_common.h"

/*********************************************************************
 * FUNCTIONS
 */

/**
 * Initialize the module
 *
 * Register with common sensors module, add the Hall GATT service,
 * and initialize the periodic Hall sensor readings
 *
 * @return bleMemAllocError not enough heap to continue
 * @return SUCCESS
 * @return bleAlreadyInRequestedMode Already initialized
 */
extern bStatus_t HallProfile_init();

/**
 * Enable the Hall Profile.
 *
 * This will start the periodic hall monitor readings
 *
 * @return SUCCESS Periodic readings started.
 * @return FAILURE ADC failed to open
 */
extern bStatus_t HallProfile_enable();

/**
 * Disable the Hall Profile.
 *
 * This will stop the periodic hall sensor readings
 *
 * @return SUCCESS
 */
extern bStatus_t HallProfile_disable();

#ifdef __cplusplus
}
#endif

#endif /* HALLPROFILE_H */
