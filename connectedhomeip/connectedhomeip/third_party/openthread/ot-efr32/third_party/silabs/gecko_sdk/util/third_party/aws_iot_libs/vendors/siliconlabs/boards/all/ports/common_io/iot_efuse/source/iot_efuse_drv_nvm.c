/***************************************************************************//**
 * @file    iot_efuse_drv_nvm.c
 * @brief   eFUSE Driver Using NVM3 Storage
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_msc.h"

/* SDK emdrv layer */
#include "nvm3.h"
#include "nvm3_default.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"

/* eFUSE config layer */
#include "iot_efuse_cfg.h"

/* eFUSE driver layer */
#include "iot_efuse_drv_nvm.h"

/*******************************************************************************
 *                             START OF MODULE
 ******************************************************************************/

#if (IOT_EFUSE_CFG_DRIVER == IOT_EFUSE_DRV_NVM_DRIVER_ID)

/*******************************************************************************
 *                    iot_efuse_drv_nvm_driver_init()
 ******************************************************************************/

sl_status_t iot_efuse_drv_nvm_driver_init()
{
  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_efuse_drv_nvm_driver_deinit()
 ******************************************************************************/

sl_status_t iot_efuse_drv_nvm_driver_deinit()
{
  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_efuse_drv_nvm_byte_write()
 ******************************************************************************/

sl_status_t iot_efuse_drv_nvm_byte_write(uint32_t ulAddr, uint8_t *pucData)
{
  /* status variable */
  Ecode_t xEcode = ECODE_NVM3_OK;

  /* store byte in NVM3 memory */
  xEcode = nvm3_writeData(nvm3_defaultHandle, ulAddr, pucData, 1);

  /* done */
  return xEcode == ECODE_NVM3_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                  iot_efuse_drv_nvm_byte_read()
 ******************************************************************************/

sl_status_t iot_efuse_drv_nvm_byte_read(uint32_t ulAddr, uint8_t *pucData)
{
  /* status variable */
  Ecode_t xEcode = ECODE_NVM3_OK;

  /* restore byte from NVM3 memory */
  xEcode = nvm3_readData(nvm3_defaultHandle, ulAddr, pucData, 1);

  /* done */
  return xEcode == ECODE_NVM3_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                             END OF MODULE
 ******************************************************************************/

#endif
