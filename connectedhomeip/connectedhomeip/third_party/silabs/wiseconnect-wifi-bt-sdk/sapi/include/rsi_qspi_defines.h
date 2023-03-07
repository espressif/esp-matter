/*******************************************************************************
* @file  rsi_qspi_defines.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#ifndef QSPI_DEFINES_H
#define QSPI_DEFINES_H

// This structure members are used to configure qspi
typedef struct spi_config_1_s {
  /**
   * QSPI operation modes, all modes are single, dual or quad 
   **/
  uint32_t inst_mode : 2;       //< instruction will be sent in this mode
  uint32_t addr_mode : 2;       //< addr will be sent in this mode
  uint32_t data_mode : 2;       //< data will be sent/received in this mode
  uint32_t dummy_mode : 2;      //< dummy bytes will be sent/received in this mode
  uint32_t extra_byte_mode : 2; //< extra bytes will be sent in this mode
  // SPI mode
#define SINGLE_MODE 0
  // dual mode
#define DUAL_MODE 1
  // quad mode
#define QUAD_MODE 2
#define OCTA_MODE 3

  uint32_t prefetch_en : 1; //< prefetch enable
  // prefetch will be enabled
#define EN_PREFETCH 1
  // prefetch will be disabled
#define DIS_PREFETCH 0

  uint32_t dummy_W_or_R : 1; //< dummy writes or read select
  // dummy's are read
#define DUMMY_READS 0
  // dummy's are written
#define DUMMY_WRITES 1

  uint32_t extra_byte_en : 1; //< Enable extra byte
  // Extra byte will be enabled
#define EN_EXTRA_BYTE
  // Extra byte will be disabled
#define DIS_EXTRA_BYTE

  uint32_t d3d2_data : 2; //< Data on D3 and D2 line in SPI or DUAL mode

  uint32_t continuous : 1; //< continuous mode select
  // continuous mode is selected
#define CONTINUOUS 1
  // discontinuous mode is selected
#define DIS_CONTINUOUS 0

  uint32_t read_cmd : 8; //< read cmd to be used
// fast read dual output
#define FREAD_DUAL_O 0x3B
// fast read quad output
#define FREAD_QUAD_O 0x6B

#define FREAD_QUAD_O_EB 0xEB

/****************************************
 * WINBOND + MACRONIX specific cmds
 ***************************************/
// fast read dual IO
#define FREAD_DUAL_IO 0xBB
// fast read quad IO
#define FREAD_QUAD_IO 0xEB

  uint32_t flash_type : 4; //< flash defines
  // sst spi flash
#define SST_SPI_FLASH 0
  // sst dual flash
#define SST_DUAL_FLASH 1
  // sst quad flash
#define SST_QUAD_FLASH 2
  // Winbond quad flash
#define WBOND_QUAD_FLASH 3
  // Atmel quad flash
#define AT_QUAD_FLASH 4
  // macronix quad flash
#define MX_QUAD_FLASH 5
  // cFeon quad flash
#define EON_QUAD_FLASH 6
  // Micron quad flash
#define MICRON_QUAD_FLASH 7
  // Giga Device flash
#define GIGA_DEVICE_FLASH 8
  // macronix octa flash
#define MX_OCTA_FLASH 9
  // Adesto octa flash
#define ADESTO_OCTA_FLASH 10

  uint32_t no_of_dummy_bytes : 4; //< no_of_dummy_bytes to be used for read operations
} spi_config_1_t;

// This structure members are used to configure qspi
typedef struct spi_config_2_s {

  uint32_t auto_mode : 1; //< mode select
  // Auto mode selection
#define EN_AUTO_MODE 1
  // Manual mode selection
#define EN_MANUAL_MODE 0

  uint32_t cs_no : 2; //<  QSPI chip_select
  // cs-0
#define CHIP_ZERO 0
  // cs-1
#define CHIP_ONE 1
  // cs-2
#define CHIP_TWO 2
  // cs-3
#define CHIP_THREE 3

  uint32_t reserved1 : 1; //< Jump Enable
  // Enables jump
#define EN_JUMP 1
  // Disables jump
#define DIS_JUMP 0

  uint32_t neg_edge_sampling : 1; //< For High speed mode, sample at neg edge
  // enables neg edge sampling
#define NEG_EDGE_SAMPLING 1
  // enables pos edge sampling
#define POS_EDGE_SAMPLING 0

  uint32_t qspi_clk_en : 1; //< qspi clk select
  // full time clk will be provided
#define QSPI_FULL_TIME_CLK 1
  // dynamic clk gating will be enabled
#define QSPI_DYNAMIC_CLK 0

  uint32_t protection : 2; //< flash protection select
  // enable write protection
#define EN_WR_PROT 2
  // remove write protection
#define REM_WR_PROT 1
  // no change to wr protection
#define DNT_REM_WR_PROT 0

  uint32_t dma_mode : 1; //< dma mode enable
  // use dma only in manaul mode
#define DMA_MODE 1
  // dma will not be used
#define NO_DMA 0

  uint32_t swap_en : 1; //< swap enable for w/r
  // swap will be enabled
#define SWAP 1
  // swap will be disabled
#define NO_SWAP 0

  uint32_t full_duplex : 2; //< full duplex mode select
  // do nothing for full duplex
#define IGNORE_FULL_DUPLEX 2
  // enable full duplex
#define EN_FULL_DUPLEX 1
  // disable full duplex
#define DIS_FULL_DUPLEX 0

  uint32_t wrap_len_in_bytes : 3; //< wrap len to be used
  // wrap is diabled
#define NO_WRAP 7
  // 8 byte wrap will be used
#define SST_8BYTE_WRAP 0
  // 16 byte wrap will be used
#define SST_16BYTE_WRAP 1
  // 32 byte wrap will be used
#define SST_32BYTE_WRAP 2
  // 64 byte wrap will be used
#define SST_64BYTE_WRAP 3

  // 16 byte wrap will be used
#define MICRON_16BYTE_WRAP 0
  // 32 byte wrap will be used
#define MICRON_32BYTE_WRAP 1
  // 64 byte wrap will be used
#define MICRON_64BYTE_WRAP 2

  uint32_t addr_width_valid : 1;

  uint32_t addr_width : 3; //< addr width to used
  // 32 bit addr is configured
#define _32BIT_ADDR 4
  // 24 bit addr is configured
#define _24BIT_ADDR 3
  // 16 bit addr is configured
#define _16BIT_ADDR 2
  // 9 bit addr is configured
#define _9BIT_ADDR 1
  // 8 bit addr is configured
#define _8BIT_ADDR 0

#define MANUAL_DUMMY_BYTE_OR_BIT_MODE BIT(25)
#define DUMMY_BYTE_OR_BIT_MODE        BIT(0)
  uint32_t dummy_cycles_for_controller : 2;

  uint32_t reserved2 : 6;
  //  uint32_t jump_inst : 8;         //< Instruction to be used in case of jump

  uint32_t pinset_valid : 1;

  uint32_t flash_pinset : 4; //< width of memory protection reg for sst flashes

} spi_config_2_t;

// This structure members are used to configure qspi
typedef struct spi_config_3_s {
#define CONTINUE_FETCH_EN BIT(12)
#define WORD_SWAP_EN      20
  uint32_t en_word_swap : 1;
  uint32_t _16bit_cmd_valid : 1;
  uint32_t _16bit_rd_cmd_msb : 8;
  uint32_t xip_mode : 1;
  uint32_t no_of_dummy_bytes_wrap : 4; //< no_of_dummy_bytes to be used for wrap operations
  uint32_t ddr_mode_en : 1;
  uint32_t wr_cmd : 8;
  uint32_t wr_inst_mode : 2;
  uint32_t wr_addr_mode : 2;
  uint32_t wr_data_mode : 2;
  uint32_t dummys_4_jump : 2; //< no_of_dummy_bytes in case of jump instruction
} spi_config_3_t;

typedef struct spi_config_4_s {
  uint32_t _16bit_wr_cmd_msb : 8;
  uint32_t qspi_ddr_clk_en : 1;
  uint32_t qspi_loop_back_mode_en : 1;
  uint32_t qspi_manual_ddr_phasse : 1;
  uint32_t ddr_data_mode : 1;
  uint32_t ddr_inst_mode : 1;
  uint32_t ddr_addr_mode : 1;
  uint32_t ddr_dummy_mode : 1;
  uint32_t ddr_extra_byte : 1;
  uint32_t dual_flash_mode : 1;
  uint32_t secondary_csn : 1;
  uint32_t polarity_mode : 1;
  uint32_t valid_prot_bits : 4;
  uint32_t no_of_ms_dummy_bytes : 4;
  uint32_t ddr_dll_en : 1;
  uint32_t continue_fetch_en : 1;
  uint32_t dma_write : 1;
  uint32_t prot_top_bottom : 1;
  uint32_t auto_csn_based_addr_en : 1;
} spi_config_4_t;
typedef struct spi_config_5_s {
  uint32_t block_erase_cmd : 16;
  uint32_t busy_bit_pos : 3;
  uint32_t d7_d4_data : 4;
  uint32_t dummy_bytes_for_rdsr : 4;
  uint32_t reset_type : 5;
} spi_config_5_t;

typedef struct spi_config_6_s {
  uint32_t chip_erase_cmd : 16;
  // chip erase cmd
#define CHIP_ERASE 0xC7
  uint32_t sector_erase_cmd : 16;
// block erase cmd
#define BLOCK_ERASE 0xD8
  // sector erase cmd
#define SECTOR_ERASE 0x20

} spi_config_6_t;

typedef struct spi_config_7_s {
  uint32_t status_reg_write_cmd : 16;
  uint32_t status_reg_read_cmd : 16;
} spi_config_7_t;

// This structure has two daughter structures to configure qspi
typedef struct spi_config_s {
  spi_config_1_t spi_config_1; //< daughter structure 1
  spi_config_2_t spi_config_2; //< daughter structure 2
  spi_config_3_t spi_config_3; //< daughter structure 3
  spi_config_4_t spi_config_4; //< daughter structure 4
  spi_config_5_t spi_config_5; //< daughter structure 5
  spi_config_6_t spi_config_6; //< daughter structure 5
  spi_config_7_t spi_config_7; //< daughter structure 5
} spi_config_t;

#endif
