/***************************************************************************//**
 * @file
 * @brief IO Sd Mem Declarations
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

#ifndef  _SD_MEM_PRIV_H_
#define  _SD_MEM_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/include/sd.h>
#include  <io/source/sd/sd_priv.h>

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

typedef struct sd_mem_drv_api {
  void *(*CardConn)(SD_FNCT_HANDLE fnct_handle);

  void (*CardDisconn)(void *p_fnct_data);
} SD_MEM_DRV_API;

/********************************************************************************************************
 *                                           ISR HANDLER FUNCTION
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

void SD_MEM_SingleBlkRd(SD_FNCT_HANDLE fnct_handle,
                        CPU_INT08U     *p_dest,
                        CPU_INT32U     blk_addr,
                        RTOS_ERR       *p_err);

void SD_MEM_MultBlkRd(SD_FNCT_HANDLE fnct_handle,
                      CPU_INT08U     *p_dest,
                      CPU_INT32U     blk_addr,
                      CPU_INT32U     blk_cnt,
                      RTOS_ERR       *p_err);

void SD_MEM_SingleBlkWr(SD_FNCT_HANDLE fnct_handle,
                        CPU_INT08U     *p_dest,
                        CPU_INT32U     blk_addr,
                        RTOS_ERR       *p_err);

void SD_MEM_MultBlkWr(SD_FNCT_HANDLE fnct_handle,
                      CPU_INT08U     *p_dest,
                      CPU_INT32U     blk_addr,
                      CPU_INT32U     blk_cnt,
                      RTOS_ERR       *p_err);

void SD_MEM_BlkInfoGet(SD_FNCT_HANDLE fnct_handle,
                       CPU_INT32U     *p_blk_len,
                       CPU_INT32U     *p_blk_size,
                       RTOS_ERR       *p_err);

#ifdef  __cplusplus
}
#endif

#endif // _SD_MEM_PRIV_H_
