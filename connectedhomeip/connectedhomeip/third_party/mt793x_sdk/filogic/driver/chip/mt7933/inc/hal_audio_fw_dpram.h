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

#ifndef __HAL_AUDIO_FW_DPRAM_H__
#define __HAL_AUDIO_FW_DPRAM_H__

/* Shared Memory Field */
#define SHERIF_START_ADDR                         0x3a00

#define DP_DSP_TASK_STATUS_BASE                   0x3a00 /* 0x3a00 */

#define DP_DSP_STATUS                             DP_DSP_TASK_STATUS_BASE + 0x000 /* 0x3a00 */
#define DP_MCU_STATUS                             DP_DSP_TASK_STATUS_BASE + 0x001 /* 0x3a01 */
#define DP_DSP_STATUS_MTCMOS                      DP_DSP_TASK_STATUS_BASE + 0x002 /* 0x3a02 */
#define DP_MCU_STATUS_MTCMOS                      DP_DSP_TASK_STATUS_BASE + 0x003 /* 0x3a03 */
#define DEBUG_ASSERT_CTRL                         DP_DSP_TASK_STATUS_BASE + 0x004 /* 0x3a04 */
#define DTIRQ_DEBUG_ASSERT_CHECK                  DP_DSP_TASK_STATUS_BASE + 0x005 /* 0x3a05 */
#define DP_RX_WIN_COUNT                           DP_DSP_TASK_STATUS_BASE + 0x006 /* 0x3a06 */
#define DP2_RX_S0_CD_DONE                         DP_DSP_TASK_STATUS_BASE + 0x007 /* 0x3a07 */
#define DP_SLOW_IDLE_DIVIDER                      DP_DSP_TASK_STATUS_BASE + 0x008 /* 0x3a08 */
#define DP_DSP_ROM_VERSION                        DP_DSP_TASK_STATUS_BASE + 0x009 /* 0x3a09 */

#define DP_RX_WINDOW_BASE                         DP_DSP_TASK_STATUS_BASE + 0x00a /* 0x3a0a */

#define DP_RX_WIN0_PROC                           DP_RX_WINDOW_BASE + 0x000 /* 0x3a0a */
#define DP_RX_WIN1_PROC                           DP_RX_WINDOW_BASE + 0x001 /* 0x3a0b */
#define DP_RX_WIN2_PROC                           DP_RX_WINDOW_BASE + 0x002 /* 0x3a0c */
#define DP_RX_WIN3_PROC                           DP_RX_WINDOW_BASE + 0x003 /* 0x3a0d */
#define DP_RX_WIN4_PROC                           DP_RX_WINDOW_BASE + 0x004 /* 0x3a0e */
#define DP_RX_WIN5_PROC                           DP_RX_WINDOW_BASE + 0x005 /* 0x3a0f */
#define DP_RX_WIN6_PROC                           DP_RX_WINDOW_BASE + 0x006 /* 0x3a10 */
#define DP_RX_WIN7_PROC                           DP_RX_WINDOW_BASE + 0x007 /* 0x3a11 */
#define DP_RX_WIN0_FREQ                           DP_RX_WINDOW_BASE + 0x008 /* 0x3a12 */
#define DP_RX_WIN1_FREQ                           DP_RX_WINDOW_BASE + 0x009 /* 0x3a13 */
#define DP_RX_WIN2_FREQ                           DP_RX_WINDOW_BASE + 0x00a /* 0x3a14 */
#define DP_RX_WIN3_FREQ                           DP_RX_WINDOW_BASE + 0x00b /* 0x3a15 */
#define DP_RX_WIN4_FREQ                           DP_RX_WINDOW_BASE + 0x00c /* 0x3a16 */
#define DP_RX_WIN5_FREQ                           DP_RX_WINDOW_BASE + 0x00d /* 0x3a17 */

#define DP2_TX_WINDOW_BASE                        DP_RX_WINDOW_BASE + 0x00e /* 0x3a18 */

#define DP2_TX_WIN0_PROC                          DP2_TX_WINDOW_BASE + 0x000 /* 0x3a18 */
#define DP2_TX_WIN1_PROC                          DP2_TX_WINDOW_BASE + 0x001 /* 0x3a19 */
#define DP2_TX_WIN2_PROC                          DP2_TX_WINDOW_BASE + 0x002 /* 0x3a1a */
#define DP2_TX_WIN3_PROC                          DP2_TX_WINDOW_BASE + 0x003 /* 0x3a1b */
#define DP2_TX_WIN4_PROC                          DP2_TX_WINDOW_BASE + 0x004 /* 0x3a1c */

#define DP2_ENCRYPTION_BASE                       DP2_TX_WINDOW_BASE + 0x005 /* 0x3a1d */

#define DP2_CRYPT_PAR_0                           DP2_ENCRYPTION_BASE + 0x000 /* 0x3a1d */
#define DP2_ENCRYP_KEY_1                          DP2_ENCRYPTION_BASE + 0x001 /* 0x3a1e */
#define DP2_ENCRYP_KEY_2                          DP2_ENCRYPTION_BASE + 0x002 /* 0x3a1f */
#define DP2_ENCRYP_KEY_3                          DP2_ENCRYPTION_BASE + 0x003 /* 0x3a20 */
#define DP2_ENCRYP_KEY_4                          DP2_ENCRYPTION_BASE + 0x004 /* 0x3a21 */

#define DP_EQ_CHANNEL_CODEC_BASE                  DP2_ENCRYPTION_BASE + 0x005 /* 0x3a22 */

#define DP_CHEST_ReNull_SN_Alpha                  DP_EQ_CHANNEL_CODEC_BASE + 0x000 /* 0x3a22 */
#define DP_RX_BUFFER_LEN                          DP_EQ_CHANNEL_CODEC_BASE + 0x001 /* 0x3a23 */
#define DP_NB_TRX                                 DP_EQ_CHANNEL_CODEC_BASE + 0x002 /* 0x3a24 */
#define DP_FCCH_CTRL                              DP_EQ_CHANNEL_CODEC_BASE + 0x003 /* 0x3a25 */
#define DP_FCB_CONSEC                             DP_EQ_CHANNEL_CODEC_BASE + 0x004 /* 0x3a26 */
#define DP_FCB1_LANDA_FXP                         DP_EQ_CHANNEL_CODEC_BASE + 0x005 /* 0x3a27 */
#define DP_FCB2_LANDA_FXP                         DP_EQ_CHANNEL_CODEC_BASE + 0x006 /* 0x3a28 */
#define DP_FCB1_COST_THRESHOLD_FXP                DP_EQ_CHANNEL_CODEC_BASE + 0x007 /* 0x3a29 */
#define DP_FCB2_COST_THRESHOLD_FXP                DP_EQ_CHANNEL_CODEC_BASE + 0x008 /* 0x3a2a */
#define DP_FCB_FOE_TH                             DP_EQ_CHANNEL_CODEC_BASE + 0x009 /* 0x3a2b */
#define DP_SHORT_FB_BURST_LEN                     DP_EQ_CHANNEL_CODEC_BASE + 0x00a /* 0x3a2c */
#define DP_SHORT_FB_RESCALING_FACTOR              DP_EQ_CHANNEL_CODEC_BASE + 0x00b /* 0x3a2d */
#define DP_SHORT_FB_RESOLUTION                    DP_EQ_CHANNEL_CODEC_BASE + 0x00c /* 0x3a2e */
#define DP_SHORT_FB_DC_AVE_LEN                    DP_EQ_CHANNEL_CODEC_BASE + 0x00d /* 0x3a2f */
#define DP_SHORT_FB_BER_THR                       DP_EQ_CHANNEL_CODEC_BASE + 0x00e /* 0x3a30 */
#define DP_SHORT_FB_DIST_THR                      DP_EQ_CHANNEL_CODEC_BASE + 0x00f /* 0x3a31 */
#define DP_SHORT_SB_BURST_LEN                     DP_EQ_CHANNEL_CODEC_BASE + 0x010 /* 0x3a32 */
#define DP_SHORT_SB_RESCALING_FACTOR              DP_EQ_CHANNEL_CODEC_BASE + 0x011 /* 0x3a33 */
#define DP_SHORT_SB_RESOLUTION                    DP_EQ_CHANNEL_CODEC_BASE + 0x012 /* 0x3a34 */
#define DP_FB_2nd_INIT_FLAG                       DP_EQ_CHANNEL_CODEC_BASE + 0x013 /* 0x3a35 */
#define DP_CC_FLAG                                DP_EQ_CHANNEL_CODEC_BASE + 0x014 /* 0x3a36 */
#define DP_PM_length_ctrl                         DP_EQ_CHANNEL_CODEC_BASE + 0x015 /* 0x3a37 */
#define DP_PHASE_IMB_FACTOR                       DP_EQ_CHANNEL_CODEC_BASE + 0x016 /* 0x3a38 */
#define DP_STEPDC_THRES                           DP_EQ_CHANNEL_CODEC_BASE + 0x017 /* 0x3a39 */
#define DP_CHEST_LEN_Alpha                        DP_EQ_CHANNEL_CODEC_BASE + 0x018 /* 0x3a3a */
#define DP_Static_State                           DP_EQ_CHANNEL_CODEC_BASE + 0x019 /* 0x3a3b */
#define DP_CW_POSITION                            DP_EQ_CHANNEL_CODEC_BASE + 0x01a /* 0x3a3c */
#define DP_QBIT_CW_SWITCH                         DP_EQ_CHANNEL_CODEC_BASE + 0x01b /* 0x3a3d */
#define DP_NB_RANGE_WIN                           DP_EQ_CHANNEL_CODEC_BASE + 0x01c /* 0x3a3e */

#define DP_EQ_SAIC_BASE                           DP_EQ_CHANNEL_CODEC_BASE + 0x01d /* 0x3a3f */

#define DP_RX_SAIC_SWITCH                         DP_EQ_SAIC_BASE + 0x000 /* 0x3a3f */
#define DP_RX_SAIC_THRES                          DP_EQ_SAIC_BASE + 0x001 /* 0x3a40 */
#define DP_EPSK_SAIC_PWR_TH                       DP_EQ_SAIC_BASE + 0x002 /* 0x3a41 */
#define DP_EPSK_SAIC_SN_TH                        DP_EQ_SAIC_BASE + 0x003 /* 0x3a42 */
#define DP_ANT_PWR_OFFSET                         DP_EQ_SAIC_BASE + 0x004 /* 0x3a43 */
#define DP_DSP_PWR_UNDERFLOW                      DP_EQ_SAIC_BASE + 0x005 /* 0x3a44 */
#define DP_BEP_K1_GMSK                            DP_EQ_SAIC_BASE + 0x006 /* 0x3a45 */
#define DP_BEP_K2_GMSK                            DP_EQ_SAIC_BASE + 0x007 /* 0x3a46 */
#define DP_BEP_K1_EPSK                            DP_EQ_SAIC_BASE + 0x008 /* 0x3a47 */
#define DP_BEP_K2_EPSK                            DP_EQ_SAIC_BASE + 0x009 /* 0x3a48 */
#define DP_BEP_K1_GMSK_SAIC_ON                    DP_EQ_SAIC_BASE + 0x00a /* 0x3a49 */
#define DP_BEP_K2_GMSK_SAIC_ON                    DP_EQ_SAIC_BASE + 0x00b /* 0x3a4a */
#define DP_BEP_K1_EPSK_SAIC_ON                    DP_EQ_SAIC_BASE + 0x00c /* 0x3a4b */
#define DP_BEP_K2_EPSK_SAIC_ON                    DP_EQ_SAIC_BASE + 0x00d /* 0x3a4c */

#define DP_EQ_CONTROL_BASE                        DP_EQ_SAIC_BASE + 0x00e /* 0x3a4d */

#define DP_MODDET_THRES                           DP_EQ_CONTROL_BASE + 0x000 /* 0x3a4d */
#define DP_CHEST_LEN35_SN_Th                      DP_EQ_CONTROL_BASE + 0x001 /* 0x3a4e */
#define DP_CHEST_LEN_TSNR                         DP_EQ_CONTROL_BASE + 0x002 /* 0x3a4f */
#define DP_CHEST_LEN_T23                          DP_EQ_CONTROL_BASE + 0x003 /* 0x3a50 */
#define DP_CHEST_LEN_T35                          DP_EQ_CONTROL_BASE + 0x004 /* 0x3a51 */
#define DP_CHEST_LEN_T57                          DP_EQ_CONTROL_BASE + 0x005 /* 0x3a52 */
#define DP_CHEST_NT_GMSK                          DP_EQ_CONTROL_BASE + 0x006 /* 0x3a53 */
#define DP_CHEST_NT_EPSK                          DP_EQ_CONTROL_BASE + 0x007 /* 0x3a54 */
#define DP_CHEST_LEN_Alpha_1Thr                   DP_EQ_CONTROL_BASE + 0x008 /* 0x3a55 */
#define DP_CHEST_LEN_Alpha_0Thr                   DP_EQ_CONTROL_BASE + 0x009 /* 0x3a56 */
#define DP_CHEST_NULL_SNThr_H                     DP_EQ_CONTROL_BASE + 0x00a /* 0x3a57 */
#define DP_CHEST_NULL_SNThr_L                     DP_EQ_CONTROL_BASE + 0x00b /* 0x3a58 */
#define DP_CHEST_ReNull_Th                        DP_EQ_CONTROL_BASE + 0x00c /* 0x3a59 */
#define DP_CHEST_ReNull_Alpha_Cnt_Th              DP_EQ_CONTROL_BASE + 0x00d /* 0x3a5a */
#define DP_CHEST_SAIC_para_NB                     DP_EQ_CONTROL_BASE + 0x00e /* 0x3a5b */
#define DP_CHEST_SAIC_para_SB                     DP_EQ_CONTROL_BASE + 0x00f /* 0x3a5c */
#define DP_CHEST_SAIC_SP_CTRL                     DP_EQ_CONTROL_BASE + 0x010 /* 0x3a5d */
#define DP_CHEST_SAIC_LEN_T35_SNH                 DP_EQ_CONTROL_BASE + 0x011 /* 0x3a5e */
#define DP_CHEST_SAIC_LEN_T35_SNL                 DP_EQ_CONTROL_BASE + 0x012 /* 0x3a5f */
#define DP_CHEST_SAIC_CIRTHRES                    DP_EQ_CONTROL_BASE + 0x013 /* 0x3a60 */
#define DP_AMR_LS_DC_SNThr                        DP_EQ_CONTROL_BASE + 0x014 /* 0x3a61 */
#define DP_EPSK_SAIC_PARA                         DP_EQ_CONTROL_BASE + 0x015 /* 0x3a62 */
#define DP_SINR_GMSK_FACTOR                       DP_EQ_CONTROL_BASE + 0x016 /* 0x3a63 */
#define DP_SINR_EPSK_FACTOR_SAIC_ON               DP_EQ_CONTROL_BASE + 0x017 /* 0x3a64 */
#define DP_SINR_EPSK_FACTOR_SAIC_OFF              DP_EQ_CONTROL_BASE + 0x018 /* 0x3a65 */
#define DP_FOE_SINR_Shift                         DP_EQ_CONTROL_BASE + 0x019 /* 0x3a66 */
#define DP_FOE_GMSK_FACTOR                        DP_EQ_CONTROL_BASE + 0x01a /* 0x3a67 */
#define DP_FOE_EPSK_FACTOR                        DP_EQ_CONTROL_BASE + 0x01b /* 0x3a68 */
#define DP_FOE_LEN                                DP_EQ_CONTROL_BASE + 0x01c /* 0x3a69 */
#define DP_FOE_Inv_SumK2                          DP_EQ_CONTROL_BASE + 0x01d /* 0x3a6a */
#define DP_CS4_BEP_TH1                            DP_EQ_CONTROL_BASE + 0x01e /* 0x3a6b */
#define DP_EQCFG_PFLEN                            DP_EQ_CONTROL_BASE + 0x01f /* 0x3a6c */
#define DP_EQCFG_LMS                              DP_EQ_CONTROL_BASE + 0x020 /* 0x3a6d */
#define DP_EQCFG_LMSTHRES                         DP_EQ_CONTROL_BASE + 0x021 /* 0x3a6e */
#define DP_EQCFG_PLL                              DP_EQ_CONTROL_BASE + 0x022 /* 0x3a6f */
#define DP_EQCFG_PLLTHRES                         DP_EQ_CONTROL_BASE + 0x023 /* 0x3a70 */
#define DP_EQCFG_DARPPLL                          DP_EQ_CONTROL_BASE + 0x024 /* 0x3a71 */
#define DP_EQCFG_DARPPLLTHRES                     DP_EQ_CONTROL_BASE + 0x025 /* 0x3a72 */
#define DP_EQCFG_SAIC_LMS_StepSize                DP_EQ_CONTROL_BASE + 0x026 /* 0x3a73 */
#define DP_SINR_FWBW_THRES                        DP_EQ_CONTROL_BASE + 0x027 /* 0x3a74 */

#define DP_VAMOS_CONTROL_BASE                     DP_EQ_CONTROL_BASE + 0x028 /* 0x3a75 */

#define DP_VAMOS_ENABLE                           DP_VAMOS_CONTROL_BASE + 0x000 /* 0x3a75 */
#define DP_VAMOS_TSCSNR_SH                        DP_VAMOS_CONTROL_BASE + 0x001 /* 0x3a76 */
#define DP_VAMOS_TSCSNR_CONST                     DP_VAMOS_CONTROL_BASE + 0x002 /* 0x3a77 */
#define DP_VAMOS_BEP_K3                           DP_VAMOS_CONTROL_BASE + 0x003 /* 0x3a78 */
#define DP_VAMOS_BEP_K4                           DP_VAMOS_CONTROL_BASE + 0x004 /* 0x3a79 */
#define DP_VAMOS_SAIC_PARA                        DP_VAMOS_CONTROL_BASE + 0x005 /* 0x3a7a */
#define DP2_VAMOS_BFI_MEAN_SNTH                   DP_VAMOS_CONTROL_BASE + 0x006 /* 0x3a7b */
#define DP2_VAMOS_BFI_D_TH                        DP_VAMOS_CONTROL_BASE + 0x007 /* 0x3a7c */
#define DP_VAMOS_BFI_SN_THRES                     DP_VAMOS_CONTROL_BASE + 0x008 /* 0x3a7d */
#define DP_FOE_AQPSK_FACTOR                       DP_VAMOS_CONTROL_BASE + 0x009 /* 0x3a7e */
#define DP_SOC3_Enable_DC_cancellation            DP_VAMOS_CONTROL_BASE + 0x00a /* 0x3a7f */
#define DP_VAMOS_ZETA_TH                          DP_VAMOS_CONTROL_BASE + 0x00b /* 0x3a80 */
#define DP_VAMOS_LS_DC_SNThr                      DP_VAMOS_CONTROL_BASE + 0x00c /* 0x3a81 */
#define DP_VAMOS_LS_DC_PThr1                      DP_VAMOS_CONTROL_BASE + 0x00d /* 0x3a82 */
#define DP_VAMOS_LS_DC_PThr2                      DP_VAMOS_CONTROL_BASE + 0x00e /* 0x3a83 */

#define DP_VAMOS_ZETA_BASE                        DP_VAMOS_CONTROL_BASE + 0x010 /* 0x3a85 */

#define DP_VAMOS_ZETA_THR_10                      DP_VAMOS_ZETA_BASE + 0x000 /* 0x3a85 */
#define DP_VAMOS_ZETA_THR_11                      DP_VAMOS_ZETA_BASE + 0x001 /* 0x3a86 */
#define DP_VAMOS_ZETA_THR_12                      DP_VAMOS_ZETA_BASE + 0x002 /* 0x3a87 */
#define DP_VAMOS_ZETA_THR_13                      DP_VAMOS_ZETA_BASE + 0x003 /* 0x3a88 */
#define DP_VAMOS_ZETA_THR_20                      DP_VAMOS_ZETA_BASE + 0x004 /* 0x3a89 */
#define DP_VAMOS_ZETA_THR_21                      DP_VAMOS_ZETA_BASE + 0x005 /* 0x3a8a */
#define DP_VAMOS_ZETA_THR_22                      DP_VAMOS_ZETA_BASE + 0x006 /* 0x3a8b */
#define DP_VAMOS_ZETA_THR_23                      DP_VAMOS_ZETA_BASE + 0x007 /* 0x3a8c */
#define DP_VAMOS_ZETA_THR_30                      DP_VAMOS_ZETA_BASE + 0x008 /* 0x3a8d */
#define DP_VAMOS_ZETA_THR_31                      DP_VAMOS_ZETA_BASE + 0x009 /* 0x3a8e */
#define DP_VAMOS_ZETA_THR_32                      DP_VAMOS_ZETA_BASE + 0x00a /* 0x3a8f */
#define DP_VAMOS_ZETA_THR_33                      DP_VAMOS_ZETA_BASE + 0x00b /* 0x3a90 */
#define DP_VAMOS_ZETA_THR_40                      DP_VAMOS_ZETA_BASE + 0x00c /* 0x3a91 */
#define DP_VAMOS_ZETA_THR_41                      DP_VAMOS_ZETA_BASE + 0x00d /* 0x3a92 */
#define DP_VAMOS_ZETA_THR_50                      DP_VAMOS_ZETA_BASE + 0x00e /* 0x3a93 */
#define DP_VAMOS_ZETA_THR_51                      DP_VAMOS_ZETA_BASE + 0x00f /* 0x3a94 */

#define DP_VAMOS_BEP_BASE                         DP_VAMOS_ZETA_BASE + 0x010 /* 0x3a95 */

#define DP_VAMOS_BEP_SCALING_0                    DP_VAMOS_BEP_BASE + 0x000 /* 0x3a95 */
#define DP_VAMOS_BEP_OFFSET_0                     DP_VAMOS_BEP_BASE + 0x001 /* 0x3a96 */
#define DP_VAMOS_BEP_SCALING_1                    DP_VAMOS_BEP_BASE + 0x002 /* 0x3a97 */
#define DP_VAMOS_BEP_OFFSET_1                     DP_VAMOS_BEP_BASE + 0x003 /* 0x3a98 */
#define DP_VAMOS_BEP_SCALING_2                    DP_VAMOS_BEP_BASE + 0x004 /* 0x3a99 */
#define DP_VAMOS_BEP_OFFSET_2                     DP_VAMOS_BEP_BASE + 0x005 /* 0x3a9a */
#define DP_VAMOS_BEP_SCALING_3                    DP_VAMOS_BEP_BASE + 0x006 /* 0x3a9b */
#define DP_VAMOS_BEP_OFFSET_3                     DP_VAMOS_BEP_BASE + 0x007 /* 0x3a9c */
#define DP_VAMOS_BEP_SCALING_4                    DP_VAMOS_BEP_BASE + 0x008 /* 0x3a9d */
#define DP_VAMOS_BEP_OFFSET_4                     DP_VAMOS_BEP_BASE + 0x009 /* 0x3a9e */
#define DP_VAMOS_BEP_SCALING_5                    DP_VAMOS_BEP_BASE + 0x00a /* 0x3a9f */
#define DP_VAMOS_BEP_OFFSET_5                     DP_VAMOS_BEP_BASE + 0x00b /* 0x3aa0 */
#define DP_VAMOS_BEP_SCALING_6                    DP_VAMOS_BEP_BASE + 0x00c /* 0x3aa1 */
#define DP_VAMOS_BEP_OFFSET_6                     DP_VAMOS_BEP_BASE + 0x00d /* 0x3aa2 */
#define DP_VAMOS_BEP_SCALING_7                    DP_VAMOS_BEP_BASE + 0x00e /* 0x3aa3 */
#define DP_VAMOS_BEP_OFFSET_7                     DP_VAMOS_BEP_BASE + 0x00f /* 0x3aa4 */
#define DP_VAMOS_BEP_SCALING_8                    DP_VAMOS_BEP_BASE + 0x010 /* 0x3aa5 */
#define DP_VAMOS_BEP_OFFSET_8                     DP_VAMOS_BEP_BASE + 0x011 /* 0x3aa6 */
#define DP_VAMOS_BEP_SCALING_9                    DP_VAMOS_BEP_BASE + 0x012 /* 0x3aa7 */
#define DP_VAMOS_BEP_OFFSET_9                     DP_VAMOS_BEP_BASE + 0x013 /* 0x3aa8 */
#define DP_VAMOS_BEP_SCALING_10                   DP_VAMOS_BEP_BASE + 0x014 /* 0x3aa9 */
#define DP_VAMOS_BEP_OFFSET_10                    DP_VAMOS_BEP_BASE + 0x015 /* 0x3aaa */
#define DP_VAMOS_BEP_SCALING_11                   DP_VAMOS_BEP_BASE + 0x016 /* 0x3aab */
#define DP_VAMOS_BEP_OFFSET_11                    DP_VAMOS_BEP_BASE + 0x017 /* 0x3aac */
#define DP_VAMOS_BEP_SCALING_12                   DP_VAMOS_BEP_BASE + 0x018 /* 0x3aad */
#define DP_VAMOS_BEP_OFFSET_12                    DP_VAMOS_BEP_BASE + 0x019 /* 0x3aae */

#define DP_VAMOS_AMRQI_BASE                       DP_VAMOS_BEP_BASE + 0x01a /* 0x3aaf */

#define DP_VAMOS_AMRQI_SCALING_0                  DP_VAMOS_AMRQI_BASE + 0x000 /* 0x3aaf */
#define DP_VAMOS_AMRQI_OFFSET_0                   DP_VAMOS_AMRQI_BASE + 0x001 /* 0x3ab0 */
#define DP_VAMOS_AMRQI_SCALING_1                  DP_VAMOS_AMRQI_BASE + 0x002 /* 0x3ab1 */
#define DP_VAMOS_AMRQI_OFFSET_1                   DP_VAMOS_AMRQI_BASE + 0x003 /* 0x3ab2 */
#define DP_VAMOS_AMRQI_SCALING_2                  DP_VAMOS_AMRQI_BASE + 0x004 /* 0x3ab3 */
#define DP_VAMOS_AMRQI_OFFSET_2                   DP_VAMOS_AMRQI_BASE + 0x005 /* 0x3ab4 */
#define DP_VAMOS_AMRQI_SCALING_3                  DP_VAMOS_AMRQI_BASE + 0x006 /* 0x3ab5 */
#define DP_VAMOS_AMRQI_OFFSET_3                   DP_VAMOS_AMRQI_BASE + 0x007 /* 0x3ab6 */
#define DP_VAMOS_AMRQI_SCALING_4                  DP_VAMOS_AMRQI_BASE + 0x008 /* 0x3ab7 */
#define DP_VAMOS_AMRQI_OFFSET_4                   DP_VAMOS_AMRQI_BASE + 0x009 /* 0x3ab8 */
#define DP_VAMOS_AMRQI_SCALING_5                  DP_VAMOS_AMRQI_BASE + 0x00a /* 0x3ab9 */
#define DP_VAMOS_AMRQI_OFFSET_5                   DP_VAMOS_AMRQI_BASE + 0x00b /* 0x3aba */
#define DP_VAMOS_AMRQI_SCALING_6                  DP_VAMOS_AMRQI_BASE + 0x00c /* 0x3abb */
#define DP_VAMOS_AMRQI_OFFSET_6                   DP_VAMOS_AMRQI_BASE + 0x00d /* 0x3abc */
#define DP_VAMOS_AMRQI_SCALING_7                  DP_VAMOS_AMRQI_BASE + 0x00e /* 0x3abd */
#define DP_VAMOS_AMRQI_OFFSET_7                   DP_VAMOS_AMRQI_BASE + 0x00f /* 0x3abe */
#define DP_VAMOS_AMRQI_SCALING_8                  DP_VAMOS_AMRQI_BASE + 0x010 /* 0x3abf */
#define DP_VAMOS_AMRQI_OFFSET_8                   DP_VAMOS_AMRQI_BASE + 0x011 /* 0x3ac0 */
#define DP_VAMOS_AMRQI_SCALING_9                  DP_VAMOS_AMRQI_BASE + 0x012 /* 0x3ac1 */
#define DP_VAMOS_AMRQI_OFFSET_9                   DP_VAMOS_AMRQI_BASE + 0x013 /* 0x3ac2 */
#define DP_VAMOS_AMRQI_SCALING_10                 DP_VAMOS_AMRQI_BASE + 0x014 /* 0x3ac3 */
#define DP_VAMOS_AMRQI_OFFSET_10                  DP_VAMOS_AMRQI_BASE + 0x015 /* 0x3ac4 */
#define DP_VAMOS_AMRQI_SCALING_11                 DP_VAMOS_AMRQI_BASE + 0x016 /* 0x3ac5 */
#define DP_VAMOS_AMRQI_OFFSET_11                  DP_VAMOS_AMRQI_BASE + 0x017 /* 0x3ac6 */
#define DP_VAMOS_AMRQI_SCALING_12                 DP_VAMOS_AMRQI_BASE + 0x018 /* 0x3ac7 */
#define DP_VAMOS_AMRQI_OFFSET_12                  DP_VAMOS_AMRQI_BASE + 0x019 /* 0x3ac8 */

#define DP_IRDMA_BASE                             DP_VAMOS_AMRQI_BASE + 0x01a /* 0x3ac9 */

#define DP_IRDMA_LA                               DP_IRDMA_BASE + 0x000 /* 0x3ac9 */
#define DP_IRDMA_UA                               DP_IRDMA_BASE + 0x001 /* 0x3aca */
#define DP_IRDMA_FB                               DP_IRDMA_BASE + 0x002 /* 0x3acb */
#define DP_IRDMA_SB                               DP_IRDMA_BASE + 0x003 /* 0x3acc */
#define DP_IRDMA_TB                               DP_IRDMA_BASE + 0x004 /* 0x3acd */
#define DP_IRDMA_ENA                              DP_IRDMA_BASE + 0x005 /* 0x3ace */
#define DP_BUF_SIZE                               DP_IRDMA_BASE + 0x006 /* 0x3acf */
#define DP_SUB_BUF_NUM                            DP_IRDMA_BASE + 0x007 /* 0x3ad0 */

#define DP_DEBUG_CTRL_BASE                        DP_IRDMA_BASE + 0x008 /* 0x3ad1 */

#define DP_DB_CNTL                                DP_DEBUG_CTRL_BASE + 0x000 /* 0x3ad1 */
#define DP_DB_FLT_MD0                             DP_DEBUG_CTRL_BASE + 0x001 /* 0x3ad2 */
#define DP_DB_FLT_MD1                             DP_DEBUG_CTRL_BASE + 0x002 /* 0x3ad3 */
#define DP_DB_FLT_MD2                             DP_DEBUG_CTRL_BASE + 0x003 /* 0x3ad4 */
#define DP_DB_FLT_MD3                             DP_DEBUG_CTRL_BASE + 0x004 /* 0x3ad5 */
#define DP_DB_FLT_SP                              DP_DEBUG_CTRL_BASE + 0x005 /* 0x3ad6 */
#define SPH_IRDBG_FLT                             DP_DEBUG_CTRL_BASE + 0x005 /* 0x3ad6 */
#define DP_DB_FLT_AU                              DP_DEBUG_CTRL_BASE + 0x006 /* 0x3ad7 */
#define DP_DB_PTR                                 DP_DEBUG_CTRL_BASE + 0x007 /* 0x3ad8 */

#define DP_MCU_DEBUG_INFO_BASE                    DP_DEBUG_CTRL_BASE + 0x008 /* 0x3ad9 */

#define DP_MCU_TQWRAP                             DP_MCU_DEBUG_INFO_BASE + 0x000 /* 0x3ad9 */
#define DP_MCU_AFC                                DP_MCU_DEBUG_INFO_BASE + 0x001 /* 0x3ada */
#define DP_MCU_ARFCN_WIN0                         DP_MCU_DEBUG_INFO_BASE + 0x002 /* 0x3adb */
#define DP_MCU_ARFCN_WIN1                         DP_MCU_DEBUG_INFO_BASE + 0x003 /* 0x3adc */
#define DP_MCU_ARFCN_WIN2                         DP_MCU_DEBUG_INFO_BASE + 0x004 /* 0x3add */
#define DP_MCU_ARFCN_WIN3                         DP_MCU_DEBUG_INFO_BASE + 0x005 /* 0x3ade */
#define DP_MCU_ARFCN_WIN4                         DP_MCU_DEBUG_INFO_BASE + 0x006 /* 0x3adf */
#define DP_MCU_AGC_WIN0                           DP_MCU_DEBUG_INFO_BASE + 0x007 /* 0x3ae0 */
#define DP_MCU_AGC_WIN1                           DP_MCU_DEBUG_INFO_BASE + 0x008 /* 0x3ae1 */
#define DP_MCU_AGC_WIN2                           DP_MCU_DEBUG_INFO_BASE + 0x009 /* 0x3ae2 */
#define DP_MCU_AGC_WIN3                           DP_MCU_DEBUG_INFO_BASE + 0x00a /* 0x3ae3 */
#define DP_MCU_AGC_WIN4                           DP_MCU_DEBUG_INFO_BASE + 0x00b /* 0x3ae4 */
#define DP_MCU_AGC_WIN5                           DP_MCU_DEBUG_INFO_BASE + 0x00c /* 0x3ae5 */

#define DP_LOGI_CHANNEL_BASE                      DP_MCU_DEBUG_INFO_BASE + 0x00d /* 0x3ae6 */

#define DP_GSM_MODE                               DP_LOGI_CHANNEL_BASE + 0x000 /* 0x3ae6 */
#define DP_TCH_S0_FLAGS                           DP_LOGI_CHANNEL_BASE + 0x001 /* 0x3ae7 */
#define DP_TCH_S1_FLAGS                           DP_LOGI_CHANNEL_BASE + 0x002 /* 0x3ae8 */
#define DP_TCH_S2_FLAGS                           DP_LOGI_CHANNEL_BASE + 0x003 /* 0x3ae9 */
#define DP_TCH_S3_FLAGS                           DP_LOGI_CHANNEL_BASE + 0x004 /* 0x3aea */
#define DP_TCH_S4_FLAGS                           DP_LOGI_CHANNEL_BASE + 0x005 /* 0x3aeb */
#define DP_SACCH_S0_POS                           DP_LOGI_CHANNEL_BASE + 0x006 /* 0x3aec */
#define DP_NB_TSC                                 DP_LOGI_CHANNEL_BASE + 0x007 /* 0x3aed */
#define DP2_TX_GPRS_CONSTELLATION0                DP_LOGI_CHANNEL_BASE + 0x008 /* 0x3aee */
#define DP2_TX_GPRS_CONSTELLATION1                DP_LOGI_CHANNEL_BASE + 0x009 /* 0x3aef */
#define DP2_fast_PCH_enable                       DP_LOGI_CHANNEL_BASE + 0x00a /* 0x3af0 */
#define DP2_REPEATED_ACCH_GOOD_FLAG               DP_LOGI_CHANNEL_BASE + 0x00b /* 0x3af1 */
#define DP2_REPEATED_ACCH_ENABLE_FLAG             DP_LOGI_CHANNEL_BASE + 0x00c /* 0x3af2 */
#define DP2_2nd_CD_Enable_Flags                   DP_LOGI_CHANNEL_BASE + 0x00d /* 0x3af3 */
#define DP2_2nd_CD_Type                           DP_LOGI_CHANNEL_BASE + 0x00e /* 0x3af4 */
#define DP2_2nd_CD_Lenght_info                    DP_LOGI_CHANNEL_BASE + 0x00f /* 0x3af5 */
#define DP2_2nd_CD_Good_Flags                     DP_LOGI_CHANNEL_BASE + 0x010 /* 0x3af6 */
#define DP2_FACCH_LIKE_THRES                      DP_LOGI_CHANNEL_BASE + 0x011 /* 0x3af7 */
#define DP2_FACCH_LIKE_FLAG                       DP_LOGI_CHANNEL_BASE + 0x012 /* 0x3af8 */
#define DP2_FACCH_THRES                           DP_LOGI_CHANNEL_BASE + 0x013 /* 0x3af9 */
#define DP2_BFI_MEAN_SNTH                         DP_LOGI_CHANNEL_BASE + 0x014 /* 0x3afa */
#define DP2_BFI_D_TH                              DP_LOGI_CHANNEL_BASE + 0x015 /* 0x3afb */
#define DP_BFI_SN_THRES                           DP_LOGI_CHANNEL_BASE + 0x016 /* 0x3afc */
#define DP_LP_BACK_MODE                           DP_LOGI_CHANNEL_BASE + 0x017 /* 0x3afd */

#define DP_EGPRS_CHANNEL_CODEC_BASE               DP_LOGI_CHANNEL_BASE + 0x018 /* 0x3afe */

#define DP2_EDGE_S0_MCS                           DP_EGPRS_CHANNEL_CODEC_BASE + 0x000 /* 0x3afe */
#define DP2_EDGE_S1_MCS                           DP_EGPRS_CHANNEL_CODEC_BASE + 0x001 /* 0x3aff */
#define DP2_EDGE_S2_MCS                           DP_EGPRS_CHANNEL_CODEC_BASE + 0x002 /* 0x3b00 */
#define DP2_EDGE_S3_MCS                           DP_EGPRS_CHANNEL_CODEC_BASE + 0x003 /* 0x3b01 */
#define DP2_EDGE_S4_MCS                           DP_EGPRS_CHANNEL_CODEC_BASE + 0x004 /* 0x3b02 */
#define DP2_EDGE_SRB_CTRL                         DP_EGPRS_CHANNEL_CODEC_BASE + 0x005 /* 0x3b03 */
#define DP2_IR_CTRL                               DP_EGPRS_CHANNEL_CODEC_BASE + 0x006 /* 0x3b04 */
#define DP2_RLC_WIN_SIZE                          DP_EGPRS_CHANNEL_CODEC_BASE + 0x007 /* 0x3b05 */
#define DP2_RLC_VQ                                DP_EGPRS_CHANNEL_CODEC_BASE + 0x008 /* 0x3b06 */
#define DP2_IR_DEC_CTRL                           DP_EGPRS_CHANNEL_CODEC_BASE + 0x009 /* 0x3b07 */
#define DP2_IR_MEM_THRE_HIGH                      DP_EGPRS_CHANNEL_CODEC_BASE + 0x00a /* 0x3b08 */
#define DP2_IR_MEM_THRE_LOW                       DP_EGPRS_CHANNEL_CODEC_BASE + 0x00b /* 0x3b09 */
#define DP2_IR_MEM_ADDR_L                         DP_EGPRS_CHANNEL_CODEC_BASE + 0x00c /* 0x3b0a */
#define DP2_IR_MEM_ADDR_H                         DP_EGPRS_CHANNEL_CODEC_BASE + 0x00d /* 0x3b0b */
#define DP2_AVAIL_IR_MEM_BLOCK_NUM                DP_EGPRS_CHANNEL_CODEC_BASE + 0x00e /* 0x3b0c */
#define DP2_IR_MEM_BLOCK_NUM                      DP_EGPRS_CHANNEL_CODEC_BASE + 0x00f /* 0x3b0d */

#define DP2_PERI_CTRL_BASE                        DP_EGPRS_CHANNEL_CODEC_BASE + 0x010 /* 0x3b0e */

#define DP2_PERI_CTRL_CONFIG                      DP2_PERI_CTRL_BASE + 0x000 /* 0x3b0e */
#define DP2_PERI_CTRL_DSP_READ1                   DP2_PERI_CTRL_BASE + 0x001 /* 0x3b0f */
#define DP2_PERI_CTRL_DSP_WRITE1                  DP2_PERI_CTRL_BASE + 0x002 /* 0x3b10 */
#define DP2_PERI_CTRL_DSP_READ2                   DP2_PERI_CTRL_BASE + 0x003 /* 0x3b11 */
#define DP2_PERI_CTRL_DSP_WRITE2                  DP2_PERI_CTRL_BASE + 0x004 /* 0x3b12 */
#define DP2_PERI_CTRL_DSP_READ3                   DP2_PERI_CTRL_BASE + 0x005 /* 0x3b13 */
#define DP2_PERI_CTRL_DSP_WRITE3                  DP2_PERI_CTRL_BASE + 0x006 /* 0x3b14 */

#define DP2_AMR_CTRL_BASE                         DP2_PERI_CTRL_BASE + 0x007 /* 0x3b15 */

#define DP2_AMR_T1                                DP2_AMR_CTRL_BASE + 0x000 /* 0x3b15 */
#define DP2_AMR_T2                                DP2_AMR_CTRL_BASE + 0x001 /* 0x3b16 */
#define DP2_AMR_T3                                DP2_AMR_CTRL_BASE + 0x002 /* 0x3b17 */
#define DP2_AMR_FLAGS                             DP2_AMR_CTRL_BASE + 0x003 /* 0x3b18 */
#define DP2_AMR_ICM                               DP2_AMR_CTRL_BASE + 0x004 /* 0x3b19 */
#define DP2_AMR_DL_ACS_01                         DP2_AMR_CTRL_BASE + 0x005 /* 0x3b1a */
#define DP2_AMR_DL_ACS_23                         DP2_AMR_CTRL_BASE + 0x006 /* 0x3b1b */
#define DP2_AMR_UL_ACS_01                         DP2_AMR_CTRL_BASE + 0x007 /* 0x3b1c */
#define DP2_AMR_UL_ACS_23                         DP2_AMR_CTRL_BASE + 0x008 /* 0x3b1d */
#define DP2_AMR_Config_Flags                      DP2_AMR_CTRL_BASE + 0x009 /* 0x3b1e */
#define DP2_AMR_CI_Compen                         DP2_AMR_CTRL_BASE + 0x00a /* 0x3b1f */

#define DP2_VAMOS_AMR_BFI_SAIC_TH_BASE            DP2_AMR_CTRL_BASE + 0x00b /* 0x3b20 */

#define DP2_VAMOS_AMR_BFI_SAIC_TH_Base            DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x000 /* 0x3b20 */
#define DP2_VAMOS_AMR_BFI_M_DTX_SAIC_0            DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x000 /* 0x3b20 */
#define DP2_VAMOS_BFI_MODEM_0_SAIC                DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x000 /* 0x3b20 */
#define DP2_VAMOS_AMR_BFI_M_DTX_SAIC_1            DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x001 /* 0x3b21 */
#define DP2_VAMOS_BFI_MODEM_1_SAIC                DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x001 /* 0x3b21 */
#define DP2_VAMOS_AMR_BFI_M_DTX_SAIC_2            DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x002 /* 0x3b22 */
#define DP2_VAMOS_BFI_SPEECH_0_SAIC               DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x002 /* 0x3b22 */
#define DP2_VAMOS_AMR_BFI_M_DTX_SAIC_3            DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x003 /* 0x3b23 */
#define DP2_VAMOS_BFI_SPEECH_1_SAIC               DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x003 /* 0x3b23 */
#define DP2_VAMOS_AMR_BFI_M_Active_SAIC_0         DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x004 /* 0x3b24 */
#define DP2_VAMOS_BFI_UFI_SAIC                    DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x004 /* 0x3b24 */
#define DP2_VAMOS_AMR_BFI_M_Active_SAIC_1         DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x005 /* 0x3b25 */
#define DP2_VAMOS_AMR_BFI_M_Active_SAIC_2         DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x006 /* 0x3b26 */
#define DP2_VAMOS_AMR_BFI_M_Active_SAIC_3         DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x007 /* 0x3b27 */
#define DP2_VAMOS_AMR_BFI_SP_0_SAIC_0             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x008 /* 0x3b28 */
#define DP2_VAMOS_AMR_BFI_SP_0_SAIC_1             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x009 /* 0x3b29 */
#define DP2_VAMOS_AMR_BFI_SP_0_SAIC_2             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x00a /* 0x3b2a */
#define DP2_VAMOS_AMR_BFI_SP_0_SAIC_3             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x00b /* 0x3b2b */
#define DP2_VAMOS_AMR_BFI_SP_1_SAIC_0             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x00c /* 0x3b2c */
#define DP2_VAMOS_AMR_BFI_SP_1_SAIC_1             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x00d /* 0x3b2d */
#define DP2_VAMOS_AMR_BFI_SP_1_SAIC_2             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x00e /* 0x3b2e */
#define DP2_VAMOS_AMR_BFI_SP_1_SAIC_3             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x00f /* 0x3b2f */
#define DP2_VAMOS_AMR_BFI_SP_2_SAIC_0             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x010 /* 0x3b30 */
#define DP2_VAMOS_AMR_BFI_SP_2_SAIC_1             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x011 /* 0x3b31 */
#define DP2_VAMOS_AMR_BFI_SP_2_SAIC_2             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x012 /* 0x3b32 */
#define DP2_VAMOS_AMR_BFI_SP_2_SAIC_3             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x013 /* 0x3b33 */
#define DP2_VAMOS_AMR_BFI_SP_2_SAIC_X             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x014 /* 0x3b34 */
#define DP2_VAMOS_AMR_BFI_SP_2_SAIC_Y             DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x015 /* 0x3b35 */

#define DP2_AMR_BFI_SAIC_TH_BASE                  DP2_VAMOS_AMR_BFI_SAIC_TH_BASE + 0x016 /* 0x3b36 */

#define DP2_AMR_BFI_SAIC_TH_Base                  DP2_AMR_BFI_SAIC_TH_BASE + 0x000 /* 0x3b36 */
#define DP2_AMR_BFI_M_DTX_SAIC_0                  DP2_AMR_BFI_SAIC_TH_BASE + 0x000 /* 0x3b36 */
#define DP2_BFI_MODEM_0_SAIC                      DP2_AMR_BFI_SAIC_TH_BASE + 0x000 /* 0x3b36 */
#define DP2_AMR_BFI_M_DTX_SAIC_1                  DP2_AMR_BFI_SAIC_TH_BASE + 0x001 /* 0x3b37 */
#define DP2_BFI_MODEM_1_SAIC                      DP2_AMR_BFI_SAIC_TH_BASE + 0x001 /* 0x3b37 */
#define DP2_AMR_BFI_M_DTX_SAIC_2                  DP2_AMR_BFI_SAIC_TH_BASE + 0x002 /* 0x3b38 */
#define DP2_BFI_SPEECH_0_SAIC                     DP2_AMR_BFI_SAIC_TH_BASE + 0x002 /* 0x3b38 */
#define DP2_AMR_BFI_M_DTX_SAIC_3                  DP2_AMR_BFI_SAIC_TH_BASE + 0x003 /* 0x3b39 */
#define DP2_BFI_SPEECH_1_SAIC                     DP2_AMR_BFI_SAIC_TH_BASE + 0x003 /* 0x3b39 */
#define DP2_AMR_BFI_M_Active_SAIC_0               DP2_AMR_BFI_SAIC_TH_BASE + 0x004 /* 0x3b3a */
#define DP2_BFI_UFI_SAIC                          DP2_AMR_BFI_SAIC_TH_BASE + 0x004 /* 0x3b3a */
#define DP2_AMR_BFI_M_Active_SAIC_1               DP2_AMR_BFI_SAIC_TH_BASE + 0x005 /* 0x3b3b */
#define DP2_BFI_MODEM_0                           DP2_AMR_BFI_SAIC_TH_BASE + 0x005 /* 0x3b3b */
#define DP2_AMR_BFI_M_Active_SAIC_2               DP2_AMR_BFI_SAIC_TH_BASE + 0x006 /* 0x3b3c */
#define DP2_BFI_MODEM_1                           DP2_AMR_BFI_SAIC_TH_BASE + 0x006 /* 0x3b3c */
#define DP2_AMR_BFI_M_Active_SAIC_3               DP2_AMR_BFI_SAIC_TH_BASE + 0x007 /* 0x3b3d */
#define DP2_BFI_SPEECH_0                          DP2_AMR_BFI_SAIC_TH_BASE + 0x007 /* 0x3b3d */
#define DP2_AMR_BFI_SP_0_SAIC_0                   DP2_AMR_BFI_SAIC_TH_BASE + 0x008 /* 0x3b3e */
#define DP2_BFI_SPEECH_1                          DP2_AMR_BFI_SAIC_TH_BASE + 0x008 /* 0x3b3e */
#define DP2_AMR_BFI_SP_0_SAIC_1                   DP2_AMR_BFI_SAIC_TH_BASE + 0x009 /* 0x3b3f */
#define DP2_BFI_UFI                               DP2_AMR_BFI_SAIC_TH_BASE + 0x009 /* 0x3b3f */
#define DP2_AMR_BFI_SP_0_SAIC_2                   DP2_AMR_BFI_SAIC_TH_BASE + 0x00a /* 0x3b40 */
#define DP2_AMR_BFI_SP_0_SAIC_3                   DP2_AMR_BFI_SAIC_TH_BASE + 0x00b /* 0x3b41 */
#define DP2_AMR_BFI_SP_1_SAIC_0                   DP2_AMR_BFI_SAIC_TH_BASE + 0x00c /* 0x3b42 */
#define DP2_AMR_BFI_SP_1_SAIC_1                   DP2_AMR_BFI_SAIC_TH_BASE + 0x00d /* 0x3b43 */
#define DP2_AMR_BFI_SP_1_SAIC_2                   DP2_AMR_BFI_SAIC_TH_BASE + 0x00e /* 0x3b44 */
#define DP2_AMR_BFI_SP_1_SAIC_3                   DP2_AMR_BFI_SAIC_TH_BASE + 0x00f /* 0x3b45 */
#define DP2_AMR_BFI_SP_2_SAIC_0                   DP2_AMR_BFI_SAIC_TH_BASE + 0x010 /* 0x3b46 */
#define DP2_AMR_BFI_SP_2_SAIC_1                   DP2_AMR_BFI_SAIC_TH_BASE + 0x011 /* 0x3b47 */
#define DP2_AMR_BFI_SP_2_SAIC_2                   DP2_AMR_BFI_SAIC_TH_BASE + 0x012 /* 0x3b48 */
#define DP2_AMR_BFI_SP_2_SAIC_3                   DP2_AMR_BFI_SAIC_TH_BASE + 0x013 /* 0x3b49 */
#define DP2_AMR_BFI_SP_2_SAIC_X                   DP2_AMR_BFI_SAIC_TH_BASE + 0x014 /* 0x3b4a */
#define DP2_AMR_BFI_SP_2_SAIC_Y                   DP2_AMR_BFI_SAIC_TH_BASE + 0x015 /* 0x3b4b */

#define DP2_AMR_THRE_BASE                         DP2_AMR_BFI_SAIC_TH_BASE + 0x016 /* 0x3b4c */

#define DP2_AMR_SP_Degrade_THRES_0                DP2_AMR_THRE_BASE + 0x000 /* 0x3b4c */
#define DP2_AMR_SP_Degrade_THRES_1                DP2_AMR_THRE_BASE + 0x001 /* 0x3b4d */
#define DP2_AMR_SP_Degrade_THRES_2                DP2_AMR_THRE_BASE + 0x002 /* 0x3b4e */
#define DP2_AMR_SP_Degrade_THRES_3                DP2_AMR_THRE_BASE + 0x003 /* 0x3b4f */

#define DP_MODEM_CTRL_RESERVED_BASE               DP2_AMR_THRE_BASE + 0x004 /* 0x3b50 */

#define DP_LOW_ANT_PWR_TH                         DP_MODEM_CTRL_RESERVED_BASE + 0x000 /* 0x3b50 */
#define DP_HEADROOM_DET_TH                        DP_MODEM_CTRL_RESERVED_BASE + 0x001 /* 0x3b51 */
#define DP_GAIN_IMB_IDX                           DP_MODEM_CTRL_RESERVED_BASE + 0x002 /* 0x3b52 */
#define DP_OBB_WEIGHT                             DP_MODEM_CTRL_RESERVED_BASE + 0x003 /* 0x3b53 */
#define DP_OT_TX_IQMM_ARTIFICIAL_DC               DP_MODEM_CTRL_RESERVED_BASE + 0x004 /* 0x3b54 */
#define DP_OT_TX_IQMM_GAIN                        DP_MODEM_CTRL_RESERVED_BASE + 0x005 /* 0x3b55 */
#define DP_OT_TX_IQMM_PHASE                       DP_MODEM_CTRL_RESERVED_BASE + 0x006 /* 0x3b56 */

#define DP2_ENCRYPT2_BASE                         DP_MODEM_CTRL_RESERVED_BASE + 0x013 /* 0x3b63 */

#define DP2_ENCRYP_KEY_5                          DP2_ENCRYPT2_BASE + 0x000 /* 0x3b63 */
#define DP2_ENCRYP_KEY_6                          DP2_ENCRYPT2_BASE + 0x001 /* 0x3b64 */
#define DP2_ENCRYP_KEY_7                          DP2_ENCRYPT2_BASE + 0x002 /* 0x3b65 */
#define DP2_ENCRYP_KEY_8                          DP2_ENCRYPT2_BASE + 0x003 /* 0x3b66 */

#define DP2_AMR_TRACE_BASE                        DP2_ENCRYPT2_BASE + 0x004 /* 0x3b67 */

#define DP2_AMR_Used_Codec                        DP2_AMR_TRACE_BASE + 0x000 /* 0x3b67 */
#define DP2_AMR_Trace_Codec                       DP2_AMR_TRACE_BASE + 0x001 /* 0x3b68 */
#define DP2_AMR_Trace_CC_TX                       DP2_AMR_TRACE_BASE + 0x002 /* 0x3b69 */
#define DP2_AMR_INB_Corr0                         DP2_AMR_TRACE_BASE + 0x003 /* 0x3b6a */
#define DP2_AMR_RX_Trace_01_Base                  DP2_AMR_TRACE_BASE + 0x003 /* 0x3b6a */
#define DP2_AMR_INB_Corr1                         DP2_AMR_TRACE_BASE + 0x004 /* 0x3b6b */
#define DP2_AMR_INB_Corr2                         DP2_AMR_TRACE_BASE + 0x005 /* 0x3b6c */
#define DP2_AMR_INB_Corr3                         DP2_AMR_TRACE_BASE + 0x006 /* 0x3b6d */
#define DP2_AMR_SLResetFlag                       DP2_AMR_TRACE_BASE + 0x007 /* 0x3b6e */
#define DP2_AMR_SP_SC                             DP2_AMR_TRACE_BASE + 0x008 /* 0x3b6f */
#define DP2_AMR_SP_SD                             DP2_AMR_TRACE_BASE + 0x009 /* 0x3b70 */
#define DP2_AMR_EQ_SN_Sum                         DP2_AMR_TRACE_BASE + 0x00a /* 0x3b71 */

#define DP_REPORT_BASE                            DP2_AMR_TRACE_BASE + 0x00b /* 0x3b72 */

#define DP_DARP_FOE_Q                             DP_REPORT_BASE + 0x000 /* 0x3b72 */
#define DP_EQ_SINR_FWBW_replace_hit               DP_REPORT_BASE + 0x001 /* 0x3b73 */
#define DP_EQSO_SUM_LO                            DP_REPORT_BASE + 0x002 /* 0x3b74 */
#define DP_EQSO_SUM_HI                            DP_REPORT_BASE + 0x003 /* 0x3b75 */
#define DP_CHL_S0                                 DP_REPORT_BASE + 0x004 /* 0x3b76 */
#define DP_CHL_S1                                 DP_REPORT_BASE + 0x005 /* 0x3b77 */
#define DP_CHL_S2                                 DP_REPORT_BASE + 0x006 /* 0x3b78 */
#define DP_CHL_S3                                 DP_REPORT_BASE + 0x007 /* 0x3b79 */
#define DP_CHL_S4                                 DP_REPORT_BASE + 0x008 /* 0x3b7a */
#define DP_CHL_R_S0                               DP_REPORT_BASE + 0x009 /* 0x3b7b */
#define DP_CHL_R_S1                               DP_REPORT_BASE + 0x00a /* 0x3b7c */
#define DP_CHL_R_S2                               DP_REPORT_BASE + 0x00b /* 0x3b7d */
#define DP_CHL_R_S3                               DP_REPORT_BASE + 0x00c /* 0x3b7e */
#define DP_CHL_R_S4                               DP_REPORT_BASE + 0x00d /* 0x3b7f */

#define DP_NB_RESULT_RX0_BASE                     DP_REPORT_BASE + 0x010 /* 0x3b82 */

#define DP_NB_RES_S0_0                            DP_NB_RESULT_RX0_BASE + 0x000 /* 0x3b82 */
#define DP_TX_IQMM_d_I_S0                         DP_NB_RESULT_RX0_BASE + 0x000 /* 0x3b82 */
#define DP_NB_RES_S0_1                            DP_NB_RESULT_RX0_BASE + 0x001 /* 0x3b83 */
#define DP_TX_IQMM_d_Q_S0                         DP_NB_RESULT_RX0_BASE + 0x001 /* 0x3b83 */
#define DP_NB_RES_S0_2                            DP_NB_RESULT_RX0_BASE + 0x002 /* 0x3b84 */
#define DP_TX_IQMM_v_I_S0                         DP_NB_RESULT_RX0_BASE + 0x002 /* 0x3b84 */
#define DP_NB_RES_S0_3                            DP_NB_RESULT_RX0_BASE + 0x003 /* 0x3b85 */
#define DP_TX_IQMM_v_Q_S0                         DP_NB_RESULT_RX0_BASE + 0x003 /* 0x3b85 */
#define DP_NB_RES_S0_4                            DP_NB_RESULT_RX0_BASE + 0x004 /* 0x3b86 */
#define DP_TX_IQMM_l_I_S0                         DP_NB_RESULT_RX0_BASE + 0x004 /* 0x3b86 */
#define DP_NB_RES_S0_5                            DP_NB_RESULT_RX0_BASE + 0x005 /* 0x3b87 */
#define DP_TX_IQMM_l_Q_S0                         DP_NB_RESULT_RX0_BASE + 0x005 /* 0x3b87 */
#define DP_NB_RES_S0_6                            DP_NB_RESULT_RX0_BASE + 0x006 /* 0x3b88 */
#define DP_NB_RES_S0_7                            DP_NB_RESULT_RX0_BASE + 0x007 /* 0x3b89 */
#define DP_NB_RES_S0_8                            DP_NB_RESULT_RX0_BASE + 0x008 /* 0x3b8a */
#define DP_DC_PROC_OUT0                           DP_NB_RESULT_RX0_BASE + 0x008 /* 0x3b8a */
#define DP_NB_RES_S0_9                            DP_NB_RESULT_RX0_BASE + 0x009 /* 0x3b8b */
#define DP_DC_PROC_OUT1                           DP_NB_RESULT_RX0_BASE + 0x009 /* 0x3b8b */
#define DP_NB_RES_S0_10                           DP_NB_RESULT_RX0_BASE + 0x00a /* 0x3b8c */
#define DP_DC_PROC_OUT2                           DP_NB_RESULT_RX0_BASE + 0x00a /* 0x3b8c */
#define DP_NB_RES_S0_11                           DP_NB_RESULT_RX0_BASE + 0x00b /* 0x3b8d */
#define DP_NB_RES_S0_12                           DP_NB_RESULT_RX0_BASE + 0x00c /* 0x3b8e */
#define DP_NB_RES_S0_13                           DP_NB_RESULT_RX0_BASE + 0x00d /* 0x3b8f */
#define DP_NB_RES_S0_14                           DP_NB_RESULT_RX0_BASE + 0x00e /* 0x3b90 */
#define DP_NB_RES_S0_15                           DP_NB_RESULT_RX0_BASE + 0x00f /* 0x3b91 */
#define DP_NB_RES_S0_16                           DP_NB_RESULT_RX0_BASE + 0x010 /* 0x3b92 */
#define DP_NB_RES_S0_17                           DP_NB_RESULT_RX0_BASE + 0x011 /* 0x3b93 */
#define DP_NB_RES_S0_18                           DP_NB_RESULT_RX0_BASE + 0x012 /* 0x3b94 */
#define DP_NB_RES_S0_19                           DP_NB_RESULT_RX0_BASE + 0x013 /* 0x3b95 */
#define DP_NB_RES_S0_20                           DP_NB_RESULT_RX0_BASE + 0x014 /* 0x3b96 */
#define DP_NB_RES_S0_21                           DP_NB_RESULT_RX0_BASE + 0x015 /* 0x3b97 */
#define DP_NB_RES_S0_22                           DP_NB_RESULT_RX0_BASE + 0x016 /* 0x3b98 */
#define DP_NB_RES_S0_23                           DP_NB_RESULT_RX0_BASE + 0x017 /* 0x3b99 */
#define DP_NB_RES_S0_24                           DP_NB_RESULT_RX0_BASE + 0x018 /* 0x3b9a */
#define DP_NB_RES_S0_25                           DP_NB_RESULT_RX0_BASE + 0x019 /* 0x3b9b */
#define DP_NB_RES_S0_26                           DP_NB_RESULT_RX0_BASE + 0x01a /* 0x3b9c */
#define DP_NB_RES_S0_27                           DP_NB_RESULT_RX0_BASE + 0x01b /* 0x3b9d */
#define DP_NB_RES_S0_28                           DP_NB_RESULT_RX0_BASE + 0x01c /* 0x3b9e */
#define DP_NB_RES_S0_29                           DP_NB_RESULT_RX0_BASE + 0x01d /* 0x3b9f */
#define DP_S0_MEAN_BEP                            DP_NB_RESULT_RX0_BASE + 0x01d /* 0x3b9f */
#define DP_ENH_MEASURE_REPORT_base                DP_NB_RESULT_RX0_BASE + 0x01d /* 0x3b9f */
#define DP_NB_RES_S0_30                           DP_NB_RESULT_RX0_BASE + 0x01e /* 0x3ba0 */
#define DP_S0_CV_BEP                              DP_NB_RESULT_RX0_BASE + 0x01e /* 0x3ba0 */

#define DP_NB_RESULT_RX1_BASE                     DP_NB_RESULT_RX0_BASE + 0x021 /* 0x3ba3 */

#define DP_NB_RES_S1_0                            DP_NB_RESULT_RX1_BASE + 0x000 /* 0x3ba3 */
#define DP_TX_IQMM_d_I_S1                         DP_NB_RESULT_RX1_BASE + 0x000 /* 0x3ba3 */
#define DP_NB_RES_S1_1                            DP_NB_RESULT_RX1_BASE + 0x001 /* 0x3ba4 */
#define DP_TX_IQMM_d_Q_S1                         DP_NB_RESULT_RX1_BASE + 0x001 /* 0x3ba4 */
#define DP_NB_RES_S1_2                            DP_NB_RESULT_RX1_BASE + 0x002 /* 0x3ba5 */
#define DP_TX_IQMM_v_I_S1                         DP_NB_RESULT_RX1_BASE + 0x002 /* 0x3ba5 */
#define DP_NB_RES_S1_3                            DP_NB_RESULT_RX1_BASE + 0x003 /* 0x3ba6 */
#define DP_TX_IQMM_v_Q_S1                         DP_NB_RESULT_RX1_BASE + 0x003 /* 0x3ba6 */
#define DP_NB_RES_S1_4                            DP_NB_RESULT_RX1_BASE + 0x004 /* 0x3ba7 */
#define DP_TX_IQMM_l_I_S1                         DP_NB_RESULT_RX1_BASE + 0x004 /* 0x3ba7 */
#define DP_NB_RES_S1_5                            DP_NB_RESULT_RX1_BASE + 0x005 /* 0x3ba8 */
#define DP_TX_IQMM_l_Q_S1                         DP_NB_RESULT_RX1_BASE + 0x005 /* 0x3ba8 */
#define DP_NB_RES_S1_6                            DP_NB_RESULT_RX1_BASE + 0x006 /* 0x3ba9 */
#define DP_TX_IQMM_T_I_S1                         DP_NB_RESULT_RX1_BASE + 0x006 /* 0x3ba9 */
#define DP_NB_RES_S1_7                            DP_NB_RESULT_RX1_BASE + 0x007 /* 0x3baa */
#define DP_TX_IQMM_T_Q_S1                         DP_NB_RESULT_RX1_BASE + 0x007 /* 0x3baa */
#define DP_NB_RES_S1_8                            DP_NB_RESULT_RX1_BASE + 0x008 /* 0x3bab */
#define DP_TX_IQMM_gT_S1                          DP_NB_RESULT_RX1_BASE + 0x008 /* 0x3bab */
#define DP_NB_RES_S1_9                            DP_NB_RESULT_RX1_BASE + 0x009 /* 0x3bac */
#define DP_TX_IQMM_phiT_S1                        DP_NB_RESULT_RX1_BASE + 0x009 /* 0x3bac */
#define DP_NB_RES_S1_10                           DP_NB_RESULT_RX1_BASE + 0x00a /* 0x3bad */
#define DP_TX_IQMM_G_I_S1                         DP_NB_RESULT_RX1_BASE + 0x00a /* 0x3bad */
#define DP_NB_RES_S1_11                           DP_NB_RESULT_RX1_BASE + 0x00b /* 0x3bae */
#define DP_TX_IQMM_G_Q_S1                         DP_NB_RESULT_RX1_BASE + 0x00b /* 0x3bae */
#define DP_NB_RES_S1_12                           DP_NB_RESULT_RX1_BASE + 0x00c /* 0x3baf */
#define DP_TX_IQMM_DC_I_S1                        DP_NB_RESULT_RX1_BASE + 0x00c /* 0x3baf */
#define DP_NB_RES_S1_13                           DP_NB_RESULT_RX1_BASE + 0x00d /* 0x3bb0 */
#define DP_TX_IQMM_DC_Q_S1                        DP_NB_RESULT_RX1_BASE + 0x00d /* 0x3bb0 */
#define DP_NB_RES_S1_14                           DP_NB_RESULT_RX1_BASE + 0x00e /* 0x3bb1 */
#define DP_NB_RES_S1_15                           DP_NB_RESULT_RX1_BASE + 0x00f /* 0x3bb2 */
#define DP_NB_RES_S1_16                           DP_NB_RESULT_RX1_BASE + 0x010 /* 0x3bb3 */
#define DP_NB_RES_S1_17                           DP_NB_RESULT_RX1_BASE + 0x011 /* 0x3bb4 */
#define DP_NB_RES_S1_18                           DP_NB_RESULT_RX1_BASE + 0x012 /* 0x3bb5 */
#define DP_NB_RES_S1_19                           DP_NB_RESULT_RX1_BASE + 0x013 /* 0x3bb6 */
#define DP_NB_RES_S1_20                           DP_NB_RESULT_RX1_BASE + 0x014 /* 0x3bb7 */
#define DP_NB_RES_S1_21                           DP_NB_RESULT_RX1_BASE + 0x015 /* 0x3bb8 */
#define DP_NB_RES_S1_22                           DP_NB_RESULT_RX1_BASE + 0x016 /* 0x3bb9 */
#define DP_NB_RES_S1_23                           DP_NB_RESULT_RX1_BASE + 0x017 /* 0x3bba */
#define DP_NB_RES_S1_24                           DP_NB_RESULT_RX1_BASE + 0x018 /* 0x3bbb */
#define DP_NB_RES_S1_25                           DP_NB_RESULT_RX1_BASE + 0x019 /* 0x3bbc */
#define DP_NB_RES_S1_26                           DP_NB_RESULT_RX1_BASE + 0x01a /* 0x3bbd */
#define DP_NB_RES_S1_27                           DP_NB_RESULT_RX1_BASE + 0x01b /* 0x3bbe */
#define DP_NB_RES_S1_28                           DP_NB_RESULT_RX1_BASE + 0x01c /* 0x3bbf */
#define DP_NB_RES_S1_29                           DP_NB_RESULT_RX1_BASE + 0x01d /* 0x3bc0 */
#define DP_S1_MEAN_BEP                            DP_NB_RESULT_RX1_BASE + 0x01d /* 0x3bc0 */
#define DP_NB_RES_S1_30                           DP_NB_RESULT_RX1_BASE + 0x01e /* 0x3bc1 */
#define DP_S1_CV_BEP                              DP_NB_RESULT_RX1_BASE + 0x01e /* 0x3bc1 */

#define DP_NB_RESULT_RX2_BASE                     DP_NB_RESULT_RX1_BASE + 0x021 /* 0x3bc4 */

#define DP_NB_RES_S2_0                            DP_NB_RESULT_RX2_BASE + 0x000 /* 0x3bc4 */
#define DP_NB_RES_S2_1                            DP_NB_RESULT_RX2_BASE + 0x001 /* 0x3bc5 */
#define DP_NB_RES_S2_2                            DP_NB_RESULT_RX2_BASE + 0x002 /* 0x3bc6 */
#define DP_NB_RES_S2_3                            DP_NB_RESULT_RX2_BASE + 0x003 /* 0x3bc7 */
#define DP_NB_RES_S2_4                            DP_NB_RESULT_RX2_BASE + 0x004 /* 0x3bc8 */
#define DP_NB_RES_S2_5                            DP_NB_RESULT_RX2_BASE + 0x005 /* 0x3bc9 */
#define DP_NB_RES_S2_6                            DP_NB_RESULT_RX2_BASE + 0x006 /* 0x3bca */
#define DP_NB_RES_S2_7                            DP_NB_RESULT_RX2_BASE + 0x007 /* 0x3bcb */
#define DP_NB_RES_S2_8                            DP_NB_RESULT_RX2_BASE + 0x008 /* 0x3bcc */
#define DP_NB_RES_S2_9                            DP_NB_RESULT_RX2_BASE + 0x009 /* 0x3bcd */
#define DP_NB_RES_S2_10                           DP_NB_RESULT_RX2_BASE + 0x00a /* 0x3bce */
#define DP_NB_RES_S2_11                           DP_NB_RESULT_RX2_BASE + 0x00b /* 0x3bcf */
#define DP_NB_RES_S2_12                           DP_NB_RESULT_RX2_BASE + 0x00c /* 0x3bd0 */
#define DP_NB_RES_S2_13                           DP_NB_RESULT_RX2_BASE + 0x00d /* 0x3bd1 */
#define DP_NB_RES_S2_14                           DP_NB_RESULT_RX2_BASE + 0x00e /* 0x3bd2 */
#define DP_NB_RES_S2_15                           DP_NB_RESULT_RX2_BASE + 0x00f /* 0x3bd3 */
#define DP_NB_RES_S2_16                           DP_NB_RESULT_RX2_BASE + 0x010 /* 0x3bd4 */
#define DP_NB_RES_S2_17                           DP_NB_RESULT_RX2_BASE + 0x011 /* 0x3bd5 */
#define DP_NB_RES_S2_18                           DP_NB_RESULT_RX2_BASE + 0x012 /* 0x3bd6 */
#define DP_NB_RES_S2_19                           DP_NB_RESULT_RX2_BASE + 0x013 /* 0x3bd7 */
#define DP_NB_RES_S2_20                           DP_NB_RESULT_RX2_BASE + 0x014 /* 0x3bd8 */
#define DP_NB_RES_S2_21                           DP_NB_RESULT_RX2_BASE + 0x015 /* 0x3bd9 */
#define DP_NB_RES_S2_22                           DP_NB_RESULT_RX2_BASE + 0x016 /* 0x3bda */
#define DP_NB_RES_S2_23                           DP_NB_RESULT_RX2_BASE + 0x017 /* 0x3bdb */
#define DP_NB_RES_S2_24                           DP_NB_RESULT_RX2_BASE + 0x018 /* 0x3bdc */
#define DP_NB_RES_S2_25                           DP_NB_RESULT_RX2_BASE + 0x019 /* 0x3bdd */
#define DP_NB_RES_S2_26                           DP_NB_RESULT_RX2_BASE + 0x01a /* 0x3bde */
#define DP_NB_RES_S2_27                           DP_NB_RESULT_RX2_BASE + 0x01b /* 0x3bdf */
#define DP_NB_RES_S2_28                           DP_NB_RESULT_RX2_BASE + 0x01c /* 0x3be0 */
#define DP_NB_RES_S2_29                           DP_NB_RESULT_RX2_BASE + 0x01d /* 0x3be1 */
#define DP_S2_MEAN_BEP                            DP_NB_RESULT_RX2_BASE + 0x01d /* 0x3be1 */
#define DP_NB_RES_S2_30                           DP_NB_RESULT_RX2_BASE + 0x01e /* 0x3be2 */
#define DP_S2_CV_BEP                              DP_NB_RESULT_RX2_BASE + 0x01e /* 0x3be2 */

#define DP_NB_RESULT_RX3_BASE                     DP_NB_RESULT_RX2_BASE + 0x021 /* 0x3be5 */

#define DP_NB_RES_S3_0                            DP_NB_RESULT_RX3_BASE + 0x000 /* 0x3be5 */
#define DP_NB_RES_S3_1                            DP_NB_RESULT_RX3_BASE + 0x001 /* 0x3be6 */
#define DP_NB_RES_S3_2                            DP_NB_RESULT_RX3_BASE + 0x002 /* 0x3be7 */
#define DP_NB_RES_S3_3                            DP_NB_RESULT_RX3_BASE + 0x003 /* 0x3be8 */
#define DP_NB_RES_S3_4                            DP_NB_RESULT_RX3_BASE + 0x004 /* 0x3be9 */
#define DP_NB_RES_S3_5                            DP_NB_RESULT_RX3_BASE + 0x005 /* 0x3bea */
#define DP_NB_RES_S3_6                            DP_NB_RESULT_RX3_BASE + 0x006 /* 0x3beb */
#define DP_NB_RES_S3_7                            DP_NB_RESULT_RX3_BASE + 0x007 /* 0x3bec */
#define DP_NB_RES_S3_8                            DP_NB_RESULT_RX3_BASE + 0x008 /* 0x3bed */
#define DP_NB_RES_S3_9                            DP_NB_RESULT_RX3_BASE + 0x009 /* 0x3bee */
#define DP_NB_RES_S3_10                           DP_NB_RESULT_RX3_BASE + 0x00a /* 0x3bef */
#define DP_NB_RES_S3_11                           DP_NB_RESULT_RX3_BASE + 0x00b /* 0x3bf0 */
#define DP_NB_RES_S3_12                           DP_NB_RESULT_RX3_BASE + 0x00c /* 0x3bf1 */
#define DP_NB_RES_S3_13                           DP_NB_RESULT_RX3_BASE + 0x00d /* 0x3bf2 */
#define DP_NB_RES_S3_14                           DP_NB_RESULT_RX3_BASE + 0x00e /* 0x3bf3 */
#define DP_NB_RES_S3_15                           DP_NB_RESULT_RX3_BASE + 0x00f /* 0x3bf4 */
#define DP_NB_RES_S3_16                           DP_NB_RESULT_RX3_BASE + 0x010 /* 0x3bf5 */
#define DP_NB_RES_S3_17                           DP_NB_RESULT_RX3_BASE + 0x011 /* 0x3bf6 */
#define DP_NB_RES_S3_18                           DP_NB_RESULT_RX3_BASE + 0x012 /* 0x3bf7 */
#define DP_NB_RES_S3_19                           DP_NB_RESULT_RX3_BASE + 0x013 /* 0x3bf8 */
#define DP_NB_RES_S3_20                           DP_NB_RESULT_RX3_BASE + 0x014 /* 0x3bf9 */
#define DP_NB_RES_S3_21                           DP_NB_RESULT_RX3_BASE + 0x015 /* 0x3bfa */
#define DP_NB_RES_S3_22                           DP_NB_RESULT_RX3_BASE + 0x016 /* 0x3bfb */
#define DP_NB_RES_S3_23                           DP_NB_RESULT_RX3_BASE + 0x017 /* 0x3bfc */
#define DP_NB_RES_S3_24                           DP_NB_RESULT_RX3_BASE + 0x018 /* 0x3bfd */
#define DP_NB_RES_S3_25                           DP_NB_RESULT_RX3_BASE + 0x019 /* 0x3bfe */
#define DP_NB_RES_S3_26                           DP_NB_RESULT_RX3_BASE + 0x01a /* 0x3bff */
#define DP_NB_RES_S3_27                           DP_NB_RESULT_RX3_BASE + 0x01b /* 0x3c00 */
#define DP_NB_RES_S3_28                           DP_NB_RESULT_RX3_BASE + 0x01c /* 0x3c01 */
#define DP_NB_RES_S3_29                           DP_NB_RESULT_RX3_BASE + 0x01d /* 0x3c02 */
#define DP_S3_MEAN_BEP                            DP_NB_RESULT_RX3_BASE + 0x01d /* 0x3c02 */
#define DP_NB_RES_S3_30                           DP_NB_RESULT_RX3_BASE + 0x01e /* 0x3c03 */
#define DP_S3_CV_BEP                              DP_NB_RESULT_RX3_BASE + 0x01e /* 0x3c03 */

#define DP_NB_RESULT_RX4_BASE                     DP_NB_RESULT_RX3_BASE + 0x021 /* 0x3c06 */

#define DP_NB_RES_S4_0                            DP_NB_RESULT_RX4_BASE + 0x000 /* 0x3c06 */
#define DP_NB_RES_S4_1                            DP_NB_RESULT_RX4_BASE + 0x001 /* 0x3c07 */
#define DP_NB_RES_S4_2                            DP_NB_RESULT_RX4_BASE + 0x002 /* 0x3c08 */
#define DP_NB_RES_S4_3                            DP_NB_RESULT_RX4_BASE + 0x003 /* 0x3c09 */
#define DP_NB_RES_S4_4                            DP_NB_RESULT_RX4_BASE + 0x004 /* 0x3c0a */
#define DP_NB_RES_S4_5                            DP_NB_RESULT_RX4_BASE + 0x005 /* 0x3c0b */
#define DP_NB_RES_S4_6                            DP_NB_RESULT_RX4_BASE + 0x006 /* 0x3c0c */
#define DP_NB_RES_S4_7                            DP_NB_RESULT_RX4_BASE + 0x007 /* 0x3c0d */
#define DP_NB_RES_S4_8                            DP_NB_RESULT_RX4_BASE + 0x008 /* 0x3c0e */
#define DP_NB_RES_S4_9                            DP_NB_RESULT_RX4_BASE + 0x009 /* 0x3c0f */
#define DP_NB_RES_S4_10                           DP_NB_RESULT_RX4_BASE + 0x00a /* 0x3c10 */
#define DP_NB_RES_S4_11                           DP_NB_RESULT_RX4_BASE + 0x00b /* 0x3c11 */
#define DP_NB_RES_S4_12                           DP_NB_RESULT_RX4_BASE + 0x00c /* 0x3c12 */
#define DP_NB_RES_S4_13                           DP_NB_RESULT_RX4_BASE + 0x00d /* 0x3c13 */
#define DP_NB_RES_S4_14                           DP_NB_RESULT_RX4_BASE + 0x00e /* 0x3c14 */
#define DP_NB_RES_S4_15                           DP_NB_RESULT_RX4_BASE + 0x00f /* 0x3c15 */
#define DP_NB_RES_S4_16                           DP_NB_RESULT_RX4_BASE + 0x010 /* 0x3c16 */
#define DP_NB_RES_S4_17                           DP_NB_RESULT_RX4_BASE + 0x011 /* 0x3c17 */
#define DP_NB_RES_S4_18                           DP_NB_RESULT_RX4_BASE + 0x012 /* 0x3c18 */
#define DP_NB_RES_S4_19                           DP_NB_RESULT_RX4_BASE + 0x013 /* 0x3c19 */
#define DP_NB_RES_S4_20                           DP_NB_RESULT_RX4_BASE + 0x014 /* 0x3c1a */
#define DP_NB_RES_S4_21                           DP_NB_RESULT_RX4_BASE + 0x015 /* 0x3c1b */
#define DP_NB_RES_S4_22                           DP_NB_RESULT_RX4_BASE + 0x016 /* 0x3c1c */
#define DP_NB_RES_S4_23                           DP_NB_RESULT_RX4_BASE + 0x017 /* 0x3c1d */
#define DP_NB_RES_S4_24                           DP_NB_RESULT_RX4_BASE + 0x018 /* 0x3c1e */
#define DP_NB_RES_S4_25                           DP_NB_RESULT_RX4_BASE + 0x019 /* 0x3c1f */
#define DP_NB_RES_S4_26                           DP_NB_RESULT_RX4_BASE + 0x01a /* 0x3c20 */
#define DP_NB_RES_S4_27                           DP_NB_RESULT_RX4_BASE + 0x01b /* 0x3c21 */
#define DP_NB_RES_S4_28                           DP_NB_RESULT_RX4_BASE + 0x01c /* 0x3c22 */
#define DP_NB_RES_S4_29                           DP_NB_RESULT_RX4_BASE + 0x01d /* 0x3c23 */
#define DP_S4_MEAN_BEP                            DP_NB_RESULT_RX4_BASE + 0x01d /* 0x3c23 */
#define DP_NB_RES_S4_30                           DP_NB_RESULT_RX4_BASE + 0x01e /* 0x3c24 */
#define DP_S4_CV_BEP                              DP_NB_RESULT_RX4_BASE + 0x01e /* 0x3c24 */

#define DP_NB_RESULT_RX5_BASE                     DP_NB_RESULT_RX4_BASE + 0x021 /* 0x3c27 */

#define DP_NB_RES_S5_0                            DP_NB_RESULT_RX5_BASE + 0x000 /* 0x3c27 */
#define DP_NB_RES_S5_1                            DP_NB_RESULT_RX5_BASE + 0x001 /* 0x3c28 */
#define DP_NB_RES_S5_2                            DP_NB_RESULT_RX5_BASE + 0x002 /* 0x3c29 */
#define DP_NB_RES_S5_3                            DP_NB_RESULT_RX5_BASE + 0x003 /* 0x3c2a */
#define DP_NB_RES_S5_4                            DP_NB_RESULT_RX5_BASE + 0x004 /* 0x3c2b */
#define DP_NB_RES_S5_5                            DP_NB_RESULT_RX5_BASE + 0x005 /* 0x3c2c */
#define DP_NB_RES_S5_6                            DP_NB_RESULT_RX5_BASE + 0x006 /* 0x3c2d */
#define DP_NB_RES_S5_7                            DP_NB_RESULT_RX5_BASE + 0x007 /* 0x3c2e */

#define DP_NB_RESULT_RX6_BASE                     DP_NB_RESULT_RX5_BASE + 0x008 /* 0x3c2f */

#define DP_NB_RES_S6_0                            DP_NB_RESULT_RX6_BASE + 0x000 /* 0x3c2f */
#define DP_NB_RES_S6_1                            DP_NB_RESULT_RX6_BASE + 0x001 /* 0x3c30 */
#define DP_NB_RES_S6_2                            DP_NB_RESULT_RX6_BASE + 0x002 /* 0x3c31 */
#define DP_NB_RES_S6_3                            DP_NB_RESULT_RX6_BASE + 0x003 /* 0x3c32 */
#define DP_NB_RES_S6_4                            DP_NB_RESULT_RX6_BASE + 0x004 /* 0x3c33 */
#define DP_NB_RES_S6_5                            DP_NB_RESULT_RX6_BASE + 0x005 /* 0x3c34 */
#define DP_NB_RES_S6_6                            DP_NB_RESULT_RX6_BASE + 0x006 /* 0x3c35 */
#define DP_NB_RES_S6_7                            DP_NB_RESULT_RX6_BASE + 0x007 /* 0x3c36 */

#define DP_NB_RESULT_RX7_BASE                     DP_NB_RESULT_RX6_BASE + 0x008 /* 0x3c37 */

#define DP_NB_RES_S7_0                            DP_NB_RESULT_RX7_BASE + 0x000 /* 0x3c37 */
#define DP_NB_RES_S7_1                            DP_NB_RESULT_RX7_BASE + 0x001 /* 0x3c38 */
#define DP_NB_RES_S7_2                            DP_NB_RESULT_RX7_BASE + 0x002 /* 0x3c39 */
#define DP_NB_RES_S7_3                            DP_NB_RESULT_RX7_BASE + 0x003 /* 0x3c3a */
#define DP_NB_RES_S7_4                            DP_NB_RESULT_RX7_BASE + 0x004 /* 0x3c3b */
#define DP_NB_RES_S7_5                            DP_NB_RESULT_RX7_BASE + 0x005 /* 0x3c3c */
#define DP_NB_RES_S7_6                            DP_NB_RESULT_RX7_BASE + 0x006 /* 0x3c3d */
#define DP_NB_RES_S7_7                            DP_NB_RESULT_RX7_BASE + 0x007 /* 0x3c3e */

#define DP_SB_RESULT_BASE                         DP_NB_RESULT_RX7_BASE + 0x008 /* 0x3c3f */

#define DP_SB_RES_0                               DP_SB_RESULT_BASE + 0x000 /* 0x3c3f */
#define DP_SB_RES_1                               DP_SB_RESULT_BASE + 0x001 /* 0x3c40 */
#define DP_SB_RES_2                               DP_SB_RESULT_BASE + 0x002 /* 0x3c41 */
#define DP_SB_RES_3                               DP_SB_RESULT_BASE + 0x003 /* 0x3c42 */
#define DP_SB_RES_4                               DP_SB_RESULT_BASE + 0x004 /* 0x3c43 */
#define DP_SB_RES_5                               DP_SB_RESULT_BASE + 0x005 /* 0x3c44 */
#define DP_SB_RES_6                               DP_SB_RESULT_BASE + 0x006 /* 0x3c45 */
#define DP_SB_RES_7                               DP_SB_RESULT_BASE + 0x007 /* 0x3c46 */
#define DP_SB_RES_8                               DP_SB_RESULT_BASE + 0x008 /* 0x3c47 */
#define DP_SB_RES_9                               DP_SB_RESULT_BASE + 0x009 /* 0x3c48 */
#define DP_SB_RES_10                              DP_SB_RESULT_BASE + 0x00a /* 0x3c49 */
#define DP_SB_RES_11                              DP_SB_RESULT_BASE + 0x00b /* 0x3c4a */
#define DP_SB_RES_12                              DP_SB_RESULT_BASE + 0x00c /* 0x3c4b */
#define DP_SB_RES_13                              DP_SB_RESULT_BASE + 0x00d /* 0x3c4c */
#define DP_SB_RES_14                              DP_SB_RESULT_BASE + 0x00e /* 0x3c4d */
#define DP_SB_RES_15                              DP_SB_RESULT_BASE + 0x00f /* 0x3c4e */

#define DP_FB_RESULT_BASE                         DP_SB_RESULT_BASE + 0x011 /* 0x3c50 */

#define DP_FB_RES_0                               DP_FB_RESULT_BASE + 0x000 /* 0x3c50 */
#define DP_FB_RES_1                               DP_FB_RESULT_BASE + 0x001 /* 0x3c51 */
#define DP_FB_RES_2                               DP_FB_RESULT_BASE + 0x002 /* 0x3c52 */
#define DP_FB_RES_3                               DP_FB_RESULT_BASE + 0x003 /* 0x3c53 */
#define DP_FB_RES_4                               DP_FB_RESULT_BASE + 0x004 /* 0x3c54 */
#define DP_FB_RES_5                               DP_FB_RESULT_BASE + 0x005 /* 0x3c55 */
#define DP_FB_RES_6                               DP_FB_RESULT_BASE + 0x006 /* 0x3c56 */
#define DP_FB_RES_7                               DP_FB_RESULT_BASE + 0x007 /* 0x3c57 */
#define DP_FB_RES_8                               DP_FB_RESULT_BASE + 0x008 /* 0x3c58 */
#define DP_FB_RES_9                               DP_FB_RESULT_BASE + 0x009 /* 0x3c59 */
#define DP_FB_RES_10                              DP_FB_RESULT_BASE + 0x00a /* 0x3c5a */
#define DP_FB_RES_11                              DP_FB_RESULT_BASE + 0x00b /* 0x3c5b */
#define DP_FB_RES_12                              DP_FB_RESULT_BASE + 0x00c /* 0x3c5c */
#define DP_FB_RES_13                              DP_FB_RESULT_BASE + 0x00d /* 0x3c5d */

#define DP2_FAST_USF_REPORT_BASE                  DP_FB_RESULT_BASE + 0x010 /* 0x3c60 */

#define DP2_CC_USF_S0                             DP2_FAST_USF_REPORT_BASE + 0x000 /* 0x3c60 */
#define DP_SF_Value                               DP2_FAST_USF_REPORT_BASE + 0x000 /* 0x3c60 */
#define DP2_CC_USF_S1                             DP2_FAST_USF_REPORT_BASE + 0x001 /* 0x3c61 */
#define DP2_AMR_BFI_TH_Base                       DP2_FAST_USF_REPORT_BASE + 0x001 /* 0x3c61 */
#define DP2_AMR_BFI_M_DTX_0                       DP2_FAST_USF_REPORT_BASE + 0x001 /* 0x3c61 */
#define DP2_CC_USF_S2                             DP2_FAST_USF_REPORT_BASE + 0x002 /* 0x3c62 */
#define DP2_AMR_BFI_M_DTX_1                       DP2_FAST_USF_REPORT_BASE + 0x002 /* 0x3c62 */
#define DP2_CC_USF_S3                             DP2_FAST_USF_REPORT_BASE + 0x003 /* 0x3c63 */
#define DP2_AMR_BFI_M_DTX_2                       DP2_FAST_USF_REPORT_BASE + 0x003 /* 0x3c63 */
#define DP2_CC_USF_S4                             DP2_FAST_USF_REPORT_BASE + 0x004 /* 0x3c64 */
#define DP2_AMR_BFI_M_DTX_3                       DP2_FAST_USF_REPORT_BASE + 0x004 /* 0x3c64 */
#define DP2_CC_USF2_S0                            DP2_FAST_USF_REPORT_BASE + 0x005 /* 0x3c65 */
#define DP2_AMR_BFI_M_Active_0                    DP2_FAST_USF_REPORT_BASE + 0x005 /* 0x3c65 */
#define DP2_CC_USF2_S1                            DP2_FAST_USF_REPORT_BASE + 0x006 /* 0x3c66 */
#define DP2_AMR_BFI_M_Active_1                    DP2_FAST_USF_REPORT_BASE + 0x006 /* 0x3c66 */
#define DP2_CC_USF2_S2                            DP2_FAST_USF_REPORT_BASE + 0x007 /* 0x3c67 */
#define DP2_AMR_BFI_M_Active_2                    DP2_FAST_USF_REPORT_BASE + 0x007 /* 0x3c67 */
#define DP2_CC_USF2_S3                            DP2_FAST_USF_REPORT_BASE + 0x008 /* 0x3c68 */
#define DP2_AMR_BFI_M_Active_3                    DP2_FAST_USF_REPORT_BASE + 0x008 /* 0x3c68 */
#define DP2_CC_USF2_S4                            DP2_FAST_USF_REPORT_BASE + 0x009 /* 0x3c69 */
#define DP2_AMR_BFI_SP_0_0                        DP2_FAST_USF_REPORT_BASE + 0x009 /* 0x3c69 */
#define DP2_CC_PRE_USF_S0                         DP2_FAST_USF_REPORT_BASE + 0x00a /* 0x3c6a */
#define DP2_AMR_BFI_SP_0_1                        DP2_FAST_USF_REPORT_BASE + 0x00a /* 0x3c6a */
#define DP2_CC_PRE_USF_S1                         DP2_FAST_USF_REPORT_BASE + 0x00b /* 0x3c6b */
#define DP2_AMR_BFI_SP_0_2                        DP2_FAST_USF_REPORT_BASE + 0x00b /* 0x3c6b */
#define DP2_CC_PRE_USF_S2                         DP2_FAST_USF_REPORT_BASE + 0x00c /* 0x3c6c */
#define DP2_AMR_BFI_SP_0_3                        DP2_FAST_USF_REPORT_BASE + 0x00c /* 0x3c6c */
#define DP2_CC_PRE_USF_S3                         DP2_FAST_USF_REPORT_BASE + 0x00d /* 0x3c6d */
#define DP2_AMR_BFI_SP_1_0                        DP2_FAST_USF_REPORT_BASE + 0x00d /* 0x3c6d */
#define DP2_CC_PRE_USF_S4                         DP2_FAST_USF_REPORT_BASE + 0x00e /* 0x3c6e */
#define DP2_AMR_BFI_SP_1_1                        DP2_FAST_USF_REPORT_BASE + 0x00e /* 0x3c6e */
#define DP2_CC_PRE_USF2_S0                        DP2_FAST_USF_REPORT_BASE + 0x00f /* 0x3c6f */
#define DP2_AMR_BFI_SP_1_2                        DP2_FAST_USF_REPORT_BASE + 0x00f /* 0x3c6f */
#define DP2_CC_PRE_USF2_S1                        DP2_FAST_USF_REPORT_BASE + 0x010 /* 0x3c70 */
#define DP2_AMR_BFI_SP_1_3                        DP2_FAST_USF_REPORT_BASE + 0x010 /* 0x3c70 */
#define DP2_CC_PRE_USF2_S2                        DP2_FAST_USF_REPORT_BASE + 0x011 /* 0x3c71 */
#define DP2_AMR_BFI_SP_2_0                        DP2_FAST_USF_REPORT_BASE + 0x011 /* 0x3c71 */
#define DP2_CC_PRE_USF2_S3                        DP2_FAST_USF_REPORT_BASE + 0x012 /* 0x3c72 */
#define DP2_AMR_BFI_SP_2_1                        DP2_FAST_USF_REPORT_BASE + 0x012 /* 0x3c72 */
#define DP2_CC_PRE_USF2_S4                        DP2_FAST_USF_REPORT_BASE + 0x013 /* 0x3c73 */
#define DP2_AMR_BFI_SP_2_2                        DP2_FAST_USF_REPORT_BASE + 0x013 /* 0x3c73 */
#define DP2_CC_CS_USF_S0                          DP2_FAST_USF_REPORT_BASE + 0x014 /* 0x3c74 */
#define DP2_AMR_BFI_SP_2_3                        DP2_FAST_USF_REPORT_BASE + 0x014 /* 0x3c74 */
#define DP2_CC_CS_USF_S1                          DP2_FAST_USF_REPORT_BASE + 0x015 /* 0x3c75 */
#define DP2_AMR_BFI_SP_2_X                        DP2_FAST_USF_REPORT_BASE + 0x015 /* 0x3c75 */
#define DP2_CC_CS_USF_S2                          DP2_FAST_USF_REPORT_BASE + 0x016 /* 0x3c76 */
#define DP2_AMR_BFI_SP_2_Y                        DP2_FAST_USF_REPORT_BASE + 0x016 /* 0x3c76 */
#define DP2_CC_CS_USF_S3                          DP2_FAST_USF_REPORT_BASE + 0x017 /* 0x3c77 */
#define DP2_CC_CS_USF_S4                          DP2_FAST_USF_REPORT_BASE + 0x018 /* 0x3c78 */
#define DP2_CC_CS_PRE_USF_S0                      DP2_FAST_USF_REPORT_BASE + 0x019 /* 0x3c79 */
#define DP2_CC_CS_PRE_USF_S1                      DP2_FAST_USF_REPORT_BASE + 0x01a /* 0x3c7a */
#define DP2_CC_CS_PRE_USF_S2                      DP2_FAST_USF_REPORT_BASE + 0x01b /* 0x3c7b */
#define DP2_CC_CS_PRE_USF_S3                      DP2_FAST_USF_REPORT_BASE + 0x01c /* 0x3c7c */
#define DP2_CC_CS_PRE_USF_S4                      DP2_FAST_USF_REPORT_BASE + 0x01d /* 0x3c7d */

#define DP2_TX_CCCH_BASE                          DP2_FAST_USF_REPORT_BASE + 0x01e /* 0x3c7e */

#define DP2_TX_CCCH_0                             DP2_TX_CCCH_BASE + 0x000 /* 0x3c7e */
#define DP2_TX_CCCH_1                             DP2_TX_CCCH_BASE + 0x001 /* 0x3c7f */
#define DP2_TX_CCCH_2                             DP2_TX_CCCH_BASE + 0x002 /* 0x3c80 */
#define DP2_TX_CCCH_3                             DP2_TX_CCCH_BASE + 0x003 /* 0x3c81 */
#define DP2_TX_CCCH_4                             DP2_TX_CCCH_BASE + 0x004 /* 0x3c82 */
#define DP2_TX_CCCH_5                             DP2_TX_CCCH_BASE + 0x005 /* 0x3c83 */
#define DP2_TX_CCCH_6                             DP2_TX_CCCH_BASE + 0x006 /* 0x3c84 */
#define DP2_TX_CCCH_7                             DP2_TX_CCCH_BASE + 0x007 /* 0x3c85 */
#define DP2_TX_CCCH_8                             DP2_TX_CCCH_BASE + 0x008 /* 0x3c86 */
#define DP2_TX_CCCH_9                             DP2_TX_CCCH_BASE + 0x009 /* 0x3c87 */
#define DP2_TX_CCCH_10                            DP2_TX_CCCH_BASE + 0x00a /* 0x3c88 */
#define DP2_TX_CCCH_11                            DP2_TX_CCCH_BASE + 0x00b /* 0x3c89 */

#define DP2_TX_FACCH_BASE                         DP2_TX_CCCH_BASE + 0x00c /* 0x3c8a */

#define DP2_TX_FACCH_0                            DP2_TX_FACCH_BASE + 0x000 /* 0x3c8a */
#define DP2_TX_FACCH_1                            DP2_TX_FACCH_BASE + 0x001 /* 0x3c8b */
#define DP2_TX_FACCH_2                            DP2_TX_FACCH_BASE + 0x002 /* 0x3c8c */
#define DP2_TX_FACCH_3                            DP2_TX_FACCH_BASE + 0x003 /* 0x3c8d */
#define DP2_TX_FACCH_4                            DP2_TX_FACCH_BASE + 0x004 /* 0x3c8e */
#define DP2_TX_FACCH_5                            DP2_TX_FACCH_BASE + 0x005 /* 0x3c8f */
#define DP2_TX_FACCH_6                            DP2_TX_FACCH_BASE + 0x006 /* 0x3c90 */
#define DP2_TX_FACCH_7                            DP2_TX_FACCH_BASE + 0x007 /* 0x3c91 */
#define DP2_TX_FACCH_8                            DP2_TX_FACCH_BASE + 0x008 /* 0x3c92 */
#define DP2_TX_FACCH_9                            DP2_TX_FACCH_BASE + 0x009 /* 0x3c93 */
#define DP2_TX_FACCH_10                           DP2_TX_FACCH_BASE + 0x00a /* 0x3c94 */
#define DP2_TX_FACCH_11                           DP2_TX_FACCH_BASE + 0x00b /* 0x3c95 */

#define DP2_TX_RACH_BASE                          DP2_TX_FACCH_BASE + 0x00c /* 0x3c96 */

#define DP2_TX_RACH_0                             DP2_TX_RACH_BASE + 0x000 /* 0x3c96 */
#define DP2_TX_RACH_1                             DP2_TX_RACH_BASE + 0x001 /* 0x3c97 */

#define DP2_TX_RATSCCH_BASE                       DP2_TX_RACH_BASE + 0x002 /* 0x3c98 */

#define DP2_TX_RATSCCH_0                          DP2_TX_RATSCCH_BASE + 0x000 /* 0x3c98 */
#define DP2_TX_RATSCCH_1                          DP2_TX_RATSCCH_BASE + 0x001 /* 0x3c99 */
#define DP2_TX_RATSCCH_2                          DP2_TX_RATSCCH_BASE + 0x002 /* 0x3c9a */
#define DP2_TX_RATSCCH_3                          DP2_TX_RATSCCH_BASE + 0x003 /* 0x3c9b */

#define DP2_RX_CCCH_PTCCH0_BASE                   DP2_TX_RATSCCH_BASE + 0x004 /* 0x3c9c */

#define DP2_RX_PTCCH0                             DP2_RX_CCCH_PTCCH0_BASE + 0x000 /* 0x3c9c */
#define DP2_RX_CCCH_0                             DP2_RX_CCCH_PTCCH0_BASE + 0x000 /* 0x3c9c */
#define DP2_RX_CCCH_1                             DP2_RX_CCCH_PTCCH0_BASE + 0x001 /* 0x3c9d */
#define DP2_RX_CCCH_2                             DP2_RX_CCCH_PTCCH0_BASE + 0x002 /* 0x3c9e */
#define DP2_RX_CCCH_3                             DP2_RX_CCCH_PTCCH0_BASE + 0x003 /* 0x3c9f */
#define DP2_RX_CCCH_4                             DP2_RX_CCCH_PTCCH0_BASE + 0x004 /* 0x3ca0 */
#define DP2_RX_CCCH_5                             DP2_RX_CCCH_PTCCH0_BASE + 0x005 /* 0x3ca1 */
#define DP2_RX_CCCH_6                             DP2_RX_CCCH_PTCCH0_BASE + 0x006 /* 0x3ca2 */
#define DP2_RX_CCCH_7                             DP2_RX_CCCH_PTCCH0_BASE + 0x007 /* 0x3ca3 */
#define DP2_RX_CCCH_8                             DP2_RX_CCCH_PTCCH0_BASE + 0x008 /* 0x3ca4 */
#define DP2_RX_CCCH_9                             DP2_RX_CCCH_PTCCH0_BASE + 0x009 /* 0x3ca5 */
#define DP2_RX_CCCH_10                            DP2_RX_CCCH_PTCCH0_BASE + 0x00a /* 0x3ca6 */
#define DP2_RX_CCCH_11                            DP2_RX_CCCH_PTCCH0_BASE + 0x00b /* 0x3ca7 */
#define DP2_RX_CCCH_12                            DP2_RX_CCCH_PTCCH0_BASE + 0x00c /* 0x3ca8 */

#define DP2_RX_FACCH_PTCCH1_BASE                  DP2_RX_CCCH_PTCCH0_BASE + 0x00d /* 0x3ca9 */

#define DP2_RX_PTCCH1                             DP2_RX_FACCH_PTCCH1_BASE + 0x000 /* 0x3ca9 */
#define DP2_RX_FACCH_0                            DP2_RX_FACCH_PTCCH1_BASE + 0x000 /* 0x3ca9 */
#define DP2_RX_FACCH_1                            DP2_RX_FACCH_PTCCH1_BASE + 0x001 /* 0x3caa */
#define DP2_RX_FACCH_2                            DP2_RX_FACCH_PTCCH1_BASE + 0x002 /* 0x3cab */
#define DP2_RX_FACCH_3                            DP2_RX_FACCH_PTCCH1_BASE + 0x003 /* 0x3cac */
#define DP2_RX_FACCH_4                            DP2_RX_FACCH_PTCCH1_BASE + 0x004 /* 0x3cad */
#define DP2_RX_FACCH_5                            DP2_RX_FACCH_PTCCH1_BASE + 0x005 /* 0x3cae */
#define DP2_RX_FACCH_6                            DP2_RX_FACCH_PTCCH1_BASE + 0x006 /* 0x3caf */
#define DP2_RX_FACCH_7                            DP2_RX_FACCH_PTCCH1_BASE + 0x007 /* 0x3cb0 */
#define DP2_RX_FACCH_8                            DP2_RX_FACCH_PTCCH1_BASE + 0x008 /* 0x3cb1 */
#define DP2_RX_FACCH_9                            DP2_RX_FACCH_PTCCH1_BASE + 0x009 /* 0x3cb2 */
#define DP2_RX_FACCH_10                           DP2_RX_FACCH_PTCCH1_BASE + 0x00a /* 0x3cb3 */
#define DP2_RX_FACCH_11                           DP2_RX_FACCH_PTCCH1_BASE + 0x00b /* 0x3cb4 */
#define DP2_RX_FACCH_12                           DP2_RX_FACCH_PTCCH1_BASE + 0x00c /* 0x3cb5 */

#define DP2_RX_SCH_BASE                           DP2_RX_FACCH_PTCCH1_BASE + 0x00d /* 0x3cb6 */

#define DP2_RX_SCH_0                              DP2_RX_SCH_BASE + 0x000 /* 0x3cb6 */
#define DP2_RX_SCH_1                              DP2_RX_SCH_BASE + 0x001 /* 0x3cb7 */
#define DP2_RX_SCH_2                              DP2_RX_SCH_BASE + 0x002 /* 0x3cb8 */
#define DP2_RX_SCH_3                              DP2_RX_SCH_BASE + 0x003 /* 0x3cb9 */

#define DP2_RX_RATSCCH_BASE                       DP2_RX_SCH_BASE + 0x004 /* 0x3cba */

#define DP2_RX_RATSCCH_0                          DP2_RX_RATSCCH_BASE + 0x000 /* 0x3cba */
#define DP2_RX_RATSCCH_1                          DP2_RX_RATSCCH_BASE + 0x001 /* 0x3cbb */
#define DP2_RX_RATSCCH_2                          DP2_RX_RATSCCH_BASE + 0x002 /* 0x3cbc */
#define DP2_RX_RATSCCH_3                          DP2_RX_RATSCCH_BASE + 0x003 /* 0x3cbd */

#define DP_TX_TCH_S0_BASE                         DP2_RX_RATSCCH_BASE + 0x004 /* 0x3cbe */

#define DP2_TX_TCH_S0_0                           DP_TX_TCH_S0_BASE + 0x000 /* 0x3cbe */
#define DP2_TX_TCH_S0_1                           DP_TX_TCH_S0_BASE + 0x001 /* 0x3cbf */
#define DP2_TX_TCH_S0_2                           DP_TX_TCH_S0_BASE + 0x002 /* 0x3cc0 */
#define DP2_TX_TCH_S0_3                           DP_TX_TCH_S0_BASE + 0x003 /* 0x3cc1 */
#define DP2_TX_TCH_S0_4                           DP_TX_TCH_S0_BASE + 0x004 /* 0x3cc2 */
#define DP2_TX_TCH_S0_5                           DP_TX_TCH_S0_BASE + 0x005 /* 0x3cc3 */
#define DP2_TX_TCH_S0_6                           DP_TX_TCH_S0_BASE + 0x006 /* 0x3cc4 */
#define DP2_TX_TCH_S0_7                           DP_TX_TCH_S0_BASE + 0x007 /* 0x3cc5 */

#define DP_TX_TCH_S1_BASE                         DP_TX_TCH_S0_BASE + 0x008 /* 0x3cc6 */

#define DP2_TX_TCH_S1_0                           DP_TX_TCH_S1_BASE + 0x000 /* 0x3cc6 */
#define DP2_BFI_QUAL_0                            DP_TX_TCH_S1_BASE + 0x000 /* 0x3cc6 */
#define DP2_TX_TCH_S1_1                           DP_TX_TCH_S1_BASE + 0x001 /* 0x3cc7 */
#define DP2_BFI_QUAL_1                            DP_TX_TCH_S1_BASE + 0x001 /* 0x3cc7 */
#define DP2_TX_TCH_S1_2                           DP_TX_TCH_S1_BASE + 0x002 /* 0x3cc8 */
#define DP2_BFI_QUAL_2                            DP_TX_TCH_S1_BASE + 0x002 /* 0x3cc8 */
#define DP2_TX_TCH_S1_3                           DP_TX_TCH_S1_BASE + 0x003 /* 0x3cc9 */
#define DP2_BFI_QUAL_3                            DP_TX_TCH_S1_BASE + 0x003 /* 0x3cc9 */
#define DP2_TX_TCH_S1_4                           DP_TX_TCH_S1_BASE + 0x004 /* 0x3cca */
#define DP2_TX_TCH_S1_5                           DP_TX_TCH_S1_BASE + 0x005 /* 0x3ccb */
#define DP2_TX_TCH_S1_6                           DP_TX_TCH_S1_BASE + 0x006 /* 0x3ccc */
#define DP2_TX_TCH_S1_7                           DP_TX_TCH_S1_BASE + 0x007 /* 0x3ccd */

#define DP_TX_TCH_S2_BASE                         DP_TX_TCH_S1_BASE + 0x008 /* 0x3cce */

#define DP2_TX_TCH_S2_0                           DP_TX_TCH_S2_BASE + 0x000 /* 0x3cce */
#define DP2_AMR_RX_Trace_00_Base                  DP_TX_TCH_S2_BASE + 0x000 /* 0x3cce */
#define DP2_AMR_RX_Flow_Flag                      DP_TX_TCH_S2_BASE + 0x000 /* 0x3cce */
#define DP2_TX_TCH_S2_1                           DP_TX_TCH_S2_BASE + 0x001 /* 0x3ccf */
#define DP2_AMR_SV_SD_Type                        DP_TX_TCH_S2_BASE + 0x001 /* 0x3ccf */
#define DP2_TX_TCH_S2_2                           DP_TX_TCH_S2_BASE + 0x002 /* 0x3cd0 */
#define DP2_AMR_Config_Flags_Trace                DP_TX_TCH_S2_BASE + 0x002 /* 0x3cd0 */
#define DP2_TX_TCH_S2_3                           DP_TX_TCH_S2_BASE + 0x003 /* 0x3cd1 */
#define DP2_AMR_IM_dec                            DP_TX_TCH_S2_BASE + 0x003 /* 0x3cd1 */
#define DP2_TX_TCH_S2_4                           DP_TX_TCH_S2_BASE + 0x004 /* 0x3cd2 */
#define DP2_AMR_T2_CD                             DP_TX_TCH_S2_BASE + 0x004 /* 0x3cd2 */
#define DP2_TX_TCH_S2_5                           DP_TX_TCH_S2_BASE + 0x005 /* 0x3cd3 */
#define DP2_AMR_BFI                               DP_TX_TCH_S2_BASE + 0x005 /* 0x3cd3 */
#define DP2_TX_TCH_S2_6                           DP_TX_TCH_S2_BASE + 0x006 /* 0x3cd4 */
#define DP2_AMR_DL_FACCH                          DP_TX_TCH_S2_BASE + 0x006 /* 0x3cd4 */
#define DP2_TX_TCH_S2_7                           DP_TX_TCH_S2_BASE + 0x007 /* 0x3cd5 */
#define DP2_AMR_DL_RATSCCH                        DP_TX_TCH_S2_BASE + 0x007 /* 0x3cd5 */

#define DP_TX_TCH_S3_BASE                         DP_TX_TCH_S2_BASE + 0x008 /* 0x3cd6 */

#define DP2_TX_TCH_S3_0                           DP_TX_TCH_S3_BASE + 0x000 /* 0x3cd6 */
#define DP2_AMR_TX_Trace_Base                     DP_TX_TCH_S3_BASE + 0x000 /* 0x3cd6 */
#define DP2_AMR_T2_CE                             DP_TX_TCH_S3_BASE + 0x000 /* 0x3cd6 */
#define DP2_TX_TCH_S3_1                           DP_TX_TCH_S3_BASE + 0x001 /* 0x3cd7 */
#define DP2_AMR_UL_FACCH                          DP_TX_TCH_S3_BASE + 0x001 /* 0x3cd7 */
#define DP2_TX_TCH_S3_2                           DP_TX_TCH_S3_BASE + 0x002 /* 0x3cd8 */
#define DP2_AMR_UL_RATSCCH                        DP_TX_TCH_S3_BASE + 0x002 /* 0x3cd8 */
#define DP2_TX_TCH_S3_3                           DP_TX_TCH_S3_BASE + 0x003 /* 0x3cd9 */
#define DP2_AMR_S2_SP_Flag                        DP_TX_TCH_S3_BASE + 0x003 /* 0x3cd9 */
#define DP2_TX_TCH_S3_4                           DP_TX_TCH_S3_BASE + 0x004 /* 0x3cda */
#define DP2_AMR_SID_Stolen                        DP_TX_TCH_S3_BASE + 0x004 /* 0x3cda */
#define DP2_TX_TCH_S3_5                           DP_TX_TCH_S3_BASE + 0x005 /* 0x3cdb */
#define DP2_AMR_SV_SC_Type                        DP_TX_TCH_S3_BASE + 0x005 /* 0x3cdb */
#define DP2_TX_TCH_S3_6                           DP_TX_TCH_S3_BASE + 0x006 /* 0x3cdc */
#define DP2_AMR_TX_Flow_Flag                      DP_TX_TCH_S3_BASE + 0x006 /* 0x3cdc */
#define DP2_TX_TCH_S3_7                           DP_TX_TCH_S3_BASE + 0x007 /* 0x3cdd */

#define DP_TX_TCH_S4_BASE                         DP_TX_TCH_S3_BASE + 0x008 /* 0x3cde */

#define DP2_TX_TCH_S4_0                           DP_TX_TCH_S4_BASE + 0x000 /* 0x3cde */
#define DP2_TX_TCH_S4_1                           DP_TX_TCH_S4_BASE + 0x001 /* 0x3cdf */
#define DP2_TX_TCH_S4_2                           DP_TX_TCH_S4_BASE + 0x002 /* 0x3ce0 */
#define DP2_TX_TCH_S4_3                           DP_TX_TCH_S4_BASE + 0x003 /* 0x3ce1 */
#define DP2_TX_TCH_S4_4                           DP_TX_TCH_S4_BASE + 0x004 /* 0x3ce2 */
#define DP2_TX_TCH_S4_5                           DP_TX_TCH_S4_BASE + 0x005 /* 0x3ce3 */
#define DP2_TX_TCH_S4_6                           DP_TX_TCH_S4_BASE + 0x006 /* 0x3ce4 */
#define DP2_TX_TCH_S4_7                           DP_TX_TCH_S4_BASE + 0x007 /* 0x3ce5 */

#define DP_RX_TCH_S0_BASE                         DP_TX_TCH_S4_BASE + 0x008 /* 0x3ce6 */

#define DP2_RX_TCH_S0_0                           DP_RX_TCH_S0_BASE + 0x000 /* 0x3ce6 */
#define DP2_RX_TCH_S0_1                           DP_RX_TCH_S0_BASE + 0x001 /* 0x3ce7 */
#define DP2_RX_TCH_S0_2                           DP_RX_TCH_S0_BASE + 0x002 /* 0x3ce8 */
#define DP2_RX_TCH_S0_3                           DP_RX_TCH_S0_BASE + 0x003 /* 0x3ce9 */
#define DP2_RX_TCH_S0_4                           DP_RX_TCH_S0_BASE + 0x004 /* 0x3cea */
#define DP2_RX_TCH_S0_5                           DP_RX_TCH_S0_BASE + 0x005 /* 0x3ceb */
#define DP2_RX_TCH_S0_6                           DP_RX_TCH_S0_BASE + 0x006 /* 0x3cec */
#define DP2_RX_TCH_S0_7                           DP_RX_TCH_S0_BASE + 0x007 /* 0x3ced */

#define DP_RX_TCH_S1_BASE                         DP_RX_TCH_S0_BASE + 0x008 /* 0x3cee */

#define DP2_RX_TCH_S1_0                           DP_RX_TCH_S1_BASE + 0x000 /* 0x3cee */
#define DP2_RX_TCH_S1_1                           DP_RX_TCH_S1_BASE + 0x001 /* 0x3cef */
#define DP2_RX_TCH_S1_2                           DP_RX_TCH_S1_BASE + 0x002 /* 0x3cf0 */
#define DP2_RX_TCH_S1_3                           DP_RX_TCH_S1_BASE + 0x003 /* 0x3cf1 */
#define DP2_RX_TCH_S1_4                           DP_RX_TCH_S1_BASE + 0x004 /* 0x3cf2 */
#define DP2_RX_TCH_S1_5                           DP_RX_TCH_S1_BASE + 0x005 /* 0x3cf3 */
#define DP2_RX_TCH_S1_6                           DP_RX_TCH_S1_BASE + 0x006 /* 0x3cf4 */
#define DP2_RX_TCH_S1_7                           DP_RX_TCH_S1_BASE + 0x007 /* 0x3cf5 */

#define DP_RX_TCH_S2_BASE                         DP_RX_TCH_S1_BASE + 0x008 /* 0x3cf6 */

#define DP2_RX_TCH_S2_0                           DP_RX_TCH_S2_BASE + 0x000 /* 0x3cf6 */
#define DP2_RX_TCH_S2_1                           DP_RX_TCH_S2_BASE + 0x001 /* 0x3cf7 */
#define DP2_RX_TCH_S2_2                           DP_RX_TCH_S2_BASE + 0x002 /* 0x3cf8 */
#define DP2_RX_TCH_S2_3                           DP_RX_TCH_S2_BASE + 0x003 /* 0x3cf9 */
#define DP2_RX_TCH_S2_4                           DP_RX_TCH_S2_BASE + 0x004 /* 0x3cfa */
#define DP2_RX_TCH_S2_5                           DP_RX_TCH_S2_BASE + 0x005 /* 0x3cfb */
#define DP2_RX_TCH_S2_6                           DP_RX_TCH_S2_BASE + 0x006 /* 0x3cfc */
#define DP2_RX_TCH_S2_7                           DP_RX_TCH_S2_BASE + 0x007 /* 0x3cfd */

#define DP_RX_TCH_S3_BASE                         DP_RX_TCH_S2_BASE + 0x008 /* 0x3cfe */

#define DP2_RX_TCH_S3_0                           DP_RX_TCH_S3_BASE + 0x000 /* 0x3cfe */
#define DP2_RX_TCH_S3_1                           DP_RX_TCH_S3_BASE + 0x001 /* 0x3cff */
#define DP2_RX_TCH_S3_2                           DP_RX_TCH_S3_BASE + 0x002 /* 0x3d00 */
#define DP2_RX_TCH_S3_3                           DP_RX_TCH_S3_BASE + 0x003 /* 0x3d01 */
#define DP2_RX_TCH_S3_4                           DP_RX_TCH_S3_BASE + 0x004 /* 0x3d02 */
#define DP2_RX_TCH_S3_5                           DP_RX_TCH_S3_BASE + 0x005 /* 0x3d03 */
#define DP2_RX_TCH_S3_6                           DP_RX_TCH_S3_BASE + 0x006 /* 0x3d04 */
#define DP2_RX_TCH_S3_7                           DP_RX_TCH_S3_BASE + 0x007 /* 0x3d05 */

#define DP_RX_TCH_S4_BASE                         DP_RX_TCH_S3_BASE + 0x008 /* 0x3d06 */

#define DP2_RX_TCH_S4_0                           DP_RX_TCH_S4_BASE + 0x000 /* 0x3d06 */
#define DP2_RX_TCH_S4_1                           DP_RX_TCH_S4_BASE + 0x001 /* 0x3d07 */
#define DP2_RX_TCH_S4_2                           DP_RX_TCH_S4_BASE + 0x002 /* 0x3d08 */
#define DP2_RX_TCH_S4_3                           DP_RX_TCH_S4_BASE + 0x003 /* 0x3d09 */
#define DP2_RX_TCH_S4_4                           DP_RX_TCH_S4_BASE + 0x004 /* 0x3d0a */
#define DP2_RX_TCH_S4_5                           DP_RX_TCH_S4_BASE + 0x005 /* 0x3d0b */
#define DP2_RX_TCH_S4_6                           DP_RX_TCH_S4_BASE + 0x006 /* 0x3d0c */
#define DP2_RX_TCH_S4_7                           DP_RX_TCH_S4_BASE + 0x007 /* 0x3d0d */

#define DP2_TEST_FEATURES_S0_BASE                 DP_RX_TCH_S4_BASE + 0x008 /* 0x3d0e */

#define DP2_TX_TEST_S0_0                          DP2_TEST_FEATURES_S0_BASE + 0x000 /* 0x3d0e */
#define DP2_TX_TEST_S0_1                          DP2_TEST_FEATURES_S0_BASE + 0x001 /* 0x3d0f */
#define DP2_TX_TEST_S0_2                          DP2_TEST_FEATURES_S0_BASE + 0x002 /* 0x3d10 */

#define DP2_TEST_FEATURES_S1_BASE                 DP2_TEST_FEATURES_S0_BASE + 0x003 /* 0x3d11 */

#define DP2_TX_TEST_S1_0                          DP2_TEST_FEATURES_S1_BASE + 0x000 /* 0x3d11 */
#define DP2_TX_TEST_S1_1                          DP2_TEST_FEATURES_S1_BASE + 0x001 /* 0x3d12 */
#define DP2_TX_TEST_S1_2                          DP2_TEST_FEATURES_S1_BASE + 0x002 /* 0x3d13 */

#define DP2_TEST_FEATURES_S2_BASE                 DP2_TEST_FEATURES_S1_BASE + 0x003 /* 0x3d14 */

#define DP2_TX_TEST_S2_0                          DP2_TEST_FEATURES_S2_BASE + 0x000 /* 0x3d14 */
#define DP2_TX_TEST_S2_1                          DP2_TEST_FEATURES_S2_BASE + 0x001 /* 0x3d15 */
#define DP2_TX_TEST_S2_2                          DP2_TEST_FEATURES_S2_BASE + 0x002 /* 0x3d16 */

#define DP2_TEST_FEATURES_S3_BASE                 DP2_TEST_FEATURES_S2_BASE + 0x003 /* 0x3d17 */

#define DP2_TX_TEST_S3_0                          DP2_TEST_FEATURES_S3_BASE + 0x000 /* 0x3d17 */
#define DP2_TX_TEST_S3_1                          DP2_TEST_FEATURES_S3_BASE + 0x001 /* 0x3d18 */
#define DP2_TX_TEST_S3_2                          DP2_TEST_FEATURES_S3_BASE + 0x002 /* 0x3d19 */

#define DP2_TEST_FEATURES_S4_BASE                 DP2_TEST_FEATURES_S3_BASE + 0x003 /* 0x3d1a */

#define DP2_TX_TEST_S4_0                          DP2_TEST_FEATURES_S4_BASE + 0x000 /* 0x3d1a */
#define DP2_TX_TEST_S4_1                          DP2_TEST_FEATURES_S4_BASE + 0x001 /* 0x3d1b */
#define DP2_TX_TEST_S4_2                          DP2_TEST_FEATURES_S4_BASE + 0x002 /* 0x3d1c */

#define DP_OUTER_REPORT_BASE                      DP2_TEST_FEATURES_S4_BASE + 0x003 /* 0x3d1d */

#define DP2_BFI_USED_THRES                        DP_OUTER_REPORT_BASE + 0x000 /* 0x3d1d */
#define DP2_BFI_USED_D                            DP_OUTER_REPORT_BASE + 0x001 /* 0x3d1e */

#define DP_EGPRS_BITMAP_DEBUG_BASE                DP_OUTER_REPORT_BASE + 0x002 /* 0x3d1f */

#define DP_EGPRS_BITMAP_DEBUG_0                   DP_EGPRS_BITMAP_DEBUG_BASE + 0x000 /* 0x3d1f */
#define DP_EGPRS_BITMAP_DEBUG_1                   DP_EGPRS_BITMAP_DEBUG_BASE + 0x001 /* 0x3d20 */
#define DP_EGPRS_BITMAP_DEBUG_2                   DP_EGPRS_BITMAP_DEBUG_BASE + 0x002 /* 0x3d21 */
#define DP_EGPRS_BITMAP_DEBUG_3                   DP_EGPRS_BITMAP_DEBUG_BASE + 0x003 /* 0x3d22 */
#define DP_EGPRS_BITMAP_DEBUG_4                   DP_EGPRS_BITMAP_DEBUG_BASE + 0x004 /* 0x3d23 */
#define DP_EGPRS_BITMAP_DEBUG_5                   DP_EGPRS_BITMAP_DEBUG_BASE + 0x005 /* 0x3d24 */

#define DP_MODEM_RESERVED_BASE                    DP_EGPRS_BITMAP_DEBUG_BASE + 0x006 /* 0x3d25 */

#define DP_VAMOS_DEBUG_ZETA_EST                   DP_MODEM_RESERVED_BASE + 0x000 /* 0x3d25 */
#define DP_VAMOS_DEBUG_SCORE1                     DP_MODEM_RESERVED_BASE + 0x001 /* 0x3d26 */
#define DP_VAMOS_DEBUG_SCORE2                     DP_MODEM_RESERVED_BASE + 0x002 /* 0x3d27 */
#define DP_PREV_T2                                DP_MODEM_RESERVED_BASE + 0x003 /* 0x3d28 */
#define DP_PREV_WIN_PROC_0123                     DP_MODEM_RESERVED_BASE + 0x004 /* 0x3d29 */
#define DP_PREV_WIN_PROC_4567                     DP_MODEM_RESERVED_BASE + 0x005 /* 0x3d2a */
#define DP_CURR_T2                                DP_MODEM_RESERVED_BASE + 0x006 /* 0x3d2b */
#define DP_CURR_WIN_PROC_0123                     DP_MODEM_RESERVED_BASE + 0x007 /* 0x3d2c */
#define DP_CURR_WIN_PROC_4567                     DP_MODEM_RESERVED_BASE + 0x008 /* 0x3d2d */
#define DP_DIS_EGPRS_REPORT                       DP_MODEM_RESERVED_BASE + 0x009 /* 0x3d2e */

#define DP2_SP_CODEC_CTRL_BASE                    DP_MODEM_RESERVED_BASE + 0x047 /* 0x3d6c */

#define DP_SC_ADDR                                DP2_SP_CODEC_CTRL_BASE + 0x000 /* 0x3d6c */
#define DP2_SC_FLAGS                              DP2_SP_CODEC_CTRL_BASE + 0x000 /* 0x3d6c */
#define SPH_COD_CTRL                              DP2_SP_CODEC_CTRL_BASE + 0x000 /* 0x3d6c */
#define DP2_DEL_PCM_R                             DP2_SP_CODEC_CTRL_BASE + 0x001 /* 0x3d6d */
#define SPH_DEL_R                                 DP2_SP_CODEC_CTRL_BASE + 0x001 /* 0x3d6d */
#define DP2_DEL_PCM_W                             DP2_SP_CODEC_CTRL_BASE + 0x002 /* 0x3d6e */
#define SPH_DEL_W                                 DP2_SP_CODEC_CTRL_BASE + 0x002 /* 0x3d6e */
#define DP_MARGIN_PCM_ADDR                        DP2_SP_CODEC_CTRL_BASE + 0x003 /* 0x3d6f */
#define DP2_MARGIN_PCM_W                          DP2_SP_CODEC_CTRL_BASE + 0x003 /* 0x3d6f */
#define SPH_DEL_M                                 DP2_SP_CODEC_CTRL_BASE + 0x003 /* 0x3d6f */
#define DP2_SC_MODE                               DP2_SP_CODEC_CTRL_BASE + 0x004 /* 0x3d70 */
#define SPH_COD_MODE                              DP2_SP_CODEC_CTRL_BASE + 0x004 /* 0x3d70 */
#define DP2_SC_VAD                                DP2_SP_CODEC_CTRL_BASE + 0x005 /* 0x3d71 */
#define DP2_SC_MUTE                               DP2_SP_CODEC_CTRL_BASE + 0x006 /* 0x3d72 */
#define DP2_Nsync_Not_ETSI_C                      DP2_SP_CODEC_CTRL_BASE + 0x006 /* 0x3d72 */
#define SPH_MUTE_CTRL                             DP2_SP_CODEC_CTRL_BASE + 0x006 /* 0x3d72 */
#define DP2_Offset_Count                          DP2_SP_CODEC_CTRL_BASE + 0x007 /* 0x3d73 */
#define SPH_DEL_O                                 DP2_SP_CODEC_CTRL_BASE + 0x007 /* 0x3d73 */
#define DP2_Nsync_value                           DP2_SP_CODEC_CTRL_BASE + 0x008 /* 0x3d74 */

#define DP2_SP_AUDIO_INTERFACE_CTRL_BASE          DP2_SP_CODEC_CTRL_BASE + 0x009 /* 0x3d75 */

#define DP_AUDIO_ADDR                             DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x000 /* 0x3d75 */
#define SPH_DL_VOL                                DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x000 /* 0x3d75 */
#define SPH_8K_CTRL                               DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x001 /* 0x3d76 */
#define AUDIO_PAR                                 DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x001 /* 0x3d76 */
#define SPH_UL_BKF_FLT_TAP                        DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x002 /* 0x3d77 */
#define SPH_DL_BKF_FLT_TAP                        DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x004 /* 0x3d79 */
#define SPH_UL_VOL                                DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x005 /* 0x3d7a */
#define SPH_ST_VOL                                DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x006 /* 0x3d7b */
#define AUDIO_ASP_WAV_OUT_GAIN                    DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x007 /* 0x3d7c */
#define SPH_PM_ADDR_BKF_FLT_COEF_UL_NB            DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x008 /* 0x3d7d */
#define SPH_PM_ADDR_BKF_FLT_COEF_DL_NB            DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x009 /* 0x3d7e */
#define SPH_PM_ADDR_BKF_FLT_COEF_UL_WB            DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x00a /* 0x3d7f */
#define SPH_PM_ADDR_BKF_FLT_COEF_DL_WB            DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x00b /* 0x3d80 */
#define SPH_PM_ADDR_ST_FLT_COEF                   DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x00c /* 0x3d81 */
#define SPH_PM_ADDR_SRC_FLT_COEF                  DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x00d /* 0x3d82 */
#define SPH_DM_ADDR_EMP                           DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x00e /* 0x3d83 */
#define DP_BT_earphone_mode                       DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x00f /* 0x3d84 */
#define SPH_BT_MODE                               DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x00f /* 0x3d84 */
#define DP_ADAPTIVE_GAIN_CONTROL                  DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x010 /* 0x3d85 */
#define SPH_ENH_DL_CTRL                           DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x010 /* 0x3d85 */

#define DP2_SP_KT_CTRL_BASE                       DP2_SP_AUDIO_INTERFACE_CTRL_BASE + 0x013 /* 0x3d88 */

#define DP_KEYTONE_CTRL                           DP2_SP_KT_CTRL_BASE + 0x000 /* 0x3d88 */
#define KEYTONE1_CTRL                             DP2_SP_KT_CTRL_BASE + 0x000 /* 0x3d88 */
#define KEYTONE2_CTRL                             DP2_SP_KT_CTRL_BASE + 0x001 /* 0x3d89 */
#define DP_ASP_FORCE_KT_8K                        DP2_SP_KT_CTRL_BASE + 0x002 /* 0x3d8a */
#define KEYTONE1_FREQ                             DP2_SP_KT_CTRL_BASE + 0x003 /* 0x3d8b */
#define KEYTONE2_FREQ                             DP2_SP_KT_CTRL_BASE + 0x006 /* 0x3d8e */
#define DP_KEYTONE_ATT_GAIN                       DP2_SP_KT_CTRL_BASE + 0x009 /* 0x3d91 */
#define KEYTONE_ATT_GAIN_ADDR                     DP2_SP_KT_CTRL_BASE + 0x009 /* 0x3d91 */

#define DP2_SP_8kPCM_CTRL_BASE                    DP2_SP_KT_CTRL_BASE + 0x00a /* 0x3d92 */

#define DP_ASP_WAV_REC_CTRL                       DP2_SP_8kPCM_CTRL_BASE + 0x000 /* 0x3d92 */
#define ASP_WAV_REC_CTRL                          DP2_SP_8kPCM_CTRL_BASE + 0x000 /* 0x3d92 */
#define DP_8kPCM_MIC_REC_IDX                      DP2_SP_8kPCM_CTRL_BASE + 0x001 /* 0x3d93 */
#define ASP_WAV_REC_IDX                           DP2_SP_8kPCM_CTRL_BASE + 0x001 /* 0x3d93 */
#define DP_8kPCM_MIC_REC_LEN                      DP2_SP_8kPCM_CTRL_BASE + 0x002 /* 0x3d94 */
#define ASP_WAV_REC_LEN                           DP2_SP_8kPCM_CTRL_BASE + 0x002 /* 0x3d94 */
#define DP_8kPCM_SPK_REC_IDX                      DP2_SP_8kPCM_CTRL_BASE + 0x003 /* 0x3d95 */
#define ASP_DL_WAV_REC_IDX                        DP2_SP_8kPCM_CTRL_BASE + 0x003 /* 0x3d95 */
#define DP_8kPCM_SPK_REC_LEN                      DP2_SP_8kPCM_CTRL_BASE + 0x004 /* 0x3d96 */
#define ASP_DL_WAV_REC_LEN                        DP2_SP_8kPCM_CTRL_BASE + 0x004 /* 0x3d96 */
#define SPH_PCM_REC_CTRL                          DP2_SP_8kPCM_CTRL_BASE + 0x005 /* 0x3d97 */
#define SPH_DM_ADDR_EPL_UL_PRE_BUF                DP2_SP_8kPCM_CTRL_BASE + 0x006 /* 0x3d98 */
#define SPH_DM_ADDR_EPL_UL_POS_BUF                DP2_SP_8kPCM_CTRL_BASE + 0x007 /* 0x3d99 */
#define SPH_DM_ADDR_EPL_DL_PRE_BUF                DP2_SP_8kPCM_CTRL_BASE + 0x008 /* 0x3d9a */
#define SPH_DM_ADDR_EPL_DL_POS_BUF                DP2_SP_8kPCM_CTRL_BASE + 0x009 /* 0x3d9b */
#define SPH_DM_ADDR_SE2_PTR                       DP2_SP_8kPCM_CTRL_BASE + 0x00a /* 0x3d9c */
#define DP_D2C_SPEECH_UL_INT                      DP2_SP_8kPCM_CTRL_BASE + 0x00c /* 0x3d9e */
#define DP_D2C_SPEECH_DL_INT                      DP2_SP_8kPCM_CTRL_BASE + 0x00d /* 0x3d9f */
#define DP_SPH_3G_SYNC_FLAG                       DP2_SP_8kPCM_CTRL_BASE + 0x00e /* 0x3da0 */
#define SPH_DM_ADDR_BGS_UL_BUF                    DP2_SP_8kPCM_CTRL_BASE + 0x00f /* 0x3da1 */
#define SPH_DM_ADDR_BGS_DL_BUF                    DP2_SP_8kPCM_CTRL_BASE + 0x010 /* 0x3da2 */
#define SPH_BGS_LEN_UL                            DP2_SP_8kPCM_CTRL_BASE + 0x011 /* 0x3da3 */
#define SPH_BGS_LEN_DL                            DP2_SP_8kPCM_CTRL_BASE + 0x012 /* 0x3da4 */
#define SPH_BGS_CTRL                              DP2_SP_8kPCM_CTRL_BASE + 0x013 /* 0x3da5 */
#define SPH_BGS_MIX                               DP2_SP_8kPCM_CTRL_BASE + 0x014 /* 0x3da6 */

#define DP2_SP_VM_CTRL_BASE                       DP2_SP_8kPCM_CTRL_BASE + 0x015 /* 0x3da7 */

#define DP_AWB_RX_TCH_S0_0                        DP2_SP_VM_CTRL_BASE + 0x000 /* 0x3da7 */
#define SPH_2G_SD_DATA_HDR                        DP2_SP_VM_CTRL_BASE + 0x000 /* 0x3da7 */
#define DP_SD1_VM_1                               DP2_SP_VM_CTRL_BASE + 0x001 /* 0x3da8 */
#define SPH_2G_SD_DATA_HB                         DP2_SP_VM_CTRL_BASE + 0x001 /* 0x3da8 */
#define DP2_AWB_SD_TCH_MOD                        DP2_SP_VM_CTRL_BASE + 0x01f /* 0x3dc6 */
#define DP_AWB_TX_TCH_S0_0                        DP2_SP_VM_CTRL_BASE + 0x020 /* 0x3dc7 */
#define SPH_2G_SE_DATA_HDR                        DP2_SP_VM_CTRL_BASE + 0x020 /* 0x3dc7 */
#define DP_SD2_VM_1                               DP2_SP_VM_CTRL_BASE + 0x021 /* 0x3dc8 */
#define SPH_2G_SE_DATA_HB                         DP2_SP_VM_CTRL_BASE + 0x021 /* 0x3dc8 */
#define DP_Encoder_Used_Mode                      DP2_SP_VM_CTRL_BASE + 0x042 /* 0x3de9 */
#define DP_Decoder_Used_Mode                      DP2_SP_VM_CTRL_BASE + 0x043 /* 0x3dea */
#define DP_VM_DBG_INFO                            DP2_SP_VM_CTRL_BASE + 0x044 /* 0x3deb */
#define DP_3G_DEBUG_INFO                          DP2_SP_VM_CTRL_BASE + 0x044 /* 0x3deb */
#define SPH_CTM_AMR_REAL_RX_TYPE                  DP2_SP_VM_CTRL_BASE + 0x07c /* 0x3e23 */

#define DP2_SP_CTM_BASE                           DP2_SP_VM_CTRL_BASE + 0x081 /* 0x3e28 */

#define SPH_DM_ADDR_PNW_UL_BUF                    DP2_SP_CTM_BASE + 0x000 /* 0x3e28 */
#define SPH_DM_ADDR_PNW_DL_BUF                    DP2_SP_CTM_BASE + 0x002 /* 0x3e2a */
#define SPH_PNW_CTRL_UL                           DP2_SP_CTM_BASE + 0x004 /* 0x3e2c */
#define SPH_PNW_CTRL_DL                           DP2_SP_CTM_BASE + 0x005 /* 0x3e2d */
#define SPH_CTM_CTRL                              DP2_SP_CTM_BASE + 0x006 /* 0x3e2e */
#define SPH_CTM_BFI_FACCH_REPORT                  DP2_SP_CTM_BASE + 0x007 /* 0x3e2f */
#define SPH_PNW_LEN_UL                            DP2_SP_CTM_BASE + 0x008 /* 0x3e30 */
#define SPH_PNW_LEN_DL                            DP2_SP_CTM_BASE + 0x009 /* 0x3e31 */
#define SPH_CTM_AMR_CODEBOOK_GAIN_LIMIT           DP2_SP_CTM_BASE + 0x00a /* 0x3e32 */
#define SPH_CTM_AMR_CODEBOOK_GAIN_UPDATE          DP2_SP_CTM_BASE + 0x00b /* 0x3e33 */
#define SPH_CTM_COSIM_CTRL                        DP2_SP_CTM_BASE + 0x00c /* 0x3e34 */

#define DP2_SP_AEC_CTRL_BASE                      DP2_SP_CTM_BASE + 0x00d /* 0x3e35 */

#define DP_EC_CTRL_ADDR                           DP2_SP_AEC_CTRL_BASE + 0x000 /* 0x3e35 */
#define DP2_EC_SUPP                               DP2_SP_AEC_CTRL_BASE + 0x000 /* 0x3e35 */
#define SPH_ENH_UL_CTRL                           DP2_SP_AEC_CTRL_BASE + 0x000 /* 0x3e35 */
#define DP_AEC_CTRL                               DP2_SP_AEC_CTRL_BASE + 0x001 /* 0x3e36 */
#define DP_TDNC_CTRL                              DP2_SP_AEC_CTRL_BASE + 0x001 /* 0x3e36 */
#define DP_EES_CTRL                               DP2_SP_AEC_CTRL_BASE + 0x002 /* 0x3e37 */
#define DP_DMNR_CTRL                              DP2_SP_AEC_CTRL_BASE + 0x003 /* 0x3e38 */
#define DP_AEC_CONTROL_WORD                       DP2_SP_AEC_CTRL_BASE + 0x004 /* 0x3e39 */
#define DP_EC_PAR_ADDR                            DP2_SP_AEC_CTRL_BASE + 0x007 /* 0x3e3c */
#define DP2_ES_Time_Const                         DP2_SP_AEC_CTRL_BASE + 0x007 /* 0x3e3c */
#define DP2_ES_Vol_Const                          DP2_SP_AEC_CTRL_BASE + 0x008 /* 0x3e3d */

#define DP2_SP_VR_CTRL_BASE                       DP2_SP_AEC_CTRL_BASE + 0x00f /* 0x3e44 */

#define DP_VR_CTRL                                DP2_SP_VR_CTRL_BASE + 0x000 /* 0x3e44 */
#define DP_VR_IO_BASE                             DP2_SP_VR_CTRL_BASE + 0x001 /* 0x3e45 */
#define DP2_DATA_AP_DATA_UL                       DP2_SP_VR_CTRL_BASE + 0x004 /* 0x3e48 */
#define DP2_DATA_AP_DATA_DL                       DP2_SP_VR_CTRL_BASE + 0x005 /* 0x3e49 */

#define DP2_SP_AUDIO_CTRL_BASE                    DP2_SP_VR_CTRL_BASE + 0x00e /* 0x3e52 */

#define DP_TEST_SIM_SBSD_CTRL                     DP2_SP_AUDIO_CTRL_BASE + 0x000 /* 0x3e52 */
#define DP2_TEST_SIM_IND                          DP2_SP_AUDIO_CTRL_BASE + 0x000 /* 0x3e52 */
#define AWB_LB_CHECKSUM_ADDR                      DP2_SP_AUDIO_CTRL_BASE + 0x001 /* 0x3e53 */
#define AFE_BT_LB_CHECKSUM_ADDR                   DP2_SP_AUDIO_CTRL_BASE + 0x001 /* 0x3e53 */
#define SPE_RAM_LB_CHECK_ADDR                     DP2_SP_AUDIO_CTRL_BASE + 0x001 /* 0x3e53 */
#define DP_AUDIO_CTRL2                            DP2_SP_AUDIO_CTRL_BASE + 0x002 /* 0x3e54 */
#define DP_ENHANCED_AUDIO_CTRL                    DP2_SP_AUDIO_CTRL_BASE + 0x003 /* 0x3e55 */
#define SPH_SCH_IMPROVE_CTRL                      DP2_SP_AUDIO_CTRL_BASE + 0x005 /* 0x3e57 */
#define SPH_BGS_UL_GAIN                           DP2_SP_AUDIO_CTRL_BASE + 0x006 /* 0x3e58 */
#define SPH_BGS_DL_GAIN                           DP2_SP_AUDIO_CTRL_BASE + 0x007 /* 0x3e59 */
#define SPH_3G_SE_RATE_UPDATE                     DP2_SP_AUDIO_CTRL_BASE + 0x008 /* 0x3e5a */
#define SPH_3G_SD_RATE_UPDATE                     DP2_SP_AUDIO_CTRL_BASE + 0x009 /* 0x3e5b */
#define DP_AWB_SE_CTRL                            DP2_SP_AUDIO_CTRL_BASE + 0x00a /* 0x3e5c */
#define DP_AWB_SE_STATUS                          DP2_SP_AUDIO_CTRL_BASE + 0x00a /* 0x3e5c */
#define DP_AWB_SD_CTRL                            DP2_SP_AUDIO_CTRL_BASE + 0x00b /* 0x3e5d */
#define DP_AWB_SD_STATUS                          DP2_SP_AUDIO_CTRL_BASE + 0x00b /* 0x3e5d */
#define DP_G723_SE_CTRL                           DP2_SP_AUDIO_CTRL_BASE + 0x00c /* 0x3e5e */
#define DP_G723_SD_CTRL                           DP2_SP_AUDIO_CTRL_BASE + 0x00d /* 0x3e5f */
#define DP_NR_CTRL                                DP2_SP_AUDIO_CTRL_BASE + 0x00e /* 0x3e60 */

#define DP2_SP_AUDIO_ENHANCE_BASE                 DP2_SP_AUDIO_CTRL_BASE + 0x00f /* 0x3e61 */

#define DP_DSP_DEAD_INT                           DP2_SP_AUDIO_ENHANCE_BASE + 0x000 /* 0x3e61 */
#define DP_AMR_Mode_Error_Assert_Type             DP2_SP_AUDIO_ENHANCE_BASE + 0x001 /* 0x3e62 */
#define DP_AMR_Mode_Error_Assert_Flag             DP2_SP_AUDIO_ENHANCE_BASE + 0x002 /* 0x3e63 */
#define DP_Audio_Flexi_Ctrl                       DP2_SP_AUDIO_ENHANCE_BASE + 0x003 /* 0x3e64 */
#define DP_VOICE_CHG_SCH                          DP2_SP_AUDIO_ENHANCE_BASE + 0x004 /* 0x3e65 */
#define DP_VOICE_CHG_TYPE                         DP2_SP_AUDIO_ENHANCE_BASE + 0x005 /* 0x3e66 */
#define DP_BT_Earphone_UL_Control                 DP2_SP_AUDIO_ENHANCE_BASE + 0x006 /* 0x3e67 */
#define SPH_BT_CTRL                               DP2_SP_AUDIO_ENHANCE_BASE + 0x006 /* 0x3e67 */
#define SPH_DM_ADDR_SE_PTR                        DP2_SP_AUDIO_ENHANCE_BASE + 0x007 /* 0x3e68 */
#define SPH_DM_ADDR_SD_PTR                        DP2_SP_AUDIO_ENHANCE_BASE + 0x008 /* 0x3e69 */
#define DP_UP_DOWN_SAMPL_CTRL                     DP2_SP_AUDIO_ENHANCE_BASE + 0x009 /* 0x3e6a */
#define DP_DL_NR_CTRL                             DP2_SP_AUDIO_ENHANCE_BASE + 0x00a /* 0x3e6b */
#define DP_Audio_DAC_CTRL                         DP2_SP_AUDIO_ENHANCE_BASE + 0x00b /* 0x3e6c */
#define DP_VBI_SYNC_BT_Earphone_CTRL              DP2_SP_AUDIO_ENHANCE_BASE + 0x00c /* 0x3e6d */
#define DP_SBSD_ThRESHOLD                         DP2_SP_AUDIO_ENHANCE_BASE + 0x00d /* 0x3e6e */
#define DP2_8k_SP_CTRL                            DP2_SP_AUDIO_ENHANCE_BASE + 0x00f /* 0x3e70 */
#define SPH_SFE_CTRL                              DP2_SP_AUDIO_ENHANCE_BASE + 0x00f /* 0x3e70 */
#define DP2_AUDIO_VIA_BT_CTRL                     DP2_SP_AUDIO_ENHANCE_BASE + 0x010 /* 0x3e71 */
#define DP2_AUDIO_VIA_BT_INT                      DP2_SP_AUDIO_ENHANCE_BASE + 0x011 /* 0x3e72 */
#define DP2_AGC_CTRL                              DP2_SP_AUDIO_ENHANCE_BASE + 0x012 /* 0x3e73 */
#define DP2_AGC_GAIN                              DP2_SP_AUDIO_ENHANCE_BASE + 0x013 /* 0x3e74 */
#define DP2_DATA_AP_CTRL                          DP2_SP_AUDIO_ENHANCE_BASE + 0x014 /* 0x3e75 */
#define DP2_20ms_SP_CTRL                          DP2_SP_AUDIO_ENHANCE_BASE + 0x015 /* 0x3e76 */
#define DP2_COMPEN_BLOCK_FILTER                   DP2_SP_AUDIO_ENHANCE_BASE + 0x015 /* 0x3e76 */
#define DP_605_Task_Id                            DP2_SP_AUDIO_ENHANCE_BASE + 0x016 /* 0x3e77 */
#define DP_605_no_8k_Task_Id                      DP2_SP_AUDIO_ENHANCE_BASE + 0x017 /* 0x3e78 */

#define DP2_Reserve_BASE                          DP2_SP_AUDIO_ENHANCE_BASE + 0x018 /* 0x3e79 */

#define SPH_3G_CTRL                               DP2_Reserve_BASE + 0x000 /* 0x3e79 */
#define DP2_DDL_MCU_DSP_HAND_SHAKE                DP2_Reserve_BASE + 0x001 /* 0x3e7a */

#define DP2_AWB_8K_BASE                           DP2_Reserve_BASE + 0x002 /* 0x3e7b */

#define DP2_RINI_INT                              DP2_AWB_8K_BASE + 0x006 /* 0x3e81 */
#define DP2_DL_RIN_sig_energy                     DP2_AWB_8K_BASE + 0x007 /* 0x3e82 */
#define SPH_DYNA_FWLA_MODE_CTRL                   DP2_AWB_8K_BASE + 0x008 /* 0x3e83 */

#define DP2_EPL_BND_BASE                          DP2_AWB_8K_BASE + 0x009 /* 0x3e84 */

#define SPH_EPL_BND                               DP2_EPL_BND_BASE + 0x000 /* 0x3e84 */

#define DP2_AWB_LINK_EXTEND_BASE                  DP2_EPL_BND_BASE + 0x001 /* 0x3e85 */

#define DP_SD1_AWB_VM_0                           DP2_AWB_LINK_EXTEND_BASE + 0x000 /* 0x3e85 */
#define DP_3G_RX_VM_0                             DP2_AWB_LINK_EXTEND_BASE + 0x000 /* 0x3e85 */
#define SPH_3G_SD_DATA_HDR                        DP2_AWB_LINK_EXTEND_BASE + 0x000 /* 0x3e85 */
#define SPH_AP_SD_DATA_HDR                        DP2_AWB_LINK_EXTEND_BASE + 0x000 /* 0x3e85 */
#define DP_Org_SD1_VM_1                           DP2_AWB_LINK_EXTEND_BASE + 0x001 /* 0x3e86 */
#define SPH_3G_SD_DATA_HB                         DP2_AWB_LINK_EXTEND_BASE + 0x001 /* 0x3e86 */
#define SPH_AP_SD_DATA_HB                         DP2_AWB_LINK_EXTEND_BASE + 0x001 /* 0x3e86 */
#define DP_SD2_AWB_VM_0                           DP2_AWB_LINK_EXTEND_BASE + 0x020 /* 0x3ea5 */
#define DP_3G_TX_VM_0                             DP2_AWB_LINK_EXTEND_BASE + 0x020 /* 0x3ea5 */
#define SPH_3G_SE_DATA_HDR                        DP2_AWB_LINK_EXTEND_BASE + 0x020 /* 0x3ea5 */
#define SPH_AP_SE_DATA_HDR                        DP2_AWB_LINK_EXTEND_BASE + 0x020 /* 0x3ea5 */
#define DP_Org_SD2_VM_1                           DP2_AWB_LINK_EXTEND_BASE + 0x021 /* 0x3ea6 */
#define SPH_3G_SE_DATA_HB                         DP2_AWB_LINK_EXTEND_BASE + 0x021 /* 0x3ea6 */
#define SPH_AP_SE_DATA_HB                         DP2_AWB_LINK_EXTEND_BASE + 0x021 /* 0x3ea6 */
#define DP_SD2_AWB_VM_18                          DP2_AWB_LINK_EXTEND_BASE + 0x032 /* 0x3eb7 */
#define DP_WB_DEBUG_INFO                          DP2_AWB_LINK_EXTEND_BASE + 0x040 /* 0x3ec5 */

#define DP_SPEECH_OTHER_BASE                      DP2_AWB_LINK_EXTEND_BASE + 0x04c /* 0x3ed1 */

#define DP_UL_COMFORT_NOISE_THRESHOLD             DP_SPEECH_OTHER_BASE + 0x000 /* 0x3ed1 */
#define DP_UL_COMFORT_NOISE_SHIFT                 DP_SPEECH_OTHER_BASE + 0x001 /* 0x3ed2 */
#define DP_DTX_DSPK_FUN_CTRL_ADDR                 DP_SPEECH_OTHER_BASE + 0x002 /* 0x3ed3 */
#define DP_HR_BER_THD_ADDR                        DP_SPEECH_OTHER_BASE + 0x003 /* 0x3ed4 */
#define DP_FR_BER_THD_ADDR                        DP_SPEECH_OTHER_BASE + 0x004 /* 0x3ed5 */
#define DP_EFR_BER_THD_ADDR                       DP_SPEECH_OTHER_BASE + 0x005 /* 0x3ed6 */
#define DP2_TIME_STAMP_2G_H                       DP_SPEECH_OTHER_BASE + 0x006 /* 0x3ed7 */
#define DP2_TIME_STAMP_2G_L                       DP_SPEECH_OTHER_BASE + 0x007 /* 0x3ed8 */
#define DP2_TIME_STAMP_3G_H                       DP_SPEECH_OTHER_BASE + 0x008 /* 0x3ed9 */
#define DP2_TIME_STAMP_3G_L                       DP_SPEECH_OTHER_BASE + 0x009 /* 0x3eda */
#define DP2_SPEECH_DEBUG_MODE                     DP_SPEECH_OTHER_BASE + 0x00a /* 0x3edb */
#define SPH_DBG_MOD                               DP_SPEECH_OTHER_BASE + 0x00a /* 0x3edb */
#define DP2_ABF_CTRL_1                            DP_SPEECH_OTHER_BASE + 0x00b /* 0x3edc */
#define DP2_ABF_CTRL_2                            DP_SPEECH_OTHER_BASE + 0x00c /* 0x3edd */
#define SPH_DUMIC_CTRL                            DP_SPEECH_OTHER_BASE + 0x00d /* 0x3ede */
#define DP_SPEECH_RESERVED_BASE                   DP_SPEECH_OTHER_BASE + 0x00e /* 0x3edf */

#define DP_SPH_COSIM_RESULT_REPORT                DP_SPEECH_RESERVED_BASE + 0x000 /* 0x3edf */
#define DP_SPH_COSIM_FAIL_TYPE                    DP_SPEECH_RESERVED_BASE + 0x001 /* 0x3ee0 */
#define DP_SPH_SPECIAL_LOOPBACK_CTRL              DP_SPEECH_RESERVED_BASE + 0x002 /* 0x3ee1 */
#define DP_TASK5_COSIM_HANDSHAKE                  DP_SPEECH_RESERVED_BASE + 0x003 /* 0x3ee2 */
#define DP_AGC_SW_GAIN1                           DP_SPEECH_RESERVED_BASE + 0x004 /* 0x3ee3 */
#define DP_AGC_SW_GAIN2                           DP_SPEECH_RESERVED_BASE + 0x005 /* 0x3ee4 */
#define SPH_DACA_CTRL                             DP_SPEECH_RESERVED_BASE + 0x006 /* 0x3ee5 */
#define SPH_DM_ADDR_DACA_UL_BUF                   DP_SPEECH_RESERVED_BASE + 0x007 /* 0x3ee6 */
#define SPH_DM_ADDR_DACA_DL_BUF                   DP_SPEECH_RESERVED_BASE + 0x008 /* 0x3ee7 */
#define SPH_DACA_LEN_UL                           DP_SPEECH_RESERVED_BASE + 0x009 /* 0x3ee8 */
#define SPH_DACA_LEN_DL                           DP_SPEECH_RESERVED_BASE + 0x00a /* 0x3ee9 */
#define SPH_SW_TRANSC_SD_CNTR                     DP_SPEECH_RESERVED_BASE + 0x00b /* 0x3eea */
#define SPH_SW_TRANSC_TAF_CNTR                    DP_SPEECH_RESERVED_BASE + 0x00c /* 0x3eeb */
#define SPH_APP_MODE                              DP_SPEECH_RESERVED_BASE + 0x00d /* 0x3eec */
#define SPH_VOLTE_JBM_CTRL                        DP_SPEECH_RESERVED_BASE + 0x00e /* 0x3eed */
#define SPH_PCM_FRM_PEAK_UL                       DP_SPEECH_RESERVED_BASE + 0x00f /* 0x3eee */
#define SPH_PCM_FRM_PEAK_DL                       DP_SPEECH_RESERVED_BASE + 0x010 /* 0x3eef */
#define SPH_WARN_MSG_UL                           DP_SPEECH_RESERVED_BASE + 0x011 /* 0x3ef0 */
#define SPH_WARN_MSG_DL                           DP_SPEECH_RESERVED_BASE + 0x012 /* 0x3ef1 */
#define SPH_8K_RESYNC_CTRL                        DP_SPEECH_RESERVED_BASE + 0x013 /* 0x3ef2 */
#define SPH_8K_RESYNC_OFFSET_UL                   DP_SPEECH_RESERVED_BASE + 0x014 /* 0x3ef3 */
#define SPH_8K_RESYNC_OFFSET_DL                   DP_SPEECH_RESERVED_BASE + 0x015 /* 0x3ef4 */
#define SPH_8K_DIS_VBI_CTRL                       DP_SPEECH_RESERVED_BASE + 0x016 /* 0x3ef5 */
#define SPH_ENH_DYNAMIC_SWITCH                    DP_SPEECH_RESERVED_BASE + 0x017 /* 0x3ef6 */
#define SPH_ENH_DEBUG_ADDR                        DP_SPEECH_RESERVED_BASE + 0x018 /* 0x3ef7 */
#define DP2_AUDIO_CTRL_FIELD_BASE                 DP_SPEECH_RESERVED_BASE + 0x019 /* 0x3ef8 */

#define DP_AUDIO_ASP_TYPE_ADDR                    DP2_AUDIO_CTRL_FIELD_BASE + 0x000 /* 0x3ef8 */
#define DP_AUDIO_ASP_FS_ADDR                      DP2_AUDIO_CTRL_FIELD_BASE + 0x001 /* 0x3ef9 */
#define DP_Audio_ASP_DEL_W_ADDR                   DP2_AUDIO_CTRL_FIELD_BASE + 0x002 /* 0x3efa */
#define DP_AUDIO_DEC_FRAME_LENGTH                 DP2_AUDIO_CTRL_FIELD_BASE + 0x003 /* 0x3efb */
#define DP_Audio_ASP_DEL_MARGIN_W_ADDR            DP2_AUDIO_CTRL_FIELD_BASE + 0x004 /* 0x3efc */
#define DP_VOL_OUT                                DP2_AUDIO_CTRL_FIELD_BASE + 0x005 /* 0x3efd */

#define A2V_BASE                                  DP2_AUDIO_CTRL_FIELD_BASE + 0x006 /* 0x3efe */
#define A2V_INT_ENABLE                            A2V_BASE + 0x000 /* 0x3efe */
#define A2V_INT_CNT                               A2V_BASE + 0x001 /* 0x3eff */
#define A2V_INT_STAMP                             A2V_BASE + 0x002 /* 0x3f00 */
#define A2V_AUDIO_FRMAE_COUNT                     A2V_BASE + 0x003 /* 0x3f01 */
#define A2V_VIDEO_FRAME_COUNT                     A2V_BASE + 0x004 /* 0x3f02 */
#define A2V_INT_DSP_COUNT                         A2V_BASE + 0x005 /* 0x3f03 */
#define A2V_INTERRUPT_PADDING_FLAG                A2V_BASE + 0x006 /* 0x3f04 */
#define A2V_SPEECH_COUNT                          A2V_BASE + 0x007 /* 0x3f05 */

#define DP2_WAV_CTRL_FIELD_BASE                   A2V_BASE + 0x008 /* 0x3f06 */
#define DP_ASP_WAV_BASE_CTRL                      DP2_WAV_CTRL_FIELD_BASE + 0x000 /* 0x3f06 */
#define DP_ASP_WAV_W_IO_ADDR                      DP2_WAV_CTRL_FIELD_BASE + 0x001 /* 0x3f07 */
#define DP_ASP_WAV_R_IO_ADDR                      DP2_WAV_CTRL_FIELD_BASE + 0x002 /* 0x3f08 */
#define DP2_WAV_STEREO_FLAG                       DP2_WAV_CTRL_FIELD_BASE + 0x003 /* 0x3f09 */

#define DP2_WT_CTRL_FIELD_BASE                    DP2_WAV_CTRL_FIELD_BASE + 0x004 /* 0x3f0a */
#define DP_WT_CONTROL                             DP2_WT_CTRL_FIELD_BASE + 0x000 /* 0x3f0a */
#define DP2_MP3_CTRL_FIELD_BASE                   DP2_WT_CTRL_FIELD_BASE + 0x001 /* 0x3f0b */

#define MP3_CONTROL_BASE_ADD                      DP2_MP3_CTRL_FIELD_BASE + 0x000 /* 0x3f0b */
#define SBC_DEC_CTRL                              DP2_MP3_CTRL_FIELD_BASE + 0x000 /* 0x3f0b */
#define PCM_SD_PLAYBACK_CTRL                      DP2_MP3_CTRL_FIELD_BASE + 0x000 /* 0x3f0b */
#define MP3_CONTROL_1                             DP2_MP3_CTRL_FIELD_BASE + 0x001 /* 0x3f0c */
#define SBC_PAR_MAGIC_WORD                        DP2_MP3_CTRL_FIELD_BASE + 0x001 /* 0x3f0c */
#define PCM_SD_PLAYBACK_STATUS                    DP2_MP3_CTRL_FIELD_BASE + 0x001 /* 0x3f0c */
#define MP3_CONTROL_2                             DP2_MP3_CTRL_FIELD_BASE + 0x002 /* 0x3f0d */
#define SBC_DEC_STATUS                            DP2_MP3_CTRL_FIELD_BASE + 0x002 /* 0x3f0d */
#define PCM_SD_PLAYBACK_DM_PAGE                   DP2_MP3_CTRL_FIELD_BASE + 0x002 /* 0x3f0d */
#define MP3_CONTROL_3                             DP2_MP3_CTRL_FIELD_BASE + 0x003 /* 0x3f0e */
#define SBC_DEC_DM_BS_PAGE                        DP2_MP3_CTRL_FIELD_BASE + 0x003 /* 0x3f0e */
#define PCM_SD_PLAYBACK_DM_ADDR                   DP2_MP3_CTRL_FIELD_BASE + 0x003 /* 0x3f0e */
#define MP3_CONTROL_4                             DP2_MP3_CTRL_FIELD_BASE + 0x004 /* 0x3f0f */
#define SBC_DEC_DM_BS_ADDR                        DP2_MP3_CTRL_FIELD_BASE + 0x004 /* 0x3f0f */
#define PCM_SD_PLAYBACK_DM_MCU_W_PTR              DP2_MP3_CTRL_FIELD_BASE + 0x004 /* 0x3f0f */
#define MP3_CONTROL_5                             DP2_MP3_CTRL_FIELD_BASE + 0x005 /* 0x3f10 */
#define SBC_DEC_DM_BS_LEN                         DP2_MP3_CTRL_FIELD_BASE + 0x005 /* 0x3f10 */
#define PCM_SD_PLAYBACK_DM_DSP_R_PTR              DP2_MP3_CTRL_FIELD_BASE + 0x005 /* 0x3f10 */
#define MP3_CONTROL_6                             DP2_MP3_CTRL_FIELD_BASE + 0x006 /* 0x3f11 */
#define SBC_DEC_DM_BS_REQ_TH                      DP2_MP3_CTRL_FIELD_BASE + 0x006 /* 0x3f11 */
#define MP3_CONTROL_7                             DP2_MP3_CTRL_FIELD_BASE + 0x007 /* 0x3f12 */
#define SBC_DEC_DM_BS_MCU_W_PTR                   DP2_MP3_CTRL_FIELD_BASE + 0x007 /* 0x3f12 */
#define MP3_CONTROL_8                             DP2_MP3_CTRL_FIELD_BASE + 0x008 /* 0x3f13 */
#define SBC_DEC_DM_BS_DSP_R_PTR                   DP2_MP3_CTRL_FIELD_BASE + 0x008 /* 0x3f13 */
#define MP3_CONTROL_9                             DP2_MP3_CTRL_FIELD_BASE + 0x009 /* 0x3f14 */
#define MP3_CONTROL_10                            DP2_MP3_CTRL_FIELD_BASE + 0x00a /* 0x3f15 */
#define MP3_CONTROL_11                            DP2_MP3_CTRL_FIELD_BASE + 0x00b /* 0x3f16 */
#define MP3_CONTROL_12                            DP2_MP3_CTRL_FIELD_BASE + 0x00c /* 0x3f17 */
#define MP3_CONTROL_13                            DP2_MP3_CTRL_FIELD_BASE + 0x00d /* 0x3f18 */
#define MP3_CONTROL_14                            DP2_MP3_CTRL_FIELD_BASE + 0x00e /* 0x3f19 */
#define MP3_CONTROL_15                            DP2_MP3_CTRL_FIELD_BASE + 0x00f /* 0x3f1a */
#define MP3_CONTROL_16                            DP2_MP3_CTRL_FIELD_BASE + 0x010 /* 0x3f1b */
#define MP3_CONTROL_17                            DP2_MP3_CTRL_FIELD_BASE + 0x011 /* 0x3f1c */
#define MP3_CONTROL_18                            DP2_MP3_CTRL_FIELD_BASE + 0x012 /* 0x3f1d */
#define MP3_CONTROL_19                            DP2_MP3_CTRL_FIELD_BASE + 0x013 /* 0x3f1e */
#define MP3_CONTROL_20                            DP2_MP3_CTRL_FIELD_BASE + 0x014 /* 0x3f1f */
#define MP3_CONTROL_21                            DP2_MP3_CTRL_FIELD_BASE + 0x015 /* 0x3f20 */
#define MP3_CONTROL_22                            DP2_MP3_CTRL_FIELD_BASE + 0x016 /* 0x3f21 */
#define MP3_CONTROL_23                            DP2_MP3_CTRL_FIELD_BASE + 0x017 /* 0x3f22 */
#define MP3_CONTROL_24                            DP2_MP3_CTRL_FIELD_BASE + 0x018 /* 0x3f23 */
#define MP3_CONTROL_25                            DP2_MP3_CTRL_FIELD_BASE + 0x019 /* 0x3f24 */
#define MP3_CONTROL_26                            DP2_MP3_CTRL_FIELD_BASE + 0x01a /* 0x3f25 */
#define MP3_CONTROL_27                            DP2_MP3_CTRL_FIELD_BASE + 0x01b /* 0x3f26 */
#define MP3_CONTROL_28                            DP2_MP3_CTRL_FIELD_BASE + 0x01c /* 0x3f27 */

#define DP2_AUDIO_PP_CTRL_BASE                    DP2_MP3_CTRL_FIELD_BASE + 0x01d /* 0x3f28 */
#define DP2_AUDIO_PP_CTRL                         DP2_AUDIO_PP_CTRL_BASE + 0x000 /* 0x3f28 */
#define DP_AUDIO_GAIN_STEP                        DP2_AUDIO_PP_CTRL_BASE + 0x001 /* 0x3f29 */
#define AUDIO_PP_TS_SPEED                         DP2_AUDIO_PP_CTRL_BASE + 0x002 /* 0x3f2a */
#define AUDIO_PP_ReserveA1                        DP2_AUDIO_PP_CTRL_BASE + 0x003 /* 0x3f2b */
#define Audio_debug_sherif1                       DP2_AUDIO_PP_CTRL_BASE + 0x003 /* 0x3f2b */
#define AUDIO_PP_TS_APM_to_DSP_Length             DP2_AUDIO_PP_CTRL_BASE + 0x004 /* 0x3f2c */
#define AUD_VSG_CTRL                              DP2_AUDIO_PP_CTRL_BASE + 0x005 /* 0x3f2d */
#define VS_MOD_FREQ                               DP2_AUDIO_PP_CTRL_BASE + 0x006 /* 0x3f2e */
#define VS_CENTER_FREQ                            DP2_AUDIO_PP_CTRL_BASE + 0x007 /* 0x3f2f */
#define VS_DIGI_GAIN                              DP2_AUDIO_PP_CTRL_BASE + 0x008 /* 0x3f30 */
#define AUDIO_CH_ASSIGNMENT                       DP2_AUDIO_PP_CTRL_BASE + 0x009 /* 0x3f31 */
#define AUDIO_IIR_CTRL                            DP2_AUDIO_PP_CTRL_BASE + 0x00a /* 0x3f32 */
#define DP_AUDIO_DEC_CNTR_THRESHOLD               DP2_AUDIO_PP_CTRL_BASE + 0x00b /* 0x3f33 */
#define DP_AUDIO_CUR_GAIN                         DP2_AUDIO_PP_CTRL_BASE + 0x00c /* 0x3f34 */
#define AUDIO_CHECK_ASP_TYPE                      DP2_AUDIO_PP_CTRL_BASE + 0x00d /* 0x3f35 */
#define PCM_Route_Data_Address                    DP2_AUDIO_PP_CTRL_BASE + 0x00e /* 0x3f36 */
#define PCM_Route_Data_Length                     DP2_AUDIO_PP_CTRL_BASE + 0x00f /* 0x3f37 */
#define PCM_Route_Data_Pagenum                    DP2_AUDIO_PP_CTRL_BASE + 0x010 /* 0x3f38 */
#define AUDIO_8K_INT_COUNT                        DP2_AUDIO_PP_CTRL_BASE + 0x011 /* 0x3f39 */
#define AUDIO_32K_Record_CTRL                     DP2_AUDIO_PP_CTRL_BASE + 0x012 /* 0x3f3a */
#define DYNAMIC_DOWNLOAD_MCU_STATUS               DP2_AUDIO_PP_CTRL_BASE + 0x013 /* 0x3f3b */
#define DYNAMIC_DOWNLOAD_STATUS                   DP2_AUDIO_PP_CTRL_BASE + 0x014 /* 0x3f3c */

#define DP2_EXTRA_APPLICATION_FILED_BASE          DP2_AUDIO_PP_CTRL_BASE + 0x015 /* 0x3f3d */

#define DP_AUDIO_ASP_COMMON_FLAG_1                DP2_EXTRA_APPLICATION_FILED_BASE + 0x000 /* 0x3f3d */
#define AUDIO_RAMPDOWN_ENABLE                     DP2_EXTRA_APPLICATION_FILED_BASE + 0x001 /* 0x3f3e */
#define Audio_SD_ISR_Cycle                        DP2_EXTRA_APPLICATION_FILED_BASE + 0x002 /* 0x3f3f */
#define DP_AUDIO_ASP_COMMON_FLAG_2                DP2_EXTRA_APPLICATION_FILED_BASE + 0x003 /* 0x3f40 */
#define DP_AUDIO_ASP_D2M_COUNT                    DP2_EXTRA_APPLICATION_FILED_BASE + 0x004 /* 0x3f41 */
#define DP_TASK4_COSIM_HANDSHAKE                  DP2_EXTRA_APPLICATION_FILED_BASE + 0x005 /* 0x3f42 */

#define DP2_Dynamic_Load_field_BASE               DP2_EXTRA_APPLICATION_FILED_BASE + 0x006 /* 0x3f43 */

#define DDL_Task_Flag                             DP2_Dynamic_Load_field_BASE + 0x000 /* 0x3f43 */
#define DP2_Dynamic_Load_field                    DP2_Dynamic_Load_field_BASE + 0x000 /* 0x3f43 */
#define DDL_Check_Position                        DP2_Dynamic_Load_field_BASE + 0x001 /* 0x3f44 */
#define DDL_Task_ID                               DP2_Dynamic_Load_field_BASE + 0x002 /* 0x3f45 */

#define DP2_AUDIO_RESERVED_BASE                   DP2_Dynamic_Load_field_BASE + 0x003 /* 0x3f46 */

#define DP2_AUDIO_RESERVED_BASE1                  DP2_AUDIO_RESERVED_BASE + 0x000 /* 0x3f46 */
#define DP2_AUDIO_RESERVED_BASE2                  DP2_AUDIO_RESERVED_BASE + 0x001 /* 0x3f47 */

#define DP2_UNDEFINEDD_BASE                       DP2_AUDIO_RESERVED_BASE + 0x002 /* 0x3f48 */

#define SPH_AUX_P2W_CTRL                          DP2_UNDEFINEDD_BASE + 0x000 /* 0x3f48 */
#define SPH_DM_ADDR_AUX_P2W_UL_BUF                DP2_UNDEFINEDD_BASE + 0x003 /* 0x3f4b */
#define SPH_DM_ADDR_AUX_P2W_DL_BUF                DP2_UNDEFINEDD_BASE + 0x004 /* 0x3f4c */
#define SPH_AFE_VDSP_CON_CTRL                     DP2_UNDEFINEDD_BASE + 0x005 /* 0x3f4d */
#define ENH_DYNAMIC_STATE_MACHINE                 DP2_UNDEFINEDD_BASE + 0x006 /* 0x3f4e */
#define ENH_FLAG_PARAMETER                        DP2_UNDEFINEDD_BASE + 0x007 /* 0x3f4f */
#define I2S_RX_AUD_DEC_CTRL                       DP2_UNDEFINEDD_BASE + 0x008 /* 0x3f50 */
#define I2S_RX_AUD_DEC_FRAME_LENGTH               DP2_UNDEFINEDD_BASE + 0x009 /* 0x3f51 */
#define I2S_RX_AUD_DEC_PAGE_NUMBER                DP2_UNDEFINEDD_BASE + 0x00a /* 0x3f52 */
#define I2S_RX_AUD_DEC_ADDRESS                    DP2_UNDEFINEDD_BASE + 0x00b /* 0x3f53 */
#define DP_TASK6_HANDSHAKE                        DP2_UNDEFINEDD_BASE + 0x00c /* 0x3f54 */
#define SPH_UL_D2M_CTRL                           DP2_UNDEFINEDD_BASE + 0x00d /* 0x3f55 */
#define SPH_UL_PERIOD                             DP2_UNDEFINEDD_BASE + 0x00e /* 0x3f56 */
#define AUD_CLK_SKEW_PIN_VAL                      DP2_UNDEFINEDD_BASE + 0x00f /* 0x3f57 */
#define AUD_CLK_SKEW_PON_VAL                      DP2_UNDEFINEDD_BASE + 0x010 /* 0x3f58 */
#define AUD_CLK_PIN_PON_IDX                       DP2_UNDEFINEDD_BASE + 0x011 /* 0x3f59 */
#define SPH_UL_WAIT_CTRL                          DP2_UNDEFINEDD_BASE + 0x012 /* 0x3f5a */
#define SPH_UL_WAIT_TICK                          DP2_UNDEFINEDD_BASE + 0x013 /* 0x3f5b */
#define SPH_SWNDVC_PowerIndex                     DP2_UNDEFINEDD_BASE + 0x014 /* 0x3f5c */

#define SHERIF_SIZE                               1373


/* The statement checks whether generates SHERIF size exceeds limitation */
#define MT2523_SHERIF_SIZE (1376)
#if (SHERIF_SIZE > MT2523_SHERIF_SIZE)
¡§SHERIF SIZE exceeds limitation !! This line makes assembler stop¡¨
#endif /* #if (SHERIF_SIZE > MT2523_SHERIF_SIZE) */

#undef SHERIF_SIZE
#define SHERIF_SIZE (MT2523_SHERIF_SIZE)

/* Macro for SHERIF to DM */
#define IO2DM    dm
#define io2dm    dm

/* Sherif marco end */
#endif /* #ifndef __HAL_AUDIO_FW_DPRAM_H__ */
