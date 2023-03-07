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
#ifndef BUILDCFG_H
#define BUILDCFG_H
#include "brcm_fw_types.h"
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

#define memcpy   mpaf_memcpy
#define memset   mpaf_memset
#define memmove  mpaf_memmove

void * mpaf_memcpy(void *dst, const void *src, int len);
void * mpaf_memset (void * ptr, int value, int num );
void * mpaf_memmove ( void * destination, const void * source, int num );


/*MPAF bt stack configuration*/
#ifdef MPAF_BT_STACK_ENABLE

#define BR_INCLUDED         TRUE

#define SDP_INCLUDED        BR_INCLUDED
#define RFCOMM_INCLUDED     BR_INCLUDED

#define GAP_INCLUDED   TRUE
#define GKI_MAX_TASKS  1

#define GAP_MAX_CONNECTIONS         1   // bt_target.h by default set to 8. Is it fine to keep to 1?


#define SDP_SERVER_ENABLED          BR_INCLUDED
#define SDP_CLIENT_ENABLED          BR_INCLUDED

/* The maximum length, in bytes, of an attribute. */
//#define SDP_MAX_ATTR_LEN            80 Can we remove this in embedded stack config. in bt_traget.h defined as 256


/* The MTU size for the L2CAP configuration. */
//#define SDP_MTU_SIZE                64 Can we remove this? in bt_target.h defined as 256

//Can we make HID_HOST_MAX_DEVICES = 1 in embedded stack?
#ifdef B3_ROM_OPTIMIZATION
#   define HID_HOST_MAX_DEVICES    4
#else
#   define HID_HOST_MAX_DEVICES    5 /* 3 for UHE, 1 for RC, 1 reserved */
#endif

//bt_target.h defined desired role as peripheral. Do we need below desired role  from buildcfg?
#define L2CAP_DESIRED_LINK_ROLE (HCI_ROLE_CENTRAL)


// --- stack customizations ---

// BTM
//#define BTM_BUSY_LEVEL_CHANGE_INCLUDED  TRUE Do we need this customization?


#define BTM_EIR_SERVER_INCLUDED         BR_INCLUDED
#define BTM_EIR_CLIENT_INCLUDED         BR_INCLUDED

#define BTM_SSR_INCLUDED                FALSE // snif subrate. Do we need to enable this? bt_traget.h defined this to TRUE
#define BTM_PRE_LISBON_INCLUDED         FALSE // Do we need to enbale this? bt_traget.h defined this to TRUE


// BTM -- reintroduced old flags
#define BTM_AUTHORIZATION_INCLUDED      FALSE // Do we need to enbale this? bt_traget.h defined this to TRUE


// Reconfiguration. Can we make below l2c as default config? btewiced stack recvd with below l2c configuration
#define L2CAP_WAKE_PARKED_LINK          FALSE
#define L2CAP_NON_FLUSHABLE_PB_INCLUDED FALSE
#define L2CAP_HCI_FLOW_CONTROL_DEBUG    FALSE
#define L2CAP_ROUND_ROBIN_CHANNEL_SERVICE   FALSE

//#undef     BTM_BUSY_LEVEL_CHANGE_INCLUDED Todo remove if above BTM_BUSY_LEVEL_CHANGE_INCLUDED configuration is not required
//#undef    SDP_MAX_ATTR_LEN Todo remove if above BTM_BUSY_LEVEL_CHANGE_INCLUDED configuration is not required

#define MPAF_CUSTOM_STACK               TRUE // Todo Need to remove this and references

#define BTEWICED                        TRUE

#define BTM_CMD_POOL_ID                 GKI_POOL_ID_1
#define BTM_CLB_INCLUDED                FALSE
#define BTM_CLB_RX_INCLUDED             FALSE
#define BTM_TBFC_INCLUDED               FALSE
#define BTM_INQ_DB_INCLUDED             FALSE
#define BTM_USE_INQ_RESULTS_FILTER      FALSE
#define BTM_SEC_FORCE_RNR_FOR_DBOND     FALSE
#define BTM_PWR_MGR_INCLUDED            FALSE
#define BTM_SCO_WAKE_PARKED_LINK        FALSE
#define BTM_BUSY_LEVEL_CHANGE_INCLUDED  FALSE
#define BTM_ALLOW_CONN_IF_NONDISCOVER   TRUE
#define BTM_MAX_REM_BD_NAME_LEN         10
#define BTM_DUMO_ADDR_CENTRAL_ENABLED   FALSE
#define BTM_SCO_INCLUDED                BR_INCLUDED
#define BTM_SCO_HCI_INCLUDED            BR_INCLUDED       /* TRUE includes SCO over HCI code */
#define BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED   FALSE
#define BTM_BLE_PRIVACY_SPT                     TRUE
#define BTM_USE_CONTROLLER_PRIVATE_ADDRESS      TRUE
#define BTM_BLE_PRIVATE_ADDR_INT        (p_btm_cfg_settings->rpa_refresh_timeout)

/* When automatic inquiry scan is enabled, this sets the inquiry scan window. */
#ifndef BTM_DEFAULT_DISC_WINDOW
#define BTM_DEFAULT_DISC_WINDOW     0x0012
#endif

#define BTU_DYNAMIC_CB_INCLUDED         TRUE
#define A2DP_SINK_DYNAMIC_MEMORY        TRUE

#define L2CAP_CMD_POOL_ID               GKI_POOL_ID_1
#define L2CAP_FCR_INCLUDED              BR_INCLUDED
#define L2CAP_UCD_INCLUDED              FALSE
#define L2CAP_MTU_SIZE                  ((UINT16)(HCI_ACL_POOL_BUF_SIZE - BT_HDR_SIZE - 8))     /* ACL bufsize minus BT_HDR, and l2cap/hci packet headers */
#define L2CAP_LE_COC_INCLUDED           TRUE
#define L2CAP_LINK_INACTIVITY_TOUT      3

#define SMP_LE_SC_INCLUDED              TRUE
#define SMP_LE_SC_OOB_INCLUDED          TRUE


#define RFCOMM_USE_EXTERNAL_SCN         BR_INCLUDED
#define RFCOMM_CMD_POOL_ID              GKI_POOL_ID_1
#define RFCOMM_CMD_POOL_BUF_SIZE        (p_btm_cfg_buf_pools[RFCOMM_CMD_POOL_ID].buf_size)
#define RFCOMM_DATA_POOL_ID             GKI_POOL_ID_2
#define RFCOMM_DATA_POOL_BUF_SIZE       (p_btm_cfg_buf_pools[RFCOMM_DATA_POOL_ID].buf_size)
#if (defined(BTU_DYNAMIC_CB_INCLUDED)  && (BTU_DYNAMIC_CB_INCLUDED == TRUE))
#define MAX_RFC_PORTS                   (p_btm_cfg_settings->rfcomm_cfg.max_ports)
#define MAX_BD_CONNECTIONS              (p_btm_cfg_settings->rfcomm_cfg.max_links)
#else
#define MAX_RFC_PORTS                   1
#endif
#define PORT_RX_CRITICAL_WM             ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_CRITICAL_WM)
#define PORT_RX_LOW_WM                  ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_LOW_WM)
#define PORT_RX_HIGH_WM                 ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_HIGH_WM)
#define PORT_RX_BUF_LOW_WM              2
#define PORT_RX_BUF_HIGH_WM             3
#define PORT_RX_BUF_CRITICAL_WM         5
#define PORT_TX_HIGH_WM                 ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_TX_BUF_HIGH_WM)
#define PORT_TX_CRITICAL_WM             ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_TX_BUF_CRITICAL_WM)
#define PORT_TX_BUF_HIGH_WM             3
#define PORT_TX_BUF_CRITICAL_WM         5
#define PORT_CREDIT_RX_LOW              2
#define PORT_CREDIT_RX_MAX              3

/* HID definitions */
#define HID_DEV_INCLUDED                BR_INCLUDED
#define HID_DEV_MAX_DESCRIPTOR_SIZE     200
#define HID_DEV_SET_CONN_MODE           FALSE
#define HID_DEV_PM_INCLUDED             BR_INCLUDED


#define HID_HOST_INCLUDED               TRUE
#define BR_HIDH_INCLUDED                BR_INCLUDED
#define BLE_HIDH_INCLUDED               TRUE

/* AVDT/A2DP/AVRC definitions */
#define A2D_INCLUDED        BR_INCLUDED
#define A2D_SBC_INCLUDED    BR_INCLUDED
#define A2D_M12_INCLUDED    BR_INCLUDED
#define A2D_M24_INCLUDED    BR_INCLUDED
#define AVDT_INCLUDED       BR_INCLUDED
#define AVDT_REPORTING                  FALSE
#define AVDT_MULTIPLEXING               FALSE
#define AVDT_NUM_LINKS                  (p_btm_cfg_settings->avdt_cfg.max_links)
#define AVDT_NUM_SEPS                   (p_btm_cfg_settings->avdt_cfg.max_seps)
#define AVDT_CMD_POOL_ID                GKI_POOL_ID_1
#define AVDT_DATA_POOL_ID               GKI_POOL_ID_3
#define AVDT_DATA_POOL_SIZE             (p_btm_cfg_buf_pools[AVDT_DATA_POOL_ID].buf_size)

#define AVRC_INCLUDED   BR_INCLUDED
#define AVCT_INCLUDED   BR_INCLUDED
#define AVCT_NUM_LINKS                  (p_btm_cfg_settings->avrc_cfg.max_links)
#define AVCT_NUM_CONN                   (avct_cb.num_conn)
#define AVRC_SEC_MASK                   (p_btm_cfg_settings->security_requirement_mask)
#define AVRC_CONTROL_MTU                (L2CAP_MTU_SIZE)
#define AVRC_BROWSE_MTU                 (L2CAP_MTU_SIZE)

#ifdef WICED_ENABLE
/* Profiles */
#if (BR_INCLUDED == TRUE )
#define A2DP_SINK_ENABLE                FALSE    // A2DP sync profile
#define HANDSFREE_ENABLE                FALSE    // Handsfree profile
#define AVRC_CT_ENABLE                  FALSE    // AVRC controller profile
#define AVRC_SCRATCH_BUFFER_POOL_ID     GKI_POOL_ID_3
#define AVRC_SCRATCH_BUFFER_SIZE        (p_btm_cfg_buf_pools[AVRC_SCRATCH_BUFFER_POOL_ID].buf_size)
#define AVRC_TG_ENABLE                  FALSE    //  AVRC target profile
#endif
#endif

#define GATT_FIXED_DB                   TRUE
#define GATT_MAX_APPS                   3
#define GATT_MAX_SR_PROFILES            3
#define GATT_MAX_ATTR_LEN               (p_btm_cfg_settings->gatt_cfg.max_attr_len)
#define GATT_MAX_MTU_SIZE               (p_btm_cfg_settings->gatt_cfg.max_mtu_size)
#define GATT_CL_MAX_LCB                 (p_btm_cfg_settings->gatt_cfg.client_max_links)
#define GATT_MAX_SCCB                   (p_btm_cfg_settings->gatt_cfg.server_max_links)

#define GATT_MAX_PHY_CHANNEL            (GATT_CL_MAX_LCB + GATT_MAX_SCCB)

#define GATTP_TRANSPORT_SUPPORTED       GATT_TRANSPORT_LE_BR_EDR
#define GATTC_NOTIF_TIMEOUT             3

#ifndef GATT_OVER_BR_EDR_INCLUDED
#define GATT_OVER_BR_EDR_INCLUDED                       TRUE
#endif

#define SIM_ACCESS_INCLUDED             FALSE
#define SAP_CLIENT_INCLUDED             FALSE

#define BLE_BRCM_INCLUDED                 TRUE
#define BLE_DATA_LEN_EXT_INCLUDED       TRUE

#define GKI_NUM_FIXED_BUF_POOLS         WICED_BT_CFG_NUM_BUF_POOLS
#define GKI_NUM_TOTAL_BUF_POOLS         (p_btm_cfg_settings->max_number_of_buffer_pools)
#define GKI_BUF0_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_0].buf_size)
#define GKI_BUF0_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_0].buf_count)
#define GKI_BUF1_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_1].buf_size)
#define GKI_BUF1_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_1].buf_count)
#define GKI_BUF2_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_2].buf_size)
#define GKI_BUF2_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_2].buf_count)
#define GKI_BUF3_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_3].buf_size)
#define GKI_BUF3_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_3].buf_count)

#define GKI_USE_DYNAMIC_BUFFERS         TRUE

#define HCIC_INCLUDED                   TRUE
#define HCI_CMD_POOL_ID                 GKI_POOL_ID_1
#define HCI_CMD_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_CMD_POOL_ID].buf_size)
#define HCI_ACL_POOL_ID                 GKI_POOL_ID_2
#define HCI_ACL_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_ACL_POOL_ID].buf_size)
#define HCI_SCO_POOL_ID                 GKI_POOL_ID_1
#define HCI_SCO_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_SCO_POOL_ID].buf_size)
#define HCI_USE_VARIABLE_SIZE_CMD_BUF   TRUE    /* Allocate smallest possible buffer (for platforms with limited RAM) */

#define BTM_INQ_DB_SIZE                 1
#define BTM_SEC_MAX_DEVICE_RECORDS      (p_btm_cfg_settings->max_simultaneous_links)
#define BTM_SEC_HOST_PRIVACY_ADDR_RESOLUTION_TABLE_SIZE    (p_btm_cfg_settings->addr_resolution_db_size)

#define BTM_SEC_MAX_SERVICE_RECORDS     4
#define BTM_SEC_SERVICE_NAME_LEN        0
#define BTM_MAX_LOC_BD_NAME_LEN         0
#define BTM_MAX_PM_RECORDS              1
#define BTM_MAX_VSE_CALLBACKS           (g_btm_cfg_num_vse_callbacks)
#define BTM_BLE_MAX_BG_CONN_DEV_NUM     (p_btm_cfg_settings->ble_filter_accept_list_size)
#define BTM_OOB_INCLUDED                BR_INCLUDED
#define BTM_BR_SC_INCLUDED              BR_INCLUDED
#define BTM_CROSS_TRANSP_KEY_DERIVATION BR_INCLUDED
#define BT_BRCM_VS_INCLUDED             TRUE

#define MAX_TRACE_RAM_SIZE              10


#define SDP_RAW_DATA_SERVER             BR_INCLUDED
#define SDP_MAX_LIST_BYTE_COUNT         (p_btm_cfg_buf_pools[SDP_POOL_ID].buf_size)
#define SDP_POOL_ID                     GKI_POOL_ID_3
#define SDP_MAX_CONNECTIONS             1
#define SDP_MAX_RECORDS                 3
#define SDP_MAX_REC_ATTR                8
#define SDP_MAX_UUID_FILTERS            3
#define SDP_MAX_ATTR_FILTERS            12
#define SDP_MAX_PROTOCOL_PARAMS         2


/* Enable/Disable *_DYNAMIC_MEMORY for stack control blocks here */
#define L2C_DYNAMIC_MEMORY          FALSE
#define SDP_DYNAMIC_MEMORY          FALSE
#define RFC_DYNAMIC_MEMORY          FALSE
#define SPP_DYNAMIC_MEMORY          FALSE
#define HID_DYNAMIC_MEMORY          FALSE

#if (defined(BTU_DYNAMIC_CB_INCLUDED)  && (BTU_DYNAMIC_CB_INCLUDED == TRUE))
#define MAX_L2CAP_CLIENTS               (btu_cb.l2c_cfg_max_clients)
#define MAX_L2CAP_LINKS                 (btu_cb.l2c_cfg_max_links)
#define MAX_L2CAP_CHANNELS              (btu_cb.l2c_cfg_max_channels)

/* Connection Oriented Channel configuration */
#define MAX_L2CAP_BLE_CLIENTS           (p_btm_cfg_settings->l2cap_application.max_le_psm)
#define MAX_L2CAP_BLE_CHANNELS          (p_btm_cfg_settings->l2cap_application.max_le_channels)

#else /* BTU_DYNAMIC_CB_INCLUDED  */
#define MAX_L2CAP_CLIENTS               3
#define MAX_L2CAP_LINKS                 1
#define MAX_L2CAP_CHANNELS              4
#endif /* BTU_DYNAMIC_CB_INCLUDED */

#define GAP_CONN_INCLUDED               FALSE

#define L2C_DEF_NUM_BLE_BUF_SHARED  1

#define MPAF_ZERO_COPY_ACL_UP_PATH      FALSE  // Does not work with current fragmentaion logic

#ifndef WIN32                            // Does not work with emulator
    #define MPAF_QUICKACCESS_API TRUE    // To reduce boot initialization time
#endif

#define BTM_COEX_INCLUDED   BR_INCLUDED

#ifndef BT_USE_TRACES
#define BT_TRACE_PROTOCOL       FALSE
#define BT_USE_TRACES           FALSE
#define BTTRC_INCLUDED          FALSE
#define BTTRC_PARSER_INCLUDED   FALSE
#endif

#define BTM_SET_DEV_NAME_UPON_RESET FALSE

/* SMP over BR/EDR */
#ifndef SMP_OVER_BR_EDR
#define SMP_OVER_BR_EDR                                 FALSE
#endif

#endif /* MPAF_BT_STACK_ENABLE */

#ifdef __cplusplus
}
#endif

#endif
