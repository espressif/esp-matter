/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/****************************************************************************/
/*                                                                          */
/*  Name:       hiddefs.h                                                   */
/*                                                                          */
/*  Function:   this file contains HID protocol definitions                 */
/*                                                                          */
/****************************************************************************/

#ifndef HIDDEFS_H
#define HIDDEFS_H

#include "wiced_bt_sdp.h"

/*
** tHID_STATUS: HID result codes, returned by HID and device and host functions.
*/
enum
{
   HID_SUCCESS,
   HID_ERR_NOT_REGISTERED,
   HID_ERR_ALREADY_REGISTERED,
   HID_ERR_NO_RESOURCES,
   HID_ERR_NO_CONNECTION,
   HID_ERR_INVALID_PARAM,
   HID_ERR_UNSUPPORTED,
   HID_ERR_UNKNOWN_COMMAND,
   HID_ERR_CONGESTED,
   HID_ERR_CONN_IN_PROCESS,
   HID_ERR_ALREADY_CONN,
   HID_ERR_DISCONNECTING,
   HID_ERR_SET_CONNABLE_FAIL,
   /* Device specific error codes */
   HID_ERR_HOST_UNKNOWN,
   HID_ERR_L2CAP_FAILED,
   HID_ERR_AUTH_FAILED,
   HID_ERR_SDP_BUSY,
   HID_ERR_GATT,

   HID_ERR_INVALID = 0xFF
};

typedef UINT8 tHID_STATUS;

#define    HID_L2CAP_CONN_FAIL (0x0100) /* Connection Attempt was made but failed */
#define    HID_L2CAP_REQ_FAIL  (0x0200)  /* L2CAP_ConnectReq API failed */
#define    HID_L2CAP_CFG_FAIL  (0x0400) /* L2CAP Configuration was rejected by peer */



/* Define the HID transaction types
*/
#define HID_TRANS_HANDSHAKE     (0)
#define HID_TRANS_CONTROL       (1)
#define HID_TRANS_GET_REPORT    (4)
#define HID_TRANS_SET_REPORT    (5)
#define HID_TRANS_GET_PROTOCOL  (6)
#define HID_TRANS_SET_PROTOCOL  (7)
#define HID_TRANS_GET_IDLE      (8)
#define HID_TRANS_SET_IDLE      (9)
#define HID_TRANS_DATA          (10)
#define HID_TRANS_DATAC         (11)

#define HID_GET_TRANS_FROM_HDR(x) ((x >> 4) & 0x0f)
#define HID_GET_PARAM_FROM_HDR(x) (x & 0x0f)
#ifndef HID_BUILD_HDR
#define HID_BUILD_HDR(t,p)  (UINT8)((t << 4) | (p & 0x0f))
#endif
#define HID_HDR_LEN (1)

/* Parameters for Handshake
*/
#define HID_PAR_HANDSHAKE_RSP_SUCCESS               (0)
#define HID_PAR_HANDSHAKE_RSP_NOT_READY             (1)
#define HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID    (2)
#define HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ   (3)
#define HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM     (4)
#define HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN           (14)
#define HID_PAR_HANDSHAKE_RSP_ERR_FATAL             (15)


/* Parameters for Control
*/
#define HID_PAR_CONTROL_NOP                         (0)
#define HID_PAR_CONTROL_HARD_RESET                  (1)
#define HID_PAR_CONTROL_SOFT_RESET                  (2)
#define HID_PAR_CONTROL_SUSPEND                     (3)
#define HID_PAR_CONTROL_EXIT_SUSPEND                (4)
#define HID_PAR_CONTROL_VIRTUAL_CABLE_UNPLUG        (5)


/* Different report types in get, set, data
*/
#define HID_PAR_REP_TYPE_MASK                       (0x03)
#define HID_PAR_REP_TYPE_OTHER                      (0x00)
#define HID_PAR_REP_TYPE_INPUT                      (0x01)
#define HID_PAR_REP_TYPE_OUTPUT                     (0x02)
#define HID_PAR_REP_TYPE_FEATURE                    (0x03)

/* Parameters for Get Report
*/

/* Buffer size in two bytes after Report ID */
#define HID_PAR_GET_REP_BUFSIZE_FOLLOWS             (0x08)


/* Parameters for Protocol Type
*/
#define HID_PAR_PROTOCOL_MASK                       (0x01)
#define HID_PAR_PROTOCOL_REPORT                     (0x01)
#define HID_PAR_PROTOCOL_BOOT_MODE                  (0x00)

#define HID_PAR_REP_TYPE_MASK                       (0x03)

/* Descriptor types in the SDP record
*/
#define HID_SDP_DESCRIPTOR_REPORT                   (0x22)
#define HID_SDP_DESCRIPTOR_PHYSICAL                 (0x23)

#pragma pack(1)
typedef PACKED struct desc_info
{
    UINT16 dl_len;
    UINT8 *dsc_list;
} tHID_DEV_DSCP_INFO;
#pragma pack()

#define HID_SSR_PARAM_INVALID    0xffff

typedef struct sdp_info
{
    char svc_name[HID_MAX_SVC_NAME_LEN];   /*Service Name */
    char svc_descr[HID_MAX_SVC_DESCR_LEN]; /*Service Description*/
    char prov_name[HID_MAX_PROV_NAME_LEN]; /*Provider Name.*/
    UINT16    rel_num;    /*Release Number */
    UINT16    hpars_ver;  /*HID Parser Version.*/
    UINT16    ssr_max_latency; /* HIDSSRHostMaxLatency value, if HID_SSR_PARAM_INVALID not used*/
    UINT16    ssr_min_tout; /* HIDSSRHostMinTimeout value, if HID_SSR_PARAM_INVALID not used* */
    UINT8     sub_class;    /*Device Subclass.*/
    UINT8     ctry_code;     /*Country Code.*/
    UINT16    sup_timeout;/* Supervisory Timeout */

    /* DIS information */
    UINT8     vendor_id_src;
    UINT16    vendor_id;            /* vendor ID */
    UINT16    product_id;           /* product ID */
    UINT16    product_version;       /* version */
    UINT64    system_id;         /* system ID */

    tHID_DEV_DSCP_INFO  dscp_info;   /* Descriptor list and Report list to be set in the SDP record.
                                       This parameter is used if HID_DEV_USE_GLB_SDP_REC is set to FALSE.*/
    wiced_bt_sdp_discovery_record_t       *p_sdp_layer_rec;
} tHID_DEV_SDP_INFO;

#endif
