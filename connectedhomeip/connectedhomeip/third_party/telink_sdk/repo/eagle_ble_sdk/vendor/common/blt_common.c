/********************************************************************************************************
 * @file	blt_common.c
 *
 * @brief	This is the source file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include <stdint.h>

#include "blt_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "tl_common.h"

u32 flash_sector_mac_address __attribute__((section(".retention_data"))) = CFG_ADR_MAC_1M_FLASH;         // default flash is 1M
u32 flash_sector_calibration __attribute__((section(".retention_data"))) = CFG_ADR_CALIBRATION_1M_FLASH; // default flash is 1M

/**
 * @brief		This function can automatically recognize the flash size,
 * 				and the system selects different customized sector according
 * 				to different sizes.
 * @param[in]	none
 * @return      none
 */
void blc_readFlashSize_autoConfigCustomFlashSector(void)
{
    unsigned int temp;
    temp         = flash_read_mid();
    u8 flash_cap = ((u8 *) &temp)[2];

    if (flash_cap == FLASH_SIZE_512K)
    {
        flash_sector_mac_address = CFG_ADR_MAC_512K_FLASH;
        flash_sector_calibration = CFG_ADR_CALIBRATION_512K_FLASH;
    }
    else if (flash_cap == FLASH_SIZE_1M)
    {
        flash_sector_mac_address = CFG_ADR_MAC_1M_FLASH;
        flash_sector_calibration = CFG_ADR_CALIBRATION_1M_FLASH;
    }
    else if (flash_cap == FLASH_SIZE_2M)
    {
        flash_sector_mac_address = CFG_ADR_MAC_2M_FLASH;
        flash_sector_calibration = CFG_ADR_CALIBRATION_2M_FLASH;
    }
    else
    {
        // This SDK do not support flash size other than 1M/2M
        // If code stop here, please check your Flash
        while (1)
            ;
    }

    flash_set_capacity(flash_cap);
}




/*
 *Kite: 	VVWWXX38C1A4YYZZ
 *Vulture:  VVWWXXD119C4YYZZ
 *Eagle:  	VVWWXX
 * public_mac:
 * 				Kite 	: VVWWXX 38C1A4
 * 				Vulture : VVWWXX D119C4
 * 				Eagle	: VVWWXX
 * random_static_mac: VVWWXXYYZZ C0
 */
/**
 * @brief		This function is used to initialize the MAC address
 * @param[in]	flash_addr - flash address for MAC address
 * @param[in]	mac_public - public address
 * @param[in]	mac_random_static - random static MAC address
 * @return      none
 */
void blc_initMacAddress(int flash_addr, u8 * mac_public, u8 * mac_random_static)
{
    if (flash_sector_mac_address == 0)
    {
        return;
    }

    u8 mac_read[8];
    flash_read_page(flash_addr, 8, mac_read);

    u8 value_rand[5];
    generateRandomNum(5, value_rand);

    u8 ff_six_byte[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    if (memcmp(mac_read, ff_six_byte, 6))
    {
        memcpy(mac_public, mac_read, 6); // copy public address from flash
    }
    else
    { // no public address on flash
        mac_public[0] = value_rand[0];
        mac_public[1] = value_rand[1];
        mac_public[2] = value_rand[2];

        // TODO
        // company id:
        mac_public[3] = 0xD1; // company id: 0xC119D1
        mac_public[4] = 0x19;
        mac_public[5] = 0xC4;

        flash_write_page(flash_addr, 6, mac_public);
    }

    mac_random_static[0] = mac_public[0];
    mac_random_static[1] = mac_public[1];
    mac_random_static[2] = mac_public[2];
    mac_random_static[5] = 0xC0; // for random static

    u16 high_2_byte = (mac_read[6] | mac_read[7] << 8);
    if (high_2_byte != 0xFFFF)
    {
        memcpy((u8 *) (mac_random_static + 3), (u8 *) (mac_read + 6), 2);
    }
    else
    {
        mac_random_static[3] = value_rand[3];
        mac_random_static[4] = value_rand[4];

        flash_write_page(flash_addr + 6, 2, (u8 *) (mac_random_static + 3));
    }
}

