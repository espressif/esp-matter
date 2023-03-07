/*******************************************************************************
* @file  rsi_socket_rom.c
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

#include "rsi_driver.h"
#ifndef ROM_WIRELESS

/** @addtogroup NETWORK5
* @{
*/
/*==============================================*/
/**
 * @brief       Get the application socket descriptor from module socket descriptor. \n
 *              This is a non-blocking API.
 * @param[in]   global_cb_p    - Pointer to the global control block
 * @param[in]   sock_id        - Module's socket descriptor
 * @return      Positive value - Application index \n
 *              Negative value - If index is not found
 */
/// @private
int32_t ROM_WL_rsi_get_application_socket_descriptor(global_cb_t *global_cb_p, int32_t sock_id)
{
  int i;

  rsi_socket_info_t *rsi_socket_pool = global_cb_p->rsi_socket_pool;

  for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
    if (rsi_socket_pool[i].sock_id == sock_id) {
      break;
    }
  }

  if (i >= NUMBER_OF_SOCKETS) {
    return -1;
  }

  return i;
}

/*==============================================*/
/**
 * @brief       Get socket descriptor from port_number. \n
 * 		This is a non-blocking API.
 * @param[in]   global_cb_p    - Pointer to the global control block
 * @param[in]   port_number    - Port number
 * @return 	Positive value - Socket descriptor \n
 *         	Negative value - If socket is not found
 *             
 */
/// @private
int32_t ROM_WL_rsi_get_primary_socket_id(global_cb_t *global_cb_p, uint16_t port_number)
{
  int i;

  rsi_socket_info_t *rsi_socket_pool = global_cb_p->rsi_socket_pool;

  for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
    if ((rsi_socket_pool[i].source_port == port_number)
        && (rsi_socket_pool[i].ltcp_socket_type == RSI_LTCP_PRIMARY_SOCKET)) {
      break;
    }
  }

  if (i >= NUMBER_OF_SOCKETS) {
    return -1;
  }
  return i;
}

/*==============================================*/
/**
 * @brief       Calculate number of buffers required for the data packet. \n
 * 		This is a non-blocking API.
 * @param[in]   global_cb_p - Pointer to the global control block
 * @param[in]   type 		    - Type of socket to create
 * @param[in]   length 		    - Length of the message
 * @return	The number of buffers required for a data packet
 */
/// @private
uint8_t ROM_WL_calculate_buffers_required(global_cb_t *global_cb_p, uint8_t type, uint16_t length)
{
  // Added to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);

  uint8_t header_size, buffers_required = 1;
  uint16_t first_buffer_available_size, remaining_length;

  // Calculate header size including extra 2 bytes based on the prototype
  if ((type & 0xF) == SOCK_STREAM) {
    header_size = 56;
  } else {
    header_size = 44;
  }

  // Increase header size by 20 for IPv6 case
  if ((type >> 4) == AF_INET6) {
    header_size += 20;
  }

  remaining_length = length;

  first_buffer_available_size = (512 - header_size - 252);

  if (length <= first_buffer_available_size) {
    return 1;
  }

  remaining_length -= first_buffer_available_size;

  do {
    buffers_required++;
    if (remaining_length > 512) {
      remaining_length -= 512;
    } else {
      remaining_length = 0;
    }

  } while (remaining_length);

  return buffers_required;
}
/*==============================================*/
/**
 * @brief	Caluculate the msg length sent using available buffers. \n
 * 		This is a non-blocking API.
 * @param[in]	global_cb_p  - Pointer to the global control block
 * @param[in]	type		- Type of the socket stream
 * @param[in] 	buffers		- Available buffers
 * @return	Length       - Length available in the buffer for msg.
 */
/// @private
uint16_t ROM_WL_calculate_length_to_send(global_cb_t *global_cb_p, uint8_t type, uint8_t buffers)
{
  // Added to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  uint8_t header_size;
  uint16_t length;

  // Calculate header size including extra 2 bytes based on the prototype
  if ((type & 0xF) == SOCK_STREAM) {
    header_size = 56;
  } else {
    header_size = 44;
  }

  // Increase header size by 20 for IPv6 case
  if ((type >> 4) == AF_INET6) {
    header_size += 20;
  }

  length = (512 - header_size - 252);

  if (buffers == 1) {
    return length;
  }

  buffers--;

  while (buffers) {
    length += 512;
    buffers--;
  }

  return length;
}
#endif
/** @} */
