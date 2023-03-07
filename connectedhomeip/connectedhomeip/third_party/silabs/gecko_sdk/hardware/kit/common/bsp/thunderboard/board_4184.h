/***************************************************************************//**
 * @file
 * @brief BOARD module header file
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

#ifndef BOARD_4184_H
#define BOARD_4184_H

#include <stdint.h>

/**************************************************************************//**
* @addtogroup BRD4184_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup BOARD_4184
 * @{
 ******************************************************************************/

uint32_t BOARD_hallSensorEnable    (bool enable);

void     BOARD_pushButton0SetIRQCallback(BOARD_IrqCallback cb);
void     BOARD_pushButton0ClearIRQ(void);

/** @} */
/** @} */

#endif // BOARD_4166_H
