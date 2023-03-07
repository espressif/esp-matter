/** @file boot_flags.c
 *
 *  @brief This file provides  API for Boot Flags
 *
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

#include "boot_flags.h"
#include "fsl_debug_console.h"

#define boot_l(...) PRINTF("boot", ##__VA_ARGS__)

#define REVID_MASK 0x1FF

/* permanent boot flags storage */
uint32_t g_boot_flags;
uint32_t g_prev_version_fw;
uint32_t g_rst_cause;

void boot_init(void)
{
    /* Read boot flags stored by boot2 */
    g_boot_flags = *BOOT_FLAGS;

    if (g_boot_flags & BOOT_MAIN_FIRMWARE_BAD_CRC)
    {
        g_prev_version_fw = 1;
    }

    g_rst_cause = boot_store_reset_cause();
}

void boot_report_flags(void)
{
    boot_l("Boot Flags: 0x%x", g_boot_flags);

    boot_l(" - Partition Table: %d ", !(!(g_boot_flags & BOOT_PARTITION_TABLE_MASK)));

    boot_l(" - Firmware Partition: %d ", g_boot_flags & BOOT_PARTITION_MASK);

    if (g_boot_flags & BOOT_MAIN_FIRMWARE_BAD_CRC)
    {
        boot_l(
            " - Backup firmware due to CRC error in main"
            " firmware");
    }
    /* Bits of register SYS_CTRL->REV_ID can be described
     * as follows:
     *             [31:24]         Company_ID
     *             [23:20]         Foundry
     *             [19:16]         Process
     *             [15:14]         Project_ID
     *             [8:6]           Project_Rev_first_part
     *             [5:3]           Project_Rev_second_part
     *             [2:0]           Project_Rev_third_part
     */
    boot_l("Boot Info:");
    boot_l(" - Chip revision id: 0x%x", SYS_CTL->REV_ID);

    boot_l("Reset Cause Register: 0x%x", boot_reset_cause());
    if (boot_reset_cause() & (1 << 5))
        boot_l(" - Watchdog reset bit is set");
}
