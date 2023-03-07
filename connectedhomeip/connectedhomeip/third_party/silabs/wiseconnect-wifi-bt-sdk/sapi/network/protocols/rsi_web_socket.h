/*******************************************************************************
* @file  rsi_web_socket.h
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

#ifndef RSI_WEB_SOCKET_H
#define RSI_WEB_SOCKET_H

/******************************************************
 * *                      Macros
 * ******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_web_socket_create(int8_t flags,
                              uint8_t *server_ip_addr,
                              uint16_t server_port,
                              uint16_t device_port,
                              uint8_t *webs_resource_name,
                              uint8_t *webs_host_name,
                              int32_t *socket_id,
                              void (*web_socket_data_receive_notify_callback)(uint32_t sock_no,
                                                                              uint8_t *buffer,
                                                                              uint32_t length));
int32_t rsi_web_socket_send_async(int32_t sockID, uint8_t opcode, int8_t *msg, int32_t msg_length);
int32_t rsi_web_socket_close(int32_t sockID);
#endif
