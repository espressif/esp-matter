/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2020
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#ifndef __HAL_ASIC_MPU_INTERNAL_H__
#define __HAL_ASIC_MPU_INTERNAL_H__

/********************************
 * CONST
 *******************************/
enum {
    CFG_OFFSET = 0,

    RABN_0_OFFSET = 0x10,
    RABN_1_OFFSET = 0x14,
    RABN_2_OFFSET = 0x18,

    WABN_0_OFFSET = 0x20,
    WABN_1_OFFSET = 0x24,
    WABN_2_OFFSET = 0x28,

    R0_APC_OFFSET = 0x30,
    BK_APC_OFFSET = 0x70,
    R0_CFG_OFFSET = 0x80,
};


/********************************
 * REGISTER ADDRESS
 *******************************/
#ifndef MTK_TFM_ENABLE
#define ASIC_MPU_BASE    (0x30000000)
#else /* #ifndef MTK_TFM_ENABLE */
#define ASIC_MPU_BASE    (0xC0000000)
#endif /* #ifndef MTK_TFM_ENABLE */

// ASIC_MPU
enum {
    MPU_IRQ_STA             = ASIC_MPU_BASE + 0x0010,
    MPU_IRQ_EA              = ASIC_MPU_BASE + 0x0014,
    MPU_ERR_STS_CLR         = ASIC_MPU_BASE + 0x0018,

    MPU_ERR_RTRN_VAL        = ASIC_MPU_BASE + 0x0020,
    MPU_ERR_RTRN_VAL_CTL    = ASIC_MPU_BASE + 0x0024,
};

/********************************
 * STATIC MACROS
 *******************************/
#define MPU_OFFSET(mpu_type, offset) \
    (ASIC_MPU_BASE + (0x0100 * (mpu_type + 1)) + offset)

#define GET_BIT(val, bit) \
    (((val) >> (bit)) & 0x1)

/********************************
 * VIO Struct
 *******************************/
struct mpu_unit_vio_info {
    /* 0: R, 1: W */
    int abn_id[2];

    int irq_sta[2];

    int irq_overflow[2];
    int abn_apc[2];
    int abn_prot[2];
    int abn_domain[2];
    int abn_region[2];

    unsigned int abn_addr[2];
};

#endif /* #ifndef __HAL_ASIC_MPU_INTERNAL_H__ */
