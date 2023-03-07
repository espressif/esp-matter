/***************************************************************************//**
 * @file
 * @brief Network - Dhcp Header File
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _DHCP_H_
#define  _DHCP_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  enum  dhcp_opt_code {
  DHCP_OPT_CODE_PAD = 0,

  DHCP_OPT_CODE_SUBNET_MASK = 1,
  DHCP_OPT_CODE_TIME_OFFSET = 2,
  DHCP_OPT_CODE_ROUTER = 3,
  DHCP_OPT_CODE_TIME_SERVER = 4,
  DHCP_OPT_CODE_NAME_SERVER = 5,
  DHCP_OPT_CODE_DOMAIN_NAME_SERVER = 6,
  DHCP_OPT_CODE_LOG_SERVER = 7,
  DHCP_OPT_CODE_COOKIE_SERVER = 8,
  DHCP_OPT_CODE_LPR_SERVER = 9,
  DHCP_OPT_CODE_IMPRESS_SERVER = 10,
  DHCP_OPT_CODE_RESSOURCE_LOCATION_SERVER = 11,
  DHCP_OPT_CODE_HOST_NAME = 12,
  DHCP_OPT_CODE_BOOT_FILE_SIZE = 13,
  DHCP_OPT_CODE_MERIT_DUMP_FILE = 14,
  DHCP_OPT_CODE_DOMAIN_NAME = 15,
  DHCP_OPT_CODE_SWAP_SERVER = 16,
  DHCP_OPT_CODE_ROOT_PATH = 17,
  DHCP_OPT_CODE_EXTENSION_PATH = 18,

  DHCP_OPT_CODE_IP_FORWARDING = 19,
  DHCP_OPT_CODE_NON_LOCAL_SOURCE_ROUTING = 20,
  DHCP_OPT_CODE_POLICY_FILTER = 21,
  DHCP_OPT_CODE_MAXIMUM_DATAGRAM_REASSEMBLY_SIZE = 22,
  DHCP_OPT_CODE_DEFAULT_IP_TIME_TO_LIVE = 23,
  DHCP_OPT_CODE_PATH_MTU_AGING_TIMEOUT = 24,
  DHCP_OPT_CODE_PATH_MTU_PLATEAU_TABLE = 25,

  DHCP_OPT_CODE_INTERFACE_MTU = 26,
  DHCP_OPT_CODE_ALL_SUBNETS_ARE_LOCAL = 27,
  DHCP_OPT_CODE_BROADCAST_ADDRESS = 28,
  DHCP_OPT_CODE_PERFORM_MASK_DISCOVERY = 29,
  DHCP_OPT_CODE_MASK_SUPPLIER = 30,
  DHCP_OPT_CODE_PERFORM_ROUTER_DISCOVERY = 31,
  DHCP_OPT_CODE_ROUTER_SOLLICITATION_ADDRESS = 32,
  DHCP_OPT_CODE_STATIC_ROUTE = 33,

  DHCP_OPT_CODE_TRAILER_ENCAPSULATION = 34,
  DHCP_OPT_CODE_ARP_CACHE_TIMEOUT = 35,
  DHCP_OPT_CODE_ETHERNET_ENCAPSULATION = 36,

  DHCP_OPT_CODE_TCP_DEFAULT_TTL = 37,
  DHCP_OPT_CODE_TCP_KEEPALIVE_INTERVAL = 38,
  DHCP_OPT_CODE_TCP_KEEPALIVE_GARBAGE = 39,

  DHCP_OPT_CODE_NETWORK_INFORMATION_SERVICE_DOMAIN = 40,
  DHCP_OPT_CODE_NETWORK_INFORMATION_SERVER = 41,
  DHCP_OPT_CODE_NETWORK_TIME_PROTOCOL_SERVER = 42,
  DHCP_OPT_CODE_VENDOR_SPECIFIC_INFORMATION = 43,
  DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_NAME_SERVER = 44,
  DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_DATAGRAM_DISTRIBUTION_SERVER = 45,
  DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_NODE_TYPE = 46,
  DHCP_OPT_CODE_NETBIOS_OVER_TCPIP_SCOPE = 47,
  DHCP_OPT_CODE_X_WINDOW_SYSTEM_FONT_SERVER = 48,
  DHCP_OPT_CODE_X_WINDOW_SYSTEM_DISPLAY_MANAGER = 49,
  DHCP_OPT_CODE_NETWORK_INFORMATION_SERVICE_PLUS_DOMAIN = 64,
  DHCP_OPT_CODE_NETWORK_INFORMATION_SERVICE_PLUS_SERVER = 65,
  DHCP_OPT_CODE_MOBILE_IP_HOME_AGENT = 68,
  DHCP_OPT_CODE_SIMPLE_MAIL_TRANSPORT_PROTOCOL_SERVER = 69,
  DHCP_OPT_CODE_POST_OFFICE_PROTOCOL_SERVER = 70,
  DHCP_OPT_CODE_NETWORK_NEWS_TRANSPORT_PROTOCOL_SERVER = 71,
  DHCP_OPT_CODE_DEFAULT_WORLD_WIDE_WEB_SERVER = 72,
  DHCP_OPT_CODE_DEFAULT_FINGER_SERVER = 73,
  DHCP_OPT_CODE_DEFAULT_INTERNET_RELAY_CHAT_SERVER = 74,
  DHCP_OPT_CODE_STREETTALK_SERVER = 75,
  DHCP_OPT_CODE_STREETTALK_DIRECTORY_ASSISTANCE_SERVER = 76,

  DHCP_OPT_CODE_REQUESTED_IP_ADDRESS = 50,
  DHCP_OPT_CODE_IP_ADDRESS_LEASE_TIME = 51,
  DHCP_OPT_CODE_OPTION_OVERLOAD = 52,
  DHCP_OPT_CODE_DHCP_MESSAGE_TYPE = 53,
  DHCP_OPT_CODE_SERVER_IDENTIFIER = 54,
  DHCP_OPT_CODE_PARAMETER_REQUEST_LIST = 55,
  DHCP_OPT_CODE_ERROR_MESSAGE = 56,
  DHCP_OPT_CODE_MAXIMUM_DHCP_MESSAGE_SIZE = 57,
  DHCP_OPT_CODE_RENEWAL_TIME_VALUE = 58,
  DHCP_OPT_CODE_REBINDING_TIME_VALUE = 59,
  DHCP_OPT_CODE_VENDOR_CLASS_IDENTIFIER = 60,
  DHCP_OPT_CODE_CLIENT_IDENTIFIER = 61,
  DHCP_OPT_CODE_TFTP_SERVER_NAME = 66,
  DHCP_OPT_CODE_BOOTFILE_NAME = 67,

  DHCP_OPT_CODE_END = 255
} DHCP_OPT_CODE;

typedef  enum  dhcp_opt_type {
  DHCP_OPT_TYPE_CODE,
  DHCP_OPT_TYPE_CODE_INT,
  DHCP_OPT_TYPE_CODE_STR,
  DHCP_OPT_TYPE_CODE_LIST
} DHCP_OPT_TYPE;

typedef  struct  dhcp_opt_tbl {
  DHCP_OPT_TYPE EntryType;
  void          *EntryPtr;
} DHCP_OPT_TBL;

typedef  struct dhcp_opt_tbl_entry_code {
  DHCP_OPT_CODE Code;
} DHCP_OPT_TBL_ENTRY_CODE;

typedef  struct dhcp_opt_tbl_entry_int {
  DHCP_OPT_CODE Code;
  CPU_SIZE_T    IntVal;
} DHCP_OPT_TBL_ENTRY_INT;

typedef  struct dhcp_opt_tbl_entry_str {
  DHCP_OPT_CODE Code;
  CPU_CHAR      *StrVal;
} DHCP_OPT_TBL_ENTRY_STR;

typedef  struct dhcp_opt_tbl_entry_list {
  DHCP_OPT_CODE Code;
  void          *ListPtr;
  CPU_SIZE_T    ElementNbr;
  CPU_SIZE_T    ElementSize;
} DHCP_OPT_TBL_ENTRY_LIST;

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _DHCP_H_
