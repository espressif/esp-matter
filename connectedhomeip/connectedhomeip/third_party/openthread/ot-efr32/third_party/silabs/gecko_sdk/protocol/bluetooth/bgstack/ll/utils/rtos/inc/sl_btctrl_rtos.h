/***************************************************************************//**
 * @brief Adaptation for running Bluetooth in RTOS
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

#ifndef SL_BTCTRL_RTOS_ADAPTATION_H
#define SL_BTCTRL_RTOS_ADAPTATION_H

/***************************************************************************//**
 * @addtogroup bluetooth_controller_rtos_adaptation
 * @{
 *
 * @brief Bluetooth RTOS adaptation
 *
 * The Bluetooth Controller RTOS adaptation component implements the relevant interfaces
 * for running the Bluetooth Link Layer in an RTOS. When initialized in @ref
 * sl_btctrl_rtos_init(), the component creates the required RTOS tasks for the
 * Bluetooth controller itself
 *
 */

/**
 * @brief Initialize Bluetooth Controller for running in RTOS.
 * @return SL_STATUS_OK if succeeds, otherwise error
 */
sl_status_t sl_btctrl_rtos_init();

/** @} end bluetooth_controller_rtos_adaptation */

#endif //SL_BTCTRL_RTOS_ADAPTATION_H
