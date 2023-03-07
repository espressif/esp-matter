/***************************************************************************//**
 * @file
 * @brief Dot matrix display SSD2119 interface using EBI
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __DMD_IF_SSD2119_EBI_H_
#define __DMD_IF_SSD2119_EBI_H_

#include <stdint.h>
#include "em_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Module Prototypes */
EMSTATUS DMDIF_init(uint32_t cmdRegAddr, uint32_t dataRegAddr);
EMSTATUS DMDIF_writeReg(uint8_t reg, uint16_t data);
uint16_t DMDIF_readDeviceCode(void);
EMSTATUS DMDIF_prepareDataAccess(void);
EMSTATUS DMDIF_writeData(uint32_t data);
EMSTATUS DMDIF_writeDataRepeated(uint32_t data, int len);
uint32_t DMDIF_readData(void);
EMSTATUS DMDIF_delay(uint32_t ms);

/** @endcond */

#ifdef __cplusplus
}
#endif

#endif
