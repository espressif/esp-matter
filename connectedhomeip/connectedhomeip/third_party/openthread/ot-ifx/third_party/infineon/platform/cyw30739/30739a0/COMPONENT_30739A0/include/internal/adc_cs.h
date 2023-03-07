/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/*
********************************************************************
*    File Name: adc.h
*
*    Abstract: This file defines the ADC chip specific data struct
*
*
********************************************************************
*/

#ifndef __ADC_CS__H__
#define __ADC_CS__H__

#include "brcm_fw_types.h"
//#include "legacy_interface/inc/types.h"
#include "arm_pl081.h"

/**  \addtogroup ADC
 *  \ingroup HardwareDrivers
*/

#define ADC_DMA_FIFO_FULL       (0x1 << 16)
#define ADC_DMA_FIFO_ERROR      (0x1 << 17)
#define ADC_DMA_REQUEST_COUNT   0xFFFF

#define ADC_AUDIO_FIFO_NUM      5

#define ADC_AUDIO_FIFO_SIZE     240

#define ADC_AUDIO_DMA_FIFO_ADDR adc_dma_data_adr
#ifdef ADC_AUDIO_2_CHANNEL_SUPPORT
#define ADC_AUDIO_DMA_FIFO2_ADDR adc_dma_data_ch2_adr
#endif

#ifdef ADC_AUDIO_SUPPORT
typedef struct
{
    DMA_LLI_t   *dmaLli;
    UINT8       *audioFifo;
    UINT16       fifoSize;
    UINT16       dataSize;
}AdcAudioFifo;
#endif

typedef enum
{
    ADC_AUDIO_FIFO_OVERLAP  = 0x1,
    ADC_AUDIO_FIFO_FULL     = 0x2
} ADC_AUDIO_FIFO_STATUS;

/// supported ADC input channel selection
/*
If you choose LHL P36 as battery input pin and you use ADC_INPUT_P36 as input channel
*/
/// supported ADC input channel selection
typedef enum ADC_INPUT_CHANNEL_SEL {
    ADC_INPUT_P17           =   0x0,    // ADC CHANNEL #1 on GPIO P17
    ADC_INPUT_P16           =   0x1,    // ADC CHANNEL #2 on GPIO P16
    ADC_INPUT_P15           =   0x2,    // ADC CHANNEL #3 on GPIO P15
    ADC_INPUT_P14           =   0x3,    // ADC CHANNEL #4 on GPIO P14
    ADC_INPUT_P13           =   0x4,    // ADC CHANNEL #5 on GPIO P13
    ADC_INPUT_P12           =   0x5,    // ADC CHANNEL #6 on GPIO P12
    ADC_INPUT_P11           =   0x6,    // ADC CHANNEL #7 on GPIO P11
    ADC_INPUT_P10           =   0x7,    // ADC CHANNEL #8 on GPIO P10
    ADC_INPUT_P9            =   0x8,    // ADC CHANNEL #9 on GPIO P9
    ADC_INPUT_P8            =   0x9,    // ADC CHANNEL #10 on GPIO P8
    ADC_INPUT_P1            =   0xA,    // ADC CHANNEL #11 on GPIO P1
    ADC_INPUT_P0            =   0xB,    // ADC CHANNEL #12 on GPIO P0
    ADC_INPUT_VDDIO         =   0xC,    // ADC_INPUT_VBAT_VDDIO on Channel 13
    ADC_INPUT_VDD_CORE      =   0xD,    // ADC_INPUT_VDDC on Channel 14
    ADC_INPUT_ADC_BGREF     =   0xE,    // ADC BANDGAP REF on Channel 15
    ADC_INPUT_ADC_REFGND    =   0xF,    // ADC REF GND on Channel 16
    ADC_INPUT_P38           =   0x10,   // ADC CHANNEL #17  on GPIO P38
    ADC_INPUT_P37           =   0x11,   // ADC CHANNEL #18  on GPIO P37
    ADC_INPUT_P36           =   0x12,   // ADC CHANNEL #19  on GPIO P36
    ADC_INPUT_P35           =   0x13,   // ADC CHANNEL #20  on GPIO P35
    ADC_INPUT_P34           =   0x14,   // ADC CHANNEL #21  on GPIO P34
    ADC_INPUT_P33           =   0x15,   // ADC CHANNEL #22  on GPIO P33
    ADC_INPUT_P32           =   0x16,   // ADC CHANNEL #23  on GPIO P32
    ADC_INPUT_P31           =   0x17,   // ADC CHANNEL #24  on GPIO P31
    ADC_INPUT_P30           =   0x18,   // ADC CHANNEL #25  on GPIO P30
    ADC_INPUT_P29           =   0x19,   // ADC CHANNEL #26  on GPIO P29
    ADC_INPUT_P28           =   0x1A,   // ADC CHANNEL #27  on GPIO P28
    ADC_INPUT_P23           =   0x1B,   // ADC CHANNEL #28  on GPIO P23
    ADC_INPUT_P22           =   0x1C,   // ADC CHANNEL #29  on GPIO P22
    ADC_INPUT_P21           =   0x1D,   // ADC CHANNEL #30  on GPIO P21
    ADC_INPUT_P19           =   0x1E,   // ADC CHANNEL #31  on GPIO P19
    ADC_INPUT_P18           =   0x1F,   // ADC CHANNEL #32  on GPIO P18
    ADC_INPUT_CHANNEL_MASK  =   0x1f,
}ADC_INPUT_CHANNEL_SEL;

/// input  voltage range selection
typedef enum ADC_INPUT_RANGE_SEL
{
    ADC_RANGE_0_3P6V = 0,
    ADC_RANGE_0_1P8V = 1,
}ADC_INPUT_RANGE_SEL;


/// ADC power type
typedef enum ADC_POWER_TYPE
{
    ADC_POWER_DOWN          = 1,
    ADC_POWER_UP            = 0,
}ADC_POWER_TYPE;


///
/// lhl_adc_ctl_adr register
///
typedef union LHL_ADC_CTL_REG
{
    /// 32 bits regsiters
    UINT32 lhl_ctl_reg;

    struct
    {
        /// reserved 6:0
        UINT32 reserved0            : 7;

        /// bit 7   -   power down
        UINT32 powerDown            : 1;                // bit 7

        /// reserved 31:8
        UINT32 reserved1            : 24;               // bit 31:8
    }bitmap;
}LHL_ADC_CTL_REG;


///
/// MIA ADC Control 0 Register
/// - mia_adc_ctl0_adr
///
typedef union ADC_CTL0_REG
{
    UINT32 adc_ctl0_reg;
    struct
    {
        /// bit 0       -   ADC IP level main power up/down control:
        /// 0: power down whole ADC IP (Default); 1: power up
        UINT32 adcPwrup                 : 1;

        /// bit 1       -   ADC BG & REF power up/down:
        /// 0: power down BG and REF (Default); 1: power up
        UINT32 adcRefPwrup              : 1;

        /// bit 2       -   ADC core power up/down:
        /// 0: power down ADC Core (Default); 1: power up
        UINT32 adcCorePwrup             : 1;

        /// bit 3       -   ADC MIC (Audio) path power up/down control:
        /// 0: power down MIC bias and PGA (Default); 1: power up
        UINT32 micPwrup                 : 1;

        /// bit 4       -   MIC bias power up/down:
        /// 0: power down MIC Bias(Default); 1: Power up
        UINT32 micBiasPwrup             : 1;

        /// bit 5       -   Output synchronization clock phase control:
        /// 0: non-inverted clock phase (default); 1: inverted clock phase
        UINT32 adcSynClkPhase           : 1;

        /// bit 6       -   MIC (Audio) input and DC measurement input selection:
        /// 0: ADC in DC Measurement Mode; DC measurement path enabled; MIC path powered down
        /// 1: ADC in Audio application Mode; MIC path enabled. DC input is gated
        UINT32 adcMode                  : 1;

        /// bit 7       -   MIC path power up/down control selection in DC measurement mode:
        /// 0: Power down MIC Bias and PGA in DC measurement Mode
        /// 1: MIC path power up/down controlled by its pwrdn signal
        UINT32 adcMicPdslt              : 1;

        /// bit 8       -   Chip global power down control:
        /// 0: power up (Default); 1: power down
        UINT32  adcIddq                 : 1;

        /// bit 11:9    -   BG PTAT current adjustment:
        UINT32  adcBgPtatCtl            : 3;

        /// bit 14:12    -   BG CTAT current adjustment:
        UINT32  adcBgCtatCtl            : 3;

        /// bit 18:15    -   ADC reference voltage programmability:
        UINT32  adcRefCtl               : 4;

        /// bit 20:19    -   Switch cap filter clock frequency selection:
        UINT32  adcScfClkDiv            : 2;

        /// bit 21          -   BG REF switch cap filter bypass
        /// 0: enable switch cap filter; 1: bypass switch cap filter
        UINT32  ADC_SCF_bypass          : 1;

        /// bit 22          -   External power up sequence for BG SCF bypass
        UINT32  adcScfBypassSeq         : 1;

        /// bit 23          -   Internal/External BG SCF power up sequence selection:
        /// 0: use internal power up sequence; 1: use external power up sequence
        UINT32  adcScfSeqSlt            : 1;

        /// bit 25:24   -   Global bias current programmability
        UINT32  adcBiasCtl              : 2;

        /// reserved bits 28:26
        UINT32 reserved0                : 3;

        /// bit 29          -   ADC input range selection for DC measurement path:
        /// 0: 0-3.6V; 1: 0-1.8V
        UINT32  adcDcInputRange         : 1;

        /// bit 30          -   ADC internal clock division:
        /// 0: disable divide-by-2; ADC sampling clock at 12MHz; 1: enable divide-by-2; ADC sampling clock at 6MHz
        UINT32  adcClkDiv2              : 1;

        /// reserved bit 31
        UINT32 reserved1                : 1;
    }bitmap;
}ADC_CTL0_REG;


enum
{
    // bit 30
    CTL0_REG_ADC_CLK_DIV2_DISABLE               =   0x0,
    CTL0_REG_ADC_CLK_DIV2_ENABLE                =   0x1,

    CTL0_REG_ADC_CLK_DIV2_DEFAULT               =   CTL0_REG_ADC_CLK_DIV2_DISABLE ,

    // bit 29
    CTL0_REG_ADC_DC_INPUT_RANGE_0_3p6V          =   0x0,
    CTL0_REG_ADC_DC_INPUT_RANGE_0_1p8V          =   0x1,

    CTL0_REG_ADC_DC_INPUT_RANGE_DEFAULT         =   CTL0_REG_ADC_DC_INPUT_RANGE_0_3p6V ,

    // bit 25:24
    CTL0_REG_ADC_BIAS_CURR_3uA                  =   0x2,
//    CTL0_REG_ADC_BIAS_CURR_2p5uA                =   0x3,
    CTL0_REG_ADC_BIAS_CURR_2p5uA                =   0x0,
    CTL0_REG_ADC_BIAS_CURR_2uA                  =   0x1,

    CTL0_REG_ADC_BIAS_CURR_DEFAULT              =   CTL0_REG_ADC_BIAS_CURR_2p5uA ,

    // bit 23
    CTL0_REG_ADC_SCF_SEQ_SELECT_INT             =   0x0,
    CTL0_REG_ADC_SCF_SEQ_SELECT_EXT             =   0x1,

    CTL0_REG_ADC_SCF_SEQ_SELECT_DEFAULT         =   CTL0_REG_ADC_SCF_SEQ_SELECT_INT ,

    // bit 22
    CTL0_REG_ADC_SCF_BYPASS_SEQ_DEFAULT         =   0 ,

    // bit 21
    CTL0_REG_ADC_SCF_ENABLE                     =   0x0,
    CTL0_REG_ADC_SCF_BYPASS                     =   0x1,

    CTL0_REG_ADC_SCF_BYPASS_DEFAULT             =   CTL0_REG_ADC_SCF_ENABLE ,

    // bit 20:19
    CTL0_REG_ADC_SCF_CLK_DIV_50KHz              =   0x0,
    CTL0_REG_ADC_SCF_CLK_DIV_25KHz              =   0x1,
    CTL0_REG_ADC_SCF_CLK_DIV_100KHz             =   0x2,
    CTL0_REG_ADC_SCF_CLK_DIV_1p6MHz             =   0x3,

    CTL0_REG_ADC_SCF_CLK_DIV_DEFAULT            =   CTL0_REG_ADC_SCF_CLK_DIV_50KHz ,

    // bit 18:15
    CTL0_REG_ADC_REF_VOL_850p0                  =   0x0,
    CTL0_REG_ADC_REF_VOL_856p1                  =   0x1,
    CTL0_REG_ADC_REF_VOL_862p1                  =   0x2,
    CTL0_REG_ADC_REF_VOL_868p2                  =   0x3,
    CTL0_REG_ADC_REF_VOL_874p3                  =   0x4,
    CTL0_REG_ADC_REF_VOL_880p4                  =   0x5,
    CTL0_REG_ADC_REF_VOL_886p4                  =   0x6,
    CTL0_REG_ADC_REF_VOL_892p5                  =   0x7,
    CTL0_REG_ADC_REF_VOL_801p4                  =   0x8,
    CTL0_REG_ADC_REF_VOL_807p5                  =   0x9,
    CTL0_REG_ADC_REF_VOL_813p6                  =   0xA,
    CTL0_REG_ADC_REF_VOL_819p6                  =   0xB,
    CTL0_REG_ADC_REF_VOL_825p7                  =   0xC,
    CTL0_REG_ADC_REF_VOL_831p8                  =   0xD,
    CTL0_REG_ADC_REF_VOL_837p9                  =   0xE,
    CTL0_REG_ADC_REF_VOL_843p9                  =   0xF,

    CTL0_REG_ADC_REF_VOL_DEFAULT                =   CTL0_REG_ADC_REF_VOL_850p0 ,

    // bit 14:12
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_3p02          =   0x3,
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_1p92          =   0x2,
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_0p92          =   0x1,
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_0             =   0x0,
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_N0p93         =   0x7,
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_N1p82         =   0x6,
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_N2p68         =   0x5,
    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_N3p51         =   0x4,

    CTL0_REG_ADC_BG_CTAT_CURR_ADJ_DEFAULT       =   CTL0_REG_ADC_BG_CTAT_CURR_ADJ_0 ,

    // bit 11:9
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_3p62          =   0x3,
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_2p32          =   0x2,
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_1p11          =   0x1,
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_0             =   0x0,
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_N1p11         =   0x7,
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_N2p17         =   0x6,
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_N3p19         =   0x5,
    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_N4p10         =   0x4,

    CTL0_REG_ADC_BG_PTAT_CURR_ADJ_DEFAULT       =   CTL0_REG_ADC_BG_PTAT_CURR_ADJ_0 ,

    // bit 8
    CTL0_REG_ADC_IDDQ_POWER_UP                  =   0x0,
    CTL0_REG_ADC_IDDQ_POWER_DOWN                =   0x1,

    CTL0_REG_ADC_IDDQ_DEFAULT                   =   CTL0_REG_ADC_IDDQ_POWER_UP ,

    // bit 7
    CTL0_REG_ADC_MIC_POWER_DOWN_IN_DC           =   0 ,
    CTL0_REG_ADC_MIC_POWER_DOWN_BY_SIGNAL       =   1 ,

    CTL0_REG_ADC_MIC_POWER_SELECT_DEFAULT       =   CTL0_REG_ADC_MIC_POWER_DOWN_IN_DC ,

    // bit 6
    CTL0_REG_ADC_MODE_DC                        =   0x0,
    CTL0_REG_ADC_MODE_AUDIO                     =   0x1,

    CTL0_REG_ADC_MODE_DEFAULT                   =   CTL0_REG_ADC_MODE_DC ,

    // bit 5
    CTL0_REG_ADC_SYN_CLK_PHASE_NON_INVERT       =   0x0,
    CTL0_REG_ADC_SYN_CLK_PHASE_INVERT           =   0x1,

    CTL0_REG_ADC_SYN_CLK_PHASE_DEFAULT          =   CTL0_REG_ADC_SYN_CLK_PHASE_NON_INVERT ,

    // bit 4
    CTL0_REG_ADC_MIC_BIAS_POWER_DOWN            =   0x0,
    CTL0_REG_ADC_MIC_BIAS_POWER_UP              =   0x1,

    CTL0_REG_ADC_MIC_BIAS_POWER_DEFAULT         =   CTL0_REG_ADC_MIC_BIAS_POWER_DOWN ,

    // bit 3
    CTL0_REG_ADC_MIC_POWER_DOWN                 =   0x0,
    CTL0_REG_ADC_MIC_POWER_UP                   =   0x1,

    CTL0_REG_ADC_MIC_POWER_DEFAULT              =   CTL0_REG_ADC_MIC_POWER_DOWN ,

    // bit 2
    CTL0_REG_ADC_CORE_POWER_DOWN                =   0x0,
    CTL0_REG_ADC_CORE_POWER_UP                  =   0x1,

    CTL0_REG_ADC_CORE_POWER_DEFAULT             =   CTL0_REG_ADC_CORE_POWER_DOWN ,

    // bit 1
    CTL0_REG_ADC_REF_POWER_DOWN                 =   0x0,
    CTL0_REG_ADC_REF_POWER_UP                   =   0x1,

    CTL0_REG_ADC_REF_POWER_DEFAULT              =   CTL0_REG_ADC_REF_POWER_DOWN ,

    // bit 0
    CTL0_REG_ADC_POWER_DOWN                     =   0,     // bit 1
    CTL0_REG_ADC_POWER_UP                       =   1,

    CTL0_REG_ADC_POWER_DEFAULT                  =   CTL0_REG_ADC_POWER_DOWN ,

};


///
/// MIA ADC Control 1 Register
/// - mia_adc_ctl1_adr
///
typedef union ADC_CTL1_REG
{
    UINT32 adc_ctl1_reg;
    struct
    {
        /// bit 0:1     -   MIC bias reference voltage programmability:
        ///00: 75% of Audio Supply (default); 10: 80% of Audio Supply; 01 / 11: 70% of Audio Supply
        UINT32 micBiasRefCtl            : 2;

        /// bit 3:2     -   MIC bias output voltage programmability:
        /// 00: 1.12 X Reference Voltage (default); 01: 1.14 X Reference voltage;
        /// 10: 1.17 X Reference voltage; 11: 1.10 X Reference voltage
        UINT32 micBiasCtl               : 2;

        /// bit 4       -   MIC bias reference selection
        /// 0: supply as MIC bias reference. MIC bias output voltage scaled with supply voltage
        /// 1: BG voltage as MIC bias reference to have constant MIC bias output voltage
        UINT32 micRefSlt                : 1;

        /// bit 6:5     -   MIC bias reference filter clock programmability
        /// 00: 50KHz (default); 01: 25KHz; 10: 100KHz; 11: 1.6MHz
        UINT32 micScfClkCtl             : 2;

        /// bit 7       -   MIC bias switch cap filter bypass
        /// 0: enable switch cap filter; 1: bypass switch cap filter
        UINT32 micScfBypass             : 1;

        /// reserved bit 8
        UINT32 reserved0                : 1;

        /// bit 9       -   External power up sequence for MIC bias SCF bypass
        UINT32 micScfBypassSeq          : 1;

        /// bit 10       -   Internal/External MIC bias SCF power up sequence selection:
        /// 0: use internal power up sequence; 1: use external power up sequence
        UINT32 micScfSeqSlt             : 1;

        /// bit 11       -   MIC bias low/high output impedance control during power down mode:
        /// 0: MIC bias output is HZ in power down mode (default); 1: MIC bias output is LZ in power down mode
        UINT32 micBiasLz                : 1;

        /// reserved bits 15:12
        UINT32  reserved2               : 4;

        /// bit 17:16   -   PGA input common mode control
        /// 01 : 0.45 * avdd; 00 : 0.4 * avdd; 10 : 0.35 * avdd
        UINT32  micPgaIncmCtl           : 2;

        /// bit 19:18   -   PGA output common mode control
        /// 01 : 0.7V; 00 : 0.6V; 10 : 0.5V
        UINT32  micPgaOutcmCtl          : 2;

        /// bit 21:20   -
        UINT32  micPgaIncmresCtl        : 2;

        /// bit 22      -
        UINT32  micNegInputSlt          : 1;

        /// reserved bits 31:23
        UINT32 reserved1                : 9;
    }bitmap;
}ADC_CTL1_REG;


enum
{
    // bit 22
    CTL1_REG_ADC_MIC_NEG_INPUT_SELECT_DEFAULT   =   0 ,

    // bit 21:20
    CTL1_REG_ADC_MIC_PGA_INCMRES_CTL_DEFAULT    =   0 ,

    // bit 19:18
    CTL1_REG_ADC_MIC_PGA_OUT_COMM_MODE_0p6V     =   0x0,
    CTL1_REG_ADC_MIC_PGA_OUT_COMM_MODE_0p7V     =   0x1,
    CTL1_REG_ADC_MIC_PGA_OUT_COMM_MODE_0p5V     =   0x2,

    CTL1_REG_ADC_MIC_PGA_OUT_COMM_MODE_DEFAULT  =   CTL1_REG_ADC_MIC_PGA_OUT_COMM_MODE_0p6V ,

    // bit 17:16
    CTL1_REG_ADC_MIC_PGA_IN_COMM_MODE_0p4_AVDD  =   0x0,
    CTL1_REG_ADC_MIC_PGA_IN_COMM_MODE_0p45_AVDD =   0x1,
    CTL1_REG_ADC_MIC_PGA_IN_COMM_MODE_0p35_AVDD =   0x2,

    CTL1_REG_ADC_MIC_PGA_IN_COMM_MODE_DEFAULT   =   CTL1_REG_ADC_MIC_PGA_IN_COMM_MODE_0p4_AVDD ,

    // bit 15:12 reserved

    // bit 11
    CTL1_REG_ADC_MIC_BIAS_HZ                    =   0 ,
    CTL1_REG_ADC_MIC_BIAS_LZ                    =   1 ,

    CTL1_REG_ADC_MIC_BIAS_LZ_DEFAULT            =   CTL1_REG_ADC_MIC_BIAS_HZ ,

    // bit 10
    CTL1_REG_MIC_SCF_SEQ_SELECT_INT             =   0x0,
    CTL1_REG_MIC_SCF_SEQ_SELECT_EXT             =   0x1,

    CTL1_REG_MIC_SCF_SEQ_SELECT_DEFAULT         =   CTL1_REG_MIC_SCF_SEQ_SELECT_INT ,

    // bit 9
    CTL1_REG_ADC_MIC_SCF_BYPASS_SEQ_DEFAULT     =   0 ,

    // bit 7
    CTL1_REG_ADC_MIC_SCF_BYPASS_ENABLE          =   0x0,
    CTL1_REG_ADC_MIC_SCF_BYPASS_DISABLE         =   0x1,

    CTL1_REG_ADC_MIC_SCF_BYPASS_DEFAULT         =   CTL1_REG_ADC_MIC_SCF_BYPASS_ENABLE ,

    // bit 6:5
    CTL1_REG_ADC_MIC_SCF_CLK_50KHz              =   0x0,
    CTL1_REG_ADC_MIC_SCF_CLK_25KHz              =   0x1,
    CTL1_REG_ADC_MIC_SCF_CLK_100KHz             =   0x2,
    CTL1_REG_ADC_MIC_SCF_CLK_1p6MHz             =   0x3,

    CTL1_REG_ADC_MIC_SCF_CLK_DEFAULT            =   CTL1_REG_ADC_MIC_SCF_CLK_50KHz ,

    // bit 4
    CTL1_REG_ADC_MIC_BIAS_REF_SEL_SUPP_VOL      =   0x0,
    CTL1_REG_ADC_MIC_BIAS_REF_SEL_BG_VOL        =   0x1,

    CTL1_REG_ADC_MIC_BIAS_REF_SEL_DEFAULT       =   CTL1_REG_ADC_MIC_BIAS_REF_SEL_SUPP_VOL ,

    // bit 3:2
    CTL1_REG_ADC_MIC_BIAS_OUTPUT_VOL_1p12X      =   0x0,
    CTL1_REG_ADC_MIC_BIAS_OUTPUT_VOL_1p14X      =   0x1,
    CTL1_REG_ADC_MIC_BIAS_OUTPUT_VOL_1p17X      =   0x2,
    CTL1_REG_ADC_MIC_BIAS_OUTPUT_VOL_1p10X      =   0x3,

    CTL1_REG_ADC_MIC_BIAS_OUTPUT_VOL_DEFAULT    =   CTL1_REG_ADC_MIC_BIAS_OUTPUT_VOL_1p12X ,

    // bit 1:0
    CTL1_REG_ADC_MIC_BIAS_REF_VOL_75PER         =   0,
    CTL1_REG_ADC_MIC_BIAS_REF_VOL_70PER         =   1,
    CTL1_REG_ADC_MIC_BIAS_REF_VOL_80PER         =   2,

    CTL1_REG_ADC_MIC_BIAS_REF_VOL_DEFAULT       =   CTL1_REG_ADC_MIC_BIAS_REF_VOL_75PER ,

};

#define CTL2_REG_ADC_MIC_PGA_GAIN_CTL_MASK 0x0003F000
#define CTL1_REG_ADC_MIC_PGA_GAIN_CTL_MAX 42

///
/// MIA ADC Control 2 Register
/// - mia_adc_ctl2_adr
///
typedef union ADC_CTL2_REG
{
    UINT32 adc_ctl2_reg;
    struct
    {
        /// bit 4:0     -   ADC DC input selection (32-to-1 MUX):
        /// 01111 ADC reference ground (ADC_REFGND); 01110 ADC BG REF (ADC_BGREF); 01101 Core supply (VDDC);
        /// 01100 Battery/IO supply (VBAT/VDDIO); 01011 GPIO 27; 01010 GPIO 26; 01001 GPIO 25; 01000 GPIO 24;
        /// 00111 GPIO 23; 00110 GPIO 22; 00101 GPIO 21; 00100 GPIO 20; 00011 GPIO 19; 00010 GPIO 18;
        /// 00001 GPIO 17; 00000 GPIO 16; 11111 GPIO 15; 11110 GPIO 14; 11101 GPIO 13; 11100 GPIO 12;
        /// 11011 GPIO 11; 11010 GPIO 10; 11001 GPIO 9; 11000 GPIO 8; 10111 GPIO 7; 10110 GPIO 6; 10101 GPIO 5;
        /// 10100 GPIO 4; 10011 GPIO 3; 10010 GPIO 2; 10001 GPIO 1; 10000 GPIO 0
        UINT32 adcDcInputMux            : 5;

        /// bit 6:5     -   Dither Sequence control
        /// 00: No dither; 01: prbs15; 10: prbs23; 11: prbs31
        UINT32 adcDithCtl               : 2;

        /// bit 7       -   Disable the shuffler:
        /// 0: disabled; 1: enabled
        UINT32 AdcShuffEn               : 1;

        /// bit 8     -   Disable reset function on DC measurement:
        /// 0: disabled; 1: enabled
        UINT32 adcResetEn               : 1;

        /// bit 9     -   MIC PGA clamping enable/disable:
        /// 0: disable MIC PGA clamping
        /// 1: enable MIC PGA clamping
        UINT32 micClampEnable           : 1;

        /// bit 11:10 PGA clamp threshold voltage control
        /// 01 : 0.925V; 00 : 0.95V; 10 : 0.975V; 11 : 1V
        UINT32 micPgaClampvrefCtl       : 2;

        /// bit 17:12 MIC PGA gain control: 1dB steps
        /// 0 to 42: 0 to 42 dB
        UINT32 micPgaGainCtl            : 6;

        /// reserved bits 21:18
        UINT32 reserved1                : 4;

        /// bit 31:22       -   spare register
        UINT32 adcSpare                 : 10;
    }bitmap;
}ADC_CTL2_REG;

enum
{
    // bit 17:12
    CTL2_REG_ADC_PGA_GAIN_DEFAULT               =   0,

    // bit 11:10
    CTL2_REG_ADC_PGA_CLAMP_THRES_VOLTAGE_0P95   =   0x0,
    CTL2_REG_ADC_PGA_CLAMP_THRES_VOLTAGE_0P925  =   0x1,
    CTL2_REG_ADC_PGA_CLAMP_THRES_VOLTAGE_0P975  =   0x2,
    CTL2_REG_ADC_PGA_CLAMP_THRES_VOLTAGE_1P00   =   0x3,

    CTL2_REG_ADC_PGA_CLAMP_THRES_DEFAULT        =   CTL2_REG_ADC_PGA_CLAMP_THRES_VOLTAGE_0P95,

    // bit 9
    CTL2_REG_ADC_MIC_PGA_CLAMP_DISABLE          =   0x0,
    CTL2_REG_ADC_MIC_PGA_CLAMP_ENABLE           =   0x1,

    CTL2_REG_ADC_MIC_PGA_CLAMP_DEFAULT          =   CTL2_REG_ADC_MIC_PGA_CLAMP_ENABLE,

    // bit 8
    CTL2_REG_ADC_RESET_DISABLE                  =   0x0,
    CTL2_REG_ADC_RESET_ENABLE                   =   0x1,

    CTL2_REG_ADC_RESET_DEFAULT                  =   CTL2_REG_ADC_RESET_DISABLE ,

    // bit 7
    CTL2_REG_ADC_SHUFFLER_DISABLE               =   0x0,
    CTL2_REG_ADC_SHUFFLER_ENABLE                =   0x1,

    CTL2_REG_ADC_SHUFFLER_DEFAULT               =   CTL2_REG_ADC_SHUFFLER_DISABLE ,

    // bit 6:5
    CTL2_REG_ADC_DITHER_CTL_NO_DITHER           =   0x0,
    CTL2_REG_ADC_DITHER_CTL_PRBS15              =   0x1,
    CTL2_REG_ADC_DITHER_CTL_PRBS23              =   0x2,
    CTL2_REG_ADC_DITHER_CTL_PRBS31              =   0x3,

    CTL2_REG_ADC_DITHER_CTL_DEFAULT             =   CTL2_REG_ADC_DITHER_CTL_NO_DITHER ,

    // bit 4:0
    CTL2_REG_ADC_DC_INPUT_SEL_DEFAULT           =   ADC_INPUT_P16

};


///
/// Mia ADC interface Control Register
///
// - mia_adc_intf_ctl_adr
///
///

typedef union INTF_CTL_REG
{
    UINT32 adc_intf_ctl_reg;

    struct
    {

        /// bit 0    - start ADC
        UINT32 startADC             : 1;

        /// bit 1    - one shot
        UINT32 SampleOneShot        : 1;

        /// reserved bits 3:2
        UINT32 reserved0            : 2;

        /// bit 6:4  - this is a lagacy reigster, should always set to zero now
        UINT32 tcLatencyCnt         : 3;

        /// reserved bits 15:7
        UINT32 reserved1            : 9;

        /// bit 16 - ADC sample ready
        UINT32 SampleReady          : 1;
        UINT32 reserved2            : 15;
    }bitmap;
}INTF_CTL_REG;


/// bit map definition for adc intf control register
enum
{
    INTF_CTL_SAMPLE_READY               =   0x1,

    INTF_CTL_ADC_SAMPLE_ONE_SHOT        =   0x1,
    INTF_CTL_ADC_SAMPLE_CONTINUOUS      =   0x0,

    INTF_CTL_START_ADC                  =   0x1,
};

///
/// Mia ADC interface Control2 Register
///
// - mia_adc_intf_ctl2_adr
///
///

typedef union INTF_CTL2_REG
{
    UINT32 adc_intf_ctl_reg;

    struct
    {

        /// bit 2:0     - pattern generator output mode
        UINT32 adcPatGenMode        : 3;

        /// bit 3       - pattern generator clock enable
        UINT32 adcPatGenEn          : 1;

        /// bit 4       - pattern generator software init, only has effect when adc_pat_gen_source == 0
        UINT32 adcPatGenInit        : 1;

        /// bit 5       - ADC DMA interface input select
        /// 0 = ADC filter output; 1 = pattern generator output
        UINT32 adcDmaUsePatGen      : 1;

        /// bit 6       - ADC API interface input select
        /// 0 = ADC filter output; 1 = pattern generator output
        UINT32 adcApiUsePatGen      : 1;

        /// bit 7       - pattern generator init source select
        /// 0 = adc_pat_gen_init register; 1 = ADC DMA interface
        UINT32 adcPatGenSource      : 1;

        /// bit 8       - program the data alignment mode of ADC DMA interface and pattern generator
        /// 0 = MSB aligned; 1 = LSB aligned
        UINT32 adcFilterLsbAligned  : 1;

        /// bit 11:9    - debug output select
        UINT32 adcDbgOutSel         : 3;

        /// bit 12       - latency_cnt counter is the sample ticker divider outside of ADC filter
        /// 0 = latency_cnt has no effect; 1 = latency_cnt has effect
        UINT32 adcUseLatencyCnt     : 1;

        /// bit 13       - 0 = no effect; 1 = software reset latency_cnt to adc_latency_cnt_max
        UINT32 adcLatencyCntSoftrst : 1;

        /// reserved bits 15:14
        UINT32 reserved1            : 2;

        ///bit 23:16    - value = (divide ratio - 1), for example, 0xf = divide by 16
        UINT32 adcLatencyCntMax     : 8;

        /// reserved bits 31:24
        UINT32 reserved2            : 8;
    }bitmap;
}INTF_CTL2_REG;


/// bit map definition for adc intf control register
enum
{
    // bit 23:16
    INTF_CTL2_LATENCY_CNT_MAX_DEFAULT   =   0xF ,

    // bit 13
    INTF_CTL2_LATENCY_CNT_SOFTRST       =   0x1 ,
    INTF_CTL2_LATENCY_CNT_SOFTRST_OFF   =   0x0 ,

#ifdef BCM20703
    // bit 12
    INTF_CTL2_USE_LATENCY_CNT_DEFAULT   =   0x1 ,
#else
    // bit 12
    INTF_CTL2_USE_LATENCY_CNT_DEFAULT   =   0x0 ,
#endif

    // bit 11:9
    INTF_CTL2_DBG_OUT_SEL_DEFAULT       =   0 ,

    // bit 8
    INTF_CTL2_FILTER_ALIGN_MSB          =   0x0 ,
    INTF_CTL2_FILTER_ALIGN_LSB          =   0x1 ,

    INTF_CTL2_FILTER_ALIGN_DEFAULT      =   INTF_CTL2_FILTER_ALIGN_MSB ,

    // bit 7
    INTF_CTL2_PAT_GEN_SRC_REG           =   0x0 ,
    INTF_CTL2_PAT_GEN_SRC_DMA           =   0x1 ,

    INTF_CTL2_PAT_GEN_SRC_DEFAULT       =   INTF_CTL2_PAT_GEN_SRC_REG ,

    // bit 6
    INTF_CTL2_API_USE_FILTER            =   0x0 ,
    INTF_CTL2_API_USE_PAT_GEN           =   0x1 ,

    INTF_CTL2_API_USE_DEFAULT           =   INTF_CTL2_API_USE_FILTER ,

    // bit 5
    INTF_CTL2_DMA_USE_FILTER            =   0x0 ,
    INTF_CTL2_DMA_USE_PAT_GEN           =   0x1 ,

    INTF_CTL2_DMA_USE_DEFAULT           =   INTF_CTL2_DMA_USE_FILTER ,

    // bit 4
    INTF_CTL2_PAT_GEN_NOT_INIT          =   0x0 ,
    INTF_CTL2_PAT_GEN_INIT              =   0x1 ,

    INTF_CTL2_PAT_GEN_INIT_DEFAULT      =   INTF_CTL2_PAT_GEN_NOT_INIT ,

    // bit 3
    INTF_CTL2_PAT_GEN_DISABLE           =   0x0 ,
    INTF_CTL2_PAT_GEN_ENABLE            =   0x1 ,

    INTF_CTL2_PAT_GEN_EN_DEFAULT        =   INTF_CTL2_PAT_GEN_DISABLE ,


    // bit 2:0
    INTF_CTL2_PAT_GEN_MODE_24B_W32B     = 0x0 ,
    INTF_CTL2_PAT_GEN_MODE_24B_OFF3     = 0x1 ,
    INTF_CTL2_PAT_GEN_MODE_24B_SAME     = 0x2 ,
    INTF_CTL2_PAT_GEN_MODE_8B           = 0x3 ,
    INTF_CTL2_PAT_GEN_MODE_16B_SAME     = 0x4 ,
    INTF_CTL2_PAT_GEN_MODE_16B_OFF2     = 0x5 ,
    INTF_CTL2_PAT_GEN_MODE_ZERO         = 0x6 ,
    INTF_CTL2_PAT_GEN_MODE_FF           = 0x7 ,

    INTF_CTL2_PAT_GEN_MODE_DEFAULT      =   INTF_CTL2_PAT_GEN_MODE_24B_W32B ,
};


///
/// MIA ADC Filter 0 Register
/// - mia_adc_filter_ctl0_adr
///
typedef union FILTER_CTL0_REG
{
    UINT32 adc_filter_ctl0_reg;
    struct
    {
        /// bit 2:0 -   Mode of operation of the CIC filter- Mode 0,1 for Audio and Mode 3,4 and 5 for DC measurement.
        /// For audio, the default needs to be 1
        UINT32 auxAdcDfMode         : 3;

        /// reserved bits 31:3
        UINT32 reserved1            : 29;
    }bitmap;
}FILTER_CTL0_REG;


#ifdef BCM20703
enum
{
    // bit 2:0
    FILTER_CTL0_REG_DF_MODE_12KHz       =   0 ,
    FILTER_CTL0_REG_DF_MODE_24KHz       =   1 ,
//    FILTER_CTL0_REG_DF_MODE_16KHz       =   2 ,
    FILTER_CTL0_REG_DF_MODE_48KHz       =   3 ,
    FILTER_CTL0_REG_DF_MODE_96KHz       =   5 ,

    FILTER_CTL0_REG_DF_MODE_DEFAULT     =   FILTER_CTL0_REG_DF_MODE_96KHz ,
};
#else
/*
// new definition for 30739
0 -> DC mode, 480kHz, fast
1 -> DC mode, 480kHz, medium
2 -> DC mode, 480kHz, slow
3 -> Audio mode, 16kHz, analog mic
4 -> Audio mode, 16kHz, digital mic
5 -> Audio mode, 8kHz, analog mic
6 -> Audio mode, 8kHz, digital mic
*/
enum
{
    // bit 2:0
    FILTER_CTL0_REG_DF_MODE_480KHz_FAST =   0 ,
    FILTER_CTL0_REG_DF_MODE_480KHz_MED  =   1 ,
    FILTER_CTL0_REG_DF_MODE_480KHz_SLOW =   2 ,
    FILTER_CTL0_REG_DF_MODE_16KHz_ANA   =   3 ,
    FILTER_CTL0_REG_DF_MODE_16KHz_DIG   =   4 ,
    FILTER_CTL0_REG_DF_MODE_8KHz_ANA    =   5 ,
    FILTER_CTL0_REG_DF_MODE_8KHz_DIG    =   6 ,

    FILTER_CTL0_REG_DF_MODE_DEFAULT     =   FILTER_CTL0_REG_DF_MODE_480KHz_MED ,
};
#endif


///
/// MIA ADC Filter 1 Register
/// - mia_adc_filter_ctl1_adr
///
typedef union FILTER_CTL1_REG
{
    UINT32 adc_filter_ctl1_reg;
    struct
    {
        /// bit 15:0    -   High Pass Filter Numerator Co-efficient
        /// 0x7ffd
        UINT32 auxAdchpfNum         : 16;

        /// bit 31:16   -   High Pass Filter Denominator Co-efficient
        /// 0x7fcf
        UINT32 auxAdchpfDen         : 16;
    }bitmap;
}FILTER_CTL1_REG;


///
/// MIA ADC Filter 2 Register
/// - mia_adc_filter_ctl2_adr
///
typedef union FILTER_CTL2_REG
{
    UINT32 adc_filter_ctl2_reg;
    struct
    {
        /// bit 15:0    -   Biquad 1 numerator - 1 (LPF)
        /// 0xa7fe
        UINT32 auxAdcBq1Num2         : 16;

        /// bit 31:16   -   Biquad 2 numerator  - 1 (LPF)
        /// 0xb109
        UINT32 auxAdcBq2Num2         : 16;
    }bitmap;
}FILTER_CTL2_REG;


///
/// MIA ADC Filter 3 Register
/// - mia_adc_filter_ctl3_adr
///
typedef union FILTER_CTL3_REG
{
    UINT32 adc_filter_ctl3_reg;
    struct
    {
        /// bit 15:0    -   Biquad 3 numerator - 1 (LPF)
        /// 0xfdf1
        UINT32 auxAdcBq3Num2         : 16;

        /// bit 31:16   -   Biquad 4 numerator - 1 (LPF)
        /// 0x39bd
        UINT32 auxAdcBq4Num2         : 16;
    }bitmap;
}FILTER_CTL3_REG;


///
/// MIA ADC Filter 4 Register
/// - mia_adc_filter_ctl4_adr
///
typedef union FILTER_CTL4_REG
{
    UINT32 adc_filter_ctl4_reg;
    struct
    {
        /// bit 15:0    -   Biquad 1 numerator - 2 (LPF)
        /// 0x3ffd
        UINT32 auxAdcBq1Num3         : 16;

        /// bit 31:16   -   Biquad 2 numerator - 2 (LPF)
        /// 0x3fa8
        UINT32 auxAdcBq2Num3         : 16;
    }bitmap;
}FILTER_CTL4_REG;


///
/// MIA ADC Filter 5 Register
/// - mia_adc_filter_ctl5_adr
///
typedef union FILTER_CTL5_REG
{
    UINT32 adc_filter_ctl5_reg;
    struct
    {
        /// bit 15:0    -   Biquad 3 numerator - 2 (LPF)
        /// 0x2ada
        UINT32 auxAdcBq3Num3         : 16;

        /// bit 31:16   -   Biquad 4 numerator -2 (LPF)
        /// 0x0c62
        UINT32 auxAdcBq4Num3         : 16;
    }bitmap;
}FILTER_CTL5_REG;


///
/// MIA ADC Filter 6 Register
/// - mia_adc_filter_ctl6_adr
///
typedef union FILTER_CTL6_REG
{
    UINT32 adc_filter_ctl6_reg;
    struct
    {
        /// bit 15:0    -   Biquad 1 denominator - 1 (LPF)
        /// 0xa656
        UINT32 auxAdcBq1Den2         : 16;

        /// bit 31:16   -   Biquad 2 denominator - 1 (LPF)
        /// 0xa607
        UINT32 auxAdcBq2Den2         : 16;
    }bitmap;
}FILTER_CTL6_REG;


///
/// MIA ADC Filter 7 Register
/// - mia_adc_filter_ctl7_adr
///
typedef union FILTER_CTL7_REG
{
    UINT32 adc_filter_ctl7_reg;
    struct
    {
        /// bit 15:0    -   Biquad 3 denominator - 1 (LPF)
        /// 0xa44c
        UINT32 auxAdcBq3Den2         : 16;

        /// bit 31:16   -   Biquad 4 denominator - 1 (LPF)
        /// 0xa1cd
        UINT32 auxAdcBq4Den2         : 16;
    }bitmap;
}FILTER_CTL7_REG;


///
/// MIA ADC Filter 8 Register
/// - mia_adc_filter_ctl8_adr
///
typedef union FILTER_CTL8_REG
{
    UINT32 adc_filter_ctl8_reg;
    struct
    {
        /// bit 15:0    -   Biquad 1 denominator - 2 (LPF)
        /// 0x20f0
        UINT32 auxAdcBq1Den3         : 16;

        /// bit 31:16   -   Biquad 2 denominator - 2 (LPF)
        /// 0x2ab5
        UINT32 auxAdcBq2Den3         : 16;
    }bitmap;
}FILTER_CTL8_REG;


///
/// MIA ADC Filter 9 Register
/// - mia_adc_filter_ctl9_adr
///
typedef union FILTER_CTL9_REG
{
    UINT32 adc_filter_ctl9_reg;
    struct
    {
        /// bit 15:0    -   Biquad 3 denominator - 2 (LPF)
        /// 0x35e7
        UINT32 auxAdcBq3Den3         : 16;

        /// bit 31:16   -   Biquad 4 denominator - 2 (LPF)
        /// 0x3d43
        UINT32 auxAdcBq4Den3         : 16;
    }bitmap;
}FILTER_CTL9_REG;


///
/// MIA ADC Filter A Register
/// - mia_adc_filter_ctla_adr
///
typedef union FILTER_CTLA_REG
{
    UINT32 adc_filter_ctla_reg;
    struct
    {
        /// bit 19:0    -   Gain corresponding to second-order-section (biquad) implementati
        /// 0x764
        UINT32 auxAdcSosGain        : 20;

        /// bit 20      -   Output strobe rate that is used to decimate the output:  1 - 16KHz, 0 - 8KHz
        UINT32 auxAdcRate8K         : 1;

        /// bit 21      -   Flag to bypass high pass filter (if there is no DC)
        UINT32 auxAdcByPassHpf      : 1;

        /// bit 22      -   To select between dc measurement and audio
        UINT32 auxAdcAudioFlg       : 1;

        /// bit 23      -   1 = Rate64K
        UINT32 auxAdcMpr            : 1;

        /// bit 26:24   -   Used to vary sampling rate of the high pass filter
        /// 0x4
        UINT32 auxAdcDfOutRate      : 3;

        /// bit 27      -   Bypass mapper
        UINT32 auxAdcMprByPass      : 1;

        /// bit 29:28   -   Low pass filter rate - 0 - ~8Khz, 1 - ~16KHz, 2 - ~64KHz, 3 - ~128KHz  (not fixed rate)
        /// 0x2
        UINT32 auxAdcLpfRate        : 2;

        /// bit 31:30      -   Output precision: 0 - 24 bits, 1 - 16 bits, 2 - 8 bits (however, the output is always 24 bits MSB aligned)
        UINT32 auxAdcOutPrec        : 2;
    }bitmap;
}FILTER_CTLA_REG;


enum
{
    //bit 31:30
    FILTER_CTLA_REG_OUT_PREC_24BITS     =   0 ,
    FILTER_CTLA_REG_OUT_PREC_16BITS     =   1 ,
    FILTER_CTLA_REG_OUT_PREC_8BITS      =   2 ,

    FILTER_CTLA_REG_OUT_PREC_DEFAULT    =   FILTER_CTLA_REG_OUT_PREC_24BITS ,

    // bit 29:28
    FILTER_CTLA_REG_LPF_RATE_8KHz       =   0 ,
    FILTER_CTLA_REG_LPF_RATE_16KHz      =   1 ,
    FILTER_CTLA_REG_LPF_RATE_64KHz      =   2 ,
    FILTER_CTLA_REG_LPF_RATE_128KHz     =   3 ,

    FILTER_CTLA_REG_LPF_RATE_DEFAULT    =   FILTER_CTLA_REG_LPF_RATE_64KHz ,

    // bit 27
    FILTER_CTLA_REG_MPR_BYPASS          =   1 ,
    FILTER_CTLA_REG_MPR_NOT_BYPASS      =   0 ,

    // bit 22
    FILTER_CTLA_REG_DC_MODE             =   0 ,
    FILTER_CTLA_REG_AUDIO_MODE          =   1 ,

    // bit 21
    FILTER_CTLA_REG_BYPASS_HPF          =   1 ,
    FILTER_CTLA_REG_NOT_BYPASS_HPF      =   0 ,

    // bit 20
    FILTER_CTLA_REG_RATE_8KHz           =   0 ,
    FILTER_CTLA_REG_RATE_16KHz          =   1 ,

    FILTER_CTLA_REG_RATE_DEFAULT        =   FILTER_CTLA_REG_RATE_8KHz ,
};


///
/// MIA ADC Filter B Register
/// - mia_adc_filter_ctlb_adr
///
typedef union FILTER_CTLB_REG
{
    UINT32 adc_filter_ctlb_reg;
    struct
    {
        /// bit 1:0     -   Mapper input corresponding to 00
        /// 0x2
        UINT32 auxAdcMpr0           : 2;

        /// bit 3:2     -   Mapper input corresponding to 01
        /// 0x0
        UINT32 auxAdcMpr1           : 2;

        /// bit 5:4     -   Mapper input corresponding to 10 - Same as Mapper input corresponding to 01
        /// 0x0
        UINT32 auxAdcMpr2           : 2;

        /// bit 7:6     -   Mapper input corresponding to 11
        /// 0x1
        UINT32 auxAdcMpr3           : 2;

#ifdef BCM20703
        /// reserved bits 15:8
        UINT32 reserved0            : 8;
#else
        /// bit 8       -   channel select for adc decimation filter (read status and debug pin out).
        UINT32 adc_channel_sel      : 1;

        /// bit 9       -   df3Byp
        UINT32 df3Byp               : 1;

        /// reserved bits 15:9
        UINT32 reserved0            : 6;
#endif

        /// bit 16   -
        UINT32 adcDmaFifoEn         : 1;

        /// bit 18:17
        UINT32 adcDmaMode           : 2;

        /// bit 19   -
        UINT32 adcDmaOvrwrOff       : 1;

        /// bit 20   -
        UINT32 adcDmaLastEn         : 1;

        /// bit 21   -
        UINT32 adcDmaContinueEn     : 1;

        /// bit 22   -
        UINT32 adcDmaSoftFullClr    : 1;

        /// bit 23   -
        UINT32 adcDmaSoftOverlapClr : 1;

        /// bit 24   -
        UINT32 adcDmaFifoWrDisable  : 1;

        /// bit 25   -
        UINT32 adcDmaMaxFrameCntEn  : 1;

        /// bit 26  -
        UINT32 adcDmaFrameCntClkOff : 1;

        /// bit 27  -
        UINT32 adcDmaFifoStatusSel  : 1;

        /// bit 31:28
        UINT32 reserved             : 4;
    }bitmap;
}FILTER_CTLB_REG;


///
/// MIA ADC Filter C Register
/// - mia_adc_filter_ctlc_adr
///
typedef union FILTER_CTLC_REG
{
    UINT32 adc_filter_ctlc_reg;
    struct
    {
        /// bit 15:0    -
        UINT32 adcDmaMaxReqCnt      : 16;

        /// bit 31:16   -
        UINT32 adcDmaMaxFrameCnt    : 16;
    }bitmap;
}FILTER_CTLC_REG;


///
/// MIA ADC Filter D Register
/// - mia_adc_filter_ctld_adr
///
typedef union FILTER_CTLD_REG
{
    UINT32 adc_filter_ctld_reg;
    struct
    {
        /// bit 0
        UINT32 adcSampleReadyClr    : 1;
        /// bit 1
        UINT32 clkAdcCntClr         : 1;
        /// bit 2
        UINT32 clkDmaCntClr         : 1;
        /// bit 3
        UINT32 dataToggleCntClr     : 1;
        /// bit 4
        UINT32 latencyCnt2En        : 1;
        /// bit 5
        UINT32 clkAdcCntEn          : 1;
        /// bit 6
        UINT32 clkDmaCntEn          : 1;
        /// bit 7
        UINT32 dataToggleCntEn      : 1;
        /// bit 15:8
        UINT32 latencyCnt2Set       : 8;
        /// bit 16
        UINT32 adcDbgOutSelMsb      : 1;
        /// bit 17
        UINT32 adcDataSel           : 1;
        /// bit 18
        UINT32 pdm_decim_en_sel_manual : 1;
        /// bit 19
        UINT32 pdm_decim_en_sel     : 1;
        /// bit 22:20
        UINT32 pdm_phase_sel        : 3;

        /// bit 31:23
        UINT32 auxAdcSpare6         : 9;
    }bitmap;
}FILTER_CTLD_REG;


///
/// MIA ADC Filter E Register
/// - mia_adc_filter_ctle_adr
///
typedef union FILTER_CTLE_REG
{
    UINT32 adc_filter_ctle_reg;
    struct
    {
        /// bit 5:0
        UINT32 apiBclkTogFEven      : 6;

        /// bit 6
        UINT32 apiFclkClrOld        : 1;

        /// bit 31:7   -   spare register
        UINT32 auxAdcSpare8         : 25;
    }bitmap;
}FILTER_CTLE_REG;


///
/// MIA ADC Filter F Register
/// - mia_adc_filter_ctlf_adr
///
typedef union FILTER_CTLF_REG
{
    UINT32 adc_filter_ctlf_reg;
    struct
    {
        /// bit 15:0    -   spare register
        UINT32 auxAdcSpare9         : 16;

        /// bit 31:16   -   spare register
        UINT32 auxAdcSpare10           : 16;
    }bitmap;
}FILTER_CTLF_REG;


///
/// MIA ADC API CTL 0 Register
/// - mia_adc_api_ctl0_adr
///
typedef union API_CTL0_REG
{
    UINT32 adc_api_ctl0_reg;
    struct
    {
        /// bit 0   -   ADC API interface enable
        UINT32 apiEnable            : 1;

        /// reserved bit 1
        UINT32 reserved0            : 1;

        /// bit 3:2 -   2'h0: MODE_16BIT; 2'h1: MODE_8BIT; 2'h2: MODE_24BIT; 2'h3: MODE_16BIT
        UINT32 apiDataMode          : 2;

        /// bit 4   -   ADC API interface input alignment mode
        /// 0 = API interface treats input data as MSB aligned; 1 = API interface treats input data as LSB aligned
        UINT32 apiInLsbAligned      : 1;

        /// bit 5   -   ADC API interface output alignment mode
        /// 0 = API interface outputs data as MSB aligned; 1 = API interface outputs data as LSB aligned
        UINT32 apiOutLsbAligned     : 1;

        /// bit 7:6 -   ADC API interface output data shift mode
        /// 2'd0: SHIFT_MODE_24B_8B0     = 24B data + 8B padded 0
        /// 2'd1: SHIFT_MODE_24B_8B0     = 24B data + 8B padded 0
        /// 2'd2: SHIFT_MODE_8B0_24B     = 8B padded 0 + 24B data
        /// 2'd3: SHIFT_MODE_4B0_24B_4B0 = 4B padded 0 + 24B data + 4B padded 0
        UINT32 apiDataOutSel        : 2;

        /// reserved bit 31:8
        UINT32 reserved1            : 24;
    }bitmap;
}API_CTL0_REG;


enum
{
    // bit 7:6
    API_CTL0_REG_OUT_DATA_SHIFT_24B_8B0 =   0 ,
//    API_CTL0_REG_OUT_DATA_SHIFT_24B_8B0 =   1 ,
    API_CTL0_REG_OUT_DATA_SHIFT_8B0_24B =   2 ,
    API_CTL0_REG_OUT_DATA_SHIFT_4B0_24B_4B0 =   3 ,

    API_CTL0_REG_OUT_DATA_SHIFT_DEFAULT    =   API_CTL0_REG_OUT_DATA_SHIFT_24B_8B0 ,

    // bit 5
    API_CTL0_REG_OUTPUT_ALIGN_MSB           =   0 ,
    API_CTL0_REG_OUTPUT_ALIGN_LSB           =   1 ,

    API_CTL0_REG_OUTPUT_ALIGN_DEFAULT       =   API_CTL0_REG_OUTPUT_ALIGN_MSB ,

    // bit 4
    API_CTL0_REG_INPUT_ALIGN_MSB            =   0 ,
    API_CTL0_REG_INPUT_ALIGN_LSB            =   1 ,

    API_CTL0_REG_INPUT_ALIGN_DEFAULT        =   API_CTL0_REG_INPUT_ALIGN_MSB ,

    // bit 3:2
    API_CTL0_REG_DATA_MODE_16BIT            =   0 ,
    API_CTL0_REG_DATA_MODE_8BIT             =   1 ,
    API_CTL0_REG_DATA_MODE_24BIT            =   2 ,
//    API_CTL0_REG_DATA_MODE_16BIT            =   3 ,

    API_CTL0_REG_DATA_MODE_DEFAULT          =   API_CTL0_REG_DATA_MODE_16BIT ,

    // bit 0
    API_CTL0_REG_API_NOT_ENABLE             =   0 ,
    API_CTL0_REG_API_ENABLE                 =   1 ,
};


///
/// MIA ADC API CTL 1 Register
/// - mia_adc_api_ctl1_adr
///
typedef union API_CTL1_REG
{
    UINT32 adc_api_ctl1_reg;
    struct
    {
        /// bit 5:0     -
        /// 0x7
        UINT32 apiBclkLl2h          : 6;

        /// reserved bit 7:6
        UINT32 reserved0            : 2;

        /// bit 13:8    -
        /// 0x3
        UINT32 apiBclkH2l           : 6;

        /// reserved bit 15:14
        UINT32 reserved1            : 2;

        /// bit 21:16   -
        /// 0x7
        UINT32 apiBclkTogF          : 6;

        /// bit 22      -
        ///0x0
        UINT32 apiBclkSetByInit     : 1;

        /// bit 23      -
        ///0x0
        UINT32 apiBclkClrByInit     : 1;

        /// bit 24      -
        ///0x0
        UINT32 apiBclkSetByDen      : 1;

        /// bit 25      -
        ///0x0
        UINT32 apiBclkClrByDen      : 1;

        /// reserved bit 31:26
        UINT32 reserved2            : 6;
    }bitmap;
}API_CTL1_REG;


///
/// MIA ADC API CTL 2 Register
/// - mia_adc_api_ctl2_adr
///
typedef union API_CTL2_REG
{
    UINT32 adc_api_ctl2_reg;
    struct
    {
        /// bit 0       -   0 = no effect; 1 = API frame clock output set to 1 at init (enable from low to high)
        UINT32 apiFclkSetByInit     : 1;

        /// bit 1       -   0 = no effect; 1 = API frame clock output reset to 0 at init (enable from low to high)
        UINT32 apiFclkClrByInit     : 1;

        /// bit 2       -   0 = no effect; 1 = API frame clock output set to 1 by every input data sample
        /// 0x1
        UINT32 apiFclkSetByDen      : 1;

        /// bit 3       -   0 = no effect; 1 = API frame clock output reset to 0 by every input data sample
        UINT32 apiFclkClrByDen      : 1;

        /// bit 8:4     -   fclk_cnt value to set API frame clock output low to high
        /// 0x1e
        UINT32 apiFclkL2hFclk       : 5;

        /// bit 13:9    -   fclk_cnt value to set API frame clock output high to low
        ///0x0
        UINT32 apiFclkH2lFclk       : 5;

        /// bit 14      -   0 = API frame clock output low to high ignores bclk_cnt
        /// 1 = API frame clock output low to high checks bclk_cnt
        ///0x0
        UINT32 apiFclkL2hBclkOn     : 1;

        /// bit 15      -   0 = API frame clock output high to low ignores bclk_cnt
        /// 1 = API frame clock output high to low checks bclk_cnt
        ///0x0
        UINT32 apiFclkH2lBclkOn     : 1;

        /// bit 21:16   -   bclk_cnt value to set API frame clock output low to high, only has effect when api_fclk_l2h_bclk_on = 1
        ///0x0
        UINT32 apiFclkL2hBclk       : 6;

        /// bit 27:22   -   bclk_cnt value to set API frame clock output high to low, only has effect when api_fclk_h2l_bclk_on = 1
        ///0x0
        UINT32 apiFclkH2lBclk       : 6;

        /// reserved bit 31:28
        UINT32 reserved0            : 4;
    }bitmap;
}API_CTL2_REG;


///
/// MIA ADC API CTL 3 Register
/// - mia_adc_api_ctl3_adr
///
typedef union API_CTL3_REG
{
    UINT32 adc_api_ctl3_reg;
    struct
    {
        /// bit 0       -   API reference clock output inversion
        /// 0 = no invert; 1 = invert
        UINT32 apiRclkInvert        : 1;

        /// bit 1       -   0 = API reference clock output is gated when api_enable == 0
        /// 1 = API reference clock output is not gated when api_enable == 0
        UINT32 apiRclkAlwaysOn      : 1;

        /// bit 6:2     -   fclk_cnt wrap around value, only effective when api_fclk_cnt_max_en == 1
        /// 0x1f
        UINT32 apiFclkCntMax        : 5;

        /// bit 7       -   0 = fclk_cnt[4:0] increments until wrap around
        /// 1 = fclk_cnt[4:0] wrap around at api_fclk_cnt_max
        /// 0
        UINT32 apiFclkCntMaxEn      : 1;

        /// bit 12:8     -   fclk_cnt reset value, this offset can be used to shift output data position
        /// 0x1f
        UINT32 apiFclkCntOffset     : 5;

        /// reserved bit 31:13
        UINT32 reserved0            : 19;
    }bitmap;
}API_CTL3_REG;


///
/// MIA ADC API CTL 4 Register
/// - mia_adc_api_ctl4_adr
///
typedef union API_CTL4_REG
{
    UINT32 adc_api_ctl4_reg;
    struct
    {
        /// bit 0
        UINT32 bclk2xSpeed          : 1;

        /// bit 1
        UINT32 fclkPhaseSel         : 1;

        /// bit 2
        UINT32 fclkLastGatedOff     : 1;

        /// bit 3
        UINT32 apiFclkl2hBclkMsb    : 1;

        /// bit 10:4
        UINT32 apiBclkTogFLast      : 7;

        /// bit 11
        UINT32 apiFclkCntMaxMsb     : 1;

        /// bit 12
        UINT32 apiFclkl2hFclkMsb    : 1;

        /// bit 13
        UINT32 apiFclkH2lFclkMsb    : 1;

        /// bit 14
        UINT32 apiFclkCntOffsetMsb  : 1;

        /// bit 15
        UINT32 api_ch_swap          : 1;

        /// bit 31:16
        UINT32 api_spare            : 16;
    }bitmap;
}API_CTL4_REG;


///
/// MIA ADC DMA CTL 0 Register
/// - mia_adc_dma_ctl0_adr
///
typedef union DMA_CTL0_REG
{
    UINT32 adc_dma_ctl0_reg;
    struct
    {
        /// bit 0   -
        UINT32 adcDmaFifoEn         : 1;

        /// bit 2:1
        UINT32 adcDmaMode           : 2;

        /// bit 3   -
        UINT32 adcDmaOvrwrOff       : 1;

        /// bit 4   -
        UINT32 adcDmaLastEn         : 1;

        /// bit 5   -
        UINT32 adcDmaContinueEn     : 1;

        /// bit 6   -
        UINT32 adcDmaSoftFullClr    : 1;

        /// bit 7   -
        UINT32 adcDmaSoftOverlapClr : 1;

        /// bit 8   -
        UINT32 adcDmaFifoWrDisable  : 1;

        /// bit 9   -
        UINT32 adcDmaMaxFrameCntEn  : 1;

        /// bit 10  -
        UINT32 adcDmaFrameCntClkOff : 1;

        /// bit 11  -
        UINT32 adcDmaFifoStatusSel  : 1;

        /// reserved bit 31:12
        UINT32 reserved1            : 20;
    }bitmap;
}DMA_CTL0_REG;


enum
{
    // bit 2:1
    DMA_CTL0_REG_DMA_MODE_16BIT         =   0 ,
    DMA_CTL0_REG_DMA_MODE_8BIT          =   1 ,
    DMA_CTL0_REG_DMA_MODE_24BIT         =   2 ,
//    DMA_CTL0_REG_DMA_MODE_16BIT         =   3 ,

    DMA_CTL0_REG_DMA_MODE_DEFAULT       = DMA_CTL0_REG_DMA_MODE_16BIT,
};



///
/// MIA ADC DMA CTL 1 Register
/// - mia_adc_dma_ctl1_adr
///
typedef union DMA_CTL1_REG
{
    UINT32 adc_dma_ctl1_reg;
    struct
    {
        /// bit 15:0    -
        UINT32 adcDmaMaxReqCnt      : 16;

        /// bit 31:16   -
        UINT32 adcDmaMaxFrameCnt    : 16;
    }bitmap;
}DMA_CTL1_REG;


///
/// MIA ADC DMA CTL 2 Register
/// - mia_adc_dma_ctl2_adr
///
typedef union DMA_CTL2_REG
{
    UINT32 adc_dma_ctl2_reg;
    struct
    {
        /// bit 15:0    -
        UINT32 adcDmaCtl2           : 16;

        /// reserved bit 31:16
        UINT32 reserved1            : 16;
    }bitmap;
}DMA_CTL2_REG;


enum
{
    ADC_MODE_DC     = 0,
    ADC_MODE_AUDIO  = 1,
#ifdef PDM_SUPPORT
    PDM_MODE        = 2
#endif
};


typedef struct
{
    /// Ground offset. Obtained form user settings or auto calibration
    INT32          gndReading;

    /// Reference voltage conversion value.
    /// Obtained from user configuation setting or auto calibration
    INT32          referenceReading;

    /// Reference micro voltage
    UINT32          refMicroVolts;

#ifdef ADC_AUDIO_SUPPORT
    UINT8           adcMode;

    UINT8           adcDcModePowerState;
#endif

    UINT8           adcPowerState;

    /// Whether the ADC driver has been initialized or not
    UINT8           inited;
} AdcState;


/// ADC calibration mode
enum
{
    /// ADC will auto calibrate when configured. Standard setting.
    ADC_CONFIG_AUTO_CALIBRATION             = 0,

    /// application provided calibration settings. Used with factory calibration
    ADC_CONFIG_USER_INPUT                   = 1,

};

#pragma pack(1)

typedef PACKED struct
{
    PACKED union
    {
        PACKED struct
        {
            /// bit 0       -   ADC IP level main power up/down control:
            /// 0: power down whole ADC IP (Default); 1: power up
            UINT32 adcPwrup                 : 1;

            /// bit 1       -   ADC BG & REF power up/down:
            /// 0: power down BG and REF (Default); 1: power up
            UINT32 adcRefPwrup              : 1;

            /// bit 2       -   ADC core power up/down:
            /// 0: power down ADC Core (Default); 1: power up
            UINT32 adcCorePwrup             : 1;

            /// bit 3       -   ADC MIC (Audio) path power up/down control:
            /// 0: power down MIC bias and PGA (Default); 1: power up
            UINT32 micPwrup                 : 1;

            /// bit 4       -   MIC bias power up/down:
            /// 0: power down MIC Bias(Default); 1: Power up
            UINT32 micBiasPwrup             : 1;

            /// bit 5       -   Output synchronization clock phase control:
            /// 0: non-inverted clock phase (default); 1: inverted clock phase
            UINT32 adcSynClkPhase           : 1;

            /// bit 6       -   MIC (Audio) input and DC measurement input selection:
            /// 0: ADC in DC Measurement Mode; DC measurement path enabled; MIC path powered down
            /// 1: ADC in Audio application Mode; MIC path enabled. DC input is gated
            UINT32 adcMode                  : 1;

            /// bit 7       -   MIC path power up/down control selection in DC measurement mode:
            /// 0: Power down MIC Bias and PGA in DC measurement Mode
            /// 1: MIC path power up/down controlled by its pwrdn signal
            UINT32 adcMicPdslt              : 1;

            /// bit 8       -   Chip global power down control:
            /// 0: power up (Default); 1: power down
            UINT32  adcIddq                 : 1;

            /// bit 11:9    -   BG PTAT current adjustment:
            UINT32  adcBgPtatCtl            : 3;

            /// bit 14:12    -   BG CTAT current adjustment:
            UINT32  adcBgCtatCtl            : 3;

            /// bit 18:15    -   ADC reference voltage programmability:
            UINT32  adcRefCtl               : 4;

            /// bit 20:19    -   Switch cap filter clock frequency selection:
            UINT32  adcScfClkDiv            : 2;

            /// bit 21          -   BG REF switch cap filter bypass
            /// 0: enable switch cap filter; 1: bypass switch cap filter
            UINT32  ADC_SCF_bypass          : 1;

            /// bit 22          -   External power up sequence for BG SCF bypass
            UINT32  adcScfBypassSeq         : 1;

            /// bit 23          -   Internal/External BG SCF power up sequence selection:
            /// 0: use internal power up sequence; 1: use external power up sequence
            UINT32  adcScfSeqSlt            : 1;

            /// bit 25:24   -   Global bias current programmability
            UINT32  adcBiasCtl              : 2;

            /// reserved bits 28:26
            UINT32 reserved0                : 3;

            /// bit 29          -   ADC input range selection for DC measurement path:
            /// 0: 0-3.6V; 1: 0-1.8V
            UINT32  adcDcInputRange         : 1;

            /// bit 30          -   ADC internal clock division:
            /// 0: disable divide-by-2; ADC sampling clock at 12MHz; 1: enable divide-by-2; ADC sampling clock at 6MHz
            UINT32  adcClkDiv2              : 1;

            /// reserved bit 31
            UINT32 reserved1                : 1;
        }bitmap_adc_ctl0;

        UINT32 adc_ctl0;
    } adc_ctl0_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 0:1     -   MIC bias reference voltage programmability:
            ///00: 75% of Audio Supply (default); 10: 80% of Audio Supply; 01 / 11: 70% of Audio Supply
            UINT32 micBiasRefCtl            : 2;

            /// bit 3:2     -   MIC bias output voltage programmability:
            /// 00: 1.12 X Reference Voltage (default); 01: 1.14 X Reference voltage;
            /// 10: 1.17 X Reference voltage; 11: 1.10 X Reference voltage
            UINT32 micBiasCtl               : 2;

            /// bit 4       -   MIC bias reference selection
            /// 0: supply as MIC bias reference. MIC bias output voltage scaled with supply voltage
            /// 1: BG voltage as MIC bias reference to have constant MIC bias output voltage
            UINT32 micRefSlt                : 1;

            /// bit 6:5     -   MIC bias reference filter clock programmability
            /// 00: 50KHz (default); 01: 25KHz; 10: 100KHz; 11: 1.6MHz
            UINT32 micScfClkCtl             : 2;

            /// bit 7       -   MIC bias switch cap filter bypass
            /// 0: enable switch cap filter; 1: bypass switch cap filter
            UINT32 micScfBypass             : 1;

            /// reserved bit 8
            UINT32 reserved0                : 1;

            /// bit 9       -   External power up sequence for MIC bias SCF bypass
            UINT32 micScfBypassSeq          : 1;

            /// bit 10       -   Internal/External MIC bias SCF power up sequence selection:
            /// 0: use internal power up sequence; 1: use external power up sequence
            UINT32 micScfSeqSlt             : 1;

            /// bit 11       -   MIC bias low/high output impedance control during power down mode:
            /// 0: MIC bias output is HZ in power down mode (default); 1: MIC bias output is LZ in power down mode
            UINT32 micBiasLz                : 1;

            /// bit 15:12   -   reserved
            UINT32  reserved                : 4;

            /// bit 17:16   -   PGA input common mode control
            /// 01 : 0.45 * avdd; 00 : 0.4 * avdd; 10 : 0.35 * avdd
            UINT32  micPgaIncmCtl           : 2;

            /// bit 19:18   -   PGA output common mode control
            /// 01 : 0.7V; 00 : 0.6V; 10 : 0.5V
            UINT32  micPgaOutcmCtl          : 2;

            /// bit 21:20   -
            UINT32  micPgaIncmresCtl        : 2;

            /// bit 22      -
            UINT32  micNegInputSlt          : 1;

            /// reserved bits 31:23
            UINT32 reserved1                : 9;
        }bitmap_adc_ctl1;

        UINT32 adc_ctl1;
    }adc_ctl1_reg;


    PACKED union
    {
        PACKED struct
        {
            /// bit 4:0     -   ADC DC input selection (32-to-1 MUX):
            /// 01111 ADC reference ground (ADC_REFGND); 01110 ADC BG REF (ADC_BGREF); 01101 Core supply (VDDC);
            /// 01100 Battery/IO supply (VBAT/VDDIO); 01011 GPIO 27; 01010 GPIO 26; 01001 GPIO 25; 01000 GPIO 24;
            /// 00111 GPIO 23; 00110 GPIO 22; 00101 GPIO 21; 00100 GPIO 20; 00011 GPIO 19; 00010 GPIO 18;
            /// 00001 GPIO 17; 00000 GPIO 16; 11111 GPIO 15; 11110 GPIO 14; 11101 GPIO 13; 11100 GPIO 12;
            /// 11011 GPIO 11; 11010 GPIO 10; 11001 GPIO 9; 11000 GPIO 8; 10111 GPIO 7; 10110 GPIO 6; 10101 GPIO 5;
            /// 10100 GPIO 4; 10011 GPIO 3; 10010 GPIO 2; 10001 GPIO 1; 10000 GPIO 0
            UINT32 adcDcInputMux            : 5;

            /// bit 6:5     -   Dither Sequence control
            /// 00: No dither; 01: prbs15; 10: prbs23; 11: prbs31
            UINT32 adcDithCtl               : 2;

            /// bit 7       -   Disable the shuffler:
            /// 0: disabled; 1: enabled
            UINT32 AdcShuffEn               : 1;

            /// bit 8     -   Disable reset function on DC measurement:
            /// 0: disabled; 1: enabled
            UINT32 adcResetEn               : 1;

            /// bit 9     -   MIC PGA clamping enable/disable:
            /// 0: disable MIC PGA clamping
            /// 1: enable MIC PGA clamping
            UINT32 micClampEnable           : 1;

            /// bit 11:10 PGA clamp threshold voltage control
            /// 01 : 0.925V; 00 : 0.95V; 10 : 0.975V; 11 : 1V
            UINT32 micPgaClampvrefCtl       : 2;

            /// bit 17:12 MIC PGA gain control: 1dB steps
            /// 0 to 42: 0 to 42 dB
            UINT32 micPgaGainCtl            : 6;

            /// reserved bits 21:18
            UINT32 reserved1                : 4;

            /// bit 31:22       -   spare register
            UINT32 adcSpare                 : 10;
        }bitmap_adc_ctl2;

        UINT32 adc_ctl2;
    } adc_ctl2_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 2:0     - pattern generator output mode
            UINT32 adcPatGenMode        : 3;

            /// bit 3       - pattern generator clock enable
            UINT32 adcPatGenEn          : 1;

            /// bit 4       - pattern generator software init, only has effect when adc_pat_gen_source == 0
            UINT32 adcPatGenInit        : 1;

            /// bit 5       - ADC DMA interface input select
            /// 0 = ADC filter output; 1 = pattern generator output
            UINT32 adcDmaUsePatGen      : 1;

            /// bit 6       - ADC API interface input select
            /// 0 = ADC filter output; 1 = pattern generator output
            UINT32 adcApiUsePatGen      : 1;

            /// bit 7       - pattern generator init source select
            /// 0 = adc_pat_gen_init register; 1 = ADC DMA interface
            UINT32 adcPatGenSource      : 1;

            /// bit 8       - program the data alignment mode of ADC DMA interface and pattern generator
            /// 0 = MSB aligned; 1 = LSB aligned
            UINT32 adcFilterLsbAligned  : 1;

            /// bit 11:9    - debug output select
            UINT32 adcDbgOutSel         : 3;

            /// bit 12       - latency_cnt counter is the sample ticker divider outside of ADC filter
            /// 0 = latency_cnt has no effect; 1 = latency_cnt has effect
            UINT32 adcUseLatencyCnt     : 1;

            /// bit 13       - 0 = no effect; 1 = software reset latency_cnt to adc_latency_cnt_max
            UINT32 adcLatencyCntSoftrst : 1;

            /// reserved bits 15:14
            UINT32 reserved1            : 2;

            ///bit 23:16    - value = (divide ratio - 1), for example, 0xf = divide by 16
            UINT32 adcLatencyCntMax     : 8;

            /// reserved bits 31:24
            UINT32 reserved2            : 8;
        }bitmap_adc_intf_ctl2;

        UINT32 adc_intf_ctl2;
    } adc_intf_ctl2_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 2:0 -   Mode of operation of the CIC filter- Mode 0,1 for Audio and Mode 3,4 and 5 for DC measurement.
            /// For audio, the default needs to be 1
            UINT32 auxAdcDfMode         : 3;

            /// reserved bits 31:3
            UINT32 reserved1            : 29;
        }bitmap_adc_filter_ctl0;

        UINT32 adc_filter_ctl0_data;
    } adc_filter_ctl0_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   High Pass Filter Numerator Co-efficient
            /// 0x7ffd
            UINT32 auxAdchpfNum         : 16;

            /// bit 31:16   -   High Pass Filter Denominator Co-efficient
            /// 0x7fcf
            UINT32 auxAdchpfDen         : 16;
        }bitmap_adc_filter_ctl1;

        UINT32 adc_filter_ctl1_data;
    } adc_filter_ctl1_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 1 numerator - 1 (LPF)
            /// 0xa7fe
            UINT32 auxAdcBq1Num2         : 16;

            /// bit 31:16   -   Biquad 2 numerator  - 1 (LPF)
            /// 0xb109
            UINT32 auxAdcBq2Num2         : 16;
        }bitmap_adc_filter_ctl2;

        UINT32 adc_filter_ctl2_data;
    } adc_filter_ctl2_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 3 numerator - 1 (LPF)
            /// 0xfdf1
            UINT32 auxAdcBq3Num2         : 16;

            /// bit 31:16   -   Biquad 4 numerator - 1 (LPF)
            /// 0x39bd
            UINT32 auxAdcBq4Num2         : 16;
        }bitmap_adc_filter_ctl3;

        UINT32 adc_filter_ctl3_data;
    } adc_filter_ctl3_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 1 numerator - 2 (LPF)
            /// 0x3ffd
            UINT32 auxAdcBq1Num3         : 16;

            /// bit 31:16   -   Biquad 2 numerator - 2 (LPF)
            /// 0x3fa8
            UINT32 auxAdcBq2Num3         : 16;
        }bitmap_adc_filter_ctl4;

        UINT32 adc_filter_ctl4_data;
    } adc_filter_ctl4_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 3 numerator - 2 (LPF)
            /// 0x2ada
            UINT32 auxAdcBq3Num3         : 16;

            /// bit 31:16   -   Biquad 4 numerator -2 (LPF)
            /// 0x0c62
            UINT32 auxAdcBq4Num3         : 16;
        }bitmap_adc_filter_ctl5;

        UINT32 adc_filter_ctl5_data;
    } adc_filter_ctl5_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 1 denominator - 1 (LPF)
            /// 0xa656
            UINT32 auxAdcBq1Den2         : 16;

            /// bit 31:16   -   Biquad 2 denominator - 1 (LPF)
            /// 0xa607
            UINT32 auxAdcBq2Den2         : 16;
        }bitmap_adc_filter_ctl6;

        UINT32 adc_filter_ctl6_data;
    } adc_filter_ctl6_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 3 denominator - 1 (LPF)
            /// 0xa44c
            UINT32 auxAdcBq3Den2         : 16;

            /// bit 31:16   -   Biquad 4 denominator - 1 (LPF)
            /// 0xa1cd
            UINT32 auxAdcBq4Den2         : 16;
        }bitmap_adc_filter_ctl7;

        UINT32 adc_filter_ctl7_data;
    } adc_filter_ctl7_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 1 denominator - 2 (LPF)
            /// 0x20f0
            UINT32 auxAdcBq1Den3         : 16;

            /// bit 31:16   -   Biquad 2 denominator - 2 (LPF)
            /// 0x2ab5
            UINT32 auxAdcBq2Den3         : 16;
        }bitmap_adc_filter_ctl8;

        UINT32 adc_filter_ctl8_data;
    } adc_filter_ctl8_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 15:0    -   Biquad 3 denominator - 2 (LPF)
            /// 0x35e7
            UINT32 auxAdcBq3Den3         : 16;

            /// bit 31:16   -   Biquad 4 denominator - 2 (LPF)
            /// 0x3d43
            UINT32 auxAdcBq4Den3         : 16;
        }bitmap_adc_filter_ctl9;

        UINT32 adc_filter_ctl9_data;
    } adc_filter_ctl9_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 19:0    -   Gain corresponding to second-order-section (biquad) implementati
            /// 0x764
            UINT32 auxAdcSosGain        : 20;

            /// bit 20      -   Output strobe rate that is used to decimate the output:  1 - 16KHz, 0 - 8KHz
            UINT32 auxAdcRate8K         : 1;

            /// bit 21      -   Flag to bypass high pass filter (if there is no DC)
            UINT32 auxAdcByPassHpf      : 1;

            /// bit 22      -   To select between dc measurement and audio
            UINT32 auxAdcAudioFlg       : 1;

            /// bit 23      -   1 = Rate64K
            UINT32 auxAdcMpr            : 1;

            /// bit 26:24   -   Used to vary sampling rate of the high pass filter
            /// 0x4
            UINT32 auxAdcDfOutRate      : 3;

            /// bit 27      -   Bypass mapper
            UINT32 auxAdcMprByPass      : 1;

            /// bit 29:28   -   Low pass filter rate - 0 - ~8Khz, 1 - ~16KHz, 2 - ~64KHz, 3 - ~128KHz  (not fixed rate)
            /// 0x2
            UINT32 auxAdcLpfRate        : 2;

            /// bit 31:30      -   Output precision: 0 - 24 bits, 1 - 16 bits, 2 - 8 bits (however, the output is always 24 bits MSB aligned)
            UINT32 auxAdcOutPrec        : 2;
        }bitmap_adc_filter_ctla;

        UINT32 adc_filter_ctla;
    } adc_filter_ctla_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 1:0     -   Mapper input corresponding to 00
            /// 0x2
            UINT32 auxAdcMpr0           : 2;

            /// bit 3:2     -   Mapper input corresponding to 01
            /// 0x0
            UINT32 auxAdcMpr1           : 2;

            /// bit 5:4     -   Mapper input corresponding to 10 - Same as Mapper input corresponding to 01
            /// 0x0
            UINT32 auxAdcMpr2           : 2;

            /// bit 7:6     -   Mapper input corresponding to 11
            /// 0x1
            UINT32 auxAdcMpr3           : 2;

#ifdef BCM20703
            /// reserved bits 15:8
            UINT32 reserved0            : 8;
#else
            /// bit 8       -   channel select for adc decimation filter (read status and debug pin out).
            UINT32 adc_channel_sel      : 1;

            /// bit 9       -   df3Byp
            UINT32 df3Byp               : 1;

            /// reserved bits 15:10
            UINT32 reserved0            : 6;
#endif

            /// bit 31:16   -   spare register
            UINT32 auxAdcSpare2         : 16;
        }bitmap_adc_filter_ctlb;

        UINT32 adc_filter_ctlb;
    } adc_filter_ctlb_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 0   -   ADC API interface enable
            UINT32 apiEnable            : 1;

            /// reserved bit 1
            UINT32 reserved0            : 1;

            /// bit 3:2 -   2'h0: MODE_16BIT; 2'h1: MODE_8BIT; 2'h2: MODE_24BIT; 2'h3: MODE_16BIT
            UINT32 apiDataMode          : 2;

            /// bit 4   -   ADC API interface input alignment mode
            /// 0 = API interface treats input data as MSB aligned; 1 = API interface treats input data as LSB aligned
            UINT32 apiInLsbAligned      : 1;

            /// bit 5   -   ADC API interface output alignment mode
            /// 0 = API interface outputs data as MSB aligned; 1 = API interface outputs data as LSB aligned
            UINT32 apiOutLsbAligned     : 1;

            /// bit 7:6 -   ADC API interface output data shift mode
            /// 2'd0: SHIFT_MODE_24B_8B0     = 24B data + 8B padded 0
            /// 2'd1: SHIFT_MODE_24B_8B0     = 24B data + 8B padded 0
            /// 2'd2: SHIFT_MODE_8B0_24B     = 8B padded 0 + 24B data
            /// 2'd3: SHIFT_MODE_4B0_24B_4B0 = 4B padded 0 + 24B data + 4B padded 0
            UINT32 apiDataOutSel        : 2;

            /// reserved bit 31:8
            UINT32 reserved1            : 24;
        }bitmap_adc_api_ctl0;

        UINT32 adc_api_ctl0_data;
    } adc_api_ctl0_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 5:0     -
            /// 0x7
            UINT32 apiBclkLl2h          : 6;

            /// reserved bit 7:6
            UINT32 reserved0            : 2;

            /// bit 13:8    -
            /// 0x3
            UINT32 apiBclkH2l           : 6;

            /// reserved bit 15:14
            UINT32 reserved1            : 2;

            /// bit 21:16   -
            /// 0x7
            UINT32 apiBclkTogF          : 6;

            /// bit 22      -
            ///0x0
            UINT32 apiBclkSetByInit     : 1;

            /// bit 23      -
            ///0x0
            UINT32 apiBclkClrByInit     : 1;

            /// bit 24      -
            ///0x0
            UINT32 apiBclkSetByDen      : 1;

            /// bit 25      -
            ///0x0
            UINT32 apiBclkClrByDen      : 1;

            /// reserved bit 31:26
            UINT32 reserved2            : 6;
        }bitmap_adc_api_ctl1;

        UINT32 adc_api_ctl1_data;
    } adc_api_ctl1_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 0       -   0 = no effect; 1 = API frame clock output set to 1 at init (enable from low to high)
            UINT32 apiFclkSetByInit     : 1;

            /// bit 1       -   0 = no effect; 1 = API frame clock output reset to 0 at init (enable from low to high)
            UINT32 apiFclkClrByInit     : 1;

            /// bit 2       -   0 = no effect; 1 = API frame clock output set to 1 by every input data sample
            /// 0x1
            UINT32 apiFclkSetByDen      : 1;

            /// bit 3       -   0 = no effect; 1 = API frame clock output reset to 0 by every input data sample
            UINT32 apiFclkClrByDen      : 1;

            /// bit 8:4     -   fclk_cnt value to set API frame clock output low to high
            /// 0x1e
            UINT32 apiFclkL2hFclk       : 5;

            /// bit 13:9    -   fclk_cnt value to set API frame clock output high to low
            ///0x0
            UINT32 apiFclkH2lFclk       : 5;

            /// bit 14      -   0 = API frame clock output low to high ignores bclk_cnt
            /// 1 = API frame clock output low to high checks bclk_cnt
            ///0x0
            UINT32 apiFclkL2hBclkOn     : 1;

            /// bit 15      -   0 = API frame clock output high to low ignores bclk_cnt
            /// 1 = API frame clock output high to low checks bclk_cnt
            ///0x0
            UINT32 apiFclkH2lBclkOn     : 1;

            /// bit 21:16   -   bclk_cnt value to set API frame clock output low to high, only has effect when api_fclk_l2h_bclk_on = 1
            ///0x0
            UINT32 apiFclkL2hBclk       : 6;

            /// bit 27:22   -   bclk_cnt value to set API frame clock output high to low, only has effect when api_fclk_h2l_bclk_on = 1
            ///0x0
            UINT32 apiFclkH2lBclk       : 6;

            /// reserved bit 31:28
            UINT32 reserved0            : 4;
        }bitmap_adc_api_ctl2;

        UINT32 adc_api_ctl2_data;
    } adc_api_ctl2_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 0       -   API reference clock output inversion
            /// 0 = no invert; 1 = invert
            UINT32 apiRclkInvert        : 1;

            /// bit 1       -   0 = API reference clock output is gated when api_enable == 0
            /// 1 = API reference clock output is not gated when api_enable == 0
            UINT32 apiRclkAlwaysOn      : 1;

            /// bit 6:2     -   fclk_cnt wrap around value, only effective when api_fclk_cnt_max_en == 1
            /// 0x1f
            UINT32 apiFclkCntMax        : 5;

            /// bit 7       -   0 = fclk_cnt[4:0] increments until wrap around
            /// 1 = fclk_cnt[4:0] wrap around at api_fclk_cnt_max
            /// 0
            UINT32 apiFclkCntMaxEn      : 1;

            /// bit 12:8     -   fclk_cnt reset value, this offset can be used to shift output data position
            /// 0x1f
            UINT32 apiFclkCntOffset     : 5;

            /// reserved bit 31:13
            UINT32 reserved0            : 19;
        }bitmap_adc_api_ctl3;

        UINT32 adc_api_ctl3_data;
    } adc_api_ctl3_reg;

    /// ADC power on delay, default 20us
    UINT8       powerondelayInUs;           // us

    /// bandgap reference channel, default 16
    UINT8       bandgapReferenceChannel;

    ///  calibration average count
    UINT8       calibrationConversionCount;

    /// input range
    UINT8      inputRangeSelect;

    /// reference voltage for bandgap, default 500mV
    UINT32      internalBandgapInMicroVolts;

    /// For legacy devices, the bandgap has to be offset by a certain amount
    INT16 ADCBandgapDelta[3];

    /// ADC api enable bit
    UINT8       adcApiEnable;

    /// ADC audio fifo number
    UINT8       adcAudioFifoNum;

    /// ADC audio fifo size
    UINT32      adcAudioFifoSize;

#ifndef BCM20703
    PACKED union
    {
        PACKED struct
        {
            /// bit 0
            UINT32 adcSampleReadyClr    : 1;
            /// bit 1
            UINT32 clkAdcCntClr         : 1;
            /// bit 2
            UINT32 clkDmaCntClr         : 1;
            /// bit 3
            UINT32 dataToggleCntClr     : 1;
            /// bit 4
            UINT32 latencyCnt2En        : 1;
            /// bit 5
            UINT32 clkAdcCntEn          : 1;
            /// bit 6
            UINT32 clkDmaCntEn          : 1;
            /// bit 7
            UINT32 dataToggleCntEn      : 1;
            /// bit 15:8
            UINT32 latencyCnt2Set       : 8;
            /// bit 16
            UINT32 adcDbgOutSelMsb      : 1;
            /// bit 17
            UINT32 adcDataSel           : 1;
            /// bit 18
            UINT32 pdm_decim_en_sel_manual : 1;
            /// bit 19
            UINT32 pdm_decim_en_sel     : 1;
            /// bit 22:20
            UINT32 pdm_phase_sel        : 3;

            /// bit 31:23
            UINT32 auxAdcSpare6         : 9;
        } bitmap_adc_filter_ctld;

        UINT32 adc_filter_ctld_data;
    } adc_filter_ctld_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 5:0
            UINT32 apiBclkTogFEven      : 6;

            /// bit 6
            UINT32 apiFclkClrOld        : 1;

            /// bit 31:7   -   spare register
            UINT32 auxAdcSpare8         : 25;
        } bitmap_adc_filter_ctle;

        UINT32 adc_filter_ctle_data;
    } adc_filter_ctle_reg;

    PACKED union
    {
        PACKED struct
        {
            /// bit 0
            UINT32 bclk2xSpeed          : 1;

            /// bit 1
            UINT32 fclkPhaseSel         : 1;

            /// bit 2
            UINT32 fclkLastGatedOff     : 1;

            /// bit 3
            UINT32 apiFclkl2hBclkMsb    : 1;

            /// bit 10:4
            UINT32 apiBclkTogFLast      : 7;

            /// bit 11
            UINT32 apiFclkCntMaxMsb     : 1;

            /// bit 12
            UINT32 apiFclkl2hFclkMsb    : 1;

            /// bit 13
            UINT32 apiFclkH2lFclkMsb    : 1;

            /// bit 14
            UINT32 apiFclkCntOffsetMsb  : 1;

            /// bit 15
            UINT32 api_ch_swap          : 1;

            /// bit 31:16
            UINT32 api_spare            : 16;
        }bitmap_adc_api_ctl4;

        UINT32 adc_api_ctl4_data;
    } adc_api_ctl4_reg;
#endif
}ADCConfig;

#pragma pack(0)

extern ADCConfig adcConfig;


#if defined(ADC_AUDIO_SUPPORT)

#ifdef __cplusplus
extern "C" {
#endif

/// change adc mode between DC mode and Audio mode
void adc_changeMode(UINT32 adcMode);

/// Start ADC Audio mode
#ifdef ADC_AUDIO_2_CHANNEL_SUPPORT
void adc_startAudio(UINT32 sampleRate, UINT32 bitsPerSample, UINT8 pgaGain, void (*func)(UINT8 *, UINT32, UINT32), void (*func2)(UINT8 *, UINT32, UINT32)
#ifdef PDM_SUPPORT
, BOOL32 isPdm
#endif  //#ifdef PDM_SUPPORT
);
#else
void adc_startAudio(UINT32 sampleRate, UINT32 bitsPerSample, UINT8 pgaGain, void (*func)(UINT8 *, UINT32, UINT32)
#ifdef PDM_SUPPORT
, BOOL32 isPdm
#endif  //#ifdef PDM_SUPPORT
);
#endif  //#ifdef ADC_AUDIO_2_CHANNEL_SUPPORT


/// Stop ADC Audio mode
void adc_stopAudio(void);

/// Check whether ADC Audio is activated
BOOL32 adc_AudioIsActive(void);

void adc_setPgaGain(UINT16 pgaGain);
#ifdef __cplusplus
}
#endif

#endif

/* @} */

#endif
