/***************************************************************************//**
 * @file    iot_efuse_drv_ram.c
 * @brief   eFUSE Driver Using RAM Simulation
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

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"

/* eFUSE config layer */
#include "iot_efuse_cfg.h"

/* eFUSE driver layer */
#include "iot_efuse_drv_ram.h"

/*******************************************************************************
 *                             START OF MODULE
 ******************************************************************************/

#if (IOT_EFUSE_CFG_DRIVER == IOT_EFUSE_DRV_RAM_DRIVER_ID)

/*******************************************************************************
 *                            STATIC VARIABLES
 ******************************************************************************/

static uint8_t iot_efuse_drv_ram_buf[IOT_EFUSE_CFG_MAX_SIZE];

/*******************************************************************************
 *                   iot_efuse_drv_ram_driver_init()
 ******************************************************************************/

sl_status_t iot_efuse_drv_ram_driver_init()
{
  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                 iot_efuse_drv_ram_driver_deinit()
 ******************************************************************************/

sl_status_t iot_efuse_drv_ram_driver_deinit()
{
  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_efuse_drv_ram_byte_write()
 ******************************************************************************/

sl_status_t iot_efuse_drv_ram_byte_write(uint32_t ulAddr, uint8_t *pucData)
{
  /* store byte in simulation buffer */
  iot_efuse_drv_ram_buf[ulAddr] = *pucData;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_efuse_drv_ram_byte_read()
 ******************************************************************************/

sl_status_t iot_efuse_drv_ram_byte_read(uint32_t ulAddr, uint8_t *pucData)
{
  /* restore byte from simulation buffer */
  *pucData = iot_efuse_drv_ram_buf[ulAddr];

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                             END OF MODULE
 ******************************************************************************/

#endif
