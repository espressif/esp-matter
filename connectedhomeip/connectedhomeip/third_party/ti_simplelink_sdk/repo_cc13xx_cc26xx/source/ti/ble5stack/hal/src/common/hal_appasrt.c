/******************************************************************************

 @file  hal_appasrt.c

 @brief This module contains the application assert functions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */

#include "hal_appasrt.h"

/*******************************************************************************
 * LOCAL VARIABLES
 */

/* Pointers to App Assert Handler function */
static APPASRTINTF_assrtHndlerFunc_t *pAssrtFunc = NULL;

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @brief   See header file for details.
 */
uint8_t HAL_APPASRT_register(APPASRTINTF_assrtHndlerFunc_t *pFptr)
{
  uint8_t status;

  //! Store parameters
  pAssrtFunc = pFptr;

  //! Make sure we have valid function pointers
  if ((pAssrtFunc != NULL) && (pAssrtFunc->callAssrtHndler))
  {
    status = APPASRTINTF_SUCCESS;
  }
  else
  {
    status = APPASRTINTF_FAILURE;
  }

  return status;
}

/*******************************************************************************
 * @brief   See header file for details.
 */
void HAL_APPASRT_callAssrtHanlder(void)
{
  pAssrtFunc->callAssrtHndler();
}

/*******************************************************************************
*******************************************************************************/
