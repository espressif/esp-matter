/******************************************************************************

 @file  TRNGCC26XX.c

 @brief This file contains an API for returning a True Random
        Number Generator Driver for CC26xx.

 Group: WCS, LPC, BTS
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

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <driverlib/trng.h>
#include <TRNGCC26XX.h>

/*******************************************************************************
 * CONSTANTS
 */

// These are the default values for TRNG when no parameters are specified.
// Note: These values shall not be changed.  The BLE Stack relies on these defaults.
#define TRNGCC26XX_MIN_SAMPLES_DEFAULT        256
#define TRNGCC26XX_MAX_SAMPLES_DEFAULT        256
#define TRNGCC26XX_SAMPLE_RATE_DEFAULT        1

#define TRNGCC26XXX_PERIPHERAL_0_INDEX        0

// Internal state constants
#define TRNGCC26XX_CLOSED 0
#define TRNGCC26XX_OPEN   1

// Internal initialization state
#define TRNGCC26XX_IS_NOT_INITIALIZED 0
#define TRNGCC26XX_IS_INITIALIZED     1

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * EXTERNAL VARIABLES
 */
extern const TRNGCC26XX_Config TRNGCC26XX_config[];

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

void closeTRNG(TRNGCC26XX_Handle handle);
void openTRNG(TRNGCC26XX_Handle handle);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== TRNGCC26XX_init ========
 */
void TRNGCC26XX_init(void)
{
  static uint8_t isInit = TRNGCC26XX_IS_NOT_INITIALIZED;
  uint16_t hwiKey;

  // Disable hardware interrupts.
  hwiKey  = (uint16_t) Hwi_disable();

  if (isInit == TRNGCC26XX_IS_NOT_INITIALIZED)
  {
    // Intialize internal state of TRNG Peripherals to closed.
    ((TRNGCC26XX_Object *)(TRNGCC26XX_config[TRNGCC26XXX_PERIPHERAL_0_INDEX].object))->state = TRNGCC26XX_CLOSED;

    isInit = TRNGCC26XX_IS_INITIALIZED;
  }

  // Enable hardware interrupts.
  Hwi_restore(hwiKey);
}

/*
 *  ======== TRNGCC26XX_open ========
 */
TRNGCC26XX_Handle TRNGCC26XX_open(uint8_t index)
{
  TRNGCC26XX_Handle handle;
  uint16_t hwiKey;

  // Get handle
  handle  = (TRNGCC26XX_Handle)&(TRNGCC26XX_config[TRNGCC26XXX_PERIPHERAL_0_INDEX]);

  // Disable hardware interrupts.
  hwiKey  = (uint16_t) Hwi_disable();

  // Open TRNG.
  openTRNG((TRNGCC26XX_Handle)(&(TRNGCC26XX_config[TRNGCC26XXX_PERIPHERAL_0_INDEX])));

  // Enable hardware interrupts.
  Hwi_restore(hwiKey);

  return (handle);
}

/*
 *  ======== TRNGCC26XX_close ========
 */
void TRNGCC26XX_close(TRNGCC26XX_Handle handle)
{
  uint16_t hwiKey;

  // Disable hardware interrupts.
  hwiKey = (uint16_t) Hwi_disable();

  // Close TRNG.
  closeTRNG(handle);

  // Enable hardware interrupts.
  Hwi_restore(hwiKey);
}

/*
 *  ======== TRNGCC26XX_Params_init ========
 */
int8_t TRNGCC26XX_Params_init(TRNGCC26XX_Params *params)
{
  int8_t status = TRNGCC26XX_STATUS_ILLEGAL_PARAM;

  if (params)
  {
    // Default values for samples per each generated random number.
    params->minSamplesPerCycle = TRNGCC26XX_MIN_SAMPLES_DEFAULT;
    params->maxSamplesPerCycle = TRNGCC26XX_MAX_SAMPLES_DEFAULT;

    // Default value for number of clock cycles per sample.
    params->clocksPerSample = TRNGCC26XX_SAMPLE_RATE_DEFAULT;

    status = TRNGCC26XX_STATUS_SUCCESS;
  }

  return (status);
}

/*
 *  ======== TRNGCC26XX_getTRNG ========
 */
uint32_t TRNGCC26XX_getNumber(TRNGCC26XX_Handle handle, TRNGCC26XX_Params *params, int8_t *status)
{
  uint16_t hwiKey;
  uint32_t trngVal;

  // Disable hardware interrupts.
  hwiKey = (uint16_t) Hwi_disable();

  // Check if driver is not open.
  if (((TRNGCC26XX_Object *)TRNGCC26XX_config[TRNGCC26XXX_PERIPHERAL_0_INDEX].object)->state == TRNGCC26XX_CLOSED)
  {
    // Open driver.
    openTRNG((TRNGCC26XX_Handle)(&(TRNGCC26XX_config[TRNGCC26XXX_PERIPHERAL_0_INDEX])));
  }

  // Check for params and that they are configured legally
  if (!params)
  {
    // Configure TRNG.  This will disable TRNG.
    TRNGConfigure(TRNGCC26XX_MIN_SAMPLES_DEFAULT,
                  TRNGCC26XX_MAX_SAMPLES_DEFAULT,
                  TRNGCC26XX_SAMPLE_RATE_DEFAULT);
  }
  else if (TRNGCC26XX_isParamValid(params))
  {
    if (status)
    {
      *status = TRNGCC26XX_STATUS_ILLEGAL_PARAM;
    }

    // Release Power dependency on TRNG.
    Power_releaseDependency(PowerCC26XX_PERIPH_TRNG);

    // Enable hardware interrupts.
    Hwi_restore(hwiKey);

    return (TRNGCC26XX_ILLEGAL_PARAM_RETURN_VALUE);
  }
  else
  {
    // Configure TRNG.  This will disable TRNG.
    TRNGConfigure(params->minSamplesPerCycle,
                  params->maxSamplesPerCycle,
                  params->clocksPerSample);
  }

  // Enable TRNG.
  TRNGEnable();

  // Poll until a number is ready.
  while(!(TRNGStatusGet() & TRNG_NUMBER_READY));

  // Get number.
  trngVal = TRNGNumberGet(TRNG_LOW_WORD);

  // Set success into status flag.
  if (status)
  {
    *status = TRNGCC26XX_STATUS_SUCCESS;
  }

  // Enable hardware interrupts.
  Hwi_restore(hwiKey);

  return (trngVal);
}

 /*
  *  ======== TRNGCC26XX_isParamValid ========
  */
int8_t TRNGCC26XX_isParamValid(TRNGCC26XX_Params *params)
{
  int8_t status = TRNGCC26XX_STATUS_ILLEGAL_PARAM;

  // Verify parameters are legal.
  if (params                                                        &&
      (params->minSamplesPerCycle == 0                              ||
       (params->minSamplesPerCycle >= TRNGCC26XX_MIN_SAMPLES_MIN    &&
        params->minSamplesPerCycle <  TRNGCC26XX_MIN_SAMPLES_MAX))  &&
      (params->maxSamplesPerCycle == 0                              ||
       (params->maxSamplesPerCycle >= TRNGCC26XX_MAX_SAMPLES_MIN    &&
        params->maxSamplesPerCycle <  TRNGCC26XX_MAX_SAMPLES_MAX))  &&
      params->clocksPerSample     <= TRNGCC26XX_CLOCKS_PER_SAMPLES_MAX)
  {
    status = TRNGCC26XX_STATUS_SUCCESS;
  }

  return (status);
}

/*
 * Open the TRNG peripheral.
 */
void openTRNG(TRNGCC26XX_Handle handle)
{
    if (((TRNGCC26XX_Object *)((TRNGCC26XX_Config *)handle)->object)->state == TRNGCC26XX_CLOSED)
    {
      ((TRNGCC26XX_Object *)((TRNGCC26XX_Config *)handle)->object)->state = TRNGCC26XX_OPEN;

      // Set dependency
      Power_setDependency(((TRNGCC26XX_HWAttrs *)((TRNGCC26XX_Config *)handle)->hwAttrs)->powerMngrId);
    }
}

/*
 * Close the TRNG peripheral.
 */
void closeTRNG(TRNGCC26XX_Handle handle)
{
  if (handle && ((TRNGCC26XX_Object *)((TRNGCC26XX_Config *)handle)->object)->state == TRNGCC26XX_OPEN)
  {
    ((TRNGCC26XX_Object *)((TRNGCC26XX_Config *)handle)->object)->state = TRNGCC26XX_CLOSED;

    // Release Power dependency on TRNG.
    Power_releaseDependency(PowerCC26XX_PERIPH_TRNG);
  }
}

/*******************************************************************************
 */
