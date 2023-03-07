/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
#ifndef __HAL_SDIO_SLAVE_H__
#define __HAL_SDIO_SLAVE_H__

#include "hal_platform.h"

#ifdef HAL_SDIO_SLAVE_MODULE_ENABLED


/**
 * @addtogroup HAL
 * @{
 * @addtogroup SDIO_SLAVE
 * @{
 * This section introduces the SDIO slave APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, SDIO slave function groups, enums, structures and functions.
 *
 * @section HAL_SDIO_SLAVE_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                  |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SDIO                         | Secure Digital Input and Output.For an introduction to the SDIO, please refer to <a href="https://en.wikipedia.org/wiki/Secure_Digital#SDIO">SDIO in Wikipedia </a>. |
 * |\b d2h                             | Device to host. |
 * |\b h2d                             | Host to device. |
 *
* @section HAL_SDIO_SLAVE_Features_Chapter Supported features
*
* - \b Supports \b DMA \b mode. \n
*    - \b DMA \b mode: In this mode, the DMA hardware carries data from the RAM to the SDIO slave FIFO and transmits to the SDIO host or
*                  reads data from the SDIO slave FIFO to the RAM directly.
*    .
*    There is two APIs for the DMA mode, one is the #hal_sdio_slave_receive_dma() to receive data from the SDIO host and the other is the #hal_sdio_slave_send_dma()
*     to send data to the SDIO host.
* \n
*
* @section HAL_SDIO_SLAVE_Architecture_Chapter Software architecture of the SDIO slave
*
* Call #hal_sdio_slave_init() to initialize the SDIO slave controller. \n
* Then user can call #hal_sdio_slave_write_mailbox() to write data to the SDIO host or call #hal_sdio_slave_read_mailbox()  to read data from the SDIO host.
* And call #hal_sdio_slave_trigger_d2h_interrupt() to trigger a software interrupt after call #hal_sdio_slave_write_mailbox() to inform host to read mailbox.
* The #hal_sdio_slave_send_dma() is used for send data to SDIO host and the #hal_sdio_slave_receive_dma() is used for receive data from the SDIO host. \n
*
*
* @section HAL_SDIO_SLAVE_Driver_Usage_Chapter How to use this driver
* - Read mailbox from the SDIO host. \n
*  - Step 1. Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
*  - Step 2. Call #hal_sdio_slave_init() to initialize the SDIO slave controller. Call this API only once.
*  - Step 3. Call #hal_sdio_slave_read_mailbox() to read mailbox after SDIO host write mailbox to slave.
*  - Sample code:
*    \include sdio_slv_pinmux.dox
*    @code
*
*        uint32_t mailbox_value = 0;
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_init(HAL_SDIO_SLAVE_PORT_0)) {
*            // Error handler
*        }
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_read_mailbox(HAL_SDIO_SLAVE_PORT_0, 0, &mailbox_value)) {
*            // Error handler
*        }
*     @endcode
*
*
* - Write mailbox to the SDIO host. \n
*  - Step 1. Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
*  - Step 2. Call #hal_sdio_slave_init() to initialize the SDIO slave controller. Call this API only once.
*  - Step 3. Call #hal_sdio_slave_write_mailbox() to write mailbox to SDIO host.
*  - Step 4. Call #hal_sdio_slave_trigger_d2h_interrupt() to inform SDIO host that slave has write the mailbox to host.
*  - Sample code:
*       \include sdio_slv_pinmux.dox
*       @code
*        uint32_t mailbox_value = 0;
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_init(HAL_SDIO_SLAVE_PORT_0)) {
*            // Error handler
*        }
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_write_mailbox(HAL_SDIO_SLAVE_PORT_0, 0, mailbox_value)) {
*            // Error handler
*        }
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_trigger_d2h_interrupt(HAL_SDIO_SLAVE_PORT_0, 0)) {
*            // Error handler
*        }
*     @endcode
*
* - Send data to the SDIO host. \n
*  - Step 1. Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
*  - Step 2. Call #hal_sdio_slave_init() to initialize the SDIO slave controller. Call this API only once.
*  - Step 3. Call #hal_sdio_slave_register_callback() register a user callback.
*  - Step 4. Call #hal_sdio_slave_send_dma() to send data to the SDIO host.
*  - Sample code:
*       \include sdio_slv_pinmux.dox
*       @code
*        uint32_t mailbox_value = 0;
*        hal_sdio_slave_rx_queue_id_t rx_queue = HAL_SDIO_SLAVE_RX_QUEUE_0;
*        uint32_t send_data_address;
*        uin32_t data_length;
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_init(HAL_SDIO_SLAVE_PORT_0)) {
*            // Error handler
*        }
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_register_callback(HAL_SDIO_SLAVE_PORT_0, sdio_slave_user_callback, NULL)) {
*            // Error handler
*        }
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_send_dma(HAL_SDIO_SLAVE_PORT_0, rx_queue, send_data_address, data_length)) {
*            // Error handler
*        }
*     @endcode
*       @code
*       // Callback function sample code. Pass this function to the driver while calling #hal_sdio_slave_register_callback().
*       void sdio_slave_user_callback(hal_sdio_slave_callback_event_t sdio_slave_event, void *parameter, void *user_data)
*       {
*            if ((HAL_SDIO_SLAVE_EVENT_RX0_DONE == sdio_slave_event) || (HAL_SDIO_SLAVE_EVENT_RX1_DONE == sdio_slave_event)) {
*                // Do something;
*            } else {
*                // Error handler;
*            }
*        }
*      @endcode
*
* - Receive data from the SDIO host. \n
*  - Step 1. Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
*  - Step 2. Call #hal_sdio_slave_init() to initialize the SDIO slave controller. Call this API only once.
*  - Step 3. Call #hal_sdio_slave_register_callback() register a user callback.
*  - Step 4. Call #hal_sdio_slave_receive_dma() to receive data from the SDIO host.
*  - Sample code:
*       \include sdio_slv_pinmux.dox
*       @code
*        uint32_t mailbox_value = 0;
*        hal_sdio_slave_tx_queue_id_t tx_queue = HAL_SDIO_SLAVE_TX_QUEUE_1;
*        uint32_t receive_data_address;
*        uin32_t data_length;
*        hal_sdio_slave_callback_tx_length_parameter_t tx_length;
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_init(HAL_SDIO_SLAVE_PORT_0)) {
*            // Error handler
*        }
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_register_callback(HAL_SDIO_SLAVE_PORT_0, sdio_slave_user_callback, NULL)) {
*            // Error handler
*        }
*
*        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_receive_dma(HAL_SDIO_SLAVE_PORT_0, tx_queue, receive_data_address, data_length)) {
*            // Error handler
*        }
*     @endcode
*       @code
*       // Callback function sample code. Pass this function to the driver while calling #hal_sdio_slave_register_callback().
*       void sdio_slave_user_callback(hal_sdio_slave_callback_event_t sdio_slave_event, void *parameter, void *user_data)
*       {
*            hal_sdio_slave_callback_tx_length_parameter_t *tx_len;
*            if (HAL_SDIO_SLAVE_EVENT_TX1_DONE == sdio_slave_event) {
*                tx_len = (hal_sdio_slave_callback_tx_length_parameter_t *)parameter;
*                tx_length.hal_sdio_slave_tx_length = tx_len->hal_sdio_slave_tx_length;
*                // Do something;
*            } else {
*                // Error handler;
*            }
*        }
*     @endcode
*/


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_sdio_slave_enum Enum
  * @{
  */

/** @brief This enum defines the SDIO slave API's return type. */
typedef enum {
    HAL_SDIO_SLAVE_STATUS_NOT_READY         = -4,       /**<  The SDIO slave is not ready. */
    HAL_SDIO_SLAVE_STATUS_ERROR             = -3,       /**<  An error occurred and the operation failed. */
    HAL_SDIO_SLAVE_STATUS_BUSY              = -2,       /**<  The SDIO slave is busy error occurred. */
    HAL_SDIO_SLAVE_STATUS_INVALID_PARAMETER = -1,       /**<  Invalid parameter.*/
    HAL_SDIO_SLAVE_STATUS_OK                = 0         /**<  No error occurred during the function call. */
} hal_sdio_slave_status_t;

/** @brief This enum defines the SDIO slave TX queue number. */
typedef enum {
    HAL_SDIO_SLAVE_TX_QUEUE_1 = 1,   /**< The SDIO slave TX queue 1. */
    HAL_SDIO_SLAVE_TX_QUEUE_MAX
} hal_sdio_slave_tx_queue_id_t;

/** @brief This enum defines the SDIO slave RX queue number. */
typedef enum {
    HAL_SDIO_SLAVE_RX_QUEUE_0 = 0,    /**< The SDIO slave RX queue 0. */
    HAL_SDIO_SLAVE_RX_QUEUE_1 = 1,    /**< The SDIO slave RX queue 1. */
    HAL_SDIO_SLAVE_RX_QUEUE_MAX
} hal_sdio_slave_rx_queue_id_t;

/** @brief This enum defines the SDIO slave callback event. */
typedef enum {
    HAL_SDIO_SLAVE_EVENT_ERROR = -1,                   /**< Indicates if an error occured. */
    HAL_SDIO_SLAVE_EVENT_RX0_DONE     = 0,             /**< SDIO slave RX0 queue send operation completed. */
    HAL_SDIO_SLAVE_EVENT_RX1_DONE    = 1,              /**< SDIO slave RX1 queue send operation completed. */
    HAL_SDIO_SLAVE_EVENT_TX1_DONE     = 2,               /**< SDIO slave TX1 queue receive operation completed. */
    HAL_SDIO_SLAVE_EVENT_SW_INTERRUPT     = 3,           /**< SDIO slave receive operation completed. */
    HAL_SDIO_SLAVE_EVENT_FW_OWN     = 4,                 /**< SDIO slave FW ownership event. */
    HAL_SDIO_SLAVE_EVENT_NONE     = 5                   /**< SDIO slave default event. */
} hal_sdio_slave_callback_event_t;

/** @brief This enum defines the SDIO slave error code. */
typedef enum {
    HAL_SDIO_SLAVE_EVENT_ERROR_NONE = 0,                   /**< Indicates if an error occured. */
    HAL_SDIO_SLAVE_EVENT_ERROR_TX0  = 1,                   /**< Device get error, when Host TX. */
    HAL_SDIO_SLAVE_EVENT_ERROR_RX0  = 2,                   /**< Device get error, when Host RX. */
    HAL_SDIO_SLAVE_EVENT_ERROR_RX1  = 3,                   /**< Device get error, when Host RX. */
    HAL_SDIO_SLAVE_EVENT_ERROR_CRC  = 4,                   /**< Device get Data CRC error when Host TX. */
    HAL_SDIO_SLAVE_EVENT_ERROR_TIMEOUT_TX = 5,                   /**< Device cannot receive the data well in pre-defined period. */
    HAL_SDIO_SLAVE_EVENT_ERROR_TIMEOUT_RX = 6,                   /**< Device cannot prepare the data well in pre-defined period. */
    HAL_SDIO_SLAVE_EVENT_ERROR_TIMEOUT_RXTX = 7,                   /**< Device Get TX Timeout and Rx Timeout*/
} hal_sdio_slave_callback_event_hw_error_type_t;


/** @brief This enum defines the software interrupt number triggered by host, and is used when the SDIO slave callback event is HAL_SDIO_SLAVE_EVENT_SW_INTERRUPT. */
typedef struct {
    uint32_t hal_sdio_slave_sw_interrupt_number;  /**< The software interrupt number triggered by host. */
} hal_sdio_slave_callback_sw_interrupt_parameter_t;

/** @brief This enum defines the data length of receive packet from host, and is used when the SDIO slave callback event is HAL_SDIO_SLAVE_EVENT_TX1_DONE. */
typedef struct {
    uint32_t hal_sdio_slave_tx_length;    /**< The data length of receive packet from host. */
} hal_sdio_slave_callback_tx_length_parameter_t;


/**
  * @}
  */

/** @defgroup hal_sdio_slave_typedef Typedef
  * @{
  */
/** @brief    This function defines the callback function prototype.
 *         Register a callback function to handle the SDIO slave interrupt.
 *           For more details about the callback, please refer to #hal_sdio_slave_register_callback().
 *  @param [in] sdio_slave_event is the transaction event for the current transaction or the interrupt event triggered by host.
 *              For more details about the event type, please refer to #hal_sdio_slave_callback_event_t.
 *  @param [in] parameter is the software interrupt number triggered by the host or the received data length from the host.
 *  @param [in] user_data is the user defined parameter obtained from #hal_sdio_slave_register_callback() function.
 */
typedef void (*hal_sdio_slave_callback_t)(hal_sdio_slave_callback_event_t sdio_slave_event, void *parameter, void *user_data);
/**
  * @}
  */

/**
 * @brief This function registers a callback function to handle the SDIO slave interrupt.
 * @param[in] sdio_slave_port is the SDIO slave port to handle an interrupt.
 * @param[in] sdio_slave_callback is the function pointer of the callback.
 *            The callback function is called when an SDIO slave interrupt occurs.
 * @param[in] user_data is the callback parameter.
 * @return    #HAL_SDIO_SLAVE_STATUS_OK, if the operation completed successfully.
 */
hal_sdio_slave_status_t hal_sdio_slave_register_callback(hal_sdio_slave_port_t sdio_slave_port, hal_sdio_slave_callback_t sdio_slave_callback, void *user_data);


/**
 * @brief  This function initializes the SDIO slave controller.
 * @param[in] sdio_slave_port is the initialization configuration port. For more details about this parameter, please refer to #hal_sdio_slave_port_t.
 * @return    Indicates whether this function call is successful.
 *            If the return value is #HAL_SDIO_SLAVE_STATUS_OK, the call succeeded, else the initialization has failed.
 */
hal_sdio_slave_status_t hal_sdio_slave_init(hal_sdio_slave_port_t sdio_slave_port);

/**
 * @brief  This function deinitializes the SDIO slave controller setting.
 * @param[in] sdio_slave_port is the deinitialization port.
 * @return    #HAL_SDIO_SLAVE_STATUS_OK, if the operation completed successfully.
 */
hal_sdio_slave_status_t hal_sdio_slave_deinit(hal_sdio_slave_port_t sdio_slave_port);

/**
 * @brief  This function reads the mailbox sent from an SDIO host to the slave.
 * @param[in] sdio_slave_port is the SDIO slave port to read the mailbox.
 * @param[in] mailbox_number is the SDIO slave mailbox number to read. The mailbox number could be set to 0 or 1.
 * @param[in] mailbox_value is the mailbox value that the SDIO host sent to the slave.
 * @return    #HAL_SDIO_SLAVE_STATUS_OK, if the operation completed successfully.
 */
hal_sdio_slave_status_t hal_sdio_slave_read_mailbox(hal_sdio_slave_port_t sdio_slave_port, uint32_t mailbox_number, uint32_t *mailbox_value);

/**
 * @brief  This function writes the mailbox to the SDIO host.
 * @param[in] sdio_slave_port is the SDIO slave port to write the mailbox.
 * @param[in] mailbox_number is the SDIO slave mailbox number to write. The mailbox number could be set to 0 or 1.
 * @param[in] mailbox_value is the mailbox value sent to the SDIO host.
 * @return    #HAL_SDIO_SLAVE_STATUS_OK, if the operation completed successfully.
 */
hal_sdio_slave_status_t hal_sdio_slave_write_mailbox(hal_sdio_slave_port_t sdio_slave_port, uint32_t mailbox_number, uint32_t mailbox_value);


/**
 * @brief  This function triggers an interrupt from slave to the host.
 * @param[in] sdio_slave_port is the SDIO slave port to trigger device to host interrupt.
 * @param[in] interrupt_number is the interrupt number sent to the host. The interrupt number range is from 0 to 24.
 * @return    #HAL_SDIO_SLAVE_STATUS_OK, if the operation completed successfully.
 */
hal_sdio_slave_status_t hal_sdio_slave_trigger_d2h_interrupt(hal_sdio_slave_port_t sdio_slave_port, uint32_t interrupt_number);

/**
 * @brief  This function sends data to the host.
 * @param[in] sdio_slave_port is the SDIO slave port to send data to the host.
 * @param[in] queue_id is the RX queue to send data. For more details about this parameter, please refer to #hal_sdio_slave_rx_queue_id_t.
 * @param[in] buffer is the address to store the data that will be sent.
 * @param[in] size is the data length to be sent.
 * @return    #HAL_SDIO_SLAVE_STATUS_OK, if the operation completed successfully.
 *                #HAL_SDIO_SLAVE_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sdio_slave_status_t hal_sdio_slave_send_dma(hal_sdio_slave_port_t sdio_slave_port, hal_sdio_slave_rx_queue_id_t queue_id, const uint32_t *buffer, uint32_t size);


/**
 * @brief  This function receives the data from host.
 * @param[in] sdio_slave_port is the SDIO slave port to receive data from the host.
 * @param[in] queue_id is the TX queue to receive data. For more details about this parameter, please refer to #hal_sdio_slave_tx_queue_id_t.
 * @param[in] buffer is the address to store the data that will be received.
 * @param[in] size is the data length to be received.
 * @return    #HAL_SDIO_SLAVE_STATUS_OK, if the operation completed successfully.
 *                #HAL_SDIO_SLAVE_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sdio_slave_status_t hal_sdio_slave_receive_dma(hal_sdio_slave_port_t sdio_slave_port, hal_sdio_slave_tx_queue_id_t queue_id, uint32_t *buffer, uint32_t size);


/* Some internal machinery exposed here for better error handling and better control over transfer process */

/**
 * @brief Check whether the bus is owned by the host.
 *
 * @return True if the bus is owned by the host.
 */
bool hal_sdio_slave_check_fw_own(void);

/**
 * @brief Stop slave's RX DMA queue (ususlly to recover from slave-to-host TX errors).
 *
 * @param queue_id Queue ID to stop.
 */
void hal_sdio_slave_set_rx_queue_stop(hal_sdio_slave_rx_queue_id_t queue_id);

/**
 * @brief Initiate data receive process by the salve to get a data from the host: set "RX-ready" status on the bus and
 *        add the given DMA region (packet placeholder) to the host's TX queue.
 *        Compared with hal_sdio_slave_receive_dma, it does not check fw own bit.
 *
 * @param sdio_slave_port Slave's port number (ususally - 0).
 * @param queue_id        Host's TX queue ID.
 * @param buffer          Pointer to the DMA region (4-byte-aligned ZIDATA in non-cached memory) - a placeholder for the new packet.
 * @param size            Max size of data to be received from the host; must be equal to the buffer's size.
 *
 * @return Operation status.
 */
hal_sdio_slave_status_t hal_sdio_slave_receive(hal_sdio_slave_port_t sdio_slave_port, hal_sdio_slave_tx_queue_id_t queue_id, uint32_t *buffer, uint32_t size);



#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/


#endif /*HAL_SDIO_SLAVE_MODULE_ENABLED*/
#endif /*__HAL_SDIO_SLAVE_H__*/

