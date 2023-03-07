/***************************************************************************//**
 * @file
 * @brief File System - Fat Fat32 Operations
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

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <fs_core_cfg.h>

#if (FS_CORE_CFG_FAT_EN == DEF_ENABLED)

#include  <fs/source/sys/fat/fs_fat_fatxx_priv.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>

#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                                    RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                                        (FS, FAT, FATXX)

#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
#define  FS_FAT_FAT12_CLUS_BAD                        0x0FF7u
#define  FS_FAT_FAT12_CLUS_EOF                        0x0FF8u
#define  FS_FAT_FAT12_CLUS_FREE                       0x0000u
#endif

#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
#define  FS_FAT_FAT16_CLUS_BAD                        0xFFF7u
#define  FS_FAT_FAT16_CLUS_EOF                        0xFFF8u
#define  FS_FAT_FAT16_CLUS_FREE                       0x0000u
#endif

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
#define  FS_FAT_FAT32_CLUS_BAD                    0x0FFFFFF7u
#define  FS_FAT_FAT32_CLUS_EOF                    0x0FFFFFF8u
#define  FS_FAT_FAT32_CLUS_FREE                   0x00000000u
#define  FS_FAT_FAT32_CLUS_MASK                   0x0FFFFFFFu
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_FAT12_ClusValWr(FS_FAT_VOL      *p_vol,
                                   FS_FAT_CLUS_NBR clus,
                                   FS_FAT_CLUS_NBR val,
                                   RTOS_ERR        *p_err);
#endif

static FS_FAT_CLUS_NBR FS_FAT_FAT12_ClusValRd(FS_FAT_VOL      *p_vol,
                                              FS_FAT_CLUS_NBR clus,
                                              RTOS_ERR        *p_err);
#endif

#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_FAT16_ClusValWr(FS_FAT_VOL      *p_vol,
                                   FS_FAT_CLUS_NBR clus,
                                   FS_FAT_CLUS_NBR val,
                                   RTOS_ERR        *p_err);
#endif

static FS_FAT_CLUS_NBR FS_FAT_FAT16_ClusValRd(FS_FAT_VOL      *p_vol,
                                              FS_FAT_CLUS_NBR clus,
                                              RTOS_ERR        *p_err);
#endif

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_FAT32_ClusValWr(FS_FAT_VOL      *p_vol,
                                   FS_FAT_CLUS_NBR clus,
                                   FS_FAT_CLUS_NBR val,
                                   RTOS_ERR        *p_err);
#endif

static FS_FAT_CLUS_NBR FS_FAT_FAT32_ClusValRd(FS_FAT_VOL      *p_vol,
                                              FS_FAT_CLUS_NBR clus,
                                              RTOS_ERR        *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERFACE STRUCTURE
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
static FS_FAT_TYPE_API FS_FAT_FAT12_API = {
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  FS_FAT_FAT12_ClusValWr,
#endif
  FS_FAT_FAT12_ClusValRd,

  FS_FAT_FAT12_CLUS_BAD,
  FS_FAT_FAT12_CLUS_EOF,
  FS_FAT_FAT12_CLUS_FREE
};
#endif

#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
static FS_FAT_TYPE_API FS_FAT_FAT16_API = {
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  FS_FAT_FAT16_ClusValWr,
#endif
  FS_FAT_FAT16_ClusValRd,

  FS_FAT_FAT16_CLUS_BAD,
  FS_FAT_FAT16_CLUS_EOF,
  FS_FAT_FAT16_CLUS_FREE
};
#endif

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
static FS_FAT_TYPE_API FS_FAT_FAT32_API = {
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  FS_FAT_FAT32_ClusValWr,
#endif
  FS_FAT_FAT32_ClusValRd,

  FS_FAT_FAT32_CLUS_BAD,
  FS_FAT_FAT32_CLUS_EOF,
  FS_FAT_FAT32_CLUS_FREE
};
#endif

FS_FAT_TYPE_API * const FS_FAT_TYPE_API_TBL[] = {
#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
  &FS_FAT_FAT12_API,
#endif
#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
  &FS_FAT_FAT16_API,
#endif
#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
  &FS_FAT_FAT32_API
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_FAT12_ClusValWr()
 *
 * @brief   Write value into cluster.
 *
 * @param   p_vol   Pointer to volume.
 *
 * @param   clus    Cluster to modify.
 *
 * @param   val     Value to write into cluster.
 *
 * @param   p_err   Error pointer.
 *
 * @note    (1) This function implements the FAT12 entry logic described in section '4.1: Determination
 *              of FAT entry for a cluster' of "Microsoft FAT Specification, August 30 2005".
 *              In this logic, the key notion is that FAT as an array of bytes (8 bit quantities). From
 *              that, the function uses certain computations to access the right location of the entry
 *              within the FAT array. The location computation varies with the cluster number parity.
 *              All 12-bit entries are packed to avoid wasting bits.
 *******************************************************************************************************/

#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_FAT12_ClusValWr(FS_FAT_VOL      *p_vol,
                                   FS_FAT_CLUS_NBR clus,
                                   FS_FAT_CLUS_NBR val,
                                   RTOS_ERR        *p_err)
{
  CPU_INT08U      *p_buf;
  FS_LB_SIZE      fat_offset;
  FS_FAT_SEC_NBR  fat_sec;
  FS_LB_SIZE      fat_sec_offset;
  FS_FAT_SEC_NBR  fat_start_sec;
  FS_FAT_CLUS_NBR val_temp;
  CPU_INT08U      lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->Vol.BlkDevHandle, p_err));

    fat_start_sec = p_vol->FAT1_Start;
    fat_offset = (FS_LB_SIZE)clus + ((FS_LB_SIZE)clus / 2u);    // Compute entry offset relative to FAT table start.
                                                                // Convert entry offset into sector number.
    fat_sec = fat_start_sec + (FS_FAT_SEC_NBR)FS_UTIL_DIV_PWR2(fat_offset, lb_size_log2);
    //                                                             Compute entry offset relative to nearest sector start
    fat_sec_offset = FS_UTIL_MODULO_PWR2(fat_offset, lb_size_log2);

    //                                                             -------------------- WR (SPLIT) --------------------
    if (fat_sec_offset == FS_UTIL_PWR2(lb_size_log2) - 1u) {    // Check for the entry spanning a sector boundary.
      BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW(&p_vol->Vol,             // Get first FAT sector.
                                       fat_sec,
                                       FS_FAT_LB_TYPE_FAT,
                                       p_vol->FatWrJobHandle,
                                       &p_buf,
                                       &p_vol->FatWrJobHandle,
                                       p_err)) {
        if (FS_UTIL_IS_ODD(clus) == DEF_YES) {                  // Write 4 useful bits if odd cluster number.
          val_temp = MEM_VAL_GET_INT08U((void *)(p_buf + fat_sec_offset));
          val_temp &= DEF_NIBBLE_MASK;
          val_temp |= (val & DEF_NIBBLE_MASK) << DEF_NIBBLE_NBR_BITS;
        } else {                                                // Write 8 useful bits if even cluster number.
          val_temp = val;
        }
        MEM_VAL_SET_INT08U((void *)(p_buf + fat_sec_offset), val_temp & DEF_OCTET_MASK);
      }

      BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_vol->Vol,            // Get next FAT sector.
                                        fat_sec + 1u,
                                        FS_FAT_LB_TYPE_FAT,
                                        p_vol->FatWrJobHandle,
                                        &p_buf,
                                        &p_vol->FatWrJobHandle,
                                        p_err) {
        if (FS_UTIL_IS_ODD(clus) == DEF_YES) {                  // Write 8 useful bits if odd cluster number.
          val_temp = val >> DEF_NIBBLE_NBR_BITS;
        } else {                                                // Write 4 useful bits if even cluster number.
          val_temp = MEM_VAL_GET_INT08U(p_buf);
          val_temp &= ~DEF_NIBBLE_MASK;
          val_temp |= (val >> DEF_OCTET_NBR_BITS) & DEF_NIBBLE_MASK;
        }
        MEM_VAL_SET_INT08U(p_buf, val_temp);
      }
    } else {                                                    // ------------------ WR (NOT SPLIT) ------------------
                                                                // Read one FAT sector.
      BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_vol->Vol,
                                        fat_sec,
                                        FS_FAT_LB_TYPE_FAT,
                                        p_vol->FatWrJobHandle,
                                        &p_buf,
                                        &p_vol->FatWrJobHandle,
                                        p_err) {
        //                                                         Get full 16-bit value at given offset.
        val_temp = MEM_VAL_GET_INT16U_LITTLE((void *)(p_buf + fat_sec_offset));
        //                                                         Prepare 12 bits according to cluster number parity.
        if (FS_UTIL_IS_ODD(clus) == DEF_YES) {
          val_temp = val_temp & 0x000Fu;
          val <<= DEF_NIBBLE_NBR_BITS;
        } else {
          val_temp = val_temp & 0xF000u;
        }
        val_temp |= val;
        //                                                         FAT entries always written in little-endian.
        MEM_VAL_SET_INT16U_LITTLE((void *)(p_buf + fat_sec_offset), val_temp);
      }
    }
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FAT12_ClusValRd()
 *
 * @brief   Read value from cluster.
 *
 * @param   p_vol   Pointer to volume.
 *
 * @param   clus    Cluster to read.
 *
 * @param   p_err   Error pointer.
 *
 * @return  Cluster value.
 *
 * @note    (1) See Note #1 of FS_FAT_FAT12_ClusValWr().
 *******************************************************************************************************/
#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
static FS_FAT_CLUS_NBR FS_FAT_FAT12_ClusValRd(FS_FAT_VOL      *p_vol,
                                              FS_FAT_CLUS_NBR clus,
                                              RTOS_ERR        *p_err)
{
  CPU_INT08U      *p_buf;
  FS_LB_SIZE      fat_offset;
  FS_FAT_SEC_NBR  fat_sec;
  FS_LB_SIZE      fat_sec_offset;
  FS_FAT_SEC_NBR  fat_start_sec;
  FS_FAT_CLUS_NBR val;
  FS_FAT_CLUS_NBR val_temp = 0u;
  CPU_INT08U      lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    val = 0u;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->Vol.BlkDevHandle, p_err));

    fat_start_sec = p_vol->FAT1_Start;
    fat_offset = (FS_LB_SIZE)clus + ((FS_LB_SIZE)clus / 2u);    // Compute entry offset relative to FAT table start.
                                                                // Convert entry offset into sector number.
    fat_sec = fat_start_sec + (FS_FAT_SEC_NBR)FS_UTIL_DIV_PWR2(fat_offset, lb_size_log2);
    //                                                             Compute entry offset relative to nearest sector start
    fat_sec_offset = FS_UTIL_MODULO_PWR2(fat_offset, lb_size_log2);

    //                                                             -------------------- RD (SPLIT) --------------------
    if (fat_sec_offset == FS_UTIL_PWR2(lb_size_log2) - 1u) {    // Check for the entry spanning a sector boundary.
                                                                // Read first FAT sector.
      BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(&p_vol->Vol,
                                        fat_sec,
                                        FS_FAT_LB_TYPE_FAT,
                                        &p_buf,
                                        p_err) {
        //                                                         Get part of 12-bit entry.
        val_temp = MEM_VAL_GET_INT08U((void *)(p_buf + fat_sec_offset));
      }

      //                                                           Read second FAT sector.
      BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(&p_vol->Vol,
                                        fat_sec + 1u,
                                        FS_FAT_LB_TYPE_FAT,
                                        &p_buf,
                                        p_err) {
        //                                                         Get remaining part of 12-bit entry.
        val_temp |= MEM_VAL_GET_INT08U((void *)(p_buf)) << DEF_OCTET_NBR_BITS;
      }
    } else {                                                    // ------------------ RD (NOT SPLIT) ------------------
                                                                // Read one FAT sector.
      BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(&p_vol->Vol,
                                        fat_sec,
                                        FS_FAT_LB_TYPE_FAT,
                                        &p_buf,
                                        p_err) {
        //                                                         Get full 16-bit value at given offset.
        val_temp = MEM_VAL_GET_INT16U_LITTLE((void *)(p_buf + fat_sec_offset));
      }
    }
    //                                                             Get proper 12 bits according to cluster number parity
    if (FS_UTIL_IS_ODD(clus) == DEF_YES) {
      val = (val_temp & 0xFFF0u) >> DEF_NIBBLE_NBR_BITS;
    } else {
      val = (val_temp & 0x0FFFu);
    }
  } WITH_SCOPE_END

  return (val);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FAT16_ClusValWr()
 *
 * @brief    Write value into cluster.
 *
 * @param    p_vol   Pointer to volume.
 *
 * @param    clus    Cluster to modify.
 *
 * @param    val     Value to write into cluster.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/

#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_FAT16_ClusValWr(FS_FAT_VOL      *p_vol,
                                   FS_FAT_CLUS_NBR clus,
                                   FS_FAT_CLUS_NBR val,
                                   RTOS_ERR        *p_err)
{
  CPU_INT08U     *p_buf;
  FS_LB_SIZE     fat_offset;
  FS_FAT_SEC_NBR fat_sec;
  FS_LB_SIZE     fat_sec_offset;
  FS_FAT_SEC_NBR fat_start_sec;
  CPU_INT08U     lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->Vol.BlkDevHandle, p_err));

    fat_start_sec = p_vol->FAT1_Start;
    fat_offset = (FS_LB_SIZE)clus * FS_FAT_FAT16_ENTRY_NBR_OCTETS;
    fat_sec = fat_start_sec + (FS_FAT_SEC_NBR)FS_UTIL_DIV_PWR2(fat_offset, lb_size_log2);
    fat_sec_offset = FS_UTIL_MODULO_PWR2(fat_offset, lb_size_log2);

    //                                                             Rd FAT sec.
    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_vol->Vol,
                                      fat_sec,
                                      FS_FAT_LB_TYPE_FAT,
                                      p_vol->FatWrJobHandle,
                                      &p_buf,
                                      &p_vol->FatWrJobHandle,
                                      p_err) {
      MEM_VAL_SET_INT16U_LITTLE((void *)(p_buf + fat_sec_offset), val);
    }
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FAT16_ClusValRd()
 *
 * @brief    Read value from cluster.
 *
 * @param    p_vol   Pointer to volume.
 *
 * @param    clus    Cluster to read.
 *
 * @param    p_err   Error pointer
 *
 * @return   Cluster value.
 *******************************************************************************************************/

#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
static FS_FAT_CLUS_NBR FS_FAT_FAT16_ClusValRd(FS_FAT_VOL      *p_vol,
                                              FS_FAT_CLUS_NBR clus,
                                              RTOS_ERR        *p_err)
{
  CPU_INT08U      *p_buf;
  FS_LB_SIZE      fat_offset;
  FS_FAT_SEC_NBR  fat_sec;
  FS_LB_SIZE      fat_sec_offset;
  FS_FAT_SEC_NBR  fat_start_sec;
  FS_FAT_CLUS_NBR val;
  CPU_INT08U      lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    val = 0u;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->Vol.BlkDevHandle, p_err));

    fat_start_sec = p_vol->FAT1_Start;
    fat_offset = (FS_LB_SIZE)clus * FS_FAT_FAT16_ENTRY_NBR_OCTETS;
    fat_sec = fat_start_sec + (FS_FAT_SEC_NBR)FS_UTIL_DIV_PWR2(fat_offset, lb_size_log2);
    fat_sec_offset = FS_UTIL_MODULO_PWR2(fat_offset, lb_size_log2);

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(&p_vol->Vol,
                                      fat_sec,
                                      FS_FAT_LB_TYPE_FAT,
                                      &p_buf,
                                      p_err) {
      val = MEM_VAL_GET_INT16U_LITTLE((void *)(p_buf + fat_sec_offset));
    }
  } WITH_SCOPE_END

  return (val);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FAT32_ClusValWr()
 *
 * @brief    Write value into cluster.
 *
 * @param    p_vol   Pointer to volume.
 *
 * @param    clus    Cluster to modify.
 *
 * @param    val     Value to write into cluster.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The FAT entry on a FAT32 volume is a 28-bit value stored in 32-bit datum; the upper
 *               four bits, which are not valid bits, are not modified.
 *******************************************************************************************************/

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_FAT32_ClusValWr(FS_FAT_VOL      *p_vol,
                                   FS_FAT_CLUS_NBR clus,
                                   FS_FAT_CLUS_NBR val,
                                   RTOS_ERR        *p_err)
{
  CPU_INT08U     *p_buf;
  FS_LB_SIZE     fat_offset;
  FS_FAT_SEC_NBR fat_sec;
  FS_LB_SIZE     fat_sec_offset;
  FS_FAT_SEC_NBR fat_start_sec;
  CPU_INT32U     val_temp;
  CPU_INT08U     lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->Vol.BlkDevHandle, p_err));

    fat_start_sec = p_vol->FAT1_Start;
    fat_offset = (FS_LB_SIZE)clus * FS_FAT_FAT32_ENTRY_NBR_OCTETS;
    fat_sec = fat_start_sec + (FS_FAT_SEC_NBR)FS_UTIL_DIV_PWR2(fat_offset, lb_size_log2);
    fat_sec_offset = FS_UTIL_MODULO_PWR2(fat_offset, lb_size_log2);

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_vol->Vol,
                                      fat_sec,
                                      FS_FAT_LB_TYPE_FAT,
                                      p_vol->FatWrJobHandle,
                                      &p_buf,
                                      &p_vol->FatWrJobHandle,
                                      p_err) {
      val_temp = MEM_VAL_GET_INT32U_LITTLE((void *)(p_buf + fat_sec_offset));
      val_temp &= ~FS_FAT_FAT32_CLUS_MASK;                      // Keep upper entry bits (see Note #1).
      val_temp |= val;
      MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + fat_sec_offset), val_temp);
    }
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FAT32_ClusValRd()
 *
 * @brief    Read value from cluster.
 *
 * @param    p_vol   Pointer to volume.
 *
 * @param    clus    Cluster to read.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Cluster value.
 *
 * @note     (1) The FAT entry on a FAT32 volume is a 28-bit value stored in 32-bit datum; the upper
 *               four bits, which are not valid bits, are masked off the returned value.
 *******************************************************************************************************/

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
static FS_FAT_CLUS_NBR FS_FAT_FAT32_ClusValRd(FS_FAT_VOL      *p_vol,
                                              FS_FAT_CLUS_NBR clus,
                                              RTOS_ERR        *p_err)
{
  CPU_INT08U      *p_buf;
  CPU_INT08U      lb_size_log2;
  FS_LB_SIZE      fat_offset;
  FS_FAT_SEC_NBR  fat_sec;
  FS_LB_SIZE      fat_sec_offset;
  FS_FAT_SEC_NBR  fat_start_sec;
  FS_FAT_CLUS_NBR val;

  WITH_SCOPE_BEGIN(p_err) {
    val = 0u;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->Vol.BlkDevHandle, p_err));

    fat_start_sec = p_vol->FAT1_Start;
    fat_offset = (FS_LB_SIZE)clus * FS_FAT_FAT32_ENTRY_NBR_OCTETS;
    fat_sec = fat_start_sec + (FS_FAT_SEC_NBR)FS_UTIL_DIV_PWR2(fat_offset, lb_size_log2);
    fat_sec_offset = FS_UTIL_MODULO_PWR2(fat_offset, lb_size_log2);

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(&p_vol->Vol,
                                      fat_sec,
                                      FS_FAT_LB_TYPE_FAT,
                                      &p_buf,
                                      p_err) {
      val = MEM_VAL_GET_INT32U_LITTLE((void *)(p_buf + fat_sec_offset));
      val &= FS_FAT_FAT32_CLUS_MASK;                            // Mask off upper entry bits (see Note #1).
    }
  } WITH_SCOPE_END

  return (val);
}
#endif

#endif // FS_CORE_CFG_FAT_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
