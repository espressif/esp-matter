/***************************************************************************//**
 * @file
 * @brief File System - NAND Flash Devices - Static Configuration
 *        Part-Layer Driver
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error NAND module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <fs_storage_cfg.h>

#include  <fs/source/storage/nand/fs_nand_priv.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                (FS, DRV, NAND)
#define  RTOS_MODULE_CUR            RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_PartStatic_Open()
 *
 * @brief    Open (initialize) a NAND part instance & get NAND device information.
 *
 * @param    p_nand_ctrlr    Pointer to a NAND controller instance.
 *
 * @param    p_part_info     Pointer to a part description structure.
 *
 * @param    p_seg           Pointer to a memory segment where to allocate the part data.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to part instance.
 *******************************************************************************************************/
FS_NAND_PART *FS_NAND_PartStatic_Add(FS_NAND_CTRLR            *p_nand_ctrlr,
                                     const FS_NAND_PART_PARAM *p_part_info,
                                     MEM_SEG                  *p_seg,
                                     RTOS_ERR                 *p_err)
{
  FS_NAND_PART *p_part;

  PP_UNUSED_PARAM(p_nand_ctrlr);

  //                                                               ------------------ VALIDATE ARGS -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET(p_part_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  p_part = (FS_NAND_PART *)Mem_SegAlloc("FS - NAND static part data",
                                        p_seg,
                                        sizeof(FS_NAND_PART),
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Could not alloc mem for part data."));
    return (DEF_NULL);
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)                        // ------------------- VALIDATE CFG -------------------
                                                                // Validate bus width.
  if ((p_part_info->BusWidth != 8u) && (p_part_info->BusWidth != 16u)) {
    LOG_ERR(("Invalid bus width: ", (u)p_part_info->BusWidth, ". Must be 8 or 16.\r\n"));
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  //                                                               Validate defect mark len.
  if (p_part_info->DefectMarkType >= DEFECT_MARK_TYPE_NBR) {
    LOG_ERR(("Invalid defect mark type: ", (u) p_part_info->DefectMarkType, "."));
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_CFG, DEF_NULL);
  }
#endif

  //                                                               Cfg part data.
  p_part->Info.BlkCnt = p_part_info->BlkCnt;
  p_part->Info.PgPerBlk = p_part_info->PgPerBlk;
  p_part->Info.PgSize = p_part_info->PgSize;
  p_part->Info.SpareSize = p_part_info->SpareSize;
  p_part->Info.NbrPgmPerPg = p_part_info->NbrPgmPerPg;
  p_part->Info.BusWidth = p_part_info->BusWidth;
  p_part->Info.ECC_NbrCorrBits = p_part_info->ECC_NbrCorrBits;
  p_part->Info.ECC_CodewordSize = p_part_info->ECC_CodewordSize;
  p_part->Info.DefectMarkType = p_part_info->DefectMarkType;
  p_part->Info.MaxBadBlkCnt = p_part_info->MaxBadBlkCnt;
  p_part->Info.MaxBlkErase = p_part_info->MaxBlkErase;

  return (p_part);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NAND_AVAIL
