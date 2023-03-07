/***************************************************************************//**
 * @file
 * @brief File System - NOR Media Operations
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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_NOR_PRIV_H_
#define  FS_NOR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs/include/fs_nor.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 *
 * Note(s) : (1) Some flash devices support a mode called XIP (eXecute In Place). XIP mode requires only
 *               an address (no instruction) to output data, improving random access time and eliminating
 *               the need to shadow code onto RAM for fast execution.
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_nor_pm_item FS_NOR_PM_ITEM;
typedef struct fs_nor_spi_pm_item FS_NOR_SPI_PM_ITEM;
typedef struct fs_nor_quad_spi_pm_item FS_NOR_QUAD_SPI_PM_ITEM;

typedef struct fs_nor_phy {
  CPU_INT32U           BlkCnt;                                  // Number of blocks composing flash device.
  CPU_INT08U           BlkSizeLog2;                             // Flash block size expressed in log base-2.
  CPU_BOOLEAN          FourByteAddrSupport;                     // Flag indicating if 4B addr supported by flash dev.
  CPU_BOOLEAN          FourByteAddrSet;                         // Flag indicating if 4B addr set or not.
  CPU_BOOLEAN          FlashXipSupport;                         // Flag indicating if XIP supported by flash dev...
                                                                // ... (see Note #1).
  const FS_NOR_PHY_API *PhyApiPtr;                              // Pointer to NOR PHY API driver.
} FS_NOR_PHY;

typedef struct fs_nor {
  FS_MEDIA             Media;

  const FS_NOR_PM_ITEM *PmItemPtr;
  FS_NOR_PHY           *PhyPtr;

  SLIST_MEMBER         ListMember;
  CPU_BOOLEAN          IsOpen;

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // -------------------- STAT CTRS ---------------------
  CPU_INT64U           StatRdOctetCtr;
  CPU_INT64U           StatWrOctetCtr;
  CPU_INT32U           StatEraseBlkCtr;
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // --------------------- ERR CTRS ---------------------
  CPU_INT16U ErrRdCtr;
  CPU_INT16U ErrWrCtr;
  CPU_INT16U ErrEraseCtr;
  CPU_INT16U ErrWrFailCtr;
  CPU_INT16U ErrEraseFailCtr;
#endif
} FS_NOR;

struct fs_nor_phy_api {
  FS_NOR_PHY *(*Add)(const FS_NOR_PM_ITEM *p_pm_item,
                     MEM_SEG              *p_seg,
                     RTOS_ERR             *p_err);

  void (*Open)(FS_NOR_PHY *p_phy,
               RTOS_ERR   *p_err);

  void (*Close)(FS_NOR_PHY *p_phy,
                RTOS_ERR   *p_err);

  void (*Rd)(FS_NOR_PHY *p_phy,
             void       *p_dest,
             CPU_INT32U start_addr,
             CPU_INT32U cnt,
             RTOS_ERR   *p_err);

  void (*Wr)(FS_NOR_PHY *p_phy,
             void       *p_src,
             CPU_INT32U start_addr,
             CPU_INT32U cnt,
             RTOS_ERR   *p_err);

  void (*BlkErase)(FS_NOR_PHY *p_phy,
                   CPU_INT32U start_addr,
                   CPU_INT32U size,
                   RTOS_ERR   *p_err);

  void (*ChipErase)(FS_NOR_PHY *p_phy,
                    RTOS_ERR   *p_err);

  void (*XipCfg)(FS_NOR_PHY  *p_phy,
                 CPU_BOOLEAN xip_en,
                 RTOS_ERR    *p_err);

  CPU_SIZE_T (*AlignReqGet)(FS_NOR_PHY *p_phy,
                            RTOS_ERR   *p_err);
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FS_NOR_PHY_RdInternal(FS_NOR     *p_nor_media,
                           void       *p_dest,
                           CPU_INT32U start,
                           CPU_INT32U cnt,
                           RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_PHY_WrInternal(FS_NOR     *p_nor_media,
                           void       *p_src,
                           CPU_INT32U start,
                           CPU_INT32U cnt,
                           RTOS_ERR   *p_err);

void FS_NOR_PHY_BlkEraseInternal(FS_NOR     *p_nor_media,
                                 CPU_INT32U blk_ix,
                                 RTOS_ERR   *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
