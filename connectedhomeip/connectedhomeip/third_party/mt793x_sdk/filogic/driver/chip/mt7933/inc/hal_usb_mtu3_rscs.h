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
 * driver for SSUSB controller
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

#ifndef __HAL_USB_MTU3_RSCS_H
#define __HAL_USB_MTU3_RSCS_H

#include <mtu3.h>
#include <usb_phy.h>
#include <ssusb_hw_regs.h>

/*
  * SSUSB_IP: indicate HW IP number
  * usb resource
  */
#define SSUSB_IP 1
#define U2_PHY_NUM 1
#define U3_PHY_NUM 0
#define PHY_TYPE MTK_TPHY
#define VBUS_VALID 33
#define IDDIG_GPIO 34
#define IDDIG_EINT HAL_EINT_NUMBER_26

#define P0_VBUS_GPIO 31
#define SSUSB_DEV_INT_ID 50

/*
  * device power domain
  */
#define SSUSB_POWER_DOMAIN

/*
  * device clk resource
  */
#define SSUSB_REF_CLK
#define SSUSB_SYS_CLK
#define SSUSB_XHCI_CLK
#define SSUSB_MCU_CLK
#define SSUSB_DMA_CLK

static struct u2phy_banks mtu3_u2_banks[U2_PHY_NUM] = {
    {
        .misc = (void *)SSUSB_SIFSLV_U2PORT0_MISC_BASE,
        .fmreg = (void *)SSUSB_SIFSLV_U2PORT0_U2FREQ_BASE,
        .com = (void *)SSUSB_SIFSLV_U2PORT0_COM_BASE,
    },
};

static struct u2phy_banks *u2_banks[U2_PHY_NUM] = {
    &(mtu3_u2_banks[0]),
};

static struct ssusb_mtk ssusb_rscs[SSUSB_IP] = {
    {
        .u3d.mac_base = (void *)(SSUSB_DEV_BASE),
        .u3d.dev_irq = SSUSB_DEV_INT_ID,
        .ippc_base = (void *)SSUSB_SIFSLV_IPPC_BASE,
        .iddig_gpio = IDDIG_GPIO,
        .vbus_gpio = P0_VBUS_GPIO,
        .dr_mode = USB_DR_MODE_OTG,
        /* phy resource */
        .phy.u2_phy_num = U2_PHY_NUM,
        .phy.u3_phy_num = U3_PHY_NUM,
        .phy.type = PHY_TYPE,
        .phy.u2_banks = (struct u2phy_banks **) &u2_banks,
    },
};

#endif /* #ifndef __HAL_USB_MTU3_RSCS_H */

