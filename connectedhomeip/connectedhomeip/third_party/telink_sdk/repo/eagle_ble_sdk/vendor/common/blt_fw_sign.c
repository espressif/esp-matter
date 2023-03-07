/********************************************************************************************************
 * @file	blt_fw_sign.c
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
#if 1
#include "tl_common.h"
#include "drivers.h"
#include "blt_fw_sign.h"
#include "stack/ble/ble_config.h"
#include "blt_common.h"
#include "proj_lib/firmware_encrypt.h"

/**
 * @brief		This function is used to check digital signature of firmware
 * @param[in]	none
 * @return      none
 */
void blt_firmware_signature_check(void)
{
		unsigned int flash_mid;
		unsigned char flash_uid[16];
		unsigned char signature_enc_key[16];
		unsigned char signature_flash_key[16];
		extern int flash_read_mid_uid_with_check( unsigned int *flash_mid ,unsigned char *flash_uid);
		int flag = flash_read_mid_uid_with_check(&flash_mid, flash_uid);

		if(flag==0){  //reading flash UID error
			while(1);
		}

		firmware_encrypt_based_on_uid (flash_uid, signature_enc_key);

		flash_read_page(flash_sector_calibration + CALIB_OFFSET_FIRMWARE_SIGNKEY, 16, signature_flash_key);
		if(memcmp(signature_enc_key, signature_flash_key, 16)){  //signature not match
			while(1);   //user can change the code here to stop firmware running
		}
}
#endif
