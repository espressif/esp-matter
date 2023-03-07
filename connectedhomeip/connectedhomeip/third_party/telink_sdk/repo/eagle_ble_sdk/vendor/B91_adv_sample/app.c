/********************************************************************************************************
 * @file	app.c
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
#include <assert.h>

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "app_config.h"
#include "app.h"

/**
 * @brief		user initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */
_attribute_no_inline_ void user_init_normal(void)
{
    u8 status = BLE_SUCCESS;
    u8 mac_public[6];
    u8 mac_random_static[6];

    static const u8	tbl_advData[] = {
        0x05, 0x09, 'e', 'H', 'I', 'D',
        0x02, 0x01, 0x05,                        // BLE limited discoverable mode and BR/EDR not supported
        0x03, 0x19, 0x80, 0x01,                  // 384, Generic Remote Control, Generic category
        0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,      // incomplete list of service class UUIDs (0x1812, 0x180F)
    };

    static const u8	tbl_scanRsp [] = {
        0x08, 0x09, 'e', 'S', 'a', 'm', 'p', 'l', 'e',
    };

    /* random number generator must be initiated here( in the beginning of user_init_nromal).
     * When deepSleep retention wakeUp, no need initialize again */
    random_generator_init();  //this is must

    /* for 1M   Flash, flash_sector_mac_address equals to 0xFF000
     * for 2M   Flash, flash_sector_mac_address equals to 0x1FF000*/
    blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);

    blc_ll_initBasicMCU();                       //mandatory
    blc_ll_initStandby_module(mac_public);       //mandatory
    blc_ll_initAdvertising_module();             //adv module: 		 mandatory for BLE slave,
    blc_ll_initSlaveRole_module();               //slave module: 	 mandatory for BLE slave,

    status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_35MS,
                                    ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
                                    0,  NULL,
                                    BLT_ENABLE_ADV_ALL,
                                    ADV_FP_NONE);
    assert(status == BLE_SUCCESS);

    status = bls_ll_setAdvData((u8 *)tbl_advData, sizeof(tbl_advData));
    assert(status == BLE_SUCCESS);

    status = bls_ll_setScanRspData((u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));
    assert(status == BLE_SUCCESS);

    status = bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //adv enable
    assert(status == BLE_SUCCESS);
}

/**
 * @brief		This is main_loop function
 * @param[in]	none
 * @return      none
 */
_attribute_no_inline_ void main_loop (void)
{
    blt_sdk_main_loop();
}
