/***************************************************************************//**
 * @file
 * @brief File System - Efm32G11 Quad Spi Controller Driver
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
 * @note    (1) This driver does support:
 *             - (a) Memory mapped 'direct' mode of operation for performing flash data transfers and
 *                   executing code from flash memory.
 *             - (b) 'Indirect' mode of operation for performing low latency and non processor intensive
 *                   flash data transfers
 *             - (c) Software triggered instruction generator to perform flash commands accessing
 *                   flash device registers and erasing entire or parts of the flash device
 *             - (d) Polling-based data communication
 *
 * @note     (2) This driver does NOT support:
 *             - (a) Dual I/O data communication, that is commands 2READ and DREAD
 *             - (b) Interrupt-based data communication
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error NOR module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <fs/include/fs_nor_quad_spi.h>

#include  <em_qspi.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 *
 * Note(s) : (1) The QSPI controller defines the SRAMPARTITIONCFG register. This register allows to define
 *               the internal QSPI SRAM memory in two partitions: one for indirect read and the other for
 *               indirect write. The register has only one field defining the size of the indirect
 *               read partition in the SRAM, in units of SRAM locations (maximum 256 locations of 32-bit
 *               words).
 *
 *               +-----------------+
 *               |                 |
 *               | Indirect writes | UPPER PART
 *               |                 |     |
 *               +-----------------+     +--> register SRAMPARTITIONCFG
 *               |                 |     |
 *               | Indirect reads  | LOWER PART
 *               |                 |
 *               +-----------------+
 *
 *               The write partition size is basically:
 *
 *               Write partition size = SRAM total size - read partition size
 *
 *               For instance, if 'read partition size' = 32 locations then write partition size is :
 *               256 - 32 = 223 locations * 32-bit = 892 bytes
 *
 *               (a) The 1-KB SRAM FIFO of the QSPI controller includes one extra location for a 32-bit
 *                   register. This location should not be accounted for the total available size for
 *                   reads and writes. Also another extra location should be marked as reserved. If the
 *                   SRAM read or write partition size is equal to 256 locations and the transfer has the
 *                   same size, the QSPI controller does not update correctly the SRAMFILL register.
 *
 *           (2) QSPI is mapped to two different memory regions, one in code space and one in system
 *               space. Each region can be used both to write to and read from external flash in direct
 *               and indirect modes. Code space is recommended for code execution and system space for
 *               data storage. FS_QSPI_SRAM_FIFO_START_ADDR_SEL can be set to one of these two values:
 *
 *               QSPI0_MEM_BASE
 *               QSPI0_CODE_MEM_BASE
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                            (FS, DRV, NOR)
#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_FS

#define  FS_QSPI_STIG_DATA_LEN_MAX                     8u       // STIG max data payload per instruction (i.e. command).
#define  FS_QSPI_DUMMY_CYCLES_PER_BYTE                 8u

#define  FS_QSPI_REG_RETRY1_MAX                        1000u
#define  FS_QSPI_REG_RETRY2_MAX                        100000u
#define  FS_QSPI_IDLE_CNT_MAX                          3u

//                                                                 Transfer mode.
#define  FS_QSPI_XFER_MODE_SINGLE_IO                   0u
#define  FS_QSPI_XFER_MODE_DUAL_IO                     1u
#define  FS_QSPI_XFER_MODE_QUAD_IO                     2u
#define  FS_QSPI_XFER_MODE_OCTAL_IO                    3u

//                                                                 QSPI SRAM cfg (see Note #1).
#define  FS_QSPI_SRAM_LOC_SIZE_BYTE                    4u
#define  FS_QSPI_SRAM_RESERVED_LOC_QTY                 2u       // See Note #1a.
#define  FS_QSPI_SRAM_MAX_LOC_QTY                      257u     // 257 entries of 32-bit (1028 total bytes).
#define  FS_QSPI_SRAM_SIZE_BYTE                        (FS_QSPI_SRAM_MAX_LOC_QTY * FS_QSPI_SRAM_LOC_SIZE_BYTE)

#define  FS_QSPI_SRAM_PARTITION_RD_SIZE_LOC_QTY        (FS_QSPI_SRAM_MAX_LOC_QTY - FS_QSPI_SRAM_RESERVED_LOC_QTY)
#define  FS_QSPI_SRAM_PARTITION_WR_SIZE_RD_LOC_QTY     1u
#define  FS_QSPI_SRAM_PARTITION_WR_SIZE_LOC_QTY_REAL   (FS_QSPI_SRAM_MAX_LOC_QTY - FS_QSPI_SRAM_PARTITION_WR_SIZE_RD_LOC_QTY)

//                                                                 QSPI memory space (see Note #2).
#define  FS_QSPI_SRAM_FIFO_START_ADDR_SEL              QSPI0_MEM_BASE

#define  FS_QSPI_EN_DELAY_MIN_mS                       10u
#define  FS_QSPI_4B_ALIGN_ADDR_MASK                    (CPU_WORD_SIZE_32 - 1u)

/********************************************************************************************************
 *                                           REGISTER BIT DEFINES
 *******************************************************************************************************/

//                                                                 ----------- QSPI CONFIGURATION REGISTER ------------
#define  FS_QSPI_REG_CONFIG_EN_MSK                             DEF_BIT_00
#define  FS_QSPI_REG_CONFIG_SEL_CLK_POL_MSK                    DEF_BIT_01
#define  FS_QSPI_REG_CONFIG_SEL_CLK_PHASE_MSK                  DEF_BIT_02
#define  FS_QSPI_REG_CONFIG_PHY_MODE_EN_MSK                    DEF_BIT_03
#define  FS_QSPI_REG_CONFIG_DAC_EN_MSK                         DEF_BIT_07
#define  FS_QSPI_REG_CONFIG_PERSELDEC_MSK                      DEF_BIT_09
#define  FS_QSPI_REG_CONFIG_PERCSLINES_MSK                     DEF_BIT_FIELD(2u, 10u)
#define  FS_QSPI_REG_CONFIG_AHB_REMAP_EN_MSK                   DEF_BIT_16
#define  FS_QSPI_REG_CONFIG_ENTER_XIP_IMM_MSK                  DEF_BIT_18
#define  FS_QSPI_REG_CONFIG_BAUDDIV_MSK                        DEF_BIT_FIELD(4u, 19u)
#define  FS_QSPI_REG_CONFIG_IDLE_MSK                           DEF_BIT_31
#define  FS_QSPI_REG_CONFIG_BAUDDIV_MIN                        2u
#define  FS_QSPI_REG_CONFIG_BAUDDIV_MAX                        32u
#define  FS_QSPI_REG_CONFIG_BAUDDIV_SDR_MIN                    4u
#define  FS_QSPI_REG_CONFIG_BAUDDIV_DDR_MIN                    8u
#define  FS_QSPI_REG_CONFIG_SEL_CLK_POL_OFFSET                 1u
#define  FS_QSPI_REG_CONFIG_SEL_CLK_PHASE_OFFSET               2u
#define  FS_QSPI_REG_CONFIG_PERCSLINES_OFFSET                  10u

//                                                                 -- DEVICE READ INSTRUCTION CONFIGURATION REGISTER --
#define  FS_QSPI_REG_DEVINSTRRDCONFIG_OPCODE_MSK               DEF_BIT_FIELD(8u, 0u)
#define  FS_QSPI_REG_DEVINSTRRDCONFIG_INST_WIDTH_MSK           DEF_BIT_FIELD(2u, 8u)
#define  FS_QSPI_REG_DEVINSTRRDCONFIG_ADDR_WIDTH_MSK           DEF_BIT_FIELD(2u, 12u)
#define  FS_QSPI_REG_DEVINSTRRDCONFIG_DATA_WIDTH_MSK           DEF_BIT_FIELD(2u, 16u)
#define  FS_QSPI_REG_DEVINSTRRDCONFIG_MODE_BITS_EN_MSK         DEF_BIT_20
#define  FS_QSPI_REG_DEVINSTRRDCONFIG_DUMMY_CLK_CYCLES_MSK     DEF_BIT_FIELD(5u, 24u)

#define  FS_QSPI_REG_DEVINSTRRDCONFIG_DUMMY_CLK_CYCLES_MAX     31u

//                                                                 - DEVICE WRITE INSTRUCTION CONFIGURATION REGISTER --
#define  FS_QSPI_REG_DEVINSTRWRCONFIG_OPCODE_MSK               DEF_BIT_FIELD(8u, 0u)
#define  FS_QSPI_REG_DEVINSTRWRCONFIG_ADDR_WIDTH_MSK           DEF_BIT_FIELD(2u, 12u)
#define  FS_QSPI_REG_DEVINSTRWRCONFIG_DATA_WIDTH_MSK           DEF_BIT_FIELD(2u, 16u)
#define  FS_QSPI_REG_DEVINSTRWRCONFIG_DUMMY_CLK_CYCLES_MSK     DEF_BIT_FIELD(5u, 24u)

#define  FS_QSPI_REG_DEVINSTRWRCONFIG_DUMMY_CLK_CYCLES_MAX     31u

//                                                                 -------- DEVICE SIZE CONFIGURATION REGISTER --------
#define  FS_QSPI_REG_DEVSIZECONFIG_NBR_ADDR_MASK               DEF_BIT_FIELD(4u, 0u)
#define  FS_QSPI_REG_DEVSIZECONFIG_PAGE_SIZE_MASK              DEF_BIT_FIELD(12u, 4u)
#define  FS_QSPI_REG_DEVSIZECONFIG_SUBSECTOR_SIZE_MASK         DEF_BIT_FIELD(4u, 16u)
#define  FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS0_MASK           DEF_BIT_FIELD(2u, 21u)
#define  FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS1_MASK           DEF_BIT_FIELD(2u, 23u)

#define  FS_QSPI_REG_DEVSIZECONFIG_NBR_ADDR_OFFSET             0u
#define  FS_QSPI_REG_DEVSIZECONFIG_PAGE_SIZE_OFFSET            4u
#define  FS_QSPI_REG_DEVSIZECONFIG_SUBSECTOR_SIZE_OFFSET       16u

#define  FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS_512Mb           0u
#define  FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS_1Gb             1u
#define  FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS_2Gb             2u
#define  FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS_4Gb             3u

//                                                                 ------------ READ DATA CAPTURE REGISTER ------------
#define  FS_QSPI_REG_RDDATACAPTURE_DLY_MASK                    DEF_BIT_FIELD(4u, 1u)
#define  FS_QSPI_REG_RDDATACAPTURE_DLY_MAX                     16u

//                                                                 ---------------- SRAM FILL REGISTER ----------------
#define  FS_QSPI_REG_SRAMFILL_INDRDPART_MASK                   DEF_BIT_FIELD(16u, 0u)
#define  FS_QSPI_REG_SRAMFILL_INDWRPART_MASK                   DEF_BIT_FIELD(16u, 16u)
#define  FS_QSPI_REG_SRAMFILL_INDWRPART_OFFSET                 16u

//                                                                 ----- INDIRECT READ TRANSFER CONTROL REGISTER ------
#define  FS_QSPI_REG_INDIRECTREADXFERCTRL_START_MASK           DEF_BIT_00
#define  FS_QSPI_REG_INDIRECTREADXFERCTRL_CANCEL_MASK          DEF_BIT_01
#define  FS_QSPI_REG_INDIRECTREADXFERCTRL_SRAM_FULL_MASK       DEF_BIT_03
#define  FS_QSPI_REG_INDIRECTREADXFERCTRL_OP_DONE_MASK         DEF_BIT_05

//                                                                 ----- INDIRECT WRITE TRANSFER CONTROL REGISTER -----
#define  FS_QSPI_REG_INDIRECTWRITEXFERCTRL_START_MASK          DEF_BIT_00
#define  FS_QSPI_REG_INDIRECTWRITEXFERCTRL_CANCEL_MASK         DEF_BIT_01
#define  FS_QSPI_REG_INDIRECTWRITEXFERCTRL_OP_DONE_MASK        DEF_BIT_05

//                                                                 ---------- FLASH COMMAND CONTROL REGISTER ----------
#define  FS_QSPI_REG_FLASHCMDCTRL_EXEC_CMD_MASK                DEF_BIT_00
#define  FS_QSPI_REG_FLASHCMDCTRL_EXEC_CMD_STATUS_MASK         DEF_BIT_01
#define  FS_QSPI_REG_FLASHCMDCTRL_DUMMY_BYTES_MSK              DEF_BIT_FIELD(5u, 7u)
#define  FS_QSPI_REG_FLASHCMDCTRL_WR_DATA_BYTES_MSK            DEF_BIT_FIELD(3u, 12u)
#define  FS_QSPI_REG_FLASHCMDCTRL_WR_DATA_EN_MSK               DEF_BIT_15
#define  FS_QSPI_REG_FLASHCMDCTRL_CMD_ADDR_EN_MSK              DEF_BIT_19
#define  FS_QSPI_REG_FLASHCMDCTRL_RD_DATA_BYTES_MSK            DEF_BIT_FIELD(3u, 20u)
#define  FS_QSPI_REG_FLASHCMDCTRL_RD_DATA_EN_MSK               DEF_BIT_23

#define  FS_QSPI_REG_FLASHCMDCTRL_DUMMY_BYTES_OFFSET           7u
#define  FS_QSPI_REG_FLASHCMDCTRL_WR_DATA_BYTES_OFFSET         12u
#define  FS_QSPI_REG_FLASHCMDCTRL_NBR_ADDR_BYTES_OFFSET        16u
#define  FS_QSPI_REG_FLASHCMDCTRL_RD_DATA_BYTES_OFFSET         20u
#define  FS_QSPI_REG_FLASHCMDCTRL_OPCODE_OFFSET                24u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               REGISTERS
 *
 * Note(s) : (1) Some flash devices does not support XIP (eXecute In Place). However it is still possible
 *               to perform XIP in software. Two types of XIP can be defined:
 *
 *               (a) XIP hardware: Quad SPI XIP mode and direct (memory-mapped) mode activated. NOR flash
 *                   XIP activated.
 *
 *               (b) XIP software: Quad SPI direct mode activated only.
 *******************************************************************************************************/

//                                                                 ---------------- QSPI REGISTERS MAP ----------------
typedef struct qspi_reg {
  CPU_REG32  CONFIG;                                            // QSPI Configuration Register.
  CPU_REG32  DEVINSTRRDCONFIG;                                  // Device Read Instruction Configuration Register.
  CPU_REG32  DEVINSTRWRCONFIG;                                  // Device Write Instruction Configuration Register.
  CPU_REG32  DEVDELAY;                                          // QSPI Device Delay Register.
  CPU_REG32  RDDATACAPTURE;                                     // Read Data Capture Register.
  CPU_REG32  DEVSIZECONFIG;                                     // Device Size Configuration Register.
  CPU_REG32  SRAMPARTITIONCFG;                                  // SRAM Partition Configuration Register.
  CPU_REG32  INDAHBADDRTRIGGER;                                 // Indirect Address Trigger Register.
  CPU_INT32U Reserved0[1u];
  CPU_REG32  REMAPADDR;                                         // Remap Address Register.
  CPU_REG32  MODEBITCONFIG;                                     // Mode Bit Configuration Register.
  CPU_REG32  SRAMFILL;                                          // SRAM Fill Register.
  CPU_REG32  TXTHRESH;                                          // TX Threshold Register.
  CPU_REG32  RXTHRESH;                                          // RX Threshold Register.
  CPU_REG32  WRITECOMPLETIONCTRL;                               // Write Completion Control Register.
  CPU_REG32  NOOFPOLLSBEFEXP;                                   // Polling Expiration Register.
  CPU_REG32  IRQSTATUS;                                         // Interrupt Status Register.
  CPU_REG32  IRQMASK;                                           // Interrupt Mask.
  CPU_INT32U Reserved1[2u];
  CPU_REG32  LOWERWRPROT;                                       // Lower Write Protection Register.
  CPU_REG32  UPPERWRPROT;                                       // Upper Write Protection Register.
  CPU_REG32  WRPROTCTRL;                                        // Write Protection Control Register.
  CPU_INT32U Reserved2[1u];
  CPU_REG32  INDIRECTREADXFERCTRL;                              // Indirect Read Transfer Control Register.
  CPU_REG32  INDIRECTREADXFERWATERMARK;                         // Indirect Read Transfer Watermark Register.
  CPU_REG32  INDIRECTREADXFERSTART;                             // Indirect Read Transfer Start Address Register.
  CPU_REG32  INDIRECTREADXFERNUMBYTES;                          // Indirect Read Transfer Number Bytes Register.
  CPU_REG32  INDIRECTWRITEXFERCTRL;                             // Indirect Write Transfer Control Register.
  CPU_REG32  INDIRECTWRITEXFERWATERMARK;                        // Indirect Write Transfer Watermark Register.
  CPU_REG32  INDIRECTWRITEXFERSTART;                            // Indirect Write Transfer Start Address Register.
  CPU_REG32  INDIRECTWRITEXFERNUMBYTES;                         // Indirect Write Transfer Number Bytes Register.
  CPU_REG32  INDIRECTTRIGGERADDRRANGE;                          // Indirect Trigger Address Range Register.
  CPU_INT32U Reserved3[2u];
  CPU_REG32  FLASHCOMMANDCTRLMEM;                               // Flash Command Control Memory Register.
  CPU_REG32  FLASHCMDCTRL;                                      // Flash Command Control Register.
  CPU_REG32  FLASHCMDADDR;                                      // Flash Command Address Register.
  CPU_INT32U Reserved4[2u];
  CPU_REG32  FLASHRDDATALOWER;                                  // Flash Command Read Data Register (Lower).
  CPU_REG32  FLASHRDDATAUPPER;                                  // Flash Command Read Data Register (Upper).
  CPU_REG32  FLASHWRDATALOWER;                                  // Flash Command Write Data Register (Lower).
  CPU_REG32  FLASHWRDATAUPPER;                                  // Flash Command Write Data Register (Upper).
  CPU_REG32  POLLINGFLASHSTATUS;                                // Polling Flash Status Register.
  CPU_REG32  PHYCONFIGURATION;                                  // PHY Configuration Register.
  CPU_REG32  PHYMASTERCONTROL;                                  // PHY DLL Master Control Register.
  CPU_REG32  DLLOBSERVABLELOWER;                                // DLL Observable Register Lower.
  CPU_REG32  DLLOBSERVABLEUPPER;                                // DLL Observable Register Upper.
  CPU_INT32U Reserved5[7u];
  CPU_REG32  OPCODEEXTLOWER;                                    // Opcode Extension Register (Lower).
  CPU_REG32  OPCODEEXTUPPER;                                    // Opcode Extension Register (Upper).
  CPU_INT32U Reserved6[5u];
  CPU_REG32  MODULEID;                                          // Module ID Register.

  CPU_REG32  CTRL;                                              // Control Register.
  CPU_REG32  ROUTEPEN;                                          // I/O Routing Pin Enable Register.
  CPU_REG32  ROUTELOC0;                                         // I/O Route Location Register 0.
} QSPI_REG;

typedef enum qspi_flash_xfer_type {
  QSPI_XFER_TYPE_NONE = 0u,
  QSPI_XFER_TYPE_STIG_RD = 1u,
  QSPI_XFER_TYPE_STIG_WR = 2u,
  QSPI_XFER_TYPE_INDIRECT_RD = 3u,
  QSPI_XFER_TYPE_INDIRECT_WR = 4u,
  QSPI_XFER_TYPE_DIRECT_RD = 5u,
  QSPI_XFER_TYPE_DIRECT_WR = 6u
} QSPI_FLASH_XFER_TYPE;

//                                                                 ------------------- DRIVER DATA --------------------
typedef struct qspi_data {
  FS_NOR_QUAD_SPI_CTRLR_INFO *QuadSPI_CtrlrInfoPtr;             // Pointer to QSPI controller info.
  CPU_BOOLEAN                DTR_En;                            // Flag indicating if Double Transfer Rate enabled.
  CPU_BOOLEAN                PHY_En;                            // Flag indicating if PHY module enabled.
  CPU_BOOLEAN                XIP_SwEn;                          // Flag indicating if XIP SW enabled (See Note #1).
  CPU_BOOLEAN                XIP_HwEn;                          // Flag indicating if XIP HW enabled (See Note #1).
  CPU_INT08U                 ChipSelID;                         // Chip select ID to which flash device is connected.
} QSPI_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   DRIVER INTERFACE FUNCTION PROTOTYPES
 *******************************************************************************************************/

static void *FS_NOR_QuadSPI_Add(const FS_NOR_QUAD_SPI_CTRLR_INFO *p_ctrlr_info,
                                const FS_NOR_QUAD_SPI_SLAVE_INFO *p_slave_info,
                                MEM_SEG                          *p_seg,
                                RTOS_ERR                         *p_err);

static void FS_NOR_QuadSPI_Start(void     *p_drv_data,
                                 RTOS_ERR *p_err);

static void FS_NOR_QuadSPI_Stop(void     *p_drv_data,
                                RTOS_ERR *p_err);

static void FS_NOR_QuadSPI_ClkSet(void       *p_drv_data,
                                  CPU_INT32U clk,
                                  RTOS_ERR   *p_err);

static void FS_NOR_QuadSPI_DTR_Set(void        *p_drv_data,
                                   CPU_BOOLEAN en,
                                   RTOS_ERR    *p_err);

static void FS_NOR_QuadSPI_FlashSizeSet(void       *p_drv_data,
                                        CPU_INT08U flash_size_log2,
                                        CPU_INT08U chip_sel,
                                        RTOS_ERR   *p_err);

static void FS_NOR_QuadSPI_CmdSend(void                           *p_drv_data,
                                   const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                   CPU_INT08U                     addr_tbl[],
                                   CPU_INT08U                     inter_data[],
                                   CPU_INT08U                     inter_cycles,
                                   void                           *p_xfer_data,
                                   CPU_INT32U                     xfer_size,
                                   RTOS_ERR                       *p_err);

static CPU_SIZE_T FS_NOR_QuadSPI_AlignReqGet(void     *p_drv_data,
                                             RTOS_ERR *p_err);

static void FS_NOR_QuadSPI_XipCfg(void        *p_drv_data,
                                  CPU_BOOLEAN xip_en,
                                  CPU_BOOLEAN flash_xip_support,
                                  RTOS_ERR    *p_err);

/********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

static void FS_NOR_QuadSPI_ChipSelect(const QSPI_DATA *p_drv_data,
                                      CPU_INT08U      chip_sel,
                                      RTOS_ERR        *p_err);

static void FS_NOR_QuadSPI_ClkOutSet(const QSPI_DATA *p_drv_data,
                                     CPU_INT32U      ref_clk_hz,
                                     CPU_INT32U      sclk_out_hz,
                                     RTOS_ERR        *p_err);

static void FS_NOR_QuadSPI_StigRd(const QSPI_DATA                *p_drv_data,
                                  const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                  CPU_INT08U                     inter_cycles,
                                  void                           *p_xfer_data,
                                  CPU_INT32U                     xfer_size,
                                  RTOS_ERR                       *p_err);

static void FS_NOR_QuadSPI_StigWr(const QSPI_DATA                *p_drv_data,
                                  const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                  CPU_INT08U                     addr_tbl[],
                                  CPU_INT08U                     inter_cycles,
                                  void                           *p_xfer_data,
                                  CPU_INT32U                     xfer_size,
                                  RTOS_ERR                       *p_err);

static void FS_NOR_QuadSPI_StigCmdFlashExec(const QSPI_DATA *p_drv_data,
                                            CPU_INT32U      reg_val,
                                            RTOS_ERR        *p_err);

static void FS_NOR_QuadSPI_IdleCheck(const QSPI_DATA *p_drv_data,
                                     RTOS_ERR        *p_err);

static void FS_NOR_QuadSPI_IndirectRd(const QSPI_DATA                *p_drv_data,
                                      const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                      CPU_INT08U                     addr_tbl[],
                                      CPU_INT08U                     inter_cycles,
                                      void                           *p_xfer_data,
                                      CPU_INT32U                     xfer_size,
                                      RTOS_ERR                       *p_err);

static void FS_NOR_QuadSPI_IndirectRd_SRAM(const QSPI_DATA *p_drv_data,
                                           void            *p_dest_buf,
                                           const void      *p_src_sram,
                                           CPU_INT32U      xfer_size,
                                           RTOS_ERR        *p_err);

static void FS_NOR_QuadSPI_DirectRd(const QSPI_DATA                *p_drv_data,
                                    const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                    CPU_INT08U                     addr_tbl[],
                                    void                           *p_xfer_data,
                                    CPU_INT32U                     xfer_size,
                                    RTOS_ERR                       *p_err);

static void FS_NOR_QuadSPI_IndirectWr(const QSPI_DATA                *p_drv_data,
                                      const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                      CPU_INT08U                     addr_tbl[],
                                      CPU_INT08U                     inter_cycles,
                                      void                           *p_xfer_data,
                                      CPU_INT32U                     xfer_size,
                                      RTOS_ERR                       *p_err);

static void FS_NOR_QuadSPI_IndirectWr_SRAM(const QSPI_DATA *p_drv_data,
                                           const void      *p_dest_sram,
                                           void            *p_src_buf,
                                           CPU_INT32U      xfer_size,
                                           RTOS_ERR        *p_err);

static void FS_NOR_QuadSPI_DirectWr(const QSPI_DATA                *p_drv_data,
                                    const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                    CPU_INT08U                     addr_tbl[],
                                    void                           *p_xfer_data,
                                    CPU_INT32U                     xfer_size,
                                    RTOS_ERR                       *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_NOR_QUAD_SPI_DRV_API FS_NOR_QuadSpiDrvAPI_Silabs_EFM32GG11 = {
  .Add = FS_NOR_QuadSPI_Add,
  .Start = FS_NOR_QuadSPI_Start,
  .Stop = FS_NOR_QuadSPI_Stop,
  .ClkSet = FS_NOR_QuadSPI_ClkSet,
  .DTR_Set = FS_NOR_QuadSPI_DTR_Set,
  .FlashSizeSet = FS_NOR_QuadSPI_FlashSizeSet,
  .CmdSend = FS_NOR_QuadSPI_CmdSend,
  .WaitWhileBusy = DEF_NULL,
  .AlignReqGet = FS_NOR_QuadSPI_AlignReqGet,
  .XipBitSet = DEF_NULL,
  .XipCfg = FS_NOR_QuadSPI_XipCfg
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_Add()
 *
 * @brief    Add a Quad SPI controller driver instance.
 *
 * @param    p_ctrlr_info    Pointer to a controller information structure.
 *
 * @param    p_slave_info    Pointer to a slave information structure.
 *
 * @param    p_seg           Pointer to a memory segment where to allocate the controller driver instance.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to driver-specific data, if NO error(s).
 *           Null pointer,                    otherwise.
 *******************************************************************************************************/
static void *FS_NOR_QuadSPI_Add(const FS_NOR_QUAD_SPI_CTRLR_INFO *p_ctrlr_info,
                                const FS_NOR_QUAD_SPI_SLAVE_INFO *p_slave_info,
                                MEM_SEG                          *p_seg,
                                RTOS_ERR                         *p_err)
{
  QSPI_DATA   *p_quad_spi_drv_data = DEF_NULL;
  QSPI_REG    *p_reg = (QSPI_REG *)p_ctrlr_info->BaseAddr;
  CPU_INT32U  reg_val;
  CPU_INT32U  clk_pol;
  CPU_INT32U  clk_phase;
  CPU_BOOLEAN ok = DEF_FAIL;

  //                                                               Alloc drv private data.
  p_quad_spi_drv_data = (QSPI_DATA *)Mem_SegAlloc(DEF_NULL,
                                                  p_seg,
                                                  sizeof(QSPI_DATA),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (DEF_NULL);
  }
  Mem_Clr(p_quad_spi_drv_data, sizeof(QSPI_DATA));

  //                                                               Save QSPI controller info for later usage by drv.
  p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr = (FS_NOR_QUAD_SPI_CTRLR_INFO *)p_ctrlr_info;

  //                                                               --------------------- BSP INIT ---------------------
  if (p_ctrlr_info->BspApiPtr->Init != DEF_NULL) {
    ok = p_ctrlr_info->BspApiPtr->Init(DEF_NULL,                // DEF_NULL = no intr used by this QSPI drv.
                                       p_quad_spi_drv_data);
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return (DEF_NULL);
    }
  }
  //                                                               Cfg clock(s) required by QSPI ctrlr.
  if (p_ctrlr_info->BspApiPtr->ClkCfg != DEF_NULL) {
    ok = p_ctrlr_info->BspApiPtr->ClkCfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return (DEF_NULL);
    }
  }
  //                                                               Cfg I/O needed by QSPI ctrlr if necessary.
  if (p_ctrlr_info->BspApiPtr->IO_Cfg != DEF_NULL) {
    ok = p_ctrlr_info->BspApiPtr->IO_Cfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return (DEF_NULL);
    }
  }
  //                                                               Cfg interrupt controller if necessary.
  if (p_ctrlr_info->BspApiPtr->IntCfg != DEF_NULL) {
    ok = p_ctrlr_info->BspApiPtr->IntCfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return (DEF_NULL);
    }
  }

  //                                                               -------------------- QSPI INIT ---------------------
  FS_NOR_QuadSPI_IdleCheck(p_quad_spi_drv_data, p_err);         // Poll QSPI idle status to ensure QSPI has finished.

  DEF_BIT_CLR(p_reg->CONFIG, FS_QSPI_REG_CONFIG_EN_MSK);        // Dis controller to ensure proper init.

  //                                                               Instruction transferred on DQ0 (1 bit).
  reg_val = p_reg->DEVINSTRRDCONFIG;
  DEF_BIT_FIELD_WR(reg_val, FS_QSPI_XFER_MODE_SINGLE_IO, FS_QSPI_REG_DEVINSTRRDCONFIG_INST_WIDTH_MSK);
  p_reg->DEVINSTRRDCONFIG = reg_val;

  //                                                               Cfg reg CONFIG.
  reg_val = p_reg->CONFIG;
  //                                                               Set polarity and phase.
  switch (p_slave_info->Mode) {
    case FS_NOR_SPI_MODE_0:
      clk_phase = 0u;
      clk_pol = 0u;
      break;

    case FS_NOR_SPI_MODE_3:
      clk_phase = 1u;
      clk_pol = 1u;
      break;

    case FS_NOR_SPI_MODE_1:
    case FS_NOR_SPI_MODE_2:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
      return (DEF_NULL);
  }

  DEF_BIT_FIELD_WR(reg_val, clk_phase, FS_QSPI_REG_CONFIG_SEL_CLK_PHASE_MSK);
  DEF_BIT_FIELD_WR(reg_val, clk_pol, FS_QSPI_REG_CONFIG_SEL_CLK_POL_MSK);

  DEF_BIT_CLR(reg_val, FS_QSPI_REG_CONFIG_DAC_EN_MSK);          // Dis Direct Access Controller.
                                                                // Dis PHY module.
  DEF_BIT_CLR(p_reg->CONFIG, FS_QSPI_REG_CONFIG_PHY_MODE_EN_MSK);
  p_quad_spi_drv_data->PHY_En = DEF_NO;

  p_reg->CONFIG = reg_val;

  //                                                               Cfg Dev Size Cfg reg with some flash info.
  reg_val = p_reg->DEVSIZECONFIG;
  DEF_BIT_FIELD_WR(reg_val, p_slave_info->PageSize, FS_QSPI_REG_DEVSIZECONFIG_PAGE_SIZE_MASK);
  DEF_BIT_FIELD_WR(reg_val, p_slave_info->BlkSizeLog2, FS_QSPI_REG_DEVSIZECONFIG_SUBSECTOR_SIZE_MASK);
  p_reg->DEVSIZECONFIG = reg_val;

  p_reg->REMAPADDR = 0;                                         // No remap. Remap Addr reg only used in direct mode.
  p_reg->IRQMASK = 0;                                           // Disable all interrupts.

  //                                                               Select the NOR flash device.
  FS_NOR_QuadSPI_ChipSelect(p_quad_spi_drv_data,
                            p_slave_info->ChipSelID,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }
  p_quad_spi_drv_data->ChipSelID = p_slave_info->ChipSelID;

  return ((void *)p_quad_spi_drv_data);
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_ClkSet()
 *
 * @brief    Set serial clock frequency outputted by the Quad SPI controller to the flash device.
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    clk         Serial clock frequency.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_ClkSet(void       *p_drv_data,
                                  CPU_INT32U clk,
                                  RTOS_ERR   *p_err)
{
  QSPI_DATA  *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;
  CPU_INT32U ref_clk_hz;

  //                                                               Get input clock of QSPI controller.
  ref_clk_hz = p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr->BspApiPtr->ClkFreqGet();

  //                                                               Cfg the serial output clk.
  FS_NOR_QuadSPI_ClkOutSet(p_quad_spi_drv_data,
                           ref_clk_hz,
                           clk,                                 // Desired clk freq to output to NOR flash dev.
                           p_err);
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_DTR_Set()
 *
 * @brief    Set double transfer rate (see Note #1).
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    en          Boolean indicating whether to enable or disable double transfer rate.
 *                       DEF_YES     Enable double transfer rate.
 *                       DEF_NO      Disable double transfer rate.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Double transfer rate is only supported by some READ commands. It does not apply to
 *               PROGRAM command, that is write path.
 *
 * @note     (2) In the QSPI controller, the DTR support is indicated in the field DDREN of register
 *               'Device Read Instruction Configuration'. The DTR will be setup in the function
 *               CmdSend() when sending a READ command that supports DTR.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_DTR_Set(void        *p_drv_data,
                                   CPU_BOOLEAN en,
                                   RTOS_ERR    *p_err)
{
  QSPI_DATA *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;

  PP_UNUSED_PARAM(p_err);
  p_quad_spi_drv_data->DTR_En = en;                             // Save DTR support of NOR flash chip (see Note #2).
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_FlashSizeSet()
 *
 * @brief    Set flash memory size.
 *
 * @param    p_drv_data          Pointer to driver-specific data.
 *
 * @param    flash_size_log2     Base-2 logarithm of the flash memory size.
 *
 * @param    chip_sel            Chip select ID associated to flash device.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_FlashSizeSet(void       *p_drv_data,
                                        CPU_INT08U flash_size_log2,
                                        CPU_INT08U chip_sel,
                                        RTOS_ERR   *p_err)
{
  QSPI_DATA  *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;
  QSPI_REG   *p_reg = (QSPI_REG *)p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT08U mem_size_val;
  CPU_INT32U cs_mask;

  PP_UNUSED_PARAM(p_err);

  if (chip_sel >= 2u) {                                         // Only 2 chip selects supported by QSPI controller.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
    return;
  }

  if (flash_size_log2 <= FS_NOR_PHY_SIZE_LOG2_512Mb) {          // Size below 512Mb configured to 512Mb val.
    mem_size_val = FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS_512Mb;
  } else if (flash_size_log2 >= FS_NOR_PHY_SIZE_LOG2_2Gb) {     // Size above 4Gb configured to 4Gb val.
    mem_size_val = FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS_4Gb;
  } else {
    mem_size_val = flash_size_log2 - 26u;                       // -26 allows to configure to 1/2Gb reg val.
  }

  if (chip_sel == 0u) {
    cs_mask = FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS0_MASK;
  } else {
    cs_mask = FS_QSPI_REG_DEVSIZECONFIG_MEM_SIZE_CS1_MASK;
  }
  DEF_BIT_FIELD_WR(p_reg->DEVSIZECONFIG, mem_size_val, cs_mask);
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_Start()
 *
 * @brief    Start controller operation.
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_Start(void     *p_drv_data,
                                 RTOS_ERR *p_err)
{
  QSPI_DATA *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;
  QSPI_REG  *p_reg = (QSPI_REG *)p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;

  PP_UNUSED_PARAM(p_err);
  DEF_BIT_SET(p_reg->CONFIG, FS_QSPI_REG_CONFIG_EN_MSK);        // En controller operations.

  KAL_Dly(FS_QSPI_EN_DELAY_MIN_mS);                             // Wait before issuing 1st cmd after QSPI ctrlr start.
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_Stop()
 *
 * @brief    Stop controller operation.
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_Stop(void     *p_drv_data,
                                RTOS_ERR *p_err)
{
  QSPI_DATA *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;
  QSPI_REG  *p_reg = (QSPI_REG *)p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;

  PP_UNUSED_PARAM(p_err);
  DEF_BIT_CLR(p_reg->CONFIG, FS_QSPI_REG_CONFIG_EN_MSK);        // Dis controller operations.
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_CmdSend()
 *
 * @brief    Send a command.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_cmd           Pointer to a command descriptor.
 *
 * @param    addr_tbl        Source / Destination address table.
 *
 * @param    inter_data      Inter data table.
 *
 * @param    inter_cycles    Inter cycle cnt.
 *
 * @param    p_xfer_data     Pointer to a buffer that contains data to be written or that receives
 *                           data to be read.
 *
 * @param    xfer_size       Number of octets to be read / written.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This QSPI Peripheral supports 'Dummy Cycle' qty rather than transferring 'Dummy Bytes'
 *               thus, the use of the 'Dummy Cycles' (inter_cycles) is used rather than having to
 *               calculate 'Dummy Bytes' (inter_byte_cnt) to send.
 *
 * @note     (2) The NOR command can have one of the following formats:
 *               - (a) OPCODE (single operation such as reset, write enable)
 *               - (b) OPCODE + DATA (flash register read/write access)
 *               - (c) OPCODE + ADDRESS (erase operation)
 *               - (d) OPCODE + ADDRESS + DATA (memory read/write access in indirect/direct mode)
 *
 * @note     (3) The Software Triggered Instruction Generator (STIG) is used to access the volatile
 *               and non-volatile configuration registers, the legacy SPI status register, and other
 *               status and protection registers on the NOR memory.
 *               - (a) It is recommended to use the STIG to perform erase operations.
 *
 * @note     (4) In indirect/direct mode, commands are ONLY for transferring data from/to flash
 *               memory.
 *
 * @note     (5) When XIP hardware activated, that is XIP mode enabled in QSPI and flash device,
 *               flash read/write transfers are not allowed via the Quad SPI driver. It is still
 *               possible to perform commands using the Software Triggered Instruction Generator
 *               (STIG) which accesses flash registers.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_CmdSend(void                           *p_drv_data,
                                   const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                   CPU_INT08U                     addr_tbl[],
                                   CPU_INT08U                     inter_data[],
                                   CPU_INT08U                     inter_cycles,
                                   void                           *p_xfer_data,
                                   CPU_INT32U                     xfer_size,
                                   RTOS_ERR                       *p_err)
{
  QSPI_DATA                  *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;
  FS_NOR_QUAD_SPI_CTRLR_INFO *p_ctrlr_info = p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr;
  QSPI_FLASH_XFER_TYPE       xfer_type = QSPI_XFER_TYPE_NONE;

  PP_UNUSED_PARAM(inter_data);

  //                                                               Determine transfer type (see Note #2).
  if (p_cmd->Form.HasOpcode) {                                  // If op code, STIG or indirect mode used.
    if ((!p_cmd->Form.HasAddr)                                  // Flash register operations do NOT use addr.
        || ((p_cmd->Form.HasAddr) && (!xfer_size))) {           // Erase cmd does NOT have data (see Note #3a).
                                                                // Command executed by STIG function (see Note #3).
      if (p_cmd->Form.IsWr) {
        xfer_type = QSPI_XFER_TYPE_STIG_WR;
      } else if (xfer_size == 0u) {
        xfer_type = QSPI_XFER_TYPE_STIG_WR;                     // Commands with only opcode (considered as STIG write).
      } else {
        xfer_type = QSPI_XFER_TYPE_STIG_RD;
      }
    } else {                                                    // Cmd executed in indirect/direct mode (see Note #4).
      if (p_quad_spi_drv_data->XIP_HwEn) {                      // If XIP HW activated, no flash xfer allowed via drv...
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                      // ...See Note #5.
        return;
      }

      if (!p_quad_spi_drv_data->XIP_SwEn) {
        if (p_cmd->Form.IsWr) {
          xfer_type = QSPI_XFER_TYPE_INDIRECT_WR;
        } else {
          xfer_type = QSPI_XFER_TYPE_INDIRECT_RD;
        }
      } else {                                                  // If XIP SW activated, direct mode is used.
        if (p_cmd->Form.IsWr) {
          xfer_type = QSPI_XFER_TYPE_DIRECT_WR;
        } else {
          xfer_type = QSPI_XFER_TYPE_DIRECT_RD;
        }
      }
    }
  }

  if (p_ctrlr_info->BspApiPtr->ChipSelEn != DEF_NULL) {
    p_ctrlr_info->BspApiPtr->ChipSelEn(p_quad_spi_drv_data->ChipSelID);
  }

  switch (xfer_type) {
    case QSPI_XFER_TYPE_STIG_RD:
      FS_NOR_QuadSPI_StigRd(p_quad_spi_drv_data,
                            p_cmd,
                            inter_cycles,
                            p_xfer_data,
                            xfer_size,
                            p_err);
      break;

    case QSPI_XFER_TYPE_STIG_WR:
      FS_NOR_QuadSPI_StigWr(p_quad_spi_drv_data,
                            p_cmd,
                            addr_tbl,
                            inter_cycles,
                            p_xfer_data,
                            xfer_size,
                            p_err);
      break;

    case QSPI_XFER_TYPE_INDIRECT_RD:
      FS_NOR_QuadSPI_IndirectRd(p_quad_spi_drv_data,
                                p_cmd,
                                addr_tbl,
                                inter_cycles,
                                p_xfer_data,
                                xfer_size,
                                p_err);
      break;

    case QSPI_XFER_TYPE_INDIRECT_WR:
      FS_NOR_QuadSPI_IndirectWr(p_quad_spi_drv_data,
                                p_cmd,
                                addr_tbl,
                                inter_cycles,
                                p_xfer_data,
                                xfer_size,
                                p_err);
      break;

    case QSPI_XFER_TYPE_DIRECT_RD:
      FS_NOR_QuadSPI_DirectRd(p_quad_spi_drv_data,
                              p_cmd,
                              addr_tbl,
                              p_xfer_data,
                              xfer_size,
                              p_err);
      break;

    case QSPI_XFER_TYPE_DIRECT_WR:
      FS_NOR_QuadSPI_DirectWr(p_quad_spi_drv_data,
                              p_cmd,
                              addr_tbl,
                              p_xfer_data,
                              xfer_size,
                              p_err);
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      break;
  }

  if (p_ctrlr_info->BspApiPtr->ChipSelDis != DEF_NULL) {
    p_ctrlr_info->BspApiPtr->ChipSelDis(p_quad_spi_drv_data->ChipSelID);
  }
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_AlignReqGet()
 *
 * @brief    Get buffer alignment requirement of the controller.
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    p_err       Error pointer.
 *
 * @return   buffer alignment requirement in octets.
 *******************************************************************************************************/
static CPU_SIZE_T FS_NOR_QuadSPI_AlignReqGet(void     *p_drv_data,
                                             RTOS_ERR *p_err)
{
  QSPI_DATA *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;

  PP_UNUSED_PARAM(p_err);

  return (p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr->AlignReq);
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_XipCfg()
 *
 * @brief    Configure Quad SPI controller in XIP (eXecute-In-Place) mode.
 *
 * @param    p_drv_data          Pointer to driver-specific data.
 *
 * @param    xip_en              XIP mode enable/disable flag.
 *
 * @param    flash_xip_support   Flag indicating if flash device supports XIP (see Note #1).
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) XIP is supported by some flash devices. When a flash device does not support XIP,
 *               it is still possible to implement XIP from a software perspective. Two types of XIP
 *               can be defined:
 *               - (a) XIP hardware: a dedicated XIP mode can be activated in the NOR flash and in the
 *                     Quad SPI controller. This XIP is considered the full XIP.
 *               - (b) XIP software: the NOR flash does not support XIP and the Quad SPI controller XIP
 *                     cannot be activated in that case. Still it is possible to perform a XIP software
 *                     by enabling the Quad SPI controller in memory-mapped (i.e. direct) mode. It is
 *                     less effective than the full XIP because the command instruction phase cannot be
 *                     omitted.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_XipCfg(void        *p_drv_data,
                                  CPU_BOOLEAN xip_en,
                                  CPU_BOOLEAN flash_xip_support,
                                  RTOS_ERR    *p_err)
{
  QSPI_DATA  *p_quad_spi_drv_data = (QSPI_DATA *)p_drv_data;
  QSPI_REG   *p_reg = (QSPI_REG *)p_quad_spi_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U reg_val = 0u;

  PP_UNUSED_PARAM(p_err);

  DEF_BIT_CLR(p_reg->CONFIG, FS_QSPI_REG_CONFIG_EN_MSK);        // Dis controller to ensure proper XIP cfg.

  //                                                               -------------------- XIP EN CFG --------------------
  reg_val = p_reg->CONFIG;
  if (xip_en) {
    DEF_BIT_SET(reg_val, FS_QSPI_REG_CONFIG_DAC_EN_MSK);        // En direct access mode.

    if (flash_xip_support) {                                    // See Note #1.
      p_reg->MODEBITCONFIG = DEF_BIT_NONE;                      // Set value of XIP confirmation bit.
                                                                // En mode bit (i.e. XIP confirmation bit).
      DEF_BIT_SET(p_reg->DEVINSTRRDCONFIG, FS_QSPI_REG_DEVINSTRRDCONFIG_MODE_BITS_EN_MSK);
      //                                                           Enter XIP Mode immediately.
      DEF_BIT_SET(reg_val, FS_QSPI_REG_CONFIG_ENTER_XIP_IMM_MSK);

      p_quad_spi_drv_data->XIP_HwEn = DEF_YES;
    } else {
      p_quad_spi_drv_data->XIP_SwEn = DEF_YES;
    }

    //                                                             -------------------- XIP DIS CFG -------------------
  } else {
    DEF_BIT_CLR(reg_val, FS_QSPI_REG_CONFIG_DAC_EN_MSK);        // Dis direct access mode.

    if (flash_xip_support) {
      p_reg->MODEBITCONFIG = DEF_BIT_NONE;                      // Reset mode bit.
                                                                // Dis mode bit (i.e. XIP confirmation bit).
      DEF_BIT_CLR(p_reg->DEVINSTRRDCONFIG, FS_QSPI_REG_DEVINSTRRDCONFIG_MODE_BITS_EN_MSK);
      //                                                           Dis XIP Mode immediately.
      DEF_BIT_CLR(reg_val, FS_QSPI_REG_CONFIG_ENTER_XIP_IMM_MSK);
      //                                                           Dis AHB remapping in case it was used.
      DEF_BIT_CLR(reg_val, FS_QSPI_REG_CONFIG_AHB_REMAP_EN_MSK);

      p_quad_spi_drv_data->XIP_HwEn = DEF_NO;
    } else {
      p_quad_spi_drv_data->XIP_SwEn = DEF_NO;
    }
  }
  p_reg->CONFIG = reg_val;

  DEF_BIT_SET(p_reg->CONFIG, FS_QSPI_REG_CONFIG_EN_MSK);        // En controller operations.

  KAL_Dly(FS_QSPI_EN_DELAY_MIN_mS);                             // Wait before issuing 1st cmd after QSPI ctrlr start.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NOR_QSPI_ChipSelect()
 *
 * @brief    Configure the chip select.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    chip_select     chip select ID associated to the flash device.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_ChipSelect(const QSPI_DATA *p_drv_data,
                                      CPU_INT08U      chip_sel,
                                      RTOS_ERR        *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U reg_val;

  if (chip_sel >= 2u) {                                         // Only 2 chip selects supported by QSPI controller.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
    return;
  }
  //                                                               Select the active NOR flash device.
  reg_val = p_reg->CONFIG;
  DEF_BIT_CLR(reg_val, FS_QSPI_REG_CONFIG_PERSELDEC_MSK);       // Only one chip select active.
                                                                // Set the active chip select. Active low.
  DEF_BIT_FIELD_WR(reg_val, chip_sel, FS_QSPI_REG_CONFIG_PERCSLINES_MSK);
  p_reg->CONFIG = reg_val;
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_ClkOutSet()
 *
 * @brief    Configure the clock source for the connected flash device.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    ref_clk_hz      Input clock frequency used to generate serial clock to flash device.
 *
 * @param    sclk_out_hz     Desired serial clock frequency for the flash device.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_ClkOutSet(const QSPI_DATA *p_drv_data,
                                     CPU_INT32U      ref_clk_hz,
                                     CPU_INT32U      sclk_out_hz,
                                     RTOS_ERR        *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U reg_val;
  CPU_INT32U field_val;
  CPU_INT32U div;

  //                                                               Clear MSTRBAUDDIV field.
  reg_val = p_reg->CONFIG;
  DEF_BIT_CLR(reg_val, FS_QSPI_REG_CONFIG_BAUDDIV_MSK);
  //                                                               Compute baud rate divisor to generate sclk_out.
  div = ref_clk_hz / sclk_out_hz;

  //                                                               Check that baud rate divisor is valid.
  if ((div < FS_QSPI_REG_CONFIG_BAUDDIV_MIN)
      || (div > FS_QSPI_REG_CONFIG_BAUDDIV_MAX)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);                        // Divisor must be between specific boundaries.
    return;
  }
  if (!p_drv_data->PHY_En) {                                    // If PHY dis, divisor must meet some requirements.
    if (!p_drv_data->DTR_En) {
      if (div < FS_QSPI_REG_CONFIG_BAUDDIV_SDR_MIN) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);                    // Divisor must be at least 4 for SDR.
        return;
      }
    } else {
      if (div < FS_QSPI_REG_CONFIG_BAUDDIV_DDR_MIN) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);                    // Divisor must be at least 8 for DDR.
        return;
      }
    }
  }
  //                                                               Fit 'div' value on a 4-bit field value.
  if ((div & 1)) {                                              // Odd value.
    field_val = (div / 2);
  } else {                                                      // Even value.
    field_val = (div / 2) - 1;
  }

  LOG_VRB(("qspi_clk = ", (u)ref_clk_hz, "Hz, sclk_out(cfg'ed) = ", (u)sclk_out_hz, "Hz, Div ", (u)div, ", sclk_out(real) = ", (u)(ref_clk_hz / ((field_val + 1) * 2)), "Hz."));

  DEF_BIT_FIELD_WR(reg_val, field_val, FS_QSPI_REG_CONFIG_BAUDDIV_MSK);
  p_reg->CONFIG = reg_val;
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_StigRd()
 *
 * @brief    Perform a NOR read command by using the Software Triggered Instruction Generator (STIG).
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_cmd           Pointer to command descriptor for command to send.
 *
 * @param    inter_cycles    Number of intermediate data CYCLES to insert between address and data
 *                           phases.
 *
 * @param    p_xfer_data     Pointer to transfer buffer.
 *
 * @param    xfer_size       Size of transfer, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This function manages the following type of read commands:
 *               - (a) Flash device register read: command = instruction opcode + data
 *                     Read commands always involve data transfer from flash device to MCU. Single
 *                     operations such as reset, erase, write enable are considered as write command which
 *                     as only an instruction opcode or instruction opcode and address.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_StigRd(const QSPI_DATA                *p_drv_data,
                                  const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                  CPU_INT08U                     inter_cycles,
                                  void                           *p_xfer_data,
                                  CPU_INT32U                     xfer_size,
                                  RTOS_ERR                       *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT08U *p_rx_buf08 = (CPU_INT08U *)p_xfer_data;
  CPU_INT32U reg_val;
  CPU_INT32U read_len;
  CPU_INT08U dummy_bytes;

  //                                                               Argument checking.
  if ((!xfer_size)
      || (p_xfer_data == DEF_NULL)) {                           // Rd cmds have always data to read.
    LOG_ERR(("Invalid input arguments xfer_size ", (u)xfer_size, "p_xfer_data 0x", (x)((CPU_INT32U)p_xfer_data)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  if (xfer_size > FS_QSPI_STIG_DATA_LEN_MAX) {
    LOG_ERR(("READ cmd with data payload > 8 bytes NOT implemented yet, xfer_size ", (u)xfer_size, "p_xfer_data 0x", (x)((CPU_INT32U)p_xfer_data)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               ------------------ COMMAND SETUP -------------------
  reg_val = p_cmd->Opcode << FS_QSPI_REG_FLASHCMDCTRL_OPCODE_OFFSET;
  DEF_BIT_SET(reg_val, FS_QSPI_REG_FLASHCMDCTRL_RD_DATA_EN_MSK);
  //                                                               Read data size is 0-based. 0 means 1 byte and so on.
  reg_val |= (((xfer_size - 1) << FS_QSPI_REG_FLASHCMDCTRL_RD_DATA_BYTES_OFFSET) & FS_QSPI_REG_FLASHCMDCTRL_RD_DATA_BYTES_MSK);
  //                                                               Set dummy bytes.
  dummy_bytes = inter_cycles / FS_QSPI_DUMMY_CYCLES_PER_BYTE;
  if ((inter_cycles % FS_QSPI_DUMMY_CYCLES_PER_BYTE) != 0) {
    dummy_bytes += 1u;
  }
  reg_val |= (dummy_bytes << FS_QSPI_REG_FLASHCMDCTRL_DUMMY_BYTES_OFFSET) & FS_QSPI_REG_FLASHCMDCTRL_DUMMY_BYTES_MSK;

  //                                                               ---------------- COMMAND EXECUTION -----------------
  FS_NOR_QuadSPI_StigCmdFlashExec(p_drv_data,
                                  reg_val,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               -------------------- DATA READ ---------------------
  reg_val = p_reg->FLASHRDDATALOWER;                            // Read lower 4 bytes.
  read_len = (xfer_size > 4) ? 4 : xfer_size;
  Mem_Copy(p_rx_buf08,                                          // Place read bytes in correct order.
           &reg_val,
           read_len);

  if (xfer_size > 4) {                                          // Read upper 4 bytes if needed.
    p_rx_buf08 += xfer_size;
    reg_val = p_reg->FLASHRDDATAUPPER;

    read_len = xfer_size - read_len;
    Mem_Copy(p_rx_buf08,
             &reg_val,
             read_len);
  }
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_StigWr()
 *
 * @brief    Perform a NOR write command by using the Software Triggered Instruction Generator (STIG).
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_cmd           Pointer to command descriptor for command to send.
 *
 * @param    addr_tbl        Array of address octets, in MSB order.
 *
 * @param    inter_cycles    Number of intermediate data CYCLES to insert between address and data
 *                           phases.
 *
 * @param    p_xfer_data     Pointer to transfer buffer.
 *
 * @param    xfer_size       Size of transfer, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This function manages the following type of write commands:
 *               - (a) Flash device register write:                  command = instruction opcode + data
 *               - (b) Single operation such as reset, write enable: command = instruction opcode
 *               - (c) Single operation such as erase:               command = instruction opcode + address
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_StigWr(const QSPI_DATA                *p_drv_data,
                                  const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                  CPU_INT08U                     addr_tbl[],
                                  CPU_INT08U                     inter_cycles,
                                  void                           *p_xfer_data,
                                  CPU_INT32U                     xfer_size,
                                  RTOS_ERR                       *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT08U *p_tx_buf08 = (CPU_INT08U *)p_xfer_data;
  CPU_INT32U reg_val = 0u;
  CPU_INT32U flash_addr = 0u;
  CPU_INT08U addr_bytes;
  CPU_INT32U wr_data;
  CPU_INT32U wr_len;
  CPU_INT08U dummy_bytes;

  //                                                               ------------------ COMMAND SETUP -------------------
  reg_val = p_cmd->Opcode << FS_QSPI_REG_FLASHCMDCTRL_OPCODE_OFFSET;
  //                                                               Configure start address if needed.
  if (p_cmd->Form.HasAddr) {
    if (p_cmd->Form.AddrLen4Bytes) {                            // 4-byte address.
      addr_bytes = 4u;
    } else {                                                    // 3-byte address.
      addr_bytes = 3u;
    }
    //                                                             Address in little endian fmt on QSPI data line.
    MEM_VAL_COPY_SET_INTU_LITTLE(&flash_addr, &addr_tbl[0u], addr_bytes);
    p_reg->FLASHCMDADDR = flash_addr;

    DEF_BIT_SET(reg_val, ((addr_bytes - 1) << FS_QSPI_REG_FLASHCMDCTRL_NBR_ADDR_BYTES_OFFSET));
    DEF_BIT_SET(reg_val, FS_QSPI_REG_FLASHCMDCTRL_CMD_ADDR_EN_MSK);
  }
  //                                                               Set dummy bytes.
  dummy_bytes = inter_cycles / FS_QSPI_DUMMY_CYCLES_PER_BYTE;
  if ((inter_cycles % FS_QSPI_DUMMY_CYCLES_PER_BYTE) != 0) {
    dummy_bytes += 1u;
  }
  reg_val |= (dummy_bytes << FS_QSPI_REG_FLASHCMDCTRL_DUMMY_BYTES_OFFSET) & FS_QSPI_REG_FLASHCMDCTRL_DUMMY_BYTES_MSK;

  //                                                               -------------------- DATA WRITE --------------------
  if (xfer_size) {
    DEF_BIT_SET(reg_val, FS_QSPI_REG_FLASHCMDCTRL_WR_DATA_EN_MSK);
    //                                                             Write data size is 0-based. 0 means 1 byte and so on.
    reg_val |= (((xfer_size - 1) << FS_QSPI_REG_FLASHCMDCTRL_WR_DATA_BYTES_OFFSET) & FS_QSPI_REG_FLASHCMDCTRL_WR_DATA_BYTES_MSK);
    //                                                             Write lower 4 bytes.
    wr_len = xfer_size > 4 ? 4 : xfer_size;
    Mem_Copy(&wr_data,
             p_tx_buf08,
             wr_len);
    p_reg->FLASHWRDATALOWER = wr_data;

    if (xfer_size > 4) {                                        // Write upper 4 bytes.
      p_tx_buf08 += wr_len;
      wr_len = xfer_size - wr_len;
      Mem_Copy(&wr_data,
               p_tx_buf08,
               wr_len);
      p_reg->FLASHWRDATAUPPER = wr_data;
    }
  }
  //                                                               ---------------- COMMAND EXECUTION -----------------
  FS_NOR_QuadSPI_StigCmdFlashExec(p_drv_data,
                                  reg_val,
                                  p_err);
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_StigCmdFlashExec()
 *
 * @brief    Execute NOR command using the Software Triggered Instruction Generator (STIG) registers.
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    reg_val     Value to write in the register 'flashcmd'.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_StigCmdFlashExec(const QSPI_DATA *p_drv_data,
                                            CPU_INT32U      reg_val,
                                            RTOS_ERR        *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U retry;

  //                                                               Configure register without starting cmd execution.
  p_reg->FLASHCMDCTRL = reg_val;
  //                                                               Start command execution.
  DEF_BIT_SET(reg_val, FS_QSPI_REG_FLASHCMDCTRL_EXEC_CMD_MASK);
  p_reg->FLASHCMDCTRL = reg_val;

  retry = FS_QSPI_REG_RETRY2_MAX;
  while (retry > 0u) {                                          // Poll until command execution has finished.
    reg_val = p_reg->FLASHCMDCTRL;
    if (DEF_BIT_IS_CLR(reg_val, FS_QSPI_REG_FLASHCMDCTRL_EXEC_CMD_STATUS_MASK)) {
      break;
    }

    KAL_Dly(1);
    retry--;
  }

  if (retry == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  FS_NOR_QuadSPI_IdleCheck(p_drv_data, p_err);                  // Poll QSPI idle status to ensure QSPI has finished.
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_IdleCheck()
 *
 * @brief    Ensure QSPI controller is idle before executing next NOR command or verifying some
 *           controller's status.
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_IdleCheck(const QSPI_DATA *p_drv_data,
                                     RTOS_ERR        *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U idle_cnt = 0u;
  CPU_INT32U retry_cnt = 0u;
  CPU_INT32U reg_val;

  while ((idle_cnt < FS_QSPI_IDLE_CNT_MAX)
         && (retry_cnt < FS_QSPI_REG_RETRY1_MAX)) {
    reg_val = p_reg->CONFIG;
    if (DEF_BIT_IS_SET(reg_val, FS_QSPI_REG_CONFIG_IDLE_MSK)) {
      idle_cnt++;
    } else {
      idle_cnt = 0;
      retry_cnt++;
    }
  }

  if (retry_cnt == FS_QSPI_REG_RETRY1_MAX) {
    LOG_ERR(("QSPI still busy after flash cmd execution completion."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_IndirectRd()
 *
 * @brief    Perform a NOR read command using the indirect access controller module.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_cmd           Pointer to command descriptor for command to send.
 *
 * @param    addr_tbl        Array of address octets, in MSB order.
 *
 * @param    inter_cycles    Number of intermediate data CYCLES to insert between address and data
 *                           phases.
 *
 * @param    p_xfer_data     Pointer to transfer buffer.
 *
 * @param    xfer_size       Size of transfer, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This function handles the following read transactions:
 *               - (a) OPCODE + ADDR + DATA
 *               - (b) OPCODE + DATA (if DATA > 8 bytes)
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_IndirectRd(const QSPI_DATA                *p_drv_data,
                                      const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                      CPU_INT08U                     addr_tbl[],
                                      CPU_INT08U                     inter_cycles,
                                      void                           *p_xfer_data,
                                      CPU_INT32U                     xfer_size,
                                      RTOS_ERR                       *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U reg_val = 0u;
  CPU_INT32U flash_addr = 0u;
  CPU_INT08U addr_bytes;
  CPU_INT08U io_mode;

  //                                                               -------------------- READ SETUP --------------------
  //                                                               Configure SRAM partition for read.
  p_reg->SRAMPARTITIONCFG = (FS_QSPI_SRAM_PARTITION_RD_SIZE_LOC_QTY - 1u); // -1 because reg val is 0-based.
  //                                                               Setup the indirect trigger address.
  p_reg->INDAHBADDRTRIGGER = FS_QSPI_SRAM_FIFO_START_ADDR_SEL;
  p_reg->INDIRECTTRIGGERADDRRANGE = 0u;
  //                                                               Instruction transferred on DQ0 (1 bit).
  DEF_BIT_FIELD_WR(reg_val, FS_QSPI_XFER_MODE_SINGLE_IO, FS_QSPI_REG_DEVINSTRRDCONFIG_INST_WIDTH_MSK);
  //                                                               Configure the opcode.
  DEF_BIT_FIELD_WR(reg_val, p_cmd->Opcode, FS_QSPI_REG_DEVINSTRRDCONFIG_OPCODE_MSK);

  //                                                               Configure start address for indirect read.
  if (p_cmd->Form.AddrLen4Bytes) {                              // 4-byte address.
    addr_bytes = 4u;
  } else {                                                      // 3-byte address.
    addr_bytes = 3u;
  }
  //                                                               Address in little endian fmt on QSPI data line.
  MEM_VAL_COPY_SET_INTU_LITTLE(&flash_addr, &addr_tbl[0u], addr_bytes);
  p_reg->INDIRECTREADXFERSTART = flash_addr;
  //                                                               Set read address transfer width.
  if (p_cmd->Form.AddrMultiIO == DEF_YES) {                     // Read address transferred on DQ0, DQ1, DQ2 and DQ3.
    io_mode = FS_QSPI_XFER_MODE_QUAD_IO;
  } else {                                                      // Read address transferred on DQ0.
    io_mode = FS_QSPI_XFER_MODE_SINGLE_IO;
  }
  DEF_BIT_FIELD_WR(reg_val, io_mode, FS_QSPI_REG_DEVINSTRRDCONFIG_ADDR_WIDTH_MSK);
  //                                                               Set read data transfer width.
  if (p_cmd->Form.DataMultiIO == DEF_YES) {                     // Read data transferred on DQ0, DQ1, DQ2 and DQ3.
    io_mode = FS_QSPI_XFER_MODE_QUAD_IO;
  } else {                                                      // Read data transferred on DQ0.
    io_mode = FS_QSPI_XFER_MODE_SINGLE_IO;
  }
  DEF_BIT_FIELD_WR(reg_val, io_mode, FS_QSPI_REG_DEVINSTRRDCONFIG_DATA_WIDTH_MSK);

  //                                                               Set dummy cycles.
  if (inter_cycles > FS_QSPI_REG_DEVINSTRRDCONFIG_DUMMY_CLK_CYCLES_MAX) {
    inter_cycles = FS_QSPI_REG_DEVINSTRRDCONFIG_DUMMY_CLK_CYCLES_MAX;
  }
  DEF_BIT_FIELD_WR(reg_val, inter_cycles, FS_QSPI_REG_DEVINSTRRDCONFIG_DUMMY_CLK_CYCLES_MSK);

  p_reg->DEVINSTRRDCONFIG = reg_val;                            // Set Device Read reg with configured value.

  //                                                               Set nbr of addr bytes.
  DEF_BIT_FIELD_WR(p_reg->DEVSIZECONFIG, (addr_bytes - 1), FS_QSPI_REG_DEVSIZECONFIG_NBR_ADDR_MASK);

  //                                                               ------------------ READ EXECUTION ------------------
  p_reg->INDIRECTREADXFERNUMBYTES = xfer_size;                  // Set read xfer cnt.
                                                                // Start the indirect read transfer.
  DEF_BIT_SET(p_reg->INDIRECTREADXFERCTRL, FS_QSPI_REG_INDIRECTREADXFERCTRL_START_MASK);
  //                                                               Read data from flash device.
  FS_NOR_QuadSPI_IndirectRd_SRAM(p_drv_data,
                                 p_xfer_data,
                                 (const void *)FS_QSPI_SRAM_FIFO_START_ADDR_SEL,
                                 xfer_size,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Indirect read: reading data from flash failed with error ", (u)RTOS_ERR_CODE_GET(*p_err)));
  }
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_IndirectRd_SRAM()
 *
 * @brief    Read data from SRAM. Indirect access controller has received data from the flash device
 *           and stored them in SRAM.
 *
 * @param    p_drv_data  Pointer to driver-specific data.
 *
 * @param    p_dest_buf  Pointer to receive buffer.
 *
 * @param    p_src_sram  Pointer to source SRAM FIFO address.
 *
 * @param    xfer_size   Size of transfer, in octets.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) When a read transfer is in progress, 3 situations can occur:
 *               - (a) Transfer size < SRAM FIFO size: reading register SRAMFILL indicates the number of
 *                     bytes received from flash.
 *               - (b) Transfer size = SRAM FIFO size: reading register SRAMFILL indicates the number of
 *                     bytes received from flash.
 *               - (c) Transfer size > SRAM FIFO size: the bit SRAMFULL in register INDIRECTREADXFERCTRL
 *                     specifies the SRAM is full. Thus the total SRAM read partition size indicates the
 *                     number of bytes received from flash. When the SRAM is full, the register SRAMFILL
 *                     is not updated to the maximum read locations.
 *
 * @note     (2) The Indirect AHB Address Trigger register (INDAHBADDRTRIGGER) allows to read a SRAM
 *               FIFO location of the QSPI controller. By configuring the proper trigger address in
 *               this register, the CPU can perform a SRAM read by directly dereferencing the address
 *               configured in the register. This will provoke an AHB read from the Data Slave
 *               Controller of QSPI (slave interface) to the CPU (master interface). Then the CPU
 *               copies the read data to the provided receive buffer.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_IndirectRd_SRAM(const QSPI_DATA *p_drv_data,
                                           void            *p_dest_buf,
                                           const void      *p_src_sram,
                                           CPU_INT32U      xfer_size,
                                           RTOS_ERR        *p_err)
{
  QSPI_REG    *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U  *p_dest_buf32 = (CPU_INT32U *)p_dest_buf;
  CPU_INT32U  xfer_rem_size = xfer_size;
  CPU_INT32U  retry;
  CPU_INT32U  sram_fill_level = 0u;
  CPU_INT32U  sram_fill_threshold;
  CPU_INT32U  sram_entry_cnt;
  CPU_BOOLEAN clr_sram_full = DEF_NO;
  CPU_INT32U  dev_rd_reg = 0u;

  //                                                               Compute nbr of 32-bit locations for this xfer.
  sram_entry_cnt = xfer_size / FS_QSPI_SRAM_LOC_SIZE_BYTE;
  if ((xfer_size % FS_QSPI_SRAM_LOC_SIZE_BYTE) != 0) {
    sram_entry_cnt += 1;
  }

  while (xfer_rem_size > 0u) {                                  // Read entire xfer.
                                                                // Wait SRAM to be filled with enough data from flash.
    sram_fill_threshold = DEF_MIN(FS_QSPI_SRAM_PARTITION_RD_SIZE_LOC_QTY, sram_entry_cnt);
    retry = FS_QSPI_REG_RETRY2_MAX;
    while (retry > 0u) {
      if (DEF_BIT_IS_SET(p_reg->INDIRECTREADXFERCTRL, FS_QSPI_REG_INDIRECTREADXFERCTRL_SRAM_FULL_MASK)) {
        sram_fill_level = sram_fill_threshold;                  // If SRAM full, set the fill level by sw (see Note #1).
        clr_sram_full = DEF_YES;
      } else {
        sram_fill_level = p_reg->SRAMFILL & FS_QSPI_REG_SRAMFILL_INDRDPART_MASK;
      }
      if (sram_fill_level == sram_fill_threshold) {
        break;
      }
      retry--;
    }

    if (retry == 0u) {
      LOG_ERR(("Indirect read: not enough data in SRAM after polling for ", (u)FS_QSPI_REG_RETRY2_MAX, " times"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    while (sram_fill_level) {                                   // Read 1 chunk of SRAM (= part of xfer).
                                                                // 32-bit word read.
      if (xfer_rem_size >= FS_QSPI_SRAM_LOC_SIZE_BYTE) {
        *p_dest_buf32 = *(CPU_INT32U *)p_src_sram;              // See Note #2.
        xfer_rem_size -= FS_QSPI_SRAM_LOC_SIZE_BYTE;
        p_dest_buf32++;
      } else {                                                  // Last 32-bit word read.
        CPU_INT32U last_data32 = 0u;

        last_data32 = *(CPU_INT32U *)p_src_sram;
        Mem_Copy((void *)p_dest_buf32,
                 (void *)&last_data32,
                 xfer_rem_size);
        xfer_rem_size -= xfer_rem_size;
      }
      sram_entry_cnt--;
      sram_fill_level--;
    }
  }

  //                                                               Check if indirect read done.
  dev_rd_reg = p_reg->INDIRECTREADXFERCTRL;
  if (!DEF_BIT_IS_SET(dev_rd_reg, FS_QSPI_REG_INDIRECTREADXFERCTRL_OP_DONE_MASK)) {
    LOG_ERR(("Indirect read: reading data OK but indirect operation still indicated as not done, dev_rd_reg = ", (X)dev_rd_reg));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    goto rd_err;
  }
  if (clr_sram_full) {                                          // If SRAM was full, clear the indicator flag.
    DEF_BIT_SET(dev_rd_reg, FS_QSPI_REG_INDIRECTREADXFERCTRL_SRAM_FULL_MASK);
  }
  //                                                               Clear indirect completion status.
  DEF_BIT_SET(dev_rd_reg, FS_QSPI_REG_INDIRECTREADXFERCTRL_OP_DONE_MASK);
  p_reg->INDIRECTREADXFERCTRL = dev_rd_reg;
  return;

rd_err:
  //                                                               Cancel the indirect read.
  DEF_BIT_SET(p_reg->INDIRECTREADXFERCTRL, FS_QSPI_REG_INDIRECTREADXFERCTRL_CANCEL_MASK);
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_DirectRd()
 *
 * @brief    Perform a NOR read command using the direct access controller module.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_cmd           Pointer to command descriptor for command to send.
 *
 * @param    addr_tbl        Array of address octets, in MSB order.
 *
 * @param    p_xfer_data     Pointer to transfer buffer.
 *
 * @param    xfer_size       Size of transfer, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) In direct mode, the flash address must be absolute as opposed to relative address
 *               used in indirect mode. That is the absolute mode is formed from the base address
 *               assigned to the flash device in the CPU memory space to which the address relative
 *               to flash device address range is added. For example, if the flash device base address
 *               in CPU memory is 0XC0000000 and the flash relative address is 0x100, then the
 *               absolute address is 0xC0000100.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_DirectRd(const QSPI_DATA                *p_drv_data,
                                    const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                    CPU_INT08U                     addr_tbl[],
                                    void                           *p_xfer_data,
                                    CPU_INT32U                     xfer_size,
                                    RTOS_ERR                       *p_err)
{
  CPU_ADDR   flash_addr = 0u;
  CPU_INT08U addr_bytes;

  PP_UNUSED_PARAM(p_err);

  //                                                               Configure start address for direct read.
  if (p_cmd->Form.AddrLen4Bytes) {                              // 4-byte address.
    addr_bytes = 4u;
  } else {                                                      // 3-byte address.
    addr_bytes = 3u;
  }
  //                                                               Form absolute flash addr (see Note #1).
  MEM_VAL_COPY_SET_INTU_LITTLE(&flash_addr, &addr_tbl[0u], addr_bytes);
  flash_addr += p_drv_data->QuadSPI_CtrlrInfoPtr->FlashMemMapStartAddr;

  Mem_Copy(p_xfer_data,
           (void *)flash_addr,
           xfer_size);
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_IndirectWr()
 *
 * @brief    Perform a NOR write command using the indirect access controller module.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_cmd           Pointer to command descriptor for command to send.
 *
 * @param    addr_tbl        Array of address octets, in MSB order.
 *
 * @param    inter_cycles    Number of intermediate data CYCLES to insert between address and data
 *                           phases.
 *
 * @param    p_xfer_data     Pointer to transfer buffer.
 *
 * @param    xfer_size       Size of transfer, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The instruction width for write commands is set in the register Device Read
 *               Instruction Configuration (DEVINSTRRDCONFIG) and not in the register Device Write
 *               Instruction Configuration (DEVINSTRWRCONFIG).
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_IndirectWr(const QSPI_DATA                *p_drv_data,
                                      const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                      CPU_INT08U                     addr_tbl[],
                                      CPU_INT08U                     inter_cycles,
                                      void                           *p_xfer_data,
                                      CPU_INT32U                     xfer_size,
                                      RTOS_ERR                       *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U reg_val = 0u;
  CPU_INT32U flash_addr = 0u;
  CPU_INT08U addr_bytes;
  CPU_INT08U io_mode;
  CPU_INT32U retry;

  //                                                               ------------------- WRITE SETUP --------------------
  //                                                               Configure SRAM partition for write.
  p_reg->SRAMPARTITIONCFG = FS_QSPI_SRAM_PARTITION_WR_SIZE_RD_LOC_QTY;
  //                                                               Setup the indirect trigger address.
  p_reg->INDAHBADDRTRIGGER = FS_QSPI_SRAM_FIFO_START_ADDR_SEL;
  p_reg->INDIRECTTRIGGERADDRRANGE = 0u;
  //                                                               Instruction transferred on DQ0 (see Note #1).
  DEF_BIT_FIELD_WR(p_reg->DEVINSTRRDCONFIG, FS_QSPI_XFER_MODE_SINGLE_IO, FS_QSPI_REG_DEVINSTRRDCONFIG_INST_WIDTH_MSK);
  //                                                               Configure the opcode.
  DEF_BIT_FIELD_WR(reg_val, p_cmd->Opcode, FS_QSPI_REG_DEVINSTRWRCONFIG_OPCODE_MSK);

  //                                                               Configure start address for indirect write.
  if (p_cmd->Form.AddrLen4Bytes) {                              // 4-byte address.
    addr_bytes = 4u;
  } else {                                                      // 3-byte address.
    addr_bytes = 3u;
  }
  //                                                               Address in little endian fmt on QSPI data line.
  MEM_VAL_COPY_SET_INTU_LITTLE(&flash_addr, &addr_tbl[0u], addr_bytes);
  p_reg->INDIRECTWRITEXFERSTART = flash_addr;
  //                                                               Set write address transfer width.
  if (p_cmd->Form.AddrMultiIO == DEF_YES) {                     // Write address transferred on DQ0, DQ1, DQ2 and DQ3.
    io_mode = FS_QSPI_XFER_MODE_QUAD_IO;
  } else {                                                      // Write address transferred on DQ0.
    io_mode = FS_QSPI_XFER_MODE_SINGLE_IO;
  }
  DEF_BIT_FIELD_WR(reg_val, io_mode, FS_QSPI_REG_DEVINSTRWRCONFIG_ADDR_WIDTH_MSK);
  //                                                               Set write data transfer width.
  if (p_cmd->Form.DataMultiIO == DEF_YES) {                     // Write data transferred on DQ0, DQ1, DQ2 and DQ3.
    io_mode = FS_QSPI_XFER_MODE_QUAD_IO;
  } else {                                                      // Write data transferred on DQ0.
    io_mode = FS_QSPI_XFER_MODE_SINGLE_IO;
  }
  DEF_BIT_FIELD_WR(reg_val, io_mode, FS_QSPI_REG_DEVINSTRWRCONFIG_DATA_WIDTH_MSK);
  //                                                               Set dummy cycles.

  if (inter_cycles > FS_QSPI_REG_DEVINSTRWRCONFIG_DUMMY_CLK_CYCLES_MAX) {
    inter_cycles = FS_QSPI_REG_DEVINSTRWRCONFIG_DUMMY_CLK_CYCLES_MAX;
  }
  DEF_BIT_FIELD_WR(reg_val, inter_cycles, FS_QSPI_REG_DEVINSTRWRCONFIG_DUMMY_CLK_CYCLES_MSK);

  p_reg->DEVINSTRWRCONFIG = reg_val;                            // Set Device Write reg with configured value.

  //                                                               Set nbr of addr bytes.
  DEF_BIT_FIELD_WR(p_reg->DEVSIZECONFIG, (addr_bytes - 1), FS_QSPI_REG_DEVSIZECONFIG_NBR_ADDR_MASK);

  //                                                               ----------------- WRITE EXECUTION ------------------
  p_reg->INDIRECTWRITEXFERNUMBYTES = xfer_size;                 // Set write xfer cnt.
                                                                // Start the indirect write transfer.
  DEF_BIT_SET(p_reg->INDIRECTWRITEXFERCTRL, FS_QSPI_REG_INDIRECTWRITEXFERCTRL_START_MASK);
  //                                                               Write data to flash device.
  FS_NOR_QuadSPI_IndirectWr_SRAM(p_drv_data,
                                 (const void *)FS_QSPI_SRAM_FIFO_START_ADDR_SEL,
                                 p_xfer_data,
                                 xfer_size,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Indirect write: writing to flash failed with error ", (u)RTOS_ERR_CODE_GET(*p_err)));
    goto wr_err;
  }
  //                                                               Check flash indirect controller status.
  retry = FS_QSPI_REG_RETRY1_MAX;
  while (retry > 0u) {
    reg_val = p_reg->INDIRECTWRITEXFERCTRL;
    if (DEF_BIT_IS_SET(reg_val, FS_QSPI_REG_INDIRECTWRITEXFERCTRL_OP_DONE_MASK)) {
      break;
    }
    KAL_Dly(1);
    retry--;
  }
  if (retry == 0u) {
    LOG_ERR(("Indirect write: writing data OK but indirect operation still indicated as not done, dev_wr_reg = ", (X)reg_val));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    goto wr_err;
  }
  //                                                               Clear indirect completion status.
  DEF_BIT_SET(p_reg->INDIRECTWRITEXFERCTRL, FS_QSPI_REG_INDIRECTWRITEXFERCTRL_OP_DONE_MASK);
  return;

wr_err:
  //                                                               Cancel the indirect write.
  DEF_BIT_SET(p_reg->INDIRECTWRITEXFERCTRL, FS_QSPI_REG_INDIRECTWRITEXFERCTRL_CANCEL_MASK);
}

/****************************************************************************************************//**
 *                                       FS_NOR_QuadSPI_IndirectWr_SRAM()
 *
 * @brief    Write data to SRAM. Indirect access controller will then write these data to the flash
 *           device.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_dest_sram     Pointer to source SRAM FIFO address.
 *
 * @param    p_src_buf       Pointer to transmit buffer.
 *
 * @param    xfer_size       Size of transfer, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The Indirect AHB Address Trigger register (INDAHBADDRTRIGGER) allows to write a SRAM
 *               location of the QSPI controller. By configuring the proper trigger address in this
 *               register, the CPU can perform a SRAM write by directly dereferencing the address
 *               configured in the register. This will provoke an AHB write from the CPU (master
 *               interface) to the Data Slave Controller of QSPI (slave interface). Then data will be
 *               written to the SRAM.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_IndirectWr_SRAM(const QSPI_DATA *p_drv_data,
                                           const void      *p_dest_sram,
                                           void            *p_src_buf,
                                           CPU_INT32U      xfer_size,
                                           RTOS_ERR        *p_err)
{
  QSPI_REG   *p_reg = (QSPI_REG *)p_drv_data->QuadSPI_CtrlrInfoPtr->BaseAddr;
  CPU_INT32U *p_src_buf32 = (CPU_INT32U *)p_src_buf;
  CPU_INT32U *p_dest_fifo32 = (CPU_INT32U *)p_dest_sram;        // See Note #1.
  CPU_INT32U xfer_rem_size = xfer_size;
  CPU_INT32U sram_fill_level;
  CPU_INT32U sram_entry_avail;
  CPU_INT32U sram_bytes_avail;
  CPU_INT32U wr_bytes;
  CPU_INT32U retry;
  CPU_INT32U page_size;

  page_size = DEF_BIT_FIELD_RD(p_reg->DEVSIZECONFIG, FS_QSPI_REG_DEVSIZECONFIG_PAGE_SIZE_MASK);

  while (xfer_rem_size > 0u) {
    //                                                             Write a page or remaining bytes.
    wr_bytes = DEF_MIN(page_size, xfer_rem_size);

    //                                                             Wait SRAM to be enough empty to send data to flash.
    retry = FS_QSPI_REG_RETRY2_MAX;
    while (retry > 0u) {
      sram_fill_level = DEF_BIT_FIELD_RD(p_reg->SRAMFILL, FS_QSPI_REG_SRAMFILL_INDWRPART_MASK);
      sram_entry_avail = FS_QSPI_SRAM_PARTITION_WR_SIZE_LOC_QTY_REAL - sram_fill_level;
      sram_bytes_avail = sram_entry_avail * FS_QSPI_SRAM_LOC_SIZE_BYTE;

      if (sram_bytes_avail >= wr_bytes) {
        break;
      }
      retry--;
    }
    if (retry == 0u) {
      LOG_ERR(("Indirect write: not enough room in SRAM after polling for ", (u)FS_QSPI_REG_RETRY2_MAX, " times"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    while (wr_bytes > 0u) {
      //                                                           32-bit word write.
      if (wr_bytes >= FS_QSPI_SRAM_LOC_SIZE_BYTE) {
        *p_dest_fifo32 = *p_src_buf32;

        wr_bytes -= FS_QSPI_SRAM_LOC_SIZE_BYTE;
        xfer_rem_size -= FS_QSPI_SRAM_LOC_SIZE_BYTE;
        p_src_buf32++;
      } else {                                                  // Last 32-bit word write.
        CPU_INT32U last_data32 = 0u;

        Mem_Copy((void *)&last_data32,
                 (void *)p_src_buf32,
                 wr_bytes);
        *p_dest_fifo32 = last_data32;
        xfer_rem_size -= wr_bytes;
        wr_bytes -= wr_bytes;
      }
    }
  }
  //                                                               Wait until last write is completed (FIFO empty).
  retry = FS_QSPI_REG_RETRY1_MAX;
  while (retry > 0u) {
    sram_fill_level = DEF_BIT_FIELD_RD(p_reg->SRAMFILL, FS_QSPI_REG_SRAMFILL_INDWRPART_MASK);
    if (sram_fill_level == 0) {
      break;
    }
    KAL_Dly(1);
    retry--;
  }
  if (retry == 0u) {
    LOG_ERR(("Indirect write: last write did not complete after ", (u)FS_QSPI_REG_RETRY1_MAX, " retries"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                           FS_NOR_QuadSPI_DirectWr()
 *
 * @brief    Perform a NOR write command using the direct access controller module.
 *
 * @param    p_drv_data      Pointer to driver-specific data.
 *
 * @param    p_cmd           Pointer to command descriptor for command to send.
 *
 * @param    addr_tbl        Array of address octets, in MSB order.
 *
 * @param    p_xfer_data     Pointer to transfer buffer.
 *
 * @param    xfer_size       Size of transfer, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) See Note #1 of FS_NOR_QuadSPI_DirectRd().
 *
 * @note     (2) The Quad SPI controller documentation indicates the following for direct writes:
 *               "The page boundary detection is only guaranteed for word (4-byte) aligned accesses.
 *               It is therefore recommended to ensure that all writes are word aligned".
 *               When a flash address not aligned is detected, a logic is implemented to handle this
 *               unaligned direct write so that an aligned write is performed.
 *******************************************************************************************************/
static void FS_NOR_QuadSPI_DirectWr(const QSPI_DATA                *p_drv_data,
                                    const FS_NOR_QUAD_SPI_CMD_DESC *p_cmd,
                                    CPU_INT08U                     addr_tbl[],
                                    void                           *p_xfer_data,
                                    CPU_INT32U                     xfer_size,
                                    RTOS_ERR                       *p_err)
{
  CPU_ADDR   flash_addr = 0u;
  CPU_INT08U addr_bytes;

  PP_UNUSED_PARAM(p_err);

  //                                                               Configure start address for direct read.
  if (p_cmd->Form.AddrLen4Bytes) {                              // 4-byte address.
    addr_bytes = 4u;
  } else {                                                      // 3-byte address.
    addr_bytes = 3u;
  }
  //                                                               Form absolute flash addr (see Note #1).
  MEM_VAL_COPY_SET_INTU_LITTLE(&flash_addr, &addr_tbl[0u], addr_bytes);
  flash_addr += p_drv_data->QuadSPI_CtrlrInfoPtr->FlashMemMapStartAddr;

  Mem_Copy((void *)flash_addr,
           p_xfer_data,
           xfer_size);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_NOR_AVAIL
