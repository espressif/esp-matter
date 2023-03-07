/*******************************************************************************
* @file  rsi_mdnsd_records.h
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

/**
 * Includes
 */
#ifndef RSI_MDNSD_RECORDS_H
#define RSI_MDNSD_RECORDS_H

typedef struct rsi_mdns_txt_rec_g {
  uint8_t *buffer;   // Pointer to data
  uint16_t buflen;   // Length of buffer
  uint16_t datalen;  // Length currently in use
  uint16_t malloced; // Non-zero if buffer was allocated via malloc()
} rsi_mdns_txt_rec_t;

void rsi_mdns_txt_rec_create(rsi_mdns_txt_rec_t *txtRecord, uint16_t bufferLen, void *buffer);
int8_t rsi_mdns_txt_rec_setvalue(rsi_mdns_txt_rec_t *txtRecord, const char *key, uint8_t valueSize, const void *value);
uint8_t *rsi_mdns_txt_rec_search(uint16_t txtLen, const void *txtRecord, const char *key, uint32_t *keylen);
int8_t rsi_mdns_txt_rec_removevalue(rsi_mdns_txt_rec_t *txtRecord, const char *key);
const void *rsi_mdns_txt_get_bytes_ptr(rsi_mdns_txt_rec_t *txtRecord);
const void *rsi_mdns_get_txt_rec_buffer(rsi_mdns_txt_rec_t *txtRecord);
#endif
