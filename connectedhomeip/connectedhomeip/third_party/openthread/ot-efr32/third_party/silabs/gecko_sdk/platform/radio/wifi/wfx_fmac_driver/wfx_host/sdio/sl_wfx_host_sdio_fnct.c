/***************************************************************************//**
 * @file sl_wfx_host_sdio_fnct.c
 * @brief Network Wi-Fi Driver for the WFX over SDIO
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 *********************************************************************************************************
 *********************************************************************************************************
 */

#include  <rtos_description.h>
#include "sl_wfx_configuration_defaults.h"

#if (defined(RTOS_MODULE_IO_SD_AVAIL))

#if (!defined(RTOS_MODULE_IO_AVAIL))

#error IO SD module requires IO module. Make sure it is part of your project and that \
  RTOS_MODULE_IO_AVAIL is defined in rtos_description.h.

#endif

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                               INCLUDES
 *********************************************************************************************************
 *********************************************************************************************************
 */

#include  <stdio.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <io/include/sd.h>
#include  <io/source/sd/sd_io_fnct_priv.h>

#include  <sl_wfx_constants.h>

#include  "sl_wfx_host_sdio_fnct.h"

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                               LOCAL DEFINES
 *********************************************************************************************************
 *********************************************************************************************************
 */

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO

#define  SDIO_FNCT_BUF_LEN                 48u
#define  SDIO_FNCT_BLK_LEN                  4u
#define  SDIO_FNCT_BLK_QTY                 64u

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 *********************************************************************************************************
 */

static KAL_SEM_HANDLE  sdio_fnct_sem;

static SD_BUS_HANDLE   sdio_fnct_bus_handle;

static SD_FNCT_HANDLE  sdio_fnct_handle;

static CPU_INT08U      sdio_fnct_buf[SDIO_FNCT_BUF_LEN];

static CPU_INT08U      sdio_fnct_blk_buf[SDIO_FNCT_BLK_LEN * SDIO_FNCT_BLK_QTY];

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                           LOCAL DATA TYPES
 *********************************************************************************************************
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                               LOCAL TABLES
 *********************************************************************************************************
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************
 *********************************************************************************************************
 */

typedef void (*sdio_fnct_card_int_callback_t)(void);

static sdio_fnct_card_int_callback_t sdio_fnct_card_int_callback;

static  CPU_BOOLEAN  sdio_fnct_probe  (SD_BUS_HANDLE     bus_handle,
                                       SD_FNCT_HANDLE    fnct_handle,
                                       CPU_INT08U        sdio_fnct_if_code,
                                       void            **pp_fnct_data);

static  void         sdio_fnct_conn   (SD_FNCT_HANDLE    fnct_handle,
                                       void             *p_fnct_data);

static  void         sdio_fnct_int    (SD_FNCT_HANDLE    fnct_handle,
                                       void             *p_fnct_data);

static  void         sdio_fnct_disconn(SD_FNCT_HANDLE    fnct_handle,
                                       void             *p_fnct_data);

/*
 *********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 *********************************************************************************************************
 */

const  SD_IO_FNCT_DRV_API  sdio_FnctDrvAPI = {
  .CardFnctProbe   = sdio_fnct_probe,
  .CardFnctConn    = sdio_fnct_conn,
  .CardFnctDisconn = sdio_fnct_disconn,
  .CardFnctInt     = sdio_fnct_int
};

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 *********************************************************************************************************
 *********************************************************************************************************
 */

static void  sdio_fnct_task (void *p_arg);

void  sdio_fnct_init(RTOS_ERR  *p_err)
{
  KAL_TASK_HANDLE  task_handle;

  sdio_fnct_card_int_callback = DEF_NULL;

  SD_IO_FnctDrvReg(&sdio_FnctDrvAPI, p_err);

  sdio_fnct_sem = KAL_SemCreate("SDIO Fnct Sem", DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  task_handle = KAL_TaskAlloc("SDIO Fnct Task", DEF_NULL, 512u, DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(task_handle, sdio_fnct_task, DEF_NULL, 3u, DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 *********************************************************************************************************
 *********************************************************************************************************
 */
void sdio_fnct_int_reg(void* callback)
{
  sdio_fnct_card_int_callback = (sdio_fnct_card_int_callback_t) callback;
}

static CPU_BOOLEAN sdio_fnct_probe(SD_BUS_HANDLE     bus_handle,
                                   SD_FNCT_HANDLE    fnct_handle,
                                   CPU_INT08U        sdio_fnct_if_code,
                                   void            **pp_fnct_data)
{
  (void)sdio_fnct_if_code;
  (void)pp_fnct_data;

  sdio_fnct_bus_handle = bus_handle;
  sdio_fnct_handle = fnct_handle;

  return (DEF_OK);
}

static void sdio_fnct_conn(SD_FNCT_HANDLE   fnct_handle,
                           void            *p_fnct_data)
{
  RTOS_ERR  err;

  (void)fnct_handle;
  (void)p_fnct_data;

  KAL_SemPost(sdio_fnct_sem, KAL_OPT_POST_NONE, &err);
}

static void sdio_fnct_disconn(SD_FNCT_HANDLE   fnct_handle,
                              void            *p_fnct_data)
{
  (void)fnct_handle;
  (void)p_fnct_data;
}

CPU_INT08U sdio_fnct_rdbyte(CPU_INT32U  reg_addr,
                            RTOS_ERR   *p_err)
{
  CPU_INT08U  byte;

  byte = SD_IO_FnctRdByte(sdio_fnct_bus_handle,
                          sdio_fnct_handle,
                          reg_addr,
                          p_err);

  return (byte);
}

void sdio_fnct_wrbyte(CPU_INT32U  reg_addr,
                      CPU_INT08U  byte,
                      RTOS_ERR   *p_err)
{
  SD_IO_FnctWrByte(sdio_fnct_bus_handle,
                   sdio_fnct_handle,
                   reg_addr,
                   byte,
                   p_err);
}

void sdio_fnct_rd(CPU_INT32U   reg_addr,
                  CPU_INT08U  *p_buf,
                  CPU_INT16U   buf_len,
                  CPU_BOOLEAN  fixed_addr,
                  RTOS_ERR    *p_err)
{
  SD_IO_FnctRd(sdio_fnct_bus_handle,
               sdio_fnct_handle,
               reg_addr,
               p_buf,
               buf_len,
               fixed_addr,
               p_err);
}

void sdio_fnct_wr(CPU_INT32U    reg_addr,
                  CPU_INT08U   *p_buf,
                  CPU_INT16U    buf_len,
                  CPU_BOOLEAN   fixed_addr,
                  RTOS_ERR     *p_err)
{
  SD_IO_FnctWr(sdio_fnct_bus_handle,
               sdio_fnct_handle,
               reg_addr,
               p_buf,
               buf_len,
               fixed_addr,
               p_err);
}

void sdio_fnct_rdblk(CPU_INT32U    reg_addr,
                     CPU_INT08U   *p_buf,
                     CPU_INT16U    blk_nbr,
                     CPU_BOOLEAN   fixed_addr,
                     RTOS_ERR     *p_err)
{
  SD_IO_FnctRdBlk(sdio_fnct_bus_handle,
                  sdio_fnct_handle,
                  reg_addr,
                  p_buf,
                  blk_nbr,
                  fixed_addr,
                  p_err);
}

void sdio_fnct_wrblk(CPU_INT32U   reg_addr,
                     CPU_INT08U  *p_buf,
                     CPU_INT16U   blk_nbr,
                     CPU_BOOLEAN  fixed_addr,
                     RTOS_ERR    *p_err)
{
  SD_IO_FnctWrBlk(sdio_fnct_bus_handle,
                  sdio_fnct_handle,
                  reg_addr,
                  p_buf,
                  blk_nbr,
                  fixed_addr,
                  p_err);
}

static void sdio_fnct_int(SD_FNCT_HANDLE  fnct_handle,
                          void           *p_fnct_data)
{
  (void)fnct_handle;
  (void)p_fnct_data;

  if (sdio_fnct_card_int_callback != DEF_NULL) {
    sdio_fnct_card_int_callback();
  }
}

void sdio_fnct_int_en(CPU_BOOLEAN  enable,
                      RTOS_ERR    *p_err)
{
  SD_IO_FnctIntEnDis(sdio_fnct_bus_handle,
                     sdio_fnct_handle,
                     enable,
                     p_err);
}

static void sdio_fnct_task(void *p_arg)
{
  RTOS_ERR     err;
  CPU_BOOLEAN  is_blk_mode_ok;

  (void)p_arg;

  while (1) {
    KAL_SemPend(sdio_fnct_sem, KAL_OPT_PEND_NONE, 0, &err);

    Mem_Set(sdio_fnct_buf, 0xFF, SDIO_FNCT_BUF_LEN);
    Mem_Set(sdio_fnct_blk_buf, 0xFF, SDIO_FNCT_BLK_LEN * SDIO_FNCT_BLK_QTY);

    is_blk_mode_ok = SD_IO_IsBlkOperSupported(sdio_fnct_bus_handle, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      CPU_SW_EXCEPTION(; );
    }

    if (is_blk_mode_ok) {
      SD_IO_FnctBlkSizeSet(sdio_fnct_bus_handle, sdio_fnct_handle, SL_WFX_SDIO_BLOCK_SIZE, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        CPU_SW_EXCEPTION(; );
      }

      SD_IO_FnctBlkSizeSet(sdio_fnct_bus_handle, SD_IO_FNCT_0, SL_WFX_SDIO_BLOCK_SIZE, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        CPU_SW_EXCEPTION(; );
      }
    }
  }
}

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 *********************************************************************************************************
 *********************************************************************************************************
 */

#endif /* (defined(RTOS_MODULE_IO_SD_AVAIL)) */
