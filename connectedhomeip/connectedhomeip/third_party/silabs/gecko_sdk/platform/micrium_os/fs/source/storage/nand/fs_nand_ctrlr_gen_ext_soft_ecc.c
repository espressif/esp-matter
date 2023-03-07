/***************************************************************************//**
 * @file
 * @brief File System - NAND Device Generic Controller Software ECC Extension
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

//                                                                 ------------------------ FS ------------------------
#include  <fs/include/fs_nand_ctrlr_gen_ext_soft_ecc.h>
#include  <fs/source/storage/nand/fs_nand_ctrlr_gen_priv.h>
#include  <fs/source/shared/crc/ecc_hamming.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH        (FS, DRV, NAND)
#define  RTOS_MODULE_CUR     RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct soft_ecc_data {
  FS_NAND_PG_SIZE OOS_SizePerCodeword;                          // Size in octets of OOS area per codeword.
  CPU_INT08U      SizePerSec;                                   // Size in octets of ECC per sector.
  CPU_INT08U      CodewordsPerSec;                              // ECC codewords per sector.
  FS_NAND_PG_SIZE BufSizePerCodeword;                           // ECC codeword size floored to a divider of sec size.

  const ECC_CALC  *ModulePtr;                                   // Pointer to ECC module.
} SOFT_ECC_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void *FS_NAND_CtrlrGen_SoftECC_Open(FS_NAND_CTRLR_GEN                   *p_ctrlr_gen,
                                           const FS_NAND_CTRLR_GEN_EXT_HW_INFO *p_gen_ext_hw_info,
                                           MEM_SEG                             *p_seg,
                                           RTOS_ERR                            *p_err);

static void FS_NAND_CtrlrGen_SoftECC_Close(void *p_ext_data);

static FS_NAND_PG_SIZE FS_NAND_CtrlrGen_SoftECC_Setup(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                                      void              *p_ext_data,
                                                      RTOS_ERR          *p_err);

static void FS_NAND_CtrlrGen_SoftECC_ECC_Calc(void            *p_ext_data,
                                              void            *p_sec_buf,
                                              void            *p_oos_buf,
                                              FS_NAND_PG_SIZE oos_size,
                                              RTOS_ERR        *p_err);

static void FS_NAND_CtrlrGen_SoftECC_ECC_Verify(void            *p_ext_data,
                                                void            *p_sec_buf,
                                                void            *p_oos_buf,
                                                FS_NAND_PG_SIZE oos_size,
                                                RTOS_ERR        *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                               NAND GENERIC CTRLR SOFTWARE ECC EXTENSION
 ********************************************************************************************************
 *******************************************************************************************************/

FS_NAND_CTRLR_GEN_EXT_API FS_NAND_CtrlrGen_SoftECC = {
  .Open = FS_NAND_CtrlrGen_SoftECC_Open,
  .Close = FS_NAND_CtrlrGen_SoftECC_Close,
  .Setup = FS_NAND_CtrlrGen_SoftECC_Setup,
  .RdStatusChk = DEF_NULL,
  .ECC_Calc = FS_NAND_CtrlrGen_SoftECC_ECC_Calc,
  .ECC_Verify = FS_NAND_CtrlrGen_SoftECC_ECC_Verify,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_NAND_CTRLR_GEN_SOFT_ECC_HW_INFO FS_NAND_CtrlrGen_SoftEcc_Hamming_HwInfo = {
  .CtrlrGenExtHwInfo.CtrlrGenExtApiPtr = &FS_NAND_CtrlrGen_SoftECC,
  .ECC_ModulePtr = &Hamming_ECC
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SoftECC_Open()
 *
 * @brief    Open software ECC extension module instance.
 *
 * @param    p_ctrlr_gen         Pointer to NAND generic controller instance.
 *
 * @param    p_gen_ext_hw_info   Pointer to a NAND generic controller extension hardware description.
 *
 * @param    p_seg               Pointer to a memory segment where to allocate controller extension
 *                               internal data structures.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Pointer to extension data.
 *******************************************************************************************************/
static void *FS_NAND_CtrlrGen_SoftECC_Open(FS_NAND_CTRLR_GEN                   *p_ctrlr_gen,
                                           const FS_NAND_CTRLR_GEN_EXT_HW_INFO *p_gen_ext_hw_info,
                                           MEM_SEG                             *p_seg,
                                           RTOS_ERR                            *p_err)
{
  FS_NAND_CTRLR_GEN_SOFT_ECC_HW_INFO *p_soft_ecc_hw_info;
  SOFT_ECC_DATA                      *p_soft_ecc_data;

  PP_UNUSED_PARAM(p_ctrlr_gen);

  //                                                               ------------------ VALIDATE ARGS -------------------
  RTOS_ASSERT_DBG_ERR_SET(p_gen_ext_hw_info != DEF_NULL, *p_err,
                          RTOS_ERR_NULL_PTR, DEF_NULL);

  p_soft_ecc_hw_info = (FS_NAND_CTRLR_GEN_SOFT_ECC_HW_INFO *)p_gen_ext_hw_info;

  //                                                               --------------- ALLOC AND INIT DATA ----------------
  p_soft_ecc_data = (SOFT_ECC_DATA *)Mem_SegAlloc("FS - NAND soft ECC data",
                                                  p_seg,
                                                  sizeof(SOFT_ECC_DATA),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_soft_ecc_data->ModulePtr = p_soft_ecc_hw_info->ECC_ModulePtr;

  return (p_soft_ecc_data);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SoftECC_Close()
 *
 * @brief    Close software ECC extension module instance.
 *
 * @param    p_ext_data  Pointer to extension data.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SoftECC_Close(void *p_ext_data)
{
  PP_UNUSED_PARAM(p_ext_data);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrGen_SoftECC_Setup()
 *
 * @brief    Setup software ECC extension module in accordance with generic controller's data.
 *
 * @param    p_ctrlr_gen     Pointer to NAND generic controller instance.
 *
 * @param    p_ext_data      Pointer to extension data.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Size required storage space for ECC per sector.
 *******************************************************************************************************/
static FS_NAND_PG_SIZE FS_NAND_CtrlrGen_SoftECC_Setup(FS_NAND_CTRLR_GEN *p_ctrlr_gen,
                                                      void              *p_ext_data,
                                                      RTOS_ERR          *p_err)
{
  SOFT_ECC_DATA *p_soft_ecc_data;

  p_soft_ecc_data = (SOFT_ECC_DATA *)p_ext_data;

  //                                                               ------------------- VALIDATE ECC -------------------
  RTOS_ASSERT_DBG_ERR_SET(p_ctrlr_gen->PartDataPtr->Info.ECC_CodewordSize <= p_soft_ecc_data->ModulePtr->BufLenMax,
                          *p_err, RTOS_ERR_INVALID_CFG, FS_NAND_PG_IX_INVALID);

  RTOS_ASSERT_DBG_ERR_SET(p_ctrlr_gen->PartDataPtr->Info.ECC_CodewordSize >= p_soft_ecc_data->ModulePtr->BufLenMin,
                          *p_err, RTOS_ERR_INVALID_CFG, FS_NAND_PG_IX_INVALID);

  RTOS_ASSERT_DBG_ERR_SET(p_soft_ecc_data->ModulePtr->NbrCorrectableBits >= p_ctrlr_gen->PartDataPtr->Info.ECC_NbrCorrBits,
                          *p_err, RTOS_ERR_INVALID_CFG, FS_NAND_PG_IX_INVALID);

  //                                                               -------------------- SETUP ECC ---------------------
  p_soft_ecc_data->CodewordsPerSec = p_ctrlr_gen->SecSize / p_ctrlr_gen->PartDataPtr->Info.ECC_CodewordSize;
  if (p_ctrlr_gen->SecSize % p_ctrlr_gen->PartDataPtr->Info.ECC_CodewordSize != 0u) {
    p_soft_ecc_data->CodewordsPerSec += 1u;
  }

  p_soft_ecc_data->BufSizePerCodeword = p_ctrlr_gen->SecSize / p_soft_ecc_data->CodewordsPerSec;
  p_soft_ecc_data->OOS_SizePerCodeword = p_ctrlr_gen->PartDataPtr->Info.ECC_CodewordSize - p_soft_ecc_data->BufSizePerCodeword;

  RTOS_ASSERT_DBG_ERR_SET(p_soft_ecc_data->OOS_SizePerCodeword * p_soft_ecc_data->CodewordsPerSec >= p_ctrlr_gen->OOS_SizePerSec,
                          *p_err, RTOS_ERR_INVALID_CFG, FS_NAND_PG_IX_INVALID);

  p_soft_ecc_data->SizePerSec = p_soft_ecc_data->ModulePtr->ECC_Len * p_soft_ecc_data->CodewordsPerSec;

  return (p_soft_ecc_data->SizePerSec);
}

/****************************************************************************************************//**
 *                                   FS_NAND_CtrlrGen_SoftECC_ECC_Calc()
 *
 * @brief    Calculate ECC code words for sector and OOS data.
 *
 * @param    p_ext_data  Pointer to extension data.
 *
 * @param    p_sec_buf   Pointer to sector data.
 *
 * @param    p_oos_buf   Pointer to OOS data.
 *
 * @param    oos_size    Size of OOS data to protect in octets, excluding storage space for ECC code words.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SoftECC_ECC_Calc(void            *p_ext_data,
                                              void            *p_sec_buf,
                                              void            *p_oos_buf,
                                              FS_NAND_PG_SIZE oos_size,
                                              RTOS_ERR        *p_err)
{
  SOFT_ECC_DATA   *p_soft_ecc_data;
  FS_NAND_PG_SIZE oos_chk_size;
  FS_NAND_PG_SIZE oos_chk_size_rem;
  CPU_INT08U      *p_sec_buf_08;
  CPU_INT08U      *p_oos_buf_08;
  CPU_INT08U      *p_ecc_08;
  CPU_DATA        ix;

  PP_UNUSED_PARAM(p_err);

  p_soft_ecc_data = (SOFT_ECC_DATA *)p_ext_data;
  p_sec_buf_08 = (CPU_INT08U *)p_sec_buf;
  p_oos_buf_08 = (CPU_INT08U *)p_oos_buf;
  p_ecc_08 = (CPU_INT08U *)p_oos_buf + oos_size;

  oos_chk_size_rem = oos_size;

  for (ix = 0u; ix < p_soft_ecc_data->CodewordsPerSec; ix++) {
    if (oos_chk_size_rem > p_soft_ecc_data->OOS_SizePerCodeword) {
      oos_chk_size = p_soft_ecc_data->OOS_SizePerCodeword;
    } else {
      oos_chk_size = (FS_NAND_PG_SIZE) oos_chk_size_rem;
    }
    oos_chk_size_rem -= oos_chk_size;

    p_soft_ecc_data->ModulePtr->Calc(p_sec_buf_08,
                                     p_soft_ecc_data->BufSizePerCodeword,
                                     p_oos_buf_08,
                                     oos_chk_size,
                                     p_ecc_08);

    p_sec_buf_08 += p_soft_ecc_data->BufSizePerCodeword;
    p_oos_buf_08 += oos_chk_size;
    p_ecc_08 += p_soft_ecc_data->SizePerSec / p_soft_ecc_data->CodewordsPerSec;
  }
}

/****************************************************************************************************//**
 *                                   FS_NAND_CtrlrGen_SoftECC_ECC_Verify()
 *
 * @brief    Verify sector and OOS data against ECC code words and correct data if needed/possible.
 *
 * @param    p_ext_data  Pointer to extension data.
 *
 * @param    p_sec_buf   Pointer to sector data.
 *
 * @param    p_oos_buf   Pointer to OOS data.
 *
 * @param    oos_size    Size of OOS data to protect in octets, excluding storage space for ECC code words.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_CtrlrGen_SoftECC_ECC_Verify(void            *p_ext_data,
                                                void            *p_sec_buf,
                                                void            *p_oos_buf,
                                                FS_NAND_PG_SIZE oos_size,
                                                RTOS_ERR        *p_err)
{
  SOFT_ECC_DATA   *p_soft_ecc_data;
  FS_NAND_PG_SIZE oos_chk_size;
  FS_NAND_PG_SIZE oos_chk_size_rem;
  RTOS_ERR        err_rtn;
  RTOS_ERR        err_ecc;
  CPU_DATA        ix;
  CPU_INT08U      *p_sec_buf_08;
  CPU_INT08U      *p_oos_buf_08;
  CPU_INT08U      *p_ecc_08;
  CPU_INT08U      ecc_size;

  p_soft_ecc_data = (SOFT_ECC_DATA *)p_ext_data;

  p_sec_buf_08 = (CPU_INT08U *)p_sec_buf;
  p_oos_buf_08 = (CPU_INT08U *)p_oos_buf;
  p_ecc_08 = (CPU_INT08U *)p_oos_buf + oos_size;

  oos_chk_size_rem = oos_size;
  ecc_size = p_soft_ecc_data->SizePerSec / p_soft_ecc_data->CodewordsPerSec;

  RTOS_ERR_SET(err_rtn, RTOS_ERR_NONE);
  for (ix = 0u; ix < p_soft_ecc_data->CodewordsPerSec; ix++) {
    if (oos_chk_size_rem > p_soft_ecc_data->OOS_SizePerCodeword) {
      oos_chk_size = p_soft_ecc_data->OOS_SizePerCodeword;
    } else {
      oos_chk_size = (FS_NAND_PG_SIZE) oos_chk_size_rem;
    }
    oos_chk_size_rem -= oos_chk_size;

    RTOS_ERR_SET(err_ecc, RTOS_ERR_NONE);
    p_soft_ecc_data->ModulePtr->Correct(p_sec_buf_08,
                                        p_soft_ecc_data->BufSizePerCodeword,
                                        p_oos_buf_08,
                                        oos_chk_size,
                                        p_ecc_08,
                                        &err_ecc);
    switch (RTOS_ERR_CODE_GET(err_ecc)) {
      case RTOS_ERR_ECC_UNCORR:
        RTOS_ERR_SET(err_rtn, RTOS_ERR_ECC_UNCORR);
        break;

      case RTOS_ERR_ECC_CORR:                                   // #### Add support for critical/non-critical errs.
        if (RTOS_ERR_CODE_GET(err_rtn) != RTOS_ERR_ECC_UNCORR) {
          RTOS_ERR_SET(err_rtn, RTOS_ERR_ECC_CRITICAL_CORR);
        }
        break;

      case RTOS_ERR_NONE:
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; )
    }

    p_sec_buf_08 += p_soft_ecc_data->BufSizePerCodeword;
    p_oos_buf_08 += oos_chk_size;
    p_ecc_08 += ecc_size;
  }

  *p_err = err_rtn;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NAND_AVAIL
