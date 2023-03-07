/***************************************************************************//**
 * @file
 * @brief Ethernet interface implementation for LwIP and WFX
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include <string.h>
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "sl_wfx_constants.h"
#include "sl_wfx_host_api.h"
#include "sl_wfx_task.h"
#include "sl_wfx.h"
#include "app_webpage.h"
#include "app_wifi_events.h"

#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>
#include <common/include/rtos_err.h>
#include <common/source/kal/kal_priv.h>
#include <common/include/rtos_err.h>
#include "sl_wfx_task.h"
#include "sl_wfx_host.h"

const char *station_netif = "st";
const char *softap_netif = "ap";

/***************************************************************************//**
 * Initializes the hardware parameters. Called from ethernetif_init().
 *
 * @param netif The already initialized lwip network interface structure
 ******************************************************************************/
static void low_level_init(struct netif *netif)
{
  /* set netif MAC hardware address length*/
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set netif MAC hardware address*/
  if (memcmp(netif->name, station_netif, 2) == 0) {
    memcpy(netif->hwaddr, wifi.mac_addr_0.octet, 6);
  } else {
    memcpy(netif->hwaddr, wifi.mac_addr_1.octet, 6);
  }
  /* set netif maximum transfer unit*/
  netif->mtu = 1500;

  /* Accept broadcast address and ARP traffic*/
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  /* Set netif link flag*/
  netif->flags |= NETIF_FLAG_LINK_UP;
}
/***************************************************************************//**
 * @brief
 *    This function should does the actual transmission of the packet(s).
 *    The packet is contained in the pbuf that is passed to the function.
 *    This pbuf might be chained.
 *
 * @param[in] netif: the lwip network interface structure
 *
 * @param[in] p: the packet to send
 *
 * @return
 *    ERR_OK if successful
 ******************************************************************************/
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  RTOS_ERR err;
  struct pbuf *q;
  uint8_t *buffer;
  sl_wfx_packet_queue_item_t *queue_item;
  sl_status_t result;

  /* Take TX queue mutex */
  OSMutexPend(&sl_wfx_tx_queue_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);

  /* Allocate a buffer for a queue item */
  result = sl_wfx_allocate_command_buffer((sl_wfx_generic_message_t**)(&queue_item),
                                          SL_WFX_SEND_FRAME_REQ_ID,
                                          SL_WFX_TX_FRAME_BUFFER,
                                          p->tot_len + sizeof(sl_wfx_packet_queue_item_t));

  if ((result != SL_STATUS_OK) || (queue_item == NULL)) {
    /* Release TX queue mutex */
    OSMutexPost(&sl_wfx_tx_queue_mutex, OS_OPT_POST_NONE, &err);
    return ERR_MEM;
  }

  buffer = queue_item->buffer.body.packet_data;

  for (q = p; q != NULL; q = q->next) {
    /* Copy the bytes */
    memcpy(buffer, q->payload, q->len);
    buffer += q->len;
  }

  /* Provide the data length the interface information to the pbuf */
  queue_item->interface = (memcmp(netif->name, station_netif, 2) == 0) ?  SL_WFX_STA_INTERFACE : SL_WFX_SOFTAP_INTERFACE;
  queue_item->data_length = p->tot_len;

  /* Determine if there is anything on the tx packet queue */
  if (sl_wfx_tx_queue_context.head_ptr != NULL) {
    sl_wfx_tx_queue_context.tail_ptr->next = queue_item;
  } else {
    /* If tx packet queue is empty, setup head & tail pointers */
    sl_wfx_tx_queue_context.head_ptr = queue_item;
  }

  /* Update the tail pointer */
  sl_wfx_tx_queue_context.tail_ptr = queue_item;

  /* Notify that a TX frame is ready */
  OSFlagPost(&bus_events, SL_WFX_BUS_EVENT_FLAG_TX, OS_OPT_POST_FLAG_SET, &err);

  /* Release TX queue mutex */
  OSMutexPost(&sl_wfx_tx_queue_mutex, OS_OPT_POST_NONE, &err);

  return ERR_OK;
}

/***************************************************************************//**
 * Transfers the receive packets from the wfx to lwip.
 *
 * @param netif lwip network interface structure
 * @param rx_buffer the ethernet frame received by the wf200
 * @returns LwIP pbuf filled with received packet, or NULL on error
 ******************************************************************************/
static struct pbuf * low_level_input(struct netif *netif, sl_wfx_received_ind_t* rx_buffer)
{
  struct pbuf *p = NULL;
  struct pbuf *q = NULL;
  uint16_t len = 0;
  uint8_t *buffer;
  uint32_t bufferoffset = 0;
  uint32_t payloadoffset = 0;
  uint32_t byteslefttocopy = 0;
  /* get received frame */

  /* Obtain the size of the packet and put it into the "len" variable. */
  len = rx_buffer->body.frame_length;
  buffer = (uint8_t *)&(rx_buffer->body.frame[rx_buffer->body.frame_padding]);

  if (len > 0) {
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  }

  if (p != NULL) {
    bufferoffset = 0;
    for (q = p; q != NULL; q = q->next) {
      byteslefttocopy = q->len;
      payloadoffset = 0;

      /* Copy remaining data in pbuf */
      memcpy( (uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), byteslefttocopy);
      bufferoffset = bufferoffset + byteslefttocopy;
    }
  }

  return p;
}

/***************************************************************************//**
 * WFX received frame callback.
 *
 * @param rx_buffer the ethernet frame received by the wfx
 ******************************************************************************/
void sl_wfx_host_received_frame_callback(sl_wfx_received_ind_t* rx_buffer)
{
  struct pbuf *p;
  struct netif *netif;
  /* Check packet interface to send to AP or STA interface */
  if ((rx_buffer->header.info & SL_WFX_MSG_INFO_INTERFACE_MASK)
      == (SL_WFX_STA_INTERFACE << SL_WFX_MSG_INFO_INTERFACE_OFFSET)) {
    /* Send to station interface */
    netif = &sta_netif;
  } else {
    /* Send to softAP interface */
    netif = &ap_netif;
  }
  if (netif != NULL) {
    p = low_level_input(netif, rx_buffer);
    if (p != NULL) {
      if (netif->input(p, netif) != ERR_OK ) {
        pbuf_free(p);
      }
    }
  }
}

/***************************************************************************//**
 * Sets up the station network interface.
 *
 * @param netif the lwip network interface structure
 * @returns ERR_OK if successful
 ******************************************************************************/
err_t sta_ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip_sta";
#endif /* LWIP_NETIF_HOSTNAME */
  /* Set the netif name to identify the interface */
  memcpy(netif->name, station_netif, 2);

  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

/***************************************************************************//**
 * Sets up the AP network interface.
 *
 * @param netif the lwip network interface structure
 * @returns ERR_OK if successful
 ******************************************************************************/
err_t ap_ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip_ap";
#endif /* LWIP_NETIF_HOSTNAME */

  memcpy(netif->name, softap_netif, 2);

  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}
