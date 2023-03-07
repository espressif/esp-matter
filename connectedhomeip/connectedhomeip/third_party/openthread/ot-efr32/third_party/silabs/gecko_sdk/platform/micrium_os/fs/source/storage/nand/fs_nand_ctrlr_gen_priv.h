/***************************************************************************//**
 * @file
 * @brief File System - NAND Flash Devices - Generic Controller-Layer Driver
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

#ifndef  FS_NAND_CTRLR_GEN_PRIV_H_
#define  FS_NAND_CTRLR_GEN_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/source/storage/nand/fs_nand_priv.h>
#include  <fs/include/fs_nand_ctrlr_gen.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------- READ ID CMD --------------------
#define  FS_NAND_CTRLR_GEN_CMD_READ_ID_DOUT_5           5u      // READ ID cmd has 5 bytes of data returned by NAND.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_nand_ctrlr_gen_ext_api FS_NAND_CTRLR_GEN_EXT_API;
typedef struct fs_nand_ctrlr_gen_ext_hw_info FS_NAND_CTRLR_GEN_EXT_HW_INFO;

/********************************************************************************************************
 *                                       NAND MEMORY CONTROLLER DRIVER
 *******************************************************************************************************/

struct fs_nand_ctrlr_drv {
  void                      *DataPtr;                           // Drv's data.
  FS_NAND_CTRLR_GEN_HW_INFO *HW_InfoPtr;                        // Ptr to generic mem controller HW info.
  CPU_INT16U                PartSlaveID;                        // NAND chip slave ID to en/dis it from MCU.
};

/********************************************************************************************************
 *                                   NAND GENERIC CONTROLLER DATA TYPE
 *******************************************************************************************************/
//                                                                 *INDENT-OFF*
struct fs_nand_ctrlr_gen {
  FS_NAND_CTRLR                    Ctrl;
  FS_NAND_CTRLR_DRV                Drv;

  CPU_INT08U                       AddrSize;                    // Size in B of addr.
  CPU_INT08U                       ColAddrSize;                 // Size in B of col addr.
  CPU_INT08U                       RowAddrSize;                 // Size in B of row addr.

  FS_NAND_PG_SIZE                  SecSize;                     // Size in octets of sec.
  FS_NAND_PG_SIZE                  SpareTotalAvailSize;         // Nbr of avail spare bytes.

  FS_NAND_PG_SIZE                  OOS_SizePerSec;              // Size in octets of OOS area per sec.
  FS_NAND_PG_SIZE                  OOS_Offset;                  // Offset in octets of OOS areas.

  FS_NAND_CTRLR_GEN_SPARE_SEG_INFO *OOS_InfoTbl;                // OOS segments info tbl.

  void                             *SpareBufPtr;                // Ptr to OOS buf.
  FS_NAND_PG_SIZE                  SpareBufSize;                // Size in octets of OOS buf.

  void                             *CtrlrExtData;               // Pointer to ctrlr ext data.
  const FS_NAND_CTRLR_GEN_EXT_API  *CtrlrExtPtr;                // Pointer to ctrlr ext.

  FS_NAND_PART                     *PartDataPtr;

  const FS_NAND_FREE_SPARE_DATA    *FreeSpareMap;

#if ((FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED) \
  || (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED))
  FS_NAND_CTRLR_GEN_CTRS           Ctrs;
#endif

  FS_NAND_CTRLR_GEN                *NextPtr;
};

/********************************************************************************************************
 *                               NAND GENERIC CONTROLLER EXTENSION DATA TYPE
 *******************************************************************************************************/

struct fs_nand_ctrlr_gen_ext_api {
  void *(*Open)(FS_NAND_CTRLR_GEN                   *p_ctrlr_gen,
                const FS_NAND_CTRLR_GEN_EXT_HW_INFO *p_ext_cfg,
                MEM_SEG                             *p_seg,
                RTOS_ERR                            *p_err);

  void (*Close)(void *p_ext_data);

  FS_NAND_PG_SIZE (*Setup)(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                           void              *p_ext_data,
                           RTOS_ERR          *p_err);

  void (*RdStatusChk)(void     *p_ext_data,
                      RTOS_ERR *p_err);

  void (*ECC_Calc)(void            *p_ext_data,
                   void            *p_sec_buf,
                   void            *p_oos_buf,
                   FS_NAND_PG_SIZE oos_size,
                   RTOS_ERR        *p_err);

  void (*ECC_Verify)(void            *p_ext_data,
                     void            *p_sec_buf,
                     void            *p_oos_buf,
                     FS_NAND_PG_SIZE oos_size,
                     RTOS_ERR        *p_err);
};
//                                                                 *INDENT-ON*
/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
