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

/****************************************************************************************************//**
 * @addtogroup FS_STORAGE
 * @{
 ********************************************************************************************************
 * @note    (1) Supports NOR-type Flash memory devices, including :
 *             - (a) Parallel NOR Flash.
 *             - (b) Serial NOR Flash (SPI and QSPI).
 *
 * @note     (2) Supported media MUST have the following characteristics :
 *             - (a) Medium organized into units (called blocks) which are erased at the same time.
 *             - (b) When erased, all bits are 1.
 *             - (c) Only an erase operation can change a bit from a 0 to a 1.
 *             - (d) Any bit  can be individually programmed  from a 1 to a 0.
 *             - (e) Any word can be individually accessed (read or programmed).
 *
 * @note     (3) Supported media TYPICALLY have the following characteristics :
 *             - (a) A program operation takes much longer than a read    operation.
 *             - (b) An erase  operation takes much longer than a program operation.
 *             - (c) The number of erase operations per block is limited.
 *
 * @note     (4) Some NOR media have non-uniform layouts, e.g. not all erase blocks are the same
 *               size.  Often, smaller blocks provide code storage in the top or bottom portion of
 *               the memory.  This driver REQUIRES that the medium be uniform, only a uniform portion
 *               of the device be used, or multiple blocks in regions of smaller blocks be represented
 *               as a single block by the physical-layer driver.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_NOR_H_
#define  FS_NOR_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs/include/fs_blk_dev.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) The commands supported by flash devices follow in general this command frame structure:
 *
 *               COMMAND OPCODE | ADDRESS | OPTION | DUMMY CYCLES | DATA
 *
 *               (a) Command opcode (8 bits): commands supported by flash devices. Commands are
 *                                               vendor-specific.
 *               (b) Address (24 or 32 bits): address is required for commands such as READ, PROGRAM,
 *                                               ERASE. Default address is 24-bit long. For flash devices
 *                                               with a size larger than 128 Mbit (16 MB), 32-bit address
 *                                               is needed.
 *               (c) Option                 : useful to activate eXecute-In-Place (XIP) mode or continuous
 *                                               mode for READ command in some serial flash devices. It
 *                                               improves the data read latency.
 *               (d) Dummy cycles           : required by some READ commands to allow flash devices to
 *                                               prepare read data for host.
 *               (e) Data                   : data payload associated to READ or PROGRAM (i.e. write)
 *                                               commands.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NOR_PHY_SIZE_LOG2_4Gb                   29u
#define  FS_NOR_PHY_SIZE_LOG2_2Gb                   28u
#define  FS_NOR_PHY_SIZE_LOG2_1Gb                   27u
#define  FS_NOR_PHY_SIZE_LOG2_512Mb                 26u
#define  FS_NOR_PHY_SIZE_LOG2_256Mb                 25u
#define  FS_NOR_PHY_SIZE_LOG2_128Mb                 24u
#define  FS_NOR_PHY_SIZE_LOG2_64Mb                  23u
#define  FS_NOR_PHY_SIZE_LOG2_32Mb                  22u
#define  FS_NOR_PHY_SIZE_LOG2_16Mb                  21u
#define  FS_NOR_PHY_SIZE_LOG2_8Mb                   20u

#define  FS_NOR_PHY_SIZE_2Gb                        0x10000000u
#define  FS_NOR_PHY_SIZE_1Gb                        0x08000000u
#define  FS_NOR_PHY_SIZE_512Mb                      0x04000000u
#define  FS_NOR_PHY_SIZE_256Mb                      0x02000000u
#define  FS_NOR_PHY_SIZE_128Mb                      0x01000000u
#define  FS_NOR_PHY_SIZE_64Mb                       0x00800000u
#define  FS_NOR_PHY_SIZE_32Mb                       0x00400000u
#define  FS_NOR_PHY_SIZE_16Mb                       0x00200000u
#define  FS_NOR_PHY_SIZE_8Mb                        0x00100000u

#define  FS_NOR_PHY_SIZE_128Mb_AND_LESS_3B_ADDR     3u
#define  FS_NOR_PHY_SIZE_128Mb_AND_MORE_4B_ADDR     4u

#define  FS_NOR_PHY_BLK_SIZE_32K                    (32u * 1024u)
#define  FS_NOR_PHY_BLK_SIZE_64K                    (64u * 1024u)

#define  FS_NOR_PHY_FLASH_MEM_MAP_NONE              0x00000000u
//                                                                 See Note #1.
#define  FS_NOR_PHY_CMD_DUMMY_CYCLES_NONE           0u
#define  FS_NOR_PHY_CMD_DATA_PHASE_NONE             0u
#define  FS_NOR_PHY_CMD_ADDR_PHASE_NONE_PTR         DEF_NULL
#define  FS_NOR_PHY_CMD_DUMMY_CYCLES_NONE_PTR       DEF_NULL
#define  FS_NOR_PHY_CMD_DATA_PHASE_NONE_PTR         DEF_NULL

#define  FS_NOR_SERIAL_BUS_WIDTH_SINGLE_IO          1u
#define  FS_NOR_SERIAL_BUS_WIDTH_DUAL_IO            2u
#define  FS_NOR_SERIAL_BUS_WIDTH_QUAD_IO            4u
#define  FS_NOR_SERIAL_BUS_WIDTH_OCTAL_IO           8u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct fs_nor_phy_api FS_NOR_PHY_API;
typedef struct fs_nor_quad_spi_hw_info FS_NOR_QUAD_SPI_HW_INFO;

/*
 ********************************************************************************************************
 *                                        NOR handle data type
 *******************************************************************************************************/

typedef struct fs_nor_handle {
  FS_MEDIA_HANDLE MediaHandle;                                  ///< Generic media handle.
} FS_NOR_HANDLE;

/*
 ********************************************************************************************************
 *                                     NOR flash part information
 *******************************************************************************************************/

typedef struct fs_nor_part_info {
  CPU_INT32U PhyApiPtr;                                         ///< Pointer to NOR PHY driver API.
  CPU_INT08U ChipSelID;                                         ///< NOR chip slave ID to en/dis it from MCU.
} FS_NOR_PART_INFO;

/*
 ********************************************************************************************************
 *                                  Generic NOR platform manager item
 *******************************************************************************************************/

struct fs_nor_pm_item {
  struct fs_media_pm_item MediaPmItem;                          ///< Platform Manager item for media.
  const FS_NOR_PART_INFO  *PartInfoPtr;                         ///< Pointer to NOR flash device info.
};

/*
 ********************************************************************************************************
 *                              Serial NOR Quad SPI platform manager item
 *******************************************************************************************************/

struct fs_nor_quad_spi_pm_item {
  struct fs_nor_pm_item         NorPmItem;                      ///< Item for NOR media type.
  const FS_NOR_QUAD_SPI_HW_INFO *HwInfoPtr;                     ///< Pointer to Quad SPI controller info.
};

/********************************************************************************************************
 *                                           PARALLEL BSP INTERFACES
 *******************************************************************************************************/

typedef CPU_BOOLEAN (*FS_NOR_PARALLEL_POLL_FNCT)(void *p_phy_data_arg);
//                                                                 *INDENT-OFF*
typedef struct fs_nor_parallel_bsp_api {
  void *(*Add)(RTOS_ERR *p_err);

  CPU_BOOLEAN (*Open)(void       *p_bsp_data,
                      CPU_ADDR   addr_base,
                      CPU_INT08U bus_width,
                      CPU_INT08U phy_dev_cnt);

  void (*Close)(void *p_bsp_data);

  void (*Rd_08)(void       *p_bsp_data,
                void       *p_dest,
                CPU_ADDR   addr_src,
                CPU_INT32U cnt);

  void (*Rd_16)(void       *p_bsp_data,
                void       *p_dest,
                CPU_ADDR   addr_src,
                CPU_INT32U cnt);

  CPU_INT08U (*RdWord_08)(void     *p_bsp_data,
                          CPU_ADDR addr_src);

  CPU_INT16U (*RdWord_16)(void     *p_bsp_data,
                          CPU_ADDR addr_src);

  void (*WrWord_08)(void       *p_bsp_data,
                    CPU_ADDR   addr_dest,
                    CPU_INT08U datum);

  void (*WrWord_16)(void       *p_bsp_data,
                    CPU_ADDR   addr_dest,
                    CPU_INT16U datum);

  CPU_BOOLEAN (*WaitWhileBusy)(void                      *p_bsp_data,
                               void                      *p_phy_data,
                               FS_NOR_PARALLEL_POLL_FNCT poll_fnct,
                               CPU_INT32U                to_us);
} FS_NOR_PARALLEL_BSP_API;
//                                                                 *INDENT-ON*
/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NOR_PHY_HANDLE_IS_NULL(phy_handle)        (phy_handle.NorPhyPtr == DEF_NULL)

#define  FS_NOR_QUAD_SPI_HW_INFO_REG(p_name, p_hw_info)                                    \
  do {                                                                                     \
    if (((FS_NOR_QUAD_SPI_HW_INFO *)(p_hw_info))->CtrlrHwInfoPtr != DEF_NULL) {            \
      static const struct fs_nor_quad_spi_pm_item _pm_item = {                             \
        .NorPmItem.MediaPmItem.PmItem.StrID = p_name,                                      \
        .NorPmItem.MediaPmItem.PmItem.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,        \
        .NorPmItem.MediaPmItem.MediaApiPtr = &FS_NOR_MediaApi,                             \
        .NorPmItem.PartInfoPtr = (const  FS_NOR_PART_INFO *)(p_hw_info),                   \
        .HwInfoPtr = (FS_NOR_QUAD_SPI_HW_INFO *)(p_hw_info)                                \
      };                                                                                   \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_pm_item.NorPmItem.MediaPmItem.PmItem); \
    }                                                                                      \
  } while (0);

#define  FS_NOR_SPI_HW_INFO_REG(p_name, p_part_info, p_ctrlr_name, cs_func)                \
  do {                                                                                     \
    if (((FS_NOR_PART_INFO *)(p_part_info))->PhyApiPtr != DEF_NULL) {                      \
      static const struct fs_nor_spi_pm_item _pm_item = {                                  \
        .NorPmItem.MediaPmItem.PmItem.StrID = p_name,                                      \
        .NorPmItem.MediaPmItem.PmItem.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,        \
        .NorPmItem.MediaPmItem.MediaApiPtr = &FS_NOR_MediaApi,                             \
        .NorPmItem.PartInfoPtr = (const struct fs_nor_part_info *)(p_part_info),           \
        .SpiCtrlrNamePtr = p_ctrlr_name,                                                   \
        .ChipSel = cs_func                                                                 \
      };                                                                                   \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_pm_item.NorPmItem.MediaPmItem.PmItem); \
    }                                                                                      \
  } while (0);

#define  FS_NOR_SPIDRV_HW_INFO_REG(p_name, p_hw_info)                                      \
  do {                                                                                     \
    if (((FS_NOR_QUAD_SPI_HW_INFO *)(p_hw_info))->CtrlrHwInfoPtr != DEF_NULL) {            \
      static const struct fs_nor_quad_spi_pm_item _pm_item = {                             \
        .NorPmItem.MediaPmItem.PmItem.StrID = p_name,                                      \
        .NorPmItem.MediaPmItem.PmItem.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,        \
        .NorPmItem.MediaPmItem.MediaApiPtr = &FS_NOR_MediaApi,                             \
        .NorPmItem.PartInfoPtr = (const  FS_NOR_PART_INFO *)(p_hw_info),                   \
        .HwInfoPtr = (FS_NOR_QUAD_SPI_HW_INFO *)(p_hw_info)                                \
      };                                                                                   \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_pm_item.NorPmItem.MediaPmItem.PmItem); \
    }                                                                                      \
  } while (0);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_NOR_HANDLE FS_NOR_NullHandle;

extern const struct fs_media_api FS_NOR_MediaApi;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

FS_NOR_HANDLE FS_NOR_Open(FS_MEDIA_HANDLE media_handle,
                          RTOS_ERR        *p_err);

void FS_NOR_Close(FS_NOR_HANDLE nor_handle,
                  RTOS_ERR      *p_err);

void FS_NOR_Rd(FS_NOR_HANDLE nor_handle,
               void          *p_dest,
               CPU_INT32U    start_addr,
               CPU_INT32U    cnt,
               RTOS_ERR      *p_err);

void FS_NOR_Wr(FS_NOR_HANDLE nor_handle,
               void          *p_src,
               CPU_INT32U    start_addr,
               CPU_INT32U    cnt,
               RTOS_ERR      *p_err);

void FS_NOR_BlkErase(FS_NOR_HANDLE nor_handle,
                     CPU_INT32U    blk_ix,
                     RTOS_ERR      *p_err);

void FS_NOR_ChipErase(FS_NOR_HANDLE nor_handle,
                      RTOS_ERR      *p_err);

void FS_NOR_XIP_Cfg(FS_NOR_HANDLE nor_handle,
                    CPU_BOOLEAN   xip_en,
                    RTOS_ERR      *p_err);

FS_NOR_HANDLE FS_NOR_Get(FS_MEDIA_HANDLE media_handle);

CPU_INT08U FS_NOR_BlkSizeLog2Get(FS_NOR_HANDLE nor_handle,
                                 RTOS_ERR      *p_err);

CPU_INT32U FS_NOR_BlkCntGet(FS_NOR_HANDLE nor_handle,
                            RTOS_ERR      *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                         MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
