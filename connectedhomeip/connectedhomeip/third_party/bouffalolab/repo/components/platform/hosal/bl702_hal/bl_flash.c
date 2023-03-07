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
#include <bl702_romdriver.h>
#include <bl702_sf_cfg_ext.h>

#include "bl_flash.h"
#include "bl_irq.h"
#include <blog.h>
#define USER_UNUSED(a) ((void)(a))


#ifdef CFG_USE_PSRAM

#define IS_PSARAM(addr) ((addr&0xFF000000) == 0x26000000 || \
                        (addr&0xFF000000) == 0x24000000 )

extern void update_sp(void);
extern void resotre_sp(void);

/** pvPortMalloc/vPortFree is dedicated for SRAM heap access */
extern void *pvPortMalloc( size_t xSize );
extern void vPortFree( void *pv );

#ifndef PSRAM_TEMP_STACK_SIZE
#define PSRAM_TEMP_STACK_SIZE 128
#endif
static __attribute__ ((aligned(16))) StackType_t __spn[PSRAM_TEMP_STACK_SIZE] = {0};
StackType_t* const __psram_taskstack_spn_top = (StackType_t*)(__spn + PSRAM_TEMP_STACK_SIZE) - 1;
uint32_t psram_taskstack_spo = 0;
#endif

static struct {
    uint32_t magic;
    SPI_Flash_Cfg_Type flashCfg;
} boot2_flashCfg; //XXX Dont change the name of varaible, since we refer this boot2_partition_table in linker script


__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_SP(void) 
{ 
  register uint32_t result; 

  __ASM volatile ("mv %0, sp\n" : "=r" (result) ); 
  return(result); 
}

int bl_flash_erase(uint32_t addr, int len)
{
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    // if (IS_PSARAM((uint32_t)&_data)) {
    //     BL702_MemSet4(__spn, 0xa5a5a5a5, PSRAM_TEMP_STACK_SIZE);
    // }
    unsigned long mstatus_tmp;
    mstatus_tmp = read_csr(mstatus);
    clear_csr(mstatus, MSTATUS_MIE);
    bl_flash_erase_need_lock(addr, len);
    write_csr(mstatus, mstatus_tmp);

    return 0;
}

int bl_flash_write(uint32_t addr, uint8_t *src, int len)
{
    uint8_t* _data = src;
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }
#ifdef CFG_USE_PSRAM
    if(IS_PSARAM((uint32_t)src)){
        _data = (uint8_t*)pvPortMalloc(len);
        if (NULL == _data) {
            return -1;
        }
        memcpy(_data, src, len);
    }
    // if (IS_PSARAM((uint32_t)&_data)) {
    //     BL702_MemSet4(__spn, 0xa5a5a5a5, PSRAM_TEMP_STACK_SIZE);
    // }
#endif

    unsigned long mstatus_tmp;
    mstatus_tmp = read_csr(mstatus);
    clear_csr(mstatus, MSTATUS_MIE);
    bl_flash_write_need_lock(addr, _data, len);
    write_csr(mstatus, mstatus_tmp);

#ifdef CFG_USE_PSRAM
    if(IS_PSARAM((uint32_t)src)){
        vPortFree(_data);
    }
#endif

    return 0;
}

int bl_flash_read(uint32_t addr, uint8_t *dst, int len)
{
    uint8_t* _data = dst;
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

#ifdef CFG_USE_PSRAM
    if(IS_PSARAM((uint32_t)dst)){
        _data = (uint8_t*)pvPortMalloc(len);
        if (NULL == _data) {
            return -1;
        }
    }
    // if (IS_PSARAM((uint32_t)&_data)) {
    //     BL702_MemSet4(__spn, 0xa5a5a5a5, PSRAM_TEMP_STACK_SIZE);
    // }
#endif

    unsigned long mstatus_tmp;
    mstatus_tmp = read_csr(mstatus);
    clear_csr(mstatus, MSTATUS_MIE);
    bl_flash_read_need_lock(addr, _data, len);
    write_csr(mstatus, mstatus_tmp);

#ifdef CFG_USE_PSRAM
    if(IS_PSARAM((uint32_t)dst)){
        memcpy(dst, _data, len);
        vPortFree(_data);
    }
#endif

    return 0;
}


#if defined(CFG_ENCRYPT_CPU)
int ATTR_TCM_SECTION bl_flash_erase_need_lock(uint32_t addr, int len)
{
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    XIP_SFlash_Opt_Enter();
    RomDriver_XIP_SFlash_Erase_Need_Lock(
            &boot2_flashCfg.flashCfg,
            boot2_flashCfg.flashCfg.ioMode&0xf,
            addr,
            addr + len - 1
    );
    XIP_SFlash_Opt_Exit();
    return 0;
}

int ATTR_TCM_SECTION bl_flash_write_need_lock(uint32_t addr, uint8_t *src, int len)
{
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    XIP_SFlash_Opt_Enter();
    RomDriver_XIP_SFlash_Write_Need_Lock(
            &boot2_flashCfg.flashCfg,
            boot2_flashCfg.flashCfg.ioMode&0xf,
            addr,
            src,
            len
    );
    XIP_SFlash_Opt_Exit();
    return 0;
}

int ATTR_TCM_SECTION bl_flash_read_need_lock(uint32_t addr, uint8_t *dst, int len)
{
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    XIP_SFlash_Opt_Enter();
    RomDriver_XIP_SFlash_Read_Need_Lock(
            &boot2_flashCfg.flashCfg,
            boot2_flashCfg.flashCfg.ioMode&0xf,
            addr,
            dst,
            len
    );
    XIP_SFlash_Opt_Exit();
    return 0;
}
#else

static inline int bl_flash_erase_need_lock_internal(uint32_t addr, int len)
{
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    RomDriver_XIP_SFlash_Erase_Need_Lock(
            &boot2_flashCfg.flashCfg,
            boot2_flashCfg.flashCfg.ioMode&0xf,
            addr,
            addr + len - 1
    );

    return 0;
}

static inline int bl_flash_write_need_lock_internal(uint32_t addr, uint8_t *src, int len)
{
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    RomDriver_XIP_SFlash_Write_Need_Lock(
            &boot2_flashCfg.flashCfg,
            boot2_flashCfg.flashCfg.ioMode&0xf,
            addr,
            src,
            len
    );

    return 0;
}

static inline int bl_flash_read_need_lock_internal(uint32_t addr, uint8_t *dst, int len)
{
    /*We assume mid zero is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    RomDriver_XIP_SFlash_Read_Need_Lock(
            &boot2_flashCfg.flashCfg,
            boot2_flashCfg.flashCfg.ioMode&0xf,
            addr,
            dst,
            len
    );

    return 0;
}

#ifdef CFG_USE_PSRAM
static inline int bl_flash_erase_need_lock_psram(uint32_t addr, int len)
{
    /* Add code in this function need to be careful, 
     * because the stack in this function is updated 
     */
    update_sp();

    int iret = bl_flash_erase_need_lock_internal(addr, len);

    resotre_sp();

    return iret;
}

static inline int bl_flash_write_need_lock_psram(uint32_t addr, uint8_t *src, int len)
{
    /* Add code in this function need to be careful, 
     * because the stack in this function is updated 
     */
    update_sp();

    int iret = bl_flash_write_need_lock_internal(addr, src, len);

    resotre_sp();

    return iret;
}

static inline int bl_flash_read_need_lock_psram(uint32_t addr, uint8_t *dst, int len)
{
    /* Add code in this function need to be careful, 
     * because the stack in this function is updated 
     */
    update_sp();

    int iret = bl_flash_read_need_lock_internal(addr, dst, len);

    resotre_sp();

    return iret;
}
#endif

int bl_flash_erase_need_lock(uint32_t addr, int len)
{
#ifdef CFG_USE_PSRAM
    if (IS_PSARAM(__get_SP())) {
        return bl_flash_erase_need_lock_psram(addr, len);
    }
#endif

    return bl_flash_erase_need_lock_internal(addr, len);
}

int bl_flash_write_need_lock(uint32_t addr, uint8_t *src, int len)
{
#ifdef CFG_USE_PSRAM
    if (IS_PSARAM(__get_SP())) {
        return bl_flash_write_need_lock_psram(addr, src, len);
    }
#endif

    return bl_flash_write_need_lock_internal(addr, src, len);
}

int bl_flash_read_need_lock(uint32_t addr, uint8_t *dst, int len)
{
#ifdef CFG_USE_PSRAM
    if (IS_PSARAM(__get_SP())) {
        return bl_flash_read_need_lock_psram(addr, dst, len);
    }
#endif

    return bl_flash_read_need_lock_internal(addr, dst, len);
}

#endif

static void _dump_flash_config()
{
    blog_info("======= FlashCfg magiccode @%p, code 0x%08lX =======\r\n",
            &boot2_flashCfg.magic,
            boot2_flashCfg.magic
    );
    blog_info("mid \t\t0x%X\r\n", boot2_flashCfg.flashCfg.mid);
    blog_info("clkDelay \t0x%X\r\n", boot2_flashCfg.flashCfg.clkDelay);
    blog_info("clkInvert \t0x%X\r\n", boot2_flashCfg.flashCfg.clkInvert);
    blog_info("sector size\t%uKBytes\r\n", boot2_flashCfg.flashCfg.sectorSize);
    blog_info("page size\t%uBytes\r\n", boot2_flashCfg.flashCfg.pageSize);
    blog_info("---------------------------------------------------------------\r\n");
}

int bl_flash_config_update(void)
{
    _dump_flash_config();

    return 0;
}

void* bl_flash_get_flashCfg(void)
{
    return &boot2_flashCfg.flashCfg;
}

static inline int bl_flash_read_byxip_internal(uint32_t addr, uint8_t *dst, int len)
{
    uint32_t offset;
    uint32_t xipaddr;

    offset = RomDriver_SF_Ctrl_Get_Flash_Image_Offset();

    if ((addr < offset) || (addr >= 0x1000000)) {
        // not support or arg err ?
        return -1;
    }

    xipaddr =  0x23000000 - offset + addr;

    memcpy(dst, (void *)xipaddr, len);

    return 0;
}

#ifdef CFG_USE_PSRAM
static inline int bl_flash_read_byxip_psram(uint32_t addr, uint8_t *dst, int len)
{

    /* Add code in this function need to be careful, 
     * because the stack in this function is updated 
     */
    update_sp();

    bl_flash_read_byxip_internal(addr, dst, len);

    resotre_sp();

    return 0;
}
#endif

int bl_flash_read_byxip(uint32_t addr, uint8_t *dst, int len)
{
#ifdef CFG_USE_PSRAM
    uint8_t* _data = dst;
    int iret;

    if(IS_PSARAM((uint32_t)dst)){
        _data = (uint8_t*)pvPortMalloc(len);
        if (NULL == _data) {
            return -1;
        }
    }

    if (IS_PSARAM(__get_SP())) {
        iret = bl_flash_read_byxip_psram(addr, _data, len);
    }
    else {
        iret = bl_flash_read_byxip_internal(addr, _data, len);
    }

    if(IS_PSARAM((uint32_t)dst)){
        memcpy(dst, _data, len);
        vPortFree(_data);
    }

    return iret;
#else

    return bl_flash_read_byxip_internal(addr, dst, len);
#endif

}

int ATTR_TCM_SECTION bl_flash_init(void)
{
    SPI_Flash_Cfg_Type *pFlashCfg = &boot2_flashCfg.flashCfg;
    uint8_t clkDelay = 1;
    uint8_t clkInvert = 1;
    uint32_t jid = 0;
    uint32_t offset = 0;
    int ret = 0;

    // patch for SF2 swap
    *(volatile uint32_t *)0x40000130 |= (1U << 16);  // enable GPIO25 input
    *(volatile uint32_t *)0x40000134 |= (1U << 16);  // enable GPIO27 input

    // get flash config from bootheader
    L1C_Cache_Flush_Ext();
    XIP_SFlash_Read_Via_Cache_Need_Lock(8 + BL702_FLASH_XIP_BASE, (uint8_t *)&boot2_flashCfg, 4 + sizeof(SPI_Flash_Cfg_Type));
    L1C_Cache_Flush_Ext();

    // update flash config
    if (pFlashCfg->mid == 0xff) {
        XIP_SFlash_Opt_Enter();
        XIP_SFlash_State_Save(pFlashCfg, &offset);

        // store clock delay and clock invert, which have already been used for flash boot
        clkDelay = pFlashCfg->clkDelay;
        clkInvert = pFlashCfg->clkInvert;

        SFlash_GetJedecId(pFlashCfg, (uint8_t *)&jid);
        ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, pFlashCfg);
        if (ret == 0) {
            if ((pFlashCfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
                SFlash_Qspi_Enable(pFlashCfg);
            }

            if (((pFlashCfg->ioMode >> 4) & 0x01) == 1) {
                L1C_Set_Wrap(DISABLE);
            } else {
                L1C_Set_Wrap(ENABLE);
                if ((pFlashCfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
                    SFlash_SetBurstWrap(pFlashCfg);
                }
            }
        }

        // replace clock delay and clock invert
        pFlashCfg->clkDelay = clkDelay;
        pFlashCfg->clkInvert = clkInvert;

        XIP_SFlash_State_Restore(pFlashCfg, pFlashCfg->ioMode & 0x0f, offset);
        XIP_SFlash_Opt_Exit();

        L1C_Cache_Flush_Ext();
    }

    return ret;
}
