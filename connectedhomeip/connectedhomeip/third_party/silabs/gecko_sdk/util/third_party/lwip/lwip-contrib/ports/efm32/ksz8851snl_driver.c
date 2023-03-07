/***************************************************************************//**
 * @brief driver wrapper for Ethernet controller Micrel KSZ8851SNL used in lwIP
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>

#ifndef ETH_USE_TINY_PRINTF
#include <stdio.h>
#else
#include "printf.h"
#endif

#include "ksz8851snl_driver.h"

#include "ksz8851snl.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include "em_device.h"
#include "em_core.h"
#include "em_gpio.h"

#include "bsp.h"

#include "lwipopts.h"
#include "lwip/debug.h"

#ifndef LWIP4EFM32
#error "lwiopts.h for EFM32 are not included"
#endif

#define LINKGOOD_MASK           0x0020

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
static struct pbuf* ksz8851snl_driver_low_level_input(struct netif *netif);
/** @endcond */

/****************************************************************************//**
 * @brief
 *   Initialize lwIP internal variables and structures.
 *
 * @note
 *   It should be passed to netif_add.
 *
 * @param[in] netif
 *   the used network interface
 *
 * @return
 *   error status
 *****************************************************************************/
err_t ksz8851snl_driver_init(struct netif *netif)
{
  EFM_ASSERT(netif != NULL);

#if LWIP_NETIF_HOSTNAME
  netif->hostname = "efm32";               /* initialize interface hostname */
#endif

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;

  /* Set the output methods to be used */
  netif->output     = etharp_output;
  netif->linkoutput = ksz8851snl_driver_output;

  /* Set the HW address length */
  netif->hwaddr_len = ETH_MAC_ADDR_LEN;

  /* Set the HW address */
  KSZ8851SNL_MacAddressGet(netif->hwaddr);

  /* Set the maximum transfer unit */
  netif->mtu = 1500;

  /* Set the flags according to device capabilities */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  /* Initialize the hardware */
  KSZ8851SNL_Init();

  return ERR_OK;
}


/****************************************************************************//**
 * @brief
 *   Transmit a raw packet.
 *
 * @param[in] netif
 *   The network interface
 *
 * @param[in] p
 *   The raw packet to be sent
 *
 * @return
 *   error status
 *****************************************************************************/
err_t ksz8851snl_driver_output(struct netif *netif, struct pbuf *p)
{
  struct eth_hdr *ethhdr = p->payload;
  struct pbuf    *q;
  CORE_DECLARE_IRQ_STATE;

  EFM_ASSERT(netif != NULL);
  EFM_ASSERT(p != NULL);
  EFM_ASSERT(ethhdr != NULL);

#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

  /* Tx must be done with interrupts disabled */
  CORE_ENTER_ATOMIC();

  /* Check for LwIP chained buffers */
  if (p->next)
  {
    if (KSZ8851SNL_TransmitBegin(p->tot_len))
    {
      for (q = p; q != NULL; q = q->next)
      {
        KSZ8851SNL_Transmit(q->len, q->payload);
      }
      KSZ8851SNL_TransmitEnd(p->tot_len);
    }
    else
    {
      /* else - PHY is out of memory - drop this packet */
      LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ksz8851snl_driver: Unable to send packet of size %u\n", p->len));
    }
  }
  else
  {
    /* When only one pbuf, use p->len */
    if (KSZ8851SNL_TransmitBegin(p->len))
    {
      KSZ8851SNL_Transmit(p->len, p->payload);
      KSZ8851SNL_TransmitEnd(p->len);
    }
    else
    {
      /* else - PHY is out of memory - drop this packet */
      LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ksz8851snl_driver: Unable to send packet of size %u\n", p->len));
    }
  }

  CORE_EXIT_ATOMIC();

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/****************************************************************************//**
 * @brief
 *   Pass the received raw packet to lwIP, using LWIPDRIVER_LowLevelInput() to
 *   handle the actual receival of data from the ethernet controller,
 *   to the appropriate handle from lwIP
 *
 * @param[in] netif
 *   The network interface
 *****************************************************************************/
void ksz8851snl_driver_input(struct netif *netif)
{
  struct eth_hdr *ethhdr;
  struct pbuf    *p;
  err_t          err;

  EFM_ASSERT(netif != NULL);

  CORE_ATOMIC_SECTION(
    KSZ8851SNL_FrameCounterSet();
  )

  /* Process frames. LWIPDRIVER_LowLevelInput()/KSZ8851SNL_Receive() decrements the frame count. */
  while (KSZ8851SNL_FrameCounterGet() > 0)
  {
    /* Frame Counter is decremented by KSZ8851SNL_Receive - some frames might be dropped due to errors */
    /* This is a recommendation in a programmer's guide */
    CORE_ATOMIC_SECTION(
      p = ksz8851snl_driver_low_level_input(netif);
    )
    /* no packet could be read, ignore, but report as DBG_3 */
    if (p == NULL)
    {
      LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ksz8851snl_driver: ksz8851snl_driver_low_level_input returned NULL\n"));
      continue;
    }
    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = p->payload;

    switch (htons(ethhdr->type))
    {
    /* IP or ARP packet? */
    case ETHTYPE_IP:
    case ETHTYPE_ARP:
  #if PPPOE_SUPPORT
    /* PPPoE packet? */
    case ETHTYPE_PPPOEDISC:
    case ETHTYPE_PPPOE:
  #endif /* PPPOE_SUPPORT */
         /* full packet send to tcpip_thread to process */
      err = netif->input(p, netif);
      if (err != ERR_OK)
      {
        LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ksz8851snl_driver: netif->input input error err=%d\n", err));
        pbuf_free(p);
        p = NULL;
      }
      break;

    case ETHTYPE_IPV6:
      LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ksz8851snl_driver: IPv6 frame type is not supported. Frame ignored.\n"));
      pbuf_free(p);
      p = NULL;
      break;

    default:
      LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ksz8851snl_driver: Unsupported Ethernet type = 0x%04lX\n", (uint32_t) htons(ethhdr->type)));
      pbuf_free(p);
      p = NULL;
      break;
    }
  }
}

/****************************************************************************//**
 * @brief
 *   Tries to allocate a pbuf and transfers the data received from the
 *   ethernet controller into the pbuf
 *
 * @param[in] netif
 *   The network interface
 *
 * @return
 *   The filled pbuf with the received data or NULL otherwise
 *****************************************************************************/
static struct pbuf *ksz8851snl_driver_low_level_input(struct netif *netif)
{
  EFM_ASSERT(netif != NULL);
  struct pbuf *p;
  uint16_t    frameLength;

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, PBUF_POOL_BUFSIZE, PBUF_POOL);
  if (p != NULL)
  {
    frameLength = KSZ8851SNL_Receive(PBUF_POOL_BUFSIZE, p->payload);
    if (frameLength != 0)
    {
      p->len = frameLength;
      if (p->next == NULL)
      {
        p->tot_len = frameLength;
      }
      LINK_STATS_INC(link.recv);
    }
    else
    {
      LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ksz8851snl_driver: KSZ8851SNL_Receive returns NULL frameLength \n"));
      pbuf_free(p);
      p = NULL;
    }
  }
  else
  {
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_LEVEL_WARNING, ("LWIPDRIVER_LowLevelInput: pbuff_alloc returns NULL \n"));
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }
  return p;
}

/****************************************************************************//**
 * @brief
 *   Handle an interrupt from the phy hardware
 *
 * @param[in] netif
 *   The network interface
 *****************************************************************************/
static void ksz8851snl_driver_phy(struct netif *netif)
{
  uint16_t phyStatus = KSZ8851SNL_PHYStatusGet();
  if (phyStatus & LINKGOOD_MASK)
  {
    netif_set_link_up(netif);
  }
  else
  {
    netif_set_link_down(netif);
  }
}

/***************************************************************************//**
 * @brief
 *   Handle an interrupt from the ksz8851snl ethernet controller.
 *
 * @details
 *   This can involve reading data from the rx queue and sending it to the ip
 *   stack. This code should be called from the main loop and not from the
 *   gpio isr since it will call into the ip stack to manage received packets.
  *
 * @param[in] netif
 *   The network interface
 ******************************************************************************/
void ksz8851snl_driver_isr(struct netif *netif)
{
  KSZ8851SNL_IntDisable();
  uint16_t ethIsrFlags = KSZ8851SNL_IntGet();
  KSZ8851SNL_IntClear(ethIsrFlags);

  if (ethIsrFlags & KSZ8851SNL_INT_LINKUP)
  {
    KSZ8851SNL_PMECRStatusClear(0x003C);
    ksz8851snl_driver_phy(netif);
  }

  if (ethIsrFlags & KSZ8851SNL_INT_LINK_CHANGE)
  {
    ksz8851snl_driver_phy(netif);
  }

  if (ethIsrFlags & KSZ8851SNL_INT_SPI_ERROR)
  {
    /* Loop here to enable the debugger to see what has happened */
    while (1)
      ;
  }

  if (ethIsrFlags & KSZ8851SNL_INT_RX_STOPPED)
  {
    KSZ8851SNL_RxQueueReset();
  }

  if (ethIsrFlags & KSZ8851SNL_INT_TX_STOPPED)
  {
    KSZ8851SNL_TxQueueReset();
  }

  if (ethIsrFlags & KSZ8851SNL_INT_RX_DONE)
  {
    ksz8851snl_driver_input(netif);
  }

  KSZ8851SNL_IntEnable();
}

