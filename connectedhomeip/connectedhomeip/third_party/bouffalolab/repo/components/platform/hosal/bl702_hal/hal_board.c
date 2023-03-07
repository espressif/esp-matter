/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>
#include <stdio.h>
#include <bl_efuse.h>
#include <bl_wireless.h>
#include <hal_boot2.h>
#include <hal_sys.h>
#include <libfdt.h>

#include <blog.h>
#include <utils_log.h>

#if defined(CFG_BLE_ENABLE) || defined(CONFIG_BLE_MFG)
#include "ble_lib_api.h"
#endif

#define USER_UNUSED(a) ((void)(a))

#define BL_FDT32_TO_U8(addr, byte_offset)   ((uint8_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U16(addr, byte_offset)  ((uint16_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U32(addr, byte_offset)  ((uint32_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))

static uint32_t factory_addr = 0;


static int update_mac_config_get_mac_from_dtb(const void *fdt, int offset1, uint8_t mac_addr[8])
{
    int lentmp;
    const uint8_t *addr_prop = 0;

    /* set mac_addr */
    addr_prop = fdt_getprop(fdt, offset1, "mac_addr", &lentmp);
    if (8 == lentmp) {

        memcpy(mac_addr, addr_prop, 8);
        //blog_info("mac_addr :\r\n");
        //blog_buf(mac_addr, 8);
    } else {
        blog_error("mac_addr NULL.\r\n");
        return -1;
    }

    return 0;
}

static int update_mac_config_get_mac_from_efuse(uint8_t mac_addr[8])
{
    uint8_t result_or, result_and;

    bl_efuse_read_mac(mac_addr);
    result_or = mac_addr[0] | mac_addr[1] | mac_addr[2] | mac_addr[3] | mac_addr[4] | mac_addr[5] | mac_addr[6] | mac_addr[7];
    result_and = mac_addr[0] & mac_addr[1] & mac_addr[2] & mac_addr[3] & mac_addr[4] & mac_addr[5] & mac_addr[6] & mac_addr[7];

    if (0 == result_or || 1 == result_and) {
        /*all zero or one found in efuse*/
        return -1;
    }
    return 0;
}

static int update_mac_config_get_mac_from_factory(uint8_t mac_addr[8])
{
    uint8_t result_or, result_and;

    if (bl_efuse_read_mac_factory(mac_addr)) {
        return -1;
    }
    result_or = mac_addr[0] | mac_addr[1] | mac_addr[2] | mac_addr[3] | mac_addr[4] | mac_addr[5] | mac_addr[6] | mac_addr[7];
    result_and = mac_addr[0] & mac_addr[1] & mac_addr[2] & mac_addr[3] & mac_addr[4] & mac_addr[5] & mac_addr[6] & mac_addr[7];
    if (0 == result_or || 1 == result_and) {
        /*all zero or one found in efuse*/
        return -1;
    }
    return 0;
}

/*
 * Update MAC address according to order string
 * BFM:
 *  'B' for EFUSE built-in MAC address
 *  'F' for Flash built-in MAC address
 *  'M' for manufacutre configured EFUSE built-in MAC address
 * */
#define MAC_ORDER_ADDR_LEN_MAX      (3)
static void update_mac_config_with_order(const void *fdt, int offset1, const char *order)
{
    int i, set, len;
    uint8_t mac_addr[8];
    static const uint8_t mac_default[] = {0x18, 0xB9, 0x05, 0x88, 0x88, 0x88, 0x88, 0x88};

    set = 0;
    len = strlen(order);
    for (i = 0; i < MAC_ORDER_ADDR_LEN_MAX && i < len; i++) {
        switch (order[i]) {
            case 'B':
            {
                if (0 == update_mac_config_get_mac_from_efuse(mac_addr)) {
                    set = 1;
                    blog_debug("get MAC from B ready\r\n");
                    goto break_scan;
                } else {
                    blog_debug("get MAC from B failed\r\n");
                }
            }
            break;
            case 'F':
            {
                if (0 == update_mac_config_get_mac_from_dtb(fdt, offset1, mac_addr)) {
                    set = 1;
                    blog_debug("get MAC from F ready\r\n");
                    goto break_scan;
                } else {
                    blog_debug("get MAC from F failed\r\n");
                }
            }
            break;
            case 'M':
            {
                if (0 == update_mac_config_get_mac_from_factory(mac_addr)) {
                    set = 1;
                    blog_debug("get MAC from M ready\r\n");
                    goto break_scan;
                } else {
                    blog_debug("get MAC from M failed\r\n");
                }
            }
            break;
            default:
            {
                BL_ASSERT(0);
            }
        }
    }
break_scan:
    if (0 == set) {
        blog_info("Using Default MAC address\r\n");
        memcpy(mac_addr, mac_default, 8);
    }
    //FIXME maybe we should set a different MAC address
    blog_info("Set MAC addrress %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
            mac_addr[0],
            mac_addr[1],
            mac_addr[2],
            mac_addr[3],
            mac_addr[4],
            mac_addr[5],
            mac_addr[6],
            mac_addr[7]
    );
    bl_wireless_mac_addr_set(mac_addr);
}

static void update_mac_config(const void *fdt, int offset1)
{
    int countindex = 0, lentmp = 0;
    const char *result = 0;
    char mac_mode[4];

    countindex = fdt_stringlist_count(fdt, offset1, "mode");
    if (1 == countindex) {
        result = fdt_stringlist_get(fdt, offset1, "mode", 0, &lentmp);
        blog_print("MAC address mode length %d\r\n", lentmp);
        if (lentmp <= MAC_ORDER_ADDR_LEN_MAX) {
            memcpy(mac_mode, result, lentmp);
            mac_mode[3] = '\0';
            blog_print("MAC address mode is %s\r\n", mac_mode);
            update_mac_config_with_order(fdt, offset1, mac_mode);
        }
    }
}


static int update_xtal_config_get_mac_from_factory(uint32_t capcode[5])
{
    uint8_t capcode_efuse = 0;

    if (bl_efuse_read_capcode(&capcode_efuse)) {
        return -1;
    }
    /*efuse only have one capcode entry, so we fill the left with hardcode*/
    capcode[0] = capcode_efuse;
    capcode[1] = capcode_efuse;
    capcode[2] = 1;
    capcode[3] = 60;
    capcode[4] = 60;

    return 0;
}

static int update_xtal_config_get_mac_from_dtb(const void *fdt, int offset1, uint32_t capcode[5])
{
    const uint8_t *addr_prop = 0;
    int lentmp = 0;

    addr_prop = fdt_getprop(fdt, offset1, "xtal", &lentmp);

    if (5*4 == lentmp) {
        blog_info(
            "xtal dtb in DEC :%u %u %u %u %u\r\n",
            BL_FDT32_TO_U8(addr_prop, 4*0),
            BL_FDT32_TO_U8(addr_prop, 4*1),
            BL_FDT32_TO_U8(addr_prop, 4*2),
            BL_FDT32_TO_U8(addr_prop, 4*3),
            BL_FDT32_TO_U8(addr_prop, 4*4)
        );
        capcode[0] = BL_FDT32_TO_U8(addr_prop, 4*0);
        capcode[1] = BL_FDT32_TO_U8(addr_prop, 4*1);
        capcode[2] = BL_FDT32_TO_U8(addr_prop, 4*2);
        capcode[3] = BL_FDT32_TO_U8(addr_prop, 4*3);
        capcode[4] = BL_FDT32_TO_U8(addr_prop, 4*4);
    } else {
        blog_error("xtal dtb NULL.");
        return -1;
    }
    return 0;
}

#define XTAL_ORDER_ADDR_LEN_MAX      (2)
static void update_xtal_config_with_order(const void *fdt, int offset1, const char *order)
{
    int i, set, len;
    uint32_t capcode[5];

    set = 0;
    len = strlen(order);
    for (i = 0; i < XTAL_ORDER_ADDR_LEN_MAX && i < len; i++) {
        switch (order[i]) {
            case 'F':
            {
                if (0 == update_xtal_config_get_mac_from_dtb(fdt, offset1, capcode)) {
                    set = 1;
                    blog_debug("get xtal from F ready\r\n");
                    goto break_scan;
                } else {
                    blog_debug("get xtal from F failed\r\n");
                }
            }
            break;
            case 'M':
            {
                if (0 == update_xtal_config_get_mac_from_factory(capcode)) {
                    set = 1;
                    blog_debug("get xtal from M ready\r\n");
                    goto break_scan;
                } else {
                    blog_debug("get xtal from M failed\r\n");
                }
            }
            break;
            default:
            {
                BL_ASSERT(0);
            }
        }
    }
break_scan:
    if (0 == set) {
        blog_info("Using Default xtal\r\n");
        capcode[0] = 50;
        capcode[1] = 50;
        capcode[2] = 1;
        capcode[3] = 60;
        capcode[4] = 60;
    }
    hal_sys_capcode_update(capcode[0], capcode[1]);
}

static void update_xtal_config(const void *fdt, int offset1)
{
    int lentmp = 0, countindex;
    char xtal_mode[3];
    const char *result = 0;

    countindex = fdt_stringlist_count(fdt, offset1, "xtal_mode");
    if (1 == countindex) {
        result = fdt_stringlist_get(fdt, offset1, "xtal_mode", 0, &lentmp);
        blog_info("xtal_mode length %d\r\n", lentmp);
        if (lentmp <= XTAL_ORDER_ADDR_LEN_MAX) {
            memcpy(xtal_mode, result, lentmp);
            xtal_mode[sizeof(xtal_mode) - 1] = '\0';
            blog_info("xtal_mode is %s\r\n", xtal_mode);
            update_xtal_config_with_order(fdt, offset1, xtal_mode);
        }
    }
}


static int update_poweroffset_config_get_mac_from_dtb(const void *fdt, int offset1, int8_t poweroffset_zigbee[16], int8_t poweroffset_ble[40])
{
    int lentmp = 0, i;
    const uint8_t *addr_prop = 0;

#define PWR_OFFSET_ZIGBEE_NUM (16)
#define PWR_OFFSET_BLE_NUM (40)
#define PWR_OFFSET_BASE (10)
    addr_prop = fdt_getprop(fdt, offset1, "pwr_offset_zigbee", &lentmp);
    if (PWR_OFFSET_ZIGBEE_NUM*4 == lentmp) {
        for (i = 0; i < PWR_OFFSET_ZIGBEE_NUM; i++) {
            poweroffset_zigbee[i] = BL_FDT32_TO_U32(addr_prop, 4*i);
        }
        blog_info("pwr_offset_zigbee from dtb:\r\n");
        blog_buf(poweroffset_zigbee, PWR_OFFSET_ZIGBEE_NUM);
        for (i = 0; i < PWR_OFFSET_ZIGBEE_NUM; i++) {
            poweroffset_zigbee[i] -= PWR_OFFSET_BASE;
            //poweroffset_zigbee[i] *= 4;
        }
        blog_info("pwr_offset_zigbee from dtb (rebase on %d):\r\n", PWR_OFFSET_BASE);
        //TODO FIXME log buffer
        log_buf_int8(poweroffset_zigbee, PWR_OFFSET_ZIGBEE_NUM);
    }  else {
        blog_error("pwr_offset_zigbee NULL. lentmp = %d\r\n", lentmp);
        return -1;
    }
    addr_prop = fdt_getprop(fdt, offset1, "pwr_offset_ble", &lentmp);
    if (PWR_OFFSET_BLE_NUM*4 == lentmp) {
        for (i = 0; i < PWR_OFFSET_BLE_NUM; i++) {
            poweroffset_ble[i] = BL_FDT32_TO_U32(addr_prop, 4*i);
        }
        blog_info("pwr_offset_ble from dtb:\r\n");
        blog_buf(poweroffset_ble, PWR_OFFSET_BLE_NUM);
        for (i = 0; i < PWR_OFFSET_BLE_NUM; i++) {
            poweroffset_ble[i] -= PWR_OFFSET_BASE;
            //poweroffset_ble[i] *= 4;
        }
        blog_info("pwr_offset_ble from dtb (rebase on %d):\r\n", PWR_OFFSET_BASE);
        //TODO FIXME log buffer
        log_buf_int8(poweroffset_ble, PWR_OFFSET_BLE_NUM);
    }  else {
        blog_error("pwr_offset_ble NULL. lentmp = %d\r\n", lentmp);
        return -1;
    }
    return 0;
}

#define PWR_OFFSET_ORDER_ADDR_LEN_MAX      (2)
static void update_poweroffset_config_with_order(const void *fdt, int offset1, const char *order)
{
    int i, set, len, j;
    int8_t poweroffset_zigbee[16], poweroffset_zigbee_tmp[16];
    int8_t poweroffset_ble[40], poweroffset_ble_tmp[40];

    memset(poweroffset_zigbee, 0, sizeof(poweroffset_zigbee));
    memset(poweroffset_ble, 0, sizeof(poweroffset_ble));
    set = 0;
    len = strlen(order);
    for (i = 0; i < PWR_OFFSET_ORDER_ADDR_LEN_MAX && i < len; i++) {
        switch (order[i]) {
            case 'B':
            case 'b':
            {
                if (0 == bl_efuse_read_pwroft_ex(poweroffset_zigbee_tmp, poweroffset_ble_tmp)) {
                    set = 1;
                    blog_info("get pwr offset from B(b) ready\r\n");
                    log_buf_int8(poweroffset_zigbee_tmp, sizeof(poweroffset_zigbee_tmp));
                    log_buf_int8(poweroffset_ble_tmp, sizeof(poweroffset_ble_tmp));
                    if ('B' == order[i]) {
                        /*non-incremental mode*/
                        for (j = 0; j < sizeof(poweroffset_zigbee); j++) {
                            poweroffset_zigbee[j] = poweroffset_zigbee_tmp[j];
                        }
                        for (j = 0; j < sizeof(poweroffset_ble); j++) {
                            poweroffset_ble[j] = poweroffset_ble_tmp[j];
                        }
                        blog_debug("Use pwr offset from B only\r\n");
                        goto break_scan;
                    } else {
                        /*incremental mode*/
                        blog_debug("Use pwr offset from b in incremental mode\r\n");
                        for (j = 0; j < sizeof(poweroffset_zigbee); j++) {
                            poweroffset_zigbee[j] += poweroffset_zigbee_tmp[j];
                        }
                        for (j = 0; j < sizeof(poweroffset_ble); j++) {
                            poweroffset_ble[j] += poweroffset_ble_tmp[j];
                        }
                    }
                } else {
                    blog_debug("get pwr offset from B(b) failed\r\n");
                }
            }
            break;
            case 'F':
            case 'f':
            {
                if (0 == update_poweroffset_config_get_mac_from_dtb(fdt, offset1, poweroffset_zigbee_tmp, poweroffset_ble_tmp)) {
                    set = 1;
                    blog_info("get pwr offset from F(f) ready\r\n");
                    if ('F' == order[i]) {
                        /*non-incremental mode*/
                        for (j = 0; j < sizeof(poweroffset_zigbee); j++) {
                            poweroffset_zigbee[j] = poweroffset_zigbee_tmp[j];
                        }
                        for (j = 0; j < sizeof(poweroffset_ble); j++) {
                            poweroffset_ble[j] = poweroffset_ble_tmp[j];
                        }
                        blog_debug("Use pwr offset from F only\r\n");
                        goto break_scan;
                    } else {
                        /*incremental mode*/
                        blog_debug("Use pwr offset from f in incremental mode\r\n");
                        for (j = 0; j < sizeof(poweroffset_zigbee); j++) {
                            poweroffset_zigbee[j] += poweroffset_zigbee_tmp[j];
                        }
                        for (j = 0; j < sizeof(poweroffset_ble); j++) {
                            poweroffset_ble[j] += poweroffset_ble_tmp[j];
                        }
                    }
                } else {
                    blog_debug("get pwr offset from F(f) failed\r\n");
                }
            }
            break;
            default:
            {
                BL_ASSERT(0);
            }
        }
    }
break_scan:
    if (0 == set) {
        blog_info("Using Default pwr offset\r\n");//all zeros actually
    }
    log_buf_int8(poweroffset_zigbee, sizeof(poweroffset_zigbee));
    log_buf_int8(poweroffset_ble, sizeof(poweroffset_ble));
    bl_wireless_power_offset_set(poweroffset_zigbee, poweroffset_ble);
}

static void update_poweroffset_config(const void *fdt, int offset1)
{
    int lentmp = 0, countindex;
    char pwr_mode[3];
    const char *result = 0;

    countindex = fdt_stringlist_count(fdt, offset1, "pwr_mode");
    if (1 == countindex) {
        result = fdt_stringlist_get(fdt, offset1, "pwr_mode", 0, &lentmp);
        blog_info("pwr_mode length %d\r\n", lentmp);
        if (lentmp <= PWR_OFFSET_ORDER_ADDR_LEN_MAX) {
            memcpy(pwr_mode, result, lentmp);
            pwr_mode[sizeof(pwr_mode) - 1] = '\0';
            blog_info("pwr_mode is %s\r\n", pwr_mode);
            update_poweroffset_config_with_order(fdt, offset1, pwr_mode);
        }
    }
}


static void update_rf_temp_config(const void *fdt, int offset1)
{
    const uint8_t *addr_prop = 0;
    int lentmp = 0;
    int en_tcal;
    int en_tsen_trim;
    int16_t tsen_refcode;

    addr_prop = fdt_getprop(fdt, offset1, "en_tcal", &lentmp);
    if (addr_prop) {
        en_tcal = BL_FDT32_TO_U32(addr_prop, 0);
        en_tsen_trim = !bl_efuse_read_tsen_refcode(&tsen_refcode);

        if (en_tcal && en_tsen_trim) {
            bl_wireless_tcal_en_set(1);
            blog_info("en_tcal = %d, en_tsen_trim = %d, tcal enabled\r\n", en_tcal, en_tsen_trim);
        } else {
            bl_wireless_tcal_en_set(0);
            blog_info("en_tcal = %d, en_tsen_trim = %d, tcal disabled\r\n", en_tcal, en_tsen_trim);
        }
    } else {
        blog_info("en_tcal NULL.\r\n");
    }
}


static int hal_board_load_fdt_info(const void *dtb)
{
    const void *fdt = (const void *)dtb;/* const */

    int wireless_offset = 0;    /* subnode wireless */
    int offset1 = 0, offset2 = 0;        /* subnode offset1 */
    const uint8_t *addr_prop = 0;
    int lentmp = 0;

    wireless_offset = fdt_subnode_offset(fdt, 0, "wireless");
    if (!(wireless_offset > 0)) {
       blog_error("wireless NULL.\r\n");
    }

    offset1 = fdt_subnode_offset(fdt, wireless_offset, "mac");
    if (offset1 > 0) {
        update_mac_config(fdt, offset1);
    }

    offset1 = fdt_subnode_offset(fdt, wireless_offset, "brd_rf");
    if (offset1 > 0) {
        /* set xtal */
        update_xtal_config(fdt, offset1);

        /* set power offset */
        update_poweroffset_config(fdt, offset1);
    }

    offset2 = fdt_subnode_offset(fdt, wireless_offset, "brd_rf");
    if (offset2 > 0) {
        int pwr = 0;
        USER_UNUSED(pwr);
        addr_prop = fdt_getprop(fdt, offset2, "pwr", &lentmp);
        if (addr_prop) {
            pwr = (int8_t)BL_FDT32_TO_U32(addr_prop, 0);
        } else {
            pwr = 0;
        }
        blog_info("set pwr = %ld in dts\r\n", pwr);
#if defined(CFG_BLE_ENABLE) || defined(CONFIG_BLE_MFG)
        ble_controller_set_tx_pwr(pwr);
#else
        extern bool bz_phy_set_tx_power(int power_dbm);
        bz_phy_set_tx_power(pwr);
#endif
    }

    offset1 = fdt_subnode_offset(fdt, wireless_offset, "rf_temp");
    if (offset1 > 0) {
        update_rf_temp_config(fdt, offset1);
    }

    return 0;
}


uint32_t hal_board_get_factory_addr(void)
{
    return factory_addr;
}

int hal_board_cfg(uint8_t board_code)
{
    int ret;
    uint32_t size;

#if defined(CFG_ZIGBEE_ENABLE)
    uint32_t media_addr = 0;
    extern uint8_t __LD_CONFIG_FLASH_SIZE;

    ret = hal_boot2_partition_addr_active("media", &media_addr, &size);
    blog_info("[MAIN] [BOARD] [FLASH] media addr from partition is %08x, ret is %d\r\n", (unsigned int)media_addr, ret);
    if ((uint32_t)&__LD_CONFIG_FLASH_SIZE != media_addr + 20*1024) {
        blog_error("[MAIN] [BOARD] [FLASH] Dead loop. Reason: Mismatch between CONFIG_FLASH_SIZE and partition found\r\n");
        while (1) {
        }
    }
#endif

    USER_UNUSED(ret);
    ret = hal_boot2_partition_addr_active("factory", &factory_addr, &size);
    blog_info("[MAIN] [BOARD] [FLASH] addr from partition is %08x, ret is %d\r\n", (unsigned int)factory_addr, ret);
    if (0 == factory_addr) {
        blog_error("[MAIN] [BOARD] [FLASH] Dead loop. Reason: NO valid Param Parition found\r\n");
        while (1) {
        }
    }

    ret = hal_boot2_partition_bus_addr_active("factory", &factory_addr, &size);
    blog_info("[MAIN] [BOARD] [XIP] addr from partition is %08x, ret is %d\r\n", (unsigned int)factory_addr, ret);
    if (0 == factory_addr) {
        blog_error("[MAIN] [BOARD] [XIP] Dead loop. Reason: NO valid Param Parition found\r\n");
        while (1) {
        }
    }

    hal_board_load_fdt_info((const void *)factory_addr);

    return 0;
}

int hal_board_reload_power_offset(void)
{
    const void *fdt = (const void *)factory_addr;
    int wireless_offset = 0;
    int offset1 = 0;

    wireless_offset = fdt_subnode_offset(fdt, 0, "wireless");
    if (!(wireless_offset > 0)) {
       blog_error("wireless NULL.\r\n");
    }

    offset1 = fdt_subnode_offset(fdt, wireless_offset, "brd_rf");
    if (offset1 > 0) {
        /* set power offset */
        update_poweroffset_config(fdt, offset1);
    }

    return 0;
}

int hal_board_get_dts_addr(const char *name, uint32_t *start, uint32_t *off)
{
    const void *fdt = (const void *)factory_addr;
    uint32_t offset;

    if (!name || !start || !off) {
        return -1;
    }

    offset = fdt_subnode_offset(fdt, 0, name);
    if (offset <= 0) {
       log_error("%s NULL.\r\n", name);
       return -1;
    }

    *start = (uint32_t)fdt;
    *off = offset;

    return 0;
}
