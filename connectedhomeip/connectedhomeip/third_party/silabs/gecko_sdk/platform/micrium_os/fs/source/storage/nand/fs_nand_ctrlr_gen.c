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

#include  <fs/include/fs_nand_ctrlr_gen.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/shared/crc/crc_utils.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/storage/nand/fs_nand_priv.h>
#include  <fs/source/storage/nand/fs_nand_ctrlr_gen_priv.h>

#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 *
 * Note(s) : (1) These #define's must be set to values larger than the specifications of the used device
 *               to avoid early timeout.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (FS, DRV, NAND)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_FS

#define  FS_NAND_CTRLR_GEN_MAX_EXT_MOD                 1u       // Max nbr of reg'd ext mods.
#define  FS_NAND_CTRLR_GEN_MAX_SPARE_ZONES           255u       // Max nbr of spare area zones.

#define  FS_NAND_CTRLR_GEN_ADDR_MAX_LEN                5u
#define  FS_NAND_CTRLR_GEN_ROW_ADDR_MAX_LEN            3u
#define  FS_NAND_CTRLR_GEN_COL_ADDR_MAX_LEN            4u

#define  FS_NAND_MAX_RD_us                           100u       // See note #1.
#define  FS_NAND_MAX_PGM_us                         1000u
#define  FS_NAND_MAX_BLK_ERASE_us                   5000u

#define  FS_NAND_CTRLR_ADDR_FMT_COL            DEF_BIT_00
#define  FS_NAND_CTRLR_ADDR_FMT_ROW            DEF_BIT_01
#define  FS_NAND_CTRLR_ADDR_FMT_SMALL_PG       DEF_BIT_02

/********************************************************************************************************
 *                                               COMMAND DEFINES
 *
 * Note(s): (1) See "Open NAND Flash Interface Specification, ONFI, Revision 3.0, 9-March-2011" for more
 *               details about the NAND commands set.
 *******************************************************************************************************/

//                                                                 ---------------- ONFI COMMANDS SET -----------------
//                                                                 See Note #1.
#define  FS_NAND_CMD_RDID                           0x90u
#define  FS_NAND_CMD_RESET                          0xFFu

#define  FS_NAND_CMD_RD_PARAM_PG                    0xECu

#define  FS_NAND_CMD_RD_SETUP                       0x00u
#define  FS_NAND_CMD_RD_SETUP_ZONE_B                0x01u
#define  FS_NAND_CMD_RD_SETUP_ZONE_C                0x50u
#define  FS_NAND_CMD_RD_CONFIRM                     0x30u

#define  FS_NAND_CMD_PAGEPGM_SETUP                  0x80u
#define  FS_NAND_CMD_PAGEPGM_CONFIRM                0x10u

#define  FS_NAND_CMD_BLKERASE_SETUP                 0x60u
#define  FS_NAND_CMD_BLKERASE_CONFIRM               0xD0u

#define  FS_NAND_CMD_CHNGWRCOL                      0x85u

#define  FS_NAND_CMD_CHNGRDCOL_SETUP                0x05u
#define  FS_NAND_CMD_CHNGRDCOL_CONFIRM              0xE0u

#define  FS_NAND_CMD_RDSTATUS                       0x70u

/********************************************************************************************************
 *                                       STATUS REGISTER BIT DEFINES
 *******************************************************************************************************/

#define  FS_NAND_SR_WRPROTECT              DEF_BIT_07
#define  FS_NAND_SR_RDY                    DEF_BIT_06
#define  FS_NAND_SR_CACHEPGMFAIL           DEF_BIT_01
#define  FS_NAND_SR_FAIL                   DEF_BIT_00

/********************************************************************************************************
 *                                           LOCAL DATA TYPES MAX VAL
 *******************************************************************************************************/

#define  FS_NAND_CTRLR_GEN_SPARE_ZONE_IX_MAX        255u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT08U FS_NAND_CTRLR_ADDR_FMT_FLAGS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CTRLR GENERIC API PROTOTYPES
 *******************************************************************************************************/

static FS_NAND_CTRLR *FS_NAND_CtrlrGen_Add(const FS_NAND_PM_ITEM *p_nand_pm_item,
                                           MEM_SEG               *p_seg,
                                           RTOS_ERR              *p_err);

static void FS_NAND_CtrlrGen_Open(FS_NAND_CTRLR *p_ctrlr,
                                  RTOS_ERR      *p_err);

//                                                                 Close NAND controller.
static void FS_NAND_CtrlrGen_Close(FS_NAND_CTRLR *p_ctrlr);

//                                                                 Get pointer to part data structure.
static FS_NAND_PART_PARAM *FS_NAND_CtrlrGen_PartInfoGet(FS_NAND_CTRLR *p_ctrlr);

//                                                                 Setup controller.
static FS_NAND_OOS_INFO FS_NAND_CtrlrGen_Setup(FS_NAND_CTRLR   *p_ctrlr,
                                               MEM_SEG         *p_seg,
                                               CPU_SIZE_T      spare_buf_align,
                                               FS_NAND_PG_SIZE sec_size,
                                               RTOS_ERR        *p_err);

//                                                                 --------------  NAND FLASH HIGH LVL OPS ------------
//                                                                 Read sector from NAND device.
static void FS_NAND_CtrlrGen_SecRd(FS_NAND_CTRLR *p_ctrlr,
                                   void          *p_dest,
                                   void          *p_dest_oos,
                                   FS_LB_NBR     sec_ix_phy,
                                   RTOS_ERR      *p_err);

//                                                                 Read out of sector data.
static void FS_NAND_CtrlrGen_OOSRdRaw(FS_NAND_CTRLR   *p_ctrlr,
                                      void            *p_dest_oos,
                                      FS_LB_NBR       sec_ix_phy,
                                      FS_NAND_PG_SIZE offset,
                                      FS_NAND_PG_SIZE len,
                                      RTOS_ERR        *p_err);

//                                                                 Read raw pg data.
static void FS_NAND_CtrlrGen_PgRdRaw(FS_NAND_CTRLR   *p_ctrlr_data_v,
                                     void            *p_dest,
                                     FS_LB_NBR       pg_ix_phy,
                                     FS_NAND_PG_SIZE offset,
                                     FS_NAND_PG_SIZE len,
                                     RTOS_ERR        *p_err);

//                                                                 Read spare area data.
static void FS_NAND_CtrlrGen_SpareRdRaw(FS_NAND_CTRLR   *p_ctrlr,
                                        void            *p_dest_spare,
                                        FS_LB_NBR       pg_ix_phy,
                                        FS_NAND_PG_SIZE offset,
                                        FS_NAND_PG_SIZE len,
                                        RTOS_ERR        *p_err);

//                                                                 Write sector to NAND device.
static void FS_NAND_CtrlrGen_SecWr(FS_NAND_CTRLR *p_ctrlr,
                                   void          *p_src,
                                   void          *p_src_oos,
                                   FS_LB_NBR     sec_ix_phy,
                                   RTOS_ERR      *p_err);

//                                                                 Erase block on NAND device.
static void FS_NAND_CtrlrGen_BlkErase(FS_NAND_CTRLR *p_ctrlr,
                                      CPU_INT32U    blk_ix_phy,
                                      RTOS_ERR      *p_err);

/********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

static void FS_NAND_CtrlrGen_OOS_Setup(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                       MEM_SEG           *p_seg,
                                       CPU_SIZE_T        spare_buf_align,
                                       CPU_INT08U        n_sec_per_pg,
                                       RTOS_ERR          *p_err);

static CPU_INT08U FS_NAND_CtrlrGen_StatusRd(FS_NAND_CTRLR_GEN *p_ctrlr_gen);

static CPU_BOOLEAN FS_NAND_CtrlrGen_PollFnct(void *p_arg);

static void FS_NAND_CtrlrGen_AddrFmt(FS_NAND_CTRLR_GEN            *p_ctrlr_gen,
                                     FS_LB_QTY                    pg_ix,
                                     FS_NAND_PG_SIZE              pg_offset,
                                     FS_NAND_CTRLR_ADDR_FMT_FLAGS addr_fmt_flags,
                                     CPU_INT08U                   *p_addr);

static void FS_NAND_CtrlrGen_SparePack(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                       void              *p_spare,
                                       CPU_INT08U        spare_seg_ix);

static void FS_NAND_CtrlrGen_SpareUnpack(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                         void              *p_spare,
                                         CPU_INT08U        spare_seg_ix);

static void FS_NAND_CtrlrGen_SpareSplit(void            *p_spare,
                                        FS_NAND_PG_SIZE buf_len,
                                        FS_NAND_PG_SIZE pos,
                                        FS_NAND_PG_SIZE len);

static void FS_NAND_CtrlrGen_SpareJoin(void            *p_spare,
                                       FS_NAND_PG_SIZE buf_len,
                                       FS_NAND_PG_SIZE pos,
                                       FS_NAND_PG_SIZE len);

static void FS_NAND_CtrlrGen_ParamPgRd(FS_NAND_CTRLR *p_ctrlr,
                                       CPU_INT16U    rel_addr,
                                       CPU_INT16U    byte_cnt,
                                       CPU_INT08U    *p_buf,
                                       RTOS_ERR      *p_err);

/********************************************************************************************************
 *                                           INTERFACE STRUCTURE
 *******************************************************************************************************/

const FS_NAND_CTRLR_API FS_NAND_CtrlrGen_API = {
  .Add = FS_NAND_CtrlrGen_Add,
  .Open = FS_NAND_CtrlrGen_Open,                                // Open NAND ctrlr.
  .Close = FS_NAND_CtrlrGen_Close,                              // Close NAND ctrlr.
  .PartInfoGet = FS_NAND_CtrlrGen_PartInfoGet,                  // Get part data ptr.
  .Setup = FS_NAND_CtrlrGen_Setup,                              // Setup NAND ctrlr.
  .SecRd = FS_NAND_CtrlrGen_SecRd,                              // Rd sec from NAND dev.
  .OOSRdRaw = FS_NAND_CtrlrGen_OOSRdRaw,                        // Rd out of sec data for specified sec without ECC.
  .SpareRdRaw = FS_NAND_CtrlrGen_SpareRdRaw,                    // Rd pg spare data from NAND dev without ECC.
  .SecWr = FS_NAND_CtrlrGen_SecWr,                              // Wr sec on NAND dev.
  .BlkErase = FS_NAND_CtrlrGen_BlkErase,                        // Erase blk on NAND dev.
  .ParamPgRd = FS_NAND_CtrlrGen_ParamPgRd,
  .PgRdRaw = FS_NAND_CtrlrGen_PgRdRaw,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           NAND CTRLR API FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlGen_Add()
 *
 * @brief    Add a generic NAND controller instance.
 *
 * @param    p_nand_pm_item  Pointer to a NAND controller platform manager item.
 *
 * @param    p_seg           Pointer to a memory segment where to allocate the controller and part data.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Handle to a generic NAND controller instance.
 *******************************************************************************************************/
static FS_NAND_CTRLR *FS_NAND_CtrlrGen_Add(const FS_NAND_PM_ITEM *p_nand_pm_item,
                                           MEM_SEG               *p_seg,
                                           RTOS_ERR              *p_err)
{
  const FS_NAND_HW_INFO           *p_nand_hw_info;
  FS_NAND_CTRLR_GEN               *p_ctrlr_gen;
  FS_NAND_CTRLR_GEN_HW_INFO       *p_ctrlr_gen_hw_info;
  FS_NAND_PART                    *p_part;
  const FS_NAND_PART_PARAM        *p_part_param;
  const FS_NAND_CTRLR_DRV_API     *p_drv;
  FS_LB_QTY                       nbr_of_pgs;
  FS_NAND_CTRLR_GEN_SPARE_ZONE_IX ix;
  FS_NAND_PG_SIZE                 free_start;
  FS_NAND_PG_SIZE                 free_end;
  FS_NAND_PG_SIZE                 free_len;
  FS_NAND_PG_SIZE                 spare_size;
  CPU_INT08U                      cmd;
  CPU_INT08U                      pg_offset_size;
  CPU_INT08U                      pg_addr_size;
  CPU_INT08U                      id[2];
  CPU_INT08U                      id_manuf;
#if LOG_ERR_IS_EN()
  CPU_INT08U id_dev;
#endif
  CPU_INT08U parity_manuf;
  CPU_INT08U addr;

  p_nand_hw_info = p_nand_pm_item->HwInfoPtr;
  p_ctrlr_gen_hw_info = (FS_NAND_CTRLR_GEN_HW_INFO *)p_nand_hw_info->CtrlrHwInfoPtr;

  RTOS_ASSERT_DBG_ERR_SET(p_ctrlr_gen_hw_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET(p_nand_hw_info->PartHwInfoPtr->FreeSpareMap != DEF_NULL, *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               -------------- GET ALLOC'D CTRLR DATA --------------
  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)Mem_SegAlloc("FS - NAND ctrlr gen data",
                                                  p_seg,
                                                  sizeof(FS_NAND_CTRLR_GEN),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_ctrlr_gen->CtrlrExtPtr = DEF_NULL;
  p_ctrlr_gen->CtrlrExtData = DEF_NULL;
  p_ctrlr_gen->PartDataPtr = DEF_NULL;
  p_ctrlr_gen->NextPtr = DEF_NULL;

  p_part_param = p_nand_hw_info->PartHwInfoPtr->PartParamPtr;
  p_ctrlr_gen->Ctrl.CtrlrApiPtr = p_nand_hw_info->CtrlrHwInfoPtr->CtrlrApiPtr;

  p_drv = p_ctrlr_gen_hw_info->DrvApiPtr;
  p_ctrlr_gen->Drv.HW_InfoPtr = p_ctrlr_gen_hw_info;            // Save gen ctrlr hw info for mem ctrlr drv usage.
                                                                // Slave ID used for NAND chip selection in BSP.
  p_ctrlr_gen->Drv.PartSlaveID = p_nand_hw_info->PartHwInfoPtr->PartSlaveID;

  //                                                               ------------------ INIT HARDWARE -------------------
  p_drv->Open(&p_ctrlr_gen->Drv,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               Reset dev.
  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);
  cmd = FS_NAND_CMD_RESET;
  FS_ERR_CHK_RTN(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
                 p_drv->ChipSelDis(&p_ctrlr_gen->Drv),
                 DEF_NULL);
  KAL_Dly(1u);
  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);

  //                                                               ------------------ OPEN EXTENSION ------------------
  p_ctrlr_gen->CtrlrExtPtr = p_ctrlr_gen_hw_info->CtrlrGenExtHwInfoPtr->CtrlrGenExtApiPtr;
  if (p_ctrlr_gen->CtrlrExtPtr->Open != DEF_NULL) {
    p_ctrlr_gen->CtrlrExtData = p_ctrlr_gen->CtrlrExtPtr->Open(p_ctrlr_gen,
                                                               p_ctrlr_gen_hw_info->CtrlrGenExtHwInfoPtr,
                                                               p_seg,
                                                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error opening ctrlr ext module."));
      return (DEF_NULL);
    }
  }

  //                                                               Read dev sig.
  cmd = FS_NAND_CMD_RDID;
  addr = 0u;

  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);
  FS_ERR_CHK_RTN(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
                 p_drv->ChipSelDis(&p_ctrlr_gen->Drv),
                 DEF_NULL);
  FS_ERR_CHK_RTN(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr, 1u, p_err),
                 p_drv->ChipSelDis(&p_ctrlr_gen->Drv),
                 DEF_NULL);
  FS_ERR_CHK_RTN(p_drv->DataRd(&p_ctrlr_gen->Drv, &id[0], sizeof(id), 8u, p_err),
                 p_drv->ChipSelDis(&p_ctrlr_gen->Drv),
                 DEF_NULL);
  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);

  p_drv->Close(&p_ctrlr_gen->Drv);

  id_manuf = id[0];
#if LOG_ERR_IS_EN()
  id_dev = id[1];
#endif

  parity_manuf = CRCUtil_PopCnt_32(id_manuf);                   // Calc parity of manuf id.
  parity_manuf &= DEF_BIT_00;

  if ((parity_manuf == 0u) || (id_manuf == 0x80)) {
    LOG_ERR(("Read invalid manufacturer id=", (x)id_manuf, " (dev id=", (x)id_dev, ")."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return (DEF_NULL);
  }

  LOG_VRB(("Found NAND manuf id=", (x)id_manuf, ", dev id=", (x)id_dev, "."));

  //                                                               -------------------- OPEN PART ---------------------
  if (p_part_param == DEF_NULL) {
    p_part = FS_NAND_PartONFI_Add(&p_ctrlr_gen->Ctrl,
                                  p_seg,
                                  p_err);
  } else {
    p_part = FS_NAND_PartStatic_Add(&p_ctrlr_gen->Ctrl,
                                    p_part_param,
                                    p_seg,
                                    p_err);
  }
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_ctrlr_gen->PartDataPtr = p_part;
  p_ctrlr_gen->FreeSpareMap = p_nand_hw_info->PartHwInfoPtr->FreeSpareMap;

  //                                                               ------------- VALIDATE PART PARAMETERS -------------
  free_end = 0u;
  spare_size = 0u;
  ix = 0u;

  while ((ix < FS_NAND_CTRLR_GEN_MAX_SPARE_ZONES) && (p_ctrlr_gen->FreeSpareMap[ix].OctetOffset != FS_NAND_PG_IX_INVALID)) {
    //                                                             Validate sections not overlapping or out of order.
    free_start = p_ctrlr_gen->FreeSpareMap[ix].OctetOffset;
    RTOS_ASSERT_DBG(free_start >= free_end, RTOS_ERR_INVALID_CFG, DEF_NULL);

    //                                                             Validate len is positive.
    free_len = p_ctrlr_gen->FreeSpareMap[ix].OctetLen;
    RTOS_ASSERT_DBG(free_len >= 1, RTOS_ERR_INVALID_CFG, DEF_NULL);

    //                                                             Validate sections do NOT overflow/wraparound.
    free_end = free_start + free_len;
    RTOS_ASSERT_DBG(free_start < free_end, RTOS_ERR_INVALID_CFG, DEF_NULL);

    //                                                             Accumulate tot avail spare size.
    spare_size += free_len;

    ix++;
  }

  //                                                               Validate that the available spare ...
  //                                                               ... is not greater than total spare.
  RTOS_ASSERT_DBG(spare_size <= p_part->Info.SpareSize,
                  RTOS_ERR_INVALID_CFG, DEF_NULL);

  p_ctrlr_gen->SpareTotalAvailSize = spare_size;
  p_ctrlr_gen->PartDataPtr = p_part;

  //                                                               -------------- CALC ADDRESSING CONSTS --------------
  //                                                               Calc nbr of octets req'd to addr pg offset (col).
  if (p_part->Info.PgSize != 512u) {                            // Large pg dev.
    pg_offset_size = FSUtil_Log2((CPU_INT32U)p_part->Info.PgSize);
    p_ctrlr_gen->ColAddrSize = (pg_offset_size >> DEF_OCTET_TO_BIT_SHIFT)
                               + ((pg_offset_size &  DEF_OCTET_TO_BIT_MASK) == 0u ? 0u : 1u);
  } else {                                                      // Small pg dev.
    p_ctrlr_gen->ColAddrSize = 1u;
  }

  //                                                               Calc nbr of octets req'd to store pg nbr (row).
  nbr_of_pgs = p_part->Info.BlkCnt * p_part->Info.PgPerBlk;

  pg_addr_size = FSUtil_Log2((CPU_INT32U)nbr_of_pgs);
  p_ctrlr_gen->RowAddrSize = (pg_addr_size >> DEF_OCTET_TO_BIT_SHIFT)
                             + ((pg_addr_size &  DEF_OCTET_TO_BIT_MASK) == 0u ? 0u : 1u);

  p_ctrlr_gen->AddrSize = p_ctrlr_gen->ColAddrSize + p_ctrlr_gen->RowAddrSize;

  //                                                               Validate addr sizes.
  RTOS_ASSERT_DBG(p_ctrlr_gen->ColAddrSize <= FS_NAND_CTRLR_GEN_COL_ADDR_MAX_LEN,
                  RTOS_ERR_INVALID_CFG, DEF_NULL);

  RTOS_ASSERT_DBG(p_ctrlr_gen->RowAddrSize <= FS_NAND_CTRLR_GEN_ROW_ADDR_MAX_LEN,
                  RTOS_ERR_INVALID_CFG, DEF_NULL);

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // ------------------ INIT STAT CTRS ------------------
  p_ctrlr_gen->Ctrs.StatRdCtr = 0u;
  p_ctrlr_gen->Ctrs.StatWrCtr = 0u;
  p_ctrlr_gen->Ctrs.StatEraseCtr = 0u;
  p_ctrlr_gen->Ctrs.StatSpareRdRawCtr = 0u;
  p_ctrlr_gen->Ctrs.StatOOSRdRawCtr = 0u;
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // ------------------ INIT ERR CTRS -------------------
  p_ctrlr_gen->Ctrs.ErrCorrECC_Ctr = 0u;
  p_ctrlr_gen->Ctrs.ErrCriticalCorrECC_Ctr = 0u;
  p_ctrlr_gen->Ctrs.ErrUncorrECC_Ctr = 0u;
  p_ctrlr_gen->Ctrs.ErrWrCtr = 0u;
  p_ctrlr_gen->Ctrs.ErrEraseCtr = 0u;
#endif

  return (&p_ctrlr_gen->Ctrl);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrGen_Open()
 *
 * @brief    Open a generic NAND controller instance.
 *
 * @param    p_ctrlr     Pointer to a NAND controller instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_Open(FS_NAND_CTRLR *p_ctrlr,
                                  RTOS_ERR      *p_err)
{
  FS_NAND_CTRLR_GEN *p_ctrlr_gen;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;

  p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr->Open(&p_ctrlr_gen->Drv,
                                               p_err);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrGen_Close()
 *
 * @brief    Close a NAND controller instance.
 *
 * @param    p_ctrlr     Pointer to a generic NAND controller instance.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_Close(FS_NAND_CTRLR *p_ctrlr)
{
  FS_NAND_CTRLR_GEN *p_ctrlr_gen;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;

  p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr->Close(&p_ctrlr_gen->Drv);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_PartInfoGet()
 *
 * @brief    Retrieve part data pointer from controller instance.
 *
 * @param    p_ctrlr     Pointer to a generic NAND controller instance.
 *
 * @return   Pointer to part data.
 *******************************************************************************************************/
static FS_NAND_PART_PARAM *FS_NAND_CtrlrGen_PartInfoGet(FS_NAND_CTRLR *p_ctrlr)
{
  FS_NAND_CTRLR_GEN *p_ctrlr_gen;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;

  return (&p_ctrlr_gen->PartDataPtr->Info);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrGen_Setup()
 *
 * @brief    Setup controller module according to chosen sector size.
 *
 * @param    p_ctrlr             Pointer to a NAND controller instance.
 *
 * @param    p_seg               Pointer to a memory segment.
 *
 * @param    sec_size            Sector size.
 *
 * @param    spare_buf_align     Spare area data buffer alignment.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Structure with size in octets of out of sector (OOS) area per sector and pointer to
 *           OOS buffer.
 *******************************************************************************************************/
static FS_NAND_OOS_INFO FS_NAND_CtrlrGen_Setup(FS_NAND_CTRLR   *p_ctrlr,
                                               MEM_SEG         *p_seg,
                                               CPU_SIZE_T      spare_buf_align,
                                               FS_NAND_PG_SIZE sec_size,
                                               RTOS_ERR        *p_err)
{
  FS_NAND_CTRLR_GEN *p_ctrlr_gen;
  FS_NAND_OOS_INFO  OOS_info;
  FS_NAND_PG_SIZE   rsvd_size;
  CPU_INT16S        OOS_size;
  CPU_INT08U        n_sec_per_pg;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;
  p_ctrlr_gen->SecSize = sec_size;
  p_ctrlr_gen->OOS_Offset = p_ctrlr_gen->FreeSpareMap[0].OctetOffset;

  OOS_info.BufPtr = DEF_NULL;
  OOS_info.Size = 0u;

  rsvd_size = 0u;                                               // ------------------ EXT MOD SETUP -------------------
  if (p_ctrlr_gen->CtrlrExtPtr->Setup != DEF_NULL) {
    rsvd_size = p_ctrlr_gen->CtrlrExtPtr->Setup(p_ctrlr_gen,
                                                p_ctrlr_gen->CtrlrExtData,
                                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (OOS_info);
    }
  }

  //                                                               -------------------- OOS SETUP ---------------------
  //                                                               Calc avail oos size per sec.
  n_sec_per_pg = p_ctrlr_gen->PartDataPtr->Info.PgSize / p_ctrlr_gen->SecSize;
  OOS_size = p_ctrlr_gen->SpareTotalAvailSize;
  OOS_size /= n_sec_per_pg;
  OOS_size -= rsvd_size;

  RTOS_ASSERT_DBG_ERR_SET(OOS_size >= 0, *p_err, RTOS_ERR_INVALID_CFG, OOS_info);

  p_ctrlr_gen->OOS_SizePerSec = (CPU_INT16U)OOS_size;

  FS_NAND_CtrlrGen_OOS_Setup(p_ctrlr_gen,                       // Setup spare segs.
                             p_seg,
                             spare_buf_align,
                             n_sec_per_pg,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (OOS_info);
  }

  OOS_info.Size = p_ctrlr_gen->OOS_SizePerSec;
  OOS_info.BufPtr = p_ctrlr_gen->SpareBufPtr;

  return (OOS_info);
}

/********************************************************************************************************
 *                                           NAND FLASH HIGH LVL OPS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrGen_SecRd()
 *
 * @brief    Read a physical sector from a NAND device & store data in buffer.
 *
 * @param    p_ctrlr     Pointer to a generic NAND controller instance.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    p_dest_oos  Pointer to destination spare buffer.
 *
 * @param    sec_ix_phy  Index of sector to read from.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The address is a 4- or 5-octet value, depending on the number of address bits necessary
 *               to specify the highest address on the NAND device. 1st and 2nd octets specify the column
 *               address, which specifies the location (in octets) within a page. 3rd, 4th and 5th octets
 *               specify the row address, which specifies the location of the page within the device.
 *               @verbatim
 *                       ----------------------------------------------------------------------         ---
 *                       | BUS CYCLE  | I/O7 | I/O6 | I/O5 | I/O4 | I/O3 | I/O2 | I/O1 | I/O0 |          |
 *                       ----------------------------------------------------------------------    Column Address
 *                       |    1st     |  A7  |  A6  |  A5  |  A4  |  A3  |  A2  |  A1  |  A0  | (octet within page)
 *                       ----------------------------------------------------------------------          |
 *                       |    2nd     |  --- |  --- |  --- |  --- |  A11 |  A10 |  A9  |  A8  |          |
 *                       ----------------------------------------------------------------------         ---
 *                       |    3rd     |  A19 |  A18 |  A17 |  A16 |  A15 |  A14 |  A13 |  A12 |          |
 *                       ----------------------------------------------------------------------     Row Address
 *                       |    4th     |  A27 |  A26 |  A25 |  A24 |  A23 |  A22 |  A21 |  A20 | (page within device)
 *                       ----------------------------------------------------------------------          |
 *                       |    5th     |  --- |  --- |  --- |  --- |  A31 |  A30 |  A29 |  A28 |          |
 *                       ----------------------------------------------------------------------         ---
 *               @endverbatim
 *               The 5th byte is only necessary for devices with more than 65536 pages.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SecRd(FS_NAND_CTRLR *p_ctrlr,
                                   void          *p_dest,
                                   void          *p_dest_oos,
                                   FS_LB_NBR     sec_ix_phy,
                                   RTOS_ERR      *p_err)
{
  FS_NAND_CTRLR_GEN            *p_ctrlr_gen;
  const FS_NAND_CTRLR_DRV_API  *p_drv;
  FS_NAND_PART                 *p_part;
  RTOS_ERR                     err_rtn;
  FS_NAND_CTRLR_ADDR_FMT_FLAGS addr_fmt_flags;
  FS_NAND_PG_SIZE              sec_size;
  CPU_INT32U                   nbr_sec_per_pg;
  CPU_INT32U                   sec_offset_pg;
  CPU_INT32U                   pg_size;
  CPU_INT32U                   row_addr;
  CPU_INT16U                   col_addr;
  CPU_INT08U                   addr[FS_NAND_CTRLR_GEN_ADDR_MAX_LEN];
  CPU_INT08U                   cmd1;
  CPU_INT08U                   cmd2;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;
  p_part = p_ctrlr_gen->PartDataPtr;
  p_drv = p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr;
  sec_size = p_ctrlr_gen->SecSize;
  pg_size = p_part->Info.PgSize;

  RTOS_ERR_SET(err_rtn, RTOS_ERR_NONE);

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_BLK_DEV_CTR_STAT_INC(p_ctrlr_gen->Ctrs.StatRdCtr);
#endif

  //                                                               -------------------- ADDR CALC ---------------------
  nbr_sec_per_pg = pg_size       / sec_size;
  sec_offset_pg = sec_ix_phy    % nbr_sec_per_pg;
  row_addr = sec_ix_phy    / nbr_sec_per_pg;
  col_addr = sec_offset_pg * sec_size;

  addr_fmt_flags = FS_NAND_CTRLR_ADDR_FMT_COL
                   | FS_NAND_CTRLR_ADDR_FMT_ROW
                   | (pg_size == 512u ? FS_NAND_CTRLR_ADDR_FMT_SMALL_PG : 0u);

  FS_NAND_CtrlrGen_AddrFmt(p_ctrlr_gen,
                           row_addr,
                           col_addr,
                           addr_fmt_flags,
                           &addr[0]);

  //                                                               -------------- EXEC CMD: RD SEC DATA ---------------
  cmd1 = FS_NAND_CMD_RD_SETUP;
  cmd2 = FS_NAND_CMD_RD_CONFIRM;

  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);

  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Sec addr.
  FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->AddrSize, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  if (pg_size != 512u) {
    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd2, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  }
  //                                                               Wait until rdy.
  p_drv->WaitWhileBusy(&p_ctrlr_gen->Drv,
                       p_ctrlr_gen,
                       FS_NAND_CtrlrGen_PollFnct,
                       FS_NAND_MAX_RD_us,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    LOG_DBG(("Timeout occurred when sending command."));
    return;
  }

  //                                                               Chk rd cmd status if needed.
  if (p_ctrlr_gen->CtrlrExtPtr->RdStatusChk != DEF_NULL) {
    p_ctrlr_gen->CtrlrExtPtr->RdStatusChk(p_ctrlr_gen->CtrlrExtData, p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_ECC_CRITICAL_CORR:
        RTOS_ERR_SET(err_rtn, RTOS_ERR_ECC_CRITICAL_CORR);
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        break;

      case RTOS_ERR_ECC_CORR:
        RTOS_ERR_SET(err_rtn, RTOS_ERR_ECC_CORR);
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        break;

      case RTOS_ERR_NONE:
        break;

      case RTOS_ERR_ECC_UNCORR:
#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
        FS_CTR_ERR_INC(p_ctrlr_gen->Ctrs.ErrUncorrECC_Ctr);
#endif
      default:
        p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
        return;                                                 // Prevent 'break NOT reachable' compiler warning.
    }
  }
  //                                                               Switch back to rd mode (poll_fcnt might have rd sta).
  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  //                                                               Rd data.
  FS_ERR_CHK(p_drv->DataRd(&p_ctrlr_gen->Drv, p_dest, p_ctrlr_gen->SecSize, p_part->Info.BusWidth, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  //                                                               ------------------ READ OOS DATA -------------------
  addr_fmt_flags = FS_NAND_CTRLR_ADDR_FMT_COL
                   | (pg_size == 512u ? FS_NAND_CTRLR_ADDR_FMT_SMALL_PG : 0u);

  FS_NAND_CtrlrGen_AddrFmt(p_ctrlr_gen,
                           row_addr,
                           p_ctrlr_gen->OOS_InfoTbl[sec_offset_pg].PgOffset,
                           addr_fmt_flags,
                           &addr[0]);

  if (pg_size != 512u) {
    cmd1 = FS_NAND_CMD_CHNGRDCOL_SETUP;
    cmd2 = FS_NAND_CMD_CHNGRDCOL_CONFIRM;

    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->ColAddrSize, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd2, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  } else {
    cmd1 = FS_NAND_CMD_RD_SETUP_ZONE_C;
    cmd2 = FS_NAND_CMD_RD_CONFIRM;

    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->AddrSize, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    //                                                             Wait until rdy.
    p_drv->WaitWhileBusy(&p_ctrlr_gen->Drv,
                         p_ctrlr_gen,
                         FS_NAND_CtrlrGen_PollFnct,
                         FS_NAND_MAX_RD_us,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
      LOG_DBG(("Timeout occurred when sending command."));
      return;
    }
    //                                                             Switch back to rd mode (poll_fcnt might have rd sta).
    cmd1 = FS_NAND_CMD_RD_SETUP;
    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  }

  //                                                               Rd data.
  FS_ERR_CHK(p_drv->DataRd(&p_ctrlr_gen->Drv, p_dest_oos, p_ctrlr_gen->OOS_InfoTbl[sec_offset_pg].Len, p_part->Info.BusWidth, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  FS_NAND_CtrlrGen_SpareUnpack(p_ctrlr_gen, p_dest_oos, sec_offset_pg);

  //                                                               -------------------- CHECK ECC ---------------------
  if (p_ctrlr_gen->CtrlrExtPtr->ECC_Verify != DEF_NULL) {
    p_ctrlr_gen->CtrlrExtPtr->ECC_Verify(p_ctrlr_gen->CtrlrExtData,
                                         p_dest,
                                         p_dest_oos,
                                         p_ctrlr_gen->OOS_SizePerSec,
                                         &err_rtn);
  }

  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
  *p_err = err_rtn;

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_ECC_UNCORR:
      FS_BLK_DEV_CTR_ERR_INC(p_ctrlr_gen->Ctrs.ErrUncorrECC_Ctr);
      break;

    case RTOS_ERR_ECC_CRITICAL_CORR:
      LOG_DBG(("Critical correctable ECC error on physical sector ", (u)sec_ix_phy, "."));
      FS_BLK_DEV_CTR_ERR_INC(p_ctrlr_gen->Ctrs.ErrCriticalCorrECC_Ctr);
      break;

    case RTOS_ERR_ECC_CORR:
      FS_BLK_DEV_CTR_ERR_INC(p_ctrlr_gen->Ctrs.ErrCorrECC_Ctr);
      break;

    default:
      break;
  }
#endif
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_OOSRdRaw()
 *
 * @brief    Read raw OOS without ECC check.
 *
 * @param    p_ctrlr     Pointer to NAND controller data.
 *
 * @param    p_dest_oos  Pointer to destination OOS (out of sector data) buffer.
 *
 * @param    sec_ix_phy  Sector index for which OOS will be read.
 *
 * @param    offset      Offset in the OOS area.
 *
 * @param    len         Number of octets to read.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_OOSRdRaw(FS_NAND_CTRLR   *p_ctrlr,
                                      void            *p_dest_oos,
                                      FS_LB_NBR       sec_ix_phy,
                                      FS_NAND_PG_SIZE offset,
                                      FS_NAND_PG_SIZE len,
                                      RTOS_ERR        *p_err)
{
  FS_NAND_CTRLR_GEN            *p_ctrlr_gen;
  FS_NAND_PART                 *p_part;
  const FS_NAND_CTRLR_DRV_API  *p_drv;
  CPU_INT08U                   *p_spare_08;
  FS_NAND_CTRLR_ADDR_FMT_FLAGS addr_fmt_flags;
  CPU_INT32U                   row_addr;
  CPU_INT08U                   addr[FS_NAND_CTRLR_GEN_ADDR_MAX_LEN];
  CPU_INT08U                   nbr_sec_per_pg;
  CPU_INT08U                   sec_offset_pg;
  CPU_INT08U                   cmd;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;
  p_part = p_ctrlr_gen->PartDataPtr;
  p_drv = p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr;

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_BLK_DEV_CTR_STAT_INC(p_ctrlr_gen->Ctrs.StatOOSRdRawCtr);
#endif

  //                                                               --------------- ADDRESS CALCULATION ----------------
  nbr_sec_per_pg = p_part->Info.PgSize / p_ctrlr_gen->SecSize;
  sec_offset_pg = sec_ix_phy          % nbr_sec_per_pg;
  row_addr = sec_ix_phy          / nbr_sec_per_pg;

  addr_fmt_flags = FS_NAND_CTRLR_ADDR_FMT_COL
                   | FS_NAND_CTRLR_ADDR_FMT_ROW
                   | (p_ctrlr_gen->PartDataPtr->Info.PgSize == 512u ? FS_NAND_CTRLR_ADDR_FMT_SMALL_PG : 0u);

  FS_NAND_CtrlrGen_AddrFmt(p_ctrlr_gen,
                           row_addr,
                           p_ctrlr_gen->OOS_InfoTbl[sec_offset_pg].PgOffset,
                           addr_fmt_flags,
                           &addr[0]);

  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);

  if (p_ctrlr_gen->PartDataPtr->Info.PgSize == 512u) {
    cmd = FS_NAND_CMD_RD_SETUP_ZONE_C;
  } else {
    cmd = FS_NAND_CMD_RD_SETUP;
  }

  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->AddrSize, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  if (p_ctrlr_gen->PartDataPtr->Info.PgSize != 512u) {
    cmd = FS_NAND_CMD_RD_CONFIRM;
    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  }
  //                                                               Wait until ready.
  p_drv->WaitWhileBusy(&p_ctrlr_gen->Drv,
                       p_ctrlr_gen,
                       FS_NAND_CtrlrGen_PollFnct,
                       FS_NAND_MAX_RD_us,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    return;
  }

  //                                                               Return to rd mode.
  cmd = FS_NAND_CMD_RD_SETUP;
  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  p_spare_08 = (CPU_INT08U *)p_ctrlr_gen->SpareBufPtr;

  FS_ERR_CHK(p_drv->DataRd(&p_ctrlr_gen->Drv, p_spare_08, p_ctrlr_gen->OOS_InfoTbl[sec_offset_pg].Len, p_part->Info.BusWidth, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  FS_NAND_CtrlrGen_SpareUnpack(p_ctrlr_gen,
                               p_spare_08,
                               sec_offset_pg);

  p_spare_08 += offset;

  Mem_Copy(p_dest_oos, p_spare_08, len);

  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SpareRdRaw()
 *
 * @brief    Read raw spare data without ECC check.
 *
 * @param    p_ctrlr         Pointer to a generic NAND controller instance.
 *
 * @param    p_dest_spare    Pointer to destination spare area data buffer.
 *
 * @param    pg_ix_phy       Page index for which spare data will be read.
 *
 * @param    offset          Offset in the spare area.
 *
 * @param    len             Number of octets to read.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SpareRdRaw(FS_NAND_CTRLR   *p_ctrlr,
                                        void            *p_dest_spare,
                                        FS_LB_NBR       pg_ix_phy,
                                        FS_NAND_PG_SIZE offset,
                                        FS_NAND_PG_SIZE len,
                                        RTOS_ERR        *p_err)
{
  FS_NAND_CTRLR_GEN *p_ctrlr_gen;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;

  FS_NAND_CtrlrGen_PgRdRaw(p_ctrlr,
                           p_dest_spare,
                           pg_ix_phy,
                           offset + p_ctrlr_gen->PartDataPtr->Info.PgSize,
                           len,
                           p_err);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_PgRdRaw()
 *
 * @brief    Read raw page data without ECC check.
 *
 * @param    p_nand_ctrlr    Pointer to NAND controller data.
 *
 * @param    p_dest          Pointer to destination data  buffer.
 *
 * @param    pg_ix_phy       Page index for which page data will be read.
 *
 * @param    offset          Offset in the page.
 *
 * @param    len             Number of octets to read.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_PgRdRaw(FS_NAND_CTRLR   *p_nand_ctrlr,
                                     void            *p_dest,
                                     FS_LB_NBR       pg_ix_phy,
                                     FS_NAND_PG_SIZE offset,
                                     FS_NAND_PG_SIZE len,
                                     RTOS_ERR        *p_err)
{
  FS_NAND_CTRLR_GEN            *p_ctrlr_gen;
  FS_NAND_PART                 *p_part;
  const FS_NAND_CTRLR_DRV_API  *p_drv;
  FS_NAND_CTRLR_ADDR_FMT_FLAGS addr_fmt_flags;
  CPU_INT08U                   addr[FS_NAND_CTRLR_GEN_ADDR_MAX_LEN];
  CPU_INT08U                   cmd;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_nand_ctrlr;
  p_part = p_ctrlr_gen->PartDataPtr;
  p_drv = p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr;

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_BLK_DEV_CTR_STAT_INC(p_ctrlr_gen->Ctrs.StatSpareRdRawCtr);
#endif

  //                                                               --------------- ADDRESS CALCULATION ----------------
  addr_fmt_flags = FS_NAND_CTRLR_ADDR_FMT_COL
                   | FS_NAND_CTRLR_ADDR_FMT_ROW
                   | (p_ctrlr_gen->PartDataPtr->Info.PgSize == 512u ? FS_NAND_CTRLR_ADDR_FMT_SMALL_PG : 0u);

  FS_NAND_CtrlrGen_AddrFmt(p_ctrlr_gen,
                           pg_ix_phy,
                           offset,
                           addr_fmt_flags,
                           &addr[0]);

  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);

  if (p_ctrlr_gen->PartDataPtr->Info.PgSize == 512u) {          // Small pg dev.
    if (offset >= 512u) {
      cmd = FS_NAND_CMD_RD_SETUP_ZONE_C;
    } else if (offset >= 256u) {
      cmd = FS_NAND_CMD_RD_SETUP_ZONE_B;
    } else {
      cmd = FS_NAND_CMD_RD_SETUP;                               // Zone A.
    }
  } else {                                                      // Large pg dev.
    cmd = FS_NAND_CMD_RD_SETUP;
  }

  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->AddrSize, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  if (p_ctrlr_gen->PartDataPtr->Info.PgSize != 512u) {
    cmd = FS_NAND_CMD_RD_CONFIRM;
    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  }
  //                                                               Wait until ready.
  p_drv->WaitWhileBusy(&p_ctrlr_gen->Drv,
                       p_ctrlr_gen,
                       FS_NAND_CtrlrGen_PollFnct,
                       FS_NAND_MAX_RD_us,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    return;
  }

  cmd = FS_NAND_CMD_RD_SETUP;
  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  FS_ERR_CHK(p_drv->DataRd(&p_ctrlr_gen->Drv, p_dest, len, p_part->Info.BusWidth, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrGen_SecWr()
 *
 * @brief    Write data to NAND device sector from a buffer.
 *
 * @param    p_ctrlr     Pointer to a generic NAND controller instance.
 *
 * @param    p_src       Pointer to source data buffer.
 *
 * @param    p_src_oos   Pointer to source OOS data buffer.
 *
 * @param    sec_ix_phy  Sector index which will be written.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SecWr(FS_NAND_CTRLR *p_ctrlr,
                                   void          *p_src,
                                   void          *p_src_oos,
                                   FS_LB_NBR     sec_ix_phy,
                                   RTOS_ERR      *p_err)
{
  FS_NAND_CTRLR_GEN            *p_ctrlr_gen;
  const FS_NAND_CTRLR_DRV_API  *p_drv;
  FS_NAND_PART                 *p_part;
  FS_NAND_PG_SIZE              ix;
  FS_NAND_CTRLR_ADDR_FMT_FLAGS addr_fmt_flags;
  FS_NAND_PG_SIZE              col_addr;
  FS_NAND_PG_SIZE              sec_size;
  CPU_INT32U                   nbr_sec_per_pg;
  CPU_INT32U                   sec_offset_pg;
  CPU_INT32U                   pg_size;
  CPU_INT32U                   row_addr;
  CPU_INT08U                   addr[FS_NAND_CTRLR_GEN_ADDR_MAX_LEN];
  CPU_INT08U                   cmd1;
  CPU_INT08U                   cmd2;
  CPU_INT08U                   sr;
  CPU_INT08U                   val;
  CPU_BOOLEAN                  sr_fail;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;
  p_part = p_ctrlr_gen->PartDataPtr;
  p_drv = p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr;
  sec_size = p_ctrlr_gen->SecSize;
  pg_size = p_part->Info.PgSize;

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_BLK_DEV_CTR_STAT_INC(p_ctrlr_gen->Ctrs.StatWrCtr);
#endif

  //                                                               --------------- ADDRESS CALCULATION ----------------
  nbr_sec_per_pg = pg_size       / sec_size;
  sec_offset_pg = sec_ix_phy    % nbr_sec_per_pg;
  row_addr = sec_ix_phy    / nbr_sec_per_pg;
  col_addr = sec_offset_pg * sec_size;

  addr_fmt_flags = FS_NAND_CTRLR_ADDR_FMT_COL
                   | FS_NAND_CTRLR_ADDR_FMT_ROW
                   | (pg_size == 512u ? FS_NAND_CTRLR_ADDR_FMT_SMALL_PG : 0u);

  FS_NAND_CtrlrGen_AddrFmt(p_ctrlr_gen,
                           row_addr,
                           col_addr,
                           addr_fmt_flags,
                           &addr[0]);

  //                                                               -------------------- WR SRC BUF --------------------
  cmd1 = FS_NAND_CMD_PAGEPGM_SETUP;
  cmd2 = FS_NAND_CMD_PAGEPGM_CONFIRM;

  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);

  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Sec addr.
  FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->AddrSize, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Wr sec data.
  FS_ERR_CHK(p_drv->DataWr(&p_ctrlr_gen->Drv, p_src, p_ctrlr_gen->SecSize, p_part->Info.BusWidth, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));

  //                                                               --------------------- ECC CALC ---------------------
  if (p_ctrlr_gen->CtrlrExtPtr->ECC_Calc != DEF_NULL) {
    p_ctrlr_gen->CtrlrExtPtr->ECC_Calc(p_ctrlr_gen->CtrlrExtData,
                                       p_src,
                                       p_src_oos,
                                       p_ctrlr_gen->OOS_SizePerSec,
                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
      return;
    }
  }

  //                                                               ----------- PACK AND WR OOS IN SPARE REG -----------
  FS_NAND_CtrlrGen_SparePack(p_ctrlr_gen,
                             p_src_oos,
                             sec_offset_pg);

  col_addr = p_ctrlr_gen->OOS_InfoTbl[sec_offset_pg].PgOffset;

  if (pg_size != 512u) {                                        // Large pg dev.
    addr_fmt_flags = FS_NAND_CTRLR_ADDR_FMT_COL;

    FS_NAND_CtrlrGen_AddrFmt(p_ctrlr_gen,
                             row_addr,
                             col_addr,
                             addr_fmt_flags,
                             &addr[0]);

    cmd1 = FS_NAND_CMD_CHNGWRCOL;

    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->ColAddrSize, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  } else {                                                      // Small pg dev.
    val = DEF_OCTET_MASK;
    for (ix = 0u; ix < col_addr - sec_size; ix++) {             // Wr 0xFF until OOS offset.
      FS_ERR_CHK(p_drv->DataWr(&p_ctrlr_gen->Drv, &val, 1u, p_part->Info.BusWidth, p_err),
                 p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    }
  }

  FS_ERR_CHK(p_drv->DataWr(&p_ctrlr_gen->Drv, p_src_oos, p_ctrlr_gen->OOS_InfoTbl[sec_offset_pg].Len,
                           p_part->Info.BusWidth, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Confirm pgrm op.
  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd2, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Wait until ready.
  p_drv->WaitWhileBusy(&p_ctrlr_gen->Drv,
                       p_ctrlr_gen,
                       FS_NAND_CtrlrGen_PollFnct,
                       FS_NAND_MAX_PGM_us,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    return;
  }

  //                                                               ------------------ CHK OP STATUS -------------------
  sr = FS_NAND_CtrlrGen_StatusRd(p_ctrlr_gen);
  sr_fail = DEF_BIT_IS_SET(sr, FS_NAND_SR_FAIL);
  if (sr_fail != DEF_NO) {                                      // Check if FAIL bit is set.
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
    FS_BLK_DEV_CTR_ERR_INC(p_ctrlr_gen->Ctrs.ErrWrCtr);
#endif
    return;
  }

  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_BlkErase()
 *
 * @brief    Erase block of NAND device.
 *
 * @param    p_ctrlr     Pointer to a generic NAND controller instance.
 *
 * @param    blk_ix_phy  Block index that will be erased.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Can be called directly by user through I/O Ctrl. Care must be taken NOT to erase bad
 *               blocks because the factory defect mark will be lost.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_BlkErase(FS_NAND_CTRLR *p_ctrlr,
                                      CPU_INT32U    blk_ix_phy,
                                      RTOS_ERR      *p_err)
{
  FS_NAND_CTRLR_GEN            *p_ctrlr_gen;
  const FS_NAND_CTRLR_DRV_API  *p_drv;
  FS_LB_QTY                    nbr_sec_per_blk;
  FS_LB_QTY                    sec_ix_phy;
  FS_NAND_CTRLR_ADDR_FMT_FLAGS addr_fmt_flags;
  CPU_INT32U                   row_addr;
  CPU_INT08U                   addr[FS_NAND_CTRLR_GEN_ROW_ADDR_MAX_LEN];
  CPU_INT08U                   cmd1;
  CPU_INT08U                   cmd2;
  CPU_INT08U                   nbr_sec_per_pg;
  CPU_INT08U                   sr;
  CPU_BOOLEAN                  sr_fail;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;
  p_drv = p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr;

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_BLK_DEV_CTR_STAT_INC(p_ctrlr_gen->Ctrs.StatEraseCtr);
#endif

  //                                                               -------------------- CALC ADDR ---------------------
  nbr_sec_per_pg = p_ctrlr_gen->PartDataPtr->Info.PgSize   / p_ctrlr_gen->SecSize;
  nbr_sec_per_blk = p_ctrlr_gen->PartDataPtr->Info.PgPerBlk * nbr_sec_per_pg;
  sec_ix_phy = blk_ix_phy * nbr_sec_per_blk;
  row_addr = sec_ix_phy / nbr_sec_per_pg;

  addr_fmt_flags = FS_NAND_CTRLR_ADDR_FMT_ROW
                   | (p_ctrlr_gen->PartDataPtr->Info.PgSize == 512u ? FS_NAND_CTRLR_ADDR_FMT_SMALL_PG : 0u);

  FS_NAND_CtrlrGen_AddrFmt(p_ctrlr_gen,
                           row_addr,
                           0u,
                           addr_fmt_flags,
                           &addr[0]);

  //                                                               --------------------- EXEC CMD ---------------------
  cmd1 = FS_NAND_CMD_BLKERASE_SETUP;
  cmd2 = FS_NAND_CMD_BLKERASE_CONFIRM;

  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);
  //                                                               Write blk erase cmd.
  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Write addr.
  FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], p_ctrlr_gen->RowAddrSize, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Write blk erase confirm cmd.
  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd2, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Wait until ready.
  p_drv->WaitWhileBusy(&p_ctrlr_gen->Drv,
                       p_ctrlr_gen,
                       FS_NAND_CtrlrGen_PollFnct,
                       FS_NAND_MAX_BLK_ERASE_us,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    return;
  }
  //                                                               ------------------ CHK OP STATUS -------------------
  sr = FS_NAND_CtrlrGen_StatusRd(p_ctrlr_gen);
  sr_fail = DEF_BIT_IS_SET(sr, FS_NAND_SR_FAIL);
  if (sr_fail != DEF_NO) {                                      // Check if FAIL bit is set.
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
    FS_BLK_DEV_CTR_ERR_INC(p_ctrlr_gen->Ctrs.ErrEraseCtr);
#endif
    return;
  }

  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_OOS_Setup()
 *
 * @brief    Setup out of sector area. Creates OOS to spare mapping and allocates buffer to use for
 *           spare area access.
 *
 * @param    p_ctrlr_gen         Pointer to a generic NAND controller instance.
 *
 * @param    p_seg               Pointer to a memory segment where to allocate the controller OOS data.
 *
 * @param    spare_buf_align     Spare area data buffer alignment.
 *
 * @param    n_sec_per_pg        Number of sectors per NAND page.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_OOS_Setup(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                       MEM_SEG           *p_seg,
                                       CPU_SIZE_T        spare_buf_align,
                                       CPU_INT08U        n_sec_per_pg,
                                       RTOS_ERR          *p_err)
{
  const FS_NAND_FREE_SPARE_DATA *free_spare_map;
  CPU_SIZE_T                    octets_reqd;
  CPU_INT08U                    OOS_ix;
  FS_NAND_PG_SIZE               cur_pg_offset;
  FS_NAND_PG_SIZE               spare_seg_max_len;
  FS_NAND_PG_SIZE               nxt_spare_pos;
  FS_NAND_PG_SIZE               spare_size_per_sec;
  CPU_INT08U                    spare_zone_ix;
  FS_NAND_PG_SIZE               spare_zone_free_space_rem;
  FS_NAND_PG_SIZE               OOS_size_rem;

  //                                                               ----------- ALLOC OOS SEGMENTS INFO TBL ------------
  p_ctrlr_gen->OOS_InfoTbl = (FS_NAND_CTRLR_GEN_SPARE_SEG_INFO *)Mem_SegAlloc("FS - NAND OOS Info tbl",
                                                                              p_seg,
                                                                              n_sec_per_pg * sizeof(FS_NAND_CTRLR_GEN_SPARE_SEG_INFO),
                                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------- CALC OOS SEGMENTS INFO --------------
  spare_size_per_sec = p_ctrlr_gen->SpareTotalAvailSize / n_sec_per_pg;

  free_spare_map = p_ctrlr_gen->FreeSpareMap;

  spare_zone_ix = 0u;
  spare_zone_free_space_rem = free_spare_map[0].OctetLen;
  spare_seg_max_len = 0u;

  cur_pg_offset = p_ctrlr_gen->PartDataPtr->Info.PgSize;        // First OOS begins at start of spare area.
  cur_pg_offset += free_spare_map[0].OctetOffset;
  for (OOS_ix = 0u; OOS_ix < n_sec_per_pg; OOS_ix++) {
    p_ctrlr_gen->OOS_InfoTbl[OOS_ix].PgOffset = cur_pg_offset;

    if ((p_ctrlr_gen->PartDataPtr->Info.BusWidth == 16u)        // Make sure seg doesn't start at invalid ix.
        && (DEF_BIT_IS_SET(cur_pg_offset, DEF_BIT_00) == DEF_YES)) {
      LOG_ERR(("OOS segment ", (u)OOS_ix, " starts at odd index ", (u)cur_pg_offset, ": need to fix FreeSpareMap."));
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
    }

    p_ctrlr_gen->OOS_InfoTbl[OOS_ix].Len = 0u;

    OOS_size_rem = spare_size_per_sec;
    while (OOS_size_rem != 0u) {
      if (spare_zone_free_space_rem >= OOS_size_rem) {          // Cur spare zone has enough free space.
        spare_zone_free_space_rem -= OOS_size_rem;
        p_ctrlr_gen->OOS_InfoTbl[OOS_ix].Len += OOS_size_rem;
        cur_pg_offset += OOS_size_rem;
        OOS_size_rem = 0u;
      } else {                                                  // Cur spare zone does not have enough free space.
                                                                // Use remainder of spare zone.
        p_ctrlr_gen->OOS_InfoTbl[OOS_ix].Len += spare_zone_free_space_rem;
        OOS_size_rem -= spare_zone_free_space_rem;
        cur_pg_offset += spare_zone_free_space_rem;
        spare_zone_free_space_rem = 0u;
      }

      if (spare_zone_free_space_rem == 0u) {                    // Switch to nxt spare zone.
        nxt_spare_pos = free_spare_map[spare_zone_ix].OctetOffset + free_spare_map[spare_zone_ix].OctetLen;
        spare_zone_ix++;
        spare_zone_free_space_rem = free_spare_map[spare_zone_ix].OctetLen;
        cur_pg_offset += free_spare_map[spare_zone_ix].OctetOffset - nxt_spare_pos;

        if (OOS_size_rem != 0u) {
          p_ctrlr_gen->OOS_InfoTbl[OOS_ix].Len += free_spare_map[spare_zone_ix].OctetOffset - nxt_spare_pos;
        }
      }
    }

    if (p_ctrlr_gen->OOS_InfoTbl[OOS_ix].Len > spare_seg_max_len) {
      spare_seg_max_len = p_ctrlr_gen->OOS_InfoTbl[OOS_ix].Len;
    }
  }

  p_ctrlr_gen->SpareBufSize = spare_seg_max_len;

  //                                                               ----------------- ALLOC SPARE BUF ------------------
  p_ctrlr_gen->SpareBufPtr = Mem_SegAllocExt("FS - NAND spare buf",
                                             p_seg,
                                             spare_seg_max_len,
                                             spare_buf_align,
                                             &octets_reqd,
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Could not alloc mem for OOS data buffer: ", (u)octets_reqd, " octets required."));
    return;
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_StatusRd()
 *
 * @brief    Read NAND status register.
 *
 * @param    p_ctrlr_gen     Pointer to a generic NAND controller instance.
 *
 * @return   Status register value.
 *******************************************************************************************************/
static CPU_INT08U FS_NAND_CtrlrGen_StatusRd(FS_NAND_CTRLR_GEN *p_ctrlr_gen)
{
  CPU_INT08U cmd;
  CPU_INT08U sr;
  RTOS_ERR   err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  cmd = FS_NAND_CMD_RDSTATUS;
  p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr->CmdWr(&p_ctrlr_gen->Drv, &cmd, 1u, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (FS_NAND_SR_FAIL);
  }
  p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr->DataRd(&p_ctrlr_gen->Drv, (void *)&sr, 1u, 8u, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (FS_NAND_SR_FAIL);
  }

  return (sr);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_PollFnct()
 *
 * @brief    Poll function.
 *
 * @param    p_arg   Argument passed to wait until busy callback.
 *
 * @return   DEF_YES, if operation is complete or error occurred.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_CtrlrGen_PollFnct(void *p_arg)
{
  FS_NAND_CTRLR_GEN *p_ctrlr_gen;
  CPU_INT08U        sr;
  CPU_BOOLEAN       rdy;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_arg;

  sr = FS_NAND_CtrlrGen_StatusRd(p_ctrlr_gen);
  rdy = DEF_BIT_IS_SET(sr, FS_NAND_SR_RDY);

  return (rdy);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_AddrFmt()
 *
 * @brief    Format address according to specified flags.
 *
 * @param    p_ctrlr_gen     Pointer to a generic NAND controller instance.
 *
 * @param    pg_ix           Page index. Use first page index of block for block-related commands or 0
 *                           when the row address is not required.
 *
 * @param    pg_offset       Page offset in octets. Use 0 if column address is not required.
 *
 * @param    addr_fmt_flags  Flags that control how address is generated:
 *                               - FS_NAND_CTRLR_ADDR_FMT_COL          Include column address.
 *                               - FS_NAND_CTRLR_ADDR_FMT_ROW          Include row address.
 *                               - FS_NAND_CTRLR_ADDR_FMT_SMALL_PG     Use small-page device addressing.
 *
 * @param    p_addr          Pointer to array that will receive the formatted address.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_AddrFmt(FS_NAND_CTRLR_GEN            *p_ctrlr_gen,
                                     FS_LB_QTY                    pg_ix,
                                     FS_NAND_PG_SIZE              pg_offset,
                                     FS_NAND_CTRLR_ADDR_FMT_FLAGS addr_fmt_flags,
                                     CPU_INT08U                   *p_addr)
{
  CPU_INT08U      addr_ix;
  CPU_INT08U      ix;
  CPU_INT08U      bus_width;
  FS_NAND_PG_SIZE col_addr;

  bus_width = p_ctrlr_gen->PartDataPtr->Info.BusWidth;
  col_addr = pg_offset;

  addr_ix = 0u;
  //                                                               --------------------- COL ADDR ---------------------
  if (DEF_BIT_IS_SET(addr_fmt_flags, FS_NAND_CTRLR_ADDR_FMT_COL) == DEF_YES) {
    if (DEF_BIT_IS_SET(addr_fmt_flags, FS_NAND_CTRLR_ADDR_FMT_SMALL_PG) == DEF_YES) {
      //                                                           Small pg dev.
      if (col_addr >= 512u) {                                   // Zone C.
        col_addr -= 512u;
      } else {
        if ((col_addr >= 256u)                                  // Zone B.
            && (bus_width != 16u)) {
          col_addr -= 256u;
        }
      }
    }

    if (bus_width == 16u) {
      col_addr >>= 1u;
    }

    for (ix = 0u; ix < p_ctrlr_gen->ColAddrSize; ix++) {
      p_addr[addr_ix] = (col_addr >> (DEF_OCTET_NBR_BITS * ix)) & DEF_OCTET_MASK;
      addr_ix++;
    }
  }

  //                                                               --------------------- ROW ADDR ---------------------
  if (DEF_BIT_IS_SET(addr_fmt_flags, FS_NAND_CTRLR_ADDR_FMT_ROW) == DEF_YES) {
    for (ix = 0u; ix < p_ctrlr_gen->RowAddrSize; ix++) {
      p_addr[addr_ix] = (pg_ix >> (DEF_OCTET_NBR_BITS * ix)) & DEF_OCTET_MASK;
      addr_ix++;
    }
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SparePack()
 *
 * @brief    Pack spare data for specified segment according to spare free map.
 *
 * @param    p_ctrlr_gen     Pointer to a buffer containing the spare data.
 *
 * @param    p_spare         Pointer to spare buffer.
 *
 * @param    spare_seg_ix    Index of spare segment.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SparePack(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                       void              *p_spare,
                                       CPU_INT08U        spare_seg_ix)
{
  const FS_NAND_FREE_SPARE_DATA    *free_spare_map;
  FS_NAND_CTRLR_GEN_SPARE_SEG_INFO spare_seg_info;
  FS_NAND_PG_SIZE                  spare_seg_start;
  FS_NAND_PG_SIZE                  spare_seg_end;
  FS_NAND_PG_SIZE                  spare_pos;
  FS_NAND_PG_SIZE                  spare_notch_pos;
  FS_NAND_PG_SIZE                  spare_notch_len;
  CPU_INT08U                       spare_zone_ix;

  free_spare_map = p_ctrlr_gen->FreeSpareMap;

  spare_seg_info = p_ctrlr_gen->OOS_InfoTbl[spare_seg_ix];
  spare_seg_start = spare_seg_info.PgOffset - p_ctrlr_gen->PartDataPtr->Info.PgSize;
  spare_seg_end = spare_seg_start + spare_seg_info.Len;
  spare_pos = spare_seg_start;
  spare_zone_ix = 0u;

  spare_notch_len = free_spare_map[0].OctetOffset;
  spare_notch_pos = 0u;

  while (spare_pos < spare_seg_end) {
    if (spare_notch_pos >= spare_pos) {                         // Notch after cur pos ...
      if (spare_notch_pos < spare_seg_end) {                    // ... and in cur seg.
        spare_pos += spare_notch_pos - spare_pos;
        FS_NAND_CtrlrGen_SpareSplit(p_spare,
                                    spare_seg_info.Len,
                                    spare_pos - spare_seg_start,
                                    spare_notch_len);
        spare_pos += spare_notch_len;
      } else {                                                  // ... and after cur seg.
        spare_pos = spare_seg_end;                              // Done.
      }
    }

    //                                                             Continue with nxt notch.
    spare_notch_pos = free_spare_map[spare_zone_ix].OctetOffset
                      + free_spare_map[spare_zone_ix].OctetLen;
    spare_zone_ix++;
    if (free_spare_map[spare_zone_ix].OctetOffset == FS_NAND_PG_IX_INVALID) {
      spare_pos = spare_seg_end;                                // No more spare notches... done.
    } else {
      spare_notch_len = free_spare_map[spare_zone_ix].OctetOffset - spare_notch_pos;
    }
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SpareUnpack()
 *
 * @brief    Unpack spare data for specified segment according to spare free map.
 *
 * @param    p_ctrlr_gen     Pointer to a buffer containing the spare data.
 *
 * @param    p_spare         Pointer to spare buffer.
 *
 * @param    spare_seg_ix    Index of spare segment.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SpareUnpack(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                         void              *p_spare,
                                         CPU_INT08U        spare_seg_ix)
{
  const FS_NAND_FREE_SPARE_DATA    *free_spare_map;
  FS_NAND_CTRLR_GEN_SPARE_SEG_INFO spare_seg_info;
  FS_NAND_PG_SIZE                  spare_seg_rstart;
  CPU_INT32S                       spare_seg_rend;
  CPU_INT32S                       spare_pos;
  FS_NAND_PG_SIZE                  spare_notch_rpos;
  FS_NAND_PG_SIZE                  spare_notch_len;
  CPU_INT08U                       spare_zone_ix;

  free_spare_map = p_ctrlr_gen->FreeSpareMap;

  //                                                               Find last spare zone.
  spare_zone_ix = 0u;
  while (free_spare_map[spare_zone_ix].OctetOffset != FS_NAND_PG_IX_INVALID) {
    spare_zone_ix++;
  }
  spare_zone_ix--;

  spare_seg_info = p_ctrlr_gen->OOS_InfoTbl[spare_seg_ix];
  spare_seg_rend = spare_seg_info.PgOffset
                   - p_ctrlr_gen->PartDataPtr->Info.PgSize
                   - 1u;
  spare_seg_rstart = spare_seg_rend + spare_seg_info.Len;
  spare_pos = spare_seg_rstart;

  spare_notch_rpos = p_ctrlr_gen->PartDataPtr->Info.SpareSize - 1u;
  spare_notch_len = p_ctrlr_gen->PartDataPtr->Info.SpareSize
                    - (free_spare_map[spare_zone_ix].OctetOffset
                       + free_spare_map[spare_zone_ix].OctetLen);

  while (spare_pos > spare_seg_rend) {
    if (spare_notch_rpos <= spare_pos) {                        // Notch before cur pos ...
      if (spare_notch_rpos > spare_seg_rend) {                  // ... and in cur seg.
        spare_pos = spare_notch_rpos - spare_notch_len + 1u;
        FS_NAND_CtrlrGen_SpareJoin(p_spare,
                                   spare_seg_info.Len,
                                   spare_pos - (spare_seg_rend + 1u),
                                   spare_notch_len);
      } else {                                                  // ... and after cur seg.
        spare_pos = spare_seg_rend;                             // Done.
      }
    }

    spare_notch_rpos = free_spare_map[spare_zone_ix].OctetOffset - 1u;
    if (spare_zone_ix >= 1u) {
      spare_zone_ix--;
      spare_notch_len = spare_notch_rpos
                        - (free_spare_map[spare_zone_ix].OctetOffset
                           + free_spare_map[spare_zone_ix].OctetLen) + 1u;
    } else {
      spare_pos = spare_seg_rend;                               // No more notches... done.
    }
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SpareSplit()
 *
 * @brief    Split spare buffer at specified spare notch.
 *
 * @param    p_spare     Pointer to a buffer containing the spare data.
 *
 * @param    buf_len     Length on which to apply the operation.
 *
 * @param    pos         Start position of the spare notch to create.
 *
 * @param    len         Length of the spare notch to create.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SpareSplit(void            *p_spare,
                                        FS_NAND_PG_SIZE buf_len,
                                        FS_NAND_PG_SIZE pos,
                                        FS_NAND_PG_SIZE len)
{
  CPU_INT08U      *p_spare_08;
  FS_NAND_PG_SIZE i;

  p_spare_08 = (CPU_INT08U *)p_spare;

  for (i = buf_len - len - 1u; i != pos; i--) {
    p_spare_08[i + len] = p_spare_08[i];
  }
  p_spare_08[pos + len] = p_spare_08[pos];

  for (i = pos; i < pos + len; i++) {
    p_spare_08[i] = 0xFFu;
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SpareJoin()
 *
 * @brief    Join spare buffer at specified spare notch.
 *
 * @param    p_spare     Pointer to a buffer containing the spare data.
 *
 * @param    buf_len     Length on which to apply the operation.
 *
 * @param    pos         Start position of the spare notch to eliminate.
 *
 * @param    len         Length of the spare notch to eliminate.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SpareJoin(void            *p_spare,
                                       FS_NAND_PG_SIZE buf_len,
                                       FS_NAND_PG_SIZE pos,
                                       FS_NAND_PG_SIZE len)
{
  CPU_INT08U      *p_spare_08;
  FS_NAND_PG_SIZE i;

  p_spare_08 = (CPU_INT08U *)p_spare;

  for (i = pos; i < buf_len - len; i++) {
    p_spare_08[i] = p_spare_08[i + len];
  }
  for (i = buf_len - len; i < buf_len; i++) {
    p_spare_08[i] = 0xFFu;
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_ParamPgRd()
 *
 * @brief    Read parameter page of ONFI devices.
 *
 * @param    p_ctrlr     Pointer to a buffer containing the controller data.
 *
 * @param    rel_addr    Relative address to read from.
 *
 * @param    byte_cnt    Number of bytes to read.
 *
 * @param    p_buf       Pointer to buffer receiving parameters describing NAND flash capabilities.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_ParamPgRd(FS_NAND_CTRLR *p_ctrlr,
                                       CPU_INT16U    rel_addr,
                                       CPU_INT16U    byte_cnt,
                                       CPU_INT08U    *p_buf,
                                       RTOS_ERR      *p_err)
{
  FS_NAND_CTRLR_GEN           *p_ctrlr_gen;
  const FS_NAND_CTRLR_DRV_API *p_drv;
  CPU_INT08U                  addr[FS_NAND_CTRLR_GEN_ADDR_MAX_LEN];
  CPU_INT08U                  cmd1;
  CPU_INT08U                  cmd2;
  CPU_DATA                    ix;

  p_ctrlr_gen = (FS_NAND_CTRLR_GEN *)p_ctrlr;
  p_drv = p_ctrlr_gen->Drv.HW_InfoPtr->DrvApiPtr;

  cmd1 = FS_NAND_CMD_RD_PARAM_PG;

  addr[0] = 0;

  p_drv->ChipSelEn(&p_ctrlr_gen->Drv);

  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Sec addr.
  FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Wait until rdy.
  p_drv->WaitWhileBusy(&p_ctrlr_gen->Drv,
                       p_ctrlr_gen,
                       FS_NAND_CtrlrGen_PollFnct,
                       FS_NAND_MAX_RD_us,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
    LOG_DBG(("Timeout occurred when sending command."));
    return;
  }

  //                                                               Calculate column.
  if (rel_addr != 0) {
    for (ix = 0u; ix < FS_NAND_CTRLR_GEN_ROW_ADDR_MAX_LEN; ix++) {
      addr[ix] = (rel_addr >> (DEF_OCTET_NBR_BITS * ix)) & DEF_OCTET_MASK;
    }

    cmd1 = FS_NAND_CMD_CHNGRDCOL_SETUP;
    cmd2 = FS_NAND_CMD_CHNGRDCOL_CONFIRM;

    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    FS_ERR_CHK(p_drv->AddrWr(&p_ctrlr_gen->Drv, &addr[0], FS_NAND_CTRLR_GEN_ROW_ADDR_MAX_LEN, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
    FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd2, 1u, p_err),
               p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  }

  cmd1 = FS_NAND_CMD_RD_SETUP;                                  // Switch back to rd mode.
  FS_ERR_CHK(p_drv->CmdWr(&p_ctrlr_gen->Drv, &cmd1, 1u, p_err),
             p_drv->ChipSelDis(&p_ctrlr_gen->Drv));
  //                                                               Rd data from parameter page.
  p_drv->DataRd(&p_ctrlr_gen->Drv, p_buf, byte_cnt, 8u, p_err);

  p_drv->ChipSelDis(&p_ctrlr_gen->Drv);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NAND_AVAIL
