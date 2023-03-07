/***************************************************************************//**
 * @file
 * @brief File System - NAND Flash Devices - ONFI Configuration Part-Layer
 *        Driver
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

#include  <fs_storage_cfg.h>

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <fs/source/storage/nand/fs_nand_priv.h>
#include  <fs/source/shared/crc/edc_crc.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (FS, DRV, NAND)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_FS

//                                                                 ONFI versions.
#define  FS_NAND_PART_ONFI_V30                          DEF_BIT_06
#define  FS_NAND_PART_ONFI_V23                          DEF_BIT_05
#define  FS_NAND_PART_ONFI_V22                          DEF_BIT_04
#define  FS_NAND_PART_ONFI_V21                          DEF_BIT_03
#define  FS_NAND_PART_ONFI_V20                          DEF_BIT_02
#define  FS_NAND_PART_ONFI_V10                          DEF_BIT_01

#define  FS_NAND_PART_ONFI_V_QTY                          6u

#define  FS_NAND_PART_ONFI_MAX_PARAM_PG                   3u

//                                                                 Supported ONFI features.
#define  FS_NAND_PART_ONFI_FEATURE_EX_PP                DEF_BIT_07
#define  FS_NAND_PART_ONFI_FEATURE_RNDM_PG_PGM          DEF_BIT_02
#define  FS_NAND_PART_ONFI_FEATURE_BUS_16               DEF_BIT_00

//                                                                 Last data byte of parameter page covered by CRC.
#define  FS_NAND_PART_ONFI_PARAM_PAGE_LAST_DATA_BYTE    253u

//                                                                 Supported section types of extended parameter page.
#define  FS_NAND_PART_ONFI_SECTION_TYPE_UNUSED            0u
#define  FS_NAND_PART_ONFI_SECTION_TYPE_SPECIFIER         1u
#define  FS_NAND_PART_ONFI_SECTION_TYPE_ECC_INFO          2u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT08U FS_NAND_PartONFI_ParamPg[FS_NAND_PART_ONFI_PARAM_PAGE_LEN];
static CPU_INT08U FS_NAND_PartONFI_ParamPageCnt;

static CPU_INT32U FS_NAND_PartONFI_ExtParamPageLen;
static CPU_INT08U *FS_NAND_PartONFI_ExtParamPageBufPtr;
static CPU_INT16U FS_NAND_PartONFI_ExtParamPageBufLen;

static CRC_MODEL_16 FS_NAND_PartONFI_CRCModel = {
  0x8005,
  0x4F4E,
  DEF_NO,
  0x0000,
  (const CPU_INT16U *) 0
};

static const CPU_INT32U FS_NAND_PartONFI_PowerOf10[10] = {
  1u,
  10u,
  100u,
  1000u,
  10000u,
  100000u,
  1000000u,
  10000000u,
  100000000u,
  1000000000u
};

static const CPU_INT16U FS_NAND_PartONFI_SupportedVersions[FS_NAND_PART_ONFI_V_QTY] = {
  FS_NAND_PART_ONFI_V30,
  FS_NAND_PART_ONFI_V23,
  FS_NAND_PART_ONFI_V22,
  FS_NAND_PART_ONFI_V21,
  FS_NAND_PART_ONFI_V20,
  FS_NAND_PART_ONFI_V10
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Parameter page read.
static void FS_NAND_PartONFI_ParamPgRdInternal(FS_NAND_CTRLR *p_ctrlr,
                                               FS_NAND_PART  *p_part,
                                               RTOS_ERR      *p_err);

//                                                                 Parse ONFI parameter page.
static CPU_BOOLEAN FS_NAND_PartONFI_ParamPageParse(FS_NAND_PART *p_part,
                                                   RTOS_ERR     *p_err);

//                                                                 Parse ONFI extended parameter page.
static void FS_NAND_PartONFI_ExtParamPageParse(FS_NAND_PART *p_part,
                                               CPU_INT08U   *p_page,
                                               CPU_INT16U   page_len,
                                               RTOS_ERR     *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_PartONFI_Add()
 *
 * @brief    Add an ONFI part.
 *
 * @param    p_nand_ctrlr    Pointer to a NAND controller instance.
 *
 * @param    p_seg           Pointer to a memory segment where to allocate part data.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to a NAND part.
 *******************************************************************************************************/
FS_NAND_PART *FS_NAND_PartONFI_Add(FS_NAND_CTRLR *p_nand_ctrlr,
                                   MEM_SEG       *p_seg,
                                   RTOS_ERR      *p_err)
{
  FS_NAND_PART *p_part;

  //                                                               ------------------ VALIDATE ARGS -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_CRITICAL_ERR_SET(p_nand_ctrlr != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  //                                                               ----------------- ALLOC PART DATA ------------------
  p_part = (FS_NAND_PART *)Mem_SegAlloc("FS - NAND ONFI part data",
                                        p_seg,
                                        sizeof(FS_NAND_PART),
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               --------------- READ PARAMETER PAGE ----------------
  FS_NAND_PartONFI_ParamPgRdInternal(p_nand_ctrlr, p_part, p_err);

  return (p_part);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_NAND_PartONFI_ParamPageParse()
 *
 * @brief    Parse the ONFI parameter page.
 *
 * @param    p_part  Pointer to a NAND part.
 *
 * @param    p_err   Error pointer.
 *
 * @return   DEF_YES, if extended parameter page is supported.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_PartONFI_ParamPageParse(FS_NAND_PART *p_part,
                                                   RTOS_ERR     *p_err)
{
  CPU_BOOLEAN is_ver_supported;
  CPU_BOOLEAN has_ext_pp;
  CPU_BOOLEAN is_bus_16;
  CPU_BOOLEAN is_invalid;
  CPU_INT08U  value;
  CPU_INT08U  multiplier;
  CPU_INT16U  version;
  CPU_INT32U  pg_size;
  CPU_INT32U  max_size;
  CPU_INT32U  nb_pg_per_blk;
  CPU_INT32U  blk_cnt;
  CPU_INT64U  max_blk_erase;
  CPU_DATA    ix;

  //                                                               -------------- REV INFO AND FEATURES ---------------
  //                                                               Validate ONFI version.
  MEM_VAL_COPY_GET_INT16U_LITTLE(&version, &FS_NAND_PartONFI_ParamPg[4]);

  is_invalid = DEF_BIT_IS_SET(version, DEF_BIT_00);
  if (is_invalid == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return (DEF_NO);
  }

  ix = 0;
  is_ver_supported = DEF_NO;
  while ((is_ver_supported != DEF_YES)
         && (ix < FS_NAND_PART_ONFI_V_QTY)) {
    is_ver_supported = DEF_BIT_IS_SET(version, FS_NAND_PartONFI_SupportedVersions[ix]);
    ix++;
  }

  if (is_ver_supported == DEF_YES) {
    version = FS_NAND_PartONFI_SupportedVersions[ix - 1u];
  } else {
    LOG_ERR(("ONFI version not supported by this driver."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return (DEF_NO);
  }

  //                                                               Identify features.
  has_ext_pp = DEF_BIT_IS_SET(FS_NAND_PartONFI_ParamPg[6], FS_NAND_PART_ONFI_FEATURE_EX_PP);

  p_part->Info.SupportsRndPgPgm = DEF_BIT_IS_SET(FS_NAND_PartONFI_ParamPg[6], FS_NAND_PART_ONFI_FEATURE_RNDM_PG_PGM);

  is_bus_16 = DEF_BIT_IS_SET(FS_NAND_PartONFI_ParamPg[6], FS_NAND_PART_ONFI_FEATURE_BUS_16);
  if (is_bus_16 == DEF_YES) {
    p_part->Info.BusWidth = 16u;
  } else {
    p_part->Info.BusWidth = 8u;
  }

  //                                                               Extended parameter page length.
  if (has_ext_pp == DEF_YES) {
    FS_NAND_PartONFI_ExtParamPageLen = FS_NAND_PartONFI_ParamPg[12];
    FS_NAND_PartONFI_ExtParamPageLen |= (CPU_INT16U)((CPU_INT16U)FS_NAND_PartONFI_ParamPg[13] << DEF_INT_08_NBR_BITS);
    FS_NAND_PartONFI_ExtParamPageLen *= 16u;
  }

  //                                                               Number of parameter pages.
  if (version >= FS_NAND_PART_ONFI_V21) {
    FS_NAND_PartONFI_ParamPageCnt = FS_NAND_PartONFI_ParamPg[14];
  } else {
    FS_NAND_PartONFI_ParamPageCnt = 3u;
  }

  //                                                               ----------- IDENTIFY MEMORY ORGANIZATION -----------
  //                                                               Page size.
  MEM_VAL_COPY_GET_INT32U_LITTLE(&pg_size, &FS_NAND_PartONFI_ParamPg[80]);
  max_size = (FS_NAND_PG_SIZE) -1;
  RTOS_ASSERT_DBG_ERR_SET(pg_size <= max_size, *p_err, RTOS_ERR_NOT_SUPPORTED, DEF_NO);

  p_part->Info.PgSize = pg_size;

  //                                                               Spare size.
  MEM_VAL_COPY_GET_INT16U_LITTLE(&(p_part->Info.SpareSize), &FS_NAND_PartONFI_ParamPg[84]);

  //                                                               Nb of pages per blk.
  MEM_VAL_COPY_GET_INT32U_LITTLE(&nb_pg_per_blk, &FS_NAND_PartONFI_ParamPg[92]);
  max_size = (FS_NAND_PG_PER_BLK_QTY) -1;
  RTOS_ASSERT_DBG_ERR_SET(nb_pg_per_blk <= max_size, *p_err, RTOS_ERR_NOT_SUPPORTED, DEF_NO);

  p_part->Info.PgPerBlk = nb_pg_per_blk;

  //                                                               Nb of block per logical unit.
  MEM_VAL_COPY_GET_INT32U_LITTLE(&blk_cnt, &FS_NAND_PartONFI_ParamPg[96]);
  max_size = (FS_NAND_BLK_QTY) -1;
  RTOS_ASSERT_DBG_ERR_SET(blk_cnt <= max_size, *p_err, RTOS_ERR_NOT_SUPPORTED, DEF_NO);

  p_part->Info.BlkCnt = blk_cnt;

  //                                                               Max nb of bad blocks per logical unit.
  MEM_VAL_COPY_GET_INT16U_LITTLE(&(p_part->Info.MaxBadBlkCnt), &FS_NAND_PartONFI_ParamPg[103]);

  //                                                               Max programming operations per block.
  value = FS_NAND_PartONFI_ParamPg[105];
  multiplier = FS_NAND_PartONFI_ParamPg[106];

  RTOS_ASSERT_DBG_ERR_SET(multiplier <= 9u, *p_err, RTOS_ERR_NOT_SUPPORTED, DEF_NO);

  max_blk_erase = value * FS_NAND_PartONFI_PowerOf10[multiplier];
  max_size = (CPU_INT32U) -1;
  RTOS_ASSERT_DBG_ERR_SET(max_blk_erase <= max_size, *p_err, RTOS_ERR_NOT_SUPPORTED, DEF_NO);

  p_part->Info.MaxBlkErase = max_blk_erase;

  //                                                               Max nb of partial page programming.
  p_part->Info.NbrPgmPerPg = FS_NAND_PartONFI_ParamPg[110];

  //                                                               ECC correction needed.
  p_part->Info.ECC_NbrCorrBits = FS_NAND_PartONFI_ParamPg[112];
  if (p_part->Info.ECC_NbrCorrBits == 0xFFu) {
    p_part->Info.ECC_CodewordSize = 0u;

    RTOS_ASSERT_DBG_ERR_SET(has_ext_pp, *p_err, RTOS_ERR_NOT_SUPPORTED, DEF_NO);
  } else {
    p_part->Info.ECC_CodewordSize = 528u;
  }

  //                                                               Factory defect mark type.
  if (version >= FS_NAND_PART_ONFI_V21) {
    p_part->Info.DefectMarkType = DEFECT_SPARE_L_1_PG_1_OR_N_ALL_0;
  } else {
    //                                                             #### NAND drv not compatible with 'any loc'.
    p_part->Info.DefectMarkType = DEFECT_SPARE_L_1_PG_1_OR_N_ALL_0;
  }

  return (has_ext_pp);
}

/****************************************************************************************************//**
 *                                   FS_NAND_PartONFI_ExtParamPageParse()
 *
 * @brief    Parse the ONFI extended parameter page.
 *
 * @param    p_part      Pointer to a NAND part data object.
 *
 * @param    p_page      Pointer to extended parameter page buffer.
 *
 * @param    page_len    Extended parameter page length.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_PartONFI_ExtParamPageParse(FS_NAND_PART *p_part,
                                               CPU_INT08U   *p_page,
                                               CPU_INT16U   page_len,
                                               RTOS_ERR     *p_err)
{
  CPU_INT08U  section_type;
  CPU_INT08U  section_len;
  CPU_INT08U  type_addr;
  CPU_INT08U  section_addr;
  CPU_BOOLEAN is_pg_parsed;
  CPU_BOOLEAN ecc_scheme_modified;
  CPU_INT32U  ix;
  CPU_INT08U  type_specifier_section_addr = 0u;
  CPU_INT08U  type_specifier_section_len = 0u;
  CPU_BOOLEAN extra_sections_specified;

  type_addr = 16;
  section_addr = 32;
  section_type = FS_NAND_PART_ONFI_SECTION_TYPE_SPECIFIER;
  is_pg_parsed = DEF_FALSE;
  ecc_scheme_modified = DEF_FALSE;
  extra_sections_specified = DEF_FALSE;

  //                                                               Parse the first 8 sections of the extended param pg.
  while (is_pg_parsed == DEF_FALSE) {
    //                                                             Get section type and length.
    section_type = p_page[type_addr];
    section_len = p_page[type_addr + 1u] * 16u;

    switch (section_type) {
      case FS_NAND_PART_ONFI_SECTION_TYPE_UNUSED:               // End parsing if unused sections are reached.
        is_pg_parsed = DEF_TRUE;
        break;

      case FS_NAND_PART_ONFI_SECTION_TYPE_SPECIFIER:            // Get extra sections specifiers.
        type_specifier_section_addr = section_type;
        type_specifier_section_len = section_len;
        extra_sections_specified = DEF_TRUE;
        break;

      case FS_NAND_PART_ONFI_SECTION_TYPE_ECC_INFO:             // Get additionnal ECC info.
        if (ecc_scheme_modified == DEF_FALSE) {
          p_part->Info.ECC_NbrCorrBits = p_page[section_addr];
          p_part->Info.ECC_CodewordSize = p_page[section_addr + 1u];
          ecc_scheme_modified = DEF_TRUE;
        } else {
          LOG_ERR(("Driver only support one ECC scheme."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
          return;
        }
        break;

      default:
        LOG_ERR(("Invalid section type in extended parameter page: ", (u)section_type, "."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
    }

    //                                                             Increment section address.
    section_addr += section_len;
    type_addr += 2u;

    //                                                             Detect end of extended parameter page.
    if (section_addr > page_len) {
      is_pg_parsed = DEF_TRUE;
    }
    if (type_addr > 30u) {
      is_pg_parsed = DEF_TRUE;
    }
  }

  //                                                               Process extra sections of extended parameter page.
  if (type_addr > 30u) {
    if (extra_sections_specified == DEF_FALSE) {
      LOG_ERR(("Extended parameter page extra sections not specified."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
    for (ix = 0u; ix < (type_specifier_section_len / 2u); ix++) {
      section_type = p_page[type_specifier_section_addr];
      section_len = p_page[type_specifier_section_addr + 1u] * 16u;

      switch (section_type) {
        case FS_NAND_PART_ONFI_SECTION_TYPE_UNUSED:
          break;

        case FS_NAND_PART_ONFI_SECTION_TYPE_ECC_INFO:
          if (ecc_scheme_modified == DEF_FALSE) {
            p_part->Info.ECC_NbrCorrBits = p_page[section_addr];
            p_part->Info.ECC_CodewordSize = p_page[section_addr + 1u];
            ecc_scheme_modified = DEF_TRUE;
          } else {
            LOG_ERR(("Driver only support one ECC scheme."));
            RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
            return;
          }
          break;

        default:
          LOG_ERR(("Invalid section type in extended parameter page: ", (u)section_type, "."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
          return;
      }

      type_specifier_section_addr += 1u;
    }
  }
}

/****************************************************************************************************//**
 *                                   FS_NAND_PartONFI_ParamPgRdInternal()
 *
 * @brief    Read parameter page and extended parameter page.
 *
 * @param    p_nand_ctrlr    Pointer to a NAND controller instance.
 *
 * @param    p_part          Pointer to a NAND part.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NAND_PartONFI_ParamPgRdInternal(FS_NAND_CTRLR *p_ctrlr,
                                               FS_NAND_PART  *p_part,
                                               RTOS_ERR      *p_err)
{
  CPU_INT16U  rel_addr;
  CPU_DATA    ix;
  CPU_INT08U  sig_cnt;
  CPU_INT16U  chk_crc;
  CPU_INT16U  calc_crc;
  CPU_BOOLEAN has_ext_pp;

  //                                                               Init CRC values to start the loop.
  calc_crc = 0u;
  chk_crc = 1u;
  ix = 0u;
  rel_addr = 0u;
  //                                                               Find a valid parameter page (matching CRC).
  while ((calc_crc != chk_crc)
         && (ix < FS_NAND_PART_ONFI_MAX_PARAM_PG)) {
    ix++;
    //                                                             Read an instance of the parameter page.
    p_ctrlr->CtrlrApiPtr->ParamPgRd(p_ctrlr,
                                    rel_addr,
                                    FS_NAND_PART_ONFI_PARAM_PAGE_LEN,
                                    &FS_NAND_PartONFI_ParamPg[0],
                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Could not recover parameter page from ONFI device."));
      return;
    }

    //                                                             --------- VERIFY PARAMETER PAGE SIGNATURE ----------
    //                                                             Signature is valid if 2 or more bytes are valid.
    sig_cnt = 0;
    if (FS_NAND_PartONFI_ParamPg[0] == 'O') {
      sig_cnt += 1;
    }
    if (FS_NAND_PartONFI_ParamPg[1] == 'N') {
      sig_cnt += 1;
    }
    if (FS_NAND_PartONFI_ParamPg[2] == 'F') {
      sig_cnt += 1;
    }
    if (FS_NAND_PartONFI_ParamPg[3] == 'I') {
      sig_cnt += 1;
    }
    //                                                             If sig invalid, last parameter page has been read.
    if (sig_cnt < 2) {
      LOG_ERR(("Could not recover a valid parameter page from ONFI device."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    } else {
      //                                                           If sig valid, verify the CRC.
      chk_crc = FS_NAND_PartONFI_ParamPg[254];
      chk_crc |= (CPU_INT16U)((CPU_INT16U)FS_NAND_PartONFI_ParamPg[255] << DEF_INT_08_NBR_BITS);

      calc_crc = CRC_ChkSumCalc_16Bit(&FS_NAND_PartONFI_CRCModel,
                                      &FS_NAND_PartONFI_ParamPg[0],
                                      FS_NAND_PART_ONFI_PARAM_PAGE_LAST_DATA_BYTE + 1u);

      //                                                           Increment the parameter page address.
      rel_addr += FS_NAND_PART_ONFI_PARAM_PAGE_LEN;
    }
  }

  if (calc_crc != chk_crc) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    LOG_ERR(("Could not recover parameter page from ONFI device."));
    return;
  }

  has_ext_pp = FS_NAND_PartONFI_ParamPageParse(p_part, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Find an extended parameter page if present.
  if (has_ext_pp) {
    //                                                             Set buffer to read the extended parameter page.
    FS_NAND_PartONFI_ExtParamPageBufPtr = &FS_NAND_PartONFI_ParamPg[0];
    FS_NAND_PartONFI_ExtParamPageBufLen = FS_NAND_PART_ONFI_PARAM_PAGE_LEN;

    rel_addr = FS_NAND_PartONFI_ParamPageCnt * FS_NAND_PART_ONFI_PARAM_PAGE_LEN;

    RTOS_ASSERT_DBG_ERR_SET(FS_NAND_PartONFI_ExtParamPageLen <= FS_NAND_PartONFI_ExtParamPageBufLen,
                            *p_err, RTOS_ERR_NOT_SUPPORTED,; );

    calc_crc = 0u;
    chk_crc = 1u;
    //                                                             Find a valid extended parameter page.
    while (chk_crc != calc_crc) {
      p_ctrlr->CtrlrApiPtr->ParamPgRd(p_ctrlr,
                                      rel_addr,
                                      FS_NAND_PartONFI_ExtParamPageLen,
                                      FS_NAND_PartONFI_ExtParamPageBufPtr,
                                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Could not recover extended parameter page from ONFI device."));
        return;
      }
      //                                                           --------------- VERIFY PARAM PG SIG ----------------
      //                                                           Signature is valid if 2 or more bytes are valid.
      sig_cnt = 0;
      if (FS_NAND_PartONFI_ExtParamPageBufPtr[2] == 'E') {
        sig_cnt += 1;
      }
      if (FS_NAND_PartONFI_ExtParamPageBufPtr[3] == 'P') {
        sig_cnt += 1;
      }
      if (FS_NAND_PartONFI_ExtParamPageBufPtr[4] == 'P') {
        sig_cnt += 1;
      }
      if (FS_NAND_PartONFI_ExtParamPageBufPtr[5] == 'S') {
        sig_cnt += 1;
      }
      //                                                           If sig invalid, last parameter page has been read.
      if (sig_cnt < 2) {
        LOG_ERR(("Could not recover a valid extended parameter page from ONFI device."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      } else {
        //                                                         If sig valid, verify the CRC.
        chk_crc = FS_NAND_PartONFI_ExtParamPageBufPtr[0];
        chk_crc |= (CPU_INT16U)((CPU_INT16U)FS_NAND_PartONFI_ExtParamPageBufPtr[1] << DEF_INT_08_NBR_BITS);

        calc_crc = CRC_ChkSumCalc_16Bit(&FS_NAND_PartONFI_CRCModel,
                                        &FS_NAND_PartONFI_ExtParamPageBufPtr[2],
                                        FS_NAND_PartONFI_ExtParamPageLen - 2);

        //                                                         Increment the extended parameter page address.
        rel_addr += FS_NAND_PartONFI_ExtParamPageLen;
      }
    }

    //                                                             Parse the extended parameter page.
    FS_NAND_PartONFI_ExtParamPageParse(p_part,
                                       FS_NAND_PartONFI_ExtParamPageBufPtr,
                                       FS_NAND_PartONFI_ExtParamPageLen,
                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NAND_AVAIL
