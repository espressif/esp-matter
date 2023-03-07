/* Copyright Statement:
 *
 * (C) 2021-2021  MediaTek Inc. All rights reserved.
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

#ifndef __FOTA_H__
#define __FOTA_H__


#ifdef MTK_FOTA_V3_ENABLE


/****************************************************************************
 *
 * HEADER FILES
 *
 ****************************************************************************/


// C library headers
#include <stdint.h>
#include <stdlib.h>


// hal headers
#include <hal_flash.h>


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
 *
 * @section FOTA_Architechture_Chapter Software Architecture of FOTA
 *
 * MediaTek MT7933 provides command line to operate FOTA functionality. It
 * also offers a reference example with details on how to download using
 * Wi-Fi, enable HTTP client on the target device and setup a HTTP server to
 * share a FOTA package file. Then device completes the downloads with the
 * specified input command.
 *
 * @image html fota_mt7687_arch.png
 *
 *@}
 */


/****************************************************************************
 *
 * TYPE DECLARATION
 *
 ****************************************************************************/


/**
  * @addtogroup FOTA
  * @{
  */


/** @brief
 * FOTA trigger update result definition
 */
typedef enum {
    FOTA_STATUS_OK,                     ///< action succeeded
    FOTA_STATUS_ERROR_INVALD_PARAMETER, ///< NULL parameter or zero length
    FOTA_STATUS_ERROR_BLOCK_ALIGN,      ///< partition not block-aligned
    FOTA_STATUS_ERROR_OUT_OF_RANGE,     ///< offset out-of-range
    FOTA_STATUS_ERROR_UNKNOWN_ID,       ///< unknown partition
    FOTA_STATUS_ERROR_NOT_INITIALIZED,  ///< not initialized yet
    FOTA_STATUS_ERROR_FLASH_OP,         ///< flash access failed
    FOTA_STATUS_ERROR_PROTOCOL,         ///< the protocol is unsupported.
    FOTA_STATUS_DOWNLOAD_FAIL,          ///< download failed during progress
    FOTA_STATUS_ERROR_CONTROL_BLOCK_CORRUPTION, ///< control block not recognized
    FOTA_STATUS_ERROR_OUT_OF_MEMORY,    ///< not enough memory
} fota_status_t;


typedef struct _fota_partition_t {
    uint32_t    id;
    uint32_t    address;
    uint32_t    length;
} fota_partition_t;


typedef struct _fota_flash_t {
    const fota_partition_t  *table;
    uint8_t                 table_entries;
    uint32_t                bus_address;
    uint32_t                block_size;
} fota_flash_t;


#define FOTA_UPGRADE_STATUS_MAGIC   (0x27182818)
typedef enum
{
    FOTA_UPGRADE_STATUS_NONE    = 0xFFFFFFFF, // no valid image
    FOTA_UPGRADE_STATUS_READY   = 0x00000001, // image is ready
    FOTA_UPGRADE_STATUS_RUNNING = 0x00000003, // upgrade was interrupted
    FOTA_UPGRADE_STATUS_INVALID = 0x00000005, // image parse failed
} fota_upgrade_state_t;


typedef struct _fota_upgrade_info_t
{
    uint32_t                magic;
    fota_upgrade_state_t    state;
    uint8_t                 reserved[ 4096 - sizeof( uint32_t ) * 2 ];
} fota_upgrade_info_t;


typedef struct _fota_io_state_t
{
    uint32_t            phy_addr;
    uint32_t            bus_addr;
    uint32_t            size;
    uint32_t            block_size;
    uint32_t            block_mask;
    hal_flash_block_t   block_type;

    uint32_t            offset;
} fota_io_state_t;


/**
  * @}
  */


/****************************************************************************
 *
 * PUBLIC FUNCTIONS
 *
 ****************************************************************************/


/**
 * @addtogroup FOTA
 * @{
 */


/**
 * @brief
 * Initialize an I/O access information of a flash partition.
 *
 * @param flash     flash partition table
 * @param partition the id of partition
 * @param io        I/O access information
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_BLOCK_ALIGN        partition not block-aligned
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID         unknown partition
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 */
fota_status_t fota_io_init(
    const fota_flash_t  *flash,
    uint32_t            partition,
    fota_io_state_t     *io);


/**
 * @brief
 * Selects the offset of read/write pointer of the specified <i>partition</i>.
 *
 * @param io        I/O access information
 * @param offset    offset of next access
 *
 * This API must be called once before reading/writing to ANY partition to
 * ensure the current offset is correct.
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 */
fota_status_t fota_io_seek(
    fota_io_state_t *io,
    int32_t         offset );


/**
 * @brief
 * Read the data at the current offset in the specified <i>partition</i> to
 * <i>buffer</i> and increments the read pointer to new offset.
 *
 * @param io        I/O access information
 * @param buffer    memory buffer to read into
 * @param length    memory buffer legnth
 *
 * @note This API is supposed to be called multiple times because the dynamic
 * memory space is smaller than flash size for some partitions.
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 * @retval FOTA_STATUS_ERROR_FLASH_OP           flash access failed
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 */
fota_status_t fota_io_read(
    fota_io_state_t *io,
    void            *buffer,
    uint32_t        length );


/**
 * @brief
 * Write the data in <i>buffer</i> to flash partition and increments the
 * write pointer to new offset.
 *
 * @note This API is supposed to be called multiple times because the dynamic
 * memory space is smaller than flash size for some partitions.
 *
 * @param io        I/O access information
 * @param buffer    memory buffer to write from
 * @param length    memory buffer legnth
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 * @retval FOTA_STATUS_ERROR_FLASH_OP           flash access failed
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 */
fota_status_t fota_io_write(
    fota_io_state_t *io,
    const void      *buffer,
    uint32_t        length );


/**
 * @brief
 * Get the bootloader FOTA update result information, and clear the update
 * results.
 *
 * @param flash     flash partition table
 * @param info      read upgrade status into <i>info</i>
 * @param partition the id of partition
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_BLOCK_ALIGN        partition not block-aligned
 * @retval FOTA_STATUS_ERROR_CONTROL_BLOCK_CORRUPTION   control block not
 *                                                      recognized
 * @retval FOTA_STATUS_ERROR_FLASH_OP           flash access failed
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 */
fota_status_t fota_read_info(
    const fota_flash_t  *flash,
    fota_upgrade_info_t *info,
    const uint32_t      partition );


/**
 * @brief
 * Write FOTA update status information.
 *
 * @param flash     flash partition table
 * @param info      write upgrade status from <i>info</i>
 * @param partition the id of partition
 *
 * @param info the upgrade status is stored into this structure.
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_BLOCK_ALIGN        partition not block-aligned
 * @retval FOTA_STATUS_ERROR_FLASH_OP           flash access failed
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID         unknown partition
 */
fota_status_t fota_write_info(
    const fota_flash_t      *flash,
    fota_upgrade_info_t     *info,
    uint32_t                partition );


/**
 * @brief
 * Set the FOTA image status to invalid.
 *
 * @param flash     flash partition table
 * @param partition the id of partition
 *
 * This is used when the image is considered broken and not able to upgrade.
 *
 * @param info the upgrade status is stored into this structure.
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_BLOCK_ALIGN        partition not block-aligned
 * @retval FOTA_STATUS_ERROR_CONTROL_BLOCK_CORRUPTION   control block not
 *                                                      recognized
 * @retval FOTA_STATUS_ERROR_FLASH_OP           flash access failed
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 * @retval FOTA_STATUS_ERROR_OUT_OF_MEMORY      not enough memory
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID         unknown partition
 */
fota_status_t fota_invalidate_info(
    const fota_flash_t      *flash,
    const uint32_t          partition );


/**
 * @brief
 * Set upgrade flag for the bootloader to check whether to enter
 * FOTA update process or not.
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_BLOCK_ALIGN        partition not block-aligned
 * @retval FOTA_STATUS_ERROR_FLASH_OP           flash access failed
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 * @retval FOTA_STATUS_ERROR_OUT_OF_MEMORY      not enough memory
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID         unknown partition
 *
 * @note Call the reboot API after using this function.
 *
 * @code
 *     // Trigger a FOTA update.
 *     fota_status_t ret;
 *     ret = fota_trigger_update();
 *     if (ret != FOTA_STATUS_OK)
 *     {
 *         // Failed to write the register flag, error handling.
 *     }
 *     // Call the reboot API, hal_wdt_software_reset().
 * @endcode
 */
fota_status_t fota_trigger_upgrade(
    const fota_flash_t      *flash,
    const uint32_t          partition );


/**
 * @brief
 * Clear upgrade indication flag so bootloader will not perform update
 * process.
 *
 * @param flash     flash partition table
 * @param partition the id of partition
 *
 * @retval FOTA_STATUS_OK                       action succeeded
 * @retval FOTA_STATUS_ERROR_BLOCK_ALIGN        partition not block-aligned
 * @retval FOTA_STATUS_ERROR_FLASH_OP           flash access failed
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER   NULL parameter or zero length
 * @retval FOTA_STATUS_ERROR_OUT_OF_MEMORY      not enough memory
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE       offset out-of-range
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID         unknown partition
 *
 * @note Call the reboot API after using this function.
 *
 * @code
 *     // Trigger a FOTA update.
 *     fota_status_t ret;
 *     ret = fota_trigger_update();
 *     if (ret != FOTA_STATUS_OK)
 *     {
 *         // Failed to write the register flag, error handling.
 *     }
 *     // Call the reboot API, hal_wdt_software_reset().
 * @endcode
 */
fota_status_t fota_defuse_upgrade(
    const fota_flash_t      *flash,
    const uint32_t          partition );


/**
  * @}
  */


#ifdef __cplusplus
}
#endif


#endif /* MTK_FOTA_V3_ENABLE */


#endif /* __FOTA_H__ */

