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

#ifndef __LP_DVT_CLI_H__
#define __LP_DVT_CLI_H__

extern cmd_t   lp_dvt_cmds[];
#define LP_DVT_CLI_ENTRY { "lp_dvt",    "Low Power DVT",  NULL,     lp_dvt_cmds },

extern cmd_t   spm_lp_dvt_cli_cmds[];
#define SPM_LP_DVT_CLI_ENTRY { "spm",           "MTCMOS and HW sleep mgr.",   NULL,     spm_lp_dvt_cli_cmds },
extern cmd_t   clk_lp_dvt_cli_cmds[];
#define CLK_LP_DVT_CLI_ENTRY { "clk",           "clock mux and CG",           NULL,     clk_lp_dvt_cli_cmds },
extern cmd_t   aud_lp_dvt_cli_cmds[];
#define AUD_LP_DVT_CLI_ENTRY { "audio",         "audio phase0-3",             NULL,     aud_lp_dvt_cli_cmds },
extern cmd_t   dsp_lp_dvt_cli_cmds[];
#define DSP_LP_DVT_CLI_ENTRY { "dsp",           "DSP voltage / freq",         NULL,     dsp_lp_dvt_cli_cmds },
#ifdef HAL_PMU_MODULE_ENABLED
extern cmd_t   pmu_lp_dvt_cli_cmds[];
#define PMU_LP_DVT_CLI_ENTRY { "pmu",           "voltage and freq. control",  NULL,     pmu_lp_dvt_cli_cmds },
#else
#define PMU_LP_DVT_CLI_ENTRY
#endif /* HAL_PMU_MODULE_ENABLED */
extern cmd_t   dcm_lp_dvt_cli_cmds[];
#define DCM_LP_DVT_CLI_ENTRY { "dcm",           "bus dynamic clock freq.",    NULL,     dcm_lp_dvt_cli_cmds },
extern cmd_t   bt_lp_dvt_cli_cmds[];
#define BT_LP_DVT_CLI_ENTRY { "bt",            "BT LP",                       NULL,     bt_lp_dvt_cli_cmds },
extern cmd_t   wf_lp_dvt_cli_cmds[];
#define WF_LP_DVT_CLI_ENTRY { "wf",            "WiFi LP",                     NULL,     wf_lp_dvt_cli_cmds },
extern cmd_t   conn_lp_dvt_cli_cmds[];
#define CONN_LP_DVT_CLI_ENTRY { "conn_infra",    "ConnInfra POS and XO trim",  NULL,     conn_lp_dvt_cli_cmds },
#ifdef HAL_RTC_MODULE_ENABLED
extern cmd_t   rtc_lp_dvt_cli_cmds[];
#define RTC_LP_DVT_CLI_ENTRY { "rtc",    "RTC LP",  NULL,     rtc_lp_dvt_cli_cmds },
#else
#define RTC_LP_DVT_CLI_ENTRY
#endif /* HAL_PMU_MODULE_ENABLED */



#define LP_DVT_CLI_CMDS                 SPM_LP_DVT_CLI_ENTRY       \
                                        CLK_LP_DVT_CLI_ENTRY       \
                                        AUD_LP_DVT_CLI_ENTRY       \
                                        DSP_LP_DVT_CLI_ENTRY       \
                                        PMU_LP_DVT_CLI_ENTRY       \
                                        DCM_LP_DVT_CLI_ENTRY       \
                                        BT_LP_DVT_CLI_ENTRY        \
                                        WF_LP_DVT_CLI_ENTRY        \
                                        CONN_LP_DVT_CLI_ENTRY      \
                                        RTC_LP_DVT_CLI_ENTRY

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __LP_DVT_CLI_H__ */

