/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexspi.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define NOR_CMD_INDEX_READ CMD_INDEX_READ               /*!< 0*/
#define NOR_CMD_INDEX_READSTATUS CMD_INDEX_READSTATUS   /*!< 1*/
#define NOR_CMD_INDEX_WRITEENABLE CMD_INDEX_WRITEENABLE /*!< 2*/
#define NOR_CMD_INDEX_ERASESECTOR 3                     /*!< 3*/
#define NOR_CMD_INDEX_PAGEPROGRAM CMD_INDEX_WRITE       /*!< 4*/
#define NOR_CMD_INDEX_CHIPERASE 5                       /*!< 5*/
#define NOR_CMD_INDEX_DUMMY 6                           /*!< 6*/


#define NOR_CMD_LUT_SEQ_IDX_READSTATUS_XPI   2 /*!< 2  Read status DPI/QPI/OPI sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE_XPI  4 /*!< 4  Write Enable DPI/QPI/OPI sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_RESTORE_NOCMD    14 /*!< 14 Restore 0-4-4/0-8-8 mode sequence id in lookupTable stored in config block*/


#define NOR_CMD_LUT_SEQ_IDX_READ        0  /*!< Read LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS  1  /*!< Read Status LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUS 2  /*!< Write Status LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 3  /*!< Write enable LUT sequence id in lookupTable stored in config block*/

#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 5  /*!< Erase sector LUT sequence id in lookupTable stored in config block*/

#define NOR_CMD_LUT_SEQ_IDX_READID      8  /*!< Read manifacture ID LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM 9  /*!< Page program LUT sequence id in lookupTable stored in config block*/

#define NOR_CMD_LUT_SEQ_IDX_CHIPERASE   11 /*!< Chip erase LUT sequence id in lookupTable stored in config block*/


#define NOR_CMD_LUT_SEQ_IDX_READ_SFDP   13 /*!< Read SFDP information sequence id in lookupTable id stored in config block*/

#define NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD  15 /*!< Exit 0-4-4/0-8-8 mode sequence id in lookupTable stored in config blobk*/

enum _flexspi_nor_status
{
    kStatus_FLEXSPINOR_ProgramFail = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 0), /*!< Status for Page programming failure*/
    kStatus_FLEXSPINOR_EraseSectorFail = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 1), /*!< Status for Sector Erase failure*/
    kStatus_FLEXSPINOR_EraseAllFail = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 2),    /*!< Status for Chip Erase failure*/
    kStatus_FLEXSPINOR_WaitTimeout = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 3),     /*!< Status for timeout*/
    kStatus_FlexSPINOR_NotSupported = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 4),    /*!< Not support*/
    kStatus_FlexSPINOR_WriteAlignmentError = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 5), /*!< Status for Alignement error*/
    kStatus_FlexSPINOR_CommandFailure =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 6), /*!< Status for Erase/Program Verify Error*/
    kStatus_FlexSPINOR_SFDP_NotFound = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 7), /*!< Status for SFDP read failure*/
    kStatus_FLEXSPINOR_Unsupported_SFDP_Version =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 8), /*!< Status for Unrecognized SFDP version*/
    kStatus_FLEXSPINOR_Flash_NotFound =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 9), /*!< Status for Flash detection failure*/
    kStatus_FLEXSPINOR_DTRRead_DummyProbeFailed =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 10), /*!< Status for DDR Read dummy probe failure*/
};

typedef enum _serial_nor_device_type
{
    kSerialNorCfgOption_Tag = 0x0E,
    kSerialNorCfgOption_DeviceType_ReadSFDP_SDR = 0,
    kSerialNorCfgOption_DeviceType_ReadSFDP_DDR = 1,
    kSerialNorCfgOption_DeviceType_HyperFLASH1V8 = 2,
    kSerialNorCfgOption_DeviceType_HyperFLASH3V0 = 3,
    kSerialNorCfgOption_DeviceType_MacronixOctalDDR = 4,
    kSerialNorCfgOption_DeviceType_MacronixOctalSDR = 5,
    kSerialNorCfgOption_DeviceType_MicronOctalDDR = 6,
    kSerialNorCfgOption_DeviceType_AdestoOctalDDR = 8,
} serial_nor_device_type_t;

typedef enum _serial_nor_quad_mode
{
    kSerialNorQuadMode_NotConfig = 0,
    kSerialNorQuadMode_StatusReg1_Bit6 = 1,
    kSerialNorQuadMode_StatusReg2_Bit1 = 2,
    kSerialNorQuadMode_StatusReg2_Bit7 = 3,
} serial_nor_quad_mode_t;

typedef enum _serial_nor_enhance_mode
{
    kSerialNorEnhanceMode_Disabled = 0,
    kSerialNorEnhanceMode_0_4_4_Mode = 1,
} serial_nor_enhance_mode_t;

/*! @brief Serial NOR configuration block */
typedef struct _flexspi_memory_config
{
    flexspi_device_config_t deviceConfig; /*!< Device configuration structure */
    flexspi_port_t devicePort;           /*!< Device connected to which port, SS0_A means port A1, SS0_B means port B1*/
    serial_nor_device_type_t deviceType; /*!< Serial nor flash device type  */
    serial_nor_quad_mode_t quadMode;     /*!< Serial nor flash quad mode setting. */
    serial_nor_enhance_mode_t enhanceMode; /*!< Serial nor flash performance enhance mode setting. */
    flexspi_pad_t commandPads;             /*!< Command padS setting. */
    flexspi_pad_t queryPads;               /*!< SFDP read pads setting. */
    uint8_t statusOverride;                /*!< Override status register value during device mode configuration. */
    uint32_t controllerMiscOption;   /*!< [0x040-0x043] Controller Misc Options, see Misc feature bit definitions for more*/
    uint32_t bytesInPageSize;   /*!< Page size in byte of Serial NOR */
    uint32_t bytesInSectorSize; /*!< Minimun Sector size in byte supported by Serial NOR */
    uint32_t bytesInMemorySize; /*!< Memory size in byte of Serial NOR */
    uint16_t busyOffset; /*!< Busy offset, valid value: 0-31, only need for check option kNandReadyCheckOption_RB */
    uint16_t busyBitPolarity; /*!< Busy flag polarity, 0 - busy flag is 1 when flash device is busy, 1 -busy flag is 0
                                   when flash device is busy, only need for check option kNandReadyCheckOption_RB */
    uint8_t ipcmdSerialClkFreq; /*!< Clock frequency for IP command*/
    uint32_t lookupTable[64];     /*!< Lookup table holds Flash command sequences */
    uint8_t serialNorType;        /*!< Serial NOR Flash type: 0/1/2/3*/
    uint8_t needExitNoCmdMode;    /*!< Need to exit NoCmd mode before other IP command*/
    uint8_t halfClkForNonReadCmd; /*!< Half the Serial Clock for non-read command: true/false*/
    uint8_t needRestoreNoCmdMode; /*!< Need to Restore NoCmd mode after IP commmand execution*/
    uint8_t deviceModeCfgEnable;  /*!< [0x010-0x010] Device Mode Configure enable flag, 1 - Enable, 0 - Disable*/
} flexspi_mem_config_t;

/*! @brief NOR Flash handle info */
typedef struct _flexspi_mem_nor_handle
{
    flexspi_port_t port;      /*!< Device connected to which port, SS0_A means port A1, SS0_B means port B1*/
    uint16_t busyOffset;      /*!< Busy offset, valid value: 0-31, only need for check option kNORReadyCheckOption_RB */
    uint16_t busyBitPolarity; /*!< Busy flag polarity, 0 - busy flag is 1 when flash device is busy, 1 -busy flag is 0
                                   when flash device is busy, only need for check option kNORReadyCheckOption_RB */
    uint32_t configuredFlashSize; /*!< Flash size configured by user through device configuration. */
} flexspi_mem_nor_handle_t;
