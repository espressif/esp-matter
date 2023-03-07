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
#include "bl_pds.h"
#include "bl_irq.h"
#include "bl_flash.h"


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


#if !defined(CFG_PDS_OPTIMIZE)
/* PDS0 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel0 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* PDS1 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel1 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* PDS2 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel2 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* PDS3 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel3 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* PDS4 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel4 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 1,  // reset misc if bootrom_protect = 1
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* PDS5 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel5 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 1,  // reset misc if bootrom_protect = 1
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* PDS6 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel6 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 1,  // reset misc if bootrom_protect = 1
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* PDS7 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel7 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 0,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 1,  // reset misc if bootrom_protect = 1
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
#endif

/* PDS31 Configuration */
static const PDS_DEFAULT_LV_CFG_Type pdsCfgLevel31 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 1,
        .socEnbForceOn           = 0,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 1,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 1,
        .MiscRst                 = 1,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};

/* Cache Way Disable, will get from l1c register */
static uint8_t cacheWayDisable;

/* EM Select, will get from glb register */
static uint32_t emSel;

/* Device Information, will get from efuse */
static Efuse_Device_Info_Type devInfo;

/* PSRAM IO Configuration, will get according to device information */
static uint32_t psramIoCfg;

/* Clock Configuration, will get from bootheader */
static sys_clk_cfg_t clkCfg;

/* Flash Configuration Pointer, will get from bl_flash_get_flashCfg() */
static SPI_Flash_Cfg_Type *flashCfgPtr;

/* Flash Image Offset, will get from SF_Ctrl_Get_Flash_Image_Offset() */
static uint32_t flashImageOffset;

/* SF Control Configuration, will set based on flash configuration */
static SF_Ctrl_Cfg_Type sfCtrlCfg;

/* Wakeup Pin Configuration */
static uint8_t hbnWakeupPin = 0;
static int8_t pdsWakeupPin = -1;
static uint32_t glbWakeupPin = 0;

/* Wakeup Event, will get from pds register after wakeup */
static uint8_t wakeupEvent = 0;

/* Wakeup Pin, will get according to wakeup event */
static uint32_t wakeupPin = 0;

/* Backup 0x4202DFF4, which will be overwritten by bootrom */
static uint32_t patchBootrom = 0;

/* Flag whether cpu registers are stored or not */
static uint8_t cpuRegStored = 0;


static void bl_pds_set_sf_ctrl(SPI_Flash_Cfg_Type *pFlashCfg)
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

static void bl_pds_xtal_cfg(void)
{
#if 0
    uint32_t tmpVal;
    
    // optimize xtal ready time
#if !defined(CFG_PDS_OPTIMIZE)
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


void bl_pds_init(void)
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
    
    // Get flash configuration pointer
    flashCfgPtr = (SPI_Flash_Cfg_Type *)bl_flash_get_flashCfg();
    
    // Get flash image offset
    flashImageOffset = SF_Ctrl_Get_Flash_Image_Offset();
    
    // Set SF control configuration
    bl_pds_set_sf_ctrl(flashCfgPtr);
    
    // Optimize xtal configuration
    bl_pds_xtal_cfg();
    
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
    
    // Enable pds wakeup interrupt
    PDS_IntMask(PDS_INT_WAKEUP, UNMASK);
    PDS_IntMask(PDS_INT_RF_DONE, MASK);
    PDS_IntMask(PDS_INT_PLL_DONE, MASK);
    
    // Configure PDS_SLEEP_CNT as wakeup source
    PDS_IntEn(PDS_INT_PDS_SLEEP_CNT, ENABLE);
    PDS_IntEn(PDS_INT_HBN_IRQ_OUT0, DISABLE);
    PDS_IntEn(PDS_INT_HBN_IRQ_OUT1, DISABLE);
    PDS_IntEn(PDS_INT_GPIO_IRQ, DISABLE);
    PDS_IntEn(PDS_INT_IRRX, DISABLE);
    PDS_IntEn(PDS_INT_BLE_SLP_IRQ, DISABLE);
    PDS_IntEn(PDS_INT_USB_WKUP, DISABLE);
    PDS_IntEn(PDS_INT_KYS_QDEC, DISABLE);
}

void bl_pds_gpio_wakeup_cfg(uint8_t pin_list[], uint8_t pin_num)
{
    int i;
    int pin;
    
    hbnWakeupPin = 0;
    pdsWakeupPin = -1;
    
    for(i = 0; i < pin_num; i++){
        pin = pin_list[i];
        
        if(pin >= 9 && pin <= 12){
            GLB_GPIO_Set_HZ(pin);
            hbnWakeupPin |= 1 << (pin - 9);
        }
        
        if(pin >= 0 && pin <= 7){
            pdsWakeupPin = pin;
        }
    }
    
    if(hbnWakeupPin == 0){
        HBN_Aon_Pad_IeSmt_Cfg(0);
        HBN_Pin_WakeUp_Mask(0x1F);
        PDS_IntEn(PDS_INT_HBN_IRQ_OUT0, DISABLE);
    }else{
        HBN_Aon_Pad_IeSmt_Cfg(hbnWakeupPin);
        HBN_Pin_WakeUp_Mask(~hbnWakeupPin & 0x1F);
        HBN_GPIO_INT_Enable(HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE);
        PDS_IntEn(PDS_INT_HBN_IRQ_OUT0, ENABLE);
    }
    
    if(pdsWakeupPin == -1){
        PDS_Set_Vddcore_GPIO_IntMask(MASK);
        PDS_IntEn(PDS_INT_GPIO_IRQ, DISABLE);
    }else{
        GLB_GPIO_Set_HZ(pdsWakeupPin);
        GLB_GPIO_INPUT_Enable(pdsWakeupPin);
        PDS_Set_Vddcore_GPIO_IntCfg(pdsWakeupPin, PDS_AON_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE);
        PDS_Set_Vddcore_GPIO_IntMask(UNMASK);
        PDS_IntEn(PDS_INT_GPIO_IRQ, ENABLE);
    }
}

void bl_pds_gpio_wakeup_cfg_ex(uint32_t bitmap)
{
    int pin;
    GLB_GPIO_Cfg_Type gpioCfg;
    
    for(pin = 0; pin <= 31; pin++){
        if(bitmap & (1 << pin)){
            gpioCfg.gpioPin = pin;
            gpioCfg.gpioFun = 11;
            gpioCfg.gpioMode = GPIO_MODE_INPUT;
            gpioCfg.pullType = GPIO_PULL_NONE;
            gpioCfg.drive = 0;
            gpioCfg.smtCtrl = 1;
            GLB_GPIO_Init(&gpioCfg);
            
            GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_PULSE);
            GLB_GPIO_IntClear(pin, SET);
            GLB_GPIO_IntMask(pin, UNMASK);
        }else{
            GLB_GPIO_IntMask(pin, MASK);
        }
    }
    
    HBN_Aon_Pad_IeSmt_Cfg((bitmap >> 9) & 0x1F);
    
    if(bitmap == 0){
        PDS_IntEn(PDS_INT_GPIO_IRQ, DISABLE);
    }else{
        PDS_IntEn(PDS_INT_GPIO_IRQ, ENABLE);
    }
    
    glbWakeupPin = bitmap;
}

void bl_pds_fastboot_cfg(uint32_t addr)
{
    HBN_Set_Wakeup_Addr(addr);
    HBN_Set_Status_Flag(HBN_STATUS_ENTER_FLAG);
}


// must be placed in pds section
static void ATTR_PDS_SECTION bl_pds_restore_flash(SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t tmp[1];
    
    *(volatile uint32_t *)0x4000E030 = 0;
    
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

// can be placed in flash, here placed in pds section to reduce fast boot time
static void ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_restore_tcm(void)
{
#if !defined(CFG_PDS_OPTIMIZE)
    extern uint8_t _tcm_load;
    extern uint8_t _tcm_run;
    extern uint8_t _tcm_run_end;
    uint32_t src = (uint32_t)&_tcm_load;
    uint32_t dst = (uint32_t)&_tcm_run;
    uint32_t end = (uint32_t)&_tcm_run_end;
#else
    extern uint8_t _tcm_load;
    extern uint8_t _tcm_run;
    extern uint8_t _pds_restore_tcm_run_end;
    uint32_t src = (uint32_t)&_tcm_load;
    uint32_t dst = (uint32_t)&_tcm_run;
    uint32_t end = (uint32_t)&_pds_restore_tcm_run_end;
#endif
    
    while(dst < end){
        *(uint32_t *)dst = *(uint32_t *)src;
        src += 4;
        dst += 4;
    }
}

// can be placed in flash, here placed in pds section to reduce fast boot time
static void ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_set_gpio_high_z(void)
{
    int pin;
    uint32_t tmpVal;
    
    // Set all gpio pads in High-Z state (GPIO0 - GPIO31)
    for(pin = 0; pin <= 31; pin++){
#if !defined(CFG_PDS_OPTIMIZE)
        if(pin == 0 || pin == 1 || pin == 2 || pin == 9){
            continue;
        }
        if(pin == 14 || pin == 15){
            continue;
        }
#endif
        if(pin >= 23 && pin <= 28){
            continue;
        }
        if(pin == pdsWakeupPin){
            continue;
        }
        if(glbWakeupPin & (1 << pin)){
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
    if(((glbWakeupPin >> 23) & 0x3F) == 0){
        GLB_Set_Psram_Pad_HZ();
    }
    
    // Will be overwritten by GLB_GPIO_Set_HZ()
    if(hbnWakeupPin != 0){
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_PAD_IE_SMT, hbnWakeupPin);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    }
}

// can be placed in flash, here placed in pds section to reduce fast boot time
static void ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_restore_cpu_reg(void)
{
    __asm__ __volatile__(
            "la     a2,     __ld_pds_bak_addr\n\t"
            "lw     ra,     0(a2)\n\t"
            "lw     sp,     1*4(a2)\n\t"
            "lw     tp,     2*4(a2)\n\t"
            "lw     t0,     3*4(a2)\n\t"
            "lw     t1,     4*4(a2)\n\t"
            "lw     t2,     5*4(a2)\n\t"
            "lw     fp,     6*4(a2)\n\t"
            "lw     s1,     7*4(a2)\n\t"
            "lw     a0,     8*4(a2)\n\t"
            "lw     a1,     9*4(a2)\n\t"
            "lw     a3,     10*4(a2)\n\t"
            "lw     a4,     11*4(a2)\n\t"
            "lw     a5,     12*4(a2)\n\t"
            "lw     a6,     13*4(a2)\n\t"
            "lw     a7,     14*4(a2)\n\t"
            "lw     s2,     15*4(a2)\n\t"
            "lw     s3,     16*4(a2)\n\t"
            "lw     s4,     17*4(a2)\n\t"
            "lw     s5,     18*4(a2)\n\t"
            "lw     s6,     19*4(a2)\n\t"
            "lw     s7,     20*4(a2)\n\t"
            "lw     s8,     21*4(a2)\n\t"
            "lw     s9,     22*4(a2)\n\t"
            "lw     s10,    23*4(a2)\n\t"
            "lw     s11,    24*4(a2)\n\t"
            "lw     t3,     25*4(a2)\n\t"
            "lw     t4,     26*4(a2)\n\t"
            "lw     t5,     27*4(a2)\n\t"
            "lw     t6,     28*4(a2)\n\t"
            "csrw   mtvec,  a0\n\t"
            "csrw   mstatus,a1\n\t"
            "ret\n\t"
    );
}

// must be placed in pds section
static void ATTR_PDS_SECTION bl_pds_fastboot_entry(void)
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
    
#if !defined(CFG_PDS_OPTIMIZE)
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
    bl_pds_restore_flash(flashCfgPtr);
    
#if FAST_BOOT_TEST == 2
    *pOut |= (1<<pos);
    RomDriver_BL702_Delay_US(1);
    *pOut &= ~(1<<pos);
#endif
    
    // Restore tcm code
    bl_pds_restore_tcm();
    
#if FAST_BOOT_TEST == 2
    *pOut |= (1<<pos);
    RomDriver_BL702_Delay_US(1);
    *pOut &= ~(1<<pos);
#endif
    
#if defined(CFG_PDS_OPTIMIZE)
    // Set all gpio pads in High-Z state
    bl_pds_set_gpio_high_z();
    
    // Wait until XTAL32M is ready for use
    while(!BL_IS_REG_BIT_SET(BL_RD_REG(AON_BASE, AON_TSEN), AON_XTAL_RDY));
    RomDriver_BL702_Delay_MS(1);
    
    // Select XTAL32M as root clock
    RomDriver_HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);
    
    // Turn on RF
    *(volatile uint32_t *)(AON_BASE + AON_RF_TOP_AON_OFFSET) |= ((1<<0)|(1<<1)|(1<<2));
    
#if FAST_BOOT_TEST == 2
    *pOut |= (1<<pos);
    RomDriver_BL702_Delay_US(1);
    *pOut &= ~(1<<pos);
#endif
#endif
    
#if FAST_BOOT_TEST == 3
    ((void (*)(void))0x23000000)();
#endif
    
    // Restore EM select
    BL_WR_REG(GLB_BASE, GLB_SEAM_MISC, emSel);
    
    // Restore 0x4202DFF4, which will be overwritten by bootrom
    *(volatile uint32_t *)0x4202DFF4 = patchBootrom;
    
    // Disable global interrupt
    __disable_irq();
    
    // Set interrupt vector
    extern void freertos_risc_v_trap_handler(void);
    write_csr(mtvec, &freertos_risc_v_trap_handler);
    
    // Set cpuRegStored flag
    cpuRegStored = 1;
    
    // Call user callback
    bl_pds_fastboot_done_callback();
    
    // Clear cpuRegStored flag
    cpuRegStored = 0;
    
    // Restore cpu registers
    bl_pds_restore_cpu_reg();
}

// can be placed in flash
static void ATTR_NOINLINE bl_pds_gpio_status_clear(void)
{
    if(hbnWakeupPin != 0){
        BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0x1F);
    }
    
    if(pdsWakeupPin != -1){
        PDS_Set_Vddcore_GPIO_IntClear();
    }
    
    if(glbWakeupPin != 0){
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_CLR1, glbWakeupPin);
        while(BL_RD_REG(GLB_BASE, GLB_GPIO_INT_STAT1) & glbWakeupPin);
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_CLR1, 0);
    }
}

// can be placed in flash, here placed in pds section to reduce fast boot time
static int ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_pre_process_1(uint32_t pdsLevel, PDS_DEFAULT_LV_CFG_Type *pdsCfg, uint32_t *pdFlash, uint32_t *store)
{
    if(pdsLevel == 31){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel31, sizeof(PDS_DEFAULT_LV_CFG_Type));
#if !defined(CFG_PDS_OPTIMIZE)
    }else if(pdsLevel == 0){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel0, sizeof(PDS_DEFAULT_LV_CFG_Type));
    }else if(pdsLevel == 1){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel1, sizeof(PDS_DEFAULT_LV_CFG_Type));
    }else if(pdsLevel == 2){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel2, sizeof(PDS_DEFAULT_LV_CFG_Type));
    }else if(pdsLevel == 3){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel3, sizeof(PDS_DEFAULT_LV_CFG_Type));
    }else if(pdsLevel == 4){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel4, sizeof(PDS_DEFAULT_LV_CFG_Type));
    }else if(pdsLevel == 5){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel5, sizeof(PDS_DEFAULT_LV_CFG_Type));
    }else if(pdsLevel == 6){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel6, sizeof(PDS_DEFAULT_LV_CFG_Type));
    }else if(pdsLevel == 7){
        ARCH_MemCpy_Fast(pdsCfg, &pdsCfgLevel7, sizeof(PDS_DEFAULT_LV_CFG_Type));
#endif
    }else{
        return -1;
    }
    
    __disable_irq();
    
#if defined(CFG_PDS_OPTIMIZE)
    // Turn off RF
    *(volatile uint32_t *)(AON_BASE + AON_RF_TOP_AON_OFFSET) &= ~(uint32_t)((1<<0)|(1<<1)|(1<<2));
#endif
    
    bl_pds_gpio_status_clear();
    
#if !defined(CFG_PDS_OPTIMIZE)
    if(pdsLevel == 31){
        HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_10_100UA);
    }else{
        HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_25_250UA);
    }
#endif
    
    if(devInfo.flash_cfg == 1 || devInfo.flash_cfg == 2){
        pdsCfg->pdsCtl.puFlash = 1;    // Power down internal flash in pds31
        pdsCfg->pdsCtl.swPuFlash = 1;  // Don't power down internal flash in pds0 - pds7
        
        if(pdsLevel == 31){
            *pdFlash = 1;
        }else{
            *pdFlash = 0;
        }
    }else{
        *pdFlash = 0;
    }
    
    if(pdsLevel >= 4 && HBN_Get_Status_Flag() != HBN_STATUS_ENTER_FLAG){
        HBN_Set_Wakeup_Addr((uint32_t)bl_pds_fastboot_entry);
        HBN_Set_Status_Flag(HBN_STATUS_ENTER_FLAG);
        patchBootrom = *(volatile uint32_t *)0x4202DFF4;
        *store = !cpuRegStored;
    }else{
        *store = 0;
    }
    
    if(pdsCfg->pdsCtl.cpu0WfiMask == 0){
        PDS_IntClear();
        bl_irq_enable(PDS_WAKEUP_IRQn);
    }
    
#if !defined(CFG_PDS_OPTIMIZE)
    bl_pds_set_gpio_high_z();
#endif
    
    return 0;
}

// can be placed in tcm section, here placed in pds section to reduce fast boot time
static void ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_pre_process_2(uint32_t pdFlash)
{
    // Power down flash
    if(!pdFlash){
        RomDriver_SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        RomDriver_SFlash_Reset_Continue_Read(flashCfgPtr);
        RomDriver_SFlash_Powerdown();
    }
    
    // Pull up flash pads
    if(devInfo.flash_cfg == 0){
        // External SF2 (GPIO23 - GPIO28)
        *(volatile uint32_t *)0x4000E030 = (0x1F << 24);  // As boot pin, GPIO28 should not be pulled up
    }else if(devInfo.flash_cfg == 3){
        // External SF1 (GPIO17 - GPIO22)
        *(volatile uint32_t *)0x4000E030 = (0x3F << 8);
    }else{
        // Internal SF2 (GPIO23 - GPIO28)
        // Do nothing
    }
    
    // Set all flash pads in High-Z state
    GLB_Set_Flash_Pad_HZ();
    
    // Select RC32M
    RomDriver_HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
    RomDriver_GLB_Power_Off_DLL();
    RomDriver_AON_Power_Off_XTAL();
}

// can be placed in tcm section, here placed in pds section to reduce fast boot time
static void ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_enter_do(PDS_DEFAULT_LV_CFG_Type *pdsCfg, uint32_t pdsSleepCycles, uint32_t store)
{
    if(store){
        __asm__ __volatile__(
            "la     a2,     __ld_pds_bak_addr\n\t"
            "sw     ra,     0(a2)\n\t"
        );
    }
    
    PDS_Default_Level_Config(pdsCfg, pdsSleepCycles);
    if(pdsCfg->pdsCtl.cpu0WfiMask == 0){
        __WFI();
    }
}

// can be placed in tcm section, here placed in pds section to reduce fast boot time
static void ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_post_process_1(uint32_t pdsLevel, uint32_t pdFlash)
{
#if !defined(CFG_PDS_OPTIMIZE)
    // For pdsLevel >=4, clock and flash will be configured in fast boot entry
    if(pdsLevel < 4){
        // Select DLL or XTAL32M
        AON_Power_On_XTAL();
        BL702_Delay_MS(1);  // actually xtal may be not ready when AON_XTAL_RDY becomes 1
        GLB_Power_On_DLL(GLB_DLL_XTAL_32M);
        if(clkCfg.pll_clk >= 2){
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_DLL);
        }else{
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);
        }
        
        // Clear flash pads pull configuration
        *(volatile uint32_t *)0x4000E030 = 0;
        
        // Power up flash
        if(!pdFlash){
            SF_Cfg_Init_Flash_Gpio((devInfo.flash_cfg<<2)|devInfo.sf_swap_cfg, 1);
            SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
            SFlash_Restore_From_Powerdown(flashCfgPtr, flashCfgPtr->cReadSupport);
        }
    }
#endif
}

// can be placed in flash, here placed in pds section to reduce fast boot time
static void ATTR_NOINLINE ATTR_PDS_SECTION bl_pds_post_process_2(void)
{
#if !defined(CFG_PDS_OPTIMIZE)
    uint32_t tmpVal;
    uint32_t hbnGpioStatus;
    uint32_t pdsGpioStatus;
    uint32_t glbGpioStatus;
    
    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    wakeupEvent = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_WAKEUP_EVENT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);
    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);
    
    wakeupPin = 0;
    
    if(wakeupEvent & 0x02){
        hbnGpioStatus = BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & 0x1F;
        wakeupPin |= hbnGpioStatus << 9;
    }
    
    if(wakeupEvent & 0x08){
        pdsGpioStatus = BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_GPIO_INT), PDS_GPIO_INT_STAT);
        if(pdsGpioStatus != 0){
            wakeupPin |= 1 << pdsWakeupPin;
        }
        
        glbGpioStatus = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_STAT1);
        wakeupPin |= glbGpioStatus;
    }
#endif
    
    __enable_irq();
}


// can be placed in tcm section, here placed in pds section to reduce fast boot time
void ATTR_PDS_SECTION bl_pds_enter(uint32_t pdsLevel, uint32_t pdsSleepCycles)
{
    PDS_DEFAULT_LV_CFG_Type pdsCfg;
    uint32_t pdFlash;
    uint32_t store;
    
    // Pre-process
    if(bl_pds_pre_process_1(pdsLevel, &pdsCfg, &pdFlash, &store) != 0){
        return;
    }
    bl_pds_pre_process_2(pdFlash);
    
    // Store cpu registers except for ra
    if(store){
        __asm__ __volatile__(
            "csrr   a0,     mtvec\n\t"
            "csrr   a1,     mstatus\n\t"
            "la     a2,     __ld_pds_bak_addr\n\t"
            "sw     sp,     1*4(a2)\n\t"
            "sw     tp,     2*4(a2)\n\t"
            "sw     t0,     3*4(a2)\n\t"
            "sw     t1,     4*4(a2)\n\t"
            "sw     t2,     5*4(a2)\n\t"
            "sw     fp,     6*4(a2)\n\t"
            "sw     s1,     7*4(a2)\n\t"
            "sw     a0,     8*4(a2)\n\t"
            "sw     a1,     9*4(a2)\n\t"
            "sw     a3,     10*4(a2)\n\t"
            "sw     a4,     11*4(a2)\n\t"
            "sw     a5,     12*4(a2)\n\t"
            "sw     a6,     13*4(a2)\n\t"
            "sw     a7,     14*4(a2)\n\t"
            "sw     s2,     15*4(a2)\n\t"
            "sw     s3,     16*4(a2)\n\t"
            "sw     s4,     17*4(a2)\n\t"
            "sw     s5,     18*4(a2)\n\t"
            "sw     s6,     19*4(a2)\n\t"
            "sw     s7,     20*4(a2)\n\t"
            "sw     s8,     21*4(a2)\n\t"
            "sw     s9,     22*4(a2)\n\t"
            "sw     s10,    23*4(a2)\n\t"
            "sw     s11,    24*4(a2)\n\t"
            "sw     t3,     25*4(a2)\n\t"
            "sw     t4,     26*4(a2)\n\t"
            "sw     t5,     27*4(a2)\n\t"
            "sw     t6,     28*4(a2)\n\t"
        );
    }
    
    // Enter PDS
    bl_pds_enter_do(&pdsCfg, pdsSleepCycles, store);
    
    // Post-process
    bl_pds_post_process_1(pdsLevel, pdFlash);
    bl_pds_post_process_2();
}

int bl_pds_get_wakeup_source(void)
{
    if(wakeupEvent & 0x01){
        return PDS_WAKEUP_BY_SLEEP_CNT;
    }else if(wakeupEvent & 0x0A){
        return PDS_WAKEUP_BY_GPIO;
    }else{
        return 0;
    }
}

uint32_t bl_pds_get_wakeup_gpio(void)
{
    return wakeupPin;
}

__attribute__((weak)) void ATTR_PDS_SECTION bl_pds_fastboot_done_callback(void)
{
    
}
