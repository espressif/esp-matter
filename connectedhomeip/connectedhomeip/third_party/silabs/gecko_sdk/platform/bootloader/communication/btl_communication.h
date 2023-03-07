/***************************************************************************//**
 * @file
 * @brief Communication interface for Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef BTL_COMMUNICATION_H
#define BTL_COMMUNICATION_H

#include <stdint.h>
#include "api/btl_errorcode.h"

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Communication Communication
 * @{
 * @brief Host communication interface
 * @details
 *   The Communication component provides an interface for implementing
 *   communication with a host device, such as a computer or a microcontroller.
 * @section communication_impl Communication Protocol Implementations
 *   Several components implement the communication interface, using different
 *   transports and protocols.
 ******************************************************************************/

/***************************************************************************//**
 * Initialize hardware for the communication component.
 ******************************************************************************/
void communication_init(void);

/***************************************************************************//**
 * Initialize communication between the bootloader and external host.
 * For example, indicate that all is well to the external host.
 *
 * @return Error code indicating success or failure
 * @retval ::BOOTLOADER_OK on success
 * @retval ::BOOTLOADER_ERROR_COMMUNICATION_START on failure
 ******************************************************************************/
int32_t communication_start(void);

/***************************************************************************//**
 * Not supposed to return until either the host signals the end of the
 * current session or a new image has been flashed and verified.
 *
 * @return Error code indicating success or failure
 * @retval ::BOOTLOADER_OK when a new image was flashed
 * @retval ::BOOTLOADER_ERROR_COMMUNICATION_ERROR on communication error
 * @retval ::BOOTLOADER_ERROR_COMMUNICATION_DONE when no image was received
 * @retval ::BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR when received image
 *         is invalid
 ******************************************************************************/
int32_t communication_main(void);

/***************************************************************************//**
 * Stop communication between the bootloader and external host.
 ******************************************************************************/
void communication_shutdown(void);

/** @} addtogroup Communication
 *  @} addtogroup Components
 */
#endif // BTL_COMMUNICATION_H
