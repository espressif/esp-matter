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

#ifndef __HAL_AUDIO_INTERNAL_AFE_H__
#define __HAL_AUDIO_INTERNAL_AFE_H__

#include "hal_platform.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#include <stdbool.h>
#include <stdio.h>
#include "hal_audio.h"
#include "hal_audio_custom.h"
#include "hal_gpt.h"
#include "hal_gpio.h"
#include "hal_audio_internal_pcm2way.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_internal_service.h"

#if defined(HAL_I2S_MODULE_ENABLED)
#include "hal_audio_ext.h"
#endif /* #if defined(HAL_I2S_MODULE_ENABLED) */
#include "hal_log.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

typedef volatile unsigned short *APBADDR;
typedef volatile uint32_t *APBADDR32;

/*MD2G*/
#define  MD2GSYS_CG_SET2    ((APBADDR)(MD2GCONFG_BASE+0x018))
#define  MD2GSYS_CG_CLR2    ((APBADDR)(MD2GCONFG_BASE+0x028))

/*AUDAC_26M*/
#define  HW_MISC        ((APBADDR)(CONFIG_BASE+0x022C))

/*Digital AFE*/
#define  AFE_COSIM_RG   ((APBADDR)(AFE_BASE+0x0200))
#define  AFE_DBG_SIG    ((APBADDR)(AFE_BASE+0x01F0))
#define  AFE_MCU_CON0   ((APBADDR)(AFE_BASE+0x0210))
#define  AFE_MCU_CON1   ((APBADDR)(AFE_BASE+0x0214))
#define  AFE_VMCU_CON0  ((APBADDR)(AFE_BASE+0x0000))
#define  AFE_VMCU_CON1  ((APBADDR)(AFE_BASE+0x000C))
#define  AFE_VMCU_CON2  ((APBADDR)(AFE_BASE+0x0010))
#define  AFE_VMCU_CON3  ((APBADDR)(AFE_BASE+0x001C))
#define  AFE_VMCU_CON4  ((APBADDR)(AFE_BASE+0x01A0))
#define  AFE_VMCU_CON6  ((APBADDR)(AFE_BASE+0x01A8))
#define  AFE_VDB_CON    ((APBADDR)(AFE_BASE+0x0014))
#define  AFE_VLB_CON    ((APBADDR)(AFE_BASE+0x0018))
#define  AFE_AMCU_CON0  ((APBADDR)(AFE_BASE+0x0020))
#define  AFE_AMCU_CON1  ((APBADDR)(AFE_BASE+0x0024))
#define  AFE_AMCU_CON2  ((APBADDR)(AFE_BASE+0x002C))
#define  AFE_AMCU_CON3  ((APBADDR)(AFE_BASE+0x0038))
#define  AFE_AMCU_CON4  ((APBADDR)(AFE_BASE+0x003C))
#define  AFE_AMCU_CON5  ((APBADDR)(AFE_BASE+0x0180))
#define  AFE_AMCU_CON6  ((APBADDR)(AFE_BASE+0x0184))
#define  AFE_AMCU_CON7  ((APBADDR)(AFE_BASE+0x0188))
#define  AFE_EDI_CON    ((APBADDR)(AFE_BASE+0x0028))
#define  AFE_DAC_TEST   ((APBADDR)(AFE_BASE+0x0030))
#define  AFE_VAM_SET    ((APBADDR)(AFE_BASE+0x0034))
#define  AFE_CMPR_CNTR  ((APBADDR)(AFE_BASE+0x01CC))
#define  AFE_VMCU_CON5  ((APBADDR)(AFE_BASE+0x01AC))
#define  AFE_FOC_TX_CON0 ((APBADDR)(AFE_BASE+0x0310))
#define  AFE_FOC_RX_CON0 ((APBADDR)(AFE_BASE+0x0330))
#define  AFE_SLV_I2S_CON ((APBADDR32)(AFE_BASE+0x0300))


/*Analog AFE*/
#define  ABBA_TOP_CON0      ((APBADDR)(ABBSYS_SD_BASE+0x0028))
#define  ABBA_VBITX_CON0    ((APBADDR)(ABBSYS_SD_BASE+0x0050))
#define  ABBA_VBITX_CON1    ((APBADDR)(ABBSYS_SD_BASE+0x0054))
#define  ABBA_VBITX_CON2    ((APBADDR)(ABBSYS_SD_BASE+0x0058))
#define  ABBA_VBITX_CON3    ((APBADDR)(ABBSYS_SD_BASE+0x005C))
#define  ABBA_VBITX_CON4    ((APBADDR)(ABBSYS_SD_BASE+0x0060))
#define  ABBA_VBITX_CON5    ((APBADDR)(ABBSYS_SD_BASE+0x0064))
#define  ABBA_VBITX_CON7    ((APBADDR)(ABBSYS_SD_BASE+0x006C))
#define  ABBA_AUDIODL_CON0  ((APBADDR)(ABBSYS_SD_BASE+0x0080))
#define  ABBA_AUDIODL_CON1  ((APBADDR)(ABBSYS_SD_BASE+0x0084))
#define  ABBA_AUDIODL_CON2  ((APBADDR)(ABBSYS_SD_BASE+0x0088))
#define  ABBA_AUDIODL_CON3  ((APBADDR)(ABBSYS_SD_BASE+0x008C))
#define  ABBA_AUDIODL_CON4  ((APBADDR)(ABBSYS_SD_BASE+0x0090))
#define  ABBA_AUDIODL_CON5  ((APBADDR)(ABBSYS_SD_BASE+0x0094))
#define  ABBA_AUDIODL_CON6  ((APBADDR)(ABBSYS_SD_BASE+0x0098))
#define  ABBA_AUDIODL_CON7  ((APBADDR)(ABBSYS_SD_BASE+0x009C))
#define  ABBA_AUDIODL_CON8  ((APBADDR)(ABBSYS_SD_BASE+0x00A0))
#define  ABBA_AUDIODL_CON9  ((APBADDR)(ABBSYS_SD_BASE+0x00A4))
#define  ABBA_AUDIODL_CON10 ((APBADDR)(ABBSYS_SD_BASE+0x00A8))
#define  ABBA_AUDIODL_CON11 ((APBADDR)(ABBSYS_SD_BASE+0x00AC))
#define  ABBA_AUDIODL_CON12 ((APBADDR)(ABBSYS_SD_BASE+0x00B0))
#define  ABBA_AUDIODL_CON13 ((APBADDR)(ABBSYS_SD_BASE+0x00B4))
#define  ABBA_AUDIODL_CON14 ((APBADDR)(ABBSYS_SD_BASE+0x00B8))
#define  ABBA_AUDIODL_CON15 ((APBADDR)(ABBSYS_SD_BASE+0x00BC))
#define  ABBA_AUDIODL_CON16 ((APBADDR)(ABBSYS_SD_BASE+0x00C0))
#define  ABBA_AUDIODL_CON17 ((APBADDR)(ABBSYS_SD_BASE+0x00C4))
#define  ABBA_AUDIODL_CON18 ((APBADDR)(ABBSYS_SD_BASE+0x00C8))

/*AuxADC*/
#define  AUXADC_CON1 ((APBADDR)(AUXADC_BASE+0x0004))
#define  AUXADC_CON3 ((APBADDR)(AUXADC_BASE+0x000C))
#define  AUXADC_DAT7 ((APBADDR)(AUXADC_BASE+0x002C))
#define  AUXADC_DAT8 ((APBADDR)(AUXADC_BASE+0x0030))
#define  PDN_CON2_VAFE  0x0100
//ABBA_VBITX_CON0
#define RG_VPGA_DC_PRECHARGE       (0x1 << 3)
#define RG_VCFG_1                  (0x1 << 10)
#define RG_VPWDB_PGA               (0x1 << 5)
//ABBA_VBITX_CON1
#define RG_VPWDB_ADC               (0x1 << 6)
#define RG_VREF24_EN               (0x1 << 2)
#define RG_VCM14_EN                (0x1 << 1)
//ABBA_VBITX_CON4
#define RG_VPWDB_MBIAS             (0x1 << 1)
//ABBA_VBITX_CON7
#define RG_AUDDIGMICEN             (0x1 << 15)
//ABBA_AUDIODL_CON0
#define RG_AUDIBIASPWRUP           (0x1)
#define RG_AUDHPRPWRUP             (0x1 << 2)
#define RG_AUDHPLPWRUP             (0x1 << 3)
#define RG_AUDHSPWRUP              (0x1 << 4)
#define RG_AUDDACRPWRUP            (0x1 << 5)
#define RG_AUDDACLPWRUP            (0x1 << 6)
//ABBA_AUDIODL_CON4
#define RG_DEPOP_VCM_EN            (0x1)
#define RG_DEPOP_CHARGEOPTION      (0x1 << 5)
#define RG_ADEPOP_EN               (0x1 << 6)
//ABBA_AUDIODL_CON10
#define RG_AUDHSBIAS               (0x1 << 12)
//ABBA_AUDIODL_CON12
#define audzcdenable               (0x1)
//ABBA_AUDIODL_CON16
#define RG_AMUTER                  (0x1 << 1)
#define RG_AMUTEL                  (0x1)
//VSBST_CON0
#define QI_VSBST_EN                (0x1)
#define RG_VSBST_BP                (0x1 << 1)
#define NI_VSBST_SSBP              (0x1 << 5)
#define VSBST_VIO28_PG_STATUS      (0x1 << 10)
//VSBST_CON4
#define QI_VSBST_PG_STATUS         (0x1 << 9)

#define MINIMUM_AGAIN_SETTING 0xE
#define MINIMUM_VGAIN_SETTING 0x8 //0x1E, change to 0dB for most usage case
#define MIC_SETTING_MASK      0xFDFF
#define MIC_SETTING_HEADSET   0x0200
#define MIC_SETTING_NORMAL    0x0000

#define STREAM_OUT_PATH        0
#define STREAM_IN_PATH         1
#define STREAM_IN_OUT_PATH     2

#define DC_CALI_BUF_LEN      120
#define HP_CALI_SWITCH         2
#define HP_CHARGE_DELAY      600
#define HP_CALI_TIMES         10  //Currently, HS CALI using the same value
#define HP_CALI_CH             2  //HS CALI using the same value (P/N)

typedef struct {
    uint16_t     VMCU_CON0;
    uint16_t     VDB_CON;
    uint16_t     VLB_CON;
    uint16_t     AMCU_CON0;
    uint16_t     AMCU_CON1;
    uint16_t     EDI_CON;
    uint16_t     AMCU_CON2;
    uint16_t     DAC_TEST;
    uint16_t     VMCU_CON1;
    uint16_t     VMCU_CON2;
    uint16_t     MCU_CON0;
    uint16_t     MCU_CON1;
} register_backup_t;

typedef struct {
    uint8_t    calibration_sta;  /*0:not yet 1:done */
    uint16_t   hs_dc_compensate_value;
    uint16_t   hp_dc_compensate_value_r;
    uint16_t   hp_dc_compensate_value_l;
} afe_dc_calibration_t;

typedef struct {
    bool                 init;
    bool                 audio_clock_enable;
    bool                 voice_clock_enable;
    bool                 audio_tx_enable;
    bool                 loopback;
    bool                 pcmif_loopback;
    bool                 stream_out_mute;
    bool                 stream_in_mute;
    bool                 mic_bias;
    bool                 sidetone_flag;
    bool                 headphone_on;
    bool                 handset_on;
    bool                 hp_hs_both_on;
    bool                 line_in_enable;
    bool                 ext_pa_on;
    bool                 edi_on;
    uint32_t             audio_fs;
    uint32_t             sidetone_volume;
    uint32_t             stream_out_analog_gain_index;
    uint32_t             stream_in_analog_gain_index;
    uint32_t             stream_out_digital_gain_index;
    uint32_t             stream_in_digital_gain_index;
    uint32_t             stream_out_analog_gain_in_01unit_db;   // 150(dex) means 1.5db
    uint32_t             stream_in_analog_gain_in_01unit_db;
    uint32_t             stream_out_digital_gain_in_01unit_db;
    uint32_t             stream_in_digital_gain_in_01unit_db;
    hal_audio_device_t   output_device;
    hal_audio_device_t   input_device;
    afe_dc_calibration_t dc_cali_data;
#if defined(HAL_I2S_MODULE_ENABLED)
    hal_audio_ext_device_t  ext_output_device;
#endif /* #if defined(HAL_I2S_MODULE_ENABLED) */
    hal_audio_active_type_t     audio_path_type;
    register_backup_t    register_backup;
} afe_t;

/*function prototype*/
void afe_voice_digital_on(void);
void afe_voice_digital_off(void);
void afe_audio_digital_on(uint16_t sample_rate);
void afe_audio_digital_off(void);
void afe_switch_mic_bias(bool on);
void afe_switch_tx_signal_path(bool on);
void afe_switch_rx_signal_path(void);
void afe_mic_power_setting(void);
void afe_audio_initial(void);
void afe_audio_deinit(void);
bool afe_is_audio_enable(void);
void afe_set_sidetone(bool enable);
void afe_digital_set_wideband_voice(bool wideband);
void afe_audio_set_output_volume(uint32_t analog_gain_in_01unit_db, uint32_t digital_gain_in_01unit_db);
void afe_audio_set_input_volume(uint32_t analog_gain_in_01unit_db, uint32_t digital_gain_in_01unit_db);
void afe_audio_set_analog_gain(void);
void afe_audio_set_digital_gain(void);
void afe_set_stream_out_mute(bool mute);
void afe_set_stream_in_mute(bool mute);
void afe_set_hardware_mute(bool mute);
void afe_audio_device_volume_update(void);
void afe_set_path_type(hal_audio_active_type_t type);
hal_audio_active_type_t afe_get_path_type(void);
void afe_digital_loopback(bool enable);
void afe_audio_set_output_device(uint16_t device);
void afe_audio_set_input_device(uint16_t device);
void afe_audio_analog_device_off(void);
uint16_t afe_audio_get_output_device(void);
uint16_t afe_audio_get_input_device(void);
void afe_volume_ramp_down(void);
void afe_audio_set_sidetone_volume(uint32_t sidetone_volume);
void afe_audio_update_sidetone_volume(int DL_PGA_Gain, int Sidetone_Volume, uint8_t SW_AGC_Ul_Gain);
void afe_audio_set_digital_mic(bool enable);

#if defined(HAL_I2S_MODULE_ENABLED)
/*I2S interface functions*/
void afe_master_i2s_edi_on(void);
void afe_master_i2s_edi_off(void);
void afe_set_slave_i2s_config(hal_i2s_sample_rate_t sample_rate);
void afe_reset_slave_i2s_config(void);
void afe_slave_i2s_enable(void);
void afe_slave_i2s_disable(void);
void afe_master_i2s_internal_loopback(bool enable);
void afe_audio_set_ext_output_device(hal_audio_ext_device_t device);
hal_audio_ext_device_t afe_audio_get_ext_output_device(void);
void afe_i2s_pinmux_switch(bool i2s_enable);

#ifdef __AFE_I2S_DYNAMIC_SWITCH__
extern const char HAL_I2S_MASTER_EDIDO_PIN;
extern const char HAL_I2S_MASTER_EDIDI_PIN;
extern const char HAL_I2S_MASTER_EDIWS_PIN;
extern const char HAL_I2S_MASTER_EDICK_PIN;
extern const char HAL_I2S_MASTER_EDIDO_PIN_M_MA_EDIDO;
extern const char HAL_I2S_MASTER_EDIDI_PIN_M_MA_EDIDI;
extern const char HAL_I2S_MASTER_EDIWS_PIN_M_MA_EDIWS;
extern const char HAL_I2S_MASTER_EDICK_PIN_M_MA_EDICK;
extern const char HAL_I2S_MASTER_EDIDO_PIN_M_GPIO;
extern const char HAL_I2S_MASTER_EDIDI_PIN_M_GPIO;
extern const char HAL_I2S_MASTER_EDIWS_PIN_M_GPIO;
extern const char HAL_I2S_MASTER_EDICK_PIN_M_GPIO;
#endif /* #ifdef __AFE_I2S_DYNAMIC_SWITCH__ */

#endif /* #if defined(HAL_I2S_MODULE_ENABLED) */

/*PCM interface functions*/
void afe_digital_switch_loopback_pcmif(bool on);
void afe_digital_switch_pcm_clock(bool pcm_on, bool frame_sync, uint16_t pcm_clk_mode);
bool afe_set_and_swtich_master_i2s(bool enable);
void afe_audio_data_from_edi_input(bool enable);
void afe_audio_data_1x_to_edi_output(bool enable);

/*MISC functions*/
void afe_backup_register(void);
void afe_restore_register(void);

#ifdef __AFE_HP_DC_CALIBRATION__
void afe_audio_buffer_dc_calibration(void);
#endif /* #ifdef __AFE_HP_DC_CALIBRATION__ */
#ifdef __AFE_HS_DC_CALIBRATION__
void afe_voice_buffer_dc_calibration(void);
#endif /* #ifdef __AFE_HS_DC_CALIBRATION__ */
afe_dc_calibration_t afe_get_dc_calibration_data(void);
void afe_nvdm_set_data(afe_dc_calibration_t nvdm_dc_calibration);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
#endif /* #ifndef __HAL_AUDIO_INTERNAL_AFE_H__ */
