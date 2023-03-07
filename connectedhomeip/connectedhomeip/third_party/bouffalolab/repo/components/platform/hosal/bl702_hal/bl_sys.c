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
#include <bl702_glb.h>
#include <bl702_timer.h>

#include <stdio.h>
#include <stdbool.h>
#include "bl_sys.h"
#include "bl_flash.h"
#include "bl_hbn.h"

volatile bool sys_log_all_enable = true;
ATTR_HBN_NOINIT_SECTION static int wdt_triger_counter;

BL_RST_REASON_E bl_sys_rstinfo_get(void)
{
    uint8_t wdt_rst;
    uint8_t hbn_rst;
    uint8_t pds_rst;

    // get reset status
    wdt_rst = WDT_GetResetStatus();
    hbn_rst = BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE,HBN_GLB),HBN_RESET_EVENT);
    pds_rst = BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE,PDS_INT),PDS_RESET_EVENT);

    // check reset source
    if(wdt_rst){
        return BL_RST_WDT;
    }else if(hbn_rst == 0x19){
        return BL_RST_BOR;
    }else if(hbn_rst == 0x08){
        return BL_RST_HBN;
    }else if(pds_rst == 0x07){
        return BL_RST_POR;
    }else{
        return BL_RST_SOFTWARE;
    }
}

void bl_sys_rstinfo_clr(void)
{
    // clear reset status
    WDT_ClearResetStatus();
    HBN_Clear_Reset_Event();
    PDS_Clear_Reset_Event();
}

int bl_sys_logall_enable(void)
{
    sys_log_all_enable = true;
    return 0;
}

int bl_sys_logall_disable(void)
{
    sys_log_all_enable = false;
    return 0;
}

int bl_sys_reset_por(void)
{
    __disable_irq();
    GLB_SW_POR_Reset();
    while (1) {
        /*empty dead loop*/
    }

    return 0;
}

void bl_sys_reset_system(void)
{
    __disable_irq();
    GLB_SW_System_Reset();
    while (1) {
        /*empty dead loop*/
    }
}

int bl_sys_isxipaddr(uint32_t addr)
{
    if (((addr & 0xFF000000) == 0x23000000) || ((addr & 0xFF000000) == 0x43000000)) {
        return 1;
    }
    return 0;
}

int bl_sys_em_config(void)
{
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    switch (em_size) {
        case 0 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_0KB);
        }
        break;
        case 8 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_8KB);
        }
        break;
        case 16 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_16KB);
        }
        break;
        default:
        {
            /*nothing here*/
        }
    }

    return 0;
}

int bl_sys_cache_config(void)
{
    extern uint8_t __CACHE_SIZE;
    volatile uint32_t cache_size;

    cache_size = (uint32_t)&__CACHE_SIZE;

    switch (cache_size) {
        case 0 * 1024:
        {
            RomDriver_L1C_Cache_Enable_Set(0x0F);
        }
        break;
        case 4 * 1024:
        {
            RomDriver_L1C_Cache_Enable_Set(0x07);
        }
        break;
        case 8 * 1024:
        {
            RomDriver_L1C_Cache_Enable_Set(0x03);
        }
        break;
        case 12 * 1024:
        {
            RomDriver_L1C_Cache_Enable_Set(0x01);
        }
        break;
        case 16 * 1024:
        {
            RomDriver_L1C_Cache_Enable_Set(0x00);
        }
        break;
        default:
        {
            /*nothing here*/
        }
    }

    return 0;
}

int bl_sys_pkg_config(void)
{
    Efuse_Device_Info_Type dev_info;
    uint8_t isInternalFlash;
    uint8_t isInternalPsram;
    uint32_t tmpVal;

    // Get device information from efuse
    EF_Ctrl_Read_Device_Info(&dev_info);
    dev_info.flash_cfg &= 0x03;

    // flash_cfg:
    // 0: external flash using SF2
    // 1: internal 0.5M flash
    // 2: internal 1M flash
    // 3: external flash using SF1
    if(dev_info.flash_cfg==1||dev_info.flash_cfg==2){
        isInternalFlash=1;
    }else{
        isInternalFlash=0;
    }

    // psram_cfg:
    // 0: no psram
    // 1: internal 2M psram
    // 2: external psram
    // 3: reserved
    if(dev_info.psram_cfg==1){
        isInternalPsram=1;
    }else{
        isInternalPsram=0;
    }

    tmpVal=BL_RD_REG(GLB_BASE,GLB_GPIO_USE_PSRAM__IO);

    if(isInternalFlash==1&&isInternalPsram==1){
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_CFG_GPIO_USE_PSRAM_IO,0x00);
    }else if(isInternalFlash==1&&isInternalPsram==0){
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_CFG_GPIO_USE_PSRAM_IO,0x3f);
    }else{
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_CFG_GPIO_USE_PSRAM_IO,0x00);
    }

    BL_WR_REG(GLB_BASE,GLB_GPIO_USE_PSRAM__IO,tmpVal);

    return 0;
}

int bl_sys_default_active_config(void)
{
    Efuse_Device_Info_Type dev_info;
    uint32_t sw_usage;
    uint8_t jtag_group;
    uint8_t i;

    // Get device information from efuse
    EF_Ctrl_Read_Device_Info(&dev_info);

    // pkg_info:
    // 0: QFN32
    // 1: QFN40
    // 2: QFN48
    // 3: reserved
    if(dev_info.pkg_info == 2){
        EF_Ctrl_Read_Sw_Usage(0, &sw_usage);
        jtag_group = (sw_usage >> 28) & 0x3;
    }else{
        jtag_group = 0;
    }

    // Set all gpio pads in High-Z state (GPIO0 - GPIO31, jtag & cci pads excluded)
    for(i=0; i<=31; i++){
        // jtag pins
        if(jtag_group == 1){
            if(i == 3 || i == 4 || i == 5 || i == 6){
                continue;
            }
        }else if(jtag_group == 2){
            if(i == 9 || i == 10 || i == 11 || i == 12){
                continue;
            }
        }else{
            if(i == 0 || i == 1 || i == 2 || i == 9){
                continue;
            }
        }

        // cci pins
        if(i == 0 || i == 1 || i == 2 || i == 7){
            continue;
        }

        // flash or psram pins
        if(i >= 23 && i <= 28){
            continue;
        }

        GLB_GPIO_Set_HZ(i);
    }

    // Set all psram pads in High-Z state
#if !defined(CFG_USE_PSRAM)
    GLB_Set_Psram_Pad_HZ();
#endif

    return 0;
}

int bl_sys_early_init(void)
{
    if(BL_RST_WDT == bl_sys_rstinfo_get()){
        wdt_triger_counter++;
        bl_sys_rstinfo_clr();
    }
    else{
        wdt_triger_counter = 0;
    }

    bl_flash_init();

    extern BL_Err_Type HBN_Aon_Pad_IeSmt_Cfg(uint8_t padCfg);
    HBN_Aon_Pad_IeSmt_Cfg(0x1F);

    extern void freertos_risc_v_trap_handler(void); //freertos_riscv_ram/portable/GCC/RISC-V/portASM.S
    write_csr(mtvec, &freertos_risc_v_trap_handler);

    PDS_Trim_RC32M();
    HBN_Trim_RC32K();

#if (defined(CFG_PDS_ENABLE) && CFG_PDS_LEVEL == 31) || defined(CFG_HBN_ENABLE)
    HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_1P00V);
#endif

#if !(defined(CFG_PDS_ENABLE) || defined(CFG_HBN_ENABLE))
    GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_DLL144M);
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
    GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_96M, 1);
#endif

    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, SystemCoreClockGet()/(GLB_Get_BCLK_Div()+1)/4000000 - 1);

    AON_Set_DCDC18_Top_0(0xc, 0x3);
    PDS_Set_Clkpll_Top_Ctrl(0x0);
    AON_Set_Xtal_CapCode(0x30, 0x30);
    AON_Set_Xtal_Cfg(0x2, 0x2, 0x0);
    AON_Set_Xtal_CapCode_Extra(1);

    GLB_Set_USB_CLK(0);

    GLB_AHB_Slave1_Clock_Gate(0, BL_AHB_SLAVE1_UART0);
    GLB_AHB_Slave1_Clock_Gate(0, BL_AHB_SLAVE1_TMR);
    GLB_AHB_Slave1_Clock_Gate(0, BL_AHB_SLAVE1_DMA);

    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_UART0);
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_TMR);
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_DMA);

    bl_sys_pkg_config();
    bl_sys_default_active_config();

    /*debuger may NOT ready don't print anything*/
    return 0;
}

int bl_sys_init(void)
{
//    bl_sys_em_config();  // will be called in setup_heap() in start.S

    return 0;
}

int bl_sys_wdt_rst_count_get()
{
    return wdt_triger_counter;
}

