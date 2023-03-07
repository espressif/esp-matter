/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
/* MediaTek restricted information */

#ifndef __BT_DI_H__
#define __BT_DI_H__

#include "bt_type.h"
#include "bt_sdp.h"
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothDI DI
 * @{
 * Device Identification Profile (DI) enables identification of a device by its peer devices within Bluetooth wireless range. DI information for the device, such as the manufacturer, product ID
 * and product version, is exported as an explicit SDP record (DI service record) on that device within Bluetooth SDP framework.
 * Terms and Acronyms
 * ======
 * |Terms                        |Details                                                                  |
 * |-----------------------------|-------------------------------------------------------------------------|
 * |\b UUID                      | A Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 * |\b SDP                       | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Service_discovery_protocol_.28SDP.29">Wikipedia</a>. |
 *
 * @section bt_di_api_usage How to use this module
 *  - Implement #bt_sdps_get_customized_record() to add SDP records for DI service. The DI's SDP record contains seven mandatory attributes: the service class ID list,
 *    the specification ID, the vendor ID, the product ID, the version, the primary record and the vendor ID source. In general, user needs to provide specific values for the vendor ID, the product ID, the version,
 *    the primary record and the vendor ID source, while the user can simply use the default value for the service class ID list and the specification ID.
 *    For more information about other optional attributes, please refer to DI specification.
 *   - Sample code:
 *      @code
 *
 *         #define MY_BT_DI_VENDOR_ID         0x1234   // The vendor ID definition.
 *         #define MY_BT_DI_PRODUCT_ID        0x0004   // A value defined for product ID and it should be managed and assigned by the vendor.
 *         #define MY_BT_DI_VERSION           0x0100   // A value defined for the version number, in this example it's 1.0.0.
 *
 *         static const uint8_t bt_di_service_class_id_list[] = {
 *             BT_DI_SERVICE_CLASS_ID_LIST
 *         };
 *
 *         static const uint8_t bt_di_specification_id[] = {
 *             BT_DI_SPECIFICATION_ID
 *         };
 *
 *         static const uint8_t bt_di_vendor_id[] = {
 *             BT_DI_VENDOR_ID(MY_BT_DI_VENDOR_ID)    // The device ID.
 *         };
 *
 *         static const uint8_t bt_di_product_id[] = {
 *             BT_DI_PRODUCT_ID(MY_BT_DI_PRODUCT_ID)    // The product ID managed by the vendor to distinguish itself from different products.
 *         };
 *
 *         static const uint8_t bt_di_version[] = {
 *             BT_DI_VERSION(MY_BT_DI_VERSION)    // The version number assigned by the vendor to identify the device's release number.
 *         };
 *
 *         static const uint8_t bt_di_primary_record[] = {
 *             BT_DI_PRIMARY_RECORD(BT_DI_PRIMARY_RECORD_TRUE)    // It should be set with #BT_DI_PRIMARY_RECORD_TRUE, when there is only one Device ID service record in the service.
 *         };
 *
 *         static const uint8_t bt_di_vendor_id_source[] = {
 *             BT_DI_VENDOR_ID_SOURCE(BT_DI_VENDOR_ID_SOURCE_USB_FORUM_ASSIGNED)
 *         };
 *
 *         static const bt_sdps_attribute_t bt_di_sdp_attributes[] = {
 *             BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST, bt_di_service_class_id_list),
 *             BT_SDP_ATTRIBUTE(BT_DI_SDP_ATTRIBUTE_SPECIFICATION_ID, bt_di_specification_id),
 *             BT_SDP_ATTRIBUTE(BT_DI_SDP_ATTRIBUTE_VENDOR_ID, bt_di_vendor_id),
 *             BT_SDP_ATTRIBUTE(BT_DI_SDP_ATTRIBUTE_PRODUCT_ID, bt_di_product_id),
 *             BT_SDP_ATTRIBUTE(BT_DI_SDP_ATTRIBUTE_VERSION, bt_di_version),
 *             BT_SDP_ATTRIBUTE(BT_DI_SDP_ATTRIBUTE_PRIMARY_RECORD, bt_di_primary_record),
 *             BT_SDP_ATTRIBUTE(BT_DI_SDP_ATTRIBUTE_VENDOR_ID_SOURCE, bt_di_vendor_id_source)
 *         };
 *
 *         static const bt_sdps_record_t bt_di_sdp_record = {
 *             .attribute_list_length = sizeof(bt_di_sdp_attributes),
 *             .attribute_list = bt_di_sdp_attributes,
 *         };
 *
 *         static const bt_sdps_record_t *my_sdps_record[] = {
 *             &bt_di_sdp_record
 *         };
 *
 *         //Implements the function #bt_sdps_get_customized_record().
 *         uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list)
 *         {
 *             *record_list = my_sdps_record;
 *             return sizeof(my_sdps_record) / sizeof(bt_sdps_record_t *);
 *         }
 *      @endcode
 */

/**
 * @defgroup Bluetoothbt_di_define Define
 * @{
 * This section defines the macros for the DI.
 */

/**
 * @brief The attribute ID definitions in the DI service record.
 */
#define BT_DI_SDP_ATTRIBUTE_SPECIFICATION_ID    0x0200  /**< The attribute ID of specification ID for DI. */
#define BT_DI_SDP_ATTRIBUTE_VENDOR_ID           0x0201  /**< The attribute ID of vendor ID for DI. */
#define BT_DI_SDP_ATTRIBUTE_PRODUCT_ID          0x0202  /**< The attribute ID of product ID for DI. */
#define BT_DI_SDP_ATTRIBUTE_VERSION             0x0203  /**< The attribute ID of version for DI. */
#define BT_DI_SDP_ATTRIBUTE_PRIMARY_RECORD      0x0204  /**< The attribute ID of primary record for DI. */
#define BT_DI_SDP_ATTRIBUTE_VENDOR_ID_SOURCE    0x0205  /**< The attribute ID of vendor ID source for DI. */

/**
 * @brief The UUID of the DI profile.
 */
#define BT_DI_SDP_SERVICE_CLASS_PNP_INFORMATION       0x1200

/**
 * @brief The version number of the supported DI specification.
 */
#define BT_DI_SPECIFICATION_VERSION            0x0103

/**
 * @brief The vendor attribute ID for MediaTek. For more information, please refer to <a href="https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers">Bluetooth SIG</a>.
 */
#define BT_DI_VENDOR_ID_MEDIATEK    0x0046

/**
 * @brief The vendor attribute ID for AIROHA. For more information, please refer to <a href="https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers">Bluetooth SIG</a>.
 */
#define BT_DI_VENDOR_ID_AIROHA    0x0094

/**
 * @brief The settings of the primary or non-primary record.
 */
#define BT_DI_PRIMARY_RECORD_TRUE   1  /**< Define the record as a primary record. */
#define BT_DI_PRIMARY_RECORD_FALSE  0  /**< Define the record as a non-primary record. */

/**
 * @brief The organization that assigns the vendor ID.
 */
#define BT_DI_VENDOR_ID_SOURCE_SIG_ASSIGNED        0x0001  /**<The vendor ID assigned by SIG. */
#define BT_DI_VENDOR_ID_SOURCE_USB_FORUM_ASSIGNED  0x0002  /**<The vendor ID assigned by the USB forum. */

/**
 * @brief  The attribute value and length of the DI service class ID list.
 */
#define BT_DI_SERVICE_CLASS_ID_LIST \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),        /* Data Element Sequence, 3 bytes. */ \
    BT_SDP_UUID_16BIT(BT_DI_SDP_SERVICE_CLASS_PNP_INFORMATION) /* The device Identification UUID in big-endian. */

/**
 * @brief  The attribute value of the supported DI specification version.
 */
#define BT_DI_SPECIFICATION_ID \
    BT_SDP_UINT_16BIT(BT_DI_SPECIFICATION_VERSION)

/**
 * @brief  The attribute value of the vendor that provides the device.
 */
#define BT_DI_VENDOR_ID(vendor_id) \
    BT_SDP_UINT_16BIT(vendor_id)

/**
 * @brief  The attribute value of the product is managed by the vendor and used to distinguish itself from different products.
 */
#define BT_DI_PRODUCT_ID(product_id) \
    BT_SDP_UINT_16BIT(product_id)

/**
 * @brief  The attribute value of the product version number is managed by the vendor and used to identify the device release number.
 */
#define BT_DI_VERSION(version) \
    BT_SDP_UINT_16BIT(version)

/**
 * @brief  The attribute value to identify the primary record when there are multiple DI service records.
 */
#define BT_DI_PRIMARY_RECORD(is_primary_record) \
    BT_SDP_BOOL(is_primary_record)

/**
 * @brief  The attribute value to identify the group that assigns the vendor ID.
 */
#define BT_DI_VENDOR_ID_SOURCE(vendor_id_source) \
    BT_SDP_UINT_16BIT(vendor_id_source)
BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 *
*/

#endif /*__BT_DI_H__*/

