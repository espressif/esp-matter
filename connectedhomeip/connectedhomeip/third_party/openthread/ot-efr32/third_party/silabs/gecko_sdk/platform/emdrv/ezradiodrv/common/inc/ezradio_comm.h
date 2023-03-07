/***************************************************************************//**
 * @file
 * @brief This file contains the EZRadio communication layer.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef _EZRADIO_COMM_H_
#define _EZRADIO_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup ezradiodrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_comm EZRADIODRV Comm Layer
 * @brief EzRadio Communication Layer
 * @{
 ******************************************************************************/

#if defined(EZRADIODRV_SPI_4WIRE_MODE)
#define EZRADIODRV_MAX_CTS_BUFF_SIZE 257
#endif

extern uint8_t ezradio_comm_CtsWentHigh;

uint8_t ezradio_comm_GetResp(uint8_t byteCount, uint8_t* pData);
void ezradio_comm_SendCmd(uint8_t byteCount, uint8_t* pData);
void ezradio_comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);
void ezradio_comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);

uint8_t ezradio_comm_PollCTS(void);
uint8_t ezradio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData,
                                    uint8_t respByteCount, uint8_t* pRespData);
void ezradio_comm_ClearCTS(void);

/** @} (end addtogroup ezradiodrv_comm) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#endif //_EZRADIO_COMM_H_
