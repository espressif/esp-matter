/***************************************************************************//**
 * @file
 * @brief USBD HID Configuration
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

#ifndef SL_USBD_HID_INSTANCE_CONFIG_H
#define SL_USBD_HID_INSTANCE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Class Configuration

// <s SL_USBD_HID_INSTANCE_CONFIGURATIONS> Configuration(s) to add this class instance to
// <i> Default: all
// <i> Comma separated list of configuration instances (like inst0, inst1)
// <i> that this HID class instance will be attached to. You can
// <i> use "all" to attach the class to all configs, or use an empty
// <i> string if you do not want to attach the interface to any configuration.
#define SL_USBD_HID_INSTANCE_CONFIGURATIONS        "all"

// </h>

// <h> Type Codes

// <o SL_USBD_HID_INSTANCE_SUBCLASS> Subclass code
//   <SL_USBD_HID_SUBCLASS_NONE=> None
//   <SL_USBD_HID_SUBCLASS_BOOT=> Boot
// <i> Default: SL_USBD_HID_SUBCLASS_BOOT
// <i> This defines the standard USB subclass code for this interface.
// <i> For most use cases, you can just select "Boot".
#define SL_USBD_HID_INSTANCE_SUBCLASS              SL_USBD_HID_SUBCLASS_BOOT

// <o SL_USBD_HID_INSTANCE_PROTOCOL> Protocol code
//   <SL_USBD_HID_PROTOCOL_NONE=> None
//   <SL_USBD_HID_PROTOCOL_KBD=> Keyboard
//   <SL_USBD_HID_PROTOCOL_MOUSE=> Mouse
// <i> Default: SL_USBD_HID_PROTOCOL_MOUSE
// <i> You can choose "Mouse" or "Keyboard" depending on what functionality
// <i> this HID class instance will provide.
#define SL_USBD_HID_INSTANCE_PROTOCOL              SL_USBD_HID_PROTOCOL_MOUSE

// <o SL_USBD_HID_INSTANCE_COUNTRY_CODE> Country code
//   <SL_USBD_HID_COUNTRY_CODE_NOT_SUPPORTED=> Not supported
//   <SL_USBD_HID_COUNTRY_CODE_ARABIC=> Arabic
//   <SL_USBD_HID_COUNTRY_CODE_BELGIAN=> Belgian
//   <SL_USBD_HID_COUNTRY_CODE_CANADIAN_BILINGUAL=> Canadian Multilingual
//   <SL_USBD_HID_COUNTRY_CODE_CANADIAN_FRENCH=> Canadian French
//   <SL_USBD_HID_COUNTRY_CODE_CZECH_REPUBLIC=> Czech Republic
//   <SL_USBD_HID_COUNTRY_CODE_DANISH=> Danish
//   <SL_USBD_HID_COUNTRY_CODE_FINNISH=> Finnish
//   <SL_USBD_HID_COUNTRY_CODE_FRENCH=> French
//   <SL_USBD_HID_COUNTRY_CODE_GERMAN=> German
//   <SL_USBD_HID_COUNTRY_CODE_GREEK=> Greek
//   <SL_USBD_HID_COUNTRY_CODE_HEBREW=> Hebrew
//   <SL_USBD_HID_COUNTRY_CODE_HUNGARY=> Hungary
//   <SL_USBD_HID_COUNTRY_CODE_INTERNATIONAL=> International
//   <SL_USBD_HID_COUNTRY_CODE_ITALIAN=> Italian
//   <SL_USBD_HID_COUNTRY_CODE_JAPAN_KATAKANA=> Japan Katakana
//   <SL_USBD_HID_COUNTRY_CODE_KOREAN=> Korean
//   <SL_USBD_HID_COUNTRY_CODE_LATIN_AMERICAN=> Latin American
//   <SL_USBD_HID_COUNTRY_CODE_NETHERLANDS_DUTCH=> Netherlands Dutch
//   <SL_USBD_HID_COUNTRY_CODE_NORWEGIAN=> Norwegian
//   <SL_USBD_HID_COUNTRY_CODE_PERSIAN_FARSI=> Persian Farsi
//   <SL_USBD_HID_COUNTRY_CODE_POLAND=> Poland
//   <SL_USBD_HID_COUNTRY_CODE_PORTUGUESE=> Portuguese
//   <SL_USBD_HID_COUNTRY_CODE_RUSSIA=> Russia
//   <SL_USBD_HID_COUNTRY_CODE_SLOVAKIA=> Slovakia
//   <SL_USBD_HID_COUNTRY_CODE_SPANISH=> Spanish
//   <SL_USBD_HID_COUNTRY_CODE_SWEDISH=> Swedish
//   <SL_USBD_HID_COUNTRY_CODE_SWISS_FRENCH=> Swiss French
//   <SL_USBD_HID_COUNTRY_CODE_SWISS_GERMAN=> Swiss German
//   <SL_USBD_HID_COUNTRY_CODE_SWITZERLAND=> Switzerland
//   <SL_USBD_HID_COUNTRY_CODE_TAIWAN=> Taiwan
//   <SL_USBD_HID_COUNTRY_CODE_TURKISH_Q=> Turkish Q
//   <SL_USBD_HID_COUNTRY_CODE_TURKISH_F=> Turkish F
//   <SL_USBD_HID_COUNTRY_CODE_UK=> United Kingdom
//   <SL_USBD_HID_COUNTRY_CODE_US=> United States
//   <SL_USBD_HID_COUNTRY_CODE_YUGOSLAVIA=> Yugoslavia
// <i> Default: SL_USBD_HID_COUNTRY_CODE_US
// <i> If this instance is implementing a keyboard interface, this
// <i> field helps the host operating system know which layout/language
// <i> the keyboard is manufactured for, or which country/localization
// <i> setting to use by default.
#define SL_USBD_HID_INSTANCE_COUNTRY_CODE          SL_USBD_HID_COUNTRY_CODE_US

// </h>

// <h> Protocol Details

// <o SL_USBD_HID_INSTANCE_INTERVAL_IN> IN polling interval
//   <1=> 1ms
//   <2=> 2ms
//   <4=> 4ms
//   <8=> 8ms
//   <16=> 16ms
//   <32=> 32ms
//   <64=> 64ms
//   <128=> 128ms
//   <256=> 256ms
//   <512=> 512ms
//   <1024=> 1024ms
//   <2048=> 2048ms
//   <4096=> 4096ms
//   <8192=> 8192ms
//   <16384=> 16384ms
//   <32768=> 32768ms
// <i> Default: 2
// <i> Polling interval for input transfers, in milliseconds.
// <i> It must be a power of 2.
#define SL_USBD_HID_INSTANCE_INTERVAL_IN          2

// <o SL_USBD_HID_INSTANCE_INTERVAL_OUT> OUT polling interval
//   <1=> 1ms
//   <2=> 2ms
//   <4=> 4ms
//   <8=> 8ms
//   <16=> 16ms
//   <32=> 32ms
//   <64=> 64ms
//   <128=> 128ms
//   <256=> 256ms
//   <512=> 512ms
//   <1024=> 1024ms
//   <2048=> 2048ms
//   <4096=> 4096ms
//   <8192=> 8192ms
//   <16384=> 16384ms
//   <32768=> 32768ms
// <i> Default: 2
// <i> Polling interval for input transfers, in milliseconds.
// <i> It must be a power of 2.
#define SL_USBD_HID_INSTANCE_INTERVAL_OUT         2

// <q SL_USBD_HID_INSTANCE_ENABLE_CTRL_RD> Enable Control Read
// <i> Default: 1
// <i> Enable read operations through the control transfers.
#define SL_USBD_HID_INSTANCE_ENABLE_CTRL_RD       1

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_HID_INSTANCE_CONFIG_H
