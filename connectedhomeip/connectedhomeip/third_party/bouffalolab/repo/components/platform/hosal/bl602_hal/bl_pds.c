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
#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <utils_list.h>
#include <bl602_glb.h>
#include <bl602_sec_eng.h>
#include <bl602_sf_cfg_ext.h>
#include <bl602_sflash_ext.h>
#include <bl602_romdriver.h>
#include <blog.h>

#include "bl_pds.h"

/* PDS0 Configuration */
static PDS_DEFAULT_LV_CFG_Type pdsCfgLevel0 = {
    .pdsCtl = {
        .pdsStart     =1,
        .sleepForever =0,
        .xtalForceOff =0,
        .saveWiFiState=0,
        .dcdc18Off    =1,
        .bgSysOff     =1,
        .clkOff       =1,
        .memStby      =1,
        .isolation    =1,
        .waitXtalRdy  =0,
        .pdsPwrOff    =1,
        .xtalOff      =1,
        .socEnbForceOn=1,
        .pdsRstSocEn  =0,
        .pdsRC32mOn   =0,
        .pdsLdoVselEn =0,
        .wfiMask      =1,
        .ldo11Off     =0,
        .pdsLdoVol    =0xA,
        .pdsCtlRfSel  =3,
        .pdsCtlPllSel =0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff   =0,
        .forceWbPwrOff    =0,
        .forceCpuIsoPwrOff=0,
        .forceWbIsoPwrOff =0,
        .forceCpuPdsRst   =0,
        .forceWbPdsRst    =0,
        .forceCpuMemStby  =0,
        .forceWbMemStby   =0,
        .forceCpuGateClk  =0,
        .forceWbGateClk   =0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff =0,
        .forceMiscIsoEn  =0,
        .forceMiscPdsRst =0,
        .forceMiscMemStby=0,
        .forceMiscGateClk=0,
        .CpuIsoEn        =0,
        .WbIsoEn         =0,
        .MiscIsoEn       =0,
    },
    .pdsCtl4 = {
        .cpuPwrOff  =0,
        .cpuRst     =0,
        .cpuMemStby =1,
        .cpuGateClk =1,
        .WbPwrOff   =0,
        .WbRst      =0,
        .WbMemStby  =1,
        .WbGateClk  =1,
        .MiscPwrOff =0,
        .MiscRst    =0,
        .MiscMemStby=1,
        .MiscGateClk=1,
    }
};

/* PDS1 Configuration */
static PDS_DEFAULT_LV_CFG_Type pdsCfgLevel1 = {
    .pdsCtl = {
        .pdsStart     =1,
        .sleepForever =0,
        .xtalForceOff =0,
        .saveWiFiState=0,
        .dcdc18Off    =1,
        .bgSysOff     =1,
        .clkOff       =1,
        .memStby      =1,
        .isolation    =1,
        .waitXtalRdy  =0,
        .pdsPwrOff    =1,
        .xtalOff      =1,
        .socEnbForceOn=1,
        .pdsRstSocEn  =0,
        .pdsRC32mOn   =0,
        .pdsLdoVselEn =0,
        .wfiMask      =1,
        .ldo11Off     =0,
        .pdsLdoVol    =0xA,
        .pdsCtlRfSel  =2,
        .pdsCtlPllSel =0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff   =0,
        .forceWbPwrOff    =0,
        .forceCpuIsoPwrOff=0,
        .forceWbIsoPwrOff =0,
        .forceCpuPdsRst   =0,
        .forceWbPdsRst    =0,
        .forceCpuMemStby  =0,
        .forceWbMemStby   =0,
        .forceCpuGateClk  =0,
        .forceWbGateClk   =0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff =0,
        .forceMiscIsoEn  =0,
        .forceMiscPdsRst =0,
        .forceMiscMemStby=0,
        .forceMiscGateClk=0,
        .CpuIsoEn        =0,
        .WbIsoEn         =1,
        .MiscIsoEn       =0,
    },
    .pdsCtl4 = {
        .cpuPwrOff  =0,
        .cpuRst     =0,
        .cpuMemStby =1,
        .cpuGateClk =1,
        .WbPwrOff   =1,
        .WbRst      =1,
        .WbMemStby  =1,
        .WbGateClk  =1,
        .MiscPwrOff =0,
        .MiscRst    =0,
        .MiscMemStby=1,
        .MiscGateClk=1,
    }
};

/* PDS2 Configuration */
static PDS_DEFAULT_LV_CFG_Type pdsCfgLevel2 = {
    .pdsCtl = {
        .pdsStart     =1,
        .sleepForever =0,
        .xtalForceOff =0,
        .saveWiFiState=0,
        .dcdc18Off    =1,
        .bgSysOff     =1,
        .clkOff       =1,
        .memStby      =1,
        .isolation    =1,
        .waitXtalRdy  =0,
        .pdsPwrOff    =1,
        .xtalOff      =1,
        .socEnbForceOn=1,
        .pdsRstSocEn  =0,
        .pdsRC32mOn   =0,
        .pdsLdoVselEn =0,
        .wfiMask      =1,
        .ldo11Off     =0,
        .pdsLdoVol    =0xA,
        .pdsCtlRfSel  =3,
        .pdsCtlPllSel =0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff   =0,
        .forceWbPwrOff    =0,
        .forceCpuIsoPwrOff=0,
        .forceWbIsoPwrOff =0,
        .forceCpuPdsRst   =0,
        .forceWbPdsRst    =0,
        .forceCpuMemStby  =0,
        .forceWbMemStby   =0,
        .forceCpuGateClk  =0,
        .forceWbGateClk   =0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff =0,
        .forceMiscIsoEn  =0,
        .forceMiscPdsRst =0,
        .forceMiscMemStby=0,
        .forceMiscGateClk=0,
        .CpuIsoEn        =1,
        .WbIsoEn         =0,
        .MiscIsoEn       =0,
    },
    .pdsCtl4 = {
        .cpuPwrOff  =1,
        .cpuRst     =1,
        .cpuMemStby =1,
        .cpuGateClk =1,
        .WbPwrOff   =0,
        .WbRst      =0,
        .WbMemStby  =1,
        .WbGateClk  =1,
        .MiscPwrOff =0,
        .MiscRst    =0,
        .MiscMemStby=1,
        .MiscGateClk=1,
    }
};

/* PDS3 Configuration */
static PDS_DEFAULT_LV_CFG_Type pdsCfgLevel3 = {
    .pdsCtl = {
        .pdsStart     =1,
        .sleepForever =0,
        .xtalForceOff =0,
        .saveWiFiState=0,
        .dcdc18Off    =1,
        .bgSysOff     =1,
        .clkOff       =1,
        .memStby      =1,
        .isolation    =1,
        .waitXtalRdy  =0,
        .pdsPwrOff    =1,
        .xtalOff      =1,
        .socEnbForceOn=1,
        .pdsRstSocEn  =0,
        .pdsRC32mOn   =0,
        .pdsLdoVselEn =0,
        .wfiMask      =1,
        .ldo11Off     =0,
        .pdsLdoVol    =0xA,
        .pdsCtlRfSel  =2,
        .pdsCtlPllSel =0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff   =0,
        .forceWbPwrOff    =0,
        .forceCpuIsoPwrOff=0,
        .forceWbIsoPwrOff =0,
        .forceCpuPdsRst   =0,
        .forceWbPdsRst    =0,
        .forceCpuMemStby  =0,
        .forceWbMemStby   =0,
        .forceCpuGateClk  =0,
        .forceWbGateClk   =0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff =0,
        .forceMiscIsoEn  =0,
        .forceMiscPdsRst =0,
        .forceMiscMemStby=0,
        .forceMiscGateClk=0,
        .CpuIsoEn        =1,
        .WbIsoEn         =1,
        .MiscIsoEn       =0,
    },
    .pdsCtl4 = {
        .cpuPwrOff  =1,
        .cpuRst     =1,
        .cpuMemStby =1,
        .cpuGateClk =1,
        .WbPwrOff   =1,
        .WbRst      =1,
        .WbMemStby  =1,
        .WbGateClk  =1,
        .MiscPwrOff =0,
        .MiscRst    =0,
        .MiscMemStby=1,
        .MiscGateClk=1,
    }
};

/* PDS7 Configuration */
static PDS_DEFAULT_LV_CFG_Type pdsCfgLevel7 = {
    .pdsCtl = {
        .pdsStart     =1,
        .sleepForever =0,
        .xtalForceOff =0,
        .saveWiFiState=0,
        .dcdc18Off    =1,
        .bgSysOff     =1,
        .clkOff       =1,
        .memStby      =1,
        .isolation    =1,
        .waitXtalRdy  =0,
        .pdsPwrOff    =1,
        .xtalOff      =1,
        .socEnbForceOn=0,
        .pdsRstSocEn  =0,
        .pdsRC32mOn   =0,
        .pdsLdoVselEn =0,
        .wfiMask      =1,
        .ldo11Off     =1,
        .pdsLdoVol    =0xA,
        .pdsCtlRfSel  =2,
        .pdsCtlPllSel =0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff   =0,
        .forceWbPwrOff    =0,
        .forceCpuIsoPwrOff=0,
        .forceWbIsoPwrOff =0,
        .forceCpuPdsRst   =0,
        .forceWbPdsRst    =0,
        .forceCpuMemStby  =0,
        .forceWbMemStby   =0,
        .forceCpuGateClk  =0,
        .forceWbGateClk   =0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff =0,
        .forceMiscIsoEn  =0,
        .forceMiscPdsRst =0,
        .forceMiscMemStby=0,
        .forceMiscGateClk=0,
        .CpuIsoEn        =1,
        .WbIsoEn         =1,
        .MiscIsoEn       =1,
    },
    .pdsCtl4 = {
        .cpuPwrOff  =1,
        .cpuRst     =1,
        .cpuMemStby =1,
        .cpuGateClk =1,
        .WbPwrOff   =1,
        .WbRst      =1,
        .WbMemStby  =1,
        .WbGateClk  =1,
        .MiscPwrOff =1,
        .MiscRst    =1,
        .MiscMemStby=1,
        .MiscGateClk=1,
    }
};

/* Flash Configuration, will get from bootheader */
static SPI_Flash_Cfg_Type flashCfg;

/* XTAL Type, will get from bootheader */
static uint8_t xtalType;

/* Flash Continuous Read Mode, will get from sf register */
static uint8_t flashContRead;


#define IS_ECO_VERSION             1

#define FLASH_CFG_OFFSET           (8+4)
#define XTAL_TYPE_OFFSET           (8+(4+sizeof(SPI_Flash_Cfg_Type)+4)+4)


void ATTR_TCM_SECTION bl_pds_init(void)
{
    // Get flash configuration and xtal type from bootheader
    __disable_irq();
    SFlash_Cache_Flush();  // Check map file. DO NOT locate between 0x42008000 - 0x42010000
    XIP_SFlash_Read_Via_Cache_Need_Lock(BL602_FLASH_XIP_BASE+FLASH_CFG_OFFSET,(uint8_t *)(&flashCfg),sizeof(SPI_Flash_Cfg_Type));
    XIP_SFlash_Read_Via_Cache_Need_Lock(BL602_FLASH_XIP_BASE+XTAL_TYPE_OFFSET,&xtalType,1);
    SFlash_Cache_Flush();
    __enable_irq();
    
    // Get whether flash continuous read mode configured
    flashContRead = *(volatile uint32_t *)0x4000B014 & (0x1<<27) ? 0 : 1;
    
    // Select XTAL32K (XTAL32K is default on)
#ifdef CFG_USE_XTAL32K
    HBN_32K_Sel(HBN_32K_XTAL);
#else
    HBN_32K_Sel(HBN_32K_RC);
#endif
    
#if 1
    // Set clock gating to unused peripherals
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_GLB);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_RF);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_GPIP_PHY_AGC);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_SEC_DBG);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_SEC);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_TZ1);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_TZ2);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_EFUSE);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_CCI);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_L1C);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_SFC);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_DMA);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_SDU);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_PDSHBN);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_WRAM);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_UART0);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_UART1);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_SPI);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_I2C);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_PWM);
    //GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_TMR);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_IRR);
    GLB_AHB_Slave1_Clock_Gate(1, BL_AHB_SLAVE1_CKS);
#endif
}

void bl_pds_fastboot_cfg(uint32_t addr)
{
    HBN_Set_Wakeup_Addr(addr);
    HBN_Set_Status_Flag(HBN_STATUS_ENTER_FLAG);
}

void ATTR_TCM_SECTION bl_pds_enter(uint32_t pdsLevel, uint32_t pdsSleepCycles)
{
    PDS_DEFAULT_LV_CFG_Type *pdsCfg;
    uint32_t pin;
    
    if(pdsLevel == 0){
        pdsCfg = &pdsCfgLevel0;
    }else if(pdsLevel == 1){
        pdsCfg = &pdsCfgLevel1;
    }else if(pdsLevel == 2){
        pdsCfg = &pdsCfgLevel2;
    }else if(pdsLevel == 3){
        pdsCfg = &pdsCfgLevel3;
    }else if(pdsLevel == 7){
        pdsCfg = &pdsCfgLevel7;
    }else{
        return;
    }
    
    // Disable global interrupt
    __disable_irq();
    
    // Disable GPIO7 pull up/down to reduce PDS current, 0x4000F014[16]=0
    HBN_Hw_Pu_Pd_Cfg(DISABLE);
    
    // Disable GPIO7 IE/SMT, 0x4000F014[8]=0
    HBN_Aon_Pad_IeSmt_Cfg(DISABLE);
    
    // Disable TRNG
#if IS_ECO_VERSION == 0
    SEC_Eng_Turn_On_Sec_Ring();
#else
    SEC_Eng_Turn_Off_Sec_Ring();
#endif
    Sec_Eng_Trng_Disable();
    
    // Power down flash
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    SFlash_Reset_Continue_Read(&flashCfg);
    SFlash_Powerdown();
    
    // Set internal flash pads (GPIO23 - GPIO28) in High-Z mode
    for(pin=23; pin<=28; pin++){
        GLB_GPIO_Set_HZ(pin);
    }
    
#if 1
    // Set all external pads (GPIO0 - GPIO22, uart pads excluded) in High-Z mode
    for(pin=0; pin<=22; pin++){
        if((pin == 7) || (pin == 16)){
            continue;
        }
        GLB_GPIO_Set_HZ(pin);
    }
#endif
    
    // Select RC32M
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
    PDS_Power_Off_PLL();
    
    // Enter PDS
    PDS_Default_Level_Config(pdsCfg, NULL, pdsSleepCycles);
    
    // Select PLL
    PDS_Power_On_PLL((PDS_PLL_XTAL_Type)xtalType);
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_PLL);
    
    // Initialize internal flash pads
    SF_Cfg_Init_Flash_Gpio(0, 1);
    
    // Power on flash
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    SFlash_Restore_From_Powerdown(&flashCfg, flashContRead);
    
    // Enable TRNG
#if IS_ECO_VERSION == 0
    SEC_Eng_Turn_Off_Sec_Ring();
#else
    SEC_Eng_Turn_On_Sec_Ring();
#endif
    Sec_Eng_Trng_Enable();
    
    // Enable GPIO7 IE/SMT, 0x4000F014[8]=1
    HBN_Aon_Pad_IeSmt_Cfg(ENABLE);
    
    // Enable GPIO7 pull up/down, 0x4000F014[16]=1
    HBN_Hw_Pu_Pd_Cfg(ENABLE);
    
    // Enable global interrupt
    __enable_irq();
}

int bl_pds_rf_turnoff(void *arg)
{
    AON_LowPower_Enter_PDS0();

    return 0;
}

int bl_pds_rf_turnon(void *arg)
{
    AON_LowPower_Exit_PDS0();

    return 0;
}
