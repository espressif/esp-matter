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
#include <bl_hbn.h>
#include <bl_irq.h>
#include <bl_flash.h>
#include <bl_rtc.h>
#include <hosal_uart.h>
#include <utils_log.h>


typedef __PACKED_STRUCT
{
    uint8_t xtal_type;
    uint8_t pll_clk;
    uint8_t hclk_div;
    uint8_t bclk_div;
    
    uint8_t flash_clk_type;
    uint8_t flash_clk_div;
    uint8_t rsvd[2];
}sys_clk_cfg_t;


#define CLK_CFG_OFFSET             (8+(4+sizeof(SPI_Flash_Cfg_Type)+4)+4)
#define CGEN_CFG0                  (~(uint8_t)((1<<1)|(1<<2)|(1<<4)))  // do not gate DMA[3], which affects M154_AES
#define CGEN_CFG1                  (~(uint32_t)((1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<7)|(1<<12)|(1<<13)|(0x7FDE<<16)))  // do not gate sf_ctrl[11], uart0[16], timer[21]

#define FAST_BOOT_TEST             0
#define TEST_GPIO                  22
#define OPEN_JTAG                  0


/* ACOMP Pin List */
static const GLB_GPIO_Type acompPinList[8] = {8, 15, 17, 11, 12, 14, 7, 9};


/* Cache Way Disable, will get from l1c register */
ATTR_HBN_NOINIT_SECTION static uint8_t cacheWayDisable;

/* EM Select, will get from glb register */
ATTR_HBN_NOINIT_SECTION static uint32_t emSel;

/* Device Information, will get from efuse */
ATTR_HBN_NOINIT_SECTION static Efuse_Device_Info_Type devInfo;

/* PSRAM IO Configuration, will get according to device information */
ATTR_HBN_NOINIT_SECTION static uint32_t psramIoCfg;

/* Clock Configuration, will get from bootheader */
ATTR_HBN_NOINIT_SECTION static sys_clk_cfg_t clkCfg;

/* Flash Configuration, will get from bl_flash_get_flashCfg() */
ATTR_HBN_NOINIT_SECTION static SPI_Flash_Cfg_Type flashCfg;

/* Flash Image Offset, will get from SF_Ctrl_Get_Flash_Image_Offset() */
ATTR_HBN_NOINIT_SECTION static uint32_t flashImageOffset;

/* SF Control Configuration, will set based on flash configuration */
ATTR_HBN_NOINIT_SECTION static SF_Ctrl_Cfg_Type sfCtrlCfg;

/* HBN Wakeup Pin Configuration */
ATTR_HBN_NOINIT_SECTION static uint8_t hbnWakeupPin;

/* HBN IRQ Status, will get from hbn register after wakeup */
ATTR_HBN_NOINIT_SECTION static uint32_t hbnIrqStatus;

/* HBN Wakeup Time, will get in rtc cycles after wakeup */
ATTR_HBN_NOINIT_SECTION static uint64_t hbnWakeupTime;


static void bl_hbn_set_sf_ctrl(SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint8_t index;
    uint8_t delay[8] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};
    
    sfCtrlCfg.owner = SF_CTRL_OWNER_SAHB;
    
    /* bit0-3 for clk delay */
    sfCtrlCfg.clkDelay = pFlashCfg->clkDelay&0x0f;
    
    /* bit4-6 for do delay */
    index = (pFlashCfg->clkDelay>>4)&0x07;
    sfCtrlCfg.doDelay = delay[index];
    
    /* bit0 for clk invert */
    sfCtrlCfg.clkInvert = pFlashCfg->clkInvert&0x01;
    
    /* bit1 for rx clk invert */
    sfCtrlCfg.rxClkInvert=(pFlashCfg->clkInvert>>1)&0x01;
    
    /* bit2-4 for di delay */
    index = (pFlashCfg->clkInvert>>2)&0x07;
    sfCtrlCfg.diDelay = delay[index];
    
    /* bit5-7 for oe delay */
    index = (pFlashCfg->clkInvert>>5)&0x07;
    sfCtrlCfg.oeDelay = delay[index];
}

static void bl_hbn_xtal_cfg(void)
{
#if 0
    uint32_t tmpVal;
    
    // optimize xtal ready time
#if !defined(CFG_HBN_OPTIMIZE)
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_SEL_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
    
    tmpVal = BL_RD_REG(AON_BASE, AON_TSEN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_INT_SEL_AON, 2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_INN_CFG_EN_AON, 1);
    BL_WR_REG(AON_BASE, AON_TSEN, tmpVal);
#else
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_SEL_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
    
    tmpVal = BL_RD_REG(AON_BASE, AON_TSEN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_INT_SEL_AON, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_INN_CFG_EN_AON, 1);
    BL_WR_REG(AON_BASE, AON_TSEN, tmpVal);
#endif
#endif
}


void bl_hbn_fastboot_init(void)
{
    unsigned long mstatus_tmp;
    
    // Get cache way disable setting
    cacheWayDisable = BL_GET_REG_BITS_VAL(BL_RD_REG(L1C_BASE, L1C_CONFIG), L1C_WAY_DIS);
    
    // Get EM select
    emSel = BL_RD_REG(GLB_BASE, GLB_SEAM_MISC);
    
    // Get device information from efuse
    EF_Ctrl_Read_Device_Info(&devInfo);
    devInfo.flash_cfg &= 0x03;
    
    // Get psram io configuration
    psramIoCfg = ((devInfo.flash_cfg == 1 || devInfo.flash_cfg == 2) && devInfo.psram_cfg != 1) ? 0x3F : 0x00;
    
    // Get clock configuration from bootheader
    mstatus_tmp = read_csr(mstatus);
    clear_csr(mstatus, MSTATUS_MIE);
    bl_flash_read_need_lock(CLK_CFG_OFFSET, (uint8_t *)(&clkCfg), sizeof(sys_clk_cfg_t));
    write_csr(mstatus, mstatus_tmp);
    
    // Get flash configuration
    memcpy(&flashCfg, bl_flash_get_flashCfg(), sizeof(SPI_Flash_Cfg_Type));
    
    // Get flash image offset
    flashImageOffset = SF_Ctrl_Get_Flash_Image_Offset();
    
    // Set SF control configuration
    bl_hbn_set_sf_ctrl(&flashCfg);
    
    // Optimize xtal configuration
    bl_hbn_xtal_cfg();
    
    // Overwrite default soft start delay (default 0, which may cause wakeup failure)
    AON_Set_LDO11_SOC_Sstart_Delay(2);
    
    // Select 32K (RC32K and XTAL32K are both default on)
#ifdef CFG_USE_XTAL32K
    HBN_32K_Sel(HBN_32K_XTAL);
    HBN_Power_Off_RC32K();
#else
    HBN_32K_Sel(HBN_32K_RC);
    HBN_Power_Off_Xtal_32K();
#endif
    
    // Disable GPIO9 pull up/down to reduce PDS/HBN current, 0x4000F014[16]=0
    HBN_Hw_Pu_Pd_Cfg(DISABLE);
    
    // Disable GPIO9 - GPIO13 IE/SMT, 0x4000F014[12:8]=5'b00000
    HBN_Aon_Pad_IeSmt_Cfg(0);
    
    // Disable GPIO9 - GPIO13 wakeup, 0x4000F014[7:3]=5'b11111
    HBN_Pin_WakeUp_Mask(0x1F);
}

void bl_hbn_gpio_wakeup_cfg(uint8_t pin_list[], uint8_t pin_num)
{
    int i;
    int pin;
    
    hbnWakeupPin = 0;
    
    for(i = 0; i < pin_num; i++){
        pin = pin_list[i];
        
        if(pin >= 9 && pin <= 12){
            GLB_GPIO_Set_HZ(pin);
            hbnWakeupPin |= 1 << (pin - 9);
        }
    }
    
    if(hbnWakeupPin == 0){
        HBN_Aon_Pad_IeSmt_Cfg(0);
        HBN_Pin_WakeUp_Mask(0x1F);
    }else{
        HBN_Aon_Pad_IeSmt_Cfg(hbnWakeupPin);
        HBN_Pin_WakeUp_Mask(~hbnWakeupPin & 0x1F);
        HBN_GPIO_INT_Enable(HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE);
    }
}

void bl_hbn_acomp_wakeup_cfg(uint8_t acomp_id, uint8_t ch_sel, uint8_t edge_sel)
{
    AON_ACOMP_CFG_Type cfg = {
        .muxEn = ENABLE,                                          /*!< ACOMP mux enable */
        .posChanSel = ch_sel,                                     /*!< ACOMP positive channel select */
        .negChanSel = AON_ACOMP_CHAN_0P3125VBAT,                  /*!< ACOMP negtive channel select */
        .levelFactor = AON_ACOMP_LEVEL_FACTOR_1,                  /*!< ACOMP level select factor */
        .biasProg = AON_ACOMP_BIAS_POWER_MODE1,                   /*!< ACOMP bias current control */
        .hysteresisPosVolt = AON_ACOMP_HYSTERESIS_VOLT_50MV,      /*!< ACOMP hysteresis voltage for positive */
        .hysteresisNegVolt = AON_ACOMP_HYSTERESIS_VOLT_50MV,      /*!< ACOMP hysteresis voltage for negtive */
    };
    
    GLB_GPIO_Func_Init(GPIO_FUN_ANALOG, (GLB_GPIO_Type *)&acompPinList[ch_sel], 1);
    
    AON_ACOMP_Init((AON_ACOMP_ID_Type)acomp_id, &cfg);
    AON_ACOMP_Enable((AON_ACOMP_ID_Type)acomp_id);
    
    if(edge_sel == HBN_ACOMP_EDGE_RISING){
        if(acomp_id == 0){
            HBN_Enable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_POSEDGE);
            HBN_Disable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_NEGEDGE);
        }else{
            HBN_Enable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_POSEDGE);
            HBN_Disable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_NEGEDGE);
        }
    }else if(edge_sel == HBN_ACOMP_EDGE_FALLING){
        if(acomp_id == 0){
            HBN_Disable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_POSEDGE);
            HBN_Enable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_NEGEDGE);
        }else{
            HBN_Disable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_POSEDGE);
            HBN_Enable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_NEGEDGE);
        }
    }else{
        if(acomp_id == 0){
            HBN_Enable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_POSEDGE);
            HBN_Enable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_NEGEDGE);
        }else{
            HBN_Enable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_POSEDGE);
            HBN_Enable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_NEGEDGE);
        }
    }
}

// must be placed in hbncode section
static void ATTR_HBN_CODE_SECTION bl_hbn_restore_flash(SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t tmp[1];
    
    *(volatile uint32_t *)0x4000F02C &= ~((0x3F << 16) | (0x3F << 24));
    
    RomDriver_SF_Cfg_Init_Flash_Gpio((devInfo.flash_cfg<<2)|devInfo.sf_swap_cfg, 1);
    
    *(volatile uint32_t *)0x40000130 |= (1U << 16);  // enable GPIO25 input
    *(volatile uint32_t *)0x40000134 |= (1U << 16);  // enable GPIO27 input
    
    RomDriver_SFlash_Init(&sfCtrlCfg);
    
    RomDriver_SFlash_Releae_Powerdown(pFlashCfg);
    
    RomDriver_SFlash_Reset_Continue_Read(pFlashCfg);
    
    RomDriver_SFlash_Software_Reset(pFlashCfg);
    
    RomDriver_SFlash_Write_Enable(pFlashCfg);
    
    RomDriver_SFlash_DisableBurstWrap(pFlashCfg);
    
    RomDriver_SFlash_SetSPIMode(SF_CTRL_SPI_MODE);
    
    RomDriver_SF_Ctrl_Set_Flash_Image_Offset(0);
    
    if((pFlashCfg->ioMode&0x0f)==SF_CTRL_QO_MODE||(pFlashCfg->ioMode&0x0f)==SF_CTRL_QIO_MODE){
        RomDriver_SFlash_Qspi_Enable(pFlashCfg);
    }
    
    if(((pFlashCfg->ioMode>>4)&0x01)==1){
        RomDriver_L1C_Set_Wrap(DISABLE);
    }else{
        RomDriver_L1C_Set_Wrap(ENABLE);
        RomDriver_SFlash_Write_Enable(pFlashCfg);
        RomDriver_SFlash_SetBurstWrap(pFlashCfg);
    }
    
    if(pFlashCfg->cReadSupport){
        RomDriver_SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        RomDriver_SFlash_Read(pFlashCfg, pFlashCfg->ioMode&0xf, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));
    }
    
    RomDriver_SF_Ctrl_Set_Flash_Image_Offset(flashImageOffset);
    RomDriver_SFlash_Cache_Read_Enable(pFlashCfg, pFlashCfg->ioMode&0xf, pFlashCfg->cReadSupport, cacheWayDisable);
    
    // Patch: restore psram io configuration
    BL_WR_REG(GLB_BASE, GLB_GPIO_USE_PSRAM__IO, psramIoCfg);
}

#if !defined(CFG_HBN_OPTIMIZE)
// can be placed in flash, here placed in hbncode section to reduce fast boot time
static void ATTR_NOINLINE ATTR_HBN_CODE_SECTION bl_hbn_restore_tcm(void)
{
    extern uint8_t _tcm_load;
    extern uint8_t _tcm_run;
    extern uint8_t _tcm_run_end;
    uint32_t src = (uint32_t)&_tcm_load;
    uint32_t dst = (uint32_t)&_tcm_run;
    uint32_t end = (uint32_t)&_tcm_run_end;
    
    while(dst < end){
        *(uint32_t *)dst = *(uint32_t *)src;
        src += 4;
        dst += 4;
    }
}

// can be placed in flash, here placed in hbncode section to reduce fast boot time
static void ATTR_NOINLINE ATTR_HBN_CODE_SECTION bl_hbn_restore_data(void)
{
    extern uint8_t _data_load;
    extern uint8_t _data_run;
    extern uint8_t _data_run_end;
    uint32_t src = (uint32_t)&_data_load;
    uint32_t dst = (uint32_t)&_data_run;
    uint32_t end = (uint32_t)&_data_run_end;
    
    while(dst < end){
        *(uint32_t *)dst = *(uint32_t *)src;
        src += 4;
        dst += 4;
    }
}

// can be placed in flash, here placed in hbncode section to reduce fast boot time
static void ATTR_NOINLINE ATTR_HBN_CODE_SECTION bl_hbn_restore_bss(void)
{
    extern uint8_t __bss_start;
    extern uint8_t __bss_end;
    uint32_t dst = (uint32_t)&__bss_start;
    uint32_t end = (uint32_t)&__bss_end;
    
    while(dst < end){
        *(uint32_t *)dst = 0;
        dst += 4;
    }
}
#else

// can be placed in flash, here placed in hbn section to reduce fast boot time
static void ATTR_NOINLINE ATTR_HBN_CODE_SECTION bl_hbn_set_gpio_high_z(void)
{
    int pin;
    uint32_t tmpVal;
    
    // Set all gpio pads in High-Z state (GPIO0 - GPIO31)
    for(pin = 0; pin <= 31; pin++){
#if OPEN_JTAG
        if(pin == 0 || pin == 1 || pin == 2 || pin == 9){
            continue;
        }
#endif
        if(pin >= 23 && pin <= 28){
            continue;
        }
#if FAST_BOOT_TEST == 2
        if(pin == TEST_GPIO){
            continue;
        }
#endif
        
        GLB_GPIO_Set_HZ(pin);
    }
    
    // Set all psram pads in High-Z state
    GLB_Set_Psram_Pad_HZ();
    
    // Will be overwritten by GLB_GPIO_Set_HZ()
    if(hbnWakeupPin != 0){
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_PAD_IE_SMT, hbnWakeupPin);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    }
}
#endif

// must be placed in hbncode section
static void ATTR_HBN_CODE_SECTION bl_hbn_fastboot_entry(void)
{
#if FAST_BOOT_TEST == 1 || FAST_BOOT_TEST == 2
    GLB_GPIO_Cfg_Type gpioCfg;
    GLB_GPIO_Type gpioPin=TEST_GPIO;
    uint32_t *pOut=(uint32_t *)(GLB_BASE+GLB_GPIO_OUTPUT_OFFSET+((gpioPin>>5)<<2));
    uint32_t pos=gpioPin%32;
    
    gpioCfg.gpioPin=gpioPin;
    gpioCfg.gpioFun=11;
    gpioCfg.gpioMode=GPIO_MODE_OUTPUT;
    gpioCfg.pullType=GPIO_PULL_NONE;
    gpioCfg.drive=0;
    gpioCfg.smtCtrl=1;
    RomDriver_GLB_GPIO_Init(&gpioCfg);
#endif
    
#if FAST_BOOT_TEST == 1
    while(1){
        *pOut |= (1<<pos);
        RomDriver_BL702_Delay_MS(100);
        *pOut &= ~(1<<pos);
        RomDriver_BL702_Delay_MS(100);
    }
#endif
    
#if FAST_BOOT_TEST == 2
    *pOut |= (1<<pos);
    RomDriver_BL702_Delay_US(1);
    *pOut &= ~(1<<pos);
#endif
    
    __asm__ __volatile__(
            ".option push\n\t"
            ".option norelax\n\t"
            "la gp, __global_pointer$\n\t"
            ".option pop\n\t"
    );
    
    hbnIrqStatus = BL_RD_REG(HBN_BASE, HBN_IRQ_STAT);
    
#if 1
    if(hbnIrqStatus & 0x01){
        *(uint32_t *)0x40000110 = 0x0B020B03;
        *(uint32_t *)0x40000188 = 0x00000200;
        *(uint32_t *)0x40000190 = 0x00000200;
        RomDriver_BL702_Delay_US(500);
        *(uint32_t *)0x40000190 = 0x00000000;
        *(uint32_t *)0x40000188 = 0x00000000;
        *(uint32_t *)0x40000110 = 0x0B030B03;
        RomDriver_BL702_Delay_US(500);
        hbnIrqStatus &= ~((*(volatile uint32_t *)0x40000180 >> 9) & 0x01);
#if OPEN_JTAG
        *(uint32_t *)0x40000110 = 0x0E020B03;
#endif
    }
#endif
    
#if !defined(CFG_HBN_OPTIMIZE)
    // Configure clock (must use rom driver, since tcm code is lost and flash is power down)
    RomDriver_GLB_Set_System_CLK(clkCfg.xtal_type, clkCfg.pll_clk);
    RomDriver_GLB_Set_System_CLK_Div(clkCfg.hclk_div, clkCfg.bclk_div);
    RomDriver_GLB_Set_SF_CLK(1, clkCfg.flash_clk_type, clkCfg.flash_clk_div);
    
#if FAST_BOOT_TEST == 2
    *pOut |= (1<<pos);
    RomDriver_BL702_Delay_US(1);
    *pOut &= ~(1<<pos);
#endif
#else
    // Power on XTAL32M, later will check whether it is ready for use
    *(volatile uint32_t *)(AON_BASE + AON_RF_TOP_AON_OFFSET) |= (0x3 << 4);
    
    // Turn off RF
    *(volatile uint32_t *)(AON_BASE + AON_RF_TOP_AON_OFFSET) &= ~(uint32_t)((1<<0)|(1<<1)|(1<<2));
    
    // Disable peripheral clock
    *(volatile uint32_t *)(GLB_BASE + GLB_CLK_CFG1_OFFSET) &= ~(uint32_t)((1<<8)|(1<<13)|(1<<24)|(1<<25)|(1<<28));
    *(volatile uint32_t *)(GLB_BASE + GLB_CLK_CFG2_OFFSET) &= ~(uint32_t)((1<<4)|(1<<23)|(0xFF<<24));
    *(volatile uint32_t *)(GLB_BASE + GLB_CLK_CFG3_OFFSET) &= ~(uint32_t)((1<<8)|(1<<24));
    
    // Switch pka clock
    *(volatile uint32_t *)(GLB_BASE + GLB_SWRST_CFG2_OFFSET) |= (0x1 << 24);
    
    // Gate peripheral clock
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, CGEN_CFG0);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, CGEN_CFG1);
    
    // Select XCLK as flash clock, XCLK is RC32M at this moment
    RomDriver_GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_XCLK, 0);
#endif
    
    // Configure flash (must use rom driver, since tcm code is lost and flash is power down)
    bl_hbn_restore_flash(&flashCfg);
    
#if FAST_BOOT_TEST == 2
    *pOut |= (1<<pos);
    RomDriver_BL702_Delay_US(1);
    *pOut &= ~(1<<pos);
#endif
    
    // Get wakeup time in rtc cycles
    hbnWakeupTime = bl_rtc_get_counter();
    
    // Switch stack pointer
    __asm__ __volatile__(
            "la sp, _sp_main\n\t"
    );
    
    // Restore EM select before using new stack pointer
    BL_WR_REG(GLB_BASE, GLB_SEAM_MISC, emSel);
    
#if !defined(CFG_HBN_OPTIMIZE)  
    // Restore tcmcode section
    bl_hbn_restore_tcm();
    
#if FAST_BOOT_TEST == 2
    GLB_GPIO_Write(TEST_GPIO, 1);
    RomDriver_BL702_Delay_US(1);
    GLB_GPIO_Write(TEST_GPIO, 0);
#endif
    
    // Restore data section
    bl_hbn_restore_data();
    
#if FAST_BOOT_TEST == 2
    GLB_GPIO_Write(TEST_GPIO, 1);
    RomDriver_BL702_Delay_US(1);
    GLB_GPIO_Write(TEST_GPIO, 0);
#endif
    
    // Restore bss section
    bl_hbn_restore_bss();
    
#if FAST_BOOT_TEST == 2
    GLB_GPIO_Write(TEST_GPIO, 1);
    RomDriver_BL702_Delay_US(1);
    GLB_GPIO_Write(TEST_GPIO, 0);
#endif
#else
    // Set all gpio pads in High-Z state
    bl_hbn_set_gpio_high_z();
    
    // Wait until XTAL32M is ready for use
    while(!BL_IS_REG_BIT_SET(BL_RD_REG(AON_BASE, AON_TSEN), AON_XTAL_RDY));
    RomDriver_BL702_Delay_MS(1);
    
    // Select XTAL32M as root clock
    RomDriver_HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);
    
    // Turn on RF
    *(volatile uint32_t *)(AON_BASE + AON_RF_TOP_AON_OFFSET) |= ((1<<0)|(1<<1)|(1<<2));
    
#if FAST_BOOT_TEST == 2
    GLB_GPIO_Write(TEST_GPIO, 1);
    RomDriver_BL702_Delay_US(1);
    GLB_GPIO_Write(TEST_GPIO, 0);
#endif
#endif
    
    // Disable global interrupt
    __disable_irq();
    
    // Set interrupt vector
    extern void freertos_risc_v_trap_handler(void);
    write_csr(mtvec, &freertos_risc_v_trap_handler);
    
    // Call PDS_Set_Clkpll_Top_Ctrl(0x0), refer to bl_sys_early_init()
    *(volatile uint32_t *)(PDS_BASE + PDS_CLKPLL_TOP_CTRL_OFFSET) &= ~(uint32_t)(0x3<<20);
    
    // Call user callback
    bl_hbn_fastboot_done_callback();
    while(1);
}

// can be placed in tcm section, here placed in hbncode section to reduce fast boot time
static void ATTR_NOINLINE ATTR_HBN_CODE_SECTION bl_hbn_mode_enter(void)
{
    uint32_t tmpVal;
    
#if 0
    // Power down flash
    RomDriver_SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    RomDriver_SFlash_Reset_Continue_Read(&flashCfg);
    RomDriver_SFlash_Powerdown();
#endif
    
    // Pull up flash pads
    if(devInfo.flash_cfg == 0){
        // External SF2 (GPIO23 - GPIO28)
        *(volatile uint32_t *)0x4000F02C |= (0x1F << 16);  // As boot pin, GPIO28 should not be pulled up
    }else if(devInfo.flash_cfg == 3){
        // External SF1 (GPIO17 - GPIO22)
        // Do nothing
    }else{
        // Internal SF2 (GPIO23 - GPIO28)
        // Do nothing
    }
    
    // Select RC32M
    RomDriver_HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
    RomDriver_GLB_Power_Off_DLL();
    RomDriver_AON_Power_Off_XTAL();
    
    // Enter HBN0
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_AON_VOUT_SEL, HBN_LDO_LEVEL_1P00V);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_RT_VOUT_SEL, HBN_LDO_LEVEL_1P00V);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);
    while(1);
}


void bl_hbn_enter_with_fastboot(uint32_t hbnSleepCycles)
{
    uint32_t valLow, valHigh;
    
    __disable_irq();
    
#if defined(CFG_HBN_OPTIMIZE)
    // Turn off RF
    *(volatile uint32_t *)(AON_BASE + AON_RF_TOP_AON_OFFSET) &= ~(uint32_t)((1<<0)|(1<<1)|(1<<2));
#endif
    
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0x0050001F);
    
    if(hbnSleepCycles != 0){
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        
        valLow += hbnSleepCycles;
        if(valLow < hbnSleepCycles){
            valHigh++;
        }
        
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, valLow, valHigh, HBN_RTC_COMP_BIT0_39);
    }
    
    HBN_Set_Wakeup_Addr((uint32_t)bl_hbn_fastboot_entry);
    HBN_Set_Status_Flag(HBN_STATUS_ENTER_FLAG);
    
    bl_hbn_mode_enter();
}

int bl_hbn_get_wakeup_source(void)
{
    // irq_rtc is cleared in bootrom, so we assume wakeup by RTC if not wakeup by GPIO and ACOMP
    if(hbnIrqStatus & 0x1F){
        return HBN_WAKEUP_BY_GPIO;
    }else if(hbnIrqStatus & (1U << 20)){
        return HBN_WAKEUP_BY_ACOMP0;
    }else if(hbnIrqStatus & (1U << 22)){
        return HBN_WAKEUP_BY_ACOMP1;
    }else{
        return HBN_WAKEUP_BY_RTC;
    }
}

uint32_t bl_hbn_get_wakeup_gpio(void)
{
    return (hbnIrqStatus & 0x1F) << 9;
}

uint64_t bl_hbn_get_wakeup_time(void)
{
    return hbnWakeupTime;
}

__attribute__((weak)) void bl_hbn_fastboot_done_callback(void)
{
    HOSAL_UART_DEV_DECL(uart_stdio, 0, 14, 15, 2000000);
    hosal_uart_init(&uart_stdio);
    
    while(1){
        printf("HBN fast boot done!\r\n");
        printf("HBN_IRQ_STAT: 0x%08lX\r\n", hbnIrqStatus);
        BL702_Delay_MS(1000);
    }
}


int bl_hbn_enter(hbn_type_t *hbn, uint32_t *time)
{
#if 0
    SPI_Flash_Cfg_Type Gd_Q80E_Q16E = {
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
            .clkInvert=0x3f,

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
#endif
    HBN_APP_CFG_Type cfg = {
        .useXtal32k =0,                                        /*!< Wheather use xtal 32K as 32K clock source,otherwise use rc32k */
        .sleepTime =0,                                         /*!< HBN sleep time */
        .gpioWakeupSrc=HBN_WAKEUP_GPIO_NONE,                   /*!< GPIO Wakeup source */
        .gpioTrigType=HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE, /*!< GPIO Triger type */
        .flashCfg= NULL,                                      /*!< Flash config pointer, used when power down flash */
        .flashPinCfg=0,                                       /*!< 0 ext flash 23-28, 1 internal flash 23-28, 2 internal flash 23-28, 3 ext flash 17-22 */
        .hbnLevel=HBN_LEVEL_1,                                /*!< HBN level */
        .ldoLevel=HBN_LDO_LEVEL_1P10V,                        /*!< LDO level */
    };

    Efuse_Device_Info_Type dev_info;
    EF_Ctrl_Read_Device_Info(&dev_info);
    dev_info.flash_cfg &= 0x03;

    if (hbn) {
        printf("hbn.buflen = %d\r\n", hbn->buflen);
        printf("hbn.active = %d\r\n", hbn->active);
        log_buf(hbn->buf, hbn->buflen);
    }

    cfg.sleepTime = (*time << 15) / 1000;  // ms -> rtc cycles
    if ((!hbn->buf) || (hbn->buflen > 5)) {
        printf("not support arg.\r\n");
        return -1;
    }

    int i;
    for (i=0; i<hbn->buflen; i++) {
        if (hbn->buf[i] == 9) {
            cfg.gpioWakeupSrc |= HBN_WAKEUP_GPIO_9;
        } else if (hbn->buf[i] == 10) {
            cfg.gpioWakeupSrc |= HBN_WAKEUP_GPIO_10;
        } else if (hbn->buf[i] == 11) {
            cfg.gpioWakeupSrc |= HBN_WAKEUP_GPIO_11;
        } else if (hbn->buf[i] == 12) {
            cfg.gpioWakeupSrc |= HBN_WAKEUP_GPIO_12;
        } else if (hbn->buf[i] == 13) {
            cfg.gpioWakeupSrc |= HBN_WAKEUP_GPIO_13;
        } else {
            printf("invalid arg.\r\n");
            return -1;
        }
    }
    if (hbn->buflen > 0) {
        printf("hbn");
        for (i=0; i<hbn->buflen; i++) {
            printf(" gpio%d", hbn->buf[i]);
        }
        printf(".\r\n");
    }

    cfg.flashCfg = bl_flash_get_flashCfg();
    cfg.flashPinCfg = dev_info.flash_cfg;

    HBN_Mode_Enter(&cfg);
    return -1;
}

