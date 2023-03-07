/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_PAL_INTERRUPTCTRL_PLAT_H
#define _CC_PAL_INTERRUPTCTRL_PLAT_H



/**
 * @brief This function removes the interrupt handler for
 * cryptocell interrupts.
 *
 */
void CC_PalFinishIrq(void);

/* @brief
*
* @param[in]
*
* @param[out]
*
* @return - CC_SUCCESS for success, CC_FAIL for failure.
*/
CCError_t CC_PalInitIrq(void);

/*!
 * Busy wait upon Interrupt Request Register (IRR) signals.
 * This function notifys for any ARM CryptoCell interrupt, it is the caller responsiblity
 * to verify and prompt the expected case interupt source.
 *
 * @param[in] data  - input data for future use
 * \return  CCError_t   - CC_OK upon success
 */
CCError_t CC_PalWaitInterrupt( uint32_t data);


#endif /* _CC_PAL_INTERRUPTCTRL_H */

