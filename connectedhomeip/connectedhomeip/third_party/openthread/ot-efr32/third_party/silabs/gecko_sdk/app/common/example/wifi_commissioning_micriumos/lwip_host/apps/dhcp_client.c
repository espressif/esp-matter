/***************************************************************************//**
 * @file
 * @brief LwIP DHCP client implementation
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
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"
#include "app_webpage.h"
#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>
#include <common/include/rtos_err.h>
#include <common/source/kal/kal_priv.h>

// DHCP client states
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

#define MAX_DHCP_TRIES  4

/// Current DHCP state machine state.
static volatile uint8_t dhcp_state = DHCP_OFF;

#define DHCP_TASK_PRIO              22u
#define DHCP_TASK_STK_SIZE         512u

/// DHCP client task stack
static CPU_STK dhcp_task_stk[DHCP_TASK_STK_SIZE];
/// DHCP client task TCB
static OS_TCB dhcp_task_tcb;

/***************************************************************************//**
 * Notify DHCP client task about the wifi status
 *
 * @param link_up link status
 ******************************************************************************/
void dhcpclient_set_link_state(int link_up)
{
  if (link_up) {
    dhcp_state = DHCP_START;
  } else {
    /* Update DHCP state machine */
    dhcp_state = DHCP_LINK_DOWN;
  }
}

/***************************************************************************//**
 * DHCP client task.
 *
 * @param arg Network interface
 ******************************************************************************/
static void dhcp_client_task(void *arg)
{
  struct netif *netif = (struct netif *) arg;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  struct dhcp *dhcp;
  for (;; ) {
    switch (dhcp_state) {
      case DHCP_START:
      {
        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);
        netifapi_dhcp_start(netif);
        dhcp_state = DHCP_WAIT_ADDRESS;
      }
      break;

      case DHCP_WAIT_ADDRESS:
      {
        if (dhcp_supplied_address(netif)) {
          dhcp_state = DHCP_ADDRESS_ASSIGNED;
          printf("IP address : %d.%d.%d.%d\r\n",
                 (uint8_t)(sta_netif.ip_addr.addr & 0xff),
                 (uint8_t)(sta_netif.ip_addr.addr >> 8),
                 (uint8_t)(sta_netif.ip_addr.addr >> 16),
                 (uint8_t)(sta_netif.ip_addr.addr >> 24));
        } else {
          dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

          // DHCP timeout
          if (dhcp->tries > MAX_DHCP_TRIES) {
            dhcp_state = DHCP_TIMEOUT;

            // Stop DHCP
            netifapi_dhcp_stop(netif);

            // Static address used
            IP_ADDR4(&ipaddr, sta_ip_addr0, sta_ip_addr1, sta_ip_addr2, sta_ip_addr3);
            IP_ADDR4(&netmask, sta_netmask_addr0, sta_netmask_addr1, sta_netmask_addr2, sta_netmask_addr3);
            IP_ADDR4(&gw, sta_gw_addr0, sta_gw_addr1, sta_gw_addr2, sta_gw_addr3);
            netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
          }
        }
      }
      break;
      case DHCP_LINK_DOWN:
      {
        // Stop DHCP
        netifapi_dhcp_stop(netif);
        dhcp_state = DHCP_OFF;
      }
      break;
      default: break;
    }

    // wait 250 ms
    KAL_Dly(250);
  }
}

/***************************************************************************//**
 * Start DHCP client task.
 ******************************************************************************/
void dhcpclient_start(void)
{
  RTOS_ERR err;
  OSTaskCreate(&dhcp_task_tcb,
               "DHCP Task",
               dhcp_client_task,
               &sta_netif,
               DHCP_TASK_PRIO,
               &dhcp_task_stk[0],
               (DHCP_TASK_STK_SIZE / 10u),
               DHCP_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //   Check error code.
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}
