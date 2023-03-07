/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2021
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
#ifndef __HAL_DEVAPC_INTERNAL_H__
#define __HAL_DEVAPC_INTERNAL_H__

#include <mt7933.h>
#include <hal_devapc.h>

/********************************
 * CONST
 *******************************/
#define MOD_NO_IN_1_APC        16
#define MOD_NO_IN_1_DOM         4
#define MOD_NO_IN_1_LOCK       32


enum devapc_ao_reg_off {
    AO_APC_CON      = 0x0F00,
    AO_MAS_DOM_0    = 0x0900,
    AO_MAS_SEC_0    = 0x0A00,
    AO_D0_APC_0     = 0x0,
    AO_APC_LOCK_0   = 0x0700,
};

enum devapc_pd_reg_off {
    PD_VIO_DBG_0      = 0x0900,
    PD_VIO_DBG_1      = 0x0904,
    PD_VIO_DBG_2      = 0x0908,
    PD_PDN_APC_CON    = 0x0F00,
    PD_VIO_SHIFT_CON  = 0x0F10,
    PD_VIO_SHIFT_STA  = 0x0F20,
    PD_VIO_SHIFT_SEL  = 0x0F30,
    PD_D0_VIO_MASK_0  = 0x0,
    PD_D0_VIO_STA_0   = 0x0400,
};

/********************************
 * REGISTER ADDRESS
 *******************************/

//Use remapping address for SAU
#define DEVAPC_INFRA_BASE              (0xC4406000)
#define DEVAPC_INFRA_AO_BASE           (0xC0300000)
#define DEVAPC_AUD_BASE                (0xD1009000)
#define DEVAPC_AUD_AO_BASE             (0xD1004000)



/********************************
 * STATIC MACROS
 *******************************/
#define P_DAPC(type)                (&g_devapc_info[(type)])
#define IRQ_NUM_TO_P_DAPC(irq)      (irq == DEVAPC_INFRA_AON_SECURE_VIO_IRQn ? \
                                     P_DAPC(HAL_DEVAPC_TYPE_INFRA) : \
                                     P_DAPC(HAL_DEVAPC_TYPE_AUD))
#define DAPC_NO(p_dapc) \
        ((p_dapc) == g_devapc_info ? HAL_DEVAPC_TYPE_INFRA : HAL_DEVAPC_TYPE_AUD)

#define GEN_MASK(h, l) \
        ((0xFFFFFFFFU - (0x1U << (l)) + 1) & (0xFFFFFFFFU >> (32 - 1 - (h))))


/********************************
 * STRUCTURE
 *******************************/
struct devapc_vio_data {
    unsigned int vio_addr_high;
    unsigned int r_vio;
    unsigned int w_vio;
    unsigned int domain_id;

    unsigned int vio_id;

    unsigned int vio_addr;
};

struct DEVAPC_INFO {
    const char *name;

    unsigned int master_dom_num;
    unsigned int master_sec_num;
    unsigned int apc_device_num;

    unsigned int ao_base;
    unsigned int pd_base;

    unsigned int vio_device_num;

    IRQn_Type irq_num;
};

static struct DEVAPC_INFO g_devapc_info[HAL_DEVAPC_TYPE_MAX] = {
    {
        .name = "INFRA",
        .master_dom_num = 10,
        .master_sec_num = 0,
        .apc_device_num = INFRA_DAPC_MODULE_MAX,

        .ao_base = DEVAPC_INFRA_AO_BASE,
        .pd_base = DEVAPC_INFRA_BASE,

        .vio_device_num = 164,
        .irq_num = DEVAPC_INFRA_AON_SECURE_VIO_IRQn,
    },
    {
        .name = "AUD",
        .master_dom_num = 1,
        .master_sec_num = 2,
        .apc_device_num = AUD_DAPC_MODULE_MAX,

        .ao_base = DEVAPC_AUD_AO_BASE,
        .pd_base = DEVAPC_AUD_BASE,

        .vio_device_num = 55,
        .irq_num = DEVAPC_AUD_BUS_SECURE_VIO_IRQn,
    }
};


#endif /* #ifndef __HAL_DEVAPC_INTERNAL_H__ */
