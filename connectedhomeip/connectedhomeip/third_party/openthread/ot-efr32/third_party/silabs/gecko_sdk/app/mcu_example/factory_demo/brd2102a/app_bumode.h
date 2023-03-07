/***************************************************************************//**
 * @file
 * @brief Helper functions for using the backup power mode
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APP_BUMODE_H
#define APP_BUMODE_H

#define RET_BODCOUNT    0
#define RET_BODTIME     1
#define RET_CURAPP      2

#include "em_adc.h"

// Function prototypes
void buSetup(uint32_t * rtccResetVal,
             uint32_t * rtccResetTs,
             uint32_t * bodCnt,
             uint32_t * buTime,
             uint32_t * bodTime,
             uint8_t * currentApp);
void buADCInputSel(ADC_PosSel_TypeDef input);
void buMeasVdd(uint16_t * buVddVoltage, uint16_t * aVddVoltage);
void buPwrSwitch(bool chargeEn, bool disCharge);
void buSetCurApp(uint8_t app);

#endif /* APP_BUMODE_H */
