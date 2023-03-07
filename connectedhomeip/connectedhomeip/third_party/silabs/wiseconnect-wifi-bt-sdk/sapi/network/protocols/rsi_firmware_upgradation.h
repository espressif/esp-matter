/*******************************************************************************
* @file  rsi_firmware_upgradation.h
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

#ifndef RSI_FWUP_H
#define RSI_FWUP_H
/******************************************************
 * *                      Macros
 * ******************************************************/
// Firmware upgrade packet types
#define RSI_FWUP_RPS_HEADER  1
#define RSI_FWUP_RPS_CONTENT 0

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
int32_t rsi_fwup_start(uint8_t *rps_header);
int32_t rsi_fwup_load(uint8_t *content, uint16_t length);
int32_t rsi_ota_firmware_upgradation(uint8_t flags,
                                     uint8_t *server_ip,
                                     uint32_t server_port,
                                     uint16_t chunk_number,
                                     uint16_t timeout,
                                     uint16_t tcp_retry_count,
                                     void (*ota_fw_up_response_handler)(uint16_t status, uint16_t chunk_number));
#endif
