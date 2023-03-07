/*******************************************************************************
* @file  rsi_driver.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

/*
  Includes
  */

#include "rsi_driver.h"
/** @addtogroup DRIVER5
* @{
*/
/*===================================================*/
/**
 * @fn			void rsi_interrupt_handler(void)
 * @brief		Handle the interrupt coming from the module. 
 * @param[in]	void 
 * @return	    void 
 */
/// @private
void rsi_interrupt_handler(void)
{
#ifndef RSI_M4_INTERFACE
  // Disable the interrupt
  rsi_hal_intr_mask();
#ifdef RSI_WITH_OS
  // Set event RX pending from device
  rsi_set_event_from_isr(RSI_RX_EVENT);
#else
  // Set event RX pending from device
  rsi_set_event(RSI_RX_EVENT);
#endif
#endif
  return;
}
/*===================================================*/
/**
 * @fn			void rsi_mask_ta_interrupt(void)
 * @brief		Mask the TA interrupt. 
 * @param[in]	void 
 * @return	    void
 */
/// @private
void rsi_mask_ta_interrupt(void)
{
#ifdef RSI_M4_INTERFACE
  // mask P2P interrupt
  mask_ta_interrupt(TX_PKT_TRANSFER_DONE_INTERRUPT | RX_PKT_TRANSFER_DONE_INTERRUPT);
#else
  rsi_hal_intr_mask();
#endif
}
/*===================================================*/
/**
 * @fn			void rsi_unmask_ta_interrupt(void)
 * @brief		Unmask the TA interrupt. 
 * @param[in]	void 
 * @return	    void
 */
/// @private
void rsi_unmask_ta_interrupt(void)
{
#ifdef RSI_M4_INTERFACE
  // unmask P2P interrupt
  unmask_ta_interrupt(TX_PKT_TRANSFER_DONE_INTERRUPT | RX_PKT_TRANSFER_DONE_INTERRUPT);
#else
  rsi_hal_intr_unmask();
#endif
}
/** @} */
