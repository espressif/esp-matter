/********************************************************************************************************
 * @file	flash_fw_check.c
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
#include "flash_fw_check.h"
#include "../../drivers/B91/flash.h"

extern _attribute_data_retention_   int		ota_program_offset;
extern _attribute_data_retention_	int		ota_program_bootAddr;
extern unsigned long crc32_half_cal(unsigned long crc, unsigned char* input, unsigned long* table, int len);

static const unsigned long fw_crc32_half_tbl[16] = {
	0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
	0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
	0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
	0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

#define FW_READ_SIZE      256 ///16 ///256 require more stack space

u32 fw_crc_init = 0xFFFFFFFF;

/***********************************
 * this function must be called after the function sys_init.
 * sys_init will set the ota_program_offset value.
 */
/**
 * @brief		This function is used to check the firmware is ok or not
 * @param[in]	crc_init_value - the initial value of CRC
 * @return		0 - CRC is check success
 * 				1 - CRC is check fail
 */
bool flash_fw_check( u32 crc_init_value ){

	if(!crc_init_value){
		fw_crc_init = 0xFFFFFFFF;
	}else{
		fw_crc_init = crc_init_value;
	}

	/////find the real FW flash address
	u32 fw_flashAddr;
	if(!ota_program_offset){ ////zero, firmware is stored at flash 0x20000.
		fw_flashAddr = ota_program_bootAddr; ///NOTE: this flash offset need to set according to OTA offset
	}else{                   ////note zero, firmware is stored at flash 0x00000.
		fw_flashAddr = 0x00000;
	}

	u32 fw_size;
	flash_read_page( (fw_flashAddr+0x18), 4, (u8*)&fw_size); ///0x18 store bin size value

	u16 fw_Block;
	u16 fw_remainSizeByte;
	fw_Block = fw_size/FW_READ_SIZE;
	fw_remainSizeByte = fw_size%FW_READ_SIZE;


	int i = 0;
	u8 fw_tmpdata[FW_READ_SIZE]; ///
	u8 ota_dat[FW_READ_SIZE<<1];
	for(i=0; i < fw_Block; i++){ ///Telink bin must align 16 bytes.

		flash_read_page( (fw_flashAddr+i*FW_READ_SIZE), FW_READ_SIZE, fw_tmpdata);

		//FW_READ_SIZE byte OTA data32  half byteCRC
		for(int i=0;i<FW_READ_SIZE;i++){
			ota_dat[i*2] = fw_tmpdata[i]&0x0f;
			ota_dat[i*2+1] = fw_tmpdata[i]>>4;
		}
		fw_crc_init = crc32_half_cal(fw_crc_init, ota_dat, (unsigned long* )fw_crc32_half_tbl, (FW_READ_SIZE<<1));
	}

	//////////////////////////////
	if(fw_remainSizeByte != 4){
		flash_read_page( (fw_flashAddr+fw_size -fw_remainSizeByte), (fw_remainSizeByte-4), fw_tmpdata);
		for(int i=0;i<(fw_remainSizeByte-4);i++){
			ota_dat[i*2] = fw_tmpdata[i]&0x0f;
			ota_dat[i*2+1] = fw_tmpdata[i]>>4;
		}
		fw_crc_init = crc32_half_cal(fw_crc_init, ota_dat, (unsigned long* )fw_crc32_half_tbl, ((fw_remainSizeByte-4)<<1));
	}

	////////read crc value and compare
	u32 fw_check_value;
	flash_read_page( (fw_flashAddr+fw_size-4), 4, (u8*)&fw_check_value);

	if(fw_check_value != fw_crc_init){
		return 1;  ///CRC check fail
	}

	return 0; ///CRC check ok
}





void blt_firmware_completeness_check(void)
{
	//user can use flash_fw_check() to check whether firmware in flash is modified.
	//Advice user to do it only when power on.
	if(flash_fw_check(0xffffffff)){ //if retrun 0, flash fw crc check ok. if retrun 1, flash fw crc check fail
		while(1);				    //Users can process according to the actual application.
	}
}


#endif



















