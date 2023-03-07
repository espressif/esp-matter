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

#include "bl702_psram.h"
#include "bl702_l1c.h"
#include "bl702_glb.h"
#include "bflb_platform.h"

/** @addtogroup  BL702_Peripheral_Case
 *  @{
 */

/** @addtogroup  DUAL_CS_PSRAM_WRITE_THROUGH
 *  @{
 */

/** @defgroup  DUAL_CS_PSRAM_WRITE_THROUGH_Private_Macros
 *  @{
 */
#define PSRAM_ADDR                             0x24000000
#define PSRAM_SIZE                             0x200000
#define BFLB_EXTFLASH_CS_GPIO                  GLB_GPIO_PIN_25
#define BFLB_EXTPSRAM_CLK_GPIO                 GLB_GPIO_PIN_27
#define BFLB_EXTPSRAM_CS_GPIO                  GLB_GPIO_PIN_17
#define BFLB_EXTPSRAM_DATA0_GPIO               GLB_GPIO_PIN_28
#define BFLB_EXTPSRAM_DATA1_GPIO               GLB_GPIO_PIN_24
#define BFLB_EXTPSRAM_DATA2_GPIO               GLB_GPIO_PIN_23
#define BFLB_EXTPSRAM_DATA3_GPIO               GLB_GPIO_PIN_26

/*@} end of group DUAL_CS_PSRAM_WRITE_THROUGH_Private_Macros */

/** @defgroup  DUAL_CS_PSRAM_WRITE_THROUGH_Private_Types
 *  @{
 */

/*@} end of group DUAL_CS_PSRAM_WRITE_THROUGH_Private_Types */

/** @defgroup  DUAL_CS_PSRAM_WRITE_THROUGH_Private_Variables
 *  @{
 */
uint8_t psramId[8] = {0};
uint32_t checkAddr = 0;

/*@} end of group DUAL_CS_PSRAM_WRITE_THROUGH_Private_Variables */

/** @defgroup  DUAL_CS_PSRAM_WRITE_THROUGH_Global_Variables
 *  @{
 */

/*@} end of group DUAL_CS_PSRAM_WRITE_THROUGH_Global_Variables */

/** @defgroup  DUAL_CS_PSRAM_WRITE_THROUGH_Private_Fun_Declaration
 *  @{
 */

/*@} end of group DUAL_CS_PSRAM_WRITE_THROUGH_Private_Fun_Declaration */

/** @defgroup  DUAL_CS_PSRAM_WRITE_THROUGH_Private_Functions_User_Define
 *  @{
 */

/*@} end of group DUAL_CS_PSRAM_WRITE_THROUGH_Private_Functions_User_Define */

/** @defgroup  DUAL_CS_PSRAM_WRITE_THROUGH_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief  psram ini gpio func
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION SF_Cfg_Init_Ext_Psram_Gpio(void)
{
#if !defined(CFG_PSRAM_DUAL_BANK)
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[7];
    uint8_t i = 0;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 3;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_FLASH_PSRAM;

    gpiopins[0] = BFLB_EXTPSRAM_CLK_GPIO;
    gpiopins[1] = BFLB_EXTPSRAM_CS_GPIO;
    gpiopins[2] = BFLB_EXTPSRAM_DATA0_GPIO;
    gpiopins[3] = BFLB_EXTPSRAM_DATA1_GPIO;
    gpiopins[4] = BFLB_EXTPSRAM_DATA2_GPIO;
    gpiopins[5] = BFLB_EXTPSRAM_DATA3_GPIO;
    gpiopins[6] = BFLB_EXTFLASH_CS_GPIO;

    for(i=0; i<sizeof(gpiopins); i++){
        cfg.gpioPin=gpiopins[i];
        if(i==0 || i==1 || i==6){
            /*flash clk and cs is output*/
            cfg.gpioMode = GPIO_MODE_OUTPUT;
        }else{
            /*data are bidir*/
            cfg.gpioMode = GPIO_MODE_AF;
        }
        GLB_GPIO_Init(&cfg);
    }
#else
    // for dual bank mode, only need to select internal psram
    // please check map file, make sure that GLB_Set_Psram_Pad_HZ() is not called
    GLB_Select_Internal_PSram();
#endif
}

/****************************************************************************//**
 * @brief  psram init func
 *
 * @param  doReset: reset psram select
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bl_psram_init(void)
{
    SPI_Psram_Cfg_Type apMemory1604 = {
        .readIdCmd = 0x9F,
        .readIdDmyClk = 0,
        .burstToggleCmd = 0xC0,
        .resetEnableCmd = 0x66,
        .resetCmd = 0x99,
        .enterQuadModeCmd = 0x35,
        .exitQuadModeCmd = 0xF5,
        .readRegCmd = 0xB5,
        .readRegDmyClk = 1,
        .writeRegCmd = 0xB1,
        .readCmd = 0x03,
        .readDmyClk = 0,
        .fReadCmd = 0x0B,
        .fReadDmyClk = 1,
        .fReadQuadCmd = 0xEB,
        .fReadQuadDmyClk = 3,
        .writeCmd = 0x02,
        .quadWriteCmd = 0x38,
        .pageSize = 512,
        .ctrlMode = PSRAM_SPI_CTRL_MODE,
        .driveStrength = PSRAM_DRIVE_STRENGTH_50_OHMS,
        .burstLength = PSRAM_BURST_LENGTH_512_BYTES,
    };
    SF_Ctrl_Cmds_Cfg cmdsCfg = {
        .cmdsEn = ENABLE,
        .burstToggleEn = ENABLE,
        .wrapModeEn = DISABLE,
        .wrapLen = SF_CTRL_WRAP_LEN_512,
    };
    SF_Ctrl_Psram_Cfg sfCtrlPsramCfg = {
        .owner = SF_CTRL_OWNER_SAHB,
#if !defined(CFG_PSRAM_DUAL_BANK)
        .padSel = SF_CTRL_PAD_SEL_DUAL_CS_SF2,
#else
        .padSel = SF_CTRL_PAD_SEL_DUAL_BANK_SF2_SF3,
#endif
        .bankSel = SF_CTRL_SEL_PSRAM,
        .psramRxClkInvertSrc = ENABLE,
        .psramRxClkInvertSel = ENABLE,
        .psramDelaySrc = ENABLE,
        .psramClkDelay = 0,
    };

    //GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_72M, 1);
    SF_Cfg_Init_Ext_Psram_Gpio();

    Psram_Init(&apMemory1604, &cmdsCfg, &sfCtrlPsramCfg);
    //if(doReset){
        Psram_SoftwareReset(&apMemory1604, apMemory1604.ctrlMode);
    //}
    Psram_ReadId(&apMemory1604, psramId);
    SF_Ctrl_Select_Bank(SF_CTRL_SEL_FLASH);

    // enable wt mode only
    Psram_Cache_Write_Set(&apMemory1604, SF_CTRL_QIO_MODE, ENABLE, DISABLE, DISABLE);
    //L1C_Cache_Enable_Set(L1C_WAY_DISABLE_NONE);

    // set burst toggle to spi mode, fix psram random access issue
    BL_WR_WORD(0x4000B084, 0x08000000);

    extern uint8_t psramId[8];
    //printf("PSRAM ID: %02X %02X %02X %02X %02X %02X %02X %02X.\r\n",
    //            psramId[0], psramId[1], psramId[2], psramId[3], psramId[4], psramId[5], psramId[6], psramId[7]);

}
