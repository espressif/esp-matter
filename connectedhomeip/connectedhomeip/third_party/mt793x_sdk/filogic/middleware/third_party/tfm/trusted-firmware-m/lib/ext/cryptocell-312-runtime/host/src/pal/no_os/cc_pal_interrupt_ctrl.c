/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files *************************************************/
#include "cc_pal_types.h"
#include "cc_pal_mutex.h"
#include "cc_pal_interrupt_ctrl_plat.h"
#include "cc_regs.h"
#include "dx_host.h"
#include "cc_hal.h"
/************************ Defines ********************************************/

/************************ Enums **********************************************/

/************************ Typedefs *******************************************/

/************************ Global Data ****************************************/

/************************ Private Functions **********************************/

/************************ Public Functions ***********************************/
/**
 * @brief
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - CC_SUCCESS for success, CC_FAIL for failure.
 */
CCError_t CC_PalInitIrq(void)
{
    return CC_SUCCESS;
}

/**
 * @brief This function removes the interrupt handler for
 * cryptocell interrupts.
 *
 */
void CC_PalFinishIrq(void)
{
}


/*!
 * Busy wait upon Interrupt Request Register (IRR) signals.
 * This function notifys for any ARM CryptoCell interrupt, it is the caller responsiblity
 * to verify and prompt the expected case interupt source.
 *
 * @param[in] data  - input data for future use
 * \return  CCError_t   - CC_OK upon success
 */
CCError_t CC_PalWaitInterrupt( uint32_t data){
    uint32_t irr = 0;
    CCError_t error = CC_OK;

    /* busy wait upon IRR signal */
    do {
        irr = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR));
        /* check APB bus error from HOST */
        if( CC_REG_FLD_GET(0, HOST_IRR, AHB_ERR_INT, irr) == CC_TRUE){
            error = CC_FAIL;
            /*set data for clearing bus error*/
            CC_REG_FLD_SET(HOST_RGF, HOST_ICR, AXI_ERR_CLEAR, data , 1);
            break;
        }
    } while (!(irr & data));

    /* clear interrupt */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ICR), data); // IRR and ICR bit map is the same use data to clear interrupt in ICR

    return error;
}
