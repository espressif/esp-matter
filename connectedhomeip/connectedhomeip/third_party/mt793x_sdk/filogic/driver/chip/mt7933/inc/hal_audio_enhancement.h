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

#ifndef __HAL_SPE_H__
#define __HAL_SPE_H__

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include "hal_audio_internal_service.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"
#include "hal_audio_internal_afe.h"
#include "hal_gpt.h"
#include <assert.h>
#include <string.h>

#define NUMBER_COMMON_PARAS  12
#define NUMBER_MODE_PARAS    48
#define NUMBER_VOL_PARAS     4
#define NUMBER_FIR_NB        45
#define NUMBER_FIR_WB        90

#define FLAG_EC_SUPP         1
#define FLAG_DDL_UL_ENTRY    2
#define FLAG_DDL_DL_ENTRY    4

#define SPE_IDLE_STATE       0  /* MCU: 0 -> 1 */
#define SPE_READY_STATE      1  /* DSP: 1 -> 2 */
#define SPE_WORKING_STATE    2  /* MCU: 2 -> 3 */
#define SPE_STOP_STATE       3  /* DSP: 3 -> 0 */

#define SPE_AEC_FLAG        (1 << 1)
#define SPE_UL_NR_FLAG      (1 << 2)
#define SPE_DL_NR_FLAG      (1 << 3)
#define SPE_AGC_FLAG        (1 << 4)
#define NUM_OF_SPH_FLAG     5

#define SPH_ENH_MASK_AEC      SPE_AEC_FLAG
#define SPH_ENH_MASK_ULNR     SPE_UL_NR_FLAG
#define SPH_ENH_MASK_DLNR     SPE_DL_NR_FLAG
#define SPH_ENH_MASK_AGC      SPE_AGC_FLAG
#define SPH_ENH_MASK_SIDETONE (1 << NUM_OF_SPH_FLAG)

#define SPH_FLT_PAGE        3
#define SPH_EMP_PAGE        6

#define VOICE_INPUT_FILTER_FLAG     0x1
#define VOICE_OUTPUT_FILTER_FLAG    0x2
#define VOICE_16K_SWITCH_FLAG       0x10
#define VOICE_MIC_MUTE_FLAG         0x20
#define VOICE_SIDETONE_FILTER_FLAG  0

#define SPH_FLT_COEF_LEN_SIDETONE  31
#define SPH_FLT_COEF_LEN_BKF_NB    45
#define SPH_FLT_COEF_LEN_BKF_WB    90

#define _spe_set_state_( _CTRL_REGISTER, STATE, MASK )  \
   _CTRL_REGISTER = ((_CTRL_REGISTER & MASK) | STATE);

//when mask = 0xFFFF, means _CTRL_REGISTER == STATE
#define _spe_wait_state_change_( _CTRL_REGISTER, STATE, MASK )  \
{  \
   uint32_t I;  \
   for( I = 0; ; I++ ) {  \
      if( (_CTRL_REGISTER & MASK) == STATE )  \
         break;  \
      assert( I < 20 );  \
      hal_gpt_delay_ms(9);  \
   }  \
}

typedef enum {
    SPH_SPE_COMMON_PARAMETER,   /**< SPE common parameter */
    SPH_SPE_MODE_PARAMETER,     /**< SPE mode parameter */
    SPH_SPE_MODE_PARAMETER_WB,     /**< SPE mode parameter */
    SPH_SPE_VOL_PARAMETER,      /**< SPE volume parameter */
} hal_speech_enhancement_parameter_t;

typedef enum {
    SPH_SPE_DSP_FILTER_SIDETONE,
    SPH_SPE_DSP_FILTER_BKF_NB_UL,
    SPH_SPE_DSP_FILTER_BKF_NB_DL,
    SPH_SPE_DSP_FILTER_BKF_WB_UL,
    SPH_SPE_DSP_FILTER_BKF_WB_DL,
    SPH_SPE_DSP_FILTER_SRC,
    SPH_SPE_DSP_FILTER_AGC
} hal_speech_filter_type_t;

typedef enum {
    SPH_MODE_EARPHONE,
    SPH_MODE_LOUDSPEAKER,
    SPH_MODE_BT_EARPHONE,
    SPH_MODE_BT_LOUDSPEAKER,
    SPH_MODE_RECORD,
    SPH_MODE_UNDEFINED
} hal_speech_device_mode_t;

typedef struct speech_enh {
    bool     spe_flag;         /*<speech enhaacement flag>*/
    uint8_t  spe_state;
    uint8_t  speech_band;
    uint16_t spe_app_mask;
    uint16_t sph_common_para[NUMBER_COMMON_PARAS];
    uint16_t sph_mode_para[NUMBER_MODE_PARAS];
    uint16_t sph_mode_para_wb[NUMBER_MODE_PARAS];
    uint16_t sph_vol_para[NUMBER_VOL_PARAS];
    int16_t  sph_in_fir[NUMBER_FIR_NB];
    int16_t  sph_out_fir[NUMBER_FIR_NB];
    int16_t  sph_in_fir_wb[NUMBER_FIR_WB];
    int16_t  sph_out_fir_wb[NUMBER_FIR_WB];
    uint16_t spe_audio_id;
    hal_speech_device_mode_t  speech_mode;
} hal_audio_spe_struct_t;

void speech_set_sidetone_volume(uint32_t sidetone_volume);
void speech_set_mode(hal_speech_device_mode_t mode);
void speech_set_enhancement(bool enable);
void speech_update_common(const uint16_t *common);
void speech_update_nb_param(const uint16_t *param);
void speech_update_wb_param(const uint16_t *param);
void speech_update_nb_fir(const int16_t *in_coeff, const int16_t *out_coeff);
void speech_update_wb_fir(const int16_t *in_coeff, const int16_t *out_coeff);
void speech_init(void);
void speech_reload_filter_coefficient(void);


//NDVC usage
uint16_t spe_ndvc_uplink_noise_index_map(uint16_t db);
void spe_ndvc_noise_setting(uint16_t index);

//Query function
void spe_dump_common_param(void);
void spe_dump_mode_param(void);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef __HAL_SPE_H__ */
