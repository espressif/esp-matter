/******************************************************************************

 @file main.c

 @brief main entry of the example application

 Group: CMCU, LPC
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

/******************************************************************************
 Includes
 *****************************************************************************/

/* Standard Library Header files */
#include <assert.h>
#include <stdint.h>

/* Driver Header files */
#include <ti/display/Display.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/NVS.h>
#include <ti/sysbios/BIOS.h>

#include <ti/drivers/AESECB.h>
#include <ti/drivers/SHA2.h>
#include <ti/drivers/ECJPAKE.h>

/* Example/Board Header files */
#include "ti_drivers_config.h"

/* Private configuration Header files */
#include "task_config.h"

/******************************************************************************
 Public Functions
 *****************************************************************************/

/**
 * Entry point for the application.
 *
 * Creates the OpenThread Stack task and Application task, then starts the
 * TI-RTOS scheduler.
 *
 * @note This function should never return.
 */
int main(void)
{
    /* Call driver init functions */
    Board_initGeneral();

#if (BOARD_DISPLAY_USE_UART || BOARD_DISPLAY_USE_LCD)
    Display_init();
#endif

    GPIO_init();

    NVS_init();

    ECJPAKE_init();

    AESECB_init();

    SHA2_init();

    Shade_taskCreate();

    /* Start sys/bios, this will never return */
    BIOS_start();

    return (0);
}

