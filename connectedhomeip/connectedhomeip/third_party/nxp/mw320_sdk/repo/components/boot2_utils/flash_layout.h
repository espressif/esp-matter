/*
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

/*! \file flash_layout.h
 * \brief Flash layout details
 *
 * This file presents entire layout of flash including secondary boot-loader,
 * firmware(including redundant partition), file-system and persistent storage
 * manager.
 *
 * The flash layout defined in this file is used in a consistent fashion
 * across all of SDK, namely by the flashprog utility, secondary boot loader
 * (boot2) and Firmware Upgrade code.
 */

#ifndef _FLASH_LAYOUT_H_
#define _FLASH_LAYOUT_H_

/** Flash base address */
#define MW300_FLASH_BASE 0x0
/** Flash sector size */
#define MW300_FLASH_SECTOR_SIZE 0x1000 /*!< 4KB */

/** Section: Secondary stage boot-loader with bootrom header
 *  Start:   0x0
 *  Length:  0x4000(16KiB)
 *  Device:  Internal Flash
 */
#define FL_BOOT2_START    (MW300_FLASH_BASE + 0x0)
#define FL_BOOTROM_H_SIZE (0x400)
/* Note: Maximum size considering future requirements */
#define FL_BOOT2_BLOCK_SIZE (MW300_FLASH_SECTOR_SIZE * 4)
#define FL_BOOT2_BLOCK_END  (FL_BOOT2_START + FL_BOOT2_BLOCK_SIZE)
#define FL_BOOT2_DEV        FL_INT

/** Section: Partition table 1
 *  Start:   0x4000
 *  Length:  0x1000(4KiB)
 *  Device:  Internal Flash
 */
#define FL_PART_SIZE       (MW300_FLASH_SECTOR_SIZE)
#define FL_PART1_START     FL_BOOT2_BLOCK_END
#define FL_PART1_TABLE_END (FL_PART1_START + FL_PART_SIZE)
#define FL_PART_DEV        FL_INT

/** Section: Partition table 2
 *  Start:   0x5000
 *  Length:  0x1000(4KiB)
 *  Device:  Internal Flash
 */
#define FL_PART2_START FL_PART1_TABLE_END
#define FL_PART2_END   (FL_PART2_START + FL_PART_SIZE)

#endif /* !  _FLASH_LAYOUT_H_ */
