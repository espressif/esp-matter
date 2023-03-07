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

#ifndef __EXCEPTION_HANDLER__
#define __EXCEPTION_HANDLER__

#include <stdbool.h>

#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533)
#include "mt2523.h"
#include "hal_flash_mtd.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 2625)
#include "mt2625.h"
#include "hal_flash_mtd.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#include "mt7687.h"
#include "flash_sfc.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682)
#include "mt7686.h"
#include "flash_sfc.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 7933)
#include "mt7933.h"
//#include "flash_sfc.h"
#define configUSE_FLASH_SUSPEND 0
#endif

#if (PRODUCT_VERSION == 5932)
#include "mt7686.h"
#define configUSE_FLASH_SUSPEND 0
#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#include "flash_map.h"
#include "xflash_map.h"
#define CRASH_CONTEXT_FLASH_BASE     CRASH_CONTEXT_BASE
#define CRASH_CONTEXT_RESERVED_SIZE  CRASH_CONTEXT_LENGTH

#define CRASH_CONTEXT_EXT_FLASH_BASE     CRASH_CONTEXT_EXT_BASE
#define CRASH_CONTEXT_EXT_RESERVED_SIZE  CRASH_CONTEXT_EXT_LENGTH

#endif

#if (PRODUCT_VERSION == 2523)
#include "memory_map.h"
#define CRASH_CONTEXT_FLASH_BASE     (CRASH_CONTEXT_BASE - BL_BASE)
#define CRASH_CONTEXT_RESERVED_SIZE  CRASH_CONTEXT_LENGTH
#endif

void exception_get_assert_expr(const char **expr, const char **file, int *line);

#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_KIPRINTF_AS_PRINTF)
extern int KiPrintf (const char *format, ...);
#define platform_printf KiPrintf
#else
#define platform_printf printf
#endif

#define DISABLE_MEMDUMP_MAGIC 0xdeadbeef
#define DISABLE_WHILELOOP_MAGIC 0xdeadaaaa

typedef enum E_EXCEPT_CFG_TYPE
{
    EXCEPT_CFG_CORE_DUMP = 0,
    EXCEPT_CFG_MEM_DUMP,
    EXCEPT_CFG_REBOOT,
} T_EXCEPT_CFG_TYPE;

typedef struct
{
  char *region_name;
  unsigned int *start_address;
  unsigned int *end_address;
  unsigned int is_dumped;
} memory_region_type;

typedef void (*f_exception_callback_t)(char *buf, unsigned int bufsize);

typedef struct
{
  f_exception_callback_t init_cb;
  f_exception_callback_t dump_cb;
} exception_config_type;

void platform_assert(const char *expr, const char *file, int line);

bool exception_register_callbacks(exception_config_type *cb);
void exception_dump_config(int cfg_type, int flag);
void exception_dump_show(unsigned int show_fmt, unsigned int *buf, unsigned int bufsize);
void exception_reboot_config(bool auto_reboot);
void exception_reboot(void);
void exception_get_assert_expr(const char **expr, const char **file, int *line);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __EXCEPTION_HANDLER__
