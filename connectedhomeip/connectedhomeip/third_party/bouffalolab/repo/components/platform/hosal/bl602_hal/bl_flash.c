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
#include <bl602_glb.h>
#include <bl602_xip_sflash.h>
#include <bl602_sf_cfg.h>
#include <bl602_sf_cfg_ext.h>
#include <bl602_xip_sflash_ext.h>
#include <bl602_romdriver.h>
#include "bl_flash.h"
#include "bl_irq.h"
#include <blog.h>

static SPI_Flash_Cfg_Type g_flash_cfg = {
    .resetCreadCmd=0xff,
    .resetCreadCmdSize=3,
    .mid=0xc8,

    .deBurstWrapCmd=0x77,
    .deBurstWrapCmdDmyClk=0x3,
    .deBurstWrapDataMode=SF_CTRL_DATA_4_LINES,
    .deBurstWrapData=0xF0,

    /*reg*/
    .writeEnableCmd=0x06,
    .wrEnableIndex=0x00,
    .wrEnableBit=0x01,
    .wrEnableReadRegLen=0x01,

    .qeIndex=1,
    .qeBit=0x01,
    .qeWriteRegLen=0x02,
    .qeReadRegLen=0x1,

    .busyIndex=0,
    .busyBit=0x00,
    .busyReadRegLen=0x1,
    .releasePowerDown=0xab,

    .readRegCmd[0]=0x05,
    .readRegCmd[1]=0x35,
    .writeRegCmd[0]=0x01,
    .writeRegCmd[1]=0x01,

    .fastReadQioCmd=0xeb,
    .frQioDmyClk=16/8,
    .cReadSupport=1,
    .cReadMode=0xA0,

    .burstWrapCmd=0x77,
    .burstWrapCmdDmyClk=0x3,
    .burstWrapDataMode=SF_CTRL_DATA_4_LINES,
    .burstWrapData=0x40,
     /*erase*/
    .chipEraseCmd=0xc7,
    .sectorEraseCmd=0x20,
    .blk32EraseCmd=0x52,
    .blk64EraseCmd=0xd8,
    /*write*/
    .pageProgramCmd=0x02,
    .qpageProgramCmd=0x32,
    .qppAddrMode=SF_CTRL_ADDR_1_LINE,

    .ioMode=SF_CTRL_QIO_MODE,
    .clkDelay=1,
    .clkInvert=0x1,

    .resetEnCmd=0x66,
    .resetCmd=0x99,
    .cRExit=0xff,
    .wrEnableWriteRegLen=0x00,

    /*id*/
    .jedecIdCmd=0x9f,
    .jedecIdCmdDmyClk=0,
    .qpiJedecIdCmd=0x9f,
    .qpiJedecIdCmdDmyClk=0x00,
    .sectorSize=4,
    .pageSize=256,

    /*read*/
    .fastReadCmd=0x0b,
    .frDmyClk=8/8,
    .qpiFastReadCmd =0x0b,
    .qpiFrDmyClk=8/8,
    .fastReadDoCmd=0x3b,
    .frDoDmyClk=8/8,
    .fastReadDioCmd=0xbb,
    .frDioDmyClk=0,
    .fastReadQoCmd=0x6b,
    .frQoDmyClk=8/8,

    .qpiFastReadQioCmd=0xeb,
    .qpiFrQioDmyClk=16/8,
    .qpiPageProgramCmd=0x02,
    .writeVregEnableCmd=0x50,

    /* qpi mode */
    .enterQpi=0x38,
    .exitQpi=0xff,

     /*AC*/
    .timeEsector=300,
    .timeE32k=1200,
    .timeE64k=1200,
    .timePagePgm=5,
    .timeCe=20*1000,
    .pdDelay=20,
    .qeData=0,
};

void *ATTR_TCM_SECTION arch_memcpy(void *dst, const void *src, uint32_t n)
{
    const uint8_t *p = src;
    uint8_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
}

/**
 * @brief flash_get_cfg
 *
 * @return BL_Err_Type
 */
BL_Err_Type flash_get_cfg(uint8_t **cfg_addr, uint32_t *len)
{
    *cfg_addr = (uint8_t *)&g_flash_cfg;
    *len = sizeof(SPI_Flash_Cfg_Type);

    return SUCCESS;
}

/**
 * @brief flash_set_qspi_enable
 *
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_set_qspi_enable(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        SFlash_Qspi_Enable(p_flash_cfg);
    }

    return SUCCESS;
}

/**
 * @brief flash_set_l1c_wrap
 *
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_set_l1c_wrap(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if (((p_flash_cfg->ioMode >> 4) & 0x01) == 1) {
        L1C_Set_Wrap(DISABLE);
    } else {
        L1C_Set_Wrap(ENABLE);
        if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(p_flash_cfg);
        }
    }

    return SUCCESS;
}

/**
 * @brief flash_get_clkdelay_from_bootheader
 *
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_get_clkdelay_from_bootheader(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    SPI_Flash_Cfg_Type flashCfg;
    uint8_t buf[sizeof(SPI_Flash_Cfg_Type)+8];
    uint32_t crc,*pCrc;
    char magic[] = "FCFG";

    SFlash_Read(p_flash_cfg, (p_flash_cfg->ioMode&0xf), 0, 8, buf, sizeof(SPI_Flash_Cfg_Type)+8);
    if(BL602_MemCmp(buf, magic,4)==0){
        crc=BFLB_Soft_CRC32((uint8_t *)buf+4,sizeof(SPI_Flash_Cfg_Type));
        pCrc=(uint32_t *)(buf+4+sizeof(SPI_Flash_Cfg_Type));
        if(*pCrc==crc){
            BL602_MemCpy_Fast(&flashCfg,(uint8_t *)buf+4,sizeof(SPI_Flash_Cfg_Type));
            p_flash_cfg->clkDelay = flashCfg.clkDelay;
            p_flash_cfg->clkInvert = flashCfg.clkInvert;
            return SUCCESS;
        }
    }
    
    return ERROR;
}

/**
 * @brief flash_config_init
 *
 * @return BL_Err_Type
 */
static BL_Err_Type ATTR_TCM_SECTION flash_config_init(SPI_Flash_Cfg_Type *p_flash_cfg, uint8_t *jedec_id)
{
    BL_Err_Type ret = ERROR;
    uint8_t isAesEnable = 0;
    uint32_t jid = 0;
    uint32_t offset = 0;

    GLOBAL_IRQ_SAVE();
    XIP_SFlash_Opt_Enter(&isAesEnable);
    XIP_SFlash_State_Save_Ext(p_flash_cfg, &offset);
    SFlash_GetJedecId(p_flash_cfg, (uint8_t *)&jid);
    arch_memcpy(jedec_id, (uint8_t *)&jid, 3);
    jid &= 0xFFFFFF;
    ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, p_flash_cfg);
    if (ret == SUCCESS) {
        p_flash_cfg->mid = (jid & 0xff);
    }
    flash_get_clkdelay_from_bootheader(p_flash_cfg);

    /* Set flash controler from p_flash_cfg */
    flash_set_qspi_enable(p_flash_cfg);
    flash_set_l1c_wrap(p_flash_cfg);
    XIP_SFlash_State_Restore_Ext(p_flash_cfg, offset);
    XIP_SFlash_Opt_Exit(isAesEnable);
    GLOBAL_IRQ_RESTORE();

    return ret;
}

/**
 * @brief multi flash adapter
 *
 * @return BL_Err_Type
 */
int ATTR_TCM_SECTION bl_flash_init(void)
{
    int ret = 1;
    uint32_t jedec_id = 0;

    ret = flash_config_init(&g_flash_cfg, (uint8_t *)&jedec_id);
#if 0
    MSG("flash ID = %08x\r\n", jedec_id);
    bflb_platform_dump((uint8_t *)&g_flash_cfg, sizeof(SPI_Flash_Cfg_Type));
    if (ret != SUCCESS) {
        MSG("flash config init fail!\r\n");
    }
#endif

    return ret;
}

#define USER_UNUSED(a) ((void)(a))

/* we do not get flash config from boot2*/
#if 0
static struct {
    uint32_t magic;
    SPI_Flash_Cfg_Type flashCfg;
} boot2_flashCfg; //XXX Dont change the name of varaible, since we refer this boot2_partition_table in linker script
#endif

int ATTR_TCM_SECTION bl_flash_erase(uint32_t addr, int len)
{
    uint8_t isAesEnable;

    /*We assume mid zeor is illegal*/
    if (0 == g_flash_cfg.mid) {
        return -1;
    }

    GLOBAL_IRQ_SAVE();
    XIP_SFlash_Clear_Status_Register_Need_Lock(&g_flash_cfg);
#if CONF_BL602_USE_1M_FLASH
    XIP_SFlash_Erase_Need_Lock_Ext(
            &g_flash_cfg,
            addr,
            addr + len - 1
    );
#else
    XIP_SFlash_Opt_Enter(&isAesEnable);
    RomDriver_XIP_SFlash_Erase_Need_Lock(
            &g_flash_cfg,
            addr,
            addr + len - 1
    );
    XIP_SFlash_Opt_Exit(isAesEnable);
#endif
    GLOBAL_IRQ_RESTORE();
    
    return 0;
}

int ATTR_TCM_SECTION bl_flash_write(uint32_t addr, uint8_t *src, int len)
{
    uint8_t isAesEnable;

    /*We assume mid zeor is illegal*/
    if (0 == g_flash_cfg.mid) {
        return -1;
    }

    GLOBAL_IRQ_SAVE();
    XIP_SFlash_Clear_Status_Register_Need_Lock(&g_flash_cfg);
#if CONF_BL602_USE_1M_FLASH
    XIP_SFlash_Write_Need_Lock_Ext(
            &g_flash_cfg,
            addr,
            src,
            len
    );
#else
    XIP_SFlash_Opt_Enter(&isAesEnable);
    RomDriver_XIP_SFlash_Write_Need_Lock(
            &g_flash_cfg,
            addr,
            src,
            len
    );
    XIP_SFlash_Opt_Exit(isAesEnable);
#endif
    GLOBAL_IRQ_RESTORE();
    
    return 0;
}

int ATTR_TCM_SECTION bl_flash_read(uint32_t addr, uint8_t *dst, int len)
{
    uint8_t isAesEnable;

    /*We assume mid zeor is illegal*/
    if (0 == g_flash_cfg.mid) {
        return -1;
    }

    GLOBAL_IRQ_SAVE();
#if CONF_BL602_USE_1M_FLASH
    XIP_SFlash_Read_Need_Lock_Ext(
            &g_flash_cfg,
            addr,
            dst,
            len
    );
#else
    XIP_SFlash_Opt_Enter(&isAesEnable);
    RomDriver_XIP_SFlash_Read_Need_Lock(
            &g_flash_cfg,
            addr,
            dst,
            len
    );
    XIP_SFlash_Opt_Exit(isAesEnable);
#endif
    GLOBAL_IRQ_RESTORE();

    return 0;
}

static void _dump_flash_config()
{
    extern uint8_t __boot2_flashCfg_src;

    USER_UNUSED(__boot2_flashCfg_src);
    
    blog_info("======= FlashCfg magiccode @%p=======\r\n", &__boot2_flashCfg_src);
    blog_info("mid \t\t0x%X\r\n", g_flash_cfg.mid);
    blog_info("clkDelay \t0x%X\r\n", g_flash_cfg.clkDelay);
    blog_info("clkInvert \t0x%X\r\n", g_flash_cfg.clkInvert);
    blog_info("sector size\t%uKBytes\r\n", g_flash_cfg.sectorSize);
    blog_info("page size\t%uBytes\r\n", g_flash_cfg.pageSize);
    blog_info("---------------------------------------------------------------\r\n");
}

int bl_flash_config_update(void)
{
    _dump_flash_config();

    return 0;
}

void* bl_flash_get_flashCfg(void)
{
    return &g_flash_cfg;
}

int bl_flash_read_byxip(uint32_t addr, uint8_t *dst, int len)
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

