/** @file boot_flags.h
 *
 *  @brief Boot Flags
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

#ifndef __boot_flags_h
#define __boot_flags_h

#include <wmtypes.h>
#include "fsl_power.h"

/* BooROM version for A0 chip */
#define BOOTROM_A0_VERSION 3

/*
 * Location where the boot flags are stored by the bootloader.
 * Currently this is stored in NVRAM area after BootROM occupied range.
 */
#define BOOT_FLAGS ((volatile uint32_t *)0x480C0024)

/* final location for those boot flags */
extern uint32_t g_boot_flags;
extern uint32_t g_prev_version_fw;
extern uint32_t g_rst_cause;

/* boot flag definitions */
/* partition number used to boot */
#define BOOT_PARTITION_MASK 0x03
/* boot partition table */
#define BOOT_PARTITION_TABLE_MASK 0x100
/* partition table1 state */
#define PARTITION_TABLE1_STATE_MASK 0x200
/* partition table2 state */
#define PARTITION_TABLE2_STATE_MASK 0x400
/* main firmware had errors */
#define BOOT_MAIN_FIRMWARE_BAD_CRC 0x20

/* secure boot config flags for firmware */
#define BOOT_SECURE_FW_ENABLE  0x10000
#define BOOT_SECURE_SIGN_FW    0x20000
#define BOOT_SECURE_ENCRYPT_FW 0x40000

/*
 * Bootrom stores some information in initial few bytes of retention ram.
 * Following is the format used for storing this info.
 */
struct bootrom_info
{
    uint32_t pm3_entryaddr;
    uint32_t bootMode;
    uint32_t powerMode;
};

extern unsigned long _nvram_start;

static inline int boot_get_partition_no()
{
    return g_boot_flags & BOOT_PARTITION_MASK;
}

static inline int boot_old_fw_version()
{
    return g_prev_version_fw;
}

static inline uint32_t boot_store_reset_cause()
{
    uint32_t rst_cause = POWER_GetResetCause();
    POWER_ClearResetCause(kPOWER_ResetCauseAll);
    return rst_cause;
}

static inline uint32_t boot_reset_cause()
{
    return g_rst_cause;
}

void boot_init();
void boot_report_flags();

#endif
