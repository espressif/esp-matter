/*******************************************************************************
* @file  rsi_http_otaf_app.h
* @brief - HTTP OTAF Specific Header File
*******************************************************************************/

#ifndef RSI_HTTP_OTAF_APP
#define RSI_HTTP_OTAF_APP

/******************************************************
 * *                      Includes
 * ******************************************************/
#include "rsi_driver.h"

/******************************************************
 * *                      Declarations
 * ******************************************************/
int32_t rsi_http_fw_update(uint8_t flags,
                           uint8_t *,
                           uint16_t,
                           uint8_t *,
                           uint8_t *,
                           uint8_t *,
                           uint8_t *,
                           uint8_t *,
                           void (*http_otaf_response_handler)(uint16_t, const uint8_t *));

void http_otaf_response_handler(uint16_t, const uint8_t *);
int32_t rsi_http_response_status(int32_t *);

#endif //RSI_HTTP_OTAF_APP
