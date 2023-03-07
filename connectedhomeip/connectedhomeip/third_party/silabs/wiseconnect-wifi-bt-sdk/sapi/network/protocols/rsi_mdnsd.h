/*******************************************************************************
* @file  rsi_mdnsd.h
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

#ifndef RSI_MDNSD_H
#define RSI_MDNSD_H

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
int32_t rsi_mdnsd_init(uint8_t ip_version, uint16_t ttl, uint8_t *host_name);
#ifndef CHIP_9117
int32_t rsi_mdnsd_register_service(uint16_t port,
                                   uint16_t ttl,
                                   uint8_t more,
                                   uint8_t *service_ptr_name,
                                   uint8_t *service_name,
                                   uint8_t *service_text);
#else
int32_t rsi_mdnsd_register_service(uint16_t port,
                                   uint16_t ttl,
                                   uint8_t *type,
                                   uint8_t *sub_type,
                                   uint8_t *service_name,
                                   uint8_t *service_text);
#endif
int32_t rsi_mdnsd_deinit(void);
#endif
