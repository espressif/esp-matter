/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Author: Min Guo <min.guo@mediatek.com>
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef __HAL_USB_XHCI_RSCS_H
#define __HAL_USB_XHCI_RSCS_H

#include <usb_phy.h>
#include <ssusb_hw_regs.h>
#include <xhci_mtk.h>

/*
  * XHCI_INSTANCE: indicate HW IP number
  * usb resource
  */
#define XHCI_INSTANCE 1
#define XHCI_U2_PHY_NUM 1
#define XHCI_U3_PHY_NUM 0
#define XHCI_PHY_TYPE MTK_TPHY
#define XHIC_INT_ID 48

#define VBUS_NUM 1
#define PORT0_VBUS_GPIO 31

/*
  * xhci power domain
  */
#define XHCI_USB_POWER_DOMAIN

/*
  * xhci clk resource
  */
#define XHCI_REF_CLK
#define XHCI_SYS_CLK
#define XHCI_CLK CLK
#define XHCI_MCU_CLK
#define XHCI_DMA_CLK

/*
  * xhci ip sleep setting
  */
#define PERI_SSUSB_SPM_GLUE_CG 0x300d0700
#define RG_SSUSB_SPM_INT_EN BIT(1)
#define RG_SSUSB_IP_SLEEP_EN BIT(4)
#define SSUSB_SPM_IRQ 92

static struct u2phy_banks xhci_u2_banks[XHCI_U2_PHY_NUM] = {
    {
        .misc = (void *)SSUSB_SIFSLV_U2PORT0_MISC_BASE,
        .fmreg = (void *)SSUSB_SIFSLV_U2PORT0_U2FREQ_BASE,
        .com = (void *)SSUSB_SIFSLV_U2PORT0_COM_BASE,
    },
};

static struct u2phy_banks *u2_banks[XHCI_U2_PHY_NUM] = {
    &(xhci_u2_banks[0]),
};

static u32 xhci_vbus[VBUS_NUM] = {
    PORT0_VBUS_GPIO,
};

static u32 *vbus_gpio[VBUS_NUM] = {
    &(xhci_vbus[0]),
};

static struct xhci_hcd_mtk mtk_hcd[XHCI_INSTANCE] = {
    {
        .xhci_base = (void *)(USB_BASE),
        .ippc_base = (void *)SSUSB_SIFSLV_IPPC_BASE,
        .xhci_irq = XHIC_INT_ID,
        .vbus_gpio = (u32 **) &vbus_gpio,
        /* phy resource */
        .phy.u2_phy_num = XHCI_U2_PHY_NUM,
        .phy.u3_phy_num = XHCI_U3_PHY_NUM,
        .phy.type = XHCI_PHY_TYPE,
        .phy.u2_banks = (struct u2phy_banks **) &u2_banks,
    },
};

#endif /* #ifndef __HAL_USB_XHCI_RSCS_H */

