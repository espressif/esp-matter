/*******************************************************************************
* @file  rsi_mdnsd_records.c
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

#include <rsi_data_types.h>
#include <rsi_common_apis.h>
#include "rsi_mdnsd_records.h"
#include "string.h"

/*==============================================*/
/**
 * @brief      Create MDNS text record.
 * @param[in]  txtRecord - Pointer to text record
 * @param[in]  bufferLen - Length of buffer that is given by the buffer
 * @param[in]  buffer 	 - Pointer to buffer that will be used for text record
 * @return     void 
 */
/// @private
void rsi_mdns_txt_rec_create(rsi_mdns_txt_rec_t *txtRecord, uint16_t bufferLen, void *buffer)
{
  rsi_mdns_txt_rec_t *txtRec = txtRecord;

  txtRec->buffer   = buffer;
  txtRec->buflen   = buffer ? bufferLen : (uint16_t)0;
  txtRec->datalen  = 0;
  txtRec->malloced = 0;
}

/*==============================================*/
/**
 * @brief      Set value in MDNS text record.
 * @param[in]  txtRecord      - Pointer to text record
 * @param[in]  key            - Pointer to Key
 * @param[in]  valueSize      - Size of value
 * @param[in]  value	      - Pointer to value
 * @return     Zero           - Success \n
 *             Non-Zero Value - Failure
 * 			   
 */
/// @private
int8_t rsi_mdns_txt_rec_setvalue(rsi_mdns_txt_rec_t *txtRecord, const char *key, uint8_t valueSize, const void *value)
{
  uint8_t *start, *p;
  const char *k;
  uint32_t keysize, keyvalsize;
  rsi_mdns_txt_rec_t *txtRec = txtRecord;

  for (k = key; *k; k++)
    if (*k < 0x20 || *k > 0x7E || *k == '=')
      return (RSI_FAILURE);

  keysize = (uint32_t)(k - key);

  keyvalsize = 1 + keysize + (value ? (1 + valueSize) : 0);

  if (keysize < 1 || keyvalsize > 255)
    return (RSI_FAILURE);
  rsi_mdns_txt_rec_removevalue(txtRecord, key);

  start = txtRec->buffer + txtRec->datalen;
  p     = start + 1;
  memcpy(p, key, keysize);
  p += keysize;

  if (value) {
    *p++ = '=';
    memcpy(p, value, valueSize);
    p += valueSize;
  }

  *start = (uint8_t)(p - start - 1);
  txtRec->datalen += p - start;

  return (0);
}

/*==============================================*/
/**
 * @brief       Search in current MDNS text record.
 * @param[in]   txtLen    - Text length
 * @param[in]   txtRecord - Pointer to Text Record
 * @param[in]   key       - pointer to Key
 * @param[in]   keylen    - pointer to Key Length
 * @return     0              -  Success \n
 *             Negative Value - Failure
 */
/// @private
uint8_t *rsi_mdns_txt_rec_search(uint16_t txtLen, const void *txtRecord, const char *key, uint32_t *keylen)
{
  uint8_t *p = (uint8_t *)txtRecord;
  uint8_t *e = p + txtLen;
  *keylen    = (uint32_t)strlen(key);

  while (p < e) {
    uint8_t *x = p;
    p += 1 + p[0];
    if (p <= e && *keylen <= x[0] && !strncasecmp(key, (char *)x + 1, *keylen))
      if (*keylen == x[0] || x[1 + *keylen] == '=')
        return (x);
  }

  return (NULL);
}

/*==============================================*/
/**
 * @brief   	Remove a value from MDNS text record.
 * @param[in]   txtRecord - Pointer to Text Record
 * @param[in]   key       - Pointer to Key
 * @return     0              -  Success \n
 *             Negative Value - Failure
 */
/// @private
int8_t rsi_mdns_txt_rec_removevalue(rsi_mdns_txt_rec_t *txtRecord, const char *key)
{
  uint32_t keylen, itemlen, remainder;
  rsi_mdns_txt_rec_t *txtRec = txtRecord;

  uint8_t *item = rsi_mdns_txt_rec_search(txtRec->datalen, txtRec->buffer, key, &keylen);

  if (!item)
    return (RSI_FAILURE);

  itemlen   = (uint32_t)(1 + item[0]);
  remainder = (uint32_t)((txtRec->buffer + txtRec->datalen) - (item + itemlen));

  // Use memmove because memcpy behavior is undefined for overlapping regions
  memmove(item, item + itemlen, remainder);
  txtRec->datalen -= itemlen;

  return RSI_SUCCESS;
}

// Function to return MDNS text record buffer pointer
/*==============================================*/
/**
 * @brief      Return MDNS text record buffer pointer.
 * @param[in]  txtRecord - Pointer to text record
 * @return     Pointer to text record buffer
 */
/// @private
const void *rsi_mdns_get_txt_rec_buffer(rsi_mdns_txt_rec_t *txtRecord)
{
  rsi_mdns_txt_rec_t *txtRec = txtRecord;

  txtRec->buffer[txtRec->buflen] = '\0';

  return (txtRec->buffer);
}
