/***************************************************************************//**
 * @file
 * @brief File System - Block Device Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @addtogroup FS_STORAGE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_BLK_DEV_H_
#define  FS_BLK_DEV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/include/fs_obj.h>
#include  <fs/include/fs_media.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_types.h>
#include  <cpu/include/cpu.h>
#include  <common/include/platform_mgr.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Null block device handle.
#define  FS_BLK_DEV_NULL                     FSBlkDev_NullHandle

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Test whether two block device handles are equal.
#define  FS_BLK_DEV_HANDLE_ARE_EQUAL(blk_dev_handle_1, blk_dev_handle_2) \
  ((blk_dev_handle_1.BlkDevPtr == blk_dev_handle_2.BlkDevPtr)            \
   && (blk_dev_handle_1.BlkDevId == blk_dev_handle_2.BlkDevId))

//                                                                 Verify whether a block device handle is null.
#define  FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle)        ((blk_dev_handle.BlkDevPtr == DEF_NULL) \
                                                           && (blk_dev_handle.BlkDevId == 0u))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Block device handle.
typedef struct fs_blk_dev_handle {
  struct fs_blk_dev *BlkDevPtr;
  FS_OBJ_ID         BlkDevId;
} FS_BLK_DEV_HANDLE;

typedef struct fs_blk_dev_cfg {
  MEM_SEG *MemSegPtr;
} FS_BLK_DEV_CFG;

typedef void (*FS_BLK_DEV_ON_CLOSE)(FS_BLK_DEV_HANDLE blk_dev_handle,
                                    RTOS_ERR          *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *
 * Note(s) : (1) The application should use the 'FS_BLK_DEV_NULL' macro instead of the 'FSBlkDev_NullHandle'
 *               global variable.
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Null block device handle (see Note #1).
extern const FS_BLK_DEV_HANDLE FSBlkDev_NullHandle;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       PUBLIC FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

FS_BLK_DEV_HANDLE FSBlkDev_Open(FS_MEDIA_HANDLE media_handle,
                                RTOS_ERR        *p_err);

void FSBlkDev_Close(FS_BLK_DEV_HANDLE blk_dev_handle,
                    RTOS_ERR          *p_err);

FS_BLK_DEV_HANDLE FSBlkDev_Get(FS_MEDIA_HANDLE media_handle);

FS_MEDIA_HANDLE FSBlkDev_MediaGet(FS_BLK_DEV_HANDLE blk_dev_handle);

void FSBlkDev_NameGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                      CPU_CHAR          *p_buf,
                      CPU_SIZE_T        buf_size,
                      RTOS_ERR          *p_err);

void FSBlkDev_Rd(FS_BLK_DEV_HANDLE blk_dev_handle,
                 void              *p_dest,
                 FS_LB_NBR         start_lb_nbr,
                 FS_LB_QTY         lb_cnt,
                 RTOS_ERR          *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSBlkDev_Wr(FS_BLK_DEV_HANDLE blk_dev_handle,
                 void              *p_src,
                 FS_LB_NBR         start_lb_nbr,
                 FS_LB_QTY         lb_cnt,
                 RTOS_ERR          *p_err);

void FSBlkDev_Sync(FS_BLK_DEV_HANDLE blk_dev_handle,
                   RTOS_ERR          *p_err);

void FSBlkDev_Trim(FS_BLK_DEV_HANDLE blk_dev_handle,
                   FS_LB_NBR         start_lb_nbr,
                   FS_LB_QTY         lb_cnt,
                   RTOS_ERR          *p_err);
#endif

CPU_INT08U FSBlkDev_LbSizeLog2Get(FS_BLK_DEV_HANDLE blk_dev_handle,
                                  RTOS_ERR          *p_err);

FS_LB_SIZE FSBlkDev_LbSizeGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                              RTOS_ERR          *p_err);

FS_LB_QTY FSBlkDev_LbCntGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                            RTOS_ERR          *p_err);

CPU_SIZE_T FSBlkDev_AlignReqGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                                RTOS_ERR          *p_err);

void FSBlkDev_OnCloseCbSet(FS_BLK_DEV_HANDLE   blk_dev_handle,
                           FS_BLK_DEV_ON_CLOSE on_close,
                           RTOS_ERR            *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                       MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
