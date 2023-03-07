/*******************************************************************************
* @file  rsi_pkt_mgmt.h
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

#ifndef RSI_PKT_MGMT_H
#define RSI_PKT_MGMT_H

#include <stdint.h>
#include <rsi_os.h>
#if ((defined RSI_SDIO_INTERFACE) && (defined CHIP_9117))
#define SIZE_OF_HEADROOM 216
#else
#define SIZE_OF_HEADROOM 0
#endif
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
// driver TX/RX packet structure
typedef struct rsi_pkt_s {
  // next packet pointer
  struct rsi_pkt_s *next;

#if ((defined RSI_SDIO_INTERFACE) && (defined CHIP_9117))
  uint8_t headroom[SIZE_OF_HEADROOM];
#endif

  // host descriptor
  uint8_t desc[16];

  // payload
  uint8_t data[1];
} rsi_pkt_t;

// packet pool structure
typedef struct rsi_pkt_pool_s {
  // Pool total packets count
  uint16_t size;

  // Pool avaialble packets count
  uint16_t avail;

  // Pool pointer
  void **pool;

  rsi_semaphore_handle_t pkt_sem;

} rsi_pkt_pool_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size);
rsi_pkt_t *rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb);
int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt);
uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb);
#endif
