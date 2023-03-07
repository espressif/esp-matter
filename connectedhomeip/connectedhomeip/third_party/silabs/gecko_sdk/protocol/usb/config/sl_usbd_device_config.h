/***************************************************************************//**
 * @file
 * @brief USBD Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_USBD_DEVICE_CONFIG_H
#define SL_USBD_DEVICE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> USB Device Configuration

// <o SL_USBD_DEVICE_VENDOR_ID> Device Vendor ID
// <i> Default: 0xFFFE
// <i> Device vendor ID.
#define SL_USBD_DEVICE_VENDOR_ID                0xFFFE

// <o SL_USBD_DEVICE_PRODUCT_ID> Device Product ID
// <i> Default: 0x1234
// <i> Device product ID.
#define SL_USBD_DEVICE_PRODUCT_ID               0x1234

// <o SL_USBD_DEVICE_RELEASE_NUMBER> Device Release Number
// <i> Default: 0x0100
// <i> Device release number.
#define SL_USBD_DEVICE_RELEASE_NUMBER           0x0100

// <s SL_USBD_DEVICE_MANUFACTURER_STRING> Device Manufacturer Name
// <i> Default: "Silicon Labs"
// <i> Device manufacturer string.
#define SL_USBD_DEVICE_MANUFACTURER_STRING      "Silicon Labs"

// <s SL_USBD_DEVICE_PRODUCT_STRING> Device Product Name
// <i> Default: "Product"
// <i> Device product string.
#define SL_USBD_DEVICE_PRODUCT_STRING           "Product"

// <s SL_USBD_DEVICE_SERIAL_NUMBER_STRING> Device Serial Number
// <i> Default: "1234567890ABCDEF"
// <i> Device serial number string.
#define SL_USBD_DEVICE_SERIAL_NUMBER_STRING     "1234567890ABCDEF"

// <o SL_USBD_DEVICE_LANGUAGE_ID> Device Language ID
//   <SL_USBD_LANG_ID_ARABIC_SAUDI_ARABIA=> Arabic
//   <SL_USBD_LANG_ID_CHINESE_TAIWAN=> Chinese
//   <SL_USBD_LANG_ID_ENGLISH_US=> US English
//   <SL_USBD_LANG_ID_ENGLISH_UK=> UK English
//   <SL_USBD_LANG_ID_FRENCH=> French
//   <SL_USBD_LANG_ID_GERMAN=> German
//   <SL_USBD_LANG_ID_GREEK=> Greek
//   <SL_USBD_LANG_ID_ITALIAN=> Italian
//   <SL_USBD_LANG_ID_PORTUGUESE=> Portuguese
//   <SL_USBD_LANG_ID_SANSKRIT=> Sanskrit
// <i> ID of language of strings of device.
// <i> Default: USBD_LANG_ID_ENGLISH_US
#define SL_USBD_DEVICE_LANGUAGE_ID            SL_USBD_LANG_ID_ENGLISH_US

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_DEVICE_CONFIG_H
