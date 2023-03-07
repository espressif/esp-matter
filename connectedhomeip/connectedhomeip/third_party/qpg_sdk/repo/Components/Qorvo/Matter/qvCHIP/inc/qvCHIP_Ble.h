/*
 * Copyright (c) 2020-2021, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Change$
 * $DateTime$
 */

/** @file "qvCHIP_Ble.h"
 *
 *  CHIP BLE wrapper API
 *
 *  Declarations of the BLE specific public functions and enumerations of qvCHIP.
*/

#ifndef _QVCHIP_BLE_H_
#define _QVCHIP_BLE_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** \name DM Callback Events
 * Events handled by the DM state machine.
 */
/**@{*/
#define QVCHIP_DM_CBACK_START 0x20 /*!< \brief DM callback event starting value */

/*! \brief DM callback events */
enum {
    QVCHIP_DM_RESET_CMPL_IND = QVCHIP_DM_CBACK_START, /*!< \brief Reset complete */
    QVCHIP_DM_ADV_START_IND,                          /*!< \brief Advertising started */
    QVCHIP_DM_ADV_STOP_IND,                           /*!< \brief Advertising stopped */
    QVCHIP_DM_ADV_NEW_ADDR_IND,                       /*!< \brief New resolvable address has been generated */
    QVCHIP_DM_SCAN_START_IND,                         /*!< \brief Scanning started */
    QVCHIP_DM_SCAN_STOP_IND,                          /*!< \brief Scanning stopped */
    QVCHIP_DM_SCAN_REPORT_IND,                        /*!< \brief Scan data received from peer device */
    QVCHIP_DM_CONN_OPEN_IND,                          /*!< \brief Connection opened */
    QVCHIP_DM_CONN_CLOSE_IND,                         /*!< \brief Connection closed */
    QVCHIP_DM_CONN_UPDATE_IND,                        /*!< \brief Connection update complete */
    QVCHIP_DM_SEC_PAIR_CMPL_IND,                      /*!< \brief Pairing completed successfully */
    QVCHIP_DM_SEC_PAIR_FAIL_IND,                      /*!< \brief Pairing failed or other security failure */
    QVCHIP_DM_SEC_ENCRYPT_IND,                        /*!< \brief Connection encrypted */
    QVCHIP_DM_SEC_ENCRYPT_FAIL_IND,                   /*!< \brief Encryption failed */
    QVCHIP_DM_SEC_AUTH_REQ_IND,                       /*!< \brief PIN or OOB data requested for pairing */
    QVCHIP_DM_SEC_KEY_IND,                            /*!< \brief Security key indication */
    QVCHIP_DM_SEC_LTK_REQ_IND,                        /*!< \brief LTK requested for encyption */
    QVCHIP_DM_SEC_PAIR_IND,                           /*!< \brief Incoming pairing request from master */
    QVCHIP_DM_SEC_SLAVE_REQ_IND,                      /*!< \brief Incoming security request from slave */
    QVCHIP_DM_SEC_CALC_OOB_IND,                       /*!< \brief Result of OOB Confirm Calculation Generation */
    QVCHIP_DM_SEC_ECC_KEY_IND,                        /*!< \brief Result of ECC Key Generation */
    QVCHIP_DM_SEC_COMPARE_IND,                        /*!< \brief Result of Just Works/Numeric Comparison Compare Value Calculation */
    QVCHIP_DM_SEC_KEYPRESS_IND,                       /*!< \brief Keypress indication from peer in passkey security */
    QVCHIP_DM_PRIV_RESOLVED_ADDR_IND,                 /*!< \brief Private address resolved */
    QVCHIP_DM_PRIV_GENERATE_ADDR_IND,                 /*!< \brief Private resolvable address generated */
    QVCHIP_DM_CONN_READ_RSSI_IND,                     /*!< \brief Connection RSSI read */
    QVCHIP_DM_PRIV_ADD_DEV_TO_RES_LIST_IND,           /*!< \brief Device added to resolving list */
    QVCHIP_DM_PRIV_REM_DEV_FROM_RES_LIST_IND,         /*!< \brief Device removed from resolving list */
    QVCHIP_DM_PRIV_CLEAR_RES_LIST_IND,                /*!< \brief Resolving list cleared */
    QVCHIP_DM_PRIV_READ_PEER_RES_ADDR_IND,            /*!< \brief Peer resolving address read */
    QVCHIP_DM_PRIV_READ_LOCAL_RES_ADDR_IND,           /*!< \brief Local resolving address read */
    QVCHIP_DM_PRIV_SET_ADDR_RES_ENABLE_IND,           /*!< \brief Address resolving enable set */
    QVCHIP_DM_REM_CONN_PARAM_REQ_IND,                 /*!< \brief Remote connection parameter requested */
    QVCHIP_DM_CONN_DATA_LEN_CHANGE_IND,               /*!< \brief Data length changed */
    QVCHIP_DM_CONN_WRITE_AUTH_TO_IND,                 /*!< \brief Write authenticated payload complete */
    QVCHIP_DM_CONN_AUTH_TO_EXPIRED_IND,               /*!< \brief Authenticated payload timeout expired */
    QVCHIP_DM_PHY_READ_IND,                           /*!< \brief Read PHY */
    QVCHIP_DM_PHY_SET_DEF_IND,                        /*!< \brief Set default PHY */
    QVCHIP_DM_PHY_UPDATE_IND,                         /*!< \brief PHY update */
    QVCHIP_DM_ADV_SET_START_IND,                      /*!< \brief Advertising set(s) started */
    QVCHIP_DM_ADV_SET_STOP_IND,                       /*!< \brief Advertising set(s) stopped */
    QVCHIP_DM_SCAN_REQ_RCVD_IND,                      /*!< \brief Scan request received */
    QVCHIP_DM_EXT_SCAN_START_IND,                     /*!< \brief Extended scanning started */
    QVCHIP_DM_EXT_SCAN_STOP_IND,                      /*!< \brief Extended scanning stopped */
    QVCHIP_DM_EXT_SCAN_REPORT_IND,                    /*!< \brief Extended scan data received from peer device */
    QVCHIP_DM_PER_ADV_SET_START_IND,                  /*!< \brief Periodic advertising set started */
    QVCHIP_DM_PER_ADV_SET_STOP_IND,                   /*!< \brief Periodic advertising set stopped */
    QVCHIP_DM_PER_ADV_SYNC_EST_IND,                   /*!< \brief Periodic advertising sync established */
    QVCHIP_DM_PER_ADV_SYNC_EST_FAIL_IND,              /*!< \brief Periodic advertising sync establishment failed */
    QVCHIP_DM_PER_ADV_SYNC_LOST_IND,                  /*!< \brief Periodic advertising sync lost */
    QVCHIP_DM_PER_ADV_SYNC_TRSF_EST_IND,              /*!< \brief Periodic advertising sync transfer established */
    QVCHIP_DM_PER_ADV_SYNC_TRSF_EST_FAIL_IND,         /*!< \brief Periodic advertising sync transfer establishment failed */
    QVCHIP_DM_PER_ADV_SYNC_TRSF_IND,                  /*!< \brief Periodic advertising sync info transferred */
    QVCHIP_DM_PER_ADV_SET_INFO_TRSF_IND,              /*!< \brief Periodic advertising set sync info transferred */
    QVCHIP_DM_PER_ADV_REPORT_IND,                     /*!< \brief Periodic advertising data received from peer device */
    QVCHIP_DM_REMOTE_FEATURES_IND,                    /*!< \brief Remote features from peer device */
    QVCHIP_DM_READ_REMOTE_VER_INFO_IND,               /*!< \brief Remote LL version information read */
    QVCHIP_DM_CONN_IQ_REPORT_IND,                     /*!< \brief IQ samples from CTE of received packet from peer device */
    QVCHIP_DM_CTE_REQ_FAIL_IND,                       /*!< \brief CTE request failed */
    QVCHIP_DM_CONN_CTE_RX_SAMPLE_START_IND,           /*!< \brief Sampling received CTE started */
    QVCHIP_DM_CONN_CTE_RX_SAMPLE_STOP_IND,            /*!< \brief Sampling received CTE stopped */
    QVCHIP_DM_CONN_CTE_TX_CFG_IND,                    /*!< \brief Connection CTE transmit parameters configured */
    QVCHIP_DM_CONN_CTE_REQ_START_IND,                 /*!< \brief Initiating connection CTE request started */
    QVCHIP_DM_CONN_CTE_REQ_STOP_IND,                  /*!< \brief Initiating connection CTE request stopped */
    QVCHIP_DM_CONN_CTE_RSP_START_IND,                 /*!< \brief Responding to connection CTE request started */
    QVCHIP_DM_CONN_CTE_RSP_STOP_IND,                  /*!< \brief Responding to connection CTE request stopped */
    QVCHIP_DM_READ_ANTENNA_INFO_IND,                  /*!< \brief Antenna information read */
    QVCHIP_DM_CIS_CIG_CONFIG_IND,                     /*!< \brief CIS CIG configure complete */
    QVCHIP_DM_CIS_CIG_REMOVE_IND,                     /*!< \brief CIS CIG remove complete */
    QVCHIP_DM_CIS_REQ_IND,                            /*!< \brief CIS request */
    QVCHIP_DM_CIS_OPEN_IND,                           /*!< \brief CIS connection opened */
    QVCHIP_DM_CIS_CLOSE_IND,                          /*!< \brief CIS connection closed */
    QVCHIP_DM_REQ_PEER_SCA_IND,                       /*!< \brief Request peer SCA complete */
    QVCHIP_DM_ISO_DATA_PATH_SETUP_IND,                /*!< \brief ISO data path setup complete */
    QVCHIP_DM_ISO_DATA_PATH_REMOVE_IND,               /*!< \brief ISO data path remove complete */
    QVCHIP_DM_DATA_PATH_CONFIG_IND,                   /*!< \brief Data path configure complete */
    QVCHIP_DM_READ_LOCAL_SUP_CODECS_IND,              /*!< \brief Local supported codecs read */
    QVCHIP_DM_READ_LOCAL_SUP_CODEC_CAP_IND,           /*!< \brief Local supported codec capabilities read */
    QVCHIP_DM_READ_LOCAL_SUP_CTR_DLY_IND,             /*!< \brief Local supported controller delay read */
    QVCHIP_DM_BIG_START_IND,                          /*!< \brief BIG started */
    QVCHIP_DM_BIG_STOP_IND,                           /*!< \brief BIG stopped */
    QVCHIP_DM_BIG_SYNC_EST_IND,                       /*!< \brief BIG sync established */
    QVCHIP_DM_BIG_SYNC_EST_FAIL_IND,                  /*!< \brief BIG sync establishment failed */
    QVCHIP_DM_BIG_SYNC_LOST_IND,                      /*!< \brief BIG sync lost */
    QVCHIP_DM_BIG_SYNC_STOP_IND,                      /*!< \brief BIG sync stopped */
    QVCHIP_DM_BIG_INFO_ADV_REPORT_IND,                /*!< \brief BIG Info advertising data received from peer device */
    QVCHIP_DM_L2C_CMD_REJ_IND,                        /*!< \brief L2CAP Command Reject */
    QVCHIP_DM_ERROR_IND,                              /*!< \brief General error */
    QVCHIP_DM_HW_ERROR_IND,                           /*!< \brief Hardware error */
    QVCHIP_DM_VENDOR_SPEC_IND                         /*!< \brief Vendor specific event */
};

#define QVCHIP_DM_CBACK_END QVCHIP_DM_VENDOR_SPEC_IND /*!< \brief DM callback event ending value */
/**@}*/

/** \name ATT Callback Events
 * Events related to ATT transactions.
 */
/**@{*/
#define QVCHIP_ATT_CBACK_START 0x02 /*!< \brief ATT callback event starting value */

/*! \brief ATT client callback events */
typedef enum qvCHIP_Ble_AttMsg_ {
    QVCHIP_ATTC_FIND_INFO_RSP = QVCHIP_ATT_CBACK_START, /*!< \brief Find information response */
    QVCHIP_ATTC_FIND_BY_TYPE_VALUE_RSP,                 /*!< \brief Find by type value response */
    QVCHIP_ATTC_READ_BY_TYPE_RSP,                       /*!< \brief Read by type value response */
    QVCHIP_ATTC_READ_RSP,                               /*!< \brief Read response */
    QVCHIP_ATTC_READ_LONG_RSP,                          /*!< \brief Read long response */
    QVCHIP_ATTC_READ_MULTIPLE_RSP,                      /*!< \brief Read multiple response */
    QVCHIP_ATTC_READ_BY_GROUP_TYPE_RSP,                 /*!< \brief Read group type response */
    QVCHIP_ATTC_WRITE_RSP,                              /*!< \brief Write response */
    QVCHIP_ATTC_WRITE_CMD_RSP,                          /*!< \brief Write command response */
    QVCHIP_ATTC_PREPARE_WRITE_RSP,                      /*!< \brief Prepare write response */
    QVCHIP_ATTC_EXECUTE_WRITE_RSP,                      /*!< \brief Execute write response */
    QVCHIP_ATTC_HANDLE_VALUE_NTF,                       /*!< \brief Handle value notification */
    QVCHIP_ATTC_HANDLE_VALUE_IND,                       /*!< \brief Handle value indication */
    QVCHIP_ATTC_READ_MULT_VAR_RSP = 16,                 /*!< \brief Read multiple variable length response */
    QVCHIP_ATTC_MULT_VALUE_NTF,                         /*!< \brief Read multiple value notification */
    /* ATT server callback events */
    QVCHIP_ATTS_HANDLE_VALUE_CNF,      /*!< \brief Handle value confirmation */
    QVCHIP_ATTS_MULT_VALUE_CNF,        /*!< \brief Handle multiple value confirmation */
    QVCHIP_ATTS_CCC_STATE_IND,         /*!< \brief Client chracteristic configuration state change */
    QVCHIP_ATTS_DB_HASH_CALC_CMPL_IND, /*!< \brief Database hash calculation complete */
    /* ATT common callback events */
    QVCHIP_ATT_MTU_UPDATE_IND,        /*!< \brief Negotiated MTU value */
    QVCHIP_ATT_EATT_CONN_CMPL_IND,    /*!< \brief EATT Connect channels complete */
    QVCHIP_ATT_EATT_RECONFIG_CMPL_IND /*!< \brief EATT Reconfigure complete */
} qvCHIP_Ble_AttMsg_t;

/*! \brief ATT callback events */
#define QVCHIP_ATT_CBACK_END QVCHIP_ATT_EATT_RECONFIG_CMPL_IND /*!< \brief ATT callback event ending value */
/**@}*/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*! Invalid connection indicator */
#define QVCHIP_DM_CONN_NONE 0

/*! Maximum number of connections */
#define QVCHIP_DM_CONN_MAX 1

/*! Number of supported advertising sets: must be set to 1 for legacy advertising */
#define QVCHIP_DM_NUM_ADV_SETS 1

#define QVCHIP_HCI_SUCCESS               0x00 /*!< Success */
#define QVCHIP_HCI_ERR_REMOTE_TERMINATED 0x13 /*!< Remote user terminated connection */
#define QVCHIP_HCI_ERR_LOCAL_TERMINATED  0x16 /*!< Connection terminated by local host */

/*! \brief      BD address length */
#define BDA_ADDR_LEN 6

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef enum qvAdvLocation_ {
    QV_ADV_DATA_LOC_ADV = 0,
    QV_ADV_DATA_LOC_SCAN = 1
} qvAdvLocation_t;

typedef struct
{
    uint8_t bytes[16];
} qvBleUUID;

/*! \brief      BD address data type */
typedef uint8_t bdAddr_t[BDA_ADDR_LEN];

/*! Attribute structure */
typedef struct
{
    uint8_t const* pUuid; /*! Pointer to the attribute's UUID */
    uint8_t* pValue;      /*! Pointer to the attribute's value */
    uint16_t* pLen;       /*! Pointer to the length of the attribute's value */
    uint16_t maxLen;      /*! Maximum length of attribute's value */
    uint8_t settings;     /*! Attribute settings */
    uint8_t permissions;  /*! Attribute permissions */
} qvCHIP_Ble_Attr_t;

/*! Common message structure passed to event handler */
typedef struct
{
    uint16_t param; /*! General purpose parameter passed to event handler */
    uint8_t event;  /*! General purpose event value passed to event handler */
    uint8_t status; /*! General purpose status value passed to event handler */
} qvCHIP_Ble_MsgHdr_t;

/*! \brief LE connection complete event */
typedef struct
{
    qvCHIP_Ble_MsgHdr_t hdr; /*!< \brief Event header */
    uint8_t status;          /*!< \brief Status. */
    uint16_t handle;         /*!< \brief Connection handle. */
    uint8_t role;            /*!< \brief Local connection role. */
    uint8_t addrType;        /*!< \brief Peer address type. */
    bdAddr_t peerAddr;       /*!< \brief Peer address. */
    uint16_t connInterval;   /*!< \brief Connection interval */
    uint16_t connLatency;    /*!< \brief Connection latency. */
    uint16_t supTimeout;     /*!< \brief Supervision timeout. */
    uint8_t clockAccuracy;   /*!< \brief Clock accuracy. */

    /* \brief enhanced fields */
    bdAddr_t localRpa; /*!< \brief Local RPA. */
    bdAddr_t peerRpa;  /*!< \brief Peer RPA. */
} qvCHIP_Ble_HciLeConnCmplEvt_t;

/*! \brief Disconnect complete event */
typedef struct
{
    qvCHIP_Ble_MsgHdr_t hdr; /*!< \brief Event header. */
    uint8_t status;          /*!< \brief Disconnect complete status. */
    uint16_t handle;         /*!< \brief Connect handle. */
    uint8_t reason;          /*!< \brief Reason. */
} qvCHIP_Ble_HciDisconnectCmplEvt_t;

/*! \brief LE connection update complete event */
typedef struct
{
    qvCHIP_Ble_MsgHdr_t hdr; /*!< \brief Event header. */
    uint8_t status;          /*!< \brief Status. */
    uint16_t handle;         /*!< \brief Connection handle. */
    uint16_t connInterval;   /*!< \brief Connection interval. */
    uint16_t connLatency;    /*!< \brief Connection latency. */
    uint16_t supTimeout;     /*!< \brief Supervision timeout. */
} qvCHIP_Ble_HciLeConnUpdateCmplEvt_t;

/* \brief Data structure for QVCHIP_DM_ADV_SET_START_IND */
typedef struct
{
    qvCHIP_Ble_MsgHdr_t hdr;                   /*! Header */
    uint8_t numSets;                           /*! Number of advertising sets */
    uint8_t advHandle[QVCHIP_DM_NUM_ADV_SETS]; /*! Advertising handle array */
} qvCHIP_Ble_DmAdvSetStartEvt_t;

/*! \brief LE advertising set terminated */
typedef struct
{
    qvCHIP_Ble_MsgHdr_t hdr; /*!< \brief Event header. */
    uint8_t status;          /*!< \brief Status. */
    uint8_t advHandle;       /*!< \brief Advertising handle. */
    uint16_t handle;         /*!< \brief Connection handle. */
    uint8_t numComplEvts;    /*!< \brief Number of completed extended advertising events. */
} qvCHIP_Ble_HciLeAdvSetTermEvt_t;

/*! \brief Union of DM callback event data types */
typedef union {
    qvCHIP_Ble_MsgHdr_t hdr;                        /*! Common header */
    qvCHIP_Ble_HciLeConnCmplEvt_t connOpen;         /*! QVCHIP_DM_CONN_OPEN_IND */
    qvCHIP_Ble_HciDisconnectCmplEvt_t connClose;    /*! QVCHIP_DM_CONN_CLOSE_IND */
    qvCHIP_Ble_HciLeConnUpdateCmplEvt_t connUpdate; /*! QVCHIP_DM_CONN_UPDATE_IND */
    qvCHIP_Ble_DmAdvSetStartEvt_t advSetStart;      /*! QVCHIP_DM_ADV_SET_START_IND */
    qvCHIP_Ble_HciLeAdvSetTermEvt_t advSetStop;     /*! QVCHIP_DM_ADV_SET_STOP_IND */
} qvCHIP_Ble_DmEvt_t;

/*!
 * \brief ATT callback event
 *
 * \param hdr.event     Callback event
 * \param hdr.param     DM connection ID
 * \param hdr.status    Event status:  ATT_SUCCESS or error status
 * \param pValue        Pointer to value data, valid if valueLen > 0
 * \param valueLen      Length of value data
 * \param handle        Attribute handle
 * \param continuing    TRUE if more response packets expected
 * \param mtu           Negotiated MTU value
 */
typedef struct
{
    qvCHIP_Ble_MsgHdr_t hdr; /*!< \brief Header structure */
    uint8_t* pValue;         /*!< \brief Value */
    uint16_t valueLen;       /*!< \brief Value length */
    uint16_t handle;         /*!< \brief Attribute handle */
    uint8_t continuing;      /*!< \brief TRUE if more response packets expected */
    uint16_t mtu;            /*!< \brief Negotiated MTU value */
} qvCHIP_Ble_AttEvt_t;

/*! \brief ATTS client characteristic configuration callback structure */
typedef struct
{
    qvCHIP_Ble_MsgHdr_t hdr; /*! Header structure */
    uint16_t handle;         /*! CCCD handle */
    uint16_t value;          /*! CCCD value */
    uint8_t idx;             /*! CCCD settings index */
} qvCHIP_Ble_AttsCccEvt_t;

/*! \brief Message processing callback */
typedef void (*qvCHIP_Ble_StackCback_t)(qvCHIP_Ble_MsgHdr_t* pDmEvt);

/*! \brief Attribute group read callback */
typedef void (*qvCHIP_Ble_ReadCback_t)(uint16_t connId, uint16_t handle, uint8_t operation,
                                       uint16_t offset, qvCHIP_Ble_Attr_t* pAttr);

/*! \brief Attribute group write callback */
typedef void (*qvCHIP_Ble_WriteCback_t)(uint16_t connId, uint16_t handle, uint8_t operation,
                                        uint16_t offset, uint16_t len, uint8_t* pValue,
                                        qvCHIP_Ble_Attr_t* pAttr);

/*! \brief CCC subscription change callback */
typedef void (*qvCHIP_Ble_ATTS_CccCback_t)(qvCHIP_Ble_AttsCccEvt_t* pEvt);

/*! \brief External callbacks structure */
typedef struct
{
    qvCHIP_Ble_StackCback_t stackCback;
    qvCHIP_Ble_ReadCback_t chrReadCback;
    qvCHIP_Ble_WriteCback_t chrWriteCback;
    qvCHIP_Ble_ATTS_CccCback_t cccCback;
} qvCHIP_Ble_Callbacks_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                    BLE API
 *****************************************************************************/

/** @brief Initialization function for BLE functionality
 *
 *  @param callbacks       Structure containing various callbacks to call in the application
 *  @return                Returns NO_ERROR if the operation completed successfully.
*/
qvStatus_t qvCHIP_BleInit(qvCHIP_Ble_Callbacks_t* callbacks);

/** @brief Sets the CHIPoBLE service and TX and RX characteristics UUIDs in human-readable order (MSB)
 *
 *  @param chipOBLE_UUID   Contains the service UUID to be used.
 *  @param txChar_UUID     Contains the UUID for TX characteristic of CHIPoBLE service.
 *  @param rxChar_UUID     Contains the UUID for RX characteristic of CHIPoBLE service.
 *  @return                Returns NO_ERROR if the operation completed successfully.
*/
qvStatus_t qvCHIP_BleSetUUIDs(uint8_t* chipOBLE_UUID, uint8_t* txChar_UUID, uint8_t* rxChar_UUID);

/** @brief Reads back the internally stored device name
 *
 *  @param buf             Buffer where to copy the internally stored device name.
 *  @param bufSize         Size of the buffer provided for the data.
 *  @return                Returns NO_ERROR if the operation completed successfully.
*/
qvStatus_t qvCHIP_BleGetDeviceName(char* buf, size_t bufSize);

/** @brief Store a device name to be used during advertising
 *
 *  @param devName         Pointer to device name string to be stored internally.
 *  @return                Returns NO_ERROR if the operation completed successfully.
*/
qvStatus_t qvCHIP_BleSetDeviceName(const char* devName);

/** @brief Triggers a connection to close
 *
 *  @param conId           Id of the connection to close.
 *  @return                Returns NO_ERROR if the operation completed successfully.
*/
qvStatus_t qvCHIP_BleCloseConnection(uint16_t conId);

/** @brief Returns the MTU parameters for a specified connection ID
 *
 *  @param conId           Id of the connection for which the MTU is wanted.
 *  @param pMTUSize        pointer set to MTU value for the specified connection.
 *  @return                INVALID_ARGUMENT if conId or pMTUSize is not valid, otherwise NO_ERROR
*/
qvStatus_t qvCHIP_BleGetMTU(uint16_t conId, uint16_t* pMTUSize);

/** @brief Writes to an attribute with the specified parameters
 *
 *  @param conId           ID of the connection to use to send data.
 *  @param handle          Handle in the GATT server for the characteristic on which to send the data.
 *  @param length          Length of the data.
 *  @param data            Pointer to the data to send.
 *  @return                INVALID_ARGUMENT if data is NULL, otherwise NO_ERROR
*/
qvStatus_t qvCHIP_BleWriteAttr(uint16_t conId, uint16_t handle, uint16_t length, uint8_t* data);

/** @brief Sends an indication with the specified parameters
 *
 *  @param conId           ID of the connection to use to send data.
 *  @param handle          Handle in the GATT server for the characteristic on which to send the data.
 *  @param length          Length of the data.
 *  @param data            Pointer to the data to send.
 *  @return                INVALID_ARGUMENT if conId or data is not valid, otherwise NO_ERROR
*/
qvStatus_t qvCHIP_BleSendIndication(uint16_t conId, uint16_t handle, uint16_t length, uint8_t* data);

/** @brief Sends a notification with the specified parameters
 *
 *  @param conId           ID of the connection to use to send data.
 *  @param handle          Handle in the GATT server for the characteristic on which to send the data.
 *  @param length          Length of the data.
 *  @param data            Pointer to the data to send.
 *  @return                INVALID_ARGUMENT if conId or data is not valid, otherwise NO_ERROR
*/
qvStatus_t qvCHIP_BleSendNotification(uint16_t conId, uint16_t handle, uint16_t length, uint8_t* data);

/** @brief Sets minimum and maximum intervals for advertising packets
 *
 *  @param intervalMin     Minimum interval between advertisement packets.
 *  @param intervalMax     Maximum interval between advertisement packets.
 *  @return                Returns NO_ERROR if the operation completed successfully
*/
qvStatus_t qvCHIP_BleSetAdvInterval(uint16_t intervalMin, uint16_t intervalMax);

/** @brief Set advertising data to be used
 *
 *  @param location        Type of advertising data that is being set (advertising or scan response).
 *  @param len             Length of the data.
 *  @param pData           Pointer to the advertising data.
 *  @return                Returns NO_ERROR if the operation completed successfully
*/
qvStatus_t qvCHIP_BleSetAdvData(qvAdvLocation_t location, uint8_t len, uint8_t* pData);

/** @brief Start BLE advertising using the default parameters
 *
 *  @return                Returns NO_ERROR if the operation completed successfully
*/
qvStatus_t qvCHIP_BleStartAdvertising(void);

/** @brief Stops BLE advertising
 *
 *  @return                Returns NO_ERROR if the operation completed successfully
*/
qvStatus_t qvCHIP_BleStopAdvertising(void);

/** @brief Returns handle for RX or TX characteristic
 *
 *  @param rxHandle        Boolean indicating if the function should return RX or TX handle.
 *  @return                Returns RX handle if rxHandle is True and TX handle if rxHandle is False.
*/
uint16_t qvCHIP_BleGetHandle(bool rxHandle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_QVCHIP_BLE_H_
