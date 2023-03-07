/***************************************************************************//**
 * @file
 * @brief File System - Quad SPI Management Layer
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
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_NOR_QUAD_SPI_H_
#define  FS_NOR_QUAD_SPI_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_mem.h>
#include  <cpu/include/cpu.h>
#include  <fs/include/fs_nor.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NOR_SPI_MODE_0      0u
#define  FS_NOR_SPI_MODE_1      1u
#define  FS_NOR_SPI_MODE_2      2u
#define  FS_NOR_SPI_MODE_3      3u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct fs_nor_quad_spi_drv_api FS_NOR_QUAD_SPI_DRV_API;
typedef struct fs_nor_quad_spi_bsp_api FS_NOR_QUAD_SPI_BSP_API;

/****************************************************************************************************//**
 *                                       QUAD SPI CONTROLLER INFO
 *
 * @note     (1) The Quad SPI controller supports a functional mode called memory-mapped or direct mode.
 *               In memory-mapped mode, the external flash memory is mapped to the microcontroller address
 *               space and is seen by the system as if it was an internal memory. This mode allows to both
 *               access and directly execute code from the external flash memory. The memory-mapped works
 *               with the NOR flash device configured in XIP (eXecute In Place) mode.
 *               When the external flash device is memory-mapped, an address range is dedicated to it
 *               besides the QSPI registers address range.
 *******************************************************************************************************/

//                                                                 ------------- QUAD SPI CONTROLLER INFO -------------
typedef struct fs_nor_quad_spi_ctrlr_info {
  FS_NOR_QUAD_SPI_DRV_API *DrvApiPtr;                           // Pointer to QSPI driver API.
  FS_NOR_QUAD_SPI_BSP_API *BspApiPtr;                           // Pointer to QSPI BSP API.
  CPU_ADDR                BaseAddr;                             // QSPI controller register base address.
  CPU_SIZE_T              AlignReq;                             // Buffer alignment requirement by QSPI if DMA used.
  CPU_INT32U              FlashMemMapStartAddr;                 // NOR flash seen as MCU internal memory (see Note #1).
  CPU_INT08U              BusWidth;                             // Width of data bus.
} FS_NOR_QUAD_SPI_CTRLR_INFO;

//                                                                 ----------------- NOR MEMORY INFO ------------------
typedef struct fs_nor_quad_spi_slave_info {
  CPU_INT08U  Mode;                                             // SPI mode (phase and polarity).
  CPU_BOOLEAN LSbFirst;                                         // Flag indicating if LSb send first on data line.
  CPU_INT08U  IdleByte;
  CPU_BOOLEAN HasDTR;                                           // Flag indicating if double transfer rate supported.
  CPU_INT32U  ClkFreqMax;                                       // Maximum serial clock frequency.
  CPU_INT32U  PageSize;                                         // Flash page size in bytes.
  CPU_INT32U  BlkSizeLog2;                                      // Flash blk size expressed in 2^power.
  CPU_INT08U  ChipSelID;                                        // Chip select ID to which flash device is connected.
} FS_NOR_QUAD_SPI_SLAVE_INFO;

//                                                                 --------------- NOR QUAD SPI HW INFO ---------------
struct fs_nor_quad_spi_hw_info {
  FS_NOR_PART_INFO                 PartHwInfo;                  // NOR memory info.
  const FS_NOR_QUAD_SPI_CTRLR_INFO *CtrlrHwInfoPtr;             // Pointer to Quad SPI controller info.
};

/********************************************************************************************************
 *                                           NARROW TYPE BITFIELDS DEFINES
 *******************************************************************************************************/

#if (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU && !defined(__STRICT_ANSI__))
typedef CPU_INT08U BITFIELD08U;
typedef CPU_INT16U BITFIELD16U;
typedef CPU_INT32U BITFIELD32U;
typedef CPU_INT64U BITFIELD64U;
#else
typedef unsigned int BITFIELD08U;
typedef unsigned int BITFIELD16U;
typedef unsigned int BITFIELD32U;
typedef unsigned int BITFIELD64U;
#endif

/********************************************************************************************************
 *                                           QUAD SPI COMMANDS
 *******************************************************************************************************/

typedef struct fs_nor_quad_spi_cmd_form_flags {                 // ------------ QUAD SPI CMD FORMAT FLAGS -------------
  BITFIELD08U HasOpcode     : 1;                                // Cmd reqs transmission of an opcode.
  BITFIELD08U HasAddr       : 1;                                // Cmd reqs transmission of an address.
  BITFIELD08U OpcodeMultiIO : 1;                                // Opcode xfer in dual/quad mode.
  BITFIELD08U AddrMultiIO   : 1;                                // Addr/inter data xfer in dual/quad mode.
  BITFIELD08U DataMultiIO   : 1;                                // Data xfer in dual/quad mode.
  BITFIELD08U AddrLen4Bytes : 1;                                // DEF_YES: addr is 4 bytes, DEF_NO: addr is 3 bytes.
  BITFIELD08U IsWr          : 1;                                // DEF_YES: data xfer direction from host to slave.
  BITFIELD08U MultiIO_Quad  : 1;                                // Multi-IO is (DEF_YES) quad or (DEF_NO) dual mode.
} FS_NOR_QUAD_SPI_CMD_FORM_FLAGS;

typedef struct fs_nor_quad_spi_cmd_desc {                       // ------------- QUAD SPI CMD DESCRIPTOR --------------
  CPU_INT08U                     Opcode;                        // Value of cmd's opcode.
  FS_NOR_QUAD_SPI_CMD_FORM_FLAGS Form;                          // Flags to define form of cmd.
} FS_NOR_QUAD_SPI_CMD_DESC;

/********************************************************************************************************
 *                                           QUAD SPI DRIVER API
 *******************************************************************************************************/

struct fs_nor_quad_spi_drv_api {
  void *(*Add)(const FS_NOR_QUAD_SPI_CTRLR_INFO *p_hw_info,
               const FS_NOR_QUAD_SPI_SLAVE_INFO *p_slave_info,
               MEM_SEG                          *p_seg,
               RTOS_ERR                         *p_err);

  void (*Start)(void     *p_drv_data,
                RTOS_ERR *p_err);

  void (*Stop)(void     *p_drv_data,
               RTOS_ERR *p_err);

  void (*ClkSet)(void       *p_drv_data,
                 CPU_INT32U clk,
                 RTOS_ERR   *p_err);

  void (*DTR_Set)(void        *p_drv_data,
                  CPU_BOOLEAN en,
                  RTOS_ERR    *p_err);

  void (*FlashSizeSet)(void       *p_drv_data,
                       CPU_INT08U flash_size_log2,
                       CPU_INT08U chip_sel,
                       RTOS_ERR   *p_err);

  void (*CmdSend)(void                           *p_drv_data,
                  const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                  CPU_INT08U                     addr_tbl[],
                  CPU_INT08U                     inter_data[],
                  CPU_INT08U                     inter_cycles,
                  void                           *p_xfer_data,
                  CPU_INT32U                     xfer_size,
                  RTOS_ERR                       *p_err);

  void (*WaitWhileBusy)(void                           *p_drv_data,
                        const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                        CPU_INT32U                     typical_dur,
                        CPU_INT32U                     max_dur,
                        CPU_INT32U                     status_reg_mask,
                        RTOS_ERR                       *p_err);

  CPU_SIZE_T (*AlignReqGet)(void     *p_drv_data,
                            RTOS_ERR *p_err);

  void       (*XipBitSet)(void       *p_drv_data,
                          CPU_INT08U dummy_byte,
                          RTOS_ERR   *p_err);

  void (*XipCfg)(void        *p_drv_data,
                 CPU_BOOLEAN xip_en,
                 CPU_BOOLEAN flash_xip_support,
                 RTOS_ERR    *p_err);
};

/********************************************************************************************************
 *                           NOR QUAD SPI CONTROLLER DRIVER ISR HANDLER FUNCTION
 *******************************************************************************************************/

typedef void (*FS_NOR_QUAD_SPI_ISR_HANDLE_FNCT)(void *p_drv_data);

/********************************************************************************************************
 *                                           QUAD SPI BSP API
 *******************************************************************************************************/

struct fs_nor_quad_spi_bsp_api {
  CPU_BOOLEAN (*Init)      (FS_NOR_QUAD_SPI_ISR_HANDLE_FNCT isr_fnct,
                            void                            *p_drv_data);

  CPU_BOOLEAN (*ClkCfg)    (void);

  CPU_BOOLEAN (*IO_Cfg)    (void);

  CPU_BOOLEAN (*IntCfg)    (void);

  CPU_BOOLEAN (*ChipSelEn) (CPU_INT16U part_slave_id);

  CPU_BOOLEAN (*ChipSelDis)(CPU_INT16U part_slave_id);

  CPU_INT32U  (*ClkFreqGet)(void);
};

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                         MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
