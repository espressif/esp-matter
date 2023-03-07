/***************************************************************************//**
 * @file
 * @brief Definitions for the DMP Tuning and Testing plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef _DMP_TUNING_H_
#define _DMP_TUNING_H_

/**
 * @defgroup dmp-tuning  DMP Tuning
 * @ingroup component soc
 * @brief API and Callbacks for the DMP Tuning Component
 *
 */

/**
 * @addtogroup dmp-tuning
 * @{
 */

extern uint32_t railScheduledEventCntr;
extern uint32_t railUnscheduledEventCntr;

// Share copy of the link layer priorities with CLI
extern sl_bt_bluetooth_ll_priorities ll_priorities;

/** @} */ // end of dmp-tuning

#endif // _DMP_TUNING_H_
