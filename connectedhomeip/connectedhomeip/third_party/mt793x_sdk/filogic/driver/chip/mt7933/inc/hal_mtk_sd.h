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

#ifndef __HAL_MTK_SD_H__
#define __HAL_MTK_SD_H__

#ifdef HAL_SD_MODULE_ENABLED

#include "hal_sd_define.h"

typedef enum {
    CSD_VERSION_1_0 = 0,
    CSD_VERSION_1_1 = 0,
    CSD_VERSION_2_0 = 1,
    CSD_VERSION_EXT
} csd_version_t;

typedef enum {
    SD_SPECIFICATION_101,
    SD_SPECIFICATION_110,
    SD_SPECIFICATION_200
} sd_specification_t;

typedef struct {
    uint8_t dat_after_erase;
    uint8_t security;
    uint8_t bus_width;
    sd_specification_t spec_ver;
} scr_information_t;

typedef enum {
    IDLE_STA,
    READY_STA,
    IDENT_STA,
    STBY_STA,
    TRAN_STA,
    DATA_STA,
    RCV_STA,
    PRG_STA,
    DIS_STA,
    INV_STA
} sd_state_t;

typedef enum {
    MMC_HS_26M = 1,
    MMC_HS_52M = 2
} mmc_high_speed_type_t;

typedef enum {
    MMC_CLOCK_50M = 50000,    /*50MHZ*/
    MMC_CLOCK_25M = 25000     /*25MHZ*/
} mmc_clock_t;

typedef enum {
    SD_COMMAND8_RESPONSE_NO_RESPONSE,        /* before SD2.0 version or MMC*/
    SD_COMMAND8_RESPONSE_INVALID,            /* SD 2.0 or higher compliant but voltage condition is not allow*/
    SD_COMMAND8_RESPONSE_VALID               /* SD 2.0 or higher compliant*/
} sd_command8_response_t;



typedef struct {
    uint8_t mmc44_reserved0[136];    /*[135:0]*/
    uint32_t enh_start_addr;         /*[139:136]*/
    uint8_t enh_size_mult[3];        /*[142:140]*/
    uint8_t gp_size_mult[12];        /*[154:143]*/
    uint8_t partition_settig;        /*[155]*/
    uint8_t partition_attr;          /*[156]*/
    uint8_t max_enh_size_mult[3];    /*[159:157]*/
    uint8_t partition_support;       /*[160]*/
    uint8_t mmc44_reserved1;         /*[161]*/
    uint8_t rst_function;            /*[162]*/
    uint8_t mmc44_rev2[5];           /*[167:163]*/
    uint8_t rpmb_size_mul;           /*[168]*/
    uint8_t fw_config;               /*[169]*/
    uint8_t mmc44_reserved3;         /*[170]*/
    uint8_t user_wp;                 /*[171]*/
    uint8_t mmc44_reserved4;         /*[172]*/
    uint8_t boot_wp;                 /*[173]*/
    uint8_t mmc44_reserved5;         /*[174]*/
    uint8_t erase_grp_def;           /*[175]*/
    uint8_t mmc44_reserved6;         /*[176]*/
    uint8_t boot_bus_width;          /*[177]*/
    uint8_t boot_config_prot;        /*[178]*/
    uint8_t partition_config;        /*[179]*/
    uint8_t mmc44_reserved7;         /*[180]*/
    uint8_t erased_mem_cont;         /*[181]*/
    uint8_t mmc44_reserved8;         /*[182]*/
    uint8_t bus_width;               /*[183]*/
    uint8_t reserved2;               /*[184]*/
    uint8_t high_speed;              /*[185]*/
    uint8_t reserved3;               /*[186]*/
    uint8_t power_class;             /*[187]*/
    uint8_t reserved4;               /*[188]*/
    uint8_t cmd_set_rev;             /*[189]*/
    uint8_t reserved5;               /*[190]*/
    uint8_t cmd_set;                 /*[191]*/
    uint8_t ext_csd_rev;             /*[192]*/
    uint8_t reserved7;               /*[193]*/
    uint8_t csd_structure;           /*[194]*/
    uint8_t reserved8;               /*[195]*/
    uint8_t card_type;               /*[196]*/
    uint8_t reserved9;               /*[197]*/
    uint8_t out_of_interrupt_time;   /*[198]*/
    uint8_t partition_switch_time;   /*[199]*/
    uint8_t pwr_52_195;              /*[200]*/
    uint8_t pwr_26_195;              /*[201]*/
    uint8_t pwr_52_360;              /*[202]*/
    uint8_t pwr_26_360;              /*[203]*/
    uint8_t reserved10;              /*[204]*/
    uint8_t min_perf_r_4_26;         /*[205]*/
    uint8_t min_perf_w_4_26;         /*[206]*/
    uint8_t min_perf_r_8_26_4_52;    /*[207]*/
    uint8_t min_perf_w_8_26_4_52;    /*[208]*/
    uint8_t min_perf_r_8_52;         /*[209]*/
    uint8_t min_perf_w_8_52;         /*[210]*/
    uint8_t reserved11;              /*[211]*/
    uint32_t sec_count;              /*[212], newly defined in MMC42*/
    uint8_t mmc44_reserved9[5];      /*[220:216]*/
    uint8_t hc_wp_grp_size;          /*221*/
    uint8_t rel_wr_sec_c;            /*222*/
    uint8_t erase_timeout_mult;      /*223*/
    uint8_t hc_erase_grp_size;       /*224*/
    uint8_t acc_size;                /*225*/
    uint8_t boot_size_mul;           /*[226]*/
    uint8_t mmc44_reserved10;        /*[227]*/
    uint8_t boot_info;               /*[228]*/
    uint8_t mmc44_reserved11[275];   /*[503:229]*/
    uint8_t s_cmd_set;
    uint8_t reserved13[7];
} extended_csd_information_t;


/* Card Specific Data(CSD) register structure */
typedef struct {
    csd_version_t     csd_structure;  /* the version of CSD structure */
    uint8_t   tacc;                   /* read access time-1 */
    uint8_t   nsac;                   /* read access time-2 */
    uint8_t   tran_speed;             /* max. data transfer rate */
    uint16_t  ccc;                    /* card command classes */
    uint32_t  write_bl_len;           /* max write data block length */
    uint32_t  read_bl_len;            /* max read data block length */
    uint8_t   write_blk_misalign;     /* write block misalighment */
    uint8_t   read_blk_misalign;      /* read block misalighment */
    uint8_t   write_bl_partial;       /* partial blocks for write allowed */
    uint8_t   read_bl_partial;        /* partial blocks for write allowed */
    uint8_t   dsr_imp;                /* DSR implemented*/
    uint32_t  c_size;                 /* device size*/
    uint8_t   erase_blk_en;           /* erase single block enable*/
    uint32_t  sector_size;            /* erase sector size */
    uint32_t  wp_grg_size;            /* write protect block size*/
    uint8_t   wp_grp_enable;          /* write protect group enable */
    uint8_t   tmp_write_protect;      /* temporary write protection  */
    uint8_t   perm_write_protect;     /* permanent write protection  */
    uint32_t  wp_grp_size_mmc;        /* write protect group size */
    uint32_t  erase_sec_size_mmc;     /* erase sector size */
    uint32_t  erase_grp_size_mmc;     /* erase group size */
    uint8_t   spec_vers;              /* defne the MMC system spec. */
    extended_csd_information_t *ext_csd;   /* pointer to EXT_CSD */
    uint64_t  capacity;               /* capacity in bytes */
} csd_information_t;


typedef struct {
    uint32_t                sd_ocr;                  /* OCR register*/
    uint32_t                block_length;            /* block length*/
    uint32_t                block_number;            /* lock numbers*/
    csd_information_t       csd;                     /* card specific data*/
    uint32_t                cid[4];                  /* card idendtification*/
    scr_information_t       scr;                     /* only for SD*/
    uint32_t                rca;                     /* store relative card address*/
    sd_state_t              sd_state;                /* indicate the current state*/
    bool                    is_inactive;             /* indicate whether card in inactive state*/
    uint8_t                 bus_width;               /* indicate 4-bits data line enable*/
    bool                    is_write_protection;     /* indicate write protection is enabled(SD)*/
    bool                    is_locked;               /* indicate whether the card is locked by switch*/
    sd_command8_response_t  command8_response;       /* receive response after COMMAND8 (SD2.0 or higher)*/
    int32_t                 error_status;
    hal_sd_card_type_t      card_type;
    mmc_information_t       emmc_information;
    volatile bool           is_initialized;
    volatile bool           is_busy;
    volatile bool           is_high_speed;
} sd_information_t;

typedef struct {
    uint16_t max_current;
    uint16_t group1_info;
    uint16_t group1_status;
    uint8_t  group1_result;
    uint8_t  structure_version;
} sd_command6_switch_status_t;

#ifdef HAL_SLEEP_MANAGER_ENABLED
typedef struct {
    hal_sd_config_t config;
    hal_sd_port_t   sd_port;
    bool            is_initialized;
} sd_backup_parameter_t;
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */

/*using for DMA interrupt transfer context save*/
typedef struct {
    uint32_t sd_current_write_read_block_num;
} sd_dma_interrupt_context_t;
#define SD_IS_R1B_BUSY  (msdc_register_base->SDC_STA & SDC_STA_FEDATBUSY_MASK)


mtk_sd_status_t mmc_get_extended_csd(void);
mtk_sd_status_t mmc_switch(uint32_t argument);
mtk_sd_status_t sd_wait_data_ready(void);
mtk_sd_status_t sd_wait_command_ready(void);
mtk_sd_status_t sd_send_command(uint32_t msdc_command, uint32_t argument);
mtk_sd_status_t sd_send_command55(uint32_t argument);
mtk_sd_status_t sd_reset(void);
hal_sd_card_type_t sd_check_sd_card_type(void);
hal_sd_card_type_t sd_check_mmc_card_type(void);
hal_sd_card_type_t sd_check_card_type(void);
mtk_sd_status_t sd_get_card_id(void);
mtk_sd_status_t sd_get_rca(void);
void sd_get_bit_field_value(uint8_t *destination, uint8_t *source, uint16_t start_bit, uint16_t width);
void sd_analysis_csd(uint32_t *csd);
mtk_sd_status_t sd_get_csd(void);
mtk_sd_status_t sd_set_dsr(void);
mtk_sd_status_t sd_wait_card_not_busy(void);
mtk_sd_status_t sd_check_card_status(void);
mtk_sd_status_t sd_select_card(uint32_t rca);
mtk_sd_status_t sd_set_block_length(uint32_t block_length);
mtk_sd_status_t sd_analysis_scr(uint32_t *scr);
mtk_sd_status_t sd_acommand42(void);
void sd_analysis_command6_switch_status(sd_command6_switch_status_t *command6_switch_status, uint8_t *buffer);
mtk_sd_status_t sd_get_scr(void);
mtk_sd_status_t sd_query_switch_high_speed(uint32_t argument);
mtk_sd_status_t sd_select_high_speed(void);
void mmc_is_version_44(void);
mtk_sd_status_t mmc_set_high_speed(void);
mtk_sd_status_t sd_stop_transfer(void);
mtk_sd_status_t sd_get_card_status(uint32_t *card_status);
mtk_sd_status_t sd_send_erase_command(uint32_t command, uint32_t address);
bool sd_output_clock_tuning(uint32_t clock);
mtk_sd_status_t sd_wait_data_ready_dma(void);
void sd_save_dma_interrupt_context(const sd_dma_interrupt_context_t *context);
hal_sd_status_t sd_wait_dma_interrupt_transfer_ready(void);

#ifdef HAL_SLEEP_MANAGER_ENABLED
void sd_backup_all(void *data);
void sd_restore_all(void *data);
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */

#endif /* #ifdef HAL_SD_MODULE_ENABLED */
#endif /* #ifndef __HAL_MTK_SD_H__ */

