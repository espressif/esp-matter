/******************************************************************************

 @file  hal_trng_wrapper.c

 @brief This file contains an API for returning a True Random
        Number Generator until one is provided elsewhere.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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

#include <inc/hw_types.h>
#include <inc/hw_sysctl.h>
#include "hal_trng_wrapper.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
#ifndef USE_FPGA
static uint32 lastTrngVal;
#endif // ! USE_FPGA

/*
** Software FIFO Application Programming Interface
*/

/*******************************************************************************
 * @fn          HalTRNG_InitTRNG
 *
 * @brief       This routine initializes the TRNG hardware.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HalTRNG_InitTRNG( void )
{
#ifndef USE_FPGA
  // configure TRNG
  // Note: Min=4x64, Max=1x256, ClkDiv=1+1 gives the same startup and refill
  //       time, and takes about 11us (~14us with overhead).
  TRNGConfigure( 256, 256, 0x01 );

  // enable TRNG
  TRNGEnable();

  // init variable to hold the last value read
  lastTrngVal = 0;
#endif // ! USE_FPGA

  return;
}


/*******************************************************************************
 * @fn          HalTRNG_WaitForReady
 *
 * @brief       This routine waits until the TRNG hardware is ready to be used.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HalTRNG_WaitForReady( void )
{
#ifndef USE_FPGA
  // poll status
  while(!(TRNGStatusGet() & TRNG_NUMBER_READY));
#endif // ! USE_FPGA

  return;
}


/*******************************************************************************
 * @fn          HalTRNG_GetTRNG
 *
 * @brief       This routine returns a 32 bit TRNG number.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      A 32 bit TRNG number.
 */
uint32 HalTRNG_GetTRNG( void )
{
#ifdef USE_FPGA
  return( 0xDEADBEEF );
#else
  uint32 trngVal;

  // initialize and enable TRNG if TRNG is not enabled
  if (0 == (HWREG(TRNG_BASE + TRNG_O_CTL) & TRNG_CTL_TRNG_EN))
  {
    HalTRNG_InitTRNG();
  }

  // check that a valid value is ready
  while(!(TRNGStatusGet() & TRNG_NUMBER_READY));

  // check to be sure we're not getting the same value repeatedly
  if ( (trngVal = TRNGNumberGet(TRNG_LOW_WORD)) == lastTrngVal )
  {
    return( 0xDEADBEEF );
  }
  else // value changed!
  {
    // so save last TRNG value
    lastTrngVal = trngVal;

    return( trngVal );
  }
#endif // USE_FPGA
}


/*******************************************************************************
 */


