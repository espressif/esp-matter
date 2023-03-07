/*******************************************************************************
* @file  rsi_pop3_client.h
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

#ifndef RSI_POP3_CLIENT_H
#define RSI_POP3_CLIENT_H

#include <stdint.h>
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

// To create the POP3 session
int32_t rsi_pop3_session_create(uint8_t flags,
                                uint8_t *server_ip_address,
                                uint16_t server_port_number,
                                uint8_t auth_type,
                                uint8_t *username,
                                uint8_t *password,
                                void (*rsi_pop3_response_handler)(uint16_t status, uint8_t type, uint8_t *buffer));
int32_t rsi_pop3_session_create_async(uint8_t flags,
                                      uint8_t *server_ip_address,
                                      uint16_t server_port_number,
                                      uint8_t auth_type,
                                      uint8_t *username,
                                      uint8_t *password,
                                      void (*rsi_pop3_client_mail_response_handler)(uint16_t status,
                                                                                    uint8_t type,
                                                                                    const uint8_t *buffer));

// Delete the POP3 session
int32_t rsi_pop3_session_delete(void);

// Get the number of mails count and total size of the mails
int32_t rsi_pop3_get_mail_stats(void);

// Get the mail size of the passed mail index
int32_t rsi_pop3_get_mail_list(uint16_t mail_index);

// Get the mail header and mail body of the passed mail index
int32_t rsi_pop3_retrive_mail(uint16_t mail_index);

// Mail will be marked as deleted on the POP3 server for the passed mail index
int32_t rsi_pop3_mark_mail(uint16_t mail_index);

// Any messages/mails marked as deleted in the current session will be unmarked
int32_t rsi_pop3_unmark_mail(void); /* or rsi_pop3_reset(void);*/

// Get the connection status of the POP3 server
int32_t rsi_pop3_get_server_status(void);

#endif
