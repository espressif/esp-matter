/*******************************************************************************
* @file  rsi_bt_sdp.h
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

#ifndef RSI_BT_SDP_H
#define RSI_BT_SDP_H

#include <rsi_data_types.h>
#include <rsi_bt_common.h>
#include <rsi_bt_apis.h>

#define BT_SERVICE_UUID_PUBLIC_BROWSE_GROUP 0x1002
#define BT_SERVICE_UUID_HID                 0x1124
#define BT_HID_CONTROL_PSM                  0x0011
#define BT_HID_INTERRUPT_PSM                0x0013

#define BT_SDP_LANG_ATTR_NATURAL_LANG_ID 0x656E /* 'en' */
#define BT_SDP_LANG_ATTR_ENCODING_UTF8   0x006A /* UTF-8 */
#define BT_SDP_LANG_ATTR_BASE_ATTR_ID    0x0100

#define BT_ATTR_ID_SERVICE_RECORD_HANDLE         0x0000
#define BT_ATTR_ID_SERVICE_CLASS_ID_LIST         0x0001
#define BT_ATTR_ID_SERVICE_RECORD_STATE          0x0002
#define BT_ATTR_ID_SERVICE_ID                    0x0003
#define BT_ATTR_ID_PROTOCOL_DESC_LIST            0x0004
#define BT_ATTR_ID_BROWSE_GROUP_LIST             0x0005
#define BT_ATTR_ID_LANGUAGE_BASE_ATTR_ID_LIST    0x0006
#define BT_ATTR_ID_SERVICE_INFO_TIME_TO_LIVE     0x0007
#define BT_ATTR_ID_SERVICE_AVAILABILITY          0x0008
#define BT_ATTR_ID_BLUETOOTH_PROFILE_DESC_LIST   0x0009
#define BT_ATTR_ID_DOCUMENTATION_URL             0x000A
#define BT_ATTR_ID_CLIENT_EXECUTABLE_URL         0x000B
#define BT_ATTR_ID_ICON_URL                      0x000C
#define BT_ATTR_ID_ADDITIONAL_PROTOCOL_DESC_LIST 0x000D

#define BT_ATTR_ID_ENGLISH_SERVICE_NAME        0x0100
#define BT_ATTR_ID_ENGLISH_SERVICE_DESCRIPTION 0x0101
#define BT_ATTR_ID_ENGLISH_PROVIDER_NAME       0x0102

#define BT_ATTR_ID_HID_VERSION_NUM          0x0200
#define BT_ATTR_ID_HID_PARSER_VERSION       0x0201
#define BT_ATTR_ID_HID_DEVICE_SUBCLASS      0x0202
#define BT_ATTR_ID_HID_COUNTRY_CODE         0x0203
#define BT_ATTR_ID_HID_VIRTUAL_CABLE        0x0204
#define BT_ATTR_ID_HID_RECONNECT_INITIATE   0x0205
#define BT_ATTR_ID_HID_DESCRIPTIOR_LIST     0x0206
#define BT_ATTR_ID_HID_LANG_ID_BASE_LIST    0x0207
#define BT_ATTR_ID_HID_SDP_DISABLE          0x0208
#define BT_ATTR_ID_HID_BATTERY_POWER        0x0209
#define BT_ATTR_ID_HID_REMOTE_WAKE          0x020A
#define BT_ATTR_ID_HID_PROFILE_VERSION      0x020B
#define BT_ATTR_ID_HID_SUPERVISION_TIMEOUT  0x020C
#define BT_ATTR_ID_HID_NORMALLY_CONNECTABLE 0x020D
#define BT_ATTR_ID_HID_BOOT_DEVICE          0x020E
#define BT_ATTR_ID_HID_SSR_HOST_MAX_LATENCY 0x020F
#define BT_ATTR_ID_HID_SSR_HOST_MIN_LATENCY 0x0210

typedef struct attr_id_array_s {
  uint16_t attr_id;
  uint16_t attr_buf_ptr_idx;
  uint16_t attr_buf_len;
} attr_id_array_t;

typedef struct rsi_sdp_att_record_s {
#define MAX_SDP_ATTR 27
  attr_id_array_t attr_array[MAX_SDP_ATTR];
  uint16_t attr_array_cnt;
#define MAX_SDP_BUFF_LEN 500
  uint16_t buf_len;
  uint8_t buf_array[MAX_SDP_BUFF_LEN];
} rsi_sdp_att_record_t;

#define BT_ATTR_TYPE_NIL              0x00
#define BT_ATTR_TYPE_UNSIGNED_INTEGER 0x01
#define BT_ATTR_TYPE_SIGNED_INTEGER   0x02
#define BT_ATTR_TYPE_UUID             0x03
#define BT_ATTR_TYPE_TEXT_STRING      0x04
#define BT_ATTR_TYPE_BOOLEAN          0x05
#define BT_ATTR_TYPE_SEQUENCE         0x06
#define BT_ATTR_TYPE_ALT_SEQUENCE     0x07
#define BT_ATTR_TYPE_URL              0x08

#define BT_MEM_WR_BE_08U(addr, val)                                                   \
  {                                                                                   \
    (*(((uint8_t *)(addr)) + 0)) = ((uint8_t)((((uint8_t)(val)) & 0xFF) >> (0 * 8))); \
  }

#define BT_MEM_WR_BE_16U(addr, val)                                                      \
  {                                                                                      \
    (*(((uint8_t *)(addr)) + 0)) = ((uint8_t)((((uint16_t)(val)) & 0xFF00) >> (1 * 8))); \
    (*(((uint8_t *)(addr)) + 1)) = ((uint8_t)((((uint16_t)(val)) & 0x00FF) >> (0 * 8))); \
  }

#define BT_MEM_WR_BE_32U(addr, val)                                                          \
  {                                                                                          \
    (*(((uint8_t *)(addr)) + 0)) = ((uint8_t)((((uint32_t)(val)) & 0xFF000000) >> (3 * 8))); \
    (*(((uint8_t *)(addr)) + 1)) = ((uint8_t)((((uint32_t)(val)) & 0x00FF0000) >> (2 * 8))); \
    (*(((uint8_t *)(addr)) + 2)) = ((uint8_t)((((uint32_t)(val)) & 0x0000FF00) >> (1 * 8))); \
    (*(((uint8_t *)(addr)) + 3)) = ((uint8_t)((((uint32_t)(val)) & 0x000000FF) >> (0 * 8))); \
  }

//protocol uuid's
#define BT_PROTOCOL_UUID_L2CAP  0x0100
#define BT_PROTOCOL_UUID_RFCOMM 0x0003
#define BT_PROTOCOL_UUID_ATT    0x0007
#define BT_PROTOCOL_UUID_HID    0x0011
#define BT_PROTOCOL_UUID_AVCTP  0x0017
#define BT_PROTOCOL_UUID_A2DP   0x0019

typedef struct bt_sdp_proto_desc_list_elem {
  uint16_t proto_uuid;
  uint16_t num_params;
  uint16_t params[2];
} bt_sdp_proto_desc_list_elem_t;

/* sdp language base attribute ID list, data element stucture */
typedef struct bt_sdp_lang_attr_id_elem {
  uint16_t lang_id;
  uint16_t lang_encode_id;
  uint16_t base_attr_id;
} bt_sdp_lang_attr_id_elem_t;

#endif
