/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2020. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef __HAL_EFUSE_H__
#define __HAL_EFUSE_H__

#ifdef HAL_EFUSE_MODULE_ENABLED

#include <stdint.h>
#include "cli.h"

extern cmd_t efuse_driver_cli[];
extern int efuse_physical_write(uint32_t group, uint32_t addr, uint32_t *para);
extern int efuse_physical_read(uint32_t group, uint32_t addr, uint32_t *buffer);
extern int efuse_logical_read(uint32_t group, uint32_t addr, uint32_t *buffer);
extern int ewriter_fuse_read(uint32_t, uint32_t, uint32_t *);
extern int ewriter_fuse_write(uint32_t, uint32_t, uint32_t *);

#define EFUSE_DRV_CLI_ENTRY    { "efusedrv", "efuse driver cli cmd", NULL, efuse_driver_cli },
#define HAL_EFUSE_READ_GRP1         (0)
#define HAL_EFUSE_WRITE_GRP1        (0)
#define HAL_EFUSE_WRITE_MAGIC       (0x35929054)

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
 * @addtogroup HAL
 * @{
 * @addtogroup EFUSE
 * @{
 * This section introduces the Efuse driver APIs
 */

/*****************************************************************************
* Enum
*****************************************************************************/
/** @defgroup hal_efuse_enum Enum
 *  @{
 */
/** @brief return error code */
typedef enum {
    HAL_EFUSE_OK, /**< HAL_EFUSE_OK means working well. */
    HAL_EFUSE_INVALID_PARAMETER, /**< HAL_EFUSE_INVALID_PARAMETER means wrong group number or address. */
    HAL_EFUSE_INVALID_MAGIC, /**< HAL_EFUSE_INVALID_MAGIC means blowing magic key error. */
    HAL_EFUSE_INVALID_ACCESS /**< HAL_EFUSE_INVALID_ACCESS means excution fail. */
} hal_efuse_status_t;

/** @defgroup hal_efuse_enum Enum
 *  @{
  */
/** @brief ewriter cmd */
typedef enum {
    HAL_EWRITER_READ = 0,
    HAL_EWRITER_WRITE,
    HAL_EWRITER_CMD_NUM
} hal_efuse_cmd_t;

/**
  * @}
  */

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief Write data into eFuse. This API just provide for GRP1.
 *
 * Write a block size of data in buffer <i>buf</i> into eFuse at the
 * physical address <i>addr</i>.
 *
 * @param group efuse group should be 0 for GRP1.
 * @param addr eeprom block address to write. example : eeprom address 0x0b0 under GRP1 is equal to
 *             0x0b0 logical address of GRP1.
 * @param buf efuse block size of data to write from.
 * @return HAL_EFUSE_OK if write succeeded.
 * @return HAL_EFUSE_INVALID_PARAMETER group is not supported.
 * @return HAL_EFUSE_INVALID_ACCESS efuse already be blown, address is not supported.
 */
hal_efuse_status_t hal_efuse_physical_write(uint32_t group, uint32_t addr, uint32_t *buf);


/**
 * @brief Read data from eFuse physical addr.
 *
 * Read a block size of data from physical address <i>addr</i> in eFuse
 * into buffer <i>buf</i>.
 *
 * @param group efuse group shoud be in range from 0 to 2
 * @param addr eeprom physical address to read from.
 * @param buf efuse block size of data to read to.
 * @return HAL_EFUSE_OK if read succeeded.
 * @return HAL_EFUSE_INVALID_ACCESS address is not supported
 * @return HAL_EFUSE_INVALID_PARAMETER group is not supported.
 */
hal_efuse_status_t hal_efuse_physical_read(uint32_t group, uint32_t addr, uint32_t *buf);

/**
 * @brief Read data from eFuse logical addr.
 *
 * Read a block size of data from logical address <i>addr</i> in eFuse
 * into buffer <i>buf</i>.
 *
 * @param group efuse group shoud be in range from 1 to 2.
 *              1 for GRP2, 2 for GRP3.
 * @param addr logical block address to read from.
 * @param buf efuse block size of data to read to.
 * @return HAL_EFUSE_INVALID_ACCESS the efuse block address doesn't exist in the mapping table(un-blown).
 * @return HAL_EFUSE_INVALID_PARAMETER group is not supported.
 */
hal_efuse_status_t hal_efuse_logical_read(uint32_t group, uint32_t addr, uint32_t *buf);


/**
 * @brief Calculate mapping value
 *
 * Calculate mapping value includes parity.
 *
 * @param addr  logical traget address
 * @return mapping value
 */
int hal_cal_mapping_table(int addr);

/**
 * This function provides for ACS and doesn't use on hal.
 * Read group1 data from eFuse physical addr.
 *
 * @param addr eeprom physical block address to read from
 * @param buf efuse block size of data to read to.
 * @return HAL_EFUSE_OK if read succeeded.
 * @return HAL_EFUSE_INVALID_PARAMETER buf is incorrect.
 * @return HAL_EFUSE_INVALID_ACCESS address is not supported.
 */
hal_efuse_status_t hal_efuse_read(uint32_t addr, uint32_t *buf);

/**
 * This function provides for ACS and doesn't use on hal.
 * Write a block size of data into eFuse group 1.
 *
 * @param magic check if a match for hal driver
 * @param addr eeprom block address to read from.
 * @param buf efuse block size of data to read to.
 * @return HAL_EFUSE_OK if write succeeded.
 * @return HAL_EFUSE_INVALID_MAGIC the magic is wrong.
 * @return HAL_EFUSE_INVALID_PARAMETER buf is incorrect, address is not supported.
 * @return HAL_EFUSE_INVALID_ACCESS the efuse address already be blown.
 */
hal_efuse_status_t hal_efuse_write(uint32_t magic, uint32_t addr, uint32_t *buf);

/**
 * Read data from eFuse logical group and print out data.
 * Only GRP2 and GRP3 have logical address
 *
 * @param group efuse group shoud be in range from 1 to 2.
 *              1 for GRP2, 2 for GRP3.
 * @return HAL_EFUSE_OK if read succeeded.
 * @return HAL_EFUSE_INVALID_PARAMETER group is incorrect.
 */
hal_efuse_status_t hal_efuse_logical_read_group(uint32_t group);

/**
 * Read data from eFuse physical group and print out of data.
 * note: GRP2/GRP3 should remap by the mapping table.
 *
 * @param group efuse group shoud be in range from 0 to 2
 * @return HAL_EFUSE_OK if read succeeded.
 * @return HAL_EFUSE_INVALID_PARAMETER group is out of range.
 */
hal_efuse_status_t hal_efuse_physical_read_group(uint32_t group);


/**
 * ewriter tool with index
 *
 * @param cmd 0:read 1:write
 * @param index reference from index table
 * @param length the index has fixing length
 * @param buf efuse block size of data to read to.
 * @return HAL_EFUSE_INVALID_ACCESS reading/writing fail
 */
hal_efuse_status_t hal_efuse_ewriter(uint32_t cmd, uint32_t index, uint32_t length, uint32_t *buf);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/**
* @}
* @}
*/

#endif /* #ifdef HAL_EFUSE_MODULE_ENABLED */

#endif /* #ifndef __HAL_EFUSE_H__ */
