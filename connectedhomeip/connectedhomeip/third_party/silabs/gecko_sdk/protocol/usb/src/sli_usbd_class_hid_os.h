/***************************************************************************//**
 * @file
 * @brief USB Device - USB Hid Class Operating System Layer
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SLI_USBD_CLASS_HID_OS_H
#define  SLI_USBD_CLASS_HID_OS_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdint.h>

#include "sl_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           sli_usbd_hid_os_init()
 *
 * @brief    Initialize HID OS interface.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_init(void);

/****************************************************************************************************//**
 *                                           sli_usbd_hid_os_lock_input()
 *
 * @brief    Lock class input report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_lock_input(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                           sli_usbd_hid_os_unlock_input()
 *
 * @brief    Unlock class input report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_unlock_input(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_os_pend_input()
 *
 * @brief    Wait for input report data transfer to complete.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    timeout_ms  Signal wait timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_input(uint8_t  class_nbr,
                                       uint16_t timeout_ms);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_os_pend_abort_input()
 *
 * @brief    Abort any operation on input report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_abort_input(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_os_post_input()
 *
 * @brief    Signal that input report data transfer has completed.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_post_input(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                           sli_usbd_hid_os_lock_output()
 *
 * @brief    Lock class output report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_lock_output(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_os_unlock_output()
 *
 * @brief    Unlock class output report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_unlock_output(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_os_pend_abort_output()
 *
 * @brief    Abort class output report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_abort_output(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_os_pend_output()
 *
 * @brief    Wait for output report data transfer to complete.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    timeout_ms  Signal wait timeout, in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_output(uint8_t  class_nbr,
                                        uint16_t timeout_ms);

/****************************************************************************************************//**
 *                                       sli_usbd_hid_os_post_output()
 *
 * @brief    Signal that output report data is available.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_post_output(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                           sli_usbd_hid_os_lock_tx()
 *
 * @brief    Lock class transmit.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_lock_tx(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                           sli_usbd_hid_os_unlock_tx()
 *
 * @brief    Unlock class transmit.
 *
 * @param    class_nbr   Class instance number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_unlock_tx(uint8_t class_nbr);

/****************************************************************************************************//**
 ********************************************************************************************************
 *                                                MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
