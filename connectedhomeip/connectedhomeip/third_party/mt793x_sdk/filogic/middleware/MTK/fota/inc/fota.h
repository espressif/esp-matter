/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
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

#ifndef __FOTA_API_H__
#define __FOTA_API_H__
#include "stdint.h"
#ifdef FOTA_FS_ENABLE
#include "bl_fota.h"
#include "stdbool.h"
#endif
#ifdef __cplusplus
extern "C"
{
#endif

/**
 *@addtogroup FOTA
 *@{
 * This section introduces the FOTA interface APIs including terms and acronyms, supported features, software architecture, details on how to use this interface, FOTA function groups, enums, structures and functions.
 * This interface writes into a pre-defined register flag. The bootloader enters the update process based on the value written in the flag. The return error code indicates if the flag is written successfully or not. 
 * After the update is complete, the UA automatically resets the register flag.
 * Note, it is recommended to call the reboot API after calling the FOTA interface, because fota_trigger_update() does not contain reboot functionality.
 * There is no dedicated FOTA read or write SDK API, it is suggested to use HAL flash SDK API to write data to specified address of flash.
 * Regarding to the FOTA update flow & usage, please refer to the Firmware Update Developers Guide located under [sdk_root]/doc folder.
 *
 *
 * @section fota_terms Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b FOTA                       | Firmware Over-The-Air (FOTA) is a Mobile Software Management technology in which the operating firmware of a mobile device is wirelessly upgraded and updated by its manufacturer. Please refer to <a href="https://www.techopedia.com/definition/24236/firmware-over-the-air-fota"> FOTA in Techopedia.</a>|
 * |\b UA                         | Update Agent performs the actual FOTA update on devices. It operates as stand-alone application in bootloader and replaces the old firmware with the new one.|
 * |\b TFTP                       | Trivial File Transfer Protocol (TFTP) is a simple, lockstep, File Transfer Protocol which allows a client to get from or put a file onto a remote host. One of its primary uses is in the early stages of nodes booting from a local area network. For an introduction to TFTP, please refer to <a href="https://en.wikipedia.org/wiki/Trivial_File_Transfer_Protocol"> TFTP in Wikipedia.</a>|
 *
 *@}
 */

/**
 * @addtogroup FOTA
 * @{
 * @section FOTA_Architechture_Chapter Software Architecture of FOTA
 * MediaTek MT7687/MT7697/MT7682/MT7686 provides command line to operate FOTA functionality. It also offers a reference example with details on how to download using Wi-Fi, enable TFTP client on the target device and setup a TFTP server to share a FOTA package file. Then device completes the downloads with the specified input command.
 * @image html fota_mt7687_arch.png
 *
 * MediaTek MT2523 provides a reference design using FOTA package downloads. The supporting connectivity between Android application package and MT2523 HDK is Bluetooth. It enables transferring packages from the file system of smartphones over FOTA to the MT2523 HDK. Then MT2523 device writes the data from the file to the specified, pre-defined partition.
 * @image html fota_mt2523_arch.png
 *
 *@}
 */

/**
  * @addtogroup FOTA
  * @{
  * @defgroup fota_enum Enums
  * @{
  */

/** @brief
 * FOTA trigger update result definition
 */
typedef enum
{
    FOTA_TRIGGER_FAIL = -1,      /**< Failed to write the FOTA register flag. */
    FOTA_TRIGGER_SUCCESS = 0     /**< The FOTA register flag is successfully written. */
}fota_ret_t;

/**
  * @}
  */

/**
 * @brief
 * Provide the register flag for the bootloader to check whether to enter FOTA update process or not.
 *
 * @return FOTA_TRIGGER_SUCCESS/FOTA_TRIGGER_FAIL.
 * @note Call the reboot API after using this function.
 * @par
 * @code
 *     //Trigger a FOTA update.
 *     fota_ret_t ret;
 *     ret = fota_trigger_update();
 *     if (ret < FOTA_TRIGGER_SUCCESS)
 *     {
 *         // Failed to write the register flag, error handling.
 *     }
 *     // Call the reboot API, hal_wdt_software_reset().
 * @endcode
 */
fota_ret_t fota_trigger_update(void);

/**
 * @}
 */

#ifdef FOTA_FS_ENABLE
/**
 * @addtogroup FOTA
 * @{
 */

/**
 * @brief
 * Get the bootloader FOTA update result information, and clear the update results.
 * @return   whether the update information is successfully got.
 * @input    bl_fota_update_info_t
 * @output   bl_fota_update_info_t
 */
bool readAndClearBlFotaUpdateInfo(bl_fota_update_info_t* pInfo);

/**
 * @brief
 * check whether the FOTA is executed in bootloader.
 * @return   true: FOTA is executed in bootloader when power up.
 * @input    no
 * @output   no
 */
bool isFotaExecuted(void);

/**
  * @}
  */
#endif
#ifdef __cplusplus
}
#endif
#endif /* __FOTA_API_H__ */

