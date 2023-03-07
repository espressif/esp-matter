/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fsl_nor_flash.h"
#include "fsl_flexspi_nor_flash.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_24BIT_ADDRESSING_SIZE (16UL * 1024 * 1024)
#define NOR_CMD_LUT_FOR_IP_CMD 1
#define SFDP_JESD216_SIGNATURE 0x50444653U
#define SFDP_JESD216_MAJOR     1
#define SFDP_JESD216_MINOR     0
#define SFDP_JESD216A_MINOR    5
#define SFDP_JESD216B_MINOR    6
#define SFDP_JESD216C_MINOR    7
#define SFDP_JESD216D_MINOR    8

static flexspi_mem_nor_handle_t flexspiMemHandle;

/*! @brief Misc feature bit definitions. */
enum
{
    kFlexSpiMiscOffset_DiffClkEnable = 0,            /*!< Bit for Differential clock enable*/
    kFlexSpiMiscOffset_Ck2Enable = 1,                /*!< Bit for CK2 enable*/
    kFlexSpiMiscOffset_ParallelEnable = 2,           /*!< Bit for Parallel mode enable*/
    kFlexSpiMiscOffset_WordAddressableEnable = 3,    /*!< Bit for Word Addressable enable*/
    kFlexSpiMiscOffset_SafeConfigFreqEnable = 4,     /*!< Bit for Safe Configuration Frequency enable*/
    kFlexSpiMiscOffset_PadSettingOverrideEnable = 5, /*!< Bit for Pad setting override enable*/
    kFlexSpiMiscOffset_DdrModeEnable = 6,            /*!< Bit for DDR clock confiuration indication.*/
};

/*! @brief Flash read sample clock source. */
typedef enum _FlashReadSampleClkSource
{
    kFlexSPIReadSampleClk_LoopbackInternally = 0,
    kFlexSPIReadSampleClk_LoopbackFromDqsPad = 1,
    kFlexSPIReadSampleClk_LoopbackFromSckPad = 2,
    kFlexSPIReadSampleClk_ExternalInputFromDqsPad = 3,
} flexspi_read_sample_clk_t;

/*! @brief NOR flash command sequence instruction. */
enum
{
    kSerialNorCmd_BasicRead_3B = 0x03,
    kSerialNorCmd_BasicRead_4B = 0x13,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_PageProgram_1_1_1_3B = 0x02,
    kSerialNorCmd_PageProgram_1_1_1_4B = 0x12,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_PageProgram_1_4_4_4B = 0x3E,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_PageProgram_1_1_4_4B = 0x34,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_4_4_3B = 0xEB,
    kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_3B = 0xED,
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_1_1_4B = 0x0C,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_4_4_4B = 0xEC,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_1_4_4B = 0x6C,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_1_1_4B = 0x0E,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_4B = 0xEE,/* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_ChipErase = 0x60, /* 0x60 or 0xC7 */
    kSerialNorCmd_WriteEnable = 0x06,     /*See instruction set table in any serial flash RM. */
    kSerialNorCmd_WriteDisable = 0x04,    /*See instruction set table in any serial flash RM. */
    kSerialNorCmd_WriteStatusReg1 = 0x01, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadStatusReg1 = 0x05,  /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_WriteStatusReg2Bit1 = 0x01, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_WriteStatusReg2Bit7 = 0x3E, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadStatusReg2Bit1 = 0x35,  /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadStatusReg2Bit7 = 0x3F,  /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadFlagReg = 0x70,
    kSerialNorCmd_ReadId = 0x9F,
};

enum
{
    kSerialNorQpiMode_NotConfig = 0,
    kSerialNorQpiMode_Cmd_0x38 = 1,
    kSerialNorQpiMode_Cmd_0x38_QE = 2,
    kSerialNorQpiMode_Cmd_0x35 = 3,
    kSerialNorQpiMode_Cmd_0x71 = 4,
    kSerialNorQpiMode_Cmd_0x61 = 5,
};

enum
{
    kSerialNorType_StandardSPI, /*!< Device that support Standard SPI and Extended SPI mode*/
    kSerialNorType_HyperBus,    /*!< Device that supports HyperBus only*/
    kSerialNorType_XPI,         /*!< Device that works under DPI, QPI or OPI mode*/
    kSerialNorType_NoCmd,       /*!< Device that works under No command mode (XIP mode/Performance Enhance mode/continous read*/
};

typedef struct _lut_seq
{
    uint32_t lut[4];
} lut_seq_t;

enum
{
    kSerialNOR_IndividualMode = false,
    kSerialNOR_ParallelMode = true,
};

enum
{
    kFlexSpiSerialClk_Update,
    kFlexSpiSerialClk_Restore
};

/*! @brief NOR flash command sequence instruction for SFDP. */
enum
{
    kSerialFlash_ReadSFDP = 0x5A,
    kSerialFlash_ReadJEDECId = 0x9F,
};

/*! @brief NOR flash SFDP header. */
typedef struct _sfdp_header
{
    uint32_t signature;
    uint8_t minor_rev;
    uint8_t major_rev;
    uint8_t param_hdr_num;
    uint8_t reserved;
} sfdp_header_t;

/*! @brief Function specific parameter table ID assignments. */
enum
{
    kParameterID_BasicSpiProtocal = 0xFF00,             /* Basic Flash Parameter Table */
    kParameterID_SectorMap = 0xFF81,                    /* Sector Map Table */
    kParameterID_4ByteAddressInstructionTable = 0xFF84, /* 4-byte Address Instruction Table */
};

/*! @brief SFDP Parameter Header. */
typedef struct _sfdp_parameter_header
{
    uint8_t parameter_id_lsb;
    uint8_t minor_rev;
    uint8_t major_rev;
    uint8_t table_length_in_32bit;
    uint8_t parameter_table_pointer[3];
    uint8_t parameter_id_msb;
} sfdp_parameter_header_t;

#define SFDP_PARAM_HEADER_ID(p)	(((p).parameter_id_msb << 8) | (p).parameter_id_lsb)
#define SFDP_PARAM_HEADER_PTP(p) \
	(((p).parameter_table_pointer[2] << 16) | \
	 ((p).parameter_table_pointer[1] <<  8) | \
	 ((p).parameter_table_pointer[0] <<  0))

/*! @brief Basic Flash Parameter Table. */
typedef struct _jedec_basic_flash_param_table
{
    /* 1st DWORD */
    struct
    {
        uint32_t erase_size : 2;
        uint32_t write_granularity : 1;
        uint32_t vsr_block_protect : 1;
        uint32_t vsr_write_enable : 1;
        uint32_t unused0 : 3;
        uint32_t erase4k_inst : 8;
        uint32_t support_1_1_2_fast_read : 1;
        uint32_t address_bytes : 2;
        uint32_t support_dtr_clocking : 1;
        uint32_t support_1_2_2_fast_read : 1;
        uint32_t supports_1_4_4_fast_read : 1;
        uint32_t support_1_1_4_fast_read : 1;
        uint32_t unused1 : 9;
    } misc;

    /* 2nd DWORD */
    uint32_t flash_density;

    /* 3rd DWORD */
    struct
    {
        uint32_t dummy_clocks_1_4_4_read : 5;
        uint32_t mode_clocks_1_4_4_read : 3;
        uint32_t inst_1_4_4_read : 8;
        uint32_t dummy_clocks_1_1_4_read : 5;
        uint32_t mode_clocks_1_1_4_read : 3;
        uint32_t inst_1_1_4_read : 8;
    } read_1_4_info;

    /* 4th DWORD */
    struct
    {
        uint32_t dummy_clocks_1_1_2_read : 5;
        uint32_t mode_clocks_1_1_2_read : 3;
        uint32_t inst_1_1_2_read : 8;
        uint32_t dummy_clocks_1_2_2_read : 5;
        uint32_t mode_clocks_1_2_2_read : 3;
        uint32_t inst_1_2_2_read : 8;
    } read_1_2_info;

    /* 5th DWORD */
    struct
    {
        uint32_t support_2_2_2_fast_read : 1;
        uint32_t reserved0 : 3;
        uint32_t support_4_4_4_fast_read : 1;
        uint32_t reserved1 : 27;
    } read_22_44_check;

    /* 6th DWORD */
    struct
    {
        uint32_t reserved0 : 16;
        uint32_t dummy_clocks_2_2_2_read : 5;
        uint32_t mode_clocks_2_2_2_read : 3;
        uint32_t inst_2_2_2_read : 8;
    } read_2_2_info;

    /* 7th DWORD */
    struct
    {
        uint32_t reserved0 : 16;
        uint32_t dummy_clocks_4_4_4_read : 5;
        uint32_t mode_clocks_4_4_4_read : 3;
        uint32_t inst_4_4_4_read : 8;
    } read_4_4_info;

    /* 8st and 9th DWORD */
    struct
    {
        uint8_t size;
        uint8_t inst;
    } erase_info[4];

    /* 10th DWORD */
    struct
    {
        uint32_t multiplier_from_typical_to_max : 4;
        uint32_t type_1_count : 5;
        uint32_t type_1_units : 2;
        uint32_t type_2_count : 5;
        uint32_t type_2_units : 2;
        uint32_t type_3_count : 5;
        uint32_t type_3_units : 2;
        uint32_t type_4_count : 5;
        uint32_t type_4_units : 2;
    } erase_timing;

    /* 11th DWORD */
    struct
    {
        uint32_t program_max_time : 4;
        uint32_t page_size : 4;
        uint32_t page_program_time_count : 5;
        uint32_t page_program_time_units : 1;
        uint32_t byte_program_time_first_count : 4;
        uint32_t byte_program_time_first_units : 1;
        uint32_t byte_program_time_additional_count : 4;
        uint32_t byte_program_time_additional_units : 1;
        uint32_t chip_erase_time_count : 5;
        uint32_t chip_erase_time_units : 2;
        uint32_t reserved0 : 1;
    } chip_erase_progrm_info;

    /* 12th DWORD */
    struct
    {
        uint32_t prohibited_oprations_during_program_suspend : 4;
        uint32_t prohibited_oprations_during_erase_suspend : 4;
        uint32_t reserved0 : 1;
        uint32_t program_resume_2_suspend_interval : 4;
        uint32_t suspend_ipp_max_latency_count : 5;
        uint32_t suspend_ipp_max_latency_units : 2;
        uint32_t erase_resume_2_suspend_interval_count : 4;
        uint32_t suspend_ipe_max_latency_count : 5;
        uint32_t suspend_ipe_max_latency_units : 2;
        uint32_t support_suspend_resume : 1;
    } suspend_resume_info;

    /* 13th DWORD */
    struct
    {
        uint32_t program_resume_inst : 8;
        uint32_t program_suspend_inst : 8;
        uint32_t resume_inst : 8;
        uint32_t suspend_inst : 8;
    } suspend_resume_inst;

    /* 14th DWORD */
    struct
    {
        uint32_t reserved0 : 2;
        uint32_t busy_status_polling : 6;
        uint32_t delay_from_deep_powerdown_to_next_operation_count : 5;
        uint32_t delay_from_deep_powerdown_to_next_operation_units : 2;
        uint32_t exit_deep_powerdown_inst : 8;
        uint32_t enter_deep_powerdown_inst : 8;
        uint32_t support_deep_powerdown : 1;
    } deep_power_down_info;

    /* 15th DWORD */
    struct
    {
        uint32_t mode_4_4_4_disable_seq : 4;
        uint32_t mode_4_4_4_enable_seq : 5;
        uint32_t support_mode_0_4_4 : 1;
        uint32_t mode_0_4_4_exit_method : 6;
        uint32_t mode_0_4_4_entry_method : 4;
        uint32_t quad_enable_requirement : 3;
        uint32_t hold_reset_disable : 1;
        uint32_t reserved0 : 8;
    } mode_4_4_info;

    /* 16th DWORD */
    struct
    {
        uint32_t status_reg_write_enable : 7;
        uint32_t reserved0 : 1;
        uint32_t soft_reset_rescue_support : 6;
        uint32_t exit_4_byte_addressing : 10;
        uint32_t enter_4_byte_addrssing : 8;
    } mode_config_info;

    /* 17th DWORD */
    struct
    {
        uint32_t dummy_clocks_1_8_8_read : 5;
        uint32_t mode_clocks_1_8_8_read : 3;
        uint32_t inst_1_8_8_read : 8;
        uint32_t dummy_clocks_1_1_8_read : 5;
        uint32_t mode_clocks_1_1_8_read : 3;
        uint32_t inst_1_1_8_read : 8;
    } read_1_8_info;

    /* 18th DWORD */
    /* 19th DWORD */
    /* 20th DWORD */
} jedec_basic_flash_param_table_t;

/*! @brief 4Byte Addressing Instruction Table. */
typedef struct _jedec_4byte_addressing_inst_table
{
    /* 1st DWORD */
    struct
    {
        uint32_t support_1_1_1_read : 1;
        uint32_t support_1_1_1_fast_read : 1;
        uint32_t support_1_1_2_fast_read : 1;
        uint32_t support_1_2_2_fast_read : 1;
        uint32_t support_1_1_4_fast_read : 1;
        uint32_t support_1_4_4_fast_read : 1;
        uint32_t support_1_1_1_page_program : 1;
        uint32_t support_1_1_4_page_program : 1;
        uint32_t support_1_4_4_page_program : 1;
        uint32_t support_erase_type1_size : 1;
        uint32_t support_erase_type2_size : 1;
        uint32_t support_erase_type3_size : 1;
        uint32_t support_erase_type4_size : 1;
        uint32_t support_1_1_1_dtr_read : 1;
        uint32_t support_1_2_2_dtr_read : 1;
        uint32_t support_1_4_4_dtr_read : 1;
        uint32_t support_volatile_sector_lock_read_cmd : 1;
        uint32_t support_volatile_sector_lock_write_cmd : 1;
        uint32_t support_nonvolatile_sector_lock_read_cmd : 1;
        uint32_t support_nonvolatile_sector_lock_write_cmd : 1;
        uint32_t support_1_1_8_fast_read : 1;
        uint32_t support_1_8_8_fast_read : 1;
        uint32_t support_1_8_8_dtr_read : 1;
        uint32_t support_1_1_8_page_program : 1;
        uint32_t support_1_8_8_page_program : 1;
        uint32_t reserved : 7;
    } cmd_4byte_support_info;

    /* 2nd DWORD */
    struct
    {
        uint8_t erase_inst[4];
    } erase_inst_info;
} jedec_4byte_addressing_inst_table_t;

typedef struct _jdec_query_table
{
    uint32_t standard_version;
    uint32_t basic_flash_param_tbl_size;
    jedec_basic_flash_param_table_t basic_flash_param_tbl;
    bool has_4b_addressing_inst_table;
    jedec_4byte_addressing_inst_table_t flash_4b_inst_tbl;
} jedec_info_table_t;

/*******************************************************************************
* Prototypes
******************************************************************************/
static status_t FLEXSPI_NOR_PrepareQuadModeEnableSequence(nor_handle_t *handle,
                                                  flexspi_mem_config_t *config,
                                                  jedec_info_table_t *tbl);
static status_t FLEXSPI_NOR_EnableQuadMode(nor_handle_t *handle, flexspi_mem_config_t *config);

static status_t FLEXSPI_NOR_GenerateConfigBlockUsingSFDP(nor_handle_t *handle, flexspi_mem_config_t *config);

static status_t FLEXSPI_NOR_WaitBusBusy(FLEXSPI_Type *base, flexspi_mem_nor_handle_t *handle, uint32_t baseAddr);

static status_t FLEXSPI_NOR_WriteEnable(FLEXSPI_Type *base, flexspi_port_t port, uint32_t baseAddr);

static status_t FLEXSPI_NOR_ParseSFDP(nor_handle_t *handle, flexspi_mem_config_t *config, jedec_info_table_t *tbl);

static status_t FLEXSPI_NOR_GetPageSectorBlockSizeFromSFDP(nor_handle_t *handle,
                                                     jedec_info_table_t *tbl,
                                                     uint32_t *sector_erase_cmd);

status_t FLEXSPI_NOR_ExitNoCMDMode(nor_handle_t *handle, flexspi_mem_config_t *config, uint32_t baseAddr);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
status_t FLEXSPI_NOR_WaitBusBusy(FLEXSPI_Type *base, flexspi_mem_nor_handle_t *handle, uint32_t baseAddr)
{
    /* Wait status ready. */
    bool isBusy = true;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = baseAddr;
    flashXfer.port = handle->port;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUS;
    flashXfer.data = &readValue;
    flashXfer.dataSize = 1;

    do
    {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }

        if (handle->busyBitPolarity)
        {
            isBusy = (~readValue) & (1 << handle->busyOffset);
        }
        else
        {
            isBusy = readValue & (1 << handle->busyOffset);
        }

    } while (isBusy);

    return status;
}

status_t FLEXSPI_NOR_WriteEnable(FLEXSPI_Type *base, flexspi_port_t port, uint32_t baseAddr)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write enable */
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port = port;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

static status_t FLEXSPI_NOR_ReadSFDP(FLEXSPI_Type *base, uint32_t address, uint32_t *buffer, uint32_t bytes)
{
    flexspi_transfer_t flashXfer;
    status_t status;
    memset(&flashXfer, 0, sizeof(flashXfer));
    flashXfer.deviceAddress = address;
    flashXfer.port = flexspiMemHandle.port;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READ_SFDP;
    flashXfer.data = buffer;
    flashXfer.dataSize = bytes;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

/*
 * SFDP Overall Header Structure
 * ---------------------
 * SFDP Header, always located at SFDP offset 0x0. Note that the SFDP space is
 * always read in 3 Byte addressing mode, where the expected SFDP address space
 * layout looks like the following:
 *
 *  ------------------0x00
 *  | SFDP Header        |  (specifying X number of Parameter Headers)
 *  ------------------0x08
 *  | Parameter Header 1 |  (specifying Y Parameter Table Pointer & Length L)
 *  ------------------0x10
 *          - - -
 *  --------------X * 0x08
 *  | Parameter Header X |  (specifying Z Parameter Table Pointer & Length K)
 *  --------(X + 1) * 0x08
 *          - - -
 *  ---------------------Y
 *  | Parameter Table 1  |
 *  -------------------Y+L
 *         - - -
 *  ---------------------Z            Key:    ------start_sfdp_offset
 *  | Parameter Table X  |                    | Region Name         |
 *  -------------------Z+K                    ------limit_sfdp_offset
 */
static status_t FLEXSPI_NOR_ReadSFDPInfo(FLEXSPI_Type *base, jedec_info_table_t *tbl)
{
    status_t status = kStatus_Fail;
    do
    {
        if (tbl == NULL)
        {
            status = kStatus_InvalidArgument;
            break;
        }

        memset(tbl, 0, sizeof(*tbl));

        /*
         * Read SFDP header
         * ---------------------
         * The SFDP Header is located at address 000000h of the SFDP data structure. It identifies the SFDP
         * Signature, the number of parameter headers, and the SFDP revision numbers.
         */
        sfdp_header_t sfdp_header;
        uint32_t address = 0;

        status = FLEXSPI_NOR_ReadSFDP(base, 0, (uint32_t *)&sfdp_header, sizeof(sfdp_header));
        if (status != kStatus_Success)
        {
            break;
        }

        if (sfdp_header.signature != SFDP_JESD216_SIGNATURE || sfdp_header.major_rev != SFDP_JESD216_MAJOR)
        {
            break;
        }

        tbl->standard_version = sfdp_header.minor_rev;

        /* This number is 0-based. Therefore, 0 indicates 1 parameter header. */
        uint32_t parameter_header_number = sfdp_header.param_hdr_num + 1;

        /*
         * Read parameter headers
         * ---------------------
         * The first parameter header is mandatory, is defined by JEDEC standard, and starts at byte offset 08h.
         */
        sfdp_parameter_header_t sfdp_param_hdrs[12];
        uint32_t max_hdr_count = parameter_header_number > 10 ? 10 : parameter_header_number;
        address = 0x08;

        status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&sfdp_param_hdrs[0],
                                       max_hdr_count * sizeof(sfdp_parameter_header_t));
        if (status != kStatus_Success)
        {
            break;
        }

        for (uint32_t i = 0; i < max_hdr_count; i++)
        {
            uint32_t parameter_id = SFDP_PARAM_HEADER_ID(sfdp_param_hdrs[i]);

            if ((parameter_id == kParameterID_BasicSpiProtocal) ||
                (parameter_id == kParameterID_4ByteAddressInstructionTable))
            {
                address = SFDP_PARAM_HEADER_PTP(sfdp_param_hdrs[i]);

                uint32_t table_size = sfdp_param_hdrs[i].table_length_in_32bit * sizeof(uint32_t);

                if (parameter_id == kParameterID_BasicSpiProtocal)
                {
                    /* Limit table size to the max supported JEDEC216C standard */
                    if (table_size > sizeof(jedec_basic_flash_param_table_t))
                    {
                        table_size = sizeof(jedec_basic_flash_param_table_t);
                    }

                    /* Read Basic SPI Protocol Table */
                    status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->basic_flash_param_tbl, table_size);

                    if (status != kStatus_Success)
                    {
                        break;
                    }

                    tbl->basic_flash_param_tbl_size = table_size;
                }
                else if (parameter_id == kParameterID_4ByteAddressInstructionTable)
                {
                    /* Read 4-byte Address Instruction Table */
                    status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->flash_4b_inst_tbl, table_size);
                    if (status != kStatus_Success)
                    {
                        break;
                    }
                    tbl->has_4b_addressing_inst_table = true;
                }
            }
            else
            {
                /* Unsupported parameter type, ignore */
            }
        }

    } while (0);

    return status;
}

status_t FLEXSPI_NOR_GetPageSectorBlockSizeFromSFDP(nor_handle_t *handle, jedec_info_table_t *tbl, uint32_t *sector_erase_cmd)
{
    jedec_basic_flash_param_table_t *param_tbl = &tbl->basic_flash_param_tbl;
    jedec_4byte_addressing_inst_table_t *flash_4b_tbl = &tbl->flash_4b_inst_tbl;

    uint32_t flash_size;
    uint32_t flash_density = tbl->basic_flash_param_tbl.flash_density;

    if (flash_density & (1U << 0x1F))
    {
        /* Flash size >= 4G bits */
        flash_size = 1U << (flash_density & ~(1U << 0x1F));
    }
    else
    {
        /* Flash size < 4G bits */
        flash_size = flash_density + 1;
    }

    /* Convert to bytes. */
    flash_size >>= 3;

    handle->bytesInMemorySize = flash_size;

    /* Calculate Page size */
    uint32_t page_size;
    if (tbl->basic_flash_param_tbl_size < 64U)
    {
        handle->bytesInPageSize = 256U;
    }
    else
    {
        page_size = 1 << (param_tbl->chip_erase_progrm_info.page_size);
        handle->bytesInPageSize = page_size == (1 << 15) ? 256U : page_size;
    }

    /* Calculate Sector Size */
    uint32_t sector_size = 0xFFFF;
    uint32_t sector_erase_type = 0;

    for (uint32_t index = 0; index < 4; index++)
    {
        if (param_tbl->erase_info[index].size != 0)
        {
            uint32_t current_erase_size = 1U << param_tbl->erase_info[index].size;
            /* Calculate min non-zero sector size */
            if ((current_erase_size < sector_size) && (current_erase_size < (1024U * 1024U)))
            {
                sector_size = current_erase_size;
                sector_erase_type = index;
            }
        }
    }

    handle->bytesInSectorSize = sector_size;

    /* Calculate erase instruction operand for LUT */
    if ((flexspiMemHandle.configuredFlashSize > MAX_24BIT_ADDRESSING_SIZE) && (tbl->has_4b_addressing_inst_table))
    {
        *sector_erase_cmd = flash_4b_tbl->erase_inst_info.erase_inst[sector_erase_type];
    }
    else
    {
        *sector_erase_cmd = param_tbl->erase_info[sector_erase_type].inst;
    }

    return kStatus_Success;
}

static status_t FLEXSPI_NOR_ParseSFDP(nor_handle_t *handle, flexspi_mem_config_t *config, jedec_info_table_t *tbl)
{
    status_t status = kStatus_InvalidArgument;
    do
    {
        jedec_basic_flash_param_table_t *param_tbl = &tbl->basic_flash_param_tbl;
        jedec_4byte_addressing_inst_table_t *flash_4b_tbl = &tbl->flash_4b_inst_tbl;

        /* Check whether DDR mode is supported. */
        bool support_ddr_mode = false;
        if (config->deviceType == kSerialNorCfgOption_DeviceType_ReadSFDP_DDR)
        {
            support_ddr_mode = param_tbl->misc.support_dtr_clocking;
            if (!support_ddr_mode)
            {
                break;
            }
        }

        uint8_t read_cmd      = 0;
        uint32_t dummy_cycles = 0;
        uint8_t mode_cycles   = 0;
        uint32_t erase_cmd    = 0;
        uint32_t address_bits = 24;
        uint32_t address_pads = kFLEXSPI_1PAD;

        /* Get page and sector information from SFDP. */
        FLEXSPI_NOR_GetPageSectorBlockSizeFromSFDP(handle, tbl, &erase_cmd);

        if ((flexspiMemHandle.configuredFlashSize > MAX_24BIT_ADDRESSING_SIZE) && (tbl->has_4b_addressing_inst_table))
        {
            address_bits = 32;
        }

        /* Set NOR flash command sequence according to SFDP*/
        uint32_t cmd_pads = config->commandPads;

        if (cmd_pads == kFLEXSPI_1PAD)
        {
            /* Prepare Quad Mode enable sequence as needed. */
            status = FLEXSPI_NOR_PrepareQuadModeEnableSequence(handle, config, tbl);

            if (status != kStatus_Success)
            {
                break;
            }

            /* Determine Read command based on SFDP. */
            if (param_tbl->misc.supports_1_4_4_fast_read)
            {
                address_pads = kFLEXSPI_4PAD;
                mode_cycles = param_tbl->read_1_4_info.mode_clocks_1_4_4_read;
                dummy_cycles = param_tbl->read_1_4_info.dummy_clocks_1_4_4_read;
            }
            else if (param_tbl->misc.support_1_1_4_fast_read)
            {
                mode_cycles = param_tbl->read_1_4_info.mode_clocks_1_1_4_read;
                dummy_cycles = param_tbl->read_1_4_info.dummy_clocks_1_1_4_read;
            }

            /* Set page program instruction for LUT*/
            if (address_bits == 32)
            {
                if (flash_4b_tbl->cmd_4byte_support_info.support_1_4_4_page_program)
                {
                    /*
                     * Page Program command for 1-4-4 mode based on 4 bytes address
                     * --------------------------------------------------------------
                     * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                     * ==============================================================
                     * 0       | CMD_SDR     | 0x0 (Single)  | 0x3E
                     * --------------------------------------------------------------
                     * 1       | ADDR_SDR    | 0x2 (Quad)    | 0x20 (4 bytes address)
                     * --------------------------------------------------------------
                     * 2       | WRITE_SDR   | 0x2 (Quad)    | Any non-zero value
                     * --------------------------------------------------------------
                     * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                     * -------------------------------------------------------------- */
                    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_4_4_4B,
                                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32);
                    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
                }
                else if (flash_4b_tbl->cmd_4byte_support_info.support_1_1_4_page_program)
                {
                    /*
                     * Page Program command for 1-1-4 mode based on 4 bytes address
                     * --------------------------------------------------------------
                     * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                     * ==============================================================
                     * 0       | CMD_SDR     | 0x0 (Single)  | 0x01
                     * --------------------------------------------------------------
                     * 1       | ADDR_SDR    | 0x0 (Single)  | 0x20 (4 bytes address)
                     * --------------------------------------------------------------
                     * 2       | WRITE_SDR   | 0x2 (Quad)    | Any non-zero value
                     * --------------------------------------------------------------
                     * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                     * -------------------------------------------------------------- */
                    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_1_4_4B,
                                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32);
                    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
                }
                else
                {
                    /*
                     * Page Program command for 1-1-1 mode based on 4 bytes address
                     * --------------------------------------------------------------
                     * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                     * ==============================================================
                     * 0       | CMD_SDR     | 0x0 (Single)  | 0x12
                     * --------------------------------------------------------------
                     * 1       | ADDR_SDR    | 0x0 (Single)  | 0x20 (4 bytes address)
                     * --------------------------------------------------------------
                     * 2       | WRITE_SDR   | 0x0 (Single)  | Any non-zero value
                     * --------------------------------------------------------------
                     * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                     * -------------------------------------------------------------- */
                    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_1_1_4B,
                                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32);
                    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
                }
            }
            else
            {
                /*
                 * Page Program command for 1-1-1 mode based on 3 bytes address
                 * --------------------------------------------------------------
                 * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                 * ==============================================================
                 * 0       | CMD_SDR     | 0x0 (Single)  | 0x12
                 * --------------------------------------------------------------
                 * 1       | ADDR_SDR    | 0x0 (Single)  | 0x18 (3 bytes address)
                 * --------------------------------------------------------------
                 * 2       | WRITE_SDR   | 0x0 (Single)  | Any non-zero value
                 * --------------------------------------------------------------
                 * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                 * -------------------------------------------------------------- */
                config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_1_1_3B,
                                    kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24);
                config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
            }
        }
        else
        {
            break;
        }

         /* Set write enable command instruction for LUT. */
        config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] = FLEXSPI_LUT_SEQ(
            kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteEnable, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);

        /* Set read status command instruction for LUT. */
        config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_READSTATUS] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg1,
                            kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04);

        /* Set sector erase command instruction for LUT. */
        config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] = FLEXSPI_LUT_SEQ(
            kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, erase_cmd, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, address_bits);

        /* Set chip erase command instruction for LUT. */
        config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_CHIPERASE] = FLEXSPI_LUT_SEQ(
            kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ChipErase, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);

        /* Set read commmand instruction oprand for LUT. */
        if (address_bits == 24)
        {
            if (support_ddr_mode)
            {
                read_cmd = kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_3B;
            }
            else if (param_tbl->misc.supports_1_4_4_fast_read)
            {
                read_cmd = param_tbl->read_1_4_info.inst_1_4_4_read;
            }
            else if (param_tbl->misc.support_1_1_4_fast_read)
            {
                read_cmd = param_tbl->read_1_4_info.inst_1_1_4_read;
            }
            else
            {
                read_cmd = kSerialNorCmd_BasicRead_3B;
                dummy_cycles = 0;
                mode_cycles = 0;
            }
        }
        else
        {
            address_pads = kFLEXSPI_4PAD;
            if (support_ddr_mode)
            {
                read_cmd = kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_4B;
            }
            else if (flash_4b_tbl->cmd_4byte_support_info.support_1_4_4_fast_read)
            {
                read_cmd = kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_4_4_4B;
            }
            else if (flash_4b_tbl->cmd_4byte_support_info.support_1_1_4_fast_read)
            {
                read_cmd = kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_1_4_4B;
                address_pads = kFLEXSPI_1PAD;
            }
            else
            {
                read_cmd = kSerialNorCmd_BasicRead_4B;
                dummy_cycles = 0;
                mode_cycles = 0;
            }
        }

        /* Set read command instruction for LUT. */
        if (support_ddr_mode)
        {
            config->controllerMiscOption |= (1 << kFlexSpiMiscOffset_DdrModeEnable);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, read_cmd,
                                                     kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, address_bits);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0 + 1] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_MODE8_DDR, kFLEXSPI_4PAD,
                                                     mode_cycles, kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_4PAD, dummy_cycles);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0 + 2] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_DDR, kFLEXSPI_4PAD,
                                                     0x04, 0, 0, 0);
        }
        else
        {
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, read_cmd,
                                                     kFLEXSPI_Command_RADDR_SDR, address_pads, address_bits);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0 + 1] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_MODE8_SDR, kFLEXSPI_4PAD,
                                                     mode_cycles, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, dummy_cycles);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0 + 2] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, address_pads,
                                                     0x04, 0, 0, 0);
        }

        if (support_ddr_mode)
        {
            config->halfClkForNonReadCmd = true;
            config->controllerMiscOption |= (1 << kFlexSpiMiscOffset_DdrModeEnable);
        }

        status = kStatus_Success;
    } while (0);

    return status;
}

status_t FLEXSPI_NOR_PrepareQuadModeEnableSequence(nor_handle_t *handle, flexspi_mem_config_t *config, jedec_info_table_t *tbl)
{
    status_t status = kStatus_InvalidArgument;

    /* See JESD216B/C/D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    FLEXSPI_Type *base = (FLEXSPI_Type *)handle->driverBaseAddr;

    do
    {
        uint32_t enter_quad_mode_option = kSerialNorQuadMode_NotConfig;
        uint32_t lut_seq[4];
        memset(lut_seq, 0, sizeof(lut_seq));

        if (tbl->basic_flash_param_tbl_size >= 64U)
        {
            config->deviceModeCfgEnable = true;

            /* Set Quad mode read instruction for LUT. */
            switch (tbl->basic_flash_param_tbl.mode_4_4_info.quad_enable_requirement)
            {
                case 1:
                case 4:
                case 5:
                case 6:
                    enter_quad_mode_option = kSerialNorQuadMode_StatusReg2_Bit1;
                    lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg2Bit1, kFLEXSPI_Command_READ_SDR,
                                                 kFLEXSPI_1PAD, 1);
                    break;
                case 2:
                    enter_quad_mode_option = kSerialNorQuadMode_StatusReg1_Bit6;
                    lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg1, kFLEXSPI_Command_READ_SDR,
                                                 kFLEXSPI_1PAD, 1);
                    break;
                case 3:
                    enter_quad_mode_option = kSerialNorQuadMode_StatusReg2_Bit7;
                    lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg2Bit7, kFLEXSPI_Command_READ_SDR,
                                                 kFLEXSPI_1PAD, 1);
                    break;
                default:
                    enter_quad_mode_option = kSerialNorQuadMode_NotConfig;
                    config->deviceModeCfgEnable = false;
                    break;
            }
        }
        else
        {
            enter_quad_mode_option = config->quadMode;
        }

        if (enter_quad_mode_option != kSerialNorQuadMode_NotConfig)
        {
            flexspi_transfer_t flashXfer;
            uint32_t status_val = 0;
            flashXfer.deviceAddress = 0;
            flashXfer.port = config->devicePort;
            flashXfer.cmdType = kFLEXSPI_Read;
            flashXfer.data = &status_val;
            flashXfer.dataSize = 1;
            flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUS;
            flashXfer.SeqNumber = 1;

            FLEXSPI_UpdateLUT(base, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4UL, lut_seq, sizeof(lut_seq)/sizeof(lut_seq[0]));

            /* Read status based on new LUT for Quad mode. */
            status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
            if (status != kStatus_Success)
            {
                break;
            }
            /* Enable Quad mode if status register corresponding QE bit is 0. */
            switch (enter_quad_mode_option)
            {
                case kSerialNorQuadMode_StatusReg1_Bit6:
                    if(!(status_val & (1UL << 0x06)))
                    {
                      FLEXSPI_NOR_EnableQuadMode(handle, config);
                    };
                    break;
                case kSerialNorQuadMode_StatusReg2_Bit1:
                    if(!((status_val <<= 8) & (1UL << 0x01)))
                    {
                      FLEXSPI_NOR_EnableQuadMode(handle, config);
                    };
                    break;
                case kSerialNorQuadMode_StatusReg2_Bit7:
                    if(!(status_val & (1UL << 0x07)))
                    {
                      FLEXSPI_NOR_EnableQuadMode(handle, config);
                    };
                    break;
                default:
                    config->deviceModeCfgEnable = false;
                    break;
            }

            /* Override status value if it is required by user. */
            if (config->statusOverride)
            {
                status_val = config->statusOverride;
            }

            /* Do modify-afer-read status and then create Quad mode Enable sequence. */
            config->deviceModeCfgEnable = true;

            switch (enter_quad_mode_option)
            {
                case kSerialNorQuadMode_StatusReg1_Bit6:
                    /* QE is set via Write Status with one data byte where bit 6 is one. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteStatusReg1,
                                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x01);
                    status_val |= (1U << 0x06);
                    break;
                case kSerialNorQuadMode_StatusReg2_Bit1:
                    /* QE is set via Write Status instruction 01h with two data bytes where bit 1 of the second byte is one. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteStatusReg2Bit1,
                                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x02);
                    status_val |= (1U << 1);
                    status_val <<= 8;
                    break;
                case kSerialNorQuadMode_StatusReg2_Bit7:
                    /*QE is set via Write status register 2 instruction 3Eh with one data byte where bit 7 is one. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteStatusReg2Bit7,
                                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x01);
                    status_val |= (1U << 0x07);
                    break;
                default:
                    config->deviceModeCfgEnable = false;
                    break;
            }

            /* Update LUT table. */
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4UL, &config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUS], 4);

            config->statusOverride = status_val;
        }

        status = kStatus_Success;
    } while (0);

    return status;
}

static status_t FLEXSPI_NOR_EnableQuadMode(nor_handle_t *handle, flexspi_mem_config_t *config)
{
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;

    flexspi_port_t port = memHandle->port;

    status_t status;
    uint32_t writeValue = config->statusOverride;

    /* Set write enable command instruction for LUT. */
    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteEnable, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);

    /* Update LUT table. */
    FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITEENABLE * 4UL, &config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 4);

    /* Write enable. */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Enable quad mode. */
    flashXfer.deviceAddress = 0;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUS;
    flashXfer.data          = &writeValue;
    flashXfer.dataSize      = 1;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);

    return status;
}

status_t FLEXSPI_NOR_GenerateConfigBlockUsingSFDP(nor_handle_t *handle, flexspi_mem_config_t *config)
{
    status_t status = kStatus_InvalidArgument;
    
    const lut_seq_t k_sdfp_lut[4] = {
          /* Read SFDP LUT sequence for 1 pad instruction. */
          {{FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialFlash_ReadSFDP, kFLEXSPI_Command_RADDR_SDR,
                         kFLEXSPI_1PAD, 24),
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0xFF),
          0, 0}},
          /* Read SFDP LUT sequence for 2 pad instruction. */
          {{FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_2PAD, kSerialFlash_ReadSFDP, kFLEXSPI_Command_RADDR_SDR,
                         kFLEXSPI_2PAD, 24),
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_2PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_2PAD, 0xFF),
          0, 0}},
          /* Read SFDP LUT sequence for 4 pad instruction. */
          {{FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, kSerialFlash_ReadSFDP, kFLEXSPI_Command_RADDR_SDR,
                         kFLEXSPI_4PAD, 24),
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0xFF),
          0, 0}},
          };

    do
    {
        if (config->queryPads != kFLEXSPI_1PAD)
        {
            break;
        }

        if (config->enhanceMode == kSerialNorEnhanceMode_0_4_4_Mode)
        {
            /* Try to exit 0-4-4 mode. */
            config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF, kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF);
            config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD + 1] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF, kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF);
            config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD + 2] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);

            status = FLEXSPI_NOR_ExitNoCMDMode(handle, config, 0);
            if (status != kStatus_Success)
            {
                break;
            }
        }

        /* Read SFDP, probe whether the Flash device is present or not. */
        jedec_info_table_t jedec_info_tbl;

        memset(&jedec_info_tbl, 0x00, sizeof(jedec_info_tbl));
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READ_SFDP * 4UL,
                          (const uint32_t *)&k_sdfp_lut[config->queryPads], sizeof(k_sdfp_lut)/sizeof(k_sdfp_lut[0]));


        status = FLEXSPI_NOR_ReadSFDPInfo((FLEXSPI_Type *)handle->driverBaseAddr, &jedec_info_tbl);
        if (status != kStatus_Success)
        {
            break;
        }
        status = FLEXSPI_NOR_ParseSFDP(handle, config, &jedec_info_tbl);

    } while (0);

    return status;
}

status_t Nor_Flash_Erase_Chip(nor_handle_t *handle)
{
    assert(handle);
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port = memHandle->port;
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Send erase command. */
    flashXfer.deviceAddress = 0;
    flashXfer.port = port;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_CHIPERASE;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Erase_Block(nor_handle_t *handle, uint32_t address, uint32_t size_Byte)
{
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port = memHandle->port;
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Send erase command. */
    flashXfer.deviceAddress = address;
    flashXfer.port = port;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    flashXfer.dataSize = size_Byte;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Page_Program(nor_handle_t *handle, uint32_t address, uint8_t *buffer)
{
    status_t status;
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port = memHandle->port;
    uint32_t pageSize = handle->bytesInPageSize;
    /* Write enable. */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Send page load command. */
    flashXfer.deviceAddress = address;
    flashXfer.port = port;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM;
    flashXfer.data = (uint32_t *)buffer;
    flashXfer.dataSize = pageSize;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Read(nor_handle_t *handle, uint32_t address, uint8_t *buffer, uint32_t length)
{
    status_t status = kStatus_Success;

    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port = memHandle->port;

    /* Read page. */
    flashXfer.deviceAddress = address;
    flashXfer.port = port;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READ;
    flashXfer.data = (uint32_t *)buffer;
    flashXfer.dataSize = length;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }
    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Init(nor_config_t *config, nor_handle_t *handle)
{
    assert(config);
    assert(handle);

    status_t status = kStatus_Success;
    flexspi_mem_config_t *memConfig = (flexspi_mem_config_t *)config->memControlConfig;
    handle->driverBaseAddr = config->driverBaseAddr;
    handle->deviceSpecific = &flexspiMemHandle;
    flexspiMemHandle.configuredFlashSize = memConfig->deviceConfig.flashSize;
    flexspiMemHandle.port = memConfig->devicePort;

    memset(memConfig->lookupTable, 0, sizeof(memConfig->lookupTable));

    /* Configure flash settings according to serial flash feature. */
    FLEXSPI_SetFlashConfig((FLEXSPI_Type *)handle->driverBaseAddr, &(memConfig->deviceConfig), memConfig->devicePort);

    status = FLEXSPI_NOR_GenerateConfigBlockUsingSFDP(handle, memConfig);

    /* Update LUT table. */
    FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, 0, memConfig->lookupTable, sizeof(memConfig->lookupTable)/sizeof(memConfig->lookupTable[0]));

    /* Do software reset. */
    FLEXSPI_SoftwareReset((FLEXSPI_Type *)handle->driverBaseAddr);

    if (status != kStatus_Success)
    {
        return status;
    }

    return kStatus_Success;
}

status_t FLEXSPI_NOR_ExitNoCMDMode(nor_handle_t *handle, flexspi_mem_config_t *config, uint32_t baseAddr)
{
    flexspi_transfer_t flashXfer;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD;
    flashXfer.SeqNumber = 1;
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port = flexspiMemHandle.port;
    FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD * 4,
                      &config->lookupTable[4 * flashXfer.seqIndex], 4);
    return FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
}
