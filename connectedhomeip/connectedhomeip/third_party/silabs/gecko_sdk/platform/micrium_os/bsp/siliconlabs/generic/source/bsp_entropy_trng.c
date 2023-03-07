/***************************************************************************//**
 * @file
 * @brief BSP Random Number Generator Module - TRNG port
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if defined(RTOS_MODULE_ENTROPY_AVAIL)

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"

#include  <cpu/include/cpu.h>

#include <entropy/include/entropy.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  TRNG_TIMEOUT    0x10000

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void       BSP_TRNG_Open(void);
static void       BSP_TRNG_Close(void);
static CPU_INT32U BSP_TRNG_Generate(CPU_INT08U *buf, CPU_INT32U len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                RANDOM NUMBER GENERATOR BSP INTERFACE
 *
 * Note(s) : (1) Board-support package (BSP) interface structures are used by the controller driver
 *               to call specific controller' BSP functions via function pointer instead of by name. This
 *               enables the Entropy module to compile & operate with multiple instances of
 *               multiple controllers & drivers.
 *******************************************************************************************************/

const ENTROPY_RNG_BSP_API BSP_RNG0_BSP_API = {
  .Open     = BSP_TRNG_Open,
  .Close    = BSP_TRNG_Close,
  .Generate = BSP_TRNG_Generate,
};

const ENTROPY_RNG_DRV_INFO BSP_RNG0_DrvInfo = {
  .BSP_API_Ptr = &BSP_RNG0_BSP_API,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                            BSP_TRNG_Open()
 *
 * @brief    Initializes BSP random number generator module.
 *******************************************************************************************************/
static void BSP_TRNG_Open( void )
{
    CPU_INT32U i    = 0;
    CPU_INT32U ctrl = 0;

    /* Enable the TRNG0 clock. */
    CMU_ClockEnable( cmuClock_TRNG0, true );

    /* enable TRNG0. */
    TRNG0->CONTROL =
          TRNG_CONTROL_ENABLE
        | TRNG_CONTROL_CONDBYPASS
        | TRNG_CONTROL_FORCERUN
        | TRNG_CONTROL_REPCOUNTIEN
        | TRNG_CONTROL_APT64IEN
        | TRNG_CONTROL_APT4096IEN
        | TRNG_CONTROL_PREIEN
        | TRNG_CONTROL_ALMIEN;

    /* store control word */
    ctrl = TRNG0->CONTROL;

    /* enable soft reset */
    ctrl |= TRNG_CONTROL_SOFTRESET;

    /* restore control word (with soft reset enabled) */
    TRNG0->CONTROL = ctrl;

    /* disable soft reset */
    ctrl &= ~TRNG_CONTROL_SOFTRESET;

    /* restore control word (with soft reset disabled) */
    TRNG0->CONTROL = ctrl;

    /* wait until there is something in FIFO. */
    while (i < TRNG_TIMEOUT && TRNG0->FIFOLEVEL == 0) {
      i++;
    }
}

/*
 ********************************************************************************************************
 *                                           BSP_TRNG_Close()
 *
 * @brief    Stops down BSP RNG module.
 *******************************************************************************************************/
static void BSP_TRNG_Close( void )
{
    /* Disable TRNG0. */
    TRNG0->CONTROL = 0;

    /* Disable the TRNG0 clock. */
    CMU_ClockEnable( cmuClock_TRNG0, false );
}

/*
 ********************************************************************************************************
 *                                          BSP_TRNG_Generate()
 *
 * @brief    Retrieve random numbers and store them in buffer buf.
 *******************************************************************************************************/
static CPU_INT32U BSP_TRNG_Generate( CPU_INT08U *buf, CPU_INT32U len )
{
    CPU_INT32U val32  = 0;
    CPU_INT32U status = 0;
    CPU_INT32U rem    = len;

    /* Loop while data is available in the FIFO and output buffer is not full. */
    while ( (rem > 0) && (TRNG0->FIFOLEVEL != 0) && (status == 0) )
    {
        /* read data */
        val32 = TRNG0->FIFO;

        /* copy read data */
        switch (rem) {
            case 1:
            case 3:
              /* 8-bit */
              *((CPU_INT08U *) buf) += (CPU_INT08U) (val32 & 0xFF);
              rem -= 1;
              buf += 1;
              break;

            case 2:
              /* 16-bit */
              *((CPU_INT16U *) buf) += (CPU_INT16U) (val32 & 0xFFFF);
              rem -= 2;
              buf += 2;
              break;

            default:
              /* 32-bit */
              *((CPU_INT32U *) buf) += (CPU_INT32U) (val32);
              rem -= 4;
              buf += 4;
              break;
        }

        /* check for errors */
        status = TRNG0->STATUS;
    }

    /* done */
    return len - rem;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of RTOS_MODULE_ENTROPY_AVAIL
