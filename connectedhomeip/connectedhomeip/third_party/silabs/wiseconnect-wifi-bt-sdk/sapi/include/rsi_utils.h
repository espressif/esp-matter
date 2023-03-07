/*******************************************************************************
* @file  rsi_utils.h
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

#ifndef RSI_UTILS_H
#define RSI_UTILS_H

#include <stdint.h>
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef BIT
#define BIT(a) ((uint32_t)1U << a)
#endif

//#define RSI_MIN(x, y) ((x) > (y) ? (y) : (x)) //This statement is modified to avoid compilation warning
#define RSI_MIN(x, y) ((int32_t)(x) > (int32_t)(y) ? (int32_t)(y) : (int32_t)(x))

#ifndef NULL
#define NULL 0
#endif

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
void rsi_uint32_to_4bytes(uint8_t *dBuf, uint32_t val);
void rsi_uint16_to_2bytes(uint8_t *dBuf, uint16_t val);
uint16_t rsi_bytes2R_to_uint16(uint8_t *dBuf);
uint32_t rsi_bytes4R_to_uint32(uint8_t *dBuf);
uint8_t multicast_mac_hash(uint8_t *mac);
uint8_t lmac_crc8_c(uint8_t crc8_din, uint8_t crc8_state, uint8_t end);
uint8_t *rsi_ascii_dev_address_to_6bytes_rev(uint8_t *hex_addr, int8_t *ascii_mac_address);
uint8_t *rsi_6byte_dev_address_to_ascii(uint8_t *ascii_mac_address, uint8_t *hex_addr);
uint8_t convert_lower_case_to_upper_case(uint8_t lwrcase);
void string2array(uint8_t *dst, uint8_t *src, uint32_t length);
int32_t rsi_atoi(const int8_t *str);
void rsi_ascii_dot_address_to_4bytes(uint8_t *hexAddr, int8_t *asciiDotAddress);
void rsi_ascii_mac_address_to_6bytes(uint8_t *hexAddr, int8_t *asciiMacAddress);
uint64_t ip_to_reverse_hex(char *ip);

int8_t rsi_ascii_hex2num(int8_t ascii_hex_in);
int8_t rsi_char_hex2dec(int8_t *cBuf);
int8_t hex_to_ascii(uint8_t hex_num);
uint8_t *rsi_itoa(uint32_t val, uint8_t *str);
int8_t asciihex_2_num(int8_t ascii_hex_in);
int8_t rsi_charhex_2_dec(int8_t *cBuf);
uint32_t rsi_ntohl(uint32_t a);
#endif
