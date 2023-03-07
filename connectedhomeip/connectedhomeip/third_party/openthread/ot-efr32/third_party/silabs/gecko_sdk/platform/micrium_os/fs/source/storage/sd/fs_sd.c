/***************************************************************************//**
 * @file
 * @brief File System - Sdmmc Media Driver
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
 * @note    (1) This driver complies with the following specifications:
 *              - (a) SD Card Association.  "Physical Layer Simplified Specification Version 2.00".
 *                    July 26, 2006.
 *              - (b) JEDEC Solid State Technology Association.  "MultiMediaCard (MMC) Electrical
 *                    Standard, High Capacity".  JESD84-B42.  July 2007.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL) || defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error SD module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/source/storage/sd/fs_sd_priv.h>
#include  <fs/include/fs_blk_dev.h>
#include  <common/source/logging/logging_priv.h>

#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (FS, DRV, SD)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_FS
//                                                                 ------------------ CHECKSUMS (4.5) -----------------
#define  FS_DEV_SD_CRC7_POLY                            0x09u   // 7-bit checksum polynomial: x7 + x3 + 1.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const FS_SD_HANDLE FS_SD_NullHandle = { 0 };

//                                                                 Converts CSD read_bl_len field to actual blk len.
static const CPU_INT32U FS_SD_BlkLen[] = {       512u,
                                                 1024u,
                                                 2048u };

//                                                                 Converts CSD c_size_mult field to actual mult.
static const CPU_INT32U FS_SD_Mult[] = {         4u,
                                                 8u,
                                                 16u,
                                                 32u,
                                                 64u,
                                                 128u,
                                                 256u,
                                                 512u,
                                                 1024u };

//                                                                 Converts CSD TAAC field's time unit to 1 ns.
static const CPU_INT32U FS_SD_TAAC_TimeUnit[] = {         1u,
                                                          10u,
                                                          100u,
                                                          1000u,
                                                          10000u,
                                                          100000u,
                                                          1000000u,
                                                          10000000u };

//                                                                 Converts CSD TAAC field's time value to mult.
static const CPU_INT32U FS_SD_TAAC_TimeValue[] = {         0u,
                                                           10u,
                                                           12u,
                                                           13u,
                                                           15u,
                                                           20u,
                                                           25u,
                                                           30u,
                                                           35u,
                                                           40u,
                                                           45u,
                                                           50u,
                                                           55u,
                                                           60u,
                                                           70u,
                                                           80u };

//                                                                 Converts CSD tran_speed field's rate unit to 10 kb/s.
static const CPU_INT32U FS_SD_TranSpdUnit[] = {     10000u,
                                                    100000u,
                                                    1000000u,
                                                    10000000u };

//                                                                 Converts CSD tran_speed field's time value to mult.
static const CPU_INT32U FS_SD_TranSpdValue[] = {         0u,
                                                         10u,
                                                         12u,
                                                         13u,
                                                         15u,
                                                         20u,
                                                         25u,
                                                         30u,
                                                         35u,
                                                         40u,
                                                         45u,
                                                         50u,
                                                         55u,
                                                         60u,
                                                         70u,
                                                         80u };

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
static const CRC_MODEL_16 FS_SD_ModelCRC16 = {
  0x1021u,
  0x0000u,
  DEF_NO,
  0x0000u,
  &CRC_TblCRC16_1021[0]
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_SD_Open()
 *
 * @brief    Open a SD Card or SPI.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to a SD Card or SPI.
 *******************************************************************************************************/
FS_SD_HANDLE FS_SD_Open(FS_MEDIA_HANDLE media_handle,
                        RTOS_ERR        *p_err)
{
  FS_SD        *p_sd;
  FS_SD_HANDLE sd_handle = FS_SD_NullHandle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, sd_handle);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Check that Storage layer is initialized.
  CORE_ENTER_ATOMIC();
  if (!FSBlkDev_Data.IsInit) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    CORE_EXIT_ATOMIC();
    return (sd_handle);
  }
  CORE_EXIT_ATOMIC();

  p_sd = (FS_SD *)media_handle.MediaPtr;
  p_sd->BusApiPtr->Open(p_sd, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (sd_handle);
  }

  sd_handle.MediaHandle = media_handle;

  return (sd_handle);
}

/****************************************************************************************************//**
 *                                               FS_SD_Close()
 *
 * @brief    Close a SD Card or SPI.
 *
 * @param    sd_handle   Handle to a SD Card or SPI.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/
void FS_SD_Close(FS_SD_HANDLE sd_handle,
                 RTOS_ERR     *p_err)
{
  FS_MEDIA *p_media;
  FS_SD    *p_sd;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_MEDIA_WITH_NO_IO(sd_handle.MediaHandle, p_err) {
    p_media = sd_handle.MediaHandle.MediaPtr;
    FS_MEDIA_LOCK_WITH(p_media) {
      p_sd = (FS_SD *)sd_handle.MediaHandle.MediaPtr;
      p_sd->BusApiPtr->Close(p_sd, p_err);
    }
  }
}

/****************************************************************************************************//**
 *                                               FS_SD_InfoGet()
 *
 * @brief    Get SD information.
 *
 * @param    sd_handle   Handle to a SD card.
 *
 * @param    p_sd_info   Pointer to a SD information structure to be populated.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/
void FS_SD_InfoGet(FS_SD_HANDLE sd_handle,
                   FS_SD_INFO   *p_sd_info,
                   RTOS_ERR     *p_err)
{
  FS_MEDIA *p_media;
  FS_SD    *p_sd;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET(p_sd_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; )

  FS_MEDIA_WITH_NO_IO(sd_handle.MediaHandle, p_err) {
    p_media = sd_handle.MediaHandle.MediaPtr;
    FS_MEDIA_LOCK_WITH(p_media) {
      p_sd = (FS_SD *)p_media;
      p_sd->BusApiPtr->InfoGet(p_sd, p_sd_info);
    }
  }
}

/****************************************************************************************************//**
 *                                               FS_SD_CID_Rd()
 *
 * @brief    Read SD CID (Card IDentification number) register.
 *
 * @param    sd_handle   Handle to a SD card.
 *
 * @param    p_dest      Pointer to 16-byte buffer that will receive SD/MMC Card ID register.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *
 * @note     (1) Refer to specification "Physical Layer Simplified Specification Version 4.10",
 *               section '5.2 CID register' for more details about CID fields.
 *******************************************************************************************************/
void FS_SD_CID_Rd(FS_SD_HANDLE sd_handle,
                  CPU_INT08U   *p_dest,
                  RTOS_ERR     *p_err)
{
  FS_MEDIA *p_media;
  FS_SD    *p_sd;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_MEDIA_WITH_NO_IO(sd_handle.MediaHandle, p_err) {
    p_media = sd_handle.MediaHandle.MediaPtr;
    FS_MEDIA_LOCK_WITH(p_media) {
      p_sd = (FS_SD *)p_media;
      p_sd->BusApiPtr->CidRd(p_sd, p_dest, p_err);
    }
  }
}

/****************************************************************************************************//**
 *                                               FS_SD_CSD_Rd()
 *
 * @brief    Read SD CSD (Card Specific Data) register.
 *
 * @param    sd_handle   Handle to a SD card.
 *
 * @param    p_dest      Pointer to 16-byte buffer that will receive SD/MMC Card specific data
 *                       register.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *
 * @note     (1) Refer to specification "Physical Layer Simplified Specification Version 4.10",
 *               section '5.3.2 CSD Register' for more details about CSD fields.
 *******************************************************************************************************/
void FS_SD_CSD_Rd(FS_SD_HANDLE sd_handle,
                  CPU_INT08U   *p_dest,
                  RTOS_ERR     *p_err)
{
  FS_MEDIA *p_media;
  FS_SD    *p_sd;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_MEDIA_WITH_NO_IO(sd_handle.MediaHandle, p_err) {
    p_media = sd_handle.MediaHandle.MediaPtr;
    FS_MEDIA_LOCK_WITH(p_media) {
      p_sd = (FS_SD *)p_media;
      p_sd->BusApiPtr->CsdRd(p_sd, p_dest, p_err);
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_SD_ParseCSD()
 *
 * @brief    Parse CSD data.
 *
 * @param    csd         Array holding CSD data.
 *
 * @param    p_sd_info   Pointer to structure into which CSD data will be parsed.
 *
 * @param    card_type   Type of card :
 *                           - FS_DEV_SD_CARDTYPE_SD_V1_X       SD card v1.x.
 *                           - FS_DEV_SD_CARDTYPE_SD_V2_0       SD card v2.0, standard capacity.
 *                           - FS_DEV_SD_CARDTYPE_SD_V2_0_HC    SD card v2.0, high capacity.
 *                           - FS_DEV_SD_CARDTYPE_MMC           MMC, standard capacity.
 *                           - FS_DEV_SD_CARDTYPE_MMC_HC        MMC, high capacity.
 *
 * @return   DEF_OK   if the CSD could be parsed.
 *           DEF_FAIL if the CSD was illegal.
 *
 * @note     (1) For SD v1.x & SD v2.0 standard capacity, the structure of the CSD is defined in
 *               [Ref 1], Section 5.3.2.
 *               For MMC cards, the structure of the CSD is defined in [Ref 2], Section 8.3.
 *               For SD v2.0 high capacity cards, the structure of the CSD is defined in [Ref 1],
 *               Section 5.3.3.
 *
 * @note     (2) The calculation of the card capacity is demonstrated in the [Ref 1], Section 5.3.2.
 *               The text does not explicitly state that the card capacity for a v1.x-compliant card
 *               shall not exceed the capacity of a 32-bit integer.
 *
 * @note     (3) According to [Ref 1], for a v1.x-compliant card, the CSD's tran_speed field "shall
 *               be always 0_0110_010b (032h)", or 25MHz.  In high-speed mode, the field value "shall
 *               be always 0_1011_010b (06Ah)", or 50MHz.
 *
 * @note     (4) According to [Ref 1], Section 4.6.2.1-2, the timeouts for high-capacity SD cards
 *               should be fixed; consequently, TSAC & NSAC should not be consulted.
 *******************************************************************************************************/
CPU_BOOLEAN FS_SD_ParseCSD(CPU_INT08U csd[],
                           FS_SD_INFO *p_sd_info,
                           CPU_INT08U card_type)
{
  CPU_INT32U block_len;
  CPU_INT32U block_nbr;
  CPU_INT32U c_size;
  CPU_INT32U c_size_mult;
  CPU_INT32U clk_freq;
  CPU_INT32U mult;
  CPU_INT08U nsac;
  CPU_INT08U taac;
  CPU_INT32U taac_time;
  CPU_INT08U rd_bl_len;
  CPU_INT08U tran_spd;
  CPU_INT08U tran_spd_unit;

  if (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0_HC) {             // ----------- CALC DISK SIZE (SD HC CARD) ------------
                                                                // Get dev size.
    c_size = (((CPU_INT32U)csd[7] & 0x3Fu) << 16)
             | (((CPU_INT32U)csd[8] & 0xFFu) <<  8)
             | (((CPU_INT32U)csd[9] & 0xFFu) <<  0);

    p_sd_info->BlkSize = FS_DEV_SD_BLK_SIZE;
    p_sd_info->NbrBlks = (c_size + 1u) * 1024u;
  } else {                                                      // ----------- CALC DISK SIZE (SD STD CARD) -----------
    rd_bl_len = csd[5] & DEF_NIBBLE_MASK;                       // Get rd blk len exp.

    if ((rd_bl_len < 9u) || (rd_bl_len > 11u)) {
      return (DEF_NO);
    }

    block_len = FS_SD_BlkLen[rd_bl_len - 9u];                   // Calc blk len.
                                                                // Get dev size.
    c_size = (((CPU_INT32U)csd[8] & 0xC0u) >>  6)
             | (((CPU_INT32U)csd[7] & 0xFFu) <<  2)
             | (((CPU_INT32U)csd[6] & 0x03u) << 10);
    //                                                             Get dev size mult.
    c_size_mult = (((CPU_INT32U)csd[10] & 0x80u) >> 7)
                  | (((CPU_INT32U)csd[9]  & 0x03u) << 1);

    if (c_size_mult >= 8u) {
      return (DEF_NO);
    }

    mult = FS_SD_Mult[c_size_mult];                             // Calc mult.
    block_nbr = (c_size + 1u) * mult;                           // Calc nbr blks.

    p_sd_info->BlkSize = block_len;
    p_sd_info->NbrBlks = block_nbr;
  }

  //                                                               ------------------- CALC CLK SPD -------------------
  taac = csd[1];                                                // TAAC data access time.
  taac_time = FS_SD_TAAC_TimeUnit[taac & 0x07u];
  taac_time *= FS_SD_TAAC_TimeValue[(taac & 0x78u) >> 3];

  nsac = csd[2];                                                // NSAC data access time.

  tran_spd = csd[3];                                            // Max transfer rate.
  tran_spd_unit = tran_spd & 0x07u;
  if (tran_spd_unit > 3u) {
    return (DEF_NO);
  }
  clk_freq = FS_SD_TranSpdUnit[tran_spd_unit];
  clk_freq *= FS_SD_TranSpdValue[(tran_spd & 0x78u) >> 3];
  p_sd_info->ClkFreq = clk_freq;                                // Max clk freq, in clk/sec.

  clk_freq /= 100u;                                             // Conv to clk/100 us.
  taac_time /= 1000000u;                                        // Conv to 100 us.
  p_sd_info->Timeout = (taac_time * clk_freq) + nsac;           // Timeout, in clks.

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                               FS_SD_ParseCID()
 *
 * @brief    Parse CID data.
 *
 * @param    cid         Array holding CID data.
 *
 * @param    p_sd_info   Pointer to structure into which CID data will be parsed.
 *
 * @param    card_type   Type of card :
 *                           - FS_DEV_SD_CARDTYPE_SD_V1_X       SD card v1.x.
 *                           - FS_DEV_SD_CARDTYPE_SD_V2_0       SD card v2.0, standard capacity.
 *                           - FS_DEV_SD_CARDTYPE_SD_V2_0_HC    SD card v2.0, high capacity.
 *                           - FS_DEV_SD_CARDTYPE_MMC           MMC, standard capacity.
 *                           - FS_DEV_SD_CARDTYPE_MMC_HC        MMC, high capacity.
 *
 * @return   DEF_OK   if the CID could be parsed.
 *           DEF_FAIL if the CID was illegal.
 *
 * @note     (1) For SD cards, the structure of the CID is defined in [Ref 1] Section 5.1.
 *
 *               For MMC cards, the structure of the CID is defined in [Ref 2] Section 8.2.
 *******************************************************************************************************/
void FS_SD_ParseCID(CPU_INT08U cid[],
                    FS_SD_INFO *p_sd_info,
                    CPU_INT08U card_type)
{
  CPU_INT08U manuf_id;
  CPU_INT16U oem_id;
  CPU_INT32U prod_sn;
  CPU_CHAR   prod_name[7];
  CPU_INT16U prod_rev;
  CPU_INT16U date;

  manuf_id = cid[0];

  oem_id = (CPU_INT16U)((CPU_INT16U)cid[1] << 8)
           | (CPU_INT16U)((CPU_INT16U)cid[2] << 0);

  if ((card_type == FS_DEV_SD_CARDTYPE_SD_V1_X)
      || (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0)
      || (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0_HC)) {
    prod_sn = ((CPU_INT32U)cid[9] << 24)
              | ((CPU_INT32U)cid[10] << 16)
              | ((CPU_INT32U)cid[11] <<  8)
              | ((CPU_INT32U)cid[12] <<  0);

    prod_name[0] = (CPU_CHAR)cid[3];
    prod_name[1] = (CPU_CHAR)cid[4];
    prod_name[2] = (CPU_CHAR)cid[5];
    prod_name[3] = (CPU_CHAR)cid[6];
    prod_name[4] = (CPU_CHAR)cid[7];
    prod_name[5] = (CPU_CHAR)'\0';
    prod_name[6] = (CPU_CHAR)'\0';

    prod_rev = ((((CPU_INT16U)cid[8] & 0xF0u) >> 4) * 10u)
               +   ((CPU_INT16U)cid[8] & 0x0Fu);

    date = (((CPU_INT16U)cid[14] & 0x0Fu) * 256u)
           + ((((CPU_INT16U)cid[13] & 0x0Fu) << 4)
              +  (((CPU_INT16U)cid[14] & 0xF0u) >> 4));
  } else {
    prod_sn = ((CPU_INT32U)cid[10] << 24)
              | ((CPU_INT32U)cid[11] << 16)
              | ((CPU_INT32U)cid[12] <<  8)
              | ((CPU_INT32U)cid[13] <<  0);

    prod_name[0] = (CPU_CHAR)cid[3];
    prod_name[1] = (CPU_CHAR)cid[4];
    prod_name[2] = (CPU_CHAR)cid[5];
    prod_name[3] = (CPU_CHAR)cid[6];
    prod_name[4] = (CPU_CHAR)cid[7];
    prod_name[5] = (CPU_CHAR)cid[8];
    prod_name[6] = (CPU_CHAR)'\0';

    prod_rev = (((CPU_INT16U)cid[9] & 0x0Fu) * 10u)
               + (((CPU_INT16U)cid[9] & 0xF0u) >> 4);

    date = ((((CPU_INT16U)cid[14] & 0xF0u) >> 4) * 256u)
           +   ((CPU_INT16U)cid[14] & 0x0Fu);
  }

  p_sd_info->ManufID = manuf_id;
  p_sd_info->OEM_ID = oem_id;
  p_sd_info->ProdSN = prod_sn;
  Mem_Copy(&p_sd_info->ProdName[0], &prod_name[0], 7u);
  p_sd_info->ProdRev = prod_rev;
  p_sd_info->Date = date;
}

/****************************************************************************************************//**
 *                                           FS_SD_ChkSumCalc_7Bit()
 *
 * @brief    Calculate 7-bit Cyclic Redundancy Checksum (CRC).
 *
 * @param    p_data  Pointer to buffer.
 *
 * @param    size    Size of buffer, in octets.
 *
 * @return   7-bit CRC, in 8-bit value.
 *
 * @note     (1) According to [Ref 1], Section 7.2.2 :
 *               - (a) The CRC7 is required for the reset command, CMD0, since the card is still in SD
 *                     mode when this command is received.
 *               - (b) Despite being in SPI mode, the card always verifies the CRC for CMD8.
 *               - (c) Otherwise, unless CMD59 is sent to enable CRC verification, the CRC7 need not
 *                     be calculated.
 *******************************************************************************************************/
CPU_INT08U FS_SD_ChkSumCalc_7Bit(CPU_INT08U *p_data,
                                 CPU_INT32U size)
{
  CPU_INT08U crc;
  CPU_INT08U data;
  CPU_INT08U mask;
  CPU_INT08U bit_ix;

  crc = 0u;

  while (size > 0u) {
    data = *p_data;
    //                                                             Add in uppermost bit of byte.
    mask = (DEF_BIT_IS_SET(data, DEF_BIT_07) == DEF_YES) ? DEF_BIT_06 : DEF_BIT_NONE;
    if (((crc & DEF_BIT_06) ^ mask) != 0x00u) {
      crc = ((CPU_INT08U)(crc << 1)) ^ FS_DEV_SD_CRC7_POLY;
    } else {
      crc <<= 1;
    }

    crc ^= data;                                                // Add in remaining bits of byte.
    for (bit_ix = 0u; bit_ix < 7u; bit_ix++) {
      if (DEF_BIT_IS_SET(crc, DEF_BIT_06) == DEF_YES) {
        crc = ((CPU_INT08U)(crc << 1)) ^ FS_DEV_SD_CRC7_POLY;
      } else {
        crc <<= 1;
      }
    }

    size--;
    p_data++;
  }

  crc &= 0x7Fu;
  return (crc);
}

/****************************************************************************************************//**
 *                                           FS_SD_ChkSumCalc_16Bit()
 *
 * @brief    Calculate 16-bit Cyclic Redundancy Checksum (CRC).
 *
 * @param    p_data  Pointer to buffer.
 *
 * @param    size    Size of buffer, in octets.
 *
 * @return   16-bit CRC.
 *******************************************************************************************************/

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
CPU_INT16U FS_SD_ChkSumCalc_16Bit(CPU_INT08U *p_data,
                                  CPU_INT32U size)
{
  CPU_INT16U crc;

  crc = CRC_ChkSumCalc_16Bit((CRC_MODEL_16 *)&FS_SD_ModelCRC16,
                             p_data,
                             size);
  return (crc);
}
#endif

/****************************************************************************************************//**
 *                                               FS_SD_TraceInfo()
 *
 * @brief    Output SD trace info.
 *
 * @param    p_sd_info   Pointer to SD info.
 *******************************************************************************************************/

#if LOG_VRB_IS_EN()
void FS_SD_TraceInfo(FS_SD_INFO *p_sd_info)
{
  switch (p_sd_info->CardType) {
    case FS_DEV_SD_CARDTYPE_SD_V1_X:
      LOG_VRB(("SD/MMC FOUND:  v1.x SD card"));
      break;

    case FS_DEV_SD_CARDTYPE_SD_V2_0:
      LOG_VRB(("SD/MMC FOUND:  v2.0 standard-capacity SD card"));
      break;

    case FS_DEV_SD_CARDTYPE_SD_V2_0_HC:
      LOG_VRB(("SD/MMC FOUND:  v2.0 high-capacity card"));
      break;

    case FS_DEV_SD_CARDTYPE_MMC:
      LOG_VRB(("SD/MMC FOUND:  standard-capacity MMC card"));
      break;

    case FS_DEV_SD_CARDTYPE_MMC_HC:
      LOG_VRB(("SD/MMC FOUND:  high-capacity MMC card"));
      break;

    default:
      break;
  }

  LOG_VRB(("               Blk Size       : ", (u)p_sd_info->BlkSize, "bytes"));
  LOG_VRB(("               # Blks         : ", (u)p_sd_info->NbrBlks));
  LOG_VRB(("               Max Clk        : ", (u)p_sd_info->ClkFreq, "Hz"));
  LOG_VRB(("               Manufacturer ID: ", (X)p_sd_info->ManufID));
  LOG_VRB(("               OEM/App ID     : ", (X)p_sd_info->OEM_ID));
  LOG_VRB(("               Prod Name      : ", (s) & p_sd_info->ProdName[0]));
  LOG_VRB(("               Prod Rev       : ", (u)p_sd_info->ProdRev / 10u, ".", (u)p_sd_info->ProdRev % 10u));
  LOG_VRB(("               Prod SN        : ", (X)p_sd_info->ProdSN));
  LOG_VRB(("               Date           : ", (u)p_sd_info->Date / 256u, "/", (u)p_sd_info->Date % 256u));
}
#endif

/****************************************************************************************************//**
 *                                               FS_SD_ClrInfo()
 *
 * @brief    Clear SD info.
 *
 * @param    p_sd_info   Pointer to SD info.
 *******************************************************************************************************/
void FS_SD_ClrInfo(FS_SD_INFO *p_sd_info)
{
  p_sd_info->BlkSize = 0u;
  p_sd_info->NbrBlks = 0u;
  p_sd_info->ClkFreq = 0u;
  p_sd_info->Timeout = 0u;
  p_sd_info->CardType = FS_DEV_SD_CARDTYPE_NONE;
  p_sd_info->HighCapacity = DEF_NO;

  p_sd_info->ManufID = 0u;
  p_sd_info->OEM_ID = 0u;
  p_sd_info->ProdSN = 0u;
  Mem_Set(&p_sd_info->ProdName[0], (CPU_INT08U)'\0', 7u);
  p_sd_info->ProdRev = 0u;
  p_sd_info->Date = 0u;
}

/****************************************************************************************************//**
 *                                           FS_SD_ParseEXT_CSD()
 *
 * @brief    Parse MMC's Extended CSD (Card Specific Data) register data.
 *
 * @param    ext_csd     Array holding Extended CSD data.
 *
 * @param    p_sd_info   Pointer to structure into which Extended CSD data will be parsed.
 *
 * @return   DEF_OK,    if the Extended CSD could be parsed.
 *           DEF_FAIL,  if the Extended CSD was illegal.
 *******************************************************************************************************/
void FS_SD_ParseEXT_CSD(CPU_INT08U ext_csd[],
                        FS_SD_INFO *p_sd_info)
{
  CPU_INT32U size;

  size = (((CPU_INT32U)ext_csd[215]) << 24)                     // Get dev size.
         | (((CPU_INT32U)ext_csd[214]) << 16)
         | (((CPU_INT32U)ext_csd[213]) <<  8)
         | (((CPU_INT32U)ext_csd[212]) <<  0);

  p_sd_info->NbrBlks = size;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL || RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL
