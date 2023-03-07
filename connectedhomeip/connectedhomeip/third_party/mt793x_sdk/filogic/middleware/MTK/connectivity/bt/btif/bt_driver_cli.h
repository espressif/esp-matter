/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __BT_DRIVER_CLI_H_
#define __BT_DRIVER_CLI_H_

#ifdef MTK_MT7933_BT_ENABLE
#include <stdint.h>
#include "cli.h"

#ifdef MTK_BT_MEM_SHRINK
#define MTK_BT_DRV_CLI_LITE
#endif

#if defined(MTK_MINICLI_ENABLE) && defined(MTK_BT_DRV_CLI_ENABLE)
extern cmd_t bt_driver_cli[];
#define BT_DRV_CLI_ENTRY {"btdrv", "bt driver cli cmd", NULL, bt_driver_cli},
#endif

unsigned char bt_driver_cli_set_dbg_lvl(unsigned char len, char *param[]);
unsigned char bt_driver_cli_init(unsigned char len, char *param[]);
unsigned char bt_driver_cli_deinit(unsigned char len, char *param[]);
unsigned char bt_driver_cli_dump_reg(unsigned char len, char *param[]);
unsigned char bt_driver_cli_loopback_set(unsigned char len, char *param[]);
unsigned char bt_driver_cli_send(unsigned char len, char *param[]);
unsigned char bt_driver_cli_inquiry(unsigned char len, char *param[]);
#else
#define BT_DRV_CLI_ENTRY
#endif

#ifdef CHIP_MT8512
void bt_driver_register_cli_cmd(void);
#endif

#endif /*__BT_DRIVER_CLI_H_*/
