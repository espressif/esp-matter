/***************************************************************************//**
 * @file
 * @brief IO - Public SPI Api
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

/****************************************************************************************************//**
 * @defgroup IO_SPI SPI API
 * @ingroup  IO
 * @brief      SPI API
 *
 * @addtogroup IO_SPI
 * @{
 *******************************************************************************************************/

#ifndef  _SPI_H_
#define  _SPI_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/platform_mgr.h>
#include  <io/include/serial.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ SPI BUS HANDLE ------------------
typedef struct spi_bus_handle * SPI_BUS_HANDLE;

//                                                                 ------------------- SPI INIT CFG -------------------
typedef struct spi_init_cfg {
  CPU_SIZE_T SlaveHandleQty;                                    // Max quantity of slave handle.
  MEM_SEG    *MemSegPtr;                                        // Ptr to mem segment from where to alloc internal data.
} SPI_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const SPI_BUS_HANDLE SPI_BusHandleNull;

extern const SPI_INIT_CFG SPI_InitCfgDflt;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

void SPI_ConfigureMemSeg(MEM_SEG *p_seg);

void SPI_ConfigureSlaveHandleQty(CPU_SIZE_T handle_nbr);

SPI_BUS_HANDLE SPI_BusAdd(const CPU_CHAR *name,
                          RTOS_ERR       *p_err);

SPI_BUS_HANDLE SPI_BusHandleGetFromName(const CPU_CHAR *name);

void SPI_BusStart(SPI_BUS_HANDLE bus_handle,
                  RTOS_ERR       *p_err);

void SPI_BusStop(SPI_BUS_HANDLE bus_handle,
                 RTOS_ERR       *p_err);

void SPI_BusLoopBackEn(SPI_BUS_HANDLE bus_handle,
                       CPU_BOOLEAN    en,
                       RTOS_ERR       *p_err);

#ifdef  __cplusplus
}
#endif

#endif

///< @}
