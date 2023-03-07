/***************************************************************************//**
 * @file
 * @brief USB Device Drivers - Api Declarations
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef  SLI_USBD_DRIVER_H
#define  SLI_USBD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#include "sl_usbd_core.h"

#include "sli_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                           USB DEVICE DRIVER APIS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   SYNOPSYS DESIGNWARE OTG FS DRIVER
 *
 * Note(s) : (1) The following MCUs are support by USBD_DrvAPI_EFx32_OTG_FS API:
 *
 *                           Silicon Labs Series 1 EFM32 Giant   Gecko series.
 *                           Silicon Labs Series 1 EFM32 Wonder  Gecko series.
 *                           Silicon Labs Series 1 EFM32 Leopard Gecko series.
 *                           Silicon Labs Series 2 EFR32 Flex    Gecko series.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           sli_usbd_driver_init()
 *
 * @brief    Initialize the Full-speed device.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Since the CPU frequency could be higher than OTG module clock, a timeout is needed
 *               to reset the OTG controller successfully.
 *
 *           (2) If a debugger is used to step into the driver, locking the OTG FS registers may make the
 *               debugger crash if the USB OTG FS register view is opened. Indeed, the various registers
 *               reads done by the debugger to refresh the registers content may not be properly handled
 *               while they are locked.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_init(void);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_get_speed()
 *
 * @brief    Get the USB speed of the device driver.
 *
 * @param    p_spd   Pointer to a variable that will receive the speed.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_get_speed(sl_usbd_device_speed_t *p_spd);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_get_endpoint_info()
 *
 * @brief    Get the EP info table structure for the device driver.
 *
 * @param    p_info_ptr  Pointer to another pointer of type (sli_usbd_driver_endpoint_info_t *) which will be set
 *                       to refer to the EP info table.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_get_endpoint_info(sli_usbd_driver_endpoint_info_t **p_info_ptr);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_start()
 *
 * @brief    Start device operation with VBUS detection disabled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_start(void);

/****************************************************************************************************//**
 *                                               sli_usbd_driver_stop()
 *
 * @brief    Stop Full-speed device operation.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Clear and disable USB interrupts.
 *
 * @note     (2) Disconnect from the USB host (e.g, reset the pull-down on the D- pin).
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_stop(void);

/****************************************************************************************************//**
 *                                               sli_usbd_driver_set_address()
 *
 * @brief    Assign an address to device.
 *
 * @param    dev_addr    Device address assigned by the host.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) For device controllers that have hardware assistance to enable the device address after
 *               the status stage has completed, the assignment of the device address can also be
 *               combined with enabling the device address mode.
 *
 * @note     (2) For device controllers that change the device address immediately, without waiting the
 *               status phase to complete, see USBD_DrvAddrEn().
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_set_address(uint8_t  dev_addr);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_get_frame_number()
 *
 * @brief    Retrieve current frame number.
 *
 * @param    p_frame_nbr   Pointer to a variable that will receive frame number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_get_frame_number(uint16_t *p_frame_nbr);

/****************************************************************************************************//**
 *                                               sli_usbd_driver_open_endpoint()
 *
 * @brief    Open and configure a device endpoint, given its characteristics (e.g., endpoint type,
 *           endpoint address, maximum packet size, etc).
 *
 * @param    ep_addr             Endpoint address.
 *
 * @param    ep_type             Endpoint type :
 *                                   - SL_USBD_ENDPOINT_TYPE_CTRL,
 *                                   - SL_USBD_ENDPOINT_TYPE_ISOC,
 *                                   - SL_USBD_ENDPOINT_TYPE_BULK,
 *                                   - SL_USBD_ENDPOINT_TYPE_INTR.
 *
 * @param    max_pkt_size        Maximum packet size.
 *
 * @param    transaction_frame   Endpoint transactions per frame.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Typically, the endpoint open function performs the following operations:
 *               - (a) Validate endpoint address, type and maximum packet size.
 *               - (b) Configure endpoint information in the device controller. This may include not
 *                     only assigning the type and maximum packet size, but also making certain that
 *                     the endpoint is successfully configured (or  realized  or  mapped ). For some
 *                     device controllers, this may not be necessary.
 *
 * @note     (2) If the endpoint address is valid, then the endpoint open function should validate
 *               the attributes allowed by the hardware endpoint :
 *               - (a) The maximum packet size 'max_pkt_size' should be validated to match hardware
 *                     capabilities.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_open_endpoint(uint8_t   ep_addr,
                                          uint8_t   ep_type,
                                          uint16_t  max_pkt_size,
                                          uint8_t   transaction_frame);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_close_endpoint()
 *
 * @brief    Close a device endpoint, and uninitialize/clear endpoint configuration in hardware.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_close_endpoint(uint8_t ep_addr);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_endpoint_rx_start()
 *
 * @brief    Configure endpoint with buffer to receive data.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to data buffer (see note #1).
 *
 * @param    buf_len     Length of the buffer.
 *
 * @param    p_pkt_len   Pointer to variable that will receive the umber of bytes that can be
 *                       handled in one transfer.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Receive buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_rx_start(uint8_t   ep_addr,
                                              uint8_t   *p_buf,
                                              uint32_t  buf_len,
                                              uint32_t  *p_pkt_len);

/****************************************************************************************************//**
 *                                               sli_usbd_driver_endpoint_rx()
 *
 * @brief    Receive the specified amount of data from device endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to data buffer.
 *
 * @param    buf_len     Length of the buffer.
 *
 * @param    p_xfer_len  Pointer to variable that will receive the number of octets received, if no
 *                       errors occur, otherwise the variable is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_rx(uint8_t   ep_addr,
                                        uint8_t   *p_buf,
                                        uint32_t  buf_len,
                                        uint32_t  *p_xfer_len);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_endpoint_rx_zlp()
 *
 * @brief    Receive zero-length packet from endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_rx_zlp(uint8_t ep_addr);

/****************************************************************************************************//**
 *                                               sli_usbd_driver_endpoint_tx()
 *
 * @brief    Configure endpoint with buffer to transmit data.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to buffer of data that will be transmitted (see note #1).
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    p_xfer_len  Pointer to variable that will receive the number of octets transmitted, if no
 *                       errors occur, otherwise the variable is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Transmit buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_tx(uint8_t   ep_addr,
                                        uint8_t   *p_buf,
                                        uint32_t  buf_len,
                                        uint32_t  *p_xfer_len);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_endpoint_tx_start()
 *
 * @brief    Transmit the specified amount of data to device endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to buffer of data that will be transmitted.
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_tx_start(uint8_t   ep_addr,
                                              uint8_t   *p_buf,
                                              uint32_t  buf_len);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_endpoint_tx_zlp()
 *
 * @brief    Transmit zero-length packet from endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_endpoint_tx_zlp(uint8_t ep_addr);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_abort_endpoint()
 *
 * @brief    Abort any pending transfer on endpoint.
 *
 * @param    ep_addr     Endpoint Address.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_abort_endpoint(uint8_t ep_addr);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_stall_endpoint()
 *
 * @brief    Set or clear stall condition on endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    state       Endpoint stall state.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_stall_endpoint(uint8_t  ep_addr,
                                           bool     state);

/****************************************************************************************************//**
 *                                           sli_usbd_driver_irq_handler()
 *
 * @brief    USB device Interrupt Service Routine (ISR) handler.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_driver_irq_handler(void);

/****************************************************************************************************//**
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
