/***************************************************************************//**
 * @file
 * @brief File System - NAND Generic Controller Operations
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

#ifndef  FS_NAND_CTRLR_GEN_H_
#define  FS_NAND_CTRLR_GEN_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/include/fs_nand.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NAND_CTRLR_GEN_CTRS_TBL_SIZE  4u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct fs_nand_ctrlr_gen FS_NAND_CTRLR_GEN;
typedef struct fs_nand_ctrlr_drv FS_NAND_CTRLR_DRV;

/********************************************************************************************************
 *                                   NAND MEMORY CONTROLLER DRIVER API
 *******************************************************************************************************/

typedef CPU_BOOLEAN (*FS_NAND_CTRLR_DRV_POLL_FNCT)(void *p_arg);

typedef struct fs_nand_ctrlr_drv_api {
  //                                                               Open mem controller.
  void (*Open)(FS_NAND_CTRLR_DRV *p_drv,
               RTOS_ERR          *p_err);
  //                                                               Close mem controller.
  void (*Close)(FS_NAND_CTRLR_DRV *p_drv);
  //                                                               Enable chip select.
  void (*ChipSelEn)(FS_NAND_CTRLR_DRV *p_drv);
  //                                                               Disable chip select.
  void (*ChipSelDis)(FS_NAND_CTRLR_DRV *p_drv);
  //                                                               Write cmd cycle(s).
  void (*CmdWr)(FS_NAND_CTRLR_DRV *p_drv,
                CPU_INT08U        *p_cmd,
                CPU_SIZE_T        cnt,
                RTOS_ERR          *p_err);
  //                                                               Write addr cycle(s).
  void (*AddrWr)(FS_NAND_CTRLR_DRV *p_drv,
                 CPU_INT08U        *p_addr,
                 CPU_SIZE_T        cnt,
                 RTOS_ERR          *p_err);
  //                                                               Write data cycle(s).
  void (*DataWr)(FS_NAND_CTRLR_DRV *p_drv,
                 void              *p_src,
                 CPU_SIZE_T        cnt,
                 CPU_INT08U        width,
                 RTOS_ERR          *p_err);
  //                                                               Read data.
  void (*DataRd)(FS_NAND_CTRLR_DRV *p_drv,
                 void              *p_dest,
                 CPU_SIZE_T        cnt,
                 CPU_INT08U        width,
                 RTOS_ERR          *p_err);
  //                                                               Wait until ready.
  void (*WaitWhileBusy)(FS_NAND_CTRLR_DRV           *p_drv,
                        void                        *poll_fcnt_arg,
                        FS_NAND_CTRLR_DRV_POLL_FNCT poll_fcnt,
                        CPU_INT32U                  to_us,
                        RTOS_ERR                    *p_err);
} FS_NAND_CTRLR_DRV_API;

/********************************************************************************************************
 *                           NAND MEMORY CONTROLLER DRIVER ISR HANDLER FUNCTION
 *******************************************************************************************************/

typedef void (*FS_NAND_CTRLR_DRV_ISR_HANDLE_FNCT)(FS_NAND_CTRLR_DRV *p_drv);

/********************************************************************************************************
 *                                   NAND MEMORY CONTROLLER BSP API
 *******************************************************************************************************/

typedef struct fs_nand_ctrlr_bsp_api {
  CPU_BOOLEAN (*Init)(FS_NAND_CTRLR_DRV_ISR_HANDLE_FNCT isr_fnct,
                      FS_NAND_CTRLR_DRV                 *p_drv);

  CPU_BOOLEAN (*ClkCfg)(void);

  CPU_BOOLEAN (*IO_Cfg)(void);

  CPU_BOOLEAN (*IntCfg)(void);

  CPU_BOOLEAN (*ChipSelEn)(CPU_INT16U part_slave_id);

  CPU_BOOLEAN (*ChipSelDis)(CPU_INT16U part_slave_id);

  CPU_BOOLEAN (*IsChipRdy)(void);
} FS_NAND_CTRLR_BSP_API;

/********************************************************************************************************
 *                                           SPARE SEGMENT INFO STRUCT
 *******************************************************************************************************/

typedef struct {
  FS_NAND_PG_SIZE PgOffset;                                     // Offset in pg of spare seg.
  FS_NAND_PG_SIZE Len;                                          // Len in bytes of spare seg.
} FS_NAND_CTRLR_GEN_SPARE_SEG_INFO;

/********************************************************************************************************
 *                           NAND GENERIC CONTROLLER STAT AND ERR CTRS STRUCT
 *******************************************************************************************************/

#if ((FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED) \
  || (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED))
typedef struct fs_nand_ctrlr_gen_ctrs {
#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // -------------------- STAT CTRS ---------------------
  CPU_INT32U StatRdCtr;                                         // Nbr of sec rd.
  CPU_INT32U StatWrCtr;                                         // Nbr of sec wr.
  CPU_INT32U StatEraseCtr;                                      // Nbr of blk erase.
  CPU_INT32U StatSpareRdRawCtr;                                 // Nbr of raw spare rd.
  CPU_INT32U StatOOSRdRawCtr;                                   // Nbr of raw OOS rd.
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // --------------------- ERR CTRS ---------------------
  CPU_INT16U ErrCorrECC_Ctr;                                    // Nbr of correctable ECC rd errs.
  CPU_INT16U ErrCriticalCorrECC_Ctr;                            // Nbr of critical correctable ECC rd errs.
  CPU_INT16U ErrUncorrECC_Ctr;                                  // Nbr of uncorrectable ECC rd errs.
  CPU_INT16U ErrWrCtr;                                          // Nbr of wr failures.
  CPU_INT16U ErrEraseCtr;                                       // Nbr of erase failures.
#endif
} FS_NAND_CTRLR_GEN_CTRS;
#endif

/********************************************************************************************************
 *                                           NAND CTRLR DATA STRUCT
 *******************************************************************************************************/

typedef CPU_INT08U FS_NAND_CTRLR_GEN_SPARE_ZONE_IX;

/********************************************************************************************************
 *                           NAND FLASH DEVICE GENERIC CONTROLLER EXT MOD DATA TYPE
 *******************************************************************************************************/

struct fs_nand_ctrlr_gen_ext_hw_info {
  const struct fs_nand_ctrlr_gen_ext_api *CtrlrGenExtApiPtr;
};

/****************************************************************************************************//**
 *                       NAND FLASH DEVICE GENERIC CONTROLLER CONFIGURATION DATA TYPE
 *
 * @note     (1) Certain NAND memory controllers support a functional mode called memory-mapped. In
 *               memory-mapped mode, the external flash memory is mapped to the microcontroller address
 *               space and is seen as an internal memory. This mode allows to access directly the external
 *               flash memory. When the external flash device is memory-mapped, an address range is
 *               dedicated to it besides the memory controller registers address range.
 *******************************************************************************************************/

typedef struct fs_nand_ctrlr_gen_hw_info {
  struct fs_nand_ctrlr_info                  CtrlrHwInfo;       // HW info for generic mem controller.
                                                                // HW info for generic mem controller extension.
  const struct fs_nand_ctrlr_gen_ext_hw_info *CtrlrGenExtHwInfoPtr;
  CPU_ADDR                                   BaseAddr;          // NAND mem controller base addr.
                                                                // NAND flash seen as MCU internal mem (see Note #1)
  CPU_INT32U                                 FlashMemMapStartAddr;
  const FS_NAND_CTRLR_DRV_API                *DrvApiPtr;        // Ptr to NAND mem controller driver API.
  const FS_NAND_CTRLR_BSP_API                *BspApiPtr;        // Ptr to NAND mem controller BSP API.
} FS_NAND_CTRLR_GEN_HW_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_NAND_CTRLR_API FS_NAND_CtrlrGen_API;

#if ((FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED) \
  || (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED))
extern CPU_INT16U             FS_NAND_CtrlrGen_UnitCtr;
extern FS_NAND_CTRLR_GEN_CTRS *FS_NAND_CtrlrGen_CtrsTbl[FS_NAND_CTRLR_GEN_CTRS_TBL_SIZE];
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NAND_CTRLR_GEN_HW_INFO_INIT(p_ctrlr_gen_ext_hw_info, p_drv_api, p_bsp_api, base_addr, mem_map_addr, align_req) \
  {                                                                                                                        \
    .CtrlrHwInfo.CtrlrApiPtr = &FS_NAND_CtrlrGen_API,                                                                      \
    .CtrlrHwInfo.AlignReq = align_req,                                                                                     \
    .CtrlrGenExtHwInfoPtr = &(p_ctrlr_gen_ext_hw_info)->CtrlrGenExtHwInfo,                                                 \
    .BaseAddr = base_addr,                                                                                                 \
    .FlashMemMapStartAddr = mem_map_addr,                                                                                  \
    .DrvApiPtr = p_drv_api,                                                                                                \
    .BspApiPtr = p_bsp_api                                                                                                 \
  };

/********************************************************************************************************
 ********************************************************************************************************
 *                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
