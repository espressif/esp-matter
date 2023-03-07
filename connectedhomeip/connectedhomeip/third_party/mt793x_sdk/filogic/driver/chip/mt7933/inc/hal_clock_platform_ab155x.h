/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#ifndef __HAL_CLOCK_PLATFORM_AB155X_H__
#define __HAL_CLOCK_PLATFORM_AB155X_H__

#include "hal_platform.h"

#ifdef HAL_CLOCK_MODULE_ENABLED
#define RF_DESENSE  1
#define DCXO_GOLDEN_VAL                                 411 // 0x19B

#if defined(AB1555)
#define DCXO_DEFAULT_VAL                                402 // 0x192
#elif defined(AB1558) || defined(AB1556)
#define DCXO_DEFAULT_VAL                                406 // 0x196
#else /* #if defined(AB1555) */
#define DCXO_DEFAULT_VAL                                431 // 0x1AF
#endif /* #if defined(AB1555) */



#define STR_VALUE(arg)      #arg
#define __stringify(name) STR_VALUE(name)

#define HFOSC_CON0__F_DA_HFOSC_EN                   ((volatile uint8_t *)0xA2040920)
#define HFOSC_CON0__F_RG_HFOSC_104M_EN              ((volatile uint8_t *)0xA2040921)
#define HFOSC_CON2__F_HFOSC_EN_SEL                  ((volatile uint8_t *)0xA204092A)
#define CKSYS_CLK_DIV_0__F_CLK_PLL1_D2_EN           ((volatile uint8_t *)0xA2020280)
#define CKSYS_CLK_DIV_0__F_CLK_PLL1_D3_EN           ((volatile uint8_t *)0xA2020281)
#define CKSYS_CLK_DIV_0__F_CLK_PLL1_D5_EN           ((volatile uint8_t *)0xA2020282)
#define CKSYS_CLK_DIV_0__F_CLK_PLL1_D7_EN           ((volatile uint8_t *)0xA2020283)
#define CKSYS_CLK_DIV_1__F_CLK_PLL2_D2_EN           ((volatile uint8_t *)0xA2020284)
#define CKSYS_CLK_DIV_1__F_CLK_PLL2_D3_EN           ((volatile uint8_t *)0xA2020285)
#define CKSYS_CLK_DIV_1__F_CLK_PLL2_D5_EN           ((volatile uint8_t *)0xA2020286)
#define CKSYS_CLK_DIV_1__F_CLK_PLL2_D7_EN           ((volatile uint8_t *)0xA2020287)
#define CKSYS_CLK_DIV_2__F_CLK_OSC_104M_D2_EN       ((volatile uint8_t *)0xA2020288)
#define CKSYS_CLK_DIV_2__F_CLK_OSC_D2_EN            ((volatile uint8_t *)0xA2020289)
#define CKSYS_CLK_DIV_2__F_CLK_OSC_D5_EN            ((volatile uint8_t *)0xA202028A)
#define CKSYS_CLK_DIV_2__F_CLK_OSC_D7_EN            ((volatile uint8_t *)0xA202028B)
#define CKSYS_CLK_DIV_3__F_CLK_PLL1_DIV_EN          ((volatile uint8_t *)0xA202028C)
#define CKSYS_CLK_DIV_3__F_CLK_PLL2_DIV_EN          ((volatile uint8_t *)0xA202028D)
#define CKSYS_CLK_DIV_3__F_CLK_OSC_DIV_EN           ((volatile uint8_t *)0xA202028E)

#define CKSYS_CLK_FORCE_ON_0__F_CLK_SYS_FORCE_ON    ((volatile uint8_t *)0xA2020270)
#define CKSYS_CLK_FORCE_ON_0__F_CLK_SFC_FORCE_ON    ((volatile uint8_t *)0xA2020271)
#define CKSYS_CLK_FORCE_ON_0__F_CLK_SPIMST0_FORCE_ON    ((volatile uint8_t *)0xA2020272)
#define CKSYS_CLK_FORCE_ON_0__F_CLK_SPIMST1_FORCE_ON    ((volatile uint8_t *)0xA2020273)
#define CKSYS_CLK_FORCE_ON_1__F_CLK_SPIMST2_FORCE_ON    ((volatile uint8_t *)0xA2020274)
#define CKSYS_CLK_FORCE_ON_1__F_CLK_SDIOMST_FORCE_ON    ((volatile uint8_t *)0xA2020275)
#define CKSYS_CLK_FORCE_ON_1__F_CLK_SPISLV_FORCE_ON ((volatile uint8_t *)0xA2020276)
#define CKSYS_CLK_FORCE_ON_1__F_CLK_USB_FORCE_ON    ((volatile uint8_t *)0xA2020277)
#define CKSYS_CLK_FORCE_ON_2__F_CLK_AUD_GPSRC_FORCE_ON  ((volatile uint8_t *)0xA2020278)
#define CKSYS_CLK_FORCE_ON_2__F_CLK_EMI_FORCE_ON    ((volatile uint8_t *)0xA2020279)
#define CKSYS_CLK_FORCE_ON_0                        ((volatile uint32_t *)0xA2020270)
#define CKSYS_CLK_FORCE_ON_1                        ((volatile uint32_t *)0xA2020274)
#define CKSYS_CLK_FORCE_ON_2                        ((volatile uint32_t *)0xA2020278)

#define CKSYS_CLK_CFG_0__F_CLK_SYS_SEL              ((volatile uint8_t *)0xA2020230)
#define CKSYS_CLK_CFG_0__F_CLK_SFC_SEL              ((volatile uint8_t *)0xA2020231)
#define CKSYS_CLK_CFG_0__F_CLK_SPIMST0_SEL          ((volatile uint8_t *)0xA2020232)
#define CKSYS_CLK_CFG_0__F_CLK_SPIMST1_SEL          ((volatile uint8_t *)0xA2020233)
#define CKSYS_CLK_CFG_1__F_CLK_SPIMST2_SEL          ((volatile uint8_t *)0xA2020234)
#define CKSYS_CLK_CFG_1__F_CLK_SDIOMST_SEL          ((volatile uint8_t *)0xA2020235)
#define CKSYS_CLK_CFG_1__F_CLK_SPISLV_SEL           ((volatile uint8_t *)0xA2020236)
#define CKSYS_CLK_CFG_1__F_CLK_USB_SEL              ((volatile uint8_t *)0xA2020237)
#define CKSYS_CLK_CFG_2__F_CLK_AUD_BUS_SEL          ((volatile uint8_t *)0xA2020238)
#define CKSYS_CLK_CFG_2__F_CLK_AUD_GPSRC_SEL        ((volatile uint8_t *)0xA2020239)
#define CKSYS_CLK_CFG_2__F_CLK_AUD_INTERFACE0_SEL   ((volatile uint8_t *)0xA202023A)
#define CKSYS_CLK_CFG_2__F_CLK_AUD_INTERFACE1_SEL   ((volatile uint8_t *)0xA202023B)
#define CKSYS_CLK_CFG_3__F_CLK_EMI_SEL              ((volatile uint8_t *)0xA202023C)
#define CKSYS_CLK_CFG_3__F_CLK_SPM_SEL              ((volatile uint8_t *)0xA202023D)
#define CKSYS_CLK_CFG_3__F_CLK_26M_SEL              ((volatile uint8_t *)0xA202023E)
#define CKSYS_CLK_CFG_4__F_CLK_AUD_DL_HIRES_SEL     ((volatile uint8_t *)0xA2020242)
#define CKSYS_CLK_CFG_4__F_CLK_AUD_UL_HIRES_SEL     ((volatile uint8_t *)0xA2020243)
#define CKSYS_CLK_CFG_5__F_UPLL_CK_SEL              ((volatile uint8_t *)0xA2020244)

#define CKSYS_CLK_UPDATE_STATUS_0__F_CHG_SYS_OK     ((volatile uint8_t *)0xA2020260)
#define CKSYS_CLK_UPDATE_STATUS_0__F_CHG_SFC_OK     ((volatile uint8_t *)0xA2020261)
#define CKSYS_CLK_UPDATE_STATUS_0__F_CHG_SPIMST0_OK ((volatile uint8_t *)0xA2020262)
#define CKSYS_CLK_UPDATE_STATUS_0__F_CHG_SPIMST1_OK ((volatile uint8_t *)0xA2020263)
#define CKSYS_CLK_UPDATE_STATUS_1__F_CHG_SPIMST2_OK ((volatile uint8_t *)0xA2020264)
#define CKSYS_CLK_UPDATE_STATUS_1__F_CHG_SDIOMST_OK ((volatile uint8_t *)0xA2020265)
#define CKSYS_CLK_UPDATE_STATUS_1__F_CHG_SPISLV_OK  ((volatile uint8_t *)0xA2020266)
#define CKSYS_CLK_UPDATE_STATUS_1__F_CHG_USB_OK     ((volatile uint8_t *)0xA2020267)
#define CKSYS_CLK_UPDATE_STATUS_2__F_CHG_AUD_GPSRC_OK     ((volatile uint8_t *)0xA2020268)
#define CKSYS_CLK_UPDATE_STATUS_2__F_CHG_EMI_OK     ((volatile uint8_t *)0xA2020269)

#define CKSYS_CLK_UPDATE_0__F_CHG_SYS               ((volatile uint8_t *)0xA2020250)
#define CKSYS_CLK_UPDATE_0__F_CHG_SFC               ((volatile uint8_t *)0xA2020251)
#define CKSYS_CLK_UPDATE_0__F_CHG_SPIMST0           ((volatile uint8_t *)0xA2020252)
#define CKSYS_CLK_UPDATE_0__F_CHG_SPIMST1           ((volatile uint8_t *)0xA2020253)
#define CKSYS_CLK_UPDATE_1__F_CHG_SPIMST2           ((volatile uint8_t *)0xA2020254)
#define CKSYS_CLK_UPDATE_1__F_CHG_SDIOMST           ((volatile uint8_t *)0xA2020255)
#define CKSYS_CLK_UPDATE_1__F_CHG_SPISLV            ((volatile uint8_t *)0xA2020256)
#define CKSYS_CLK_UPDATE_1__F_CHG_USB               ((volatile uint8_t *)0xA2020257)
#define CKSYS_CLK_UPDATE_2__F_CHG_AUD_GPSRC         ((volatile uint8_t *)0xA2020258)
#define CKSYS_CLK_UPDATE_2__F_CHG_EMI               ((volatile uint8_t *)0xA2020259)

#define SFC_DCM_CON_0__F_RG_SFC_DCM_DBC_NUM         ((volatile uint8_t *)0xA2020141)
#define SFC_DCM_CON_0__F_RG_SFC_DCM_DBC_EN          ((volatile uint8_t *)0xA2020142)
#define SFC_DCM_CON_1__F_RG_SFC_CLKOFF_EN           ((volatile uint8_t *)0xA2020144)
#define SFC_DCM_CON_0__F_RG_SFC_DCM_APB_SEL         ((volatile uint8_t *)0xA2020143)
#define SFC_DCM_CON_1__F_RG_SFC_DCM_APB_TOG         ((volatile uint8_t *)0xA2020147)
#define BUS_DCM_CON_0__F_RG_BUS_SFSEL               ((volatile uint8_t *)0xA2270100)
#define BUS_DCM_CON_0__F_RG_BUS_DCM_DBC_NUM         ((volatile uint8_t *)0xA2270101)
#define BUS_DCM_CON_0__F_RG_BUS_DCM_DBC_EN          ((volatile uint8_t *)0xA2270102)
#define DSP0_SLOW_CON4__F_RG_DSP0_SFSEL             ((volatile uint8_t *)0xA2270410)
#define DSP1_SLOW_CON4__F_RG_DSP1_SFSEL             ((volatile uint8_t *)0xA2270510)
#define BUS_DCM_CON_1__F_RG_BUS_CLKSLOW_EN          ((volatile uint8_t *)0xA2270105)
#define BUS_DCM_CON_1__F_RG_BUS_CLKOFF_EN           ((volatile uint8_t *)0xA2270104)
#define BUS_DCM_CON_0__F_RG_BUS_DCM_EN              ((volatile uint8_t *)0xA2270103)
#define DSP0_SLOW_CON0__F_RG_DSP0_SLOW_EN           ((volatile uint8_t *)0xA2270400)
#define DSP1_SLOW_CON0__F_RG_DSP1_SLOW_EN           ((volatile uint8_t *)0xA2270500)
#define CLK_FREQ_SWCH__F_RG_PLLCK_SEL               ((volatile uint8_t *)0xA2270170)
#define ABIST_FQMTR_CON0                            ((volatile uint16_t *)0xA2020400)
#define ABIST_FQMTR_CON1                            ((volatile uint16_t *)0xA2020404)
#define CKSYS_TST_SEL_1_F_TCKSEL                    ((volatile uint8_t *)0xA2020224)
#define CKSYS_TST_SEL_1_F_FCKSEL                    ((volatile uint8_t *)0xA2020225)
#define PLL_ABIST_FQMTR_DATA__F_FQMTR_DATA          ((volatile uint32_t *)0xA202040C)
#define HFOSC_CON0__F_RG_HFOSC_CALI                 ((volatile uint8_t *)0XA2040923)
#define HFOSC_CON0__F_RG_HFOSC_FT                   ((volatile uint8_t *)0XA2040922)

#define PDN_COND0_F_PDR_COND0                       ((volatile uint32_t *)0xA2270300)
#define PDN_SETD0_F_PDR_SETD0                       ((volatile uint32_t *)0xA2270310)
#define PDN_CLRD0_F_PDR_CLRD0                       ((volatile uint32_t *)0xA2270320)
#define PDN_COND0_F_PDR_COND1                       ((volatile uint32_t *)0xA2270330)
#define PDN_SETD0_F_PDR_SETD1                       ((volatile uint32_t *)0xA2270340)
#define PDN_CLRD0_F_PDR_CLRD1                       ((volatile uint32_t *)0xA2270350)
#define XO_PDN_COND0                                ((volatile uint32_t *)0xA2030B00)
#define XO_PDN_SETD0                                ((volatile uint32_t *)0xA2030B10)
#define XO_PDN_CLRD0                                ((volatile uint32_t *)0xA2030B20)

#define CLKSQ_CON0__F_DA_SRCLKENA                   ((volatile uint8_t *)0xA2040020)
#define DPM_CON1__F_UPLL_SETTLE_TIME                ((volatile uint16_t *)0xA2040096)
#define DPM_CPN2__F_MPLL_SETTLE_TIME                ((volatile uint16_t *)0xA2040098)
#define UPLL_CON0__F_DA_UPLL_EN                     ((volatile uint8_t *)0xA2040140)
#define UPLL_CON1__F_RG_UPLL_POSTDIV                ((volatile uint8_t *)0xA2040147)
#define PLL_CON2                                    ((volatile uint32_t *)0xA2040048)
#define PLL_CON3                                    ((volatile uint32_t *)0xA204004C)
#define PLLTD_CON0__F_BP_PLL_DLY                    ((volatile uint32_t *)0xA2040700)
#define UPLL_CON0__F_RG_UPLL_RDY                    ((volatile uint8_t *)0xA2040143)
#define MPLL_CON1__F_RG_MPLL_POSTDIV                ((volatile uint8_t *)0xA2040107)
#define PLL_CON4__F_RG_PLL_PGDET_EN                 ((volatile uint8_t *)0xA2040050)
#define MDDS_CON0__F_RG_MDDS_EN                     ((volatile uint8_t *)0xA2040640)
#define MPLL_CON0__F_DA_MPLL_EN                     ((volatile uint8_t *)0xA2040100)
#define MPLL_CON0__F_RG_MPLL_RDY                    ((volatile uint8_t *)0xA2040103)
#define CLKSQ_CON0__F_DA_SRCLKENA                   ((volatile uint8_t *)0xA2040020)
#define PLLTD_CON2__F_S1_STB_TIME                   ((volatile uint8_t *)0xA2040708)
#define CKSYS_CLK_CFG_4__F_USB_48M_SEL              ((volatile uint8_t *)0xA2020241)

#define APLL1_CTL0__F_RG_APLL1_V2I_EN               ((volatile uint8_t *)0xA2050003)
#define APLL1_CTL0__F_RG_APLL1_DDS_PWR_ON           ((volatile uint8_t *)0xA2050000)
#define APLL1_CTL0__F_RG_APLL1_DDS_ISO_EN           ((volatile uint8_t *)0xA2050001)
#define APLL1_CTL1__F_RG_APLL1_EN                   ((volatile uint8_t *)0xA2050004)
#define APLL1_CTL1__F_RG_APLL1_LCDDS_PWDB           ((volatile uint8_t *)0xA205002C)
#define APLL2_CTL0__F_RG_APLL2_V2I_EN               ((volatile uint8_t *)0xA2050103)
#define APLL2_CTL0__F_RG_APLL2_DDS_PWR_ON           ((volatile uint8_t *)0xA2050100)
#define APLL2_CTL0__F_RG_APLL2_DDS_ISO_EN           ((volatile uint8_t *)0xA2050101)
#define APLL2_CTL1__F_RG_APLL2_EN                   ((volatile uint8_t *)0xA2050104)
#define APLL2_CTL1__F_RG_APLL2_LCDDS_PWDB           ((volatile uint8_t *)0xA205012C)
#define APLL1_CTL12__F_RG_APLL1_LCDDS_PCW_NCPO_CHG  ((volatile uint8_t *)0xA2050030)
#define APLL2_CTL12__F_RG_APLL2_LCDDS_PCW_NCPO_CHG  ((volatile uint8_t *)0xA2050130)



#define APLL1_CTL14__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO ((volatile uint8_t *)0xA2050038)
#define APLL1_CTL12__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO ((volatile uint8_t *)0xA2050030)
#define APLL1_CTL13__F_RG_APLL1_LCDDS_TUNER_EN       ((volatile uint8_t *)0xA2050034)


#define APLL1_CTL15__F_RG_APLL1_LCDDS_PCW_NCPO       ((volatile uint8_t *)0xA205003C)
#define APLL1_CTL10__F_RG_APLL1_LCDDS_PCW_NCPO       ((volatile uint8_t *)0xA2050028)

#define APLL2_CTL15__F_RG_APLL2_LCDDS_PCW_NCPO       ((volatile uint8_t *)0xA205013C)
#define APLL2_CTL10__F_RG_APLL2_LCDDS_PCW_NCPO       ((volatile uint8_t *)0xA2050128)



#define APLL2_CTL14__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO ((volatile uint8_t *)0xA2050138)
#define APLL2_CTL12__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO ((volatile uint8_t *)0xA2050130)
#define APLL2_CTL13__F_RG_APLL2_LCDDS_TUNER_EN       ((volatile uint8_t *)0xA2050134)



#define DCXO_PCON4__F_DCXO_PWR_EN_TD                ((volatile uint8_t *)0XA2060010)
#define DCXO_PCON3__F_DCXO_EN_TD                    ((volatile uint8_t *)0XA206000F)
#define DCXO_PCON3__F_DCXO_BUF_EN_TD                ((volatile uint8_t *)0XA206000E)
#define DCXO_PCON3__F_DCXO_ISO_EN_TD                ((volatile uint8_t *)0XA206000D)
#define DCXO_PCON0__F_GSM_DCXO_CTL_EN               ((volatile uint8_t *)0XA2060000)
#define DCXO_PCON1__F_EXT_DCXO_CTL_EN               ((volatile uint8_t *)0XA2060007)
#define DCXO_PCON4__F_LPM_EN                        ((volatile uint8_t *)0XA2060012)
#define DCXO_PCON5__F_DCXO_ACAL_EFUSE               ((volatile uint8_t *)0XA2060016)
#define DCXO_PCON5__F_DCXO_ACAL_EFUSE_SEL           ((volatile uint8_t *)0XA2060014)
#define DCXO_PCON__F_DCXO_26M_RDY_EN                ((volatile uint8_t *)0XA2060013)
#define DCXO_CAP_ID                                 ((volatile uint32_t *)0xA2060018)  /*[24:16] cal data [0] sel*/
#define DCXO_CAP_ID_BIT                             16
#define DCXO_CAP_ID_MASK                            0x01FF0000
#define DCXO_PCON1__F_ENFRC_COCLK_EN                ((volatile uint8_t *)0XA2060006)

#define DSP0_SLOW_CON0                              ((volatile uint8_t *)0XA2270400)
#define DSP0_SLOW_CON1                              ((volatile uint8_t *)0XA2270404)
#define DSP0_SLOW_CON2                              ((volatile uint8_t *)0XA2270408)
#define DSP0_SLOW_CON3                              ((volatile uint8_t *)0XA227040C)

#define DSP1_SLOW_CON0                              ((volatile uint8_t *)0XA2270500)
#define DSP1_SLOW_CON1                              ((volatile uint8_t *)0XA2270504)
#define DSP1_SLOW_CON2                              ((volatile uint8_t *)0XA2270508)
#define DSP1_SLOW_CON3                              ((volatile uint8_t *)0XA227050C)

#define CKSYS_CLK_DIV_4__F_CHOP_DIV_SEL             ((volatile uint16_t *)0XA2020292)
#define CKSYS_CLK_DIV_4__F_CHOP_DIV_CHG             ((volatile uint8_t *)0XA2020291)
#define CKSYS_CLK_DIV_4__F_CHOP_DIV_EN              ((volatile uint8_t *)0XA2020290)

#define FH_CON5__F_MPLL_FRDDS_DNLMT                 ((volatile uint8_t*)(0XA2040514))
#define FH_CON5__F_MPLL_FRDDS_UPLMT                 ((volatile uint8_t*)(0XA2040516))
#define FH_CON3__F_MPLL_FRDDS_DTS                   ((volatile uint8_t*)(0xA204050D))
#define FH_CON3__F_MPLL_FRDDS_DYS                   ((volatile uint8_t*)(0xA204050E))
#define FH_CON4__F_MPLL_FHCTL_EN                    ((volatile uint8_t*)(0xA2040510))
#define FH_CON4__F_MPLL_FRDDS_EN                    ((volatile uint8_t*)(0xA2040512))

#define FH_CON3__F_MPLL_SFSTR_DYS                   ((volatile uint8_t*)(0xA204050C))
#define FH_CON4__F_MPLL_SFSTR_DTS                   ((volatile uint8_t*)(0xA2040513))
#define FH_CON4__F_MPLL_SFSTR_EN                    ((volatile uint8_t*)(0xA2040511))
#define FH_CON0__F_PLL_FHSET                        ((volatile uint16_t*)(0xA2040500))
#define FH_CON0__F_PLL_FREQ_STR                     ((volatile uint8_t*)(0xA2040502))

#define SSC_CON1__F_HFOSC_SFSTR_DYS                 ((volatile uint8_t *)0xA2040955)
#define SSC_CON0__F_HFOSC_SFSTR_EN                  ((volatile uint8_t *)0xA2040951)
#define SSC_CON4__F_HFOSC_FHSET                     ((volatile uint16_t *)0xA2040962)
#define SSC_CON4__F_HFOSC_FREQ_STR                  ((volatile uint8_t *)0xA2040960)
#define SSC_CON0__F_HFOSC_FHCTL_EN                  ((volatile uint8_t *)0xA2040950)
#define SSC_CON3__F_HFOSC_FRSSC_UPLMT               ((volatile uint16_t *)0xA204095C)
#define SSC_CON3__F_HFOSC_FRSSC_DNLMT               ((volatile uint16_t *)0xA204095E)
#define SSC_CON0__F_HFOSC_FRSSC_EN                  ((volatile uint8_t *)0xA2040952)

/*************************************************************************
 * Define clock mux register and bits structure
 *************************************************************************/

typedef enum {
    CLK_SYS_SEL,
    CLK_SFC_SEL,
    CLK_SPIMST0_SEL,
    CLK_SDIOMST_SEL,
    CLK_SPISLV_SEL,
    CLK_USB_SEL,
    CLK_AUD_BUS_SEL,
    CLK_AUD_GPSRC_SEL,
    CLK_AUD_INTERFACE0_SEL,
    CLK_EMI_SEL,
    CLK_SPM_SEL,
    CLK_26M_SEL,
    CLK_AUD_DL_HIRES,
    CLK_AUD_UL_HIRES,
    NR_MUXS,
    CLK_SPIMST1_SEL,
    CLK_SPIMST2_SEL,
    CLK_AUD_INTERFACE1_SEL,
    CLK_MUX_SEL_NONE,
} clock_mux_sel_id;


/*************************************************************************
* Define clock gating register and bits structure
* Note: MUST modify clk_cg_mask in hal_clock.c if hal_clock_cg_id is changed
*************************************************************************/
#if 0 //marcus
enum {
    NR_CLOCKS           = HAL_CLOCK_CG_END
};
#endif /* #if 0 //marcus */

/*************************************************************************
 * Define clock PLLs
 *************************************************************************/
typedef enum {
    UPLL,
    MPLL,
    OSC,
    APLL1,
    APLL2,
    XO
} source_t;

typedef enum {
    CLK_UPLL,
    CLK_MPLL,
    CLK_GPLL = 1,   /* UPLL/MPLL */
    CLK_OSC,
    CLK_APLL1,
    CLK_APLL2,
    NR_PLLS,
    CLK_XO
} clock_pll_id;

typedef enum {
    CLK_UPLL_312M,
    CLK_UPLL_624M,
    NR_ULLS_CONFIG,
} clock_upll_config;

typedef enum {
    CLK_DSP0,
    CLK_DSP1,
    NR_DSP,
} clock_dsp_num;

#endif /* #ifdef HAL_CLOCK_MODULE_ENABLED */
#endif /* #ifndef __HAL_CLOCK_PLATFORM_AB155X_H__ */
