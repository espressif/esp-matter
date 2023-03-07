/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave UART.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_UART_H_
#define ZPAL_UART_H_

#include <stdbool.h>
#include <stddef.h>
#include "zpal_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief
 * Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-uart
 * @brief
 * Defines a platform abstraction layer for the Z-Wave UART.
 *
 * How to use the UART API
 *
 * The ZPAL UART API is used by only SerialAPI application, all functions are required.
 *
 * The following outlines an example of use:
 * 1. To initialize UART interface, invoke zpal_uart_init().
 * 2. Enable UART by invoking zpal_uart_enable().
 * 3. Invoke zpal_uart_transmit() to send data.
 * 4. Use zpal_uart_transmit_in_progress() to check if transmission is in progress.
 * 5. To get number of bytes available for reading, invoke zpal_uart_get_available().
 * 6. Read received data with zpal_uart_receive().
 *
 * Requirements:
 * - This interface must implement a non-blocking behavior.
 * - Transmit and receive buffers are provided by application.
 * - Transmit and receive buffers should be at least 200 bytes large.
 *
 * @{
 */

/**
 * @brief IDs for each of the UARTs.
 */
typedef enum
{
  ZPAL_UART0,
  ZPAL_UART1,
} zpal_uart_id_t;

/**
 * @brief IDs for each of the parity mode.
 */
typedef enum
{
  ZPAL_UART_NO_PARITY,      ///< No parity.
  ZPAL_UART_EVEN_PARITY,    ///< Even parity.
  ZPAL_UART_ODD_PARITY,     ///< Odd parity.
} zpal_uart_parity_bit_t;

/**
 * @brief IDs for each of the stop bits configuration.
 */
typedef enum
{
  ZPAL_UART_STOP_BITS_0P5,  ///< 0.5 stop bits.
  ZPAL_UART_STOP_BITS_1,    ///< 1 stop bits.
  ZPAL_UART_STOP_BITS_1P5,  ///< 1.5 stop bits.
  ZPAL_UART_STOP_BITS_2,    ///< 2 stop bits.
} zpal_uart_stop_bits_t;

/**
 * @brief UART handle type.
 */
typedef void * zpal_uart_handle_t;

/**
 * @brief Defines a type for the UART receive callback that is invoked whenever data is ready.
 *
 * @param[in] handle UART handle.
 * @param[in] length Length of available data in bytes.
 */
typedef void (*zpal_uart_receive_callback_t)(zpal_uart_handle_t handle, size_t length);

/**
 * @brief Defines a type for the UART transmit done callback that is invoked when data has been transmitted.
 *
 * @param[in] handle UART handle.
 */
typedef void (*zpal_uart_transmit_done_t)(zpal_uart_handle_t handle);

/**
 * @brief UART configuration.
 */
typedef struct {
  zpal_uart_id_t id;                              ///<Uart id.
  uint8_t tx_pin;                                 ///< Pin for UART Tx.
  uint8_t tx_port;                                ///< Port for UART Tx pin.
  uint8_t tx_loc;                                 ///< Location number for UART Tx pin.
  uint8_t rx_pin;                                 ///< Pin for UART Rx.
  uint8_t rx_port;                                ///< Port for UART Rx pin.
  uint8_t rx_loc;                                 ///< Location number for UART Tx pin.
  uint8_t *tx_buffer;                             ///<Poniter to transmit buffer.
  size_t tx_buffer_len;                           ///<Size of transmit buffer.
  uint8_t *rx_buffer;                             ///<Poniter to receive buffer.
  size_t rx_buffer_len;                           ///<Size of receive buffer.
  uint32_t baud_rate;                             ///<Baud rate of UART.
  uint8_t data_bits;                              ///<Defines amount of data bits in the UART frame.
  zpal_uart_parity_bit_t parity_bit;              ///<Defines parity bit in the UART frame.
  zpal_uart_stop_bits_t stop_bits;                ///<Define stop bit in the UART frame
  zpal_uart_receive_callback_t receive_callback;  ///< Expect this callback to be invoked in interrupt context.
  void *ptr;                                      ///< This can be used for anything, but might not work for all platform implementations.
} zpal_uart_config_t;

/**
 * @brief Initializes a UART based on the given configuration.
 *
 * @param[in]   config Pointer to UART configuration.
 * @param[out]  handle Outputs a handle to a UART.
 * @return @ref ZPAL_STATUS_OK on success and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_uart_init(const zpal_uart_config_t *config, zpal_uart_handle_t *handle);

/**
 * @brief Enables a given UART.
 *
 * @param[in] handle UART handle.
 * @return @ref ZPAL_STATUS_OK on success and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_uart_enable(zpal_uart_handle_t handle);

/**
 * @brief Disables a given UART.
 *
 * @param[in] handle UART handle.
 * @return @ref ZPAL_STATUS_OK on success and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_uart_disable(zpal_uart_handle_t handle);

/**
 * @brief Transmits data using a given UART.
 *
 * @param[in] handle  UART handle.
 * @param[in] data    Pointer to data.
 * @param[in] length  Length of data.
 * @param[in] tx_cb   Transmission done callback.
 * @return @ref ZPAL_STATUS_OK if transmission has started,
 *         @ref ZPAL_STATUS_FAIL otherwise.
 *
 * @note Expect @ref tx_cb callback to be invoked in interrupt context.
 * @note This function will block if length of data exceeds internal transmit buffer.
 */
zpal_status_t zpal_uart_transmit(zpal_uart_handle_t handle, const uint8_t *data, size_t length,
                                 zpal_uart_transmit_done_t tx_cb);

/**
 * @brief Returns whether transmission is in progress.
 *
 * @param[in] handle UART handle.
 * @return True if transmission is in progress, false otherwise.
 */
bool zpal_uart_transmit_in_progress(zpal_uart_handle_t handle);

/**
 * @brief Get the number of bytes ready for reading.
 *
 * @param[in] handle UART handle.
 * @return Number of bytes in the receive buffer available for reading with
 *         @ref zpal_uart_receive
 */
size_t zpal_uart_get_available(zpal_uart_handle_t handle);

/**
 * @brief Receive available data into buffer using a given UART.
 *
 * @param[in]   handle  UART handle.
 * @param[out]  data    The data buffer to receive into
 * @param[in]   length  the maximum number of bytes we'd like to read
 * @return Number of bytes received into buffer
 */
size_t zpal_uart_receive(zpal_uart_handle_t handle, uint8_t *data, size_t length);

/**
 * @} //zpal-uart
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_UART_H_ */
