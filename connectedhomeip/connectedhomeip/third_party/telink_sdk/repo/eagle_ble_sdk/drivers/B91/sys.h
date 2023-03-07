/********************************************************************************************************
 * @file	sys.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
/**	@page SYS
 *
 *	Introduction
 *	===============
 *	Clock init and system timer delay.
 *
 *	API Reference
 *	===============
 *	Header File: sys.h
 */

#ifndef SYS_H_
#define SYS_H_
#include "bit.h"
#include "reg_include/stimer_reg.h"


/**********************************************************************************************************************
 *                                         global constants                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                           global macro                                                             *
 *********************************************************************************************************************/
/*
 * brief instruction delay
 */

#define	_ASM_NOP_					__asm__("nop")

#define	CLOCK_DLY_1_CYC				_ASM_NOP_
#define	CLOCK_DLY_2_CYC				_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_3_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_4_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_5_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_6_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_7_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_8_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_9_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_10_CYC			_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_

#define FLASH_R_BASE_ADDR   		0x20000000
#define REG_RW_BASE_ADDR  			0x80000000
#define REG_ADDR8(a)				(*(volatile unsigned char*)(REG_RW_BASE_ADDR | (a)))
#define REG_ADDR16(a)				(*(volatile unsigned short*)(REG_RW_BASE_ADDR | (a)))
#define REG_ADDR32(a)				(*(volatile unsigned long*)(REG_RW_BASE_ADDR | (a)))

#define write_reg8(addr,v)			(*(volatile unsigned char*)(REG_RW_BASE_ADDR | (addr)) = (unsigned char)(v))
#define write_reg16(addr,v)			(*(volatile unsigned short*)(REG_RW_BASE_ADDR | (addr)) = (unsigned short)(v))
#define write_reg32(addr,v)			(*(volatile unsigned long*)(REG_RW_BASE_ADDR | (addr)) = (unsigned long)(v))

#define read_reg8(addr)				(*(volatile unsigned char*)(REG_RW_BASE_ADDR | (addr)))
#define read_reg16(addr)            (*(volatile unsigned short*)(REG_RW_BASE_ADDR | (addr)))
#define read_reg32(addr)            (*(volatile unsigned long*)(REG_RW_BASE_ADDR | (addr)))

#define write_sram8(addr,v)			(*(volatile unsigned char*)( (addr)) = (unsigned char)(v))
#define write_sram16(addr,v)		(*(volatile unsigned short*)( (addr)) = (unsigned short)(v))
#define write_sram32(addr,v)		(*(volatile unsigned long*)( (addr)) = (unsigned long)(v))

#define read_sram8(addr)			(*(volatile unsigned char*)((addr)))
#define read_sram16(addr)           (*(volatile unsigned short*)((addr)))
#define read_sram32(addr)           (*(volatile unsigned long*)((addr)))
#define TCMD_UNDER_BOTH				0xc0
#define TCMD_UNDER_RD				0x80
#define TCMD_UNDER_WR				0x40

#define TCMD_MASK					0x3f

#define TCMD_WRITE					0x3
#define TCMD_WAIT					0x7
#define TCMD_WAREG					0x8
//#if 1 //optimize
/*
 * IRAM area:0x00000~0x1FFFF BIT(19) is 0,BIT(16~0) 128K is address offset
 * DRAM area:0x80000~0x9FFFF BIT(19) is 1,BIT(16~0) 128K is address offset
 * ILM area:0xc0000000~0xc0020000 BIT(31~19) is 3,BIT(21) is 0, BIT(20~17) do not care  BIT(16~0) 128K is address offset 128K is address offset
 * DLM area:0xc0200000~0xc0220000 BIT(31~19) is 3,BIT(21) is 1, BIT(20~17) do not care  BIT(16~0) 128K is address offset 128K is address offset
 * BIT(19) is used to distinguish from IRAM to DRAM, BIT(21) is used to distinguish from ILM to DLM.
 * so we can write it as follow
 * #define  convert_ram_addr_cpu2bus  (((((addr))&0x80000)? ((addr)| 0xc0200000) : ((addr)|0xc0000000)))
 * BIT(20~17) are invalid address line ,IRAM address is less than 0x80000, (address-0x80000)must borrow from BIT(21)
 *   #define convert(addr) ((addr)-0x80000+0xc0200000)
 *  to simplify
 *  #define convert(addr) ((addr)+0xc0180000)
 * */
#define convert_ram_addr_cpu2bus(addr)  ((unsigned int)(addr)+0xc0180000)
//#else  //no optimize
//#define  convert_ram_addr_cpu2bus  (((((unsigned int)(addr)) >=0x80000)?(((unsigned int)(addr))-0x80000+0xc0200000) : (((unsigned int)(addr)) + 0xc0000000)))
//#endif

#define convert_ram_addr_bus2cpu(addr)  (((((unsigned int)(addr)) >=0xc0200000)?(((unsigned int)(addr)) + 0x80000-0xc0200000) : (((unsigned int)(addr)) - 0xc0000000)))

/**********************************************************************************************************************
 *                                         global data type                                                           *
 *********************************************************************************************************************/

/**
 * @brief 	Power type for different application
 */
typedef enum{
	LDO_1P4_LDO_1P8 	= 0x00,	/**< 1.4V-LDO & 1.8V-LDO mode */
	DCDC_1P4_LDO_1P8	= 0x01,	/**< 1.4V-DCDC & 1.8V-LDO mode */
	DCDC_1P4_DCDC_1P8	= 0x03,	/**< 1.4V-DCDC & 1.8V-DCDC mode */
}power_mode_e;

/**
 * @brief 	The maximum voltage that the chip can withstand is 3.6V.
 * 			When the vbat power supply voltage is lower than 3.6V, it is configured as VBAT_MAX_VALUE_LESS_THAN_3V6 mode,
 * 			bypass is turned on, and the vbat voltage directly supplies power to the chip.
 * 			When the vbat power supply voltage may be higher than 3.6V, it is configured as VBAT_MAX_VALUE_GREATER_THAN_3V6 mode,
 * 			the bypass is closed, and the vbat voltage passes through an LDO to supply power to the chip.
 */
typedef enum{
	VBAT_MAX_VALUE_GREATER_THAN_3V6	= 0x00,	/*VBAT may be greater than 3.6V. */
	VBAT_MAX_VALUE_LESS_THAN_3V6	= BIT(3),	/*VBAT must be below 3.6V. */
}vbat_type_e;

/**
 * @brief command table for special registers
 */
typedef struct tbl_cmd_set_t {
	unsigned int  	adr;
	unsigned char	dat;
	unsigned char	cmd;
} tbl_cmd_set_t;


/**********************************************************************************************************************
 *                                     global variable declaration                                                    *
 *********************************************************************************************************************/

extern unsigned int g_chip_version;

/**********************************************************************************************************************
 *                                      global function prototype                                                     *
 *********************************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      This function reboot mcu.
 * @return     none
 */
static inline void sys_reboot(void)
{
	write_reg8(0x1401ef, 0x20);
}
/**
 * @brief   	This function serves to initialize system.
 * @param[in]	power_mode - power mode(LDO/DCDC/LDO_DCDC)
 * @param[in]	vbat_v		- vbat voltage type: 0 vbat may be greater than 3.6V,  1 vbat must be below 3.6V.
 * @return  	none
 */
void sys_init(power_mode_e power_mode, vbat_type_e vbat_v);

/**
 * @brief      This function performs a series of operations of writing digital or analog registers
 *             according to a command table
 * @param[in]  pt    - pointer to a command table containing several writing commands
 * @param[in]  size  - number of commands in the table
 * @return     number of commands are carried out
 */

int write_reg_table(const tbl_cmd_set_t * pt, int size);

#ifdef __cplusplus
}
#endif

#endif
