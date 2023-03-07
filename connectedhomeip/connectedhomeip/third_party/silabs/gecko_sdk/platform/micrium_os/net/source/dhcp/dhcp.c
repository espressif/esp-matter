/***************************************************************************//**
 * @file
 * @brief Network DHCP Module
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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net/include/net_cfg_net.h>

#ifdef NET_DHCP_CLIENT_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "dhcp_priv.h"

#include  <net/include/dhcp.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const DHCP_OPT_DICT DHCP_OptDict[] = {
  { DHCP_OPT_CODE_PAD, 1 },

  { DHCP_OPT_CODE_SUBNET_MASK, 4 },
  { DHCP_OPT_CODE_TIME_OFFSET, 4 },
  { DHCP_OPT_CODE_ROUTER, 4 },
  { DHCP_OPT_CODE_TIME_SERVER, 4 },
  { DHCP_OPT_CODE_NAME_SERVER, 4 },
  { DHCP_OPT_CODE_DOMAIN_NAME_SERVER, 4 },
  { DHCP_OPT_CODE_LOG_SERVER, 4 },
  { DHCP_OPT_CODE_COOKIE_SERVER, 4 },
  { DHCP_OPT_CODE_LPR_SERVER, 4 },
  { DHCP_OPT_CODE_IMPRESS_SERVER, 4 },
  { DHCP_OPT_CODE_RESSOURCE_LOCATION_SERVER, 4 },
  { DHCP_OPT_CODE_HOST_NAME, 1 },
  { DHCP_OPT_CODE_BOOT_FILE_SIZE, 2 },
  { DHCP_OPT_CODE_MERIT_DUMP_FILE, 1 },
  { DHCP_OPT_CODE_DOMAIN_NAME, 1 },
  { DHCP_OPT_CODE_SWAP_SERVER, 4 },
  { DHCP_OPT_CODE_ROOT_PATH, 1 },
  { DHCP_OPT_CODE_EXTENSION_PATH, 1 },

  { DHCP_OPT_CODE_IP_FORWARDING, 1 },
  { DHCP_OPT_CODE_NON_LOCAL_SOURCE_ROUTING, 1 },
  { DHCP_OPT_CODE_POLICY_FILTER, 8 },
  { DHCP_OPT_CODE_MAXIMUM_DATAGRAM_REASSEMBLY_SIZE, 2 },
  { DHCP_OPT_CODE_DEFAULT_IP_TIME_TO_LIVE, 1 },
  { DHCP_OPT_CODE_PATH_MTU_AGING_TIMEOUT, 4 },
  { DHCP_OPT_CODE_PATH_MTU_PLATEAU_TABLE, 2 },

  { DHCP_OPT_CODE_INTERFACE_MTU, 2 },
  { DHCP_OPT_CODE_ALL_SUBNETS_ARE_LOCAL, 1 },
  { DHCP_OPT_CODE_BROADCAST_ADDRESS, 4 },
  { DHCP_OPT_CODE_PERFORM_MASK_DISCOVERY, 1 },
  { DHCP_OPT_CODE_MASK_SUPPLIER, 1 },
  { DHCP_OPT_CODE_PERFORM_ROUTER_DISCOVERY, 1 },
  { DHCP_OPT_CODE_ROUTER_SOLLICITATION_ADDRESS, 4 },
  { DHCP_OPT_CODE_STATIC_ROUTE, 8 },

  { DHCP_OPT_CODE_TRAILER_ENCAPSULATION, 1 },
  { DHCP_OPT_CODE_ARP_CACHE_TIMEOUT, 4 },
  { DHCP_OPT_CODE_ETHERNET_ENCAPSULATION, 1 },

  { DHCP_OPT_CODE_TCP_DEFAULT_TTL, 1 },
  { DHCP_OPT_CODE_TCP_KEEPALIVE_INTERVAL, 4 },
  { DHCP_OPT_CODE_TCP_KEEPALIVE_GARBAGE, 1 },

  { DHCP_OPT_CODE_NETWORK_INFORMATION_SERVICE_DOMAIN, 1 },
  { DHCP_OPT_CODE_NETWORK_INFORMATION_SERVER, 4 },
  { DHCP_OPT_CODE_NETWORK_TIME_PROTOCOL_SERVER, 4 },
  { DHCP_OPT_CODE_VENDOR_SPECIFIC_INFORMATION, 1 },
  { DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_NAME_SERVER, 4 },
  { DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_DATAGRAM_DISTRIBUTION_SERVER, 4 },
  { DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_NODE_TYPE, 1 },
  { DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_SCOPE, 1 },
  { DHCP_OPT_CODE_X_WINDOW_SYSTEM_FONT_SERVER, 4 },
  { DHCP_OPT_CODE_X_WINDOW_SYSTEM_DISPLAY_MANAGER, 4 },
  { DHCP_OPT_CODE_NETWORK_INFORMATION_SERVICE_PLUS_DOMAIN, 1 },
  { DHCP_OPT_CODE_NETWORK_INFORMATION_SERVICE_PLUS_SERVER, 4 },
  { DHCP_OPT_CODE_MOBILE_IP_HOME_AGENT, 0 },
  { DHCP_OPT_CODE_SIMPLE_MAIL_TRANSPORT_PROTOCOL_SERVER, 4 },
  { DHCP_OPT_CODE_POST_OFFICE_PROTOCOL_SERVER, 4 },
  { DHCP_OPT_CODE_NETWORK_NEWS_TRANSPORT_PROTOCOL_SERVER, 4 },
  { DHCP_OPT_CODE_DEFAULT_WORLD_WIDE_WEB_SERVER, 4 },
  { DHCP_OPT_CODE_DEFAULT_FINGER_SERVER, 4 },
  { DHCP_OPT_CODE_DEFAULT_INTERNET_RELAY_CHAT_SERVER, 4 },
  { DHCP_OPT_CODE_STREETTALK_SERVER, 4 },
  { DHCP_OPT_CODE_STREETTALK_DIRECTORY_ASSISTANCE_SERVER, 4 },

  { DHCP_OPT_CODE_REQUESTED_IP_ADDRESS, 4 },
  { DHCP_OPT_CODE_IP_ADDRESS_LEASE_TIME, 4 },
  { DHCP_OPT_CODE_OPTION_OVERLOAD, 1 },
  { DHCP_OPT_CODE_DHCP_MESSAGE_TYPE, 1 },
  { DHCP_OPT_CODE_SERVER_IDENTIFIER, 4 },
  { DHCP_OPT_CODE_PARAMETER_REQUEST_LIST, 1 },
  { DHCP_OPT_CODE_ERROR_MESSAGE, 1 },
  { DHCP_OPT_CODE_MAXIMUM_DHCP_MESSAGE_SIZE, 2 },
  { DHCP_OPT_CODE_RENEWAL_TIME_VALUE, 4 },
  { DHCP_OPT_CODE_REBINDING_TIME_VALUE, 4 },
  { DHCP_OPT_CODE_VENDOR_CLASS_IDENTIFIER, 1 },
  { DHCP_OPT_CODE_CLIENT_IDENTIFIER, 2 },
  { DHCP_OPT_CODE_TFTP_SERVER_NAME, 1 },
  { DHCP_OPT_CODE_BOOTFILE_NAME, 1 },

  { DHCP_OPT_CODE_END, 1 }
};

CPU_SIZE_T DHCP_OptDictSize = sizeof(DHCP_OptDict);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        DHCP_OptDictEntryGet()
 *
 * @brief    Retrieve the dictionary entry associated with the given DHCP option code.
 *
 * @param    code    DHCP option code
 *
 * @return   Pointer to dictionary entry, if no errors.
 *           DEF_NULL, otherwise
 *******************************************************************************************************/
DHCP_OPT_DICT *DHCP_OptDictEntryGet(DHCP_OPT_CODE code)
{
  DHCP_OPT_DICT *p_entry;
  CPU_INT32U    nbr_entry;
  CPU_INT32U    ix;

  nbr_entry = DHCP_OptDictSize / sizeof(DHCP_OPT_DICT);
  p_entry = (DHCP_OPT_DICT *)&DHCP_OptDict;
  for (ix = 0; ix < nbr_entry; ix++) {                          // Srch until last entry is reached.
    if (p_entry->Code == code) {                                // If keys match ...
      return (p_entry);                                         // ... the first entry is found.
    }

    p_entry++;                                                  // Move to next entry.
  }

  return (DEF_NULL);                                            // No entry found.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_DHCP_CLIENT_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
