/********************************************************************************************************
 * @file	flash.c
 *
 * @brief	This is the source file for B91
 *
 * @author	Driver Group
 * @date	2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "flash.h"
#include "core.h"
#include "mspi.h"
#include "plic.h"
#include "stimer.h"
#include "sys.h"
#include "timer.h"

#define DISABLE_BTB __asm__("csrci 	0x7D0,8")
#define ENABLE_BTB __asm__("csrsi 	0x7D0,8")

volatile unsigned char flash_cnt = 1;

static preempt_config_t s_flash_preempt_config = {
    .preempt_en = 0,
    .threshold  = 1,
};

/**
 * @brief 		This function serves to set priority threshold. when the interrupt priority > Threshold flash process will
 * disturb by interrupt.
 * @param[in]   preempt_en	- 1 can disturb by interrupt, 0 can disturb by interrupt.
 * @param[in]	threshold	- priority Threshold.
 * @return    	none.
 */
void flash_plic_preempt_config(unsigned char preempt_en, unsigned char threshold)
{
    s_flash_preempt_config.preempt_en = preempt_en;
    s_flash_preempt_config.threshold  = threshold;
}

/********************************************************************************************************
 *								Functions for internal use in flash,
 *		There is no need to add an evasion solution to solve the problem of access flash conflicts.
 *******************************************************************************************************/
/**
 * @brief		This function to determine whether the flash is busy..
 * @return		1:Indicates that the flash is busy. 0:Indicates that the flash is free
 */
static inline int flash_is_busy(void)
{
    return mspi_read() & 0x01; //  the busy bit, pls check flash spec
}

/**
 * @brief		This function serves to set flash write command.This function interface is only used internally by flash,
 * 				and is currently included in the H file for compatibility with other SDKs. When using this
 * interface, please ensure that you understand the precautions of flash before using it.
 * @param[in]	cmd	- set command.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ void flash_send_cmd(unsigned char cmd)
{
    mspi_high();
    CLOCK_DLY_10_CYC;
    mspi_low();
    mspi_write(cmd);
    mspi_wait();
}

/**
 * @brief		This function serves to send flash address.
 * @param[in]	addr	- the flash address.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ static void flash_send_addr(unsigned int addr)
{
    mspi_write((unsigned char) (addr >> 16));
    mspi_wait();
    mspi_write((unsigned char) (addr >> 8));
    mspi_wait();
    mspi_write((unsigned char) (addr));
    mspi_wait();
}

/**
 * @brief     This function serves to wait flash done.(make this a asynchorous version).
 * @return    none.
 */
_attribute_ram_code_sec_noinline_ static void flash_wait_done(void)
{
    flash_send_cmd(FLASH_READ_STATUS_CMD);

    int i;
    for (i = 0; i < 10000000; ++i)
    {
        if (!flash_is_busy())
        {
            flash_cnt++;
            break;
        }
    }
    mspi_high();
}


/********************************************************************************************************
 *		It is necessary to add an evasion plan to solve the problem of access flash conflict.
 *******************************************************************************************************/
/**
 * @brief 		This function serves to erase a sector.
 * @param[in]   addr	- the start address of the sector needs to erase.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_erase_sector_ram(unsigned long addr)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);
    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_SECT_ERASE_CMD);
    flash_send_addr(addr);
    mspi_high();
    flash_wait_done();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_erase_sector(unsigned long addr)
{
    DISABLE_BTB;
    flash_erase_sector_ram(addr);
    ENABLE_BTB;
}

/**
 * @brief 		This function writes the buffer's content to a page.
 * @param[in]   addr	- the start address of the page.
 * @param[in]   len		- the length(in byte) of content needs to write into the page.
 * @param[in]   buf		- the start address of the content needs to write into.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_write_page_ram(unsigned long addr, unsigned long len, unsigned char * buf)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);
    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_WRITE_CMD);
    flash_send_addr(addr);

    unsigned int i;
    for (i = 0; i < len; ++i)
    {
        mspi_write(buf[i]); /* write data */
        mspi_wait();
    }
    mspi_high();
    flash_wait_done();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_write_page(unsigned long addr, unsigned long len, unsigned char * buf)
{
    unsigned int ns = PAGE_SIZE - (addr & 0xff);
    int nw          = 0;

    do
    {
        nw = len > ns ? ns : len;
        DISABLE_BTB;
        flash_write_page_ram(addr, nw, buf);
        ENABLE_BTB;
        ns = PAGE_SIZE;
        addr += nw;
        buf += nw;
        len -= nw;
    } while (len > 0);
}

/**
 * @brief 		This function reads the content from a page to the buf.
 * @param[in]   addr	- the start address of the page.
 * @param[in]   len		- the length(in byte) of content needs to read out from the page.
 * @param[out]  buf		- the start address of the buffer.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_read_page_ram(unsigned long addr, unsigned long len, unsigned char * buf)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);
    mspi_stop_xip();
    flash_send_cmd(FLASH_READ_CMD);
    flash_send_addr(addr);

    mspi_write(0x00); /* dummy,  to issue clock */
    mspi_wait();
    mspi_fm_rd_en(); /* auto mode, mspi_get() automatically triggers mspi_write(0x00) once. */
    mspi_wait();
    /* get data */
    for (unsigned int i = 0; i < len; ++i)
    {
        *buf++ = mspi_get();
        mspi_wait();
    }
    mspi_fm_rd_dis(); /* off read auto mode */
    mspi_high();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_read_page(unsigned long addr, unsigned long len, unsigned char * buf)
{
    DISABLE_BTB;
    flash_read_page_ram(addr, len, buf);
    ENABLE_BTB;
}

/**
 * @brief     	This function serves to erase a chip.
 * @return    	none.
 */
_attribute_ram_code_sec_noinline_ void flash_erase_chip_ram(void)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);
    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_CHIP_ERASE_CMD);
    mspi_high();
    flash_wait_done();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_erase_chip(void)
{
    DISABLE_BTB;
    flash_erase_chip_ram();
    ENABLE_BTB;
}

/**
 * @brief     	This function serves to erase a page(256 bytes).
 * @param[in] 	addr	- the start address of the page needs to erase.
 * @return    	none.
 */
_attribute_ram_code_sec_noinline_ void flash_erase_page_ram(unsigned int addr)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_PAGE_ERASE_CMD);
    flash_send_addr(addr);
    mspi_high();
    flash_wait_done();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_erase_page(unsigned int addr)
{
    DISABLE_BTB;
    flash_erase_page_ram(addr);
    ENABLE_BTB;
}

/**
 * @brief 		This function serves to erase a block(32k).
 * @param[in]   addr	- the start address of the block needs to erase.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_erase_32kblock_ram(unsigned int addr)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);
    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_32KBLK_ERASE_CMD);
    flash_send_addr(addr);
    mspi_high();
    flash_wait_done();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_erase_32kblock(unsigned int addr)
{
    DISABLE_BTB;
    flash_erase_32kblock_ram(addr);
    ENABLE_BTB;
}

/**
 * @brief 		This function serves to erase a block(64k).
 * @param[in]   addr	- the start address of the block needs to erase.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_erase_64kblock_ram(unsigned int addr)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);
    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_64KBLK_ERASE_CMD);
    flash_send_addr(addr);
    mspi_high();
    flash_wait_done();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_erase_64kblock(unsigned int addr)
{
    DISABLE_BTB;
    flash_erase_64kblock_ram(addr);
    ENABLE_BTB;
}

/**
 * @brief 		This function write the status of flash.
 * @param[in]  	data	- the value of status.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_write_status_ram(unsigned short data)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);
    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_WRITE_STATUS_CMD);
    mspi_write((unsigned char) data);
    mspi_wait();
    mspi_write((unsigned char) (data >> 8));
    mspi_wait();
    mspi_high();
    flash_wait_done();
    mspi_high();
    CLOCK_DLY_5_CYC;
    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_write_status(unsigned short data)
{
    DISABLE_BTB;
    flash_write_status_ram(data);
    ENABLE_BTB;
}

/**
 * @brief 		This function reads the status of flash.
 * @return 		the value of status.
 */
_attribute_ram_code_sec_noinline_ unsigned short flash_read_status_ram(void)
{
    unsigned short status = 0;
    unsigned int r        = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(FLASH_READ_STATUS_1_CMD); /* get high 8 bit status */
    status = (mspi_read() << 8);
    mspi_high();
    flash_send_cmd(FLASH_READ_STATUS_CMD); /* get low 8 bit status */
    status |= mspi_read();
    mspi_high();
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
    return status;
}
_attribute_text_sec_ unsigned short flash_read_status(void)
{
    DISABLE_BTB;
    unsigned short status = flash_read_status_ram();
    ENABLE_BTB;
    return status;
}

/**
 * @brief		Deep Power Down mode to put the device in the lowest consumption mode
 * 				it can be used as an extra software protection mechanism,while the device
 * 				is not in active use,since in the mode,  all write,Program and Erase commands
 * 				are ignored,except the Release from Deep Power-Down and Read Device ID(RDI)
 * 				command.This release the device from this mode
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_deep_powerdown_ram(void)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(FLASH_POWER_DOWN);
    mspi_high();
    delay_us(1);
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_deep_powerdown(void)
{
    DISABLE_BTB;
    flash_deep_powerdown_ram();
    ENABLE_BTB;
}

/**
 * @brief		The Release from Power-Down or High Performance Mode/Device ID command is a
 * 				Multi-purpose command.it can be used to release the device from the power-Down
 * 				State or High Performance Mode or obtain the devices electronic identification
 * 				(ID)number.Release from Power-Down will take the time duration of tRES1 before
 * 				the device will resume normal operation and other command are accepted.The CS#
 * 				pin must remain high during the tRES1(8us) time duration.
 * @return      none.
 */
_attribute_ram_code_sec_noinline_ void flash_release_deep_powerdown_ram(void)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(FLASH_POWER_DOWN_RELEASE);
    mspi_high();
    flash_wait_done();
    mspi_high();
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_release_deep_powerdown(void)
{
    DISABLE_BTB;
    flash_release_deep_powerdown_ram();
    ENABLE_BTB;
}

/**
 * @brief	  	This function serves to read MID of flash(MAC id). Before reading UID of flash,
 * 				you must read MID of flash. and then you can look up the related table to select
 * 				the idcmd and read UID of flash
 * @return    	MID of the flash(4 bytes).
 */
_attribute_ram_code_sec_noinline_ unsigned int flash_read_mid_ram(void)
{

    unsigned char j        = 0;
    unsigned int flash_mid = 0;
    unsigned int r         = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(FLASH_GET_JEDEC_ID);
    mspi_write(0x00); /* dummy,  to issue clock */
    mspi_wait();
    mspi_fm_rd_en(); /* auto mode, mspi_get() automatically triggers mspi_write(0x00) once. */
    mspi_wait();

    for (j = 0; j < 3; ++j)
    {
        ((unsigned char *) (&flash_mid))[j] = mspi_get();
        mspi_wait();
    }
    mspi_fm_rd_dis(); /* off read auto mode */
    mspi_high();
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
    return flash_mid;
}
_attribute_text_sec_ unsigned int flash_read_mid(void)
{

    unsigned int flash_mid = 0;
    DISABLE_BTB;
    flash_mid = flash_read_mid_ram();
    ENABLE_BTB;
    return flash_mid;
}

/**
 * @brief	  	This function serves to read UID of flash
 * @param[in] 	idcmd	- different flash vendor have different read-uid command. E.g: GD/PUYA:0x4B; XTX: 0x5A
 * @param[in] 	buf		- store UID of flash
 * @return    	none.
 */
_attribute_ram_code_sec_noinline_ void flash_read_uid_ram(unsigned char idcmd, unsigned char * buf)
{
    unsigned char j = 0;
    unsigned int r  = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(idcmd);
    if (idcmd == FLASH_GD_PUYA_READ_UID_CMD) //< GD/puya
    {
        flash_send_addr(0x00);
        mspi_write(0x00); /* dummy,  to issue clock */
        mspi_wait();
    }
    else if (idcmd == FLASH_XTX_READ_UID_CMD) //< XTX
    {
        flash_send_addr(0x80);
        mspi_write(0x00); /* dummy,  to issue clock */
        mspi_wait();
    }
    mspi_write(0x00); /* dummy,  to issue clock */
    mspi_wait();
    mspi_fm_rd_en(); /* auto mode, mspi_get() automatically triggers mspi_write(0x00) once. */
    mspi_wait();

    for (j = 0; j < 16; ++j)
    {
        *buf++ = mspi_get();
        mspi_wait();
    }
    mspi_fm_rd_dis(); /* off read auto mode */
    mspi_high();
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_read_uid(unsigned char idcmd, unsigned char * buf)
{
    DISABLE_BTB;
    flash_read_uid_ram(idcmd, buf);
    ENABLE_BTB;
}


/**
 * @brief 		This function serves to set the protection area of the flash.
 * @param[in]   type	- flash type include Puya.
 * @param[in]   data	- refer to Driver API Doc.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_lock_ram(flash_type_e type, unsigned short data)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_WRITE_STATUS_CMD);
    if (type == FLASH_TYPE_PUYA)
    {
        mspi_write((unsigned char) data);
        mspi_wait();
        mspi_write((unsigned char) (data >> 8)); // 16bit status
    }
    mspi_wait();
    mspi_high();
    flash_wait_done();
    mspi_high();
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_lock(flash_type_e type, unsigned short data)
{
    DISABLE_BTB;
    flash_lock_ram(type, data);
    ENABLE_BTB;
}

/**
 * @brief 		This function serves to flash release protection.
 * @param[in]   type	- flash type include Puya.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_unlock_ram(flash_type_e type)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
    flash_send_cmd(FLASH_WRITE_STATUS_CMD);
    if (type == FLASH_TYPE_PUYA)
    {
        mspi_write(0);
        mspi_wait();
        mspi_write(0); // 16bit status
    }
    mspi_wait();
    mspi_high();
    flash_wait_done();
    mspi_high();
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_unlock(flash_type_e type)
{
    DISABLE_BTB;
    flash_unlock_ram(type);
    ENABLE_BTB;
}
/**
 * @brief 		This function is used to update the configuration parameters of xip(eXecute In Place),
 * 				this configuration will affect the speed of MCU fetching,
 * 				this parameter needs to be consistent with the corresponding parameters in the flash datasheet.
 * @param[in]	config	- xip configuration,reference structure flash_xip_config_t
 * @return none
 */
_attribute_ram_code_sec_noinline_ void flash_set_xip_config_sram(flash_xip_config_t config)
{
    unsigned int r = plic_enter_critical_sec(s_flash_preempt_config.preempt_en, s_flash_preempt_config.threshold);

    mspi_stop_xip();
    reg_mspi_xip_config = *((unsigned short *) (&config));
    CLOCK_DLY_5_CYC;

    plic_exit_critical_sec(s_flash_preempt_config.preempt_en, r);
}
_attribute_text_sec_ void flash_set_xip_config(flash_xip_config_t config)
{
    DISABLE_BTB;
    flash_set_xip_config_sram(config);
    ENABLE_BTB;
}

/********************************************************************************************************
 *									secondary calling function,
 *	there is no need to add an circumvention solution to solve the problem of access flash conflicts.
 *******************************************************************************************************/
/**
 * @brief		This function serves to read flash mid and uid,and check the correctness of mid and uid.
 * @param[out]	flash_mid	- Flash Manufacturer ID
 * @param[out]	flash_uid	- Flash Unique ID
 * @return		0: flash no uid or not a known flash model 	 1:the flash model is known and the uid is read.
 */
_attribute_text_sec_ int flash_read_mid_uid_with_check(unsigned int * flash_mid, unsigned char * flash_uid)
{

    unsigned char no_uid[16] = { 0x51, 0x01, 0x51, 0x01, 0x51, 0x01, 0x51, 0x01, 0x51, 0x01, 0x51, 0x01, 0x51, 0x01, 0x51, 0x01 };
    int i, f_cnt = 0;
    unsigned int mid;

    mid        = flash_read_mid();
    mid        = mid & 0xffff;
    *flash_mid = mid;
    //     	  			CMD         MID
    //  P25Q80U			0x4b		0x6085
    if (mid == 0x6085)
    {
        flash_read_uid(FLASH_GD_PUYA_READ_UID_CMD, (unsigned char *) flash_uid);
    }
    else
    {
        return 0;
    }
    for (i = 0; i < 16; i++)
    {
        if (flash_uid[i] == no_uid[i])
        {
            f_cnt++;
        }
    }
    if (f_cnt == 16)
    { // no uid flash
        return 0;
    }
    else
    {
        return 1;
    }
}

