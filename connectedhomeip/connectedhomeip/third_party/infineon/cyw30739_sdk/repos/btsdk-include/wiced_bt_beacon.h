/***************************************************************************//**
* \file <wiced_bt_beacon.h>
*
* \brief
*    The APIs to create Google Eddystone and Apple iBeacon advertisements.
*
*   To Eddystone, see specification at https://github.com/google/eddystone.
*
*   To iBeacon, see specification at https://developer.apple.com/ibeacon/.
*
*   For more usage details, refer example demo.beacon.
*
********************************************************************************
* \copyright
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
*******************************************************************************/
#ifndef WICED_BT_BEACON_H
#define WICED_BT_BEACON_H
#include "wiced_bt_ble.h"

#if defined(CYW43012C0)
#include "wiced_bt_ble_multi_adv.h"
#endif
/* Max adv data length */
#define WICED_BT_BEACON_ADV_DATA_MAX 31

/******************************************************************************
*                   Definitions for Google Eddystone
******************************************************************************/
/* Type of eddystone frame
   https://github.com/google/eddystone/blob/master/protocol-specification.md */
#define EDDYSTONE_FRAME_TYPE_UID          0x00
#define EDDYSTONE_FRAME_TYPE_URL          0x10
#define EDDYSTONE_FRAME_TYPE_TLM          0x20
#define EDDYSTONE_FRAME_TYPE_EID          0x30

/* Definitions for UID frame format */
#define EDDYSTONE_UID_FRAME_LEN           20
#define EDDYSTONE_UID_NAMESPACE_LEN       10
#define EDDYSTONE_UID_INSTANCE_ID_LEN     6

/* Definitions for URL frame format */
#define EDDYSTONE_URL_FRAME_LEN           20
#define EDDYSTONE_URL_VALUE_MAX_LEN       17

/* Definitions for TLM frame format */
#define EDDYSTONE_TLM_UNENCRYPTED_VERSION            0
#define EDDYSTONE_TLM_ENCRYPTED_VERSION              1
#define EDDYSTONE_TLM_UNENCRYPTED_FRAME_LEN         14
#define EDDYSTONE_TLM_ENCRYPTED_FRAME_LEN           18
#define EDDYSTONE_ETLM_LEN 12

/* Definitions for EID frame format */
#define EDDYSTONE_EID_FRAME_LEN           10
#define EDDYSTONE_EID_LEN                 8

/* Eddystone UUID*/
#define EDDYSTONE_UUID16                  0xFEAA

/* Number of advertiment elements for Eddystone*/
#define EDDYSTONE_ELEM_NUM                3

/******************************************************************************
* URL Scheme Prefix for Google Eddystone
* Decimal Hex   Expansion
*   0      0x00   http://www.
*   1      0x01   https://www.
*   2      0x02   http://
*   3      0x03   https://
*
******************************************************************************/
#define EDDYSTONE_URL_SCHEME_0           0x00
#define EDDYSTONE_URL_SCHEME_1           0x01
#define EDDYSTONE_URL_SCHEME_2           0x02
#define EDDYSTONE_URL_SCHEME_3           0x03

/******************************************************************************
* Function Name: wiced_bt_eddystone_set_data_for_uid
***************************************************************************//**
*
* \brief Creates Google Eddystone UID format advertising data.
* \details The application calls the API to populate advertisement data for
*          Google Eddystone UID advertisement  format. It returns the adv data buffer and length.
*
* @param[in]   eddystone_ranging_data     Calibrated TX power, see specification for details for input parameters.
* @param[in]   eddystone_namespace        UID namespace
* @param[in]   eddystone_instance         Instance
* @param[out]  adv_data                   Buffer of advertisement data
* @param[out]  adv_len                    Length of advertisement data
*
* \return     None.
*
******************************************************************************/
void wiced_bt_eddystone_set_data_for_uid(uint8_t eddystone_ranging_data,
                                         uint8_t eddystone_namespace[EDDYSTONE_UID_NAMESPACE_LEN],
                                         uint8_t eddystone_instance[EDDYSTONE_UID_INSTANCE_ID_LEN],
                                         uint8_t adv_data[WICED_BT_BEACON_ADV_DATA_MAX], uint8_t *adv_len);


/******************************************************************************
* Function Name: wiced_bt_eddystone_set_data_for_url
***************************************************************************//**
*
* \brief Creates Google Eddystone URL format advertising data
* \details The application calls the API to populate advertisement data
*          for Google Eddystone URL advertisement format. It returns the adv data buffer and length.
*
* @param[in]   tx_power         Calibrated TX power, see specification for details for input parameters.
* @param[in]   urlscheme        URL scheme
* @param[in]   encoded_url      URL
* @param[out]  adv_data         Buffer of advertisement data
* @param[out]  adv_len          Length of advertisement data
*
* @return     None
******************************************************************************/
void wiced_bt_eddystone_set_data_for_url(uint8_t tx_power,
                                         uint8_t urlscheme,
                                         uint8_t encoded_url[EDDYSTONE_URL_VALUE_MAX_LEN],
                                         uint8_t adv_data[WICED_BT_BEACON_ADV_DATA_MAX], uint8_t *adv_len);

/******************************************************************************
* Function Name: wiced_bt_eddystone_set_data_for_eid
***************************************************************************//**
*
* \brief Creates Google Eddystone EID format advertising data
* \details The application calls the API to populate advertisement data
*          for Google Eddystone EID advertisement format. It returns the adv data buffer and length.
*
* @param[in]   tx_power         Calibrated TX power, see specification for details for input parameters.
* @param[in]   urlscheme        URL scheme
* @param[in]   encoded_url      URL
* @param[out]  adv_data         Buffer of advertisement data
* @param[out]  adv_len          Length of advertisement data
*
* @return     None.
*
******************************************************************************/
void wiced_bt_eddystone_set_data_for_eid(uint8_t eddystone_ranging_data,
    uint8_t eid[EDDYSTONE_EID_LEN],
    uint8_t adv_data[WICED_BT_BEACON_ADV_DATA_MAX], uint8_t *adv_len);

/******************************************************************************
* Function Name: wiced_bt_eddystone_set_data_for_tlm_unencrypted
***************************************************************************//**
*
* \brief Creates Google Eddystone TLM unencrypted format advertising data
* \details The application calls the API to populate advertisement data
*          for Google Eddystone TLM unencrypted advertisement format. It returns the adv data buffer and length.
*
* @param[in]   temp         Beacon temperature, see specification for details for input parameters.
* @param[in]   adv_cnt      Advertising PDU count
* @param[in]   sec_cnt      Time since power-on or reboot
* @param[out]  adv_data     Buffer of advertisement data
* @param[out]  adv_len      Length of advertisement data
*
* \return     None.
*
******************************************************************************/
void wiced_bt_eddystone_set_data_for_tlm_unencrypted(uint16_t vbatt,
                                                     uint16_t temp,
                                                     uint32_t adv_cnt,
                                                     uint32_t sec_cnt,
                                                     uint8_t adv_data[WICED_BT_BEACON_ADV_DATA_MAX], uint8_t *adv_len);

/******************************************************************************
* Function Name: wiced_bt_eddystone_set_data_for_tlm_encrypted
***************************************************************************//**
*
* \brief Creates Google Eddystone TLM encrypted format advertising data
* \details The application calls the API to populate advertisement data
*          for Google Eddystone TLM encrypted advertisement format. It returns the adv data buffer and length.
*
* @param[in]   etlm         Encrypted TLM data, see specification for details for input parameters.
* @param[in]   salt         16-bit Salt
* @param[in]   mic          16 bit Message Integrity Check
* @param[out]  adv_data     Buffer of advertisement data
* @param[out]  adv_len      Length of advertisement data
*
* @return     None.
*
******************************************************************************/
void wiced_bt_eddystone_set_data_for_tlm_encrypted(uint8_t etlm[EDDYSTONE_ETLM_LEN],
                                                   uint16_t salt,
                                                   uint16_t mic,
                                                   uint8_t adv_data[WICED_BT_BEACON_ADV_DATA_MAX], uint8_t *adv_len);

/******************************************************************************
    Definitions for Apple iBeacon
******************************************************************************/
/* See specification at https://developer.apple.com/ibeacon/ */

/******************************************************************************
*                               Constants
******************************************************************************/
/* iBeacon type */
#define IBEACON_TYPE               0x01
#define IBEACON_PROXIMITY          0x02, 0x15
/* iBeacon company ID */
#define IBEACON_COMPANY_ID_APPLE   0x4c, 0x00
/* iBeacon data length */
#define IBEACON_DATA_LENGTH        0x19
/* number of elements in advertisement */
#define IBEACON_ELEM_NUM           2

/******************************************************************************
* Function Name: wiced_bt_ibeacon_set_adv_data
***************************************************************************//**
*
* \brief Creates Apple iBeacon format advertising data
* \details The application calls the API to populate advertisement data
*          for Apple iBeacon advertisement format. It returns the adv data buffer and length.
*
* @param[in]   ibeacon_uuid                 Customer beacon UUID, see specification for details for input parameters.
* @param[in]   ibeacon_major_number         Beacon major number
* @param[in]   ibeacon_minor_number         Beacon minor number
* @param[in]   tx_power_lcl                 measured power
* @param[out]  adv_data                     Buffer of advertisement data
* @param[out]  adv_len                      Length of advertisement data
*
* @return     None.
*
******************************************************************************/
void wiced_bt_ibeacon_set_adv_data(uint8_t ibeacon_uuid[LEN_UUID_128],
                                   uint16_t ibeacon_major_number,
                                   uint16_t ibeacon_minor_number,
                                   uint8_t tx_power_lcl,
                                   uint8_t adv_data[WICED_BT_BEACON_ADV_DATA_MAX], uint8_t *adv_len);



/* Structure to hold advertisement element data */
typedef struct
{
    uint8_t data[WICED_BT_BEACON_ADV_DATA_MAX];     /* Advertisement  data */
    uint8_t len;                                    /* Advertisement length */
    wiced_bt_ble_advert_type_t advert_type;         /* Advertisement data type */
}wiced_bt_beacon_ble_advert_elem_t;

#if defined(CYW20735B1) || defined(CYW20819A1) || defined(CYW20719B2) || defined(CYW20721B2) || defined (WICEDX) || defined(CYW20835B1)
#else
/* Structure to hold multi-adv data */
typedef struct
{
    uint16_t adv_int_min;
    uint16_t adv_int_max;
    wiced_bt_ble_multi_advert_type_t adv_type;
    wiced_bt_ble_address_type_t own_addr_type;
    wiced_bt_device_address_t own_bd_addr;
    wiced_bt_ble_address_type_t peer_addr_type;
    wiced_bt_device_address_t peer_bd_addr;
    wiced_bt_ble_advert_chnl_map_t channel_map;
    wiced_bt_ble_multi_advert_filtering_policy_t adv_filter_policy;
    uint8_t adv_instance;
    int8_t adv_tx_power;
}wiced_bt_beacon_multi_advert_data_t;
#endif

#endif /* _WICED_BT_BEACON_H_ */
