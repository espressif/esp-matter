/***************************************************************************//**
 * @file
 * @brief File System - NAND PHY Operations
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_NAND_PRIV_H_
#define  FS_NAND_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs/include/fs_nand.h>
#include  <fs/source/storage/fs_media_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/source/collections/slist_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NAND_PART_ONFI_PARAM_PAGE_LEN                   256u

#define  FS_NAND_PG_IX_INVALID                              ((FS_NAND_PG_SIZE)-1)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_nand_ctrlr {
  const FS_NAND_CTRLR_API *CtrlrApiPtr;
} FS_NAND_CTRLR;

typedef struct fs_nand {
  FS_MEDIA      Media;
  FS_NAND_CTRLR *CtrlrPtr;
  CPU_BOOLEAN   IsOpen;
} FS_NAND;

typedef struct fs_nand_part {
  FS_NAND_PART_PARAM Info;
} FS_NAND_PART;

typedef struct fs_nand_oos_info {
  FS_NAND_PG_SIZE Size;
  void            *BufPtr;
} FS_NAND_OOS_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_NAND_PART *FS_NAND_PartONFI_Add(FS_NAND_CTRLR *p_nand_ctrlr,
                                   MEM_SEG       *p_seg,
                                   RTOS_ERR      *p_err);

FS_NAND_PART *FS_NAND_PartStatic_Add(FS_NAND_CTRLR            *p_nand_ctrlr,
                                     const FS_NAND_PART_PARAM *p_part_info,
                                     MEM_SEG                  *p_seg,
                                     RTOS_ERR                 *p_err);

void FS_NAND_BlkEraseInternal(FS_NAND         *p_nand,
                              FS_NAND_BLK_QTY blk_ix_phy,
                              RTOS_ERR        *p_err);
//                                                                 *INDENT-OFF*
struct fs_nand_ctrlr_api {
  FS_NAND_CTRLR *(*Add)(const FS_NAND_PM_ITEM *p_pm_item,
                        MEM_SEG               *p_seg,
                        RTOS_ERR              *p_err);

  void (*Open)(FS_NAND_CTRLR *p_nand_ctrlr,
                             RTOS_ERR      *p_err);

  void (*Close)(FS_NAND_CTRLR *p_nand_ctrlr);

  FS_NAND_PART_PARAM *(*PartInfoGet)(FS_NAND_CTRLR *p_nand_ctrlr);

  FS_NAND_OOS_INFO (*Setup)(FS_NAND_CTRLR   *p_nand_ctrlr,
                            MEM_SEG         *p_seg,
                            CPU_SIZE_T      spare_buf_align,
                            FS_NAND_PG_SIZE sec_size,
                            RTOS_ERR        *p_err);

  void (*SecRd)(FS_NAND_CTRLR *p_nand_ctrlr,
                void          *p_dest,
                void          *p_dest_oos,
                FS_LB_NBR     sec_ix_phy,
                RTOS_ERR      *p_err);

  void (*OOSRdRaw)(FS_NAND_CTRLR   *p_nand_ctrlr,
                   void            *p_dest_oos,
                   FS_LB_NBR       sec_nbr_phy,
                   FS_NAND_PG_SIZE offset,
                   FS_NAND_PG_SIZE length,
                   RTOS_ERR        *p_err);

  void (*SpareRdRaw)(FS_NAND_CTRLR   *p_nand_ctrlr,
                     void            *p_dest_oos,
                     FS_LB_QTY       pg_nbr_phy,
                     FS_NAND_PG_SIZE offset,
                     FS_NAND_PG_SIZE length,
                     RTOS_ERR        *p_err);

  void (*SecWr)(FS_NAND_CTRLR *p_nand_ctrlr,
                void          *p_src,
                void          *p_src_spare,
                FS_LB_NBR     sec_nbr_phy,
                RTOS_ERR      *p_err);

  void (*BlkErase)(FS_NAND_CTRLR *p_nand_ctrlr,
                   CPU_INT32U    blk_nbr_phy,
                   RTOS_ERR      *p_err);

  void (*ParamPgRd)(FS_NAND_CTRLR *p_nand_ctrlr,
                    CPU_INT16U    rel_addr,
                    CPU_INT16U    byte_cnt,
                    CPU_INT08U    *p_buf,
                    RTOS_ERR      *p_err);

  void (*PgRdRaw)(FS_NAND_CTRLR   *p_nand_ctrlr,
                  void            *p_dest,
                  FS_LB_NBR       pg_ix_phy,
                  FS_NAND_PG_SIZE offset,
                  FS_NAND_PG_SIZE len,
                  RTOS_ERR        *p_err);
};
//                                                                 *INDENT-ON*
/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
