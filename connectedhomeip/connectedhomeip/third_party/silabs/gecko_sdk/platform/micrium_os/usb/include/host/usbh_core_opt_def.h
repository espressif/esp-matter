/***************************************************************************//**
 * @file
 * @brief USB Host Option Defines
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CORE_OPT_DEF_H_
#define  _USBH_CORE_OPT_DEF_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FIELD EN MASKS
 *******************************************************************************************************/

#define  USBH_CFG_FIELD_EN_DEV_SPEC_NBR                    DEF_BIT_00
#define  USBH_CFG_FIELD_EN_DEV_SUBCLASS                    DEF_BIT_02
#define  USBH_CFG_FIELD_EN_DEV_PROTOCOL                    DEF_BIT_03
#define  USBH_CFG_FIELD_EN_DEV_VENDOR_ID                   DEF_BIT_04
#define  USBH_CFG_FIELD_EN_DEV_PRODUCT_ID                  DEF_BIT_05
#define  USBH_CFG_FIELD_EN_DEV_REL_NBR                     DEF_BIT_06

#define  USBH_CFG_FIELD_EN_CONFIG_MAX_PWR                  DEF_BIT_07
#define  USBH_CFG_FIELD_EN_CONFIG_ATTR                     DEF_BIT_08

#define  USBH_CFG_FIELD_EN_FNCT_SUBCLASS                   DEF_BIT_09
#define  USBH_CFG_FIELD_EN_FNCT_PROTOCOL                   DEF_BIT_10

#define  USBH_CFG_FIELD_EN_IF_CLASS                        DEF_BIT_11
#define  USBH_CFG_FIELD_EN_IF_SUBCLASS                     DEF_BIT_12
#define  USBH_CFG_FIELD_EN_IF_PROTOCOL                     DEF_BIT_13

#define  USBH_CFG_FIELD_EN_ALL                             DEF_BIT_FIELD(14u, 0u)
#define  USBH_CFG_FIELD_EN_NONE                            DEF_BIT_NONE

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif