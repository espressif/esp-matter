/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_AUDIO_FW_SHERIF_H__
#define __HAL_AUDIO_FW_SHERIF_H__

#include <stdint.h>
#include "hal_audio_fw_dpram.h"
#include "mt7933.h"

/* Base address definition */
#define DPRAM_CPU_BASE      IDMA_DM
#define MD2GCONFG_BASE      MD2G_CONFG
#define SHARE_BASE          SHARE_D1
#define PATCH_BASE          PATCH
#define AFE_BASE            AFE
#define ABBSYS_SD_BASE      ABBSYS_BASE
#define MDCONFIG_BASE       MODEM_CONFG_BASE
#define IDMA_CM0            IDMA_CM
#define IDMA_PM0            IDMA_PM
#define IDMA_DM0            IDMA_DM

/* Access Macros  */
#define DPRAM_BASE(a)           ((volatile uint16_t *)DPRAM_CPU_BASE + (a))
#define DSP_CM_ADDR(page,addr)  ((volatile uint32_t *)(IDMA_CM0 + ((page) << 16) + (addr << 2)))
#define DSP_PM_ADDR(page,addr)  ((volatile uint16_t *)(IDMA_PM0 + ((page) << 16) + (addr << 1)))
#define DSP_DM_ADDR(page,addr)  ((volatile uint16_t *)(IDMA_DM0 + ((page) << 16) + (addr << 1)))
#define HW_WRITE(ptr,data)      (*(ptr) = (data))
#define HW_READ(ptr)            (*(ptr))
#define DSP_WRITE(ptr,data)     HW_WRITE(ptr,data)
#define DSP_READ(ptr)           HW_READ(ptr)
#define SHERIF_WRITE(ptr,data)  DSP_WRITE(ptr,data)
#define SHERIF_READ(ptr)        DSP_READ(ptr)

/* Common definition */
/* Hardware Registers */
#define DSP_START_CON           ((volatile uint32_t *)(IDMA_CM0 + 0x0000))
#define DSP_CLK_CON             ((volatile uint16_t *)(MD2GCONFG_BASE + 0x040))  /* DSP Clock Control Register                           */
#define SHARE_DSPCON            ((volatile uint16_t *)(SHARE_BASE + 0x0000))     /* DSP Control Register                                 */
#define SHARE_M2DI1             ((volatile uint16_t *)(SHARE_BASE + 0x0004))     /* MCU-to-DSP Interrupt1 Register                       */
#define SHARE_M2DI2             ((volatile uint16_t *)(SHARE_BASE + 0x0008))     /* MCU-to-DSP Interrupt2 Register                       */
#define SHARE_D2MCON            ((volatile uint16_t *)(SHARE_BASE + 0x000C))     /* DSP-to-MCU Interrupt Control Register                */
#define SHARE_D2MSTA            ((volatile uint16_t *)(SHARE_BASE + 0x0010))     /* DSP-to-MCU Interrupt Status Register                 */
#define SHARE_D2MTID            ((volatile uint16_t *)(SHARE_BASE + 0x0014))     /* DSP Task Identification Register                     */
#define SHARE_PWDNCON           ((volatile uint16_t *)(SHARE_BASE + 0x0048))     /* CNTR POWER DOWN CONTROL                              */
#define SHARE_DSP1CKR           ((volatile uint16_t *)(SHARE_BASE + 0x004C))     /* Slave DSP Clock Switch Register(for deep sleep mode) */
#define MCU_PATCH_START_ADDR    ((volatile uint32_t *)(PATCH_BASE + 0x0000))
#define MCU_PATCH_ENABLE        ((volatile uint32_t *)(PATCH_BASE + 0x0100))
#define SHARE_D2M_WAKEUP_CTL    ((volatile uint16_t *)(SHARE_BASE + 0x0078))    /*16bit*/
#define SHARE_D2M_WAKEUP_STA    ((volatile uint16_t *)(SHARE_BASE + 0x007C))   /*16bit*/

/* DSP Memories */
#define DSP_DM_DUMMY_RW1        DSP_DM_ADDR(5, 0x0000)  /* Dummy write / read DSP DM address */
#define DSP_DM_DUMMY_RW2        DSP_DM_ADDR(5, 0x0002)  /* Dummy write / read DSP DM address */

/* DSP interrupt */
#define DSP_D2M_TASK0           ((volatile uint16_t *)(SHARE_BASE + 0x0050))     /* 0th FW task interrupt                                */
#define DSP_D2M_TASK1           ((volatile uint16_t *)(SHARE_BASE + 0x0054))     /* 1st FW task interrupt                                */
#define DSP_D2M_TASK2           ((volatile uint16_t *)(SHARE_BASE + 0x0058))     /* 2nd FW task interrupt                                */
#define DSP_D2M_TASK3           ((volatile uint16_t *)(SHARE_BASE + 0x005C))     /* 3rd FW task interrupt                                */
#define DSP_D2M_TASK4           ((volatile uint16_t *)(SHARE_BASE + 0x0060))     /* 4th FW task interrupt                                */
#define DSP_D2M_TASK5           ((volatile uint16_t *)(SHARE_BASE + 0x0064))     /* 5th FW task interrupt                                */
#define DSP_D2M_TASK6           ((volatile uint16_t *)(SHARE_BASE + 0x0068))     /* 6th FW task interrupt                                */
#define DSP_D2M_TASK7           ((volatile uint16_t *)(SHARE_BASE + 0x006C))     /* 7th FW task interrupt                                */

/* Audio definition  */
#define DSP_DSP_STATUS                     DPRAM_BASE(DP_DSP_STATUS)
#define DSP_MCU_STATUS                     DPRAM_BASE(DP_MCU_STATUS)
#define DSP_MCU_STATUS_MTCMOS              DPRAM_BASE(DP_MCU_STATUS_MTCMOS)
#define DSP_DSP_STATUS_MTCMOS              DPRAM_BASE(DP_DSP_STATUS_MTCMOS)
#define DSP_NB_RES_S0_0                    DPRAM_BASE(DP_NB_RES_S0_0)                  /*DSP_PATCH_START_ADDR*/
#define DSP_DYNAMIC_DOWNLOAD_MCU_STATUS    DPRAM_BASE(DYNAMIC_DOWNLOAD_MCU_STATUS)     /*DSP_DDL_MCU_STATUS*/
#define DSP_DDL_MCU_DSP_HAND_SHAKE         DPRAM_BASE(DP2_DDL_MCU_DSP_HAND_SHAKE)      /*DSP_DDL_HANDSHAKE*/
#define DSP_SLOW_IDLE_DIVIDER              DPRAM_BASE(DP_SLOW_IDLE_DIVIDER)            /*DSP_SLOW_IDLE_CTRL*/
#define DSP_SLT_READY                      DPRAM_BASE(SPH_BGS_UL_GAIN)
#define DSP_SLT_CHECKSUM                   DPRAM_BASE(SPH_BGS_DL_GAIN)

#define DSP_OFFSET_COUNT                   DPRAM_BASE(DP2_Offset_Count)
#define DSP_SP_CODEC_CTRL_BASE             DPRAM_BASE(DP2_SP_CODEC_CTRL_BASE)
#define DSP_SPH_COD_CTRL                   DPRAM_BASE(SPH_COD_CTRL)
#define DSP_AUDIO_ASP_TYPE_ADDR            DPRAM_BASE(DP_AUDIO_ASP_TYPE_ADDR)
#define DSP_AUDIO_ASP_FS_ADDR              DPRAM_BASE(DP_AUDIO_ASP_FS_ADDR)            /*DSP_ASP_FS*/
#define DSP_AUDIO_ASP_DEL_W_ADDR           DPRAM_BASE(DP_Audio_ASP_DEL_W_ADDR)         /*DSP_ASP_DEL_W*/
#define DSP_AUDIO_ASP_DEL_MARGIN_W_ADDR    DPRAM_BASE(DP_Audio_ASP_DEL_MARGIN_W_ADDR)  /*SP_ASP_DEL_M*/
#define DSP_SPH_DEL_R                      DPRAM_BASE(SPH_DEL_R)
#define DSP_SPH_DEL_W                      DPRAM_BASE(SPH_DEL_W)
#define DSP_SPH_DEL_M                      DPRAM_BASE(SPH_DEL_M)
#define DSP_AUDIO_DEC_FRAME_LENGTH         DPRAM_BASE(DP_AUDIO_DEC_FRAME_LENGTH)
#define DSP_AUDIO_ASP_WAV_OUT_GAIN         DPRAM_BASE(AUDIO_ASP_WAV_OUT_GAIN)          /*DSP_PCM_DGAIN, target gain*/
#define DSP_AUDIO_CUR_GAIN                 DPRAM_BASE(DP_AUDIO_CUR_GAIN)               /*DSP_PCM_CGAIN, current gain */
#define DSP_AUDIO_GAIN_STEP                DPRAM_BASE(DP_AUDIO_GAIN_STEP)              /*DSP_PCM_GAINSTEP*/
#define DSP_AUDIO_ASP_COMMON_FLAG_1        DPRAM_BASE(DP_AUDIO_ASP_COMMON_FLAG_1)      /*DSP_AUDIO_ASP_COMMON_FLAG_1*/
#define DSP_AUDIO_RAMPDOWN_ENABLE          DPRAM_BASE(AUDIO_RAMPDOWN_ENABLE)           /*DSP_AUDIO_RAMPDOWN_ENABLE*/
#define DSP_AUDIO_SD_ISR_CYCLE             DPRAM_BASE(Audio_SD_ISR_Cycle)              /*DSP_AUDIO_ISR_CYCLE*/
#define DSP_AUDIO_ASP_COMMON_FLAG_2        DPRAM_BASE(DP_AUDIO_ASP_COMMON_FLAG_2)      /*DSP_AUDIO_ASP_COMMON_FLAG_2*/
#define DSP_AUDIO_ASP_D2M_COUNT            DPRAM_BASE(DP_AUDIO_ASP_D2M_COUNT)          /*DSP_AUDIO_ASP_D2M_COUNT*/
#define DSP_TASK4_COSIM_HANDSHAKE          DPRAM_BASE(DP_TASK4_COSIM_HANDSHAKE)        /*DSP_AUDIO_ASP_TASK4_DM_HANDSHAKE*/
#define DSP_SPH_DL_VOL                     DPRAM_BASE(SPH_DL_VOL)                      /*DSP_VOL_OUT_PCM*/
#define DSP_SPH_UL_VOL                     DPRAM_BASE(SPH_UL_VOL)                      /*DSP_VOL_IN_PCM*/
#define DSP_AUDIO_CTRL2                    DPRAM_BASE(DP_AUDIO_CTRL2)                  /*DSP_AUDIO_CTRL*/
#define DSP_AUDIO_FLEXI_CTRL               DPRAM_BASE(DP_Audio_Flexi_Ctrl)
#define DSP_AUDIO_PP_CTRL_BASE             DPRAM_BASE(DP2_AUDIO_PP_CTRL_BASE)
#define DSP_AUDIO_PP_CTRL                  DPRAM_BASE(DP2_AUDIO_PP_CTRL)               /*DSP_AUDIO_PP_CTRL*/
#define DSP_ASP_WAV_R_IO_ADDR              DPRAM_BASE(DP_ASP_WAV_R_IO_ADDR)
#define DSP_ASP_WAV_W_IO_ADDR              DPRAM_BASE(DP_ASP_WAV_W_IO_ADDR)
#define DSP_ASP_WAV_BASE_CTRL              DPRAM_BASE(DP_ASP_WAV_BASE_CTRL)
#define DSP_AUDIO_DEL_CNTR_THRESHOLD       DPRAM_BASE(DP_AUDIO_DEC_CNTR_THRESHOLD)
#define DSP_AUDIO_ASP_COMMON_FLAG_1        DPRAM_BASE(DP_AUDIO_ASP_COMMON_FLAG_1)
#define DSP_SPH_PCM_REC_CTRL               DPRAM_BASE(SPH_PCM_REC_CTRL)
#define DSP_AUDIO_CHANNEL_ASSIGNMENT       DPRAM_BASE(AUDIO_CH_ASSIGNMENT)
#define DSP_AUDIO_CLOCK_COUNTER            DPRAM_BASE(AUDIO_8K_INT_COUNT)

/* PCM route sherif */
#define DSP_PCM_ROUTE_DATA_PAGENUM         DPRAM_BASE(PCM_Route_Data_Pagenum)
#define DSP_PCM_ROUTE_DATA_ADDRESS         DPRAM_BASE(PCM_Route_Data_Address)
#define DSP_PCM_ROUTE_DATA_LENGTH          DPRAM_BASE(PCM_Route_Data_Length)
#define DSP_AUDIO_PP_TS_APM_TO_DSP_LENGTH  DPRAM_BASE(AUDIO_PP_TS_APM_to_DSP_Length)

/* Audio Low Power definition */
#define DSP_AUDLP_D2M_COUNT                DPRAM_BASE(MP3_CONTROL_4)
#define DSP_AUDLP_MCUBUFF_THRESHOLD        DPRAM_BASE(MP3_CONTROL_5)
#define DSP_AUDLP_MCUBUFF_SIZE             DPRAM_BASE(MP3_CONTROL_6)

/* Speech definition */
#define DSP_D2C_SPEECH_UL_INT              DPRAM_BASE(DP_D2C_SPEECH_UL_INT)
#define DSP_D2C_SPEECH_DL_INT              DPRAM_BASE(DP_D2C_SPEECH_DL_INT)
#define DSP_ASP_FORCE_KT_8K                DPRAM_BASE(DP_ASP_FORCE_KT_8K)
#define DSP_SPH_DM_ADDR_PNW_UL_BUF         DPRAM_BASE(SPH_DM_ADDR_PNW_UL_BUF)
#define DSP_SPH_DM_ADDR_PNW_DL_BUF         DPRAM_BASE(SPH_DM_ADDR_PNW_DL_BUF)
#define DSP_SPH_DUMIC_CTRL                 DPRAM_BASE(SPH_DUMIC_CTRL)
#define DSP_SPH_DBG_MOD                    DPRAM_BASE(SPH_DBG_MOD)
#define DSP_SPH_DL_VOL                     DPRAM_BASE(SPH_DL_VOL)                       /*DP_VOL_OUT_PCM*/
#define DSP_SPH_PNW_BUF_DL2                DPRAM_BASE(SPH_DM_ADDR_PNW_DL_BUF)
#define DSP_SPH_PNW_BUF_UL1                DPRAM_BASE(SPH_DM_ADDR_PNW_UL_BUF)
#define DSP_SPH_PNW_CTRL_UL                DPRAM_BASE(SPH_PNW_CTRL_UL)
#define DSP_SPH_PNW_CTRL_DL                DPRAM_BASE(SPH_PNW_CTRL_DL)
#define DSP_SPH_BT_CTRL                    DPRAM_BASE(SPH_BT_CTRL)                      /*DP_BT_PATCH_CTRL*/
#define DSP_SPH_BT_MODE                    DPRAM_BASE(SPH_BT_MODE)
#define DSP_SPH_DL_VOL                     DPRAM_BASE(SPH_DL_VOL)                       /*DP_VOL_OUT_PCM*/
#define DSP_SPH_8K_CTRL                    DPRAM_BASE(SPH_8K_CTRL)
#define DSP_SPH_SFE_CTRL                   DPRAM_BASE(SPH_SFE_CTRL)
#define DSP_AGC_GAIN                       DPRAM_BASE(DP2_AGC_GAIN)
#define DSP_AGC_CTRL                       DPRAM_BASE(DP2_AGC_CTRL)
#define DSP_SPH_ST_VOL                     DPRAM_BASE(SPH_ST_VOL)                       /*DP_SIDETONE_VOL*/
#define DSP_EC_SUPP                        DPRAM_BASE(DP2_EC_SUPP)
#define DSP_AEC_CTRL                       DPRAM_BASE(DP_EES_CTRL)
#define DSP_SPH_EMP_ADDR                   DPRAM_BASE(SPH_DM_ADDR_EMP)
#define DSP_AUDIO_PAR                      DPRAM_BASE(AUDIO_PAR)
#define DSP_SPH_SCH_IMPROVE_CTRL           DPRAM_BASE(SPH_SCH_IMPROVE_CTRL)
#define DSP_8k_SP_CTRL                     DPRAM_BASE(DP2_8k_SP_CTRL)
#define DSP_20ms_SP_CTRL                   DPRAM_BASE(DP2_20ms_SP_CTRL)
#define DSP_SC_MUTE                        DPRAM_BASE(DP2_SC_MUTE)
#define DSP_SPH_MUTE_CTRL                  DPRAM_BASE(SPH_MUTE_CTRL)
#define DSP_SC_MODE                        DPRAM_BASE(DP2_SC_MODE)
#define DSP_AWB_8K_BASE                    DPRAM_BASE(DP2_AWB_8K_BASE)
#define DSP_SPH_DM_ADDR_SE_PTR             DPRAM_BASE(SPH_DM_ADDR_SE_PTR)
#define DSP_SPH_SE2_PTR                    0x3321
#define DSP_SPH_ENH_DYNAMIC_SWITCH        DPRAM_BASE(SPH_ENH_DYNAMIC_SWITCH)

/* Speech VM related */
#define DSP_SPH_COD_CTRL                   DPRAM_BASE(SPH_COD_CTRL)
#define DSP_SPH_EPL_BND                    DPRAM_BASE(SPH_EPL_BND)
#define DSP_SPH_EPL_UL_PRE_BUF             DPRAM_BASE(SPH_DM_ADDR_EPL_UL_PRE_BUF)
#define DSP_SPH_EPL_UL_POS_BUF             DPRAM_BASE(SPH_DM_ADDR_EPL_UL_POS_BUF)
#define DSP_SPH_EPL_DL_PRE_BUF             DPRAM_BASE(SPH_DM_ADDR_EPL_DL_PRE_BUF)
#define DSP_SPH_EPL_DL_POS_BUF             DPRAM_BASE(SPH_DM_ADDR_EPL_DL_POS_BUF)
#define DSP_SPH_VM_DBG_INFO                DPRAM_BASE(DP_VM_DBG_INFO)
#define DSP_SPH_2G_SD_DATA_HB              DPRAM_BASE(SPH_2G_SD_DATA_HB)
#define DSP_SPH_2G_SE_DATA_HB              DPRAM_BASE(SPH_2G_SE_DATA_HB)
#define DSP_SPH_3G_SD_DATA_HB              DPRAM_BASE(SPH_3G_SD_DATA_HB)
#define DSP_SPH_3G_SE_DATA_HB              DPRAM_BASE(SPH_3G_SE_DATA_HB)
#define DSP_SPH_2G_SD_DATA_HDR             DPRAM_BASE(SPH_2G_SD_DATA_HDR)
#define DSP_SPH_2G_SE_DATA_HDR             DPRAM_BASE(SPH_2G_SE_DATA_HDR)
#define DSP_SPH_3G_SD_DATA_HDR             DPRAM_BASE(SPH_3G_SD_DATA_HDR)
#define DSP_SPH_3G_SE_DATA_HDR             DPRAM_BASE(SPH_3G_SE_DATA_HDR)
#define DSP_SPH_Decoder_Used_Mode          DPRAM_BASE(DP_Decoder_Used_Mode)
#define DSP_SPH_Encoder_Used_Mode          DPRAM_BASE(DP_Encoder_Used_Mode)
#define DSP_SPH_AGC_SW_GAIN1               DPRAM_BASE(DP_AGC_SW_GAIN1)
#define DSP_SPH_AGC_SW_GAIN2               DPRAM_BASE(DP_AGC_SW_GAIN2)

/*Speech filter related*/
#define DSP_COMPEN_BLOCK_FLT_PAR           DPRAM_BASE(DP2_COMPEN_BLOCK_FILTER)
#define DSP_COMPEN_BLOCK_FLT_PAR_PTR       DSP_COMPEN_BLOCK_FLT_PAR
#define DSP_SPH_FLT_COEF_ADDR_ST           DPRAM_BASE(SPH_PM_ADDR_ST_FLT_COEF)
#define DSP_SPH_FLT_COEF_ADDR_BKF_NB_UL    DPRAM_BASE(SPH_PM_ADDR_BKF_FLT_COEF_UL_NB)
#define DSP_SPH_FLT_COEF_ADDR_BKF_NB_DL    DPRAM_BASE(SPH_PM_ADDR_BKF_FLT_COEF_DL_NB)
#define DSP_SPH_FLT_COEF_ADDR_SRC          DPRAM_BASE(SPH_PM_ADDR_SRC_FLT_COEF)
#define DSP_SPH_FLT_COEF_ADDR_BKF_WB_UL    DPRAM_BASE(SPH_PM_ADDR_BKF_FLT_COEF_UL_WB)
#define DSP_SPH_FLT_COEF_ADDR_BKF_WB_DL    DPRAM_BASE(SPH_PM_ADDR_BKF_FLT_COEF_DL_WB)
#define DSP_SPH_FLT_COEF_ADDR_AGC          0x11A

/*Noise reduction control*/
#define DSP_SPE_DL_DLL_ENTRY               DPRAM_BASE(SPH_ENH_DL_CTRL)
#define DSP_UL_NR_CTRL                     DPRAM_BASE(DP_NR_CTRL)
#define DSP_DL_NR_CTRL                     DPRAM_BASE(DP_DL_NR_CTRL)

/*Stream in comfort noise*/
#define DSP_UL_COMFORT_NOISE_THRESHOLD     DPRAM_BASE(DP_UL_COMFORT_NOISE_THRESHOLD)
#define DSP_UL_COMFORT_NOISE_SHIFT         DPRAM_BASE(DP_UL_COMFORT_NOISE_SHIFT)
#define DSP_SPH_AFE_VDSP_CON_CTRL          DPRAM_BASE(SPH_AFE_VDSP_CON_CTRL)

/* I2S definition */
#define DSP_I2S_PLAYERROR_CONTROL          DPRAM_BASE(MP3_CONTROL_4)
#define DSP_I2S_PLAYERROR_INFO             DPRAM_BASE(MP3_CONTROL_5)

/* SBC decoder definition */
#define DSP_SBC_DEC_CTRL                   DPRAM_BASE(SBC_DEC_CTRL)
#define DSP_SBC_PAR_MAGIC_WORD             DPRAM_BASE(SBC_PAR_MAGIC_WORD)
#define DSP_SBC_DEC_STATUS                 DPRAM_BASE(SBC_DEC_STATUS)
#define DSP_SBC_DEC_DM_BS_PAGE             DPRAM_BASE(SBC_DEC_DM_BS_PAGE)
#define DSP_SBC_DEC_DM_BS_ADDR             DPRAM_BASE(SBC_DEC_DM_BS_ADDR)
#define DSP_SBC_DEC_DM_BS_LEN              DPRAM_BASE(SBC_DEC_DM_BS_LEN)
#define DSP_SBC_DEC_DM_BS_REQ_TH           DPRAM_BASE(SBC_DEC_DM_BS_REQ_TH)
#define DSP_SBC_DEC_DM_BS_MCU_W_PTR        DPRAM_BASE(SBC_DEC_DM_BS_MCU_W_PTR)
#define DSP_SBC_DEC_DM_BS_DSP_R_PTR        DPRAM_BASE(SBC_DEC_DM_BS_DSP_R_PTR)

/* AAC decoder definition */
#define DSP_AAC_MAIN_CONTROL               DPRAM_BASE(MP3_CONTROL_BASE_ADD)
#define DSP_AAC_DEC_DM_BS_ADDR             DPRAM_BASE(MP3_CONTROL_2)
#define DSP_AAC_DEC_DM_BS_LEN              DPRAM_BASE(MP3_CONTROL_3)
#define DSP_AAC_DEC_DM_BS_DSP_R_PTR        DPRAM_BASE(MP3_CONTROL_4)
#define DSP_AAC_DEC_DM_BS_MCU_W_PTR        DPRAM_BASE(MP3_CONTROL_5)
#define DSP_AAC_DEC_ERROR_REPORT           DPRAM_BASE(MP3_CONTROL_6)
#define DSP_AAC_DEC_DUAL_SCE               DPRAM_BASE(MP3_CONTROL_8)
#define DSP_AAC_DEC_ALLERROR_REPORT        DPRAM_BASE(MP3_CONTROL_9)

/* PCM SD task playback definition */
#define DSP_PCM_SD_PLAYBACK_CTRL           DPRAM_BASE(PCM_SD_PLAYBACK_CTRL)
#define DSP_PCM_SD_PLAYBACK_DM_MCU_W_PTR   DPRAM_BASE(PCM_SD_PLAYBACK_DM_MCU_W_PTR)
#define DSP_PCM_SD_PLAYBACK_DM_DSP_R_PTR   DPRAM_BASE(PCM_SD_PLAYBACK_DM_DSP_R_PTR)

/* Audio IIR filter definition */
#define DSP_AUD_IIR_CTRL                   DPRAM_BASE(AUDIO_IIR_CTRL)

/* I2S Rx + DSP audio decoder playback definition */
#define DSP_I2S_RX_AUD_DEC_CTRL            DPRAM_BASE(I2S_RX_AUD_DEC_CTRL)
#define DSP_I2S_RX_AUD_DEC_FRAME_LENGTH    DPRAM_BASE(I2S_RX_AUD_DEC_FRAME_LENGTH)
#define DSP_I2S_RX_AUD_DEC_PAGE_NUMBER     DPRAM_BASE(I2S_RX_AUD_DEC_PAGE_NUMBER)
#define DSP_I2S_RX_AUD_DEC_ADDRESS         DPRAM_BASE(I2S_RX_AUD_DEC_ADDRESS)
#define DSP_TASK6_HANDSHAKE                DPRAM_BASE(DP_TASK6_HANDSHAKE)

#define DSP_SPH_8K_DIS_VBI_CTRL            DPRAM_BASE(SPH_8K_DIS_VBI_CTRL)
#define DSP_SPH_UL_D2M_CTRL                DPRAM_BASE(SPH_UL_D2M_CTRL)
#define DSP_SPH_UL_PERIOD                  DPRAM_BASE(SPH_UL_PERIOD)
#define DSP_AUD_CLK_SKEW_PIN_VAL           DPRAM_BASE(AUD_CLK_SKEW_PIN_VAL)
#define DSP_AUD_CLK_SKEW_PON_VAL           DPRAM_BASE(AUD_CLK_SKEW_PON_VAL)
#define DSP_AUD_CLK_PIN_PON_IDX            DPRAM_BASE(AUD_CLK_PIN_PON_IDX)

#define DSP_ADD_POINT_PIN                  DSP_PM_ADDR(3, 0x3C00)
#define DSP_ADD_POINT_PON                  DSP_PM_ADDR(3, 0x3C60)
#define DSP_ADD_POINT_IDX                  DSP_PM_ADDR(3, 0x3CC0)

#define DSP_SPH_UL_WAIT_CTRL               DPRAM_BASE(SPH_UL_WAIT_CTRL)
#define DSP_SPH_UL_WAIT_TICK               DPRAM_BASE(SPH_UL_WAIT_TICK)

/* NDVC*/
#define DSP_SPH_SWNDVC_POWER_INDEX         DPRAM_BASE(SPH_SWNDVC_PowerIndex)

#endif /* #ifndef __HAL_AUDIO_FW_SHERIF_H__ */
