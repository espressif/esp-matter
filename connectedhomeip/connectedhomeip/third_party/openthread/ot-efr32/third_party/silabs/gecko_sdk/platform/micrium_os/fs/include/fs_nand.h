/***************************************************************************//**
 * @file
 * @brief File System - NAND Media Operations
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

#ifndef  FS_NAND_H_
#define  FS_NAND_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  "fs_blk_dev.h"

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/
//                                                                 Controller and part API forward declaration.
typedef struct fs_nand_ctrlr_api FS_NAND_CTRLR_API;

typedef CPU_INT16U FS_NAND_BLK_QTY;                             // Block quantity.
typedef CPU_INT16U FS_NAND_PG_SIZE;                             // Page size
typedef CPU_INT16U FS_NAND_PG_PER_BLK_QTY;                      // Number of pages per block.

/*
 ********************************************************************************************************
 *                                   NAND flash defect mark data type
 *******************************************************************************************************/

typedef enum {
  DEFECT_SPARE_L_1_PG_1_OR_N_ALL_0 = 0,                         ///< Spare byte/word 1 in first or last pg == 0.
  DEFECT_SPARE_ANY_PG_1_OR_N_ALL_0 = 1,                         ///< Spare any byte/word in first or last pg == 0.
  DEFECT_SPARE_B_6_W_1_PG_1_OR_2 = 2,                           ///< Spare byte 6/word 1 in pg 1 or 2 != FFh.
  DEFECT_SPARE_L_1_PG_1_OR_2 = 3,                               ///< Spare byte/word 1 in pg 1 or 2 != FFh.
  DEFECT_SPARE_B_1_6_W_1_IN_PG_1 = 4,                           ///< Spare byte 1&6/word 1 in pg 1 != FFh.
  DEFECT_PG_L_1_OR_N_PG_1_OR_2 = 5,                             ///< Byte/word 1 (main area) in page 1 or 2 != FFh.
  DEFECT_MARK_TYPE_NBR = 6                                      ///< Must be last in enum.
} FS_NAND_DEFECT_MARK_TYPE;

/*
 ********************************************************************************************************
 *                                       NAND free spare area data type
 *******************************************************************************************************/

typedef struct fs_nand_free_spare_data {
  FS_NAND_PG_SIZE OctetOffset;                                  ///< Offset in octets of free section of spare.
  FS_NAND_PG_SIZE OctetLen;                                     ///< Len in octets of free section of spare.
} FS_NAND_FREE_SPARE_DATA;

/*
 ********************************************************************************************************
 *                                        NAND handle data type
 *******************************************************************************************************/

typedef struct fs_nand_handle {
  FS_MEDIA_HANDLE MediaHandle;                                  ///< Generic media handle.
} FS_NAND_HANDLE;

/*
 ********************************************************************************************************
 *                                     NAND flash part parameters
 *******************************************************************************************************/

typedef struct fs_nand_part_param {
  FS_NAND_BLK_QTY          BlkCnt;                              ///< Total number of blocks.
  FS_NAND_PG_PER_BLK_QTY   PgPerBlk;                            ///< Nbr of pgs per blk.
  FS_NAND_PG_SIZE          PgSize;                              ///< Size (in octets) of each pg.
  FS_NAND_PG_SIZE          SpareSize;                           ///< Size (in octets) of spare area per pg.
  CPU_BOOLEAN              SupportsRndPgPgm;                    ///< Supports random page programming.
  CPU_INT08U               NbrPgmPerPg;                         ///< Nbr of program operation per pg.
  CPU_INT08U               BusWidth;                            ///< Bus width of NAND dev.
  CPU_INT08U               ECC_NbrCorrBits;                     ///< Nbr of bits of ECC correctability.
  FS_NAND_PG_SIZE          ECC_CodewordSize;                    ///< ECC codeword size in bytes.
  FS_NAND_DEFECT_MARK_TYPE DefectMarkType;                      ///< Factory defect mark type.
  FS_NAND_BLK_QTY          MaxBadBlkCnt;                        ///< Max nbr of bad blk in dev.
  CPU_INT32U               MaxBlkErase;                         ///< Maximum number of erase operations per block.
} FS_NAND_PART_PARAM;

/*
 ********************************************************************************************************
 *                                     NAND flash part information
 *******************************************************************************************************/

typedef struct fs_nand_part_hw_info {
  const FS_NAND_PART_PARAM      *PartParamPtr;                  ///< Ptr to struct describing NAND flash parameters.
  const FS_NAND_FREE_SPARE_DATA *FreeSpareMap;                  ///< Ptr to NAND flash spare area layout within pages.
  CPU_INT16U                    PartSlaveID;                    ///< NAND chip slave ID to en/dis it from MCU.
} FS_NAND_PART_HW_INFO;

/*
 ********************************************************************************************************
 *                                     NAND controller information
 *******************************************************************************************************/

struct fs_nand_ctrlr_info {
  const FS_NAND_CTRLR_API *CtrlrApiPtr;                         ///< Pointer to NAND controller driver API.
  CPU_SIZE_T              AlignReq;                             ///< Buffer alignment required by NAND controller.
};

/*
 ********************************************************************************************************
 *                                   NAND flash hardware information
 *******************************************************************************************************/

typedef struct fs_nand_hw_info {
  const struct fs_nand_ctrlr_info *CtrlrHwInfoPtr;              ///< Pointer to NAND controller info.
  const FS_NAND_PART_HW_INFO      *PartHwInfoPtr;               ///< Pointer to NAND flash device info.
} FS_NAND_HW_INFO;

/*
 ********************************************************************************************************
 *                                     NAND platform manager item
 *******************************************************************************************************/

typedef struct fs_nand_pm_item {
  struct fs_media_pm_item MediaPmItem;                          ///< Item for NAND media type.
  const FS_NAND_HW_INFO   *HwInfoPtr;                           ///< Pointer to NAND hw info.
} FS_NAND_PM_ITEM;

/*
 ********************************************************************************************************
 *                                     NAND dump utility function
 *******************************************************************************************************/

typedef void (*FS_NAND_DUMP_FUNC) (void       *buf,
                                   CPU_SIZE_T buf_len,
                                   RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const struct fs_media_api FS_NAND_MediaApi;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NAND_HW_INFO_INIT(p_ctrlr_hw_info, p_part_hw_info) \
  {                                                            \
    .CtrlrHwInfoPtr = &(p_ctrlr_hw_info)->CtrlrHwInfo,         \
    .PartHwInfoPtr = p_part_hw_info                            \
  };

#define  FS_NAND_HW_INFO_REG(name, p_hw_info)                                    \
  do {                                                                           \
    if (((FS_NAND_HW_INFO *)(p_hw_info))->CtrlrHwInfoPtr != DEF_NULL) {          \
      static const struct fs_nand_pm_item _pm_item = {                           \
        .MediaPmItem.PmItem.StrID = name,                                        \
        .MediaPmItem.PmItem.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NAND,       \
        .MediaPmItem.MediaApiPtr = &FS_NAND_MediaApi,                            \
        .HwInfoPtr = (const FS_NAND_HW_INFO *)(p_hw_info)                        \
      };                                                                         \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_pm_item.MediaPmItem.PmItem); \
    }                                                                            \
  } while (0);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

FS_NAND_HANDLE FS_NAND_Open(FS_MEDIA_HANDLE media_handle,
                            RTOS_ERR        *p_err);

void FS_NAND_Close(FS_NAND_HANDLE nand_handle,
                   RTOS_ERR       *p_err);

void FS_NAND_BlkErase(FS_NAND_HANDLE  nand_handle,
                      FS_NAND_BLK_QTY blk_ix,
                      RTOS_ERR        *p_err);

void FS_NAND_ChipErase(FS_NAND_HANDLE nand_handle,
                       RTOS_ERR       *p_err);

#if (FS_NAND_CFG_DUMP_SUPPORT_EN == DEF_ENABLED)
void FS_NAND_Dump(FS_NAND_HANDLE    nand_handle,
                  FS_NAND_DUMP_FUNC dump_func,
                  void              *p_buf,
                  FS_NAND_BLK_QTY   first_blk_ix,
                  FS_NAND_BLK_QTY   blk_cnt,
                  RTOS_ERR          *p_err);
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
