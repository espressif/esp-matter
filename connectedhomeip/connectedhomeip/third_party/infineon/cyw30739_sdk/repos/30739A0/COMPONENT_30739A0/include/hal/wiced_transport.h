/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 * @defgroup Transport     AIROC Transport
 * @ingroup HardwareDrivers
 *
 * Defines the transport utilities for configuring the transport, send data and
 * receive data. This supports UART transport in HCI Mode
 *
 * Wiced Transport uses the generic pool available for receiving and sending packet
 * over the transport. Following are the configurations of the generic pool
 * 1:   buffer size - 8 bytes, buffer count - 128
 * 2:   buffer size - 32 bytes, buffer count - 48
 * 3:   buffer size - 96 bytes, buffer count - 50
 * 4:   buffer size - 268 bytes, buffer count - 12
 *
 * Using above generic pools the max payload size supportable is 252.(16 bytes for internal headers)
 * If the application wants to send packet of size more than the max size of the buffer
 * available in the generic pool, application can do so by creating a transport buffer
 * pool of the desired size. (Refer APIs wiced_transport_create_buffer_pool,
 * wiced_transport_allocate_buffer,wiced_transport_send_buffer)
 *
 *If the application wants to recieve packet of size more than the max size of the buffer
 * available in the generic pool, application can do so by configuring a receive buffer pool
 * of the desired size when doing transport init.
 *
 * @{
 */
#ifndef _WICED_TRANSPORT_H_
#define _WICED_TRANSPORT_H_

#include "wiced_hal_pspi.h"

/*****************************************************************************
**                                                Type Definitions
*****************************************************************************/

/** Wiced transport types */
typedef enum
{
    WICED_TRANSPORT_UART,
    WICED_TRANSPORT_SPI,
    WICED_TRANSPORT_UNDEFINED
}wiced_transport_type_t;

/** Wiced uart transport mode */
typedef enum
{
    WICED_TRANSPORT_UART_HCI_MODE,
    WICED_TRANSPORT_UART_RAW_MODE,
}wiced_transport_uart_mode_t;


/** wiced transport buffer pool */
typedef struct _wiced_trans_buffer_pool_t wiced_transport_buffer_pool_t;

/** Wiced transport status handler  */
typedef void (*wiced_transport_status_handler_t)( wiced_transport_type_t type );

/** Wiced transport data handler
* Call back registered by the application to recieve data. Application has to free the
* buffer in which data is received. Use the API wiced_transport_free_buffer to free
* the rx buffer
*
* @param[in] p_data : Pointer to the received data buffer.
* @param[in] data_len : length of the data pointed to by p_data in bytes.
*
* @Return the status in case of UART in HCI mode
*/
typedef uint32_t (*wiced_tranport_data_handler_t)( uint8_t* p_data, uint32_t data_len );

/** Wiced transport tx complete indication. Indicates the application that a packet is sent using
 *    a buffer in the indicated pool.
 *
 * @param[in] p_pool : Pool pointer, buffer is used from this pool for sending the packet
 *
 * @return : None
 */
typedef void (*wiced_transport_tx_complete_t)( wiced_transport_buffer_pool_t* p_pool );

#pragma pack(1)

/** UART transport config */
typedef PACKED struct
{
    wiced_transport_uart_mode_t     mode;                        /**<  UART mode, HCI or Raw */
    uint32_t                        baud_rate;                   /**<  UART baud rate */
}wiced_uart_transport_cfg_t;

/** SPI transport config

    Please not that the SPI pin selection must be done using
    Supermux array (wiced_platform_pin_config.c).
*/
typedef PACKED struct
{
    uint32_t         clock_speed;         /**< Clock speed (non-zero for master, zero for slave).*/
    SPI_ENDIAN       endian;              /**< Direction of bit data flow (MSB or LSB first) */
    SPI_SS_POLARITY  polarity;            /**< Active high or active low for chip select line */
    SPI_MODE         mode;                /**< SPI mode (0-3). */
}wiced_spi_transport_cfg_t;

/** Wiced transport interface config */
typedef PACKED union
{
    wiced_uart_transport_cfg_t uart_cfg;
    wiced_spi_transport_cfg_t  spi_cfg;
}wiced_transport_interface_cfg_t;

/** Wiced receive buffer pool configuration. Application shall use this to receive
* packet of size( i.e if payload size > 252 ) > 268 bytes
*/

typedef PACKED struct
{
    uint32_t buffer_size;
    uint32_t buffer_count;
}wiced_transport_rx_buff_pool_cfg_t;

/** Wiced transport config */
typedef PACKED struct
{
    wiced_transport_type_t              type;                   /**< Wiced transport type. */
    wiced_transport_interface_cfg_t     cfg;                    /**< Wiced transport interface config. */
    wiced_transport_rx_buff_pool_cfg_t  rx_buff_pool_cfg;       /**< Wiced rx buffer pool config. */
    wiced_transport_status_handler_t    p_status_handler;       /**< Wiced transport status handler.*/
    wiced_tranport_data_handler_t       p_data_handler;         /**< Wiced transport receive data handler. */
    wiced_transport_tx_complete_t       p_tx_complete_cback;    /**< Wiced transport tx complete callback. */
}wiced_transport_cfg_t;

#pragma pack()

/*****************************************************************************
**                                                 Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_transport_init
 *
 * Intializes and configures the transport and also registers the handlers to be invoked on transport
 * detection and on receiving the data
 *
 * * @param[in]    p_cfg           :wiced transport config
 *
 * @return     : wiced_result_t
 */
wiced_result_t wiced_transport_init( const wiced_transport_cfg_t* p_cfg );

/**
 * Function         wiced_transport_create_buffer_pool
 *
 * Creates a buffer pool for transport usage. Application shall create the buffer pool if it has to send
 * packet of size > 268 bytes. Application shall specify the payload length as the buffer size. Transport
 * will take care of creating a pool of desired size considering the transport header requirements and the
 * application specified payload size
 *
 * * @param[in]    buffer_size           : Size of each buffer in pool. Application shall specify
 *                                                   the payload length as the buffer size
 * * @param[in]    buffer_count        : Number of buffers in the pool
 *
 * @return     : pointer to the buffer pool on success, NULL on failure
 */
wiced_transport_buffer_pool_t* wiced_transport_create_buffer_pool( uint32_t buffer_size, uint32_t buffer_count );

/**
 * Function         wiced_transport_allocate_buffer
 *
 * Allocates a buffer from the pool
 *
 * @param[in]    p_pool           : Pointer to buffer pool returned from wiced_transport_create_buffer_pool
 *
 * @return     : Pointer to the payload on success, NULL on failure.
 *                   Application shall write the payload starting from this location
 */
void* wiced_transport_allocate_buffer( wiced_transport_buffer_pool_t* p_pool );

/**
 * Function         wiced_transport_get_buffer_size
 *
 * Returns the size of buffer in the pool
 *
 * @param[in]    p_pool           : Pointer to buffer pool returned from wiced_trans_create_buffer_pool
 *
 * @return     : size of the buffers in the pool
 */
uint32_t wiced_transport_get_buffer_size( wiced_transport_buffer_pool_t *p_pool );
/**
 * Function         wiced_transport_get_buffer_count
 *
 * To get the number of buffers available in the pool
 *
 * @param[in]    p_pool           : Pointer to buffer pool created using wiced_transport_create_buffer_pool
 *
 * @return     : the number of buffers available in the pool
 */
uint32_t wiced_transport_get_buffer_count( wiced_transport_buffer_pool_t *p_pool );

/**
 * Function         wiced_transport_send_buffer
 *
 *    Send the packet to the host over the transport using the buffer allocated by the application.
 *    This function takes care of preparing the header and sending the data. The buffer will be freed
 *    by the transport after sending the packet.
 *
 * Note: Application has to allocate buffer from transport pool using wiced_transport_allocate_buffer
 *         and copy the payload to this buffer and send the payload pointer.
 *         This allows the application to use custom size buffers and avoid overrun of generic buffers,
 *         which is shared across firmware code.
 *
 *@param[in]    code                 :Group code and command code
 *@param[in]    p_buf                :Pointer to the payload
 *@param[in]    length               :Payload length
 * @return   wiced_result_t
 */
wiced_result_t wiced_transport_send_buffer( uint16_t code, uint8_t* p_buf, uint16_t length );

/**
 * Function         wiced_transport_free_buffer
 *
 * Frees the transport buffer.
 *
 * Note: When receiving a packet, application should take care of freeing the rx buffers.
 *         When sending a packet, transport will take care of freeing the buffer after the
 *         packet is sent.
 *
 *@param[in]    p_buf                :Pointer to the buffer to be freed
 *
 * @return   None
 */
void wiced_transport_free_buffer( void * p_buf );

/**
 * Function         wiced_transport_send_data
 *
 *Send the packet to the host over the transport.
*This function allocates a buffer internally and prepare the header, copy the payload
 *and then sends the packet over the transport. Maximum size of the buffer that can be
 *allocated is 268 bytes
 *
 * Transport internally uses a buffer from the pool which is available for all general purposes
 * Following are the configuration of the internal pool
 * 1:   buffer size - 8 bytes, buffer count - 128
 * 2:   buffer size - 32 bytes, buffer count - 48
 * 3:   buffer size - 96 bytes, buffer count - 50
 * 4:   buffer size - 268 bytes, buffer count - 12

 *
 * Note: Using above genric pools max supportable payload size = 252(16 bytes for internal headers).
 *@param[in]    code                   :Group code and command code
 *@param[in]    p_data                :Pointer to the payload
 *@param[in]    length                 :Payload length
 *
 * @return   wiced_result_t
 */
wiced_result_t wiced_transport_send_data ( uint16_t code, uint8_t* p_data, uint16_t length );

/**
 * Function         wiced_transport_send_hci_trace
 *
 * Send the hci trace data over the transport.
 *
 *@param[in]    hci_trans_pool       :Pass the pointer to the pool created by the application
                                                 incase application  has created a dedicated trans pool for
                                                 communicating to host. Pass NULL if the application wants the stack to
                                                 take care of allocating the buffer for sending the data to host.
                                                 Application should be able to use transport buffer pool that it allocates and trace the whole HCI packets.
                                                 In case of stack allocation, the size of trace compromised according to buffer availability.

 *@param[in]    type                    :HCI trace type
 *@param[in]    p_data                 :Pointer to the data payload
 *@param[in]    length                  :Data payload length
 *
 * @return   wiced_result_t          WICED_SUCCESS, if success,
 *                                    WICED_NO_MEMORY if buffers not available to send,
 *                                    WICED_ERROR otherwise
 */
wiced_result_t wiced_transport_send_hci_trace( wiced_transport_buffer_pool_t *hci_trans_pool ,
                                                             wiced_bt_hci_trace_type_t type, uint16_t length, uint8_t* p_data );

/**
 * Function         wiced_transport_set_detect_on
 *
 * Set transport sense detect as ON within following seconds
 *
 * Note: If HID off mode sleep configured, transport will be disabled.
 *       Application can use this API to let transport on within assigned duration.
 *
 * @param[in]    detect_on_second   : user assigned duration (unit: second) to enable transport
 *
 * @return       wiced_result_t     : WICED_SUCCESS, if success,
 *                                    WICED_ERROR otherwise
 */
wiced_result_t wiced_transport_set_detect_on( UINT16 detect_on_second );

/**
 * Function         wiced_transport_send_raw_buffer
 *
 *    Used when transport mode is WICED_TRANSPORT_UART_RAW_MODE. Available with the wiced_uart_raw_mode_lib.
 *    Send the packet to the host over the transport using the buffer allocated by the application.
 *    This function takes care of preparing the header and sending the data. The buffer must be freed
 *    by the application if return status is WICED_SUCCESS.
 *
 * Note: Application has to allocate buffer from transport pool using wiced_transport_allocate_buffer
 *         and copy the payload to this buffer and send the payload pointer.
 *         This allows the application to use custom size buffers and avoid overrun of generic buffers,
 *         which is shared across firmware code.
 *
 *@param[in]    p_buf                :Pointer to the payload
 *@param[in]    length               :Payload length
 * @return   wiced_result_t
 */
wiced_result_t wiced_transport_send_raw_buffer( uint8_t* p_buf, uint16_t length );

/**
 * Function         wiced_transport_uart_rx_pause
 *
 * Pause / Disable the HCI UART Rx.
 *
 */
void wiced_transport_uart_rx_pause(void);

/**
 * Function         wiced_transport_uart_rx_resume
 *
 * Resume / Enable the HCI UART Rx.
 *
 */
void wiced_transport_uart_rx_resume(void);

/**
 * Function         wiced_transport_uart_interrupt_disable
 *
 * Disable UART interrupt.
 *
 */
void wiced_transport_uart_interrupt_disable(void);

/**
 * Function         wiced_transport_uart_interrupt_enable
 *
 * Enable UART interrupt.
 *
 */
void wiced_transport_uart_interrupt_enable(void);

/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // _WICED_TRANSPORT_H_
