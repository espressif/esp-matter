/***************************************************************************//**
 * @file
 * @brief USB Device Endpoint Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 ***********************************************************************************************"*******/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sl_bit.h"
#include "sl_status.h"

#include "em_core.h"

#include "sl_usbd_core.h"
#include "sl_usbd_core_config.h"

#include "sli_usbd_core.h"
#include "sli_usbd_driver.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SLI_USBD_ENDPOINT_ADDR_CTRL_OUT                          0x00u
#define  SLI_USBD_ENDPOINT_ADDR_CTRL_IN                           0x80u

#define  SLI_USBD_URB_FLAG_XFER_END                 0x01u // BIT_00: Flag indicating if xfer requires a ZLP to complete.
#define  SLI_USBD_URB_FLAG_EXTRA_URB                0x02u // BIT_01  Flag indicating if the URB is an 'extra' URB.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static sl_status_t usbd_core_endpoint_process_async_rx(sli_usbd_endpoint_t *p_ep,
                                                       sli_usbd_urb_t      *p_urb,
                                                       uint8_t             *p_buf_cur,
                                                       uint32_t            len);

static sl_status_t usbd_core_endpoint_process_async_tx(sli_usbd_endpoint_t *p_ep,
                                                       sli_usbd_urb_t      *p_urb,
                                                       uint8_t             *p_buf_cur,
                                                       uint32_t            len);

static sl_status_t usbd_core_endpoint_read(sli_usbd_endpoint_t      *p_ep,
                                           void                     *p_buf,
                                           uint32_t                 buf_len,
                                           sl_usbd_async_function_t async_fnct,
                                           void                     *p_async_arg,
                                           uint16_t                 timeout_ms,
                                           uint32_t                 *p_xfer_len);

static sl_status_t usbd_core_endpoint_write(sli_usbd_endpoint_t      *p_ep,
                                            void                     *p_buf,
                                            uint32_t                 buf_len,
                                            sl_usbd_async_function_t async_fnct,
                                            void                     *p_async_arg,
                                            uint16_t                 timeout_ms,
                                            bool                     end,
                                            uint32_t                 *p_xfer_len);

static sl_status_t usbd_core_abort_urb(sli_usbd_endpoint_t *p_ep,
                                       sli_usbd_urb_t      **p_urb_ptr);

static void usbd_core_complete_async_urb(sli_usbd_endpoint_t *p_ep,
                                         sli_usbd_urb_t      **p_urb_ptr,
                                         sl_status_t         status);

static void usbd_core_end_async_urb(sli_usbd_endpoint_t *p_ep,
                                    sli_usbd_urb_t      *p_urb_head);

static void usbd_core_free_urb(sli_usbd_endpoint_t *p_ep,
                               sli_usbd_urb_t      *p_urb);

static sl_status_t usbd_core_get_urb(sli_usbd_endpoint_t *p_ep,
                                     sli_usbd_urb_t      **p_urb_ptr);

static void usbd_core_queue_urb(sli_usbd_endpoint_t *p_ep,
                                sli_usbd_urb_t      *p_urb);

static void usbd_core_dequeue_urb(sli_usbd_endpoint_t *p_ep);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       BULK TRANSFER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Receives data on Bulk OUT endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_bulk_sync(uint8_t  ep_addr,
                                        void     *p_buf,
                                        uint32_t buf_len,
                                        uint16_t timeout_ms,
                                        uint32_t *p_xfer_len)
{
  sli_usbd_endpoint_t        *p_ep;
  sl_usbd_device_state_t state;
  uint8_t     ep_phy_nbr;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(bulk_rx_sync_exec_nbr);

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_BULK)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_OUT)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  // Call generic EP rx fnct.
  status = usbd_core_endpoint_read(p_ep,
                                   p_buf,
                                   buf_len,
                                   NULL,
                                   NULL,
                                   timeout_ms,
                                   p_xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(bulk_rx_sync_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Receives data on Bulk OUT endpoint asynchronously
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_bulk_async(uint8_t                  ep_addr,
                                         void                     *p_buf,
                                         uint32_t                 buf_len,
                                         sl_usbd_async_function_t async_fnct,
                                         void                     *p_async_arg)
{
  sli_usbd_endpoint_t        *p_ep;
  sl_usbd_device_state_t state;
  uint8_t     ep_phy_nbr;
  uint32_t     xfer_len;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(bulk_rx_async_exec_nbr);

  if (async_fnct == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_BULK)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_OUT)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_HANDLE;
  }

  // Call generic EP rx fnct.
  status = usbd_core_endpoint_read(p_ep,
                                   p_buf,
                                   buf_len,
                                   async_fnct,
                                   p_async_arg,
                                   0u,
                                   &xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(bulk_rx_async_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Sends data on a Bulk IN endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_bulk_sync(uint8_t     ep_addr,
                                         void        *p_buf,
                                         uint32_t    buf_len,
                                         uint16_t    timeout_ms,
                                         bool        end,
                                         uint32_t    *p_xfer_len)
{
  sli_usbd_endpoint_t        *p_ep;
  sl_usbd_device_state_t state;
  uint8_t     ep_phy_nbr;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(bulk_tx_sync_exec_nbr);

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_BULK)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_IN)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  // Call generic EP tx fnct.
  status = usbd_core_endpoint_write(p_ep,
                                    p_buf,
                                    buf_len,
                                    NULL,
                                    NULL,
                                    timeout_ms,
                                    end,
                                    p_xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(bulk_tx_sync_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Sends data on the Bulk IN endpoint asynchronously
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_bulk_async(uint8_t                  ep_addr,
                                          void                     *p_buf,
                                          uint32_t                 buf_len,
                                          sl_usbd_async_function_t async_fnct,
                                          void                     *p_async_arg,
                                          bool                     end)
{
  sli_usbd_endpoint_t        *p_ep;
  sl_usbd_device_state_t state;
  uint8_t     ep_phy_nbr;
  uint32_t     xfer_len;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(bulk_tx_async_exec_nbr);

  if (async_fnct == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_BULK)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_IN)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_HANDLE;
  }

  status = usbd_core_endpoint_write(p_ep,
                                    p_buf,
                                    buf_len,
                                    async_fnct,
                                    p_async_arg,
                                    0u,
                                    end,
                                    &xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(bulk_tx_async_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       INTERRUPT TRANSFER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Receives data on the Interrupt OUT endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_interrupt_sync(uint8_t  ep_addr,
                                             void     *p_buf,
                                             uint32_t buf_len,
                                             uint16_t timeout_ms,
                                             uint32_t *p_xfer_len)
{
  sli_usbd_endpoint_t        *p_ep;
  sl_usbd_device_state_t state;
  uint8_t     ep_phy_nbr;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(interrupt_rx_sync_exec_nbr);

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }
  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_INTR)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_OUT)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = usbd_core_endpoint_read(p_ep,
                                   p_buf,
                                   buf_len,
                                   NULL,
                                   NULL,
                                   timeout_ms,
                                   p_xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(interrupt_rx_sync_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Receives data on Interrupt OUT endpoint asynchronously
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_interrupt_async(uint8_t                  ep_addr,
                                              void                     *p_buf,
                                              uint32_t                 buf_len,
                                              sl_usbd_async_function_t async_fnct,
                                              void                     *p_async_arg)
{
  sli_usbd_endpoint_t        *p_ep;
  uint8_t     ep_phy_nbr;
  uint32_t     xfer_len;
  sl_usbd_device_state_t state;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(interrupt_rx_async_exec_nbr);

  if (async_fnct == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_INTR)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_OUT)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_HANDLE;
  }

  // Call generic EP rx fnct.
  status = usbd_core_endpoint_read(p_ep,
                                   p_buf,
                                   buf_len,
                                   async_fnct,
                                   p_async_arg,
                                   0u,
                                   &xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(interrupt_rx_async_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Sends data on Interrupt IN endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_interrupt_sync(uint8_t   ep_addr,
                                              void      *p_buf,
                                              uint32_t  buf_len,
                                              uint16_t  timeout_ms,
                                              bool      end,
                                              uint32_t  *p_xfer_len)
{
  sli_usbd_endpoint_t        *p_ep;
  uint8_t     ep_phy_nbr;
  sl_usbd_device_state_t state;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(interrupt_tx_sync_exec_nbr);

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_INTR)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_IN)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = usbd_core_endpoint_write(p_ep,
                                    p_buf,
                                    buf_len,
                                    NULL,
                                    NULL,
                                    timeout_ms,
                                    end,
                                    p_xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(interrupt_tx_sync_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Sends data on the Interrupt IN endpoint asynchronously
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_interrupt_async(uint8_t                  ep_addr,
                                               void                     *p_buf,
                                               uint32_t                 buf_len,
                                               sl_usbd_async_function_t async_fnct,
                                               void                     *p_async_arg,
                                               bool                     end)
{
  sli_usbd_endpoint_t        *p_ep;
  uint8_t     ep_phy_nbr;
  uint32_t     xfer_len;
  sl_usbd_device_state_t state;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(interrupt_tx_async_exec_nbr);

  if (async_fnct == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }
  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_INTR)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_IN)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_HANDLE;
  }

  status = usbd_core_endpoint_write(p_ep,
                                    p_buf,
                                    buf_len,
                                    async_fnct,
                                    p_async_arg,
                                    0u,
                                    end,
                                    &xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(interrupt_tx_async_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Receive data on an isochronous OUT endpoint asynchronously
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_read_isochronous_async(uint8_t                   ep_addr,
                                                void                      *p_buf,
                                                uint32_t                  buf_len,
                                                sl_usbd_async_function_t  async_fnct,
                                                void                      *p_async_arg)
{
  sli_usbd_endpoint_t        *p_ep;
  sl_usbd_device_state_t state;
  uint8_t     ep_phy_nbr;
  uint32_t     xfer_len;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(isochronous_rx_async_exec_nbr);

  if (async_fnct == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_ISOC)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK)  != SL_USBD_ENDPOINT_DIR_OUT)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_HANDLE;
  }

  // Call generic EP rx fnct.
  status = usbd_core_endpoint_read(p_ep,
                                   p_buf,
                                   buf_len,
                                   async_fnct,
                                   p_async_arg,
                                   0u,
                                   &xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(isochronous_rx_async_success_nbr, (status == SL_STATUS_OK));

  return status;
}
#endif

/****************************************************************************************************//**
 * Sends data on an isochronous IN endpoint asynchronously
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_write_isochronous_async(uint8_t                  ep_addr,
                                                 void                     *p_buf,
                                                 uint32_t                 buf_len,
                                                 sl_usbd_async_function_t async_fnct,
                                                 void                     *p_async_arg)
{
  sli_usbd_endpoint_t        *p_ep;
  sl_usbd_device_state_t state;
  uint8_t     ep_phy_nbr;
  uint32_t     xfer_len;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(isochronous_tx_async_exec_nbr);

  if (async_fnct == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);

  // EP transfers are ONLY allowed in config'd state.
  if (state != SL_USBD_DEVICE_STATE_CONFIGURED) {
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if (((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_ISOC)
      || ((ep_addr   & SL_USBD_ENDPOINT_DIR_MASK) != SL_USBD_ENDPOINT_DIR_IN)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_HANDLE;
  }

  status = usbd_core_endpoint_write(p_ep,
                                    p_buf,
                                    buf_len,
                                    async_fnct,
                                    p_async_arg,
                                    0u,
                                    false,
                                    &xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(isochronous_tx_async_success_nbr, (status == SL_STATUS_OK));

  return status;
}
#endif

/****************************************************************************************************//**
 * Handles status stage from host on control (EP0) IN endpoint
 *******************************************************************************************************/
sl_status_t sli_usbd_core_get_control_tx_status(uint16_t timeout_ms)
{
  sl_usbd_device_state_t state;
  sl_status_t status;

  SLI_USBD_DBG_STATS_DEV_INC(ctrl_tx_status_exec_nbr);

  status = sl_usbd_core_get_device_state(&state);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((state != SL_USBD_DEVICE_STATE_DEFAULT)
      && (state != SL_USBD_DEVICE_STATE_ADDRESSED)
      && (state != SL_USBD_DEVICE_STATE_CONFIGURED)) {
    return SL_STATUS_INVALID_STATE;
  }

  status = sl_usbd_core_endpoint_write_zlp(SLI_USBD_ENDPOINT_ADDR_CTRL_IN, timeout_ms);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(ctrl_tx_status_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Receives data on the Control OUT endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_control_sync(void       *p_buf,
                                           uint32_t   buf_len,
                                           uint16_t   timeout_ms,
                                           uint32_t   *p_xfer_len)
{
  sli_usbd_endpoint_t        *p_ep;
  uint8_t     ep_phy_nbr;
  sl_usbd_device_state_t state;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(ctrl_rx_sync_exec_nbr);

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);
  if ((state != SL_USBD_DEVICE_STATE_DEFAULT)
      && (state != SL_USBD_DEVICE_STATE_ADDRESSED)
      && (state != SL_USBD_DEVICE_STATE_CONFIGURED)) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SLI_USBD_ENDPOINT_ADDR_CTRL_OUT);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_CTRL) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = usbd_core_endpoint_read(p_ep,
                                   p_buf,
                                   buf_len,
                                   NULL,
                                   NULL,
                                   timeout_ms,
                                   p_xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(ctrl_rx_sync_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Sends data on the Control IN endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_control_sync(void        *p_buf,
                                            uint32_t    buf_len,
                                            uint16_t    timeout_ms,
                                            bool        end,
                                            uint32_t    *p_xfer_len)
{
  sli_usbd_endpoint_t        *p_ep;
  uint8_t     ep_phy_nbr;
  sl_usbd_device_state_t state;
  sl_status_t      status;

  SLI_USBD_DBG_STATS_DEV_INC(ctrl_tx_sync_exec_nbr);

  if (p_xfer_len == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sl_usbd_core_get_device_state(&state);
  if ((state != SL_USBD_DEVICE_STATE_DEFAULT)
      && (state != SL_USBD_DEVICE_STATE_ADDRESSED)
      && (state != SL_USBD_DEVICE_STATE_CONFIGURED)) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  if (buf_len == 0u) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_PARAMETER;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SLI_USBD_ENDPOINT_ADDR_CTRL_IN);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_STATE;
  }

  // Chk EP attrib.
  if ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_CTRL) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    *p_xfer_len = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  status = usbd_core_endpoint_write(p_ep,
                                    p_buf,
                                    buf_len,
                                    NULL,
                                    NULL,
                                    timeout_ms,
                                    end,
                                    p_xfer_len);

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(ctrl_tx_sync_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/****************************************************************************************************//**
 * Handles the status stage from the host on a control (EP0) OUT endpoint
 *******************************************************************************************************/
sl_status_t sli_usbd_core_get_control_rx_status(uint16_t timeout_ms)
{
  sl_usbd_device_state_t state;
  sl_status_t status;

  SLI_USBD_DBG_STATS_DEV_INC(ctrl_rx_status_exec_nbr);

  status = sl_usbd_core_get_device_state(&state);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((state != SL_USBD_DEVICE_STATE_DEFAULT)
      && (state != SL_USBD_DEVICE_STATE_ADDRESSED)
      && (state != SL_USBD_DEVICE_STATE_CONFIGURED)) {
    return SL_STATUS_INVALID_STATE;
  }

  status = sl_usbd_core_endpoint_read_zlp(SLI_USBD_ENDPOINT_ADDR_CTRL_OUT, timeout_ms);

  SLI_USBD_DBG_STATS_DEV_INC_IF_TRUE(ctrl_rx_status_success_nbr, (status == SL_STATUS_OK));

  return status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       CONTROL ENDPOINT FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Opens control endpoints
 *******************************************************************************************************/
sl_status_t sli_usbd_core_open_control_endpoint(uint16_t max_pkt_size)
{
  sl_status_t status;

  status = sli_usbd_core_open_endpoint(SLI_USBD_ENDPOINT_ADDR_CTRL_IN,
                                       max_pkt_size,
                                       SL_USBD_ENDPOINT_TYPE_CTRL,
                                       0u);

  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sli_usbd_core_open_endpoint(SLI_USBD_ENDPOINT_ADDR_CTRL_OUT,
                                       max_pkt_size,
                                       SL_USBD_ENDPOINT_TYPE_CTRL,
                                       0u);

  if (status != SL_STATUS_OK) {
    sli_usbd_core_close_endpoint(SLI_USBD_ENDPOINT_ADDR_CTRL_IN);
    return status;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Close control endpoint
 *******************************************************************************************************/
sl_status_t sli_usbd_core_close_control_endpoint(void)
{
  sl_status_t status_in;
  sl_status_t status_out;

  status_in = sli_usbd_core_close_endpoint(SLI_USBD_ENDPOINT_ADDR_CTRL_IN);
  status_out = sli_usbd_core_close_endpoint(SLI_USBD_ENDPOINT_ADDR_CTRL_OUT);

  if (status_in != SL_STATUS_OK) {
    return status_in;
  } else {
    return status_out;
  }
}

/****************************************************************************************************//**
 * Stall control endpoint
 *******************************************************************************************************/
sl_status_t sli_usbd_core_stall_control_endpoint()
{
  sli_usbd_endpoint_t      *p_ep_out;
  sli_usbd_endpoint_t      *p_ep_in;
  uint8_t   ep_phy_nbr;
  sl_status_t    status_in;
  sl_status_t    status_out;
  sl_status_t    status;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SLI_USBD_ENDPOINT_ADDR_CTRL_OUT);
  p_ep_out = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SLI_USBD_ENDPOINT_ADDR_CTRL_IN);
  p_ep_in = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if ((p_ep_out == NULL) || (p_ep_in == NULL)) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep_out->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep_in->index);

  if (status != SL_STATUS_OK) {
    sli_usbd_core_os_release_endpoint_lock(p_ep_out->index);
    return status;
  }

  if ((p_ep_out->state == SLI_USBD_ENDPOINT_STATE_OPEN) && (p_ep_in->state == SLI_USBD_ENDPOINT_STATE_OPEN)) {
    status_in = sli_usbd_driver_stall_endpoint(SLI_USBD_ENDPOINT_ADDR_CTRL_IN, 1u);
    status_out = sli_usbd_driver_stall_endpoint(SLI_USBD_ENDPOINT_ADDR_CTRL_OUT, 1u);
    if ((status_in != SL_STATUS_OK) || (status_out != SL_STATUS_OK)) {
      status = SL_STATUS_FAIL;
    } else {
      status = SL_STATUS_OK;
    }
  } else {
    status = SL_STATUS_INVALID_STATE;
  }

  sli_usbd_core_os_release_endpoint_lock(p_ep_in->index);
  sli_usbd_core_os_release_endpoint_lock(p_ep_out->index);

  return status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       GENERAL ENDPOINT FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initialize endpoint structures
 *******************************************************************************************************/
sl_status_t sli_usbd_core_init_endpoint(void)
{
  uint8_t ep_ix;
  uint16_t urb_ix;
  sli_usbd_endpoint_t    *p_ep;
  sli_usbd_urb_t   *p_urb;

#if (USBD_CFG_URB_EXTRA_EN == 1)
  usbd_ptr->urb_extra_avail_count = SL_USBD_EXTRA_URB_QUANTITY;
#endif

  for (ep_ix = 0u; ep_ix < SL_USBD_ENDPOINT_MAX_NBR; ep_ix++) {
    if (ep_ix < SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
      p_ep = &usbd_ptr->endpoint_table[ep_ix];
      p_ep->address = SL_USBD_ENDPOINT_ADDR_NONE;
      p_ep->attrib = 0x00u;
      p_ep->max_pkt_size = 0u;
      p_ep->interval = 0u;
      p_ep->index = 0u;
#if (USBD_CFG_URB_EXTRA_EN == 1)
      p_ep->urb_main_avail = true;
#endif
      p_ep->urb_head_ptr = NULL;
      p_ep->urb_tail_ptr = NULL;

      SLI_USBD_DBG_STATS_EP_RESET(ep_ix);
    }
    usbd_ptr->endpoint_table_ptrs[ep_ix] = NULL;
  }

  usbd_ptr->urb_table_ptr = &usbd_ptr->urb_table[0u];

  for (urb_ix = 0u; urb_ix < SL_USBD_URB_MAX_NBR; urb_ix++) {
    p_urb = &usbd_ptr->urb_table[urb_ix];
    p_urb->buf_ptr = NULL;
    p_urb->buf_len = 0u;
    p_urb->transfer_len = 0u;
    p_urb->next_xfer_len = 0u;
    p_urb->flags = 0u;
    p_urb->state = SLI_USBD_URB_STATE_IDLE;
    p_urb->async_function = NULL;
    p_urb->async_function_arg = NULL;
    p_urb->status = SL_STATUS_OK;
    if (urb_ix < (SL_USBD_URB_MAX_NBR - 1u)) {
      p_urb->next_ptr = &usbd_ptr->urb_table[urb_ix + 1];
    } else {
      p_urb->next_ptr = NULL;
    }
  }

  usbd_ptr->endpoint_open_ctrl = 0u;
  usbd_ptr->endpoint_open_bitmap = 0xFFFFFFFFu;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Read/write data asynchronously from/to non-control endpoints
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_transfer_async(uint8_t      ep_addr,
                                                 sl_status_t  xfer_err)
{
  uint8_t   ep_phy_nbr;
  sli_usbd_endpoint_t      *p_ep;
  bool  ep_dir_in;
  sl_status_t    local_status;
  sli_usbd_urb_t     *p_urb;
  sli_usbd_urb_t     *p_urb_cmpl;
  uint8_t   *p_buf_cur;
  uint32_t   xfer_len;
  uint32_t   xfer_rem;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  ep_dir_in = SL_USBD_ENDPOINT_IS_IN(p_ep->address);

  local_status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (local_status != SL_STATUS_OK) {
    return local_status;
  }

  if (p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_NONE) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }

  p_urb = p_ep->urb_head_ptr;

  if (p_urb == NULL) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    SLI_USBD_LOG_DBG(("USBD_EP_Process(): no URB to process for endpoint addr: 0x", (X)ep_addr));
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((p_urb->state == SLI_USBD_URB_STATE_IDLE) || (p_urb->state == SLI_USBD_URB_STATE_TRANSFER_SYNC)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    SLI_USBD_LOG_ERR(("USBD_EP_Process(): incorrect URB state for endpoint addr: 0x", (X)ep_addr));
    return SL_STATUS_INVALID_STATE;
  }

  p_urb_cmpl = NULL;
  if (xfer_err == SL_STATUS_OK) {
    // See Note #1.
    xfer_rem = p_urb->buf_len - p_urb->transfer_len;
    p_buf_cur = &p_urb->buf_ptr[p_urb->transfer_len];

    if (ep_dir_in == true) {
      // In Transfer
      // Another transaction must be done.
      if (xfer_rem > 0u) {
        local_status = usbd_core_endpoint_process_async_tx(p_ep,
                                                           p_urb,
                                                           p_buf_cur,
                                                           xfer_rem);
        if (local_status != SL_STATUS_OK) {
          usbd_core_complete_async_urb(p_ep, &p_urb_cmpl, local_status);
        }
      } else if ((SL_IS_BIT_SET(p_urb->flags, SLI_USBD_URB_FLAG_XFER_END) == true)
                 && (p_urb->transfer_len % p_ep->max_pkt_size == 0u)
                 && (p_urb->transfer_len != 0u)) {
        // TODO This case should be tested more thoroughly.
        // Send ZLP if needed, at end of xfer.
        SL_CLEAR_BIT(p_urb->flags, (uint8_t)SLI_USBD_URB_FLAG_XFER_END);

        SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_zlp_nbr);

        local_status = sli_usbd_driver_endpoint_tx_zlp(p_ep->address);
        if (local_status != SL_STATUS_OK) {
          usbd_core_complete_async_urb(p_ep, &p_urb_cmpl, local_status);
        }
        SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, driver_tx_zlp_nbr, (local_status == SL_STATUS_OK));
      } else {
        // Xfer is completed.
        usbd_core_complete_async_urb(p_ep, &p_urb_cmpl, SL_STATUS_OK);
      }
    } else {
      // Out Transfer
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_nbr);

      local_status = sli_usbd_driver_endpoint_rx(p_ep->address,
                                                 p_buf_cur,
                                                 p_urb->next_xfer_len,
                                                 &xfer_len);
      if (local_status != SL_STATUS_OK) {
        usbd_core_complete_async_urb(p_ep, &p_urb_cmpl, local_status);
      } else {
        SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_success_nbr);

        p_urb->transfer_len += xfer_len;

        if ((xfer_len == 0u)                                // Rx'd a ZLP.
            || (xfer_len < p_urb->next_xfer_len)            // Rx'd a short pkt (see Note #2).
            || (p_urb->transfer_len == p_urb->buf_len)) {   // All bytes rx'd.
          // Xfer finished.
          usbd_core_complete_async_urb(p_ep, &p_urb_cmpl, SL_STATUS_OK);
        } else {
          // Xfer not finished.
          p_buf_cur = &p_urb->buf_ptr[p_urb->transfer_len];
          xfer_len = p_urb->buf_len - p_urb->transfer_len;

          local_status = usbd_core_endpoint_process_async_rx(p_ep,
                                                             p_urb,
                                                             p_buf_cur,
                                                             xfer_len);
          if (local_status != SL_STATUS_OK) {
            usbd_core_complete_async_urb(p_ep, &p_urb_cmpl, local_status);
          }
        }
      }
    }
  } else {
    usbd_core_complete_async_urb(p_ep, &p_urb_cmpl, xfer_err);
  }

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  if (p_urb_cmpl != NULL) {
    // Execute callback and free aborted URB(s), if any.
    usbd_core_end_async_urb(p_ep, p_urb_cmpl);
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Open non-control endpoint
 *******************************************************************************************************/
sl_status_t sli_usbd_core_open_endpoint(uint8_t  ep_addr,
                                        uint16_t max_pkt_size,
                                        uint8_t  attrib,
                                        uint8_t  interval)
{
  sli_usbd_endpoint_t      *p_ep;
  uint8_t   ep_bit;
  uint8_t   ep_ix;
  uint8_t   ep_phy_nbr;
  uint8_t   transaction_frame;
  sl_status_t    status;
  CORE_DECLARE_IRQ_STATE;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep != NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  CORE_ENTER_ATOMIC();
#if (RTOS_ARG_CHK_EXT_EN == 1)
  if (usbd_ptr->endpoint_open_ctrl >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_NOT_AVAILABLE;
  }
#endif

  ep_bit = (uint8_t)(SL_USBD_ENDPOINT_MAX_NBR - 1u - __CLZ(usbd_ptr->endpoint_open_bitmap));
  SL_CLEAR_BIT(usbd_ptr->endpoint_open_bitmap, SLI_USBD_SINGLE_BIT_MASK_32(ep_bit));
  usbd_ptr->endpoint_open_ctrl++;
  CORE_EXIT_ATOMIC();

  ep_ix = SL_USBD_ENDPOINT_MAX_NBR - 1u - ep_bit;

  status = sli_usbd_core_os_create_endpoint_signal(ep_ix);

  if (status != SL_STATUS_OK) {
    goto end_clean;
  }

  status = sli_usbd_core_os_create_endpoint_lock(ep_ix);

  if (status != SL_STATUS_OK) {
    goto end_signal_clean;
  }

  transaction_frame = (max_pkt_size >> 11u) & 0x3;
  transaction_frame += 1u;

  // Open EP in dev drv.
  status = sli_usbd_driver_open_endpoint(ep_addr,
                                         // Mask out transactions per microframe.
                                         attrib & SL_USBD_ENDPOINT_TYPE_MASK,
                                         max_pkt_size & 0x7FF,
                                         transaction_frame);

  if (status != SL_STATUS_OK) {
    goto end_lock_signal_clean;
  }

  p_ep = &usbd_ptr->endpoint_table[ep_ix];

  CORE_ENTER_ATOMIC();
  p_ep->address = ep_addr;
  p_ep->attrib = attrib;
  p_ep->max_pkt_size = max_pkt_size;
  p_ep->interval = interval;
  p_ep->state = SLI_USBD_ENDPOINT_STATE_OPEN;
  p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_NONE;
  p_ep->index = ep_ix;

  usbd_ptr->endpoint_table_ptrs[ep_phy_nbr] = p_ep;
  CORE_EXIT_ATOMIC();

#if (SLI_USBD_CFG_DBG_STATS_EN == 1)
  // reset stats only if EP address changed.
  if (ep_addr != SLI_USBD_DBG_STATS_EP_GET(ep_ix, address)) {
    SLI_USBD_DBG_STATS_EP_RESET(ep_ix);
    SLI_USBD_DBG_STATS_EP_SET_ADDR(ep_ix, ep_addr);
  }
#endif
  SLI_USBD_DBG_STATS_EP_INC(ep_ix, endpoint_open_nbr);

  SLI_USBD_LOG_DBG(("EP Open for endpoint addr: 0x", (X)ep_addr));

  status = SL_STATUS_OK;
  return status;

  end_lock_signal_clean:
  sli_usbd_core_os_delete_endpoint_lock(ep_ix);

  end_signal_clean:
  sli_usbd_core_os_delete_endpoint_signal(ep_ix);

  end_clean:
  CORE_ENTER_ATOMIC();
  SL_SET_BIT(usbd_ptr->endpoint_open_bitmap, SLI_USBD_SINGLE_BIT_MASK_32(ep_bit));
  usbd_ptr->endpoint_open_ctrl -= 1u;
  CORE_EXIT_ATOMIC();

  return status;
}

/****************************************************************************************************//**
 * Retrieves the endpoint maximum packet size
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_max_endpoint_packet_size(uint8_t  ep_addr,
                                                      uint16_t *p_max_size)
{
  sli_usbd_endpoint_t    *p_ep;
  uint8_t ep_phy_nbr;
  uint16_t max_pkt_len;

  if (p_max_size == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_max_size = 0u;
    return SL_STATUS_INVALID_HANDLE;
  }

  // Mask out transactions per microframe.
  max_pkt_len = p_ep->max_pkt_size & 0x7FF;

  *p_max_size = max_pkt_len;
  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                           sl_usbd_core_get_max_open_endpoint_number()
 *
 * @brief    Retrieves the maximum number of opened endpoints.
 *
 * @param    p_nbr_open  Pointer to a variable that will receive the maximum number of opened endpoints.
 *                       The variable is set to the maximum number of opened endpoints if no errors are
 *                       returned, otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_max_open_endpoint_number(uint8_t *p_nbr_open)
{
  if (p_nbr_open == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *p_nbr_open = usbd_ptr->endpoint_open_ctrl;

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Aborts I/O transfer on the endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_abort_endpoint(uint8_t ep_addr)
{
  sli_usbd_endpoint_t    *p_ep;
  uint8_t ep_phy_nbr;
  sli_usbd_urb_t   *p_urb_head_aborted;
  sl_status_t  status;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, endpoint_abort_exec_nbr);

  if ((p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) && (p_ep->state != SLI_USBD_ENDPOINT_STATE_STALL)) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_INVALID_STATE;
  }

  // Abort xfers in progress, keep ptr to head of list.
  status = usbd_core_abort_urb(p_ep, &p_urb_head_aborted);

  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, endpoint_abort_success_nbr, (status == SL_STATUS_OK));

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  // Execute callback and free aborted URB(s), if any.
  if (p_urb_head_aborted != NULL) {
    usbd_core_end_async_urb(p_ep, p_urb_head_aborted);
  }

  return status;
}

/****************************************************************************************************//**
 * Close non-control endpoint
 *******************************************************************************************************/
sl_status_t sli_usbd_core_close_endpoint(uint8_t ep_addr)
{
  sli_usbd_endpoint_t    *p_ep;
  uint8_t ep_bit;
  uint8_t ep_phy_nbr;
  sli_usbd_urb_t   *p_urb_head_aborted;
  sl_status_t  status;
  CORE_DECLARE_IRQ_STATE;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_ep->state == SLI_USBD_ENDPOINT_STATE_CLOSE) {
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);
    return SL_STATUS_OK;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, endpoint_close_exec_nbr);

  ep_bit = SL_USBD_ENDPOINT_MAX_NBR - 1u - p_ep->index;

  // Abort xfers in progress, keep ptr to head of list.
  status = usbd_core_abort_urb(p_ep, &p_urb_head_aborted);

  p_ep->state = SLI_USBD_ENDPOINT_STATE_CLOSE;

  CORE_ENTER_ATOMIC();
  SL_SET_BIT(usbd_ptr->endpoint_open_bitmap, SLI_USBD_SINGLE_BIT_MASK_32(ep_bit));
  usbd_ptr->endpoint_open_ctrl--;
  CORE_EXIT_ATOMIC();

  sli_usbd_driver_close_endpoint(ep_addr);

  p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_NONE;
  usbd_ptr->endpoint_table_ptrs[ep_phy_nbr] = NULL;

  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, endpoint_close_success_nbr, (status == SL_STATUS_OK));

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  sli_usbd_core_os_delete_endpoint_signal(p_ep->index);
  sli_usbd_core_os_delete_endpoint_lock(p_ep->index);

  // Execute callback and free aborted URB(s), if any.
  if (p_urb_head_aborted != NULL) {
    usbd_core_end_async_urb(p_ep, p_urb_head_aborted);
  }

  return status;
}

/****************************************************************************************************//**
 * Stalls the non-control endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_stall_endpoint(uint8_t  ep_addr,
                                        bool     state)
{
  uint8_t  ep_phy_nbr;
  sli_usbd_endpoint_t     *p_ep;
  sli_usbd_urb_t    *p_urb_head_aborted;
  sl_status_t   status;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  p_urb_head_aborted = NULL;

  switch (p_ep->state) {
    case SLI_USBD_ENDPOINT_STATE_OPEN:
      if (state == 1u) {
        // Abort xfers in progress, keep ptr to head of list.
        status = usbd_core_abort_urb(p_ep, &p_urb_head_aborted);
        if (status != SL_STATUS_OK) {
          break;
        }
        p_ep->state = SLI_USBD_ENDPOINT_STATE_STALL;
      }

      status = sli_usbd_driver_stall_endpoint(p_ep->address, state);
      break;

    case SLI_USBD_ENDPOINT_STATE_STALL:
      if (state == 0u) {
        status = sli_usbd_driver_stall_endpoint(p_ep->address, 0u);
        if (status == SL_STATUS_OK) {
          p_ep->state = SLI_USBD_ENDPOINT_STATE_OPEN;
        }
      }
      break;

    case SLI_USBD_ENDPOINT_STATE_CLOSE:
    default:
      status = SL_STATUS_INVALID_STATE;
      break;
  }

  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  // Execute callback and free aborted URB(s), if any.
  if (p_urb_head_aborted != NULL) {
    usbd_core_end_async_urb(p_ep, p_urb_head_aborted);
  }

  return status;
}

/****************************************************************************************************//**
 * Gets the stall status of a non-control endpoint
 *******************************************************************************************************/
sl_status_t sl_usbd_core_is_endpoint_stalled(uint8_t ep_addr,
                                             bool    *p_ep_stall)
{
  sli_usbd_endpoint_t    *p_ep;
  uint8_t ep_phy_nbr;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    *p_ep_stall = false;
    return SL_STATUS_INVALID_HANDLE;
  }

  if (p_ep->state == SLI_USBD_ENDPOINT_STATE_STALL) {
    *p_ep_stall = true;
  } else {
    *p_ep_stall = false;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notify USB stack that packet receive has completed
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_read_complete(uint8_t ep_log_nbr)
{
  sli_usbd_endpoint_t    *p_ep;
  uint8_t ep_phy_nbr;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SL_USBD_ENDPOINT_LOG_TO_ADDR_OUT(ep_log_nbr));
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_cmpl_err_nbr);
    return SL_STATUS_INVALID_PARAMETER;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_cmpl_nbr);

  if (p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_SYNC) {
    sli_usbd_core_os_post_endpoint_signal(p_ep->index);
  } else if ((p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_ASYNC)
             || (p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_ASYNC_PARTIAL)) {
    sli_usbd_core_endpoint_event(p_ep->address, SL_STATUS_OK);
  } else {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_cmpl_err_nbr);
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notify USB stack that packet transmit has completed
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_write_complete(uint8_t ep_log_nbr)
{
  sli_usbd_endpoint_t    *p_ep;
  uint8_t ep_phy_nbr;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SL_USBD_ENDPOINT_LOG_TO_ADDR_IN(ep_log_nbr));
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_cmpl_err_nbr);
    return SL_STATUS_INVALID_PARAMETER;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_cmpl_nbr);

  if (p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_SYNC) {
    sli_usbd_core_os_post_endpoint_signal(p_ep->index);
  } else if ((p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_ASYNC)
             || (p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_ASYNC_PARTIAL)) {
    sli_usbd_core_endpoint_event(p_ep->address, SL_STATUS_OK);
  } else {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_cmpl_err_nbr);
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Notify USB stack that packet transmit has completed
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_write_complete_extended(uint8_t     ep_log_nbr,
                                                          sl_status_t xfer_err)
{
  sli_usbd_endpoint_t    *p_ep;
  uint8_t ep_phy_nbr;
  sl_status_t  status;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(SL_USBD_ENDPOINT_LOG_TO_ADDR_IN(ep_log_nbr));
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_cmpl_err_nbr);
    return SL_STATUS_INVALID_PARAMETER;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_cmpl_nbr);

  status = SL_STATUS_OK;

  if (p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_SYNC) {
    status = sli_usbd_core_os_abort_endpoint_signal(p_ep->index);
  } else if ((p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_ASYNC)
             || (p_ep->transfer_state == SLI_USBD_TRANSFER_STATE_ASYNC_PARTIAL)) {
    sli_usbd_core_endpoint_event(p_ep->address, xfer_err);
  } else {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_cmpl_err_nbr);
  }

  return status;
}

/****************************************************************************************************//**
 * Send zero-length packet to the host
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_write_zlp(uint8_t  ep_addr,
                                            uint16_t timeout_ms)
{
  sli_usbd_endpoint_t      *p_ep;
  sli_usbd_urb_t     *p_urb;
  uint8_t   ep_phy_nbr;
  sl_status_t    status;
  sl_status_t    local_status;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_zlp_exec_nbr);

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    status = SL_STATUS_INVALID_STATE;
    goto lock_release;
  }
  // Chk EP attrib.
  if ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC) {
    status = SL_STATUS_INVALID_HANDLE;
    goto lock_release;
  }

  if (p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_NONE) {
    status = SL_STATUS_NOT_READY;
    goto lock_release;
  }

  status = usbd_core_get_urb(p_ep, &p_urb);

  if (status != SL_STATUS_OK) {
    goto lock_release;
  }

  // Only state needs to be set to indicate sync xfer.
  p_urb->state = SLI_USBD_URB_STATE_TRANSFER_SYNC;
  // Set transfer_state before submitting xfer.
  p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_SYNC;

  usbd_core_queue_urb(p_ep, p_urb);

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_zlp_nbr);

  status = sli_usbd_driver_endpoint_tx_zlp(ep_addr);

  if (status == SL_STATUS_OK) {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_zlp_success_nbr);

    // Unlock before pending on completion.
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);

    local_status = sli_usbd_core_os_pend_endpoint_signal(p_ep->index, timeout_ms);

    // Re-lock EP after xfer completion.
    status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

    if (local_status == SL_STATUS_TIMEOUT) {
      sli_usbd_driver_abort_endpoint(ep_addr);
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_sync_timeout_err_nbr);
      status = local_status;
    }
  }

  usbd_core_dequeue_urb(p_ep);

  usbd_core_free_urb(p_ep, p_urb);

  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, driver_tx_zlp_success_nbr, (status == SL_STATUS_OK));

  lock_release:
  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  return status;
}

/****************************************************************************************************//**
 * Receive zero-length packet from the host
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_read_zlp(uint8_t  ep_addr,
                                           uint16_t timeout_ms)
{
  sli_usbd_endpoint_t      *p_ep;
  sli_usbd_urb_t     *p_urb;
  uint32_t   ep_pkt_len;
  uint8_t   ep_phy_nbr;
  sl_status_t    status;
  sl_status_t    local_status;

  ep_phy_nbr = SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr);
  p_ep = usbd_ptr->endpoint_table_ptrs[ep_phy_nbr];

  if (p_ep == NULL) {
    return SL_STATUS_INVALID_HANDLE;
  }

  status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

  if (status != SL_STATUS_OK) {
    return status;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_zlp_exec_nbr);

  if (p_ep->state != SLI_USBD_ENDPOINT_STATE_OPEN) {
    status = SL_STATUS_INVALID_STATE;
    goto lock_release;
  }

  if ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) == SL_USBD_ENDPOINT_TYPE_ISOC) {
    status = SL_STATUS_INVALID_HANDLE;
    goto lock_release;
  }

  if (p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_NONE) {
    status = SL_STATUS_NOT_READY;
    goto lock_release;
  }

  status = usbd_core_get_urb(p_ep, &p_urb);

  if (status != SL_STATUS_OK) {
    goto lock_release;
  }

  // Only state needs to be set to indicate sync xfer.
  p_urb->state = SLI_USBD_URB_STATE_TRANSFER_SYNC;
  // Set transfer_state before submitting xfer.
  p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_SYNC;

  usbd_core_queue_urb(p_ep, p_urb);

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_start_nbr);

  status = sli_usbd_driver_endpoint_rx_start(ep_addr,
                                             NULL,
                                             0u,
                                             &ep_pkt_len);

  if (status == SL_STATUS_OK) {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_start_success_nbr);

    // Unlock before pending on completion.
    sli_usbd_core_os_release_endpoint_lock(p_ep->index);

    local_status = sli_usbd_core_os_pend_endpoint_signal(p_ep->index, timeout_ms);

    // Re-lock EP after xfer completion.
    status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

    if ((status == SL_STATUS_OK) && (local_status == SL_STATUS_OK)) {
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_zlp_nbr);
      status = sli_usbd_driver_endpoint_rx_zlp(ep_addr);
      SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, driver_rx_zlp_success_nbr, (status == SL_STATUS_OK));
    } else if (local_status == SL_STATUS_TIMEOUT) {
      sli_usbd_driver_abort_endpoint(ep_addr);
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_sync_timeout_err_nbr);
      status = local_status;
    }
  }

  usbd_core_dequeue_urb(p_ep);

  usbd_core_free_urb(p_ep, p_urb);

  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, rx_zlp_success_nbr, (status == SL_STATUS_OK));

  lock_release:
  sli_usbd_core_os_release_endpoint_lock(p_ep->index);

  return status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       usbd_core_endpoint_process_async_rx()
 *
 * @brief    Process driver's asynchronous RxStart operation.
 *
 * @param    p_ep        Pointer to endpoint on which data will be received.
 *
 * @param    p_urb       Pointer to USB request block.
 *
 * @param    p_buf_cur   Pointer to source buffer to receive data.
 *
 * @param    len         Number of octets to receive.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static sl_status_t usbd_core_endpoint_process_async_rx(sli_usbd_endpoint_t    *p_ep,
                                                       sli_usbd_urb_t         *p_urb,
                                                       uint8_t                *p_buf_cur,
                                                       uint32_t               len)
{
  sl_status_t    status;
  CORE_DECLARE_IRQ_STATE;

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_start_nbr);
  status = sli_usbd_driver_endpoint_rx_start(p_ep->address,
                                             p_buf_cur,
                                             len,
                                             &(p_urb->next_xfer_len));
  if (status != SL_STATUS_OK) {
    return status;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_start_success_nbr);

  if (p_urb->next_xfer_len != len) {
    CORE_ENTER_ATOMIC();
    // Xfer will have to be done in many transactions.
    p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_ASYNC_PARTIAL;
    CORE_EXIT_ATOMIC();
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                           usbd_core_endpoint_process_async_tx()
 *
 * @brief    Process driver's asynchronous Tx operation.
 *
 * @param    p_ep        Pointer to endpoint on which data will be transmitted.
 *
 * @param    p_urb       Pointer to USB request block.
 *
 * @param    p_buf_cur   Pointer to source buffer to transmit data.
 *
 * @param    len         Number of octets to transmit.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static sl_status_t usbd_core_endpoint_process_async_tx(sli_usbd_endpoint_t    *p_ep,
                                                       sli_usbd_urb_t         *p_urb,
                                                       uint8_t                *p_buf_cur,
                                                       uint32_t               len)
{
  sl_status_t status;

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_nbr);

  status = sli_usbd_driver_endpoint_tx(p_ep->address,
                                       p_buf_cur,
                                       len,
                                       &(p_urb->next_xfer_len));
  if (status != SL_STATUS_OK) {
    return status;
  }

  if (p_urb->next_xfer_len == len) {
    // Xfer can be done is a single transaction.
    p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_ASYNC;
  } else if ((p_ep->attrib & SL_USBD_ENDPOINT_TYPE_MASK) != SL_USBD_ENDPOINT_TYPE_ISOC) {
    // Xfer will have to be done in many transactions.
    p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_ASYNC_PARTIAL;
  } else {
    // Cannot split xfer on isoc EP.
    return SL_STATUS_TRANSMIT;
  }

  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_success_nbr);
  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_start_nbr);

  status = sli_usbd_driver_endpoint_tx_start(p_ep->address,
                                             p_buf_cur,
                                             p_urb->next_xfer_len);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Error not accounted on total xfer len.
  p_urb->transfer_len += p_urb->next_xfer_len;
  p_urb->next_xfer_len = 0u;
  SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_start_success_nbr);

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                               usbd_core_endpoint_read()
 *
 * @brief    Receive data on OUT endpoint.This function should not be called from Interrupt Context.
 *
 * @param    p_ep            Pointer to endpoint on which data will be received.
 *
 * @param    ----            Argument checked by caller.
 *
 * @param    p_buf           Pointer to destination buffer to receive data.
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    timeout_ms      Timeout in milliseconds.
 *
 * @param    p_xfer_len      Pointer to a variable that will be set to number of octets received.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Endpoint must be locked when calling this function.
 *
 * @note     (3) During a synchronous transfer, endpoint is unlocked before pending on transfer
 *               completion to be able to abort. Since the endpoint is already locked when this
 *               function is called (see callers functions), it releases the lock before pending and
 *               re-locks once the transfer completes.
 *
 * @note     (4) This condition covers also the case where the transfer length is multiple of the
 *               maximum packet size. In that case, host sends a zero-length packet considered as
 *               a short packet for the condition.
 *******************************************************************************************************/
static sl_status_t usbd_core_endpoint_read(sli_usbd_endpoint_t         *p_ep,
                                           void                        *p_buf,
                                           uint32_t                    buf_len,
                                           sl_usbd_async_function_t    async_fnct,
                                           void                        *p_async_arg,
                                           uint16_t                    timeout_ms,
                                           uint32_t                    *p_xfer_len)
{
  sli_usbd_urb_t        *p_urb;
  sli_usbd_transfer_state_t prev_xfer_state;
  uint8_t      *p_buf_cur;
  uint32_t      xfer_len;
  uint32_t      xfer_tot;
  uint32_t      prev_xfer_len;
  sl_status_t       status;
  sl_status_t       local_status;

  if ((buf_len != 0u) && (p_buf == NULL)) {
    *p_xfer_len = 0u;
    return SL_STATUS_NULL_POINTER;
  }

  if (async_fnct == NULL) {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_sync_exec_nbr);
    if (p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_NONE) {
      *p_xfer_len = 0u;
      return SL_STATUS_NOT_READY;
    }
  } else {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_async_exec_nbr);
    if ((p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_NONE)
        && (p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_ASYNC)) {
      *p_xfer_len = 0u;
      return SL_STATUS_NOT_READY;
    }
  }

  status = usbd_core_get_urb(p_ep, &p_urb);
  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  // Init 'p_urb' fields.
  p_urb->buf_ptr = (uint8_t *)p_buf;
  p_urb->buf_len = buf_len;
  p_urb->transfer_len = 0u;
  p_urb->next_xfer_len = 0u;
  p_urb->async_function = async_fnct;
  p_urb->async_function_arg = p_async_arg;
  p_urb->next_ptr = NULL;
  p_urb->status = SL_STATUS_OK;

  // Async Transfer
  if (async_fnct != NULL) {
    p_urb->state = SLI_USBD_URB_STATE_TRANSFER_ASYNC;
    // Keep prev transfer_state, to restore in case of err.
    prev_xfer_state = p_ep->transfer_state;
    // Set transfer_state before submitting the xfer.
    p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_ASYNC;

    status = usbd_core_endpoint_process_async_rx(p_ep,
                                                 p_urb,
                                                 p_urb->buf_ptr,
                                                 p_urb->buf_len);
    if (status == SL_STATUS_OK) {
      // If no err, queue URB.
      usbd_core_queue_urb(p_ep, p_urb);
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_async_success_nbr);
    } else {
      // If an err occured, restore prev transfer_state.
      p_ep->transfer_state = prev_xfer_state;
      // Free URB.
      usbd_core_free_urb(p_ep, p_urb);
    }

    *p_xfer_len = 0u;
    return status;
  }

  // Sync Transfer
  p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_SYNC;
  p_urb->state = SLI_USBD_URB_STATE_TRANSFER_SYNC;

  usbd_core_queue_urb(p_ep, p_urb);

  p_urb->next_xfer_len = p_urb->buf_len;

  status = SL_STATUS_OK;

  while ((status == SL_STATUS_OK) && (p_urb->next_xfer_len > 0u)) {
    p_buf_cur = &p_urb->buf_ptr[p_urb->transfer_len];

    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_start_nbr);
    status = sli_usbd_driver_endpoint_rx_start(p_ep->address,
                                               p_buf_cur,
                                               p_urb->next_xfer_len,
                                               &(p_urb->next_xfer_len));
    if (status != SL_STATUS_OK) {
      break;
    }
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_start_success_nbr);

    sli_usbd_core_os_release_endpoint_lock(p_ep->index);

    status = sli_usbd_core_os_pend_endpoint_signal(p_ep->index, timeout_ms);

    // Re-lock EP after xfer completion. See Note #3.
    local_status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

    if (local_status != SL_STATUS_OK) {
      if (status == SL_STATUS_OK) {
        status = local_status;
      }
      break;
    }

    if (status == SL_STATUS_TIMEOUT) {
      sli_usbd_driver_abort_endpoint(p_ep->address);
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, rx_sync_timeout_err_nbr);
      break;
    } else if (status != SL_STATUS_OK) {
      break;
    }

    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_nbr);
    status = sli_usbd_driver_endpoint_rx(p_ep->address,
                                         p_buf_cur,
                                         p_urb->next_xfer_len,
                                         &xfer_len);
    if (status != SL_STATUS_OK) {
      break;
    }
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_rx_success_nbr);

    if (xfer_len > p_urb->next_xfer_len) {
      // Rx'd more data than what was expected.
      p_urb->transfer_len += p_urb->next_xfer_len;
      status = SL_STATUS_RECEIVE;
    } else {
      p_urb->transfer_len += xfer_len;
      prev_xfer_len = p_urb->next_xfer_len;
      p_urb->next_xfer_len = p_urb->buf_len - p_urb->transfer_len;
      if ((xfer_len == 0u)                                      // Rx'd a ZLP.
          || (xfer_len < prev_xfer_len)) {                      // Rx'd a short pkt (see Note #4).
        p_urb->next_xfer_len = 0u;
      }
    }
  }

  xfer_tot = p_urb->transfer_len;

  usbd_core_dequeue_urb(p_ep);

  usbd_core_free_urb(p_ep, p_urb);

  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, rx_sync_success_nbr, (status == SL_STATUS_OK));

  *p_xfer_len = xfer_tot;

  return status;
}

/****************************************************************************************************//**
 *                                               usbd_core_endpoint_write()
 *
 * @brief    Send data on IN endpoints.
 *
 * @param    p_ep            Pointer to endpoint on which data will be sent.
 *
 * @param    ----            Argument checked by caller.
 *
 * @param    p_buf           Pointer to buffer of data that will be sent.
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    timeout_ms      Timeout in milliseconds.
 *
 * @param    end             End-of-transfer flag (see Note #2).
 *
 * @param    p_xfer_len      Pointer to a variable that will be set to number of octets transmitted.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *
 * @note     (3) Endpoint must be locked when calling this function.
 *
 * @note     (4) During a synchronous transfer, endpoint is unlocked before pending on transfer
 *               completion to be able to abort. Since the endpoint is already locked when this
 *               function is called (see callers functions), it releases the lock before pending and
 *               re-locks once the transfer completes.
 *******************************************************************************************************/
static sl_status_t usbd_core_endpoint_write(sli_usbd_endpoint_t         *p_ep,
                                            void                        *p_buf,
                                            uint32_t                    buf_len,
                                            sl_usbd_async_function_t    async_fnct,
                                            void                        *p_async_arg,
                                            uint16_t                    timeout_ms,
                                            bool                        end,
                                            uint32_t                    *p_xfer_len)
{
  sli_usbd_urb_t        *p_urb;
  sli_usbd_transfer_state_t prev_xfer_state;
  uint8_t      *p_buf_cur;
  uint32_t      xfer_rem;
  uint32_t      xfer_tot;
  sl_status_t       status;
  sl_status_t       local_status;
  bool     zlp_flag = false;

  if ((buf_len != 0u) && (p_buf == NULL)) {
    *p_xfer_len = 0u;
    return SL_STATUS_NULL_POINTER;
  }

  if (async_fnct == NULL) {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_sync_exec_nbr);
    if (p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_NONE) {
      *p_xfer_len = 0u;
      return SL_STATUS_NOT_READY;
    }
  } else {
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_async_exec_nbr);
    if ((p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_NONE)
        && (p_ep->transfer_state != SLI_USBD_TRANSFER_STATE_ASYNC)) {
      *p_xfer_len = 0u;
      return SL_STATUS_NOT_READY;
    }
  }

  if (buf_len == 0u) {
    zlp_flag = true;
  }

  status = usbd_core_get_urb(p_ep, &p_urb);
  if (status != SL_STATUS_OK) {
    *p_xfer_len = 0u;
    return status;
  }

  // Init 'p_urb' fields.
  p_urb->buf_ptr = (uint8_t *)p_buf;
  p_urb->buf_len = buf_len;
  p_urb->transfer_len = 0u;
  p_urb->next_xfer_len = 0u;
  p_urb->async_function = async_fnct;
  p_urb->async_function_arg = p_async_arg;
  p_urb->next_ptr = NULL;
  p_urb->status = SL_STATUS_OK;
  if (end == true) {
    SL_SET_BIT(p_urb->flags, SLI_USBD_URB_FLAG_XFER_END);
  }

  // Async Transfer
  if (async_fnct != NULL) {
    p_urb->state = SLI_USBD_URB_STATE_TRANSFER_ASYNC;
    // Keep prev transfer_state, to restore in case of err.
    prev_xfer_state = p_ep->transfer_state;
    // Set transfer_state before submitting the xfer.
    p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_ASYNC;

    status = usbd_core_endpoint_process_async_tx(p_ep,
                                                 p_urb,
                                                 p_urb->buf_ptr,
                                                 p_urb->buf_len);

    if (status == SL_STATUS_OK) {
      // If no err, queue URB.
      usbd_core_queue_urb(p_ep, p_urb);
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_async_success_nbr);
    } else {
      // If an err occured, restore prev transfer_state.
      p_ep->transfer_state = prev_xfer_state;
      // Free URB.
      usbd_core_free_urb(p_ep, p_urb);
    }

    *p_xfer_len = 0u;
    return status;
  }

  // Sync Transfer
  p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_SYNC;
  p_urb->state = SLI_USBD_URB_STATE_TRANSFER_SYNC;

  usbd_core_queue_urb(p_ep, p_urb);

  xfer_rem = p_urb->buf_len;

  status = SL_STATUS_OK;

  while ((status == SL_STATUS_OK) && ((xfer_rem > 0u) || (zlp_flag == true))) {
    if (zlp_flag == false) {
      p_buf_cur = &p_urb->buf_ptr[p_urb->transfer_len];
    } else {
      p_buf_cur = NULL;
      // If Tx ZLP, loop done only once.
      zlp_flag = false;
    }

    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_nbr);

    status = sli_usbd_driver_endpoint_tx(p_ep->address,
                                         p_buf_cur,
                                         xfer_rem,
                                         &(p_urb->next_xfer_len));
    if (status != SL_STATUS_OK) {
      break;
    }

    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_success_nbr);
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_start_nbr);

    status = sli_usbd_driver_endpoint_tx_start(p_ep->address,
                                               p_buf_cur,
                                               p_urb->next_xfer_len);
    if (status != SL_STATUS_OK) {
      break;
    }

    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_start_success_nbr);

    sli_usbd_core_os_release_endpoint_lock(p_ep->index);

    status = sli_usbd_core_os_pend_endpoint_signal(p_ep->index, timeout_ms);

    // Re-lock EP after xfer completion. See Note #4.
    local_status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

    if (local_status != SL_STATUS_OK) {
      if (status == SL_STATUS_OK) {
        status = local_status;
      }
      break;
    }

    if (status == SL_STATUS_TIMEOUT) {
      sli_usbd_driver_abort_endpoint(p_ep->address);
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, tx_sync_timeout_err_nbr);
      break;
    } else if (status != SL_STATUS_OK) {
      break;
    }

    p_urb->transfer_len += p_urb->next_xfer_len;
    xfer_rem -= p_urb->next_xfer_len;
  }

  xfer_tot = p_urb->transfer_len;

  if ((end == true)
      && (status == SL_STATUS_OK)
      && ((p_urb->buf_len % p_ep->max_pkt_size) == 0u)
      && (p_urb->buf_len != 0u)) {
    // TODO This case should be tested more thoroughly.
    SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_zlp_nbr);

    status = sli_usbd_driver_endpoint_tx_zlp(p_ep->address);

    if (status == SL_STATUS_OK) {
      SLI_USBD_DBG_STATS_EP_INC(p_ep->index, driver_tx_success_nbr);

      // Unlock before pending on completion. See Note #4.
      sli_usbd_core_os_release_endpoint_lock(p_ep->index);

      status = sli_usbd_core_os_pend_endpoint_signal(p_ep->index, timeout_ms);

      // Re-lock EP after xfer completion. See Note #4.
      local_status = sli_usbd_core_os_acquire_endpoint_lock(p_ep->index);

      if (local_status != SL_STATUS_OK) {
        status = local_status;
      }
    }
  }

  usbd_core_dequeue_urb(p_ep);

  usbd_core_free_urb(p_ep, p_urb);

  SLI_USBD_DBG_STATS_EP_INC_IF_TRUE(p_ep->index, tx_sync_success_nbr, (status == SL_STATUS_OK));

  *p_xfer_len = xfer_tot;

  return status;
}

/****************************************************************************************************//**
 *                                           usbd_core_abort_urb()
 *
 * @brief    Aborts endpoint's URB(s). Does not free the URB(s), see Note #1.
 *
 * @param    p_ep        Pointer to endpoint on which data will be sent.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_urb_ptr   Pointer to a (URB*) ptr that will be loaded with the head of completed
 *                       URB list if asynchronous transfer is in progress, or with NULL otherwise.
 *
 * @param    ----        Argument checked by caller.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If a synchronous transfer was in progress, a single URB was aborted and must be
 *               freed by calling  usbd_core_free_urb(). If an asynchronous transfer was in progress, it
 *               is possible that multiple URBs were aborted and must be freed by calling
 *               usbd_core_end_async_urb() with the pointer to the head of the aborted URB(s) list returned
 *               by this function as a parameter.
 *
 * @note     (2) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static sl_status_t usbd_core_abort_urb(sli_usbd_endpoint_t  *p_ep,
                                       sli_usbd_urb_t       **p_urb_ptr)
{
  sli_usbd_urb_t     *p_urb_head;
  sli_usbd_urb_t     *p_urb;
  sli_usbd_urb_t     *p_urb_cur;
  sl_status_t    status;

  p_urb_head = NULL;
  p_urb_cur = NULL;

  switch (p_ep->transfer_state) {
    case SLI_USBD_TRANSFER_STATE_NONE:
      status = SL_STATUS_OK;
      break;

    case SLI_USBD_TRANSFER_STATE_ASYNC:
    case SLI_USBD_TRANSFER_STATE_ASYNC_PARTIAL:
      p_urb = p_ep->urb_head_ptr;
      status = SL_STATUS_ABORT;
      while (p_urb != NULL) {
        usbd_core_complete_async_urb(p_ep, &p_urb, status);
        if (p_urb_head == NULL) {
          p_urb_head = p_urb;
        } else {
          p_urb_cur->next_ptr = p_urb;
        }
        p_urb_cur = p_urb;
      }
      // Call drv's abort fnct.
      status = sli_usbd_driver_abort_endpoint(p_ep->address);
      break;

    case SLI_USBD_TRANSFER_STATE_SYNC:
      sli_usbd_core_os_abort_endpoint_signal(p_ep->index);
      status = sli_usbd_driver_abort_endpoint(p_ep->address);
      break;

    default:
      status = SL_STATUS_ABORT;
      break;
  }

  *p_urb_ptr = p_urb_head;
  // See Note #1.
  return status;
}

/****************************************************************************************************//**
 *                                           usbd_core_complete_async_urb()
 *
 * @brief    Notify URB completion to asynchronous callback.
 *
 * @param    p_ep    Pointer to endpoint on which transfer has completed.
 *
 * @param    ----  Argument checked by caller.
 *
 * @param    err     Error associated with transfer.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_urb_ptr   Pointer to a (URB*) ptr that will be loaded with the head of completed
 *                       URB list if any asynchronous transfer is completed, or with NULL otherwise.
 *
 * @param    ----        Argument checked by caller.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *
 * @note     (2) Endpoint must have an asynchronous transfer in progress.
 *******************************************************************************************************/
static void usbd_core_complete_async_urb(sli_usbd_endpoint_t   *p_ep,
                                         sli_usbd_urb_t        **p_urb_ptr,
                                         sl_status_t           status)
{
  sli_usbd_urb_t *p_urb;

  // Get head URB for EP.
  p_urb = p_ep->urb_head_ptr;
  if (p_urb == NULL) {
    *p_urb_ptr = p_urb;
    return;
  }

  // Dequeue first URB from EP.
  usbd_core_dequeue_urb(p_ep);

  // Set err for curr URB.
  p_urb->status = status;
  // Remove links with 'p_ep' URB linked list.
  p_urb->next_ptr = NULL;

  *p_urb_ptr = p_urb;
  return;
}

/****************************************************************************************************//**
 *                                           usbd_core_end_async_urb()
 *
 * @brief    Execute callback associated with each USB request block in the linked list and free them.
 *
 * @param    p_ep        Pointer to endpoint structure.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_urb_head  Pointer to head of USB request block linked list.
 *
 * @param    -----       Argument checked by caller.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Endpoint must NOT be locked when calling this function.
 *******************************************************************************************************/
static void usbd_core_end_async_urb(sli_usbd_endpoint_t    *p_ep,
                                    sli_usbd_urb_t         *p_urb_head)
{
  sli_usbd_urb_t            *p_urb_cur;
  sli_usbd_urb_t            *p_urb_next;
  void                      *p_buf;
  uint32_t                  buf_len;
  uint32_t                  xfer_len;
  sl_usbd_async_function_t  async_fnct;
  void                      *p_async_arg;
  sl_status_t               status;

  p_urb_cur = p_urb_head;
  // Iterate through linked list.
  while (p_urb_cur != NULL) {
    p_buf = (void *)p_urb_cur->buf_ptr;
    buf_len = p_urb_cur->buf_len;
    xfer_len = p_urb_cur->transfer_len;
    async_fnct = p_urb_cur->async_function;
    p_async_arg = p_urb_cur->async_function_arg;
    p_urb_next = p_urb_cur->next_ptr;
    status = p_urb_cur->status;

    // Free URB to pool.
    usbd_core_free_urb(p_ep, p_urb_cur);

    // Execute callback fnct.
    async_fnct(p_ep->address,
               p_buf,
               buf_len,
               xfer_len,
               p_async_arg,
               status);

    p_urb_cur = p_urb_next;
  }
}

/****************************************************************************************************//**
 *                                               usbd_core_free_urb()
 *
 * @brief    Free URB to URB pool.
 *
 * @param    p_ep        Pointer to endpoint structure.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_urb       Pointer to USB request block.
 *
 * @param    -----       Argument checked by caller.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
static void usbd_core_free_urb(sli_usbd_endpoint_t    *p_ep,
                               sli_usbd_urb_t         *p_urb)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_urb->state = SLI_USBD_URB_STATE_IDLE;
  p_urb->next_ptr = usbd_ptr->urb_table_ptr;
  usbd_ptr->urb_table_ptr = p_urb;

#if (USBD_CFG_URB_EXTRA_EN == 1)
  if (SL_IS_BIT_SET(p_urb->flags, SLI_USBD_URB_FLAG_EXTRA_URB)) {
    // If the URB freed is an 'extra' URB, dec ctr.
    // USBD_URB_ExtraCtr--;
    usbd_ptr->urb_extra_avail_count++;
  } else {
    p_ep->urb_main_avail = true;
  }
#else
  (void)&p_ep;
#endif
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               usbd_core_get_urb()
 *
 * @brief    Get URB from URB pool.
 *
 * @param    p_ep        Pointer to endpoint structure.
 *
 * @param    ----        Argument checked by caller.
 *
 * @param    p_urb_ptr   Pointer to a (URB*) ptr that will be loaded with USB request block if no
 *                       errors are returned, or with NULL otherwise.
 *
 * @param    ----        Argument checked by caller.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
static sl_status_t usbd_core_get_urb(sli_usbd_endpoint_t    *p_ep,
                                     sli_usbd_urb_t         **p_urb_ptr)
{
  bool ep_empty;
  sli_usbd_urb_t    *p_urb;
  sl_status_t   status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  ep_empty = ((p_ep->urb_head_ptr == NULL) && (p_ep->urb_tail_ptr == NULL)) ? true : false;

#if (USBD_CFG_URB_EXTRA_EN == 1)
  // Check if EP is empty, if enough URB rem or if main URB avail.
  if ((ep_empty == true)
      || (usbd_ptr->urb_extra_avail_count > 0u) // TODO
      || (p_ep->urb_main_avail == true)) {
#else
  // Check if EP is empty.
  if (ep_empty == true) {
#endif

    p_urb = usbd_ptr->urb_table_ptr;
    if (p_urb != NULL) {
      usbd_ptr->urb_table_ptr = p_urb->next_ptr;

      p_urb->next_ptr = NULL;
      p_urb->flags = 0u;
#if (USBD_CFG_URB_EXTRA_EN == 1)
      if ((ep_empty == false)
          && (p_ep->urb_main_avail == false)) {
        // If the EP already has an URB in progress, inc ctr and mark the URB as an 'extra' URB.
        usbd_ptr->urb_extra_avail_count--;
        SL_SET_BIT(p_urb->flags, SLI_USBD_URB_FLAG_EXTRA_URB);
      } else if (p_ep->urb_main_avail == true) {
        p_ep->urb_main_avail = false;
      }
#endif
      status = SL_STATUS_OK;
    } else {
      status = SL_STATUS_FULL;
      p_urb = NULL;
    }
  } else {
    status = SL_STATUS_FULL;
    p_urb = NULL;
  }
  CORE_EXIT_ATOMIC();

  *p_urb_ptr = p_urb;
  return status;
}

/****************************************************************************************************//**
 *                                               usbd_core_queue_urb()
 *
 * @brief    Queue USB request block into endpoint.
 *
 * @param    p_ep    Pointer to endpoint structure.
 *
 * @param    ----    Argument checked by caller.
 *
 * @param    p_urb   Pointer to USB request block.
 *
 * @param    -----   Argument checked by caller.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static void usbd_core_queue_urb(sli_usbd_endpoint_t  *p_ep,
                                sli_usbd_urb_t       *p_urb)
{
  p_urb->next_ptr = NULL;

  if (p_ep->urb_tail_ptr == NULL) {
    // Q is empty.
    p_ep->urb_head_ptr = p_urb;
    p_ep->urb_tail_ptr = p_urb;
  } else {
    // Q is not empty.
    p_ep->urb_tail_ptr->next_ptr = p_urb;
    p_ep->urb_tail_ptr = p_urb;
  }

  return;
}

/****************************************************************************************************//**
 *                                           usbd_core_dequeue_urb()
 *
 * @brief    Dequeue head USB request block from endpoint.
 *
 * @param    p_ep    Pointer to endpoint structure.
 *
 * @param    ----    Argument checked by caller.
 *
 * @note     (1) Endpoint must be locked when calling this function.
 *******************************************************************************************************/
static void usbd_core_dequeue_urb(sli_usbd_endpoint_t *p_ep)
{
  sli_usbd_urb_t *p_urb;

  p_urb = p_ep->urb_head_ptr;
  if (p_urb == NULL) {
    return;
  }

  if (p_urb->next_ptr == NULL) {
    // Only one URB is queued.
    p_ep->urb_head_ptr = NULL;
    p_ep->urb_tail_ptr = NULL;
    p_ep->transfer_state = SLI_USBD_TRANSFER_STATE_NONE;
  } else {
    p_ep->urb_head_ptr = p_urb->next_ptr;
  }
}
