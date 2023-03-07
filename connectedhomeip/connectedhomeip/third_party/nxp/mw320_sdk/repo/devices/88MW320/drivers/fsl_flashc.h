/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLASHC_H_
#define _FSL_FLASHC_H_

#include "fsl_common.h"

/*!
 * @addtogroup flashc_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief FLASHC configuration way selection */
typedef enum
{
    kFLASHC_HardwareConfig, /*!< Use hardware configuration  */
    kFLASHC_SoftwareConfig, /*!< Use software configuration  */
} flashc_config_selection_t;

/*! @brief FLASHC hardware configuration */
typedef enum
{
    kFLASHC_HardwareCmdNormalRead,                    /*!< read data(0x03)  */
    kFLASHC_HardwareCmdFastRead,                      /*!< fast read data(0x0B)  */
    kFLASHC_HardwareCmdFastReadDualOutput,            /*!< fast read dual output (0x3B)  */
    kFLASHC_HardwareCmdFastReadQuadOutput,            /*!< fast read quad output (0x6B)  */
    kFLASHC_HardwareCmdFastReadDualIO,                /*!< fast read dual I/O (0xBB)  */
    kFLASHC_HardwareCmdFastReadDualIOContinuous,      /*!< fast read dual I/O with continuous read (0xBB)  */
    kFLASHC_HardwareCmdFastReadQuadIO,                /*!< fast read quad I/O (0xEB)  */
    kFLASHC_HardwareCmdFastReadQuadIOContinuous,      /*!< fast read quad I/O with continuous read (0xEB)  */
    kFLASHC_HardwareCmdWordReadQuadIO,                /*!< word read quad I/O (0xE7)  */
    kFLASHC_HardwareCmdWordReadQuadIOContinuous,      /*!< word read quad I/O with continuous read(0xE7)  */
    kFLASHC_HardwareCmdOctalWordReadQuadIO,           /*!< Octal word read quad I/O (0xE3) */
    kFLASHC_HardwareCmdOctalWordReadQuadIOContinuous, /*!< Octal word read quad I/O with continuous read(0xE3) */
    kFLASHC_HardwareCmdExitFromDual,                  /*!< exit from continuous read mode (dual) */
    kFLASHC_HardwareCmdExitFromQuad,                  /*!< exit from continuous read mode (quad) */
} flashc_hw_cmd_t;

/*! @brief FLASHC data pin */
typedef enum _flashc_data_pin
{
    kFLASHC_DataPinSingle, /*!< Use 1 serial interface pin.  */
    kFLASHC_DataPinDual,   /*!< Use 2 serial interface pins. */
    kFLASHC_DataPinQuad,   /*!< Use 4 serial interface pins. */
} flashc_data_pin_t;

/*! @brief FLASHC address pin */
typedef enum _flashc_addr_pin
{
    kFLASHC_AddrPinSingle, /*!< Use 1 serial interface pin */
    kFLASHC_AddrPinAsData, /*!< Use the number of pins as indicated in DATA_PIN */
} flashc_addr_pin_t;

/*! @brief FLASHC clock phase */
typedef enum _flashc_clock_phase
{
    kFLASHC_ClockPhaseRising,  /*!< Data is latched on the rising edge of the serial clock. */
    kFLASHC_ClockPhaseFalling, /*!< Data is latched on the falling edge of the serial clock. */
} flashc_clock_phase_t;

/*! @brief FLASHC clock polarity */
typedef enum _flashc_clock_polarity
{
    kFLASHC_ClockPolarityLow,  /*!< Inactive state of serial clock is low. */
    kFLASHC_ClockPolarityHigh, /*!< Inactive state of serial clock is high. */
} flashc_clock_polarity_t;

/*! @brief FLASHC capture clock edge */
typedef enum _flashc_capture_edge
{
    kFLASHC_CaptureEdgeFirst,  /*!< Input data is captured on the first edge of the serial clock  */
    kFLASHC_CaptureEdgeSecond, /*!< Input data is captured on the second edge of the serial clock */
} flashc_capture_edge_t;

/*! @brief FLASHC transfer length */
typedef enum _flashc_transfer_length
{
    kFLASHC_TransferLength1Byte,  /*!< 1 byte in each serial interface I/O transfer  */
    kFLASHC_TransferLength4Bytes, /*!< 4 bytes in each serial interface I/O transfer */
} flashc_transfer_length_t;

/*! @brief FLASHC clock prescaler */
typedef enum _flashc_clock_prescaler
{
    kFLASHC_ClockDiv1  = 0x1,  /*!< Serial interface clock prescaler is SPI clock / 1  */
    kFLASHC_ClockDiv2  = 0x2,  /*!< Serial interface clock prescaler is SPI clock / 2  */
    kFLASHC_ClockDiv3  = 0x3,  /*!< Serial interface clock prescaler is SPI clock / 3  */
    kFLASHC_ClockDiv4  = 0x4,  /*!< Serial interface clock prescaler is SPI clock / 4  */
    kFLASHC_ClockDiv5  = 0x5,  /*!< Serial interface clock prescaler is SPI clock / 5  */
    kFLASHC_ClockDiv6  = 0x6,  /*!< Serial interface clock prescaler is SPI clock / 6  */
    kFLASHC_ClockDiv7  = 0x7,  /*!< Serial interface clock prescaler is SPI clock / 7  */
    kFLASHC_ClockDiv8  = 0x8,  /*!< Serial interface clock prescaler is SPI clock / 8  */
    kFLASHC_ClockDiv9  = 0x9,  /*!< Serial interface clock prescaler is SPI clock / 9  */
    kFLASHC_ClockDiv10 = 0xA,  /*!< Serial interface clock prescaler is SPI clock / 10 */
    kFLASHC_ClockDiv11 = 0xB,  /*!< Serial interface clock prescaler is SPI clock / 11 */
    kFLASHC_ClockDiv12 = 0xC,  /*!< Serial interface clock prescaler is SPI clock / 12 */
    kFLASHC_ClockDiv13 = 0xD,  /*!< Serial interface clock prescaler is SPI clock / 13 */
    kFLASHC_ClockDiv14 = 0xE,  /*!< Serial interface clock prescaler is SPI clock / 14 */
    kFLASHC_ClockDiv15 = 0xF,  /*!< Serial interface clock prescaler is SPI clock / 15 */
    kFLASHC_ClockDiv16 = 0x18, /*!< Serial interface clock prescaler is SPI clock / 16 */
    kFLASHC_ClockDiv18 = 0x19, /*!< Serial interface clock prescaler is SPI clock / 18 */
    kFLASHC_ClockDiv20 = 0x1A, /*!< Serial interface clock prescaler is SPI clock / 20 */
    kFLASHC_ClockDiv22 = 0x1B, /*!< Serial interface clock prescaler is SPI clock / 22 */
    kFLASHC_ClockDiv24 = 0x1C, /*!< Serial interface clock prescaler is SPI clock / 24 */
    kFLASHC_ClockDiv26 = 0x1D, /*!< Serial interface clock prescaler is SPI clock / 26 */
    kFLASHC_ClockDiv28 = 0x1E, /*!< Serial interface clock prescaler is SPI clock / 28 */
    kFLASHC_ClockDiv30 = 0x1F, /*!< Serial interface clock prescaler is SPI clock / 30 */
} flashc_clock_prescaler_t;

/*! @brief FLASHC Instruction Count */
typedef enum _flashc_instruction_count
{
    kFLASHC_InstructionCnt0Byte, /*!< Instrution count: 0 byte  */
    kFLASHC_InstructionCnt1Byte, /*!< Instrution count: 1 byte  */
    kFLASHC_InstructionCnt2Byte, /*!< Instrution count: 2 bytes */
} flashc_instruction_count_t;

/*! @brief FLASHC Address Count */
typedef enum _flashc_address_count
{
    kFLASHC_AddressCnt0Byte, /*!< Address count: 0 byte  */
    kFLASHC_AddressCnt1Byte, /*!< Address count: 1 byte  */
    kFLASHC_AddressCnt2Byte, /*!< Address count: 2 bytes */
    kFLASHC_AddressCnt3Byte, /*!< Address count: 3 bytes */
    kFLASHC_AddressCnt4Byte, /*!< Address count: 4 bytes */
} flashc_address_count_t;

/*! @brief FLASHC Read Mode Count */
typedef enum _flashc_read_mode_count
{
    kFLASHC_ReadModeCnt0Byte, /*!< Read mode count: 0 byte  */
    kFLASHC_ReadModeCnt1Byte, /*!< Read mode count: 1 byte  */
    kFLASHC_ReadModeCnt2Byte, /*!< Read mode count: 2 bytes */
} flashc_read_mode_count_t;

/*! @brief FLASHC Dummy Count */
typedef enum _flashc_dummy_count
{
    kFLASHC_DummyCnt0Byte, /*!< Dummy count: 0 byte  */
    kFLASHC_DummyCnt1Byte, /*!< Dummy count: 1 byte  */
    kFLASHC_DummyCnt2Byte, /*!< Dummy count: 2 bytes */
} flashc_dummy_count_t;

/*! @brief FLASHC timing configure structure*/
typedef struct _flashc_timing_config
{
    flashc_clock_phase_t clockPhase;       /*!< Serial Interface Clock Phase. */
    flashc_clock_polarity_t clockPolarity; /*!< Serial Interface Clock Polarity. */
    flashc_clock_prescaler_t preScaler;    /*!< Configure FLASHC prescaler for serial interface */
    flashc_capture_edge_t captEdge;        /*!< Configure FLASHC capture clock edge */
    uint8_t clkOutDly;                     /*!< Delay on the outgoing clock to flash */
    uint8_t clkInDly;                      /*!< Delay on the clock that capture read data from flash */
    uint8_t dinDly;                        /*!< Delay on the incoming data from flash */
} flashc_timing_config_t;

/*! @brief FLASHC configure structure*/
typedef struct _flashc_config
{
    flashc_timing_config_t timingConfig;  /*!< Timing configuration */
    flashc_data_pin_t dataPinMode;        /*!< Configure FLASHC data pin */
    flashc_addr_pin_t addrPinMode;        /*!< Configure FLASHC addr pin */
    flashc_transfer_length_t byteLen;     /*!< Configure FLASHC length each serial transfer */
    flashc_dummy_count_t dummyCnt;        /*!< Dummy count */
    flashc_read_mode_count_t readModeCnt; /*!< read mode count */
    uint16_t readMode;                    /*!< read mode */
    flashc_address_count_t addrCnt;       /*!< address count */
    flashc_instruction_count_t instrCnt;  /*!< instruction count */
    uint16_t instrucion;                  /*!< instruction */
} flashc_config_t;

/******************************************************************************
 * API
 *****************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Get the instance number for FLASHC.
 *
 * @param base FLASHC base pointer.
 */
uint32_t FLASHC_GetInstance(FLASHC_Type *base);

/*!
 * @brief Initializes FLASHC, it override the HW configuration (FCCR.CMD_TYPE) if it is enabled.
 *
 * @param base FLASHC base pointer.
 * @param config FLASHC configure.
 */
void FLASHC_Init(FLASHC_Type *base, flashc_config_t *config);

/*!
 * @brief De-initializes FLASHC.
 *
 * @param base FLASHC base pointer.
 */
void FLASHC_Deinit(FLASHC_Type *base);

/*!
 * @brief Gets default settings for FLASHC.
 *
 * @param config FLASHC configuration structure.
 */
void FLASHC_GetDefaultConfig(flashc_config_t *config);

/*!
 * @brief Configure the timing of the FLASHC.
 *
 * @param base FLASHC base pointer.
 * @param config FLASHC timing configure.
 */
void FLASHC_SetTimingConfig(FLASHC_Type *base, flashc_timing_config_t *config);

/*!
 * @brief Flush the Cache.
 *
 * @param base FLASHC base pointer.
 * @return return status.
 */
status_t FLASHC_FlushCache(FLASHC_Type *base);

/*!
 * @brief Enable Flash Cache Hit Counters.
 *
 * @param base FLASHC base pointer.
 * @param enable true/false.
 */
void FLASHC_EnableCacheHitCounter(FLASHC_Type *base, bool enable);

/*!
 * @brief Enable Flash Cache Miss Counters.
 *
 * @param base FLASHC base pointer.
 * @param enable true/false.
 */
void FLASHC_EnableCacheMissCounter(FLASHC_Type *base, bool enable);

/*!
 * @brief Enable Flash Cache Miss Counters.
 *
 * @param base FLASHC base pointer.
 * @param hitCnt cache hit counter.
 * @param missCnt cache miss counter.
 */
void FLASHC_GetCacheCounter(FLASHC_Type *base, uint32_t *hitCnt, uint32_t *missCnt);

/*!
 * @brief Reset Flash Controller Cache Counters.
 *
 * @param base FLASHC base pointer.
 */
void FLASHC_ResetCacheCounter(FLASHC_Type *base);

/*!
 * @brief Enable the cache mode.
 */
static inline void FLASHC_EnableCacheMode(FLASHC_Type *base)
{
    FLASHC->FCCR |= FLASHC_FCCR_CACHE_EN_MASK;
}

/*!
 * @brief Disable the cache mode.
 */
static inline void FLASHC_DisableCacheMode(FLASHC_Type *base)
{
    FLASHC->FCCR &= ~FLASHC_FCCR_CACHE_EN_MASK;
}

/*!
 * @brief Enable the sram mode.
 */
static inline void FLASHC_EnableSramMode(FLASHC_Type *base)
{
    FLASHC->FCCR |= FLASHC_FCCR_SRAM_MODE_EN_MASK;
}

/*!
 * @brief Disable the sram mode.
 */
static inline void FLASHC_DisableSramMode(FLASHC_Type *base)
{
    FLASHC->FCCR &= ~FLASHC_FCCR_SRAM_MODE_EN_MASK;
}

/*!
 * @brief Hardware command configuration for FLASHC.
 *
 * @param base FLASHC base pointer.
 * @param command FLASHC hardware command for read.
 */
static inline void FLASHC_SetHardwareCommand(FLASHC_Type *base, flashc_hw_cmd_t command)
{
    uint32_t temp = 0x00U;

    temp = base->FCCR;

    base->FCCR = (temp & ~FLASHC_FCCR_CMD_TYPE_MASK) | FLASHC_FCCR_CMD_TYPE(command);
}

/*!
 * @brief Set selection hardware or software configuration for FLASHC.
 *
 * @param base FLASHC base pointer.
 * @param cfgSel FLASHC Configuration Override.
 */
static inline void FLASHC_SetConfigSelection(FLASHC_Type *base, flashc_config_selection_t cfgSel)
{
    uint32_t temp = 0x00U;

    temp = base->FCCR2;

    base->FCCR2 = (temp & ~FLASHC_FCCR2_USE_CFG_OVRD_MASK) | FLASHC_FCCR2_USE_CFG_OVRD(cfgSel);
}

/*!
 * @brief do not use offset address for flash memory access.
 *
 * @param base FLASHC base pointer.
 */
static inline void FLASHC_DisableOffsetAddr(FLASHC_Type *base)
{
    base->FCACR &= ~FLASHC_FCACR_OFFSET_EN_MASK;
}

/*!
 * @brief Use offset address for flash memory access and set the offset address.
 *
 * @param base FLASHC base pointer.
 * @param address offset address.
 */
void FLASHC_EnableOffsetAddr(FLASHC_Type *base, uint32_t address);

/*!
 * @brief Get exit continuous read mode status.
 *
 * @param base FLASHC base pointer.
 */
static inline bool FLASHC_GetExitContReadStatus(FLASHC_Type *base)
{
    return ((base->FCSR & FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK) ? true : false);
}

/*!
 * @brief Exit dual continuous read mode.
 *
 * @param base FLASHC base pointer.
 * @return return status.
 */
status_t FLASHC_ExitDualContReadMode(FLASHC_Type *base);

/*!
 * @brief Exit quad continuous read mode.
 *
 * @param base FLASHC base pointer.
 * @return return status.
 */
status_t FLASHC_ExitQuadContReadMode(FLASHC_Type *base);

/*!
 * @brief Switch into QSPI from FLASHC.
 *
 * @param base FLASHC base pointer.
 * @return return status.
 */
status_t FLASHC_EnableQSPIPad(FLASHC_Type *base);

/*!
 * @brief Switch back FLASHC from QSPI.
 *
 * @param base FLASHC base pointer.
 * @param mode FLASHC read mode.
 * @param jedecID JEDEC ID.
 */
void FLASHC_EnableFLASHCPad(FLASHC_Type *base, flashc_hw_cmd_t mode, uint32_t jedecID);

/*!
 * @brief Set quad mode read command.
 *
 * @param base FLASHC base pointer.
 * @param jedecID JEDEC ID.
 */
void FLASH_SetQuadModeReadCmd(FLASHC_Type *base, uint32_t jedecID);

/*! @} */

#if defined(__cplusplus)
}
#endif

/* @}*/

#endif /* _FSL_QSPI_H_*/
