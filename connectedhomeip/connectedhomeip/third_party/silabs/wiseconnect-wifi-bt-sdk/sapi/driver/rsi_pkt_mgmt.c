/*******************************************************************************
* @file  rsi_pkt_mgmt.c
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

/*
  Include files
  */
#include "rsi_driver.h"
/** @addtogroup DRIVER5
* @{
*/
/*==============================================*/
/**
 * @fn         int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)
 * @brief      Initialize/create packet pool from the provided buffer
 * @param[in]  pool_cb    - packet pool pointer which needs to be initialized  \n 
 * @param[in]  buffer     - buffer pointer to create pool \n 
 * @param[in]  total_size - size of pool to create \n 
 * @param[in]  pkt_size   - size of each packet in pool \n 
 * @return     0              - Success \n
 *             Negative Value - Failure
 *
 */
///@private
int32_t rsi_pkt_pool_init(rsi_pkt_pool_t *pool_cb, uint8_t *buffer, uint32_t total_size, uint32_t pkt_size)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_pkt_pool_init(global_cb_p, pool_cb, buffer, total_size, pkt_size);
#else
  return api_wl->rsi_pkt_pool_init(global_cb_p, pool_cb, buffer, total_size, pkt_size);
#endif
}

/*==============================================*/
/**
 * @fn          rsi_pkt_t *rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb)
 * @brief       Allocate packet from a given packet pool 
 * @param[in]   pool_cb - packet pool pointer from which packet needs to be allocated  
 * @return      0              - Success \n
 *              Positive Value - Failure
 *
 */
///@private
rsi_pkt_t *rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_pkt_alloc(global_cb_p, pool_cb);
#else
  return api_wl->rsi_pkt_alloc(global_cb_p, pool_cb);
#endif
}

/*==============================================*/
/**
 * @fn         int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt)
 * @brief      Free the packet
 * @param[in]  pool_cb - packet pool to which packet needs to be freed  
 * @param[in]  pkt     - packet pointer which needs to be freed 
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 *
 */
///@private
int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_pkt_free(global_cb_p, pool_cb, pkt);
#else
  return api_wl->rsi_pkt_free(global_cb_p, pool_cb, pkt);
#endif
}

/*==============================================*/
/**
 * @fn         uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb)
 * @brief      Return number of available packets in pool
 * @param[in]  pool_cb - pool pointer to check available packet count 
 * @return     >=0     - Number of packets in pool
 *
 */
///@private
uint32_t rsi_is_pkt_available(rsi_pkt_pool_t *pool_cb)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_is_pkt_available(global_cb_p, pool_cb);
#else
  return api_wl->rsi_is_pkt_available(global_cb_p, pool_cb);
#endif
}
/** @} */
