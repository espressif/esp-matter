/***************************************************************************//**
 * @file
 * @brief Mass Storage class Device (MSD) driver.
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
#ifndef __MSDD_H
#define __MSDD_H

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Msd
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define MEDIA_BUFSIZ    4096      /**< Intermediate media storage buffer size */

/**************************************************************************//**
 * @brief Status info for one BOT CBW -> Data I/O -> CSW cycle.
 *****************************************************************************/
typedef struct {
  bool        valid;     /**< True if the CBW is valid.    */
  uint8_t     direction; /**< Set if BOT direction is IN.  */
  uint8_t     *pData;    /**< Media data pointer.          */
  uint32_t    lba;       /**< SCSI Read/Write lba address. */
  uint32_t    xferLen;   /**< BOT transfer length.         */
  uint32_t    maxBurst;  /**< Max length of one transfer.  */
  enum { XFER_MEMORYMAPPED = 0, XFER_INDIRECT } xferType;
  /**< MSD media access type.       */
} MSDD_CmdStatus_TypeDef;

/*** MSDD Device Driver Function prototypes ***/

bool MSDD_Handler(void);
void MSDD_Init(int activityLedPort, uint32_t activityLedPin);
int  MSDD_SetupCmd(const USB_Setup_TypeDef *setup);
void MSDD_StateChangeEvent(USBD_State_TypeDef oldState,
                           USBD_State_TypeDef newState);

#ifdef __cplusplus
}
#endif

/** @} (end group Msd) */
/** @} (end group kitdrv) */

#endif /* __MSDD_H */
