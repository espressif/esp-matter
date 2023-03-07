/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flashc.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flashc"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Base pointer array */
static FLASHC_Type *const s_flashcBases[] = FLASHC_BASE_PTRS;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief Get the instance number for FLASHC.
 *
 * param base FLASHC base pointer.
 */
uint32_t FLASHC_GetInstance(FLASHC_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_flashcBases); instance++)
    {
        if (s_flashcBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_flashcBases));

    return instance;
}

/*!
 * brief Initializes FLASHC, it override the HW configuration (FCCR.CMD_TYPE) if it is enabled.
 *
 * param base FLASHC base pointer.
 * param config FLASHC configure.
 */
void FLASHC_Init(FLASHC_Type *base, flashc_config_t *config)
{
    uint32_t temp = 0x00U;

    /* Set instruction count*/
    temp |= FLASHC_FCCR2_INSTR_CNT(config->instrCnt);
    /* Set address count*/
    temp |= FLASHC_FCCR2_ADDR_CNT(config->addrCnt);
    /* Set read mode count */
    temp |= FLASHC_FCCR2_RM_CNT(config->readModeCnt);
    /* Set dummy count */
    temp |= FLASHC_FCCR2_DUMMY_CNT(config->dummyCnt);
    /* Set data length mode */
    temp |= FLASHC_FCCR2_BYTE_LEN(config->byteLen);
    /* Set address pin number */
    temp |= FLASHC_FCCR2_ADDR_PIN(config->addrPinMode);
    /* Set data pin number */
    temp |= FLASHC_FCCR2_DATA_PIN(config->dataPinMode);
    base->FCCR2 = temp;

    /* Set read mode  */
    base->FRMR = FLASHC_FRMR_RDMODE(config->readMode);

    /* Set instruction */
    base->FINSTR = FLASHC_FINSTR_INSTR(config->instrucion);

    /* Set timing configuration. */
    FLASHC_SetTimingConfig(base, &config->timingConfig);
}

/*!
 * @brief De-initializes FLASHC.
 *
 * @param base FLASHC base pointer.
 */
void FLASHC_Deinit(FLASHC_Type *base)
{
}

/*!
 * brief Gets default settings for FLASHC.
 *
 * param config FLASHC configuration structure.
 */
void FLASHC_GetDefaultConfig(flashc_config_t *config)
{
    /* Initializes the configure structure to zero. */
    (void)memset(config, 0x00U, sizeof(*config));

    config->timingConfig.clockPhase    = kFLASHC_ClockPhaseRising;
    config->timingConfig.clockPolarity = kFLASHC_ClockPolarityLow;
    config->timingConfig.preScaler     = kFLASHC_ClockDiv1;
    config->timingConfig.captEdge      = kFLASHC_CaptureEdgeFirst;
    config->timingConfig.clkOutDly     = 0x00U;
    config->timingConfig.clkInDly      = 0x00U;
    config->timingConfig.dinDly        = 0x00U;

    config->dataPinMode = kFLASHC_DataPinSingle;
    config->addrPinMode = kFLASHC_AddrPinSingle;
    config->byteLen     = kFLASHC_TransferLength1Byte;
    config->dummyCnt    = kFLASHC_DummyCnt0Byte;
    config->readModeCnt = kFLASHC_ReadModeCnt0Byte;
    config->readMode    = 0x00U;
    config->addrCnt     = kFLASHC_AddressCnt0Byte;
    config->instrCnt    = kFLASHC_InstructionCnt0Byte;
    config->instrucion  = 0x00U;
}

/*!
 * brief Configure the timing of the FLASHC.
 *
 * param base FLASHC base pointer.
 * param config FLASHC timing configure.
 */
void FLASHC_SetTimingConfig(FLASHC_Type *base, flashc_timing_config_t *config)
{
    uint32_t temp = 0x00U;

    temp = FLASHC->FCCR;
    /* Set FLASHC clock phase and polarity. */
    temp = ((temp & ~FLASHC_FCCR_CLK_PHA_MASK) | FLASHC_FCCR_CLK_PHA(config->clockPhase)) |
           ((temp & ~FLASHC_FCCR_CLK_POL_MASK) | FLASHC_FCCR_CLK_POL(config->clockPolarity));

    /* Set FLASHC clock prescaler */
    temp = ((temp & ~FLASHC_FCCR_CLK_PRESCALE_MASK) | FLASHC_FCCR_CLK_PRESCALE(config->preScaler));

    FLASHC->FCCR = temp;

    temp = FLASHC->FCTR;

    /* Set FLASHC capture clock edge */
    temp = ((temp & ~FLASHC_FCTR_CLK_CAPT_EDGE_MASK) | FLASHC_FCTR_CLK_CAPT_EDGE(config->captEdge));

    /* set clock out delay */
    temp = ((temp & ~FLASHC_FCTR_CLK_OUT_DLY_MASK) | FLASHC_FCTR_CLK_OUT_DLY(config->clkOutDly));

    /* set clock in delay */
    temp = ((temp & ~FLASHC_FCTR_CLK_IN_DLY_MASK) | FLASHC_FCTR_CLK_IN_DLY(config->clkInDly));

    /* set data in delay */
    temp = ((temp & ~FLASHC_FCTR_DIN_DLY_MASK) | FLASHC_FCTR_DIN_DLY(config->dinDly));

    FLASHC->FCTR = temp;
}

/*!
 * brief Flush the Cache.
 *
 * param base FLASHC base pointer.
 * return return status.
 */
status_t FLASHC_FlushCache(FLASHC_Type *base)
{
    status_t status       = kStatus_Fail;
    volatile uint32_t cnt = 0U;

    /* Cache Line Flush. */
    FLASHC->FCCR |= FLASHC_FCCR_CACHE_LINE_FLUSH_MASK;

    while (cnt < 0x200000U)
    {
        if ((FLASHC->FCCR & FLASHC_FCCR_CACHE_LINE_FLUSH_MASK) == 0U)
        {
            status = kStatus_Success;
            break;
        }

        cnt++;
    }

    return status;
}

/*!
 * brief Enable Flash Cache Hit Counters.
 *
 * param base FLASHC base pointer.
 * param enable true/false.
 */
void FLASHC_EnableCacheHitCounter(FLASHC_Type *base, bool enable)
{
    if (enable == true)
    {
        FLASHC->FCACR |= FLASHC_FCACR_HIT_CNT_EN_MASK;
    }
    else
    {
        FLASHC->FCACR &= ~FLASHC_FCACR_HIT_CNT_EN_MASK;
    }
}

/*!
 * brief Enable Flash Cache Miss Counters.
 *
 * param base FLASHC base pointer.
 * param enable true/false.
 */
void FLASHC_EnableCacheMissCounter(FLASHC_Type *base, bool enable)
{
    if (enable == true)
    {
        FLASHC->FCACR |= FLASHC_FCACR_MISS_CNT_EN_MASK;
    }
    else
    {
        FLASHC->FCACR &= ~FLASHC_FCACR_MISS_CNT_EN_MASK;
    }
}

/*!
 * brief Enable Flash Cache Miss Counters.
 *
 * param base FLASHC base pointer.
 * param hitCnt cache hit counter.
 * param missCnt cache miss counter.
 */
void FLASHC_GetCacheCounter(FLASHC_Type *base, uint32_t *hitCnt, uint32_t *missCnt)
{
    assert(hitCnt != NULL);
    assert(missCnt != NULL);

    /* Get cache hit counter. */
    *hitCnt = FLASHC->FCHCR;
    /* Get cache miss counter. */
    *missCnt = FLASHC->FCMCR;
}

/*!
 * brief Reset Flash Controller Cache Counters.
 *
 * param base FLASHC base pointer.
 */
void FLASHC_ResetCacheCounter(FLASHC_Type *base)
{
    /* Clear cache hit counter. */
    FLASHC->FCHCR = 0x00U;
    /* Clear cache miss counter. */
    FLASHC->FCMCR = 0x00U;
}

/*!
 * brief Use offset address for flash memory access and set the offset address.
 *
 * param base FLASHC base pointer.
 * param address offset address.
 */
void FLASHC_EnableOffsetAddr(FLASHC_Type *base, uint32_t address)
{
    base->FCACR |= FLASHC_FCACR_OFFSET_EN_MASK;

    base->FAOFFR = address;
}

/*!
 * brief Exit dual continuous read mode.
 *
 * param base FLASHC base pointer.
 * return return status.
 */
status_t FLASHC_ExitDualContReadMode(FLASHC_Type *base)
{
    volatile uint32_t cnt = 0x00U;
    uint32_t temp         = 0x00U;
    uint32_t cacheMode;

    /*Backup cache mode. */
    cacheMode = (base->FCCR & FLASHC_FCCR_CACHE_EN_MASK) >> FLASHC_FCCR_CACHE_EN_SHIFT;
    /* Disable cache mode. */
    base->FCCR &= ~FLASHC_FCCR_CACHE_EN_MASK;

    /* clear the continuous read mode exit status bit. */
    base->FCSR = FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK;

    temp = base->FCCR;
    /* Set hardware command for exit from dual read mode. */
    base->FCCR = (temp & ~FLASHC_FCCR_CMD_TYPE_MASK) | FLASHC_FCCR_CMD_TYPE(kFLASHC_HardwareCmdExitFromDual);

    while (cnt < 0x200000U)
    {
        if ((base->FCSR & FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK) == FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK)
        {
            /* clear the continuous read mode exit status bit. */
            base->FCSR = FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK;
            base->FCCR |= FLASHC_FCCR_CACHE_EN(cacheMode);
            return kStatus_Success;
        }

        cnt++;
    }

    /* Restore cache mode. */
    base->FCCR |= FLASHC_FCCR_CACHE_EN(cacheMode);

    return kStatus_Fail;
}

/*!
 * brief Exit quad continuous read mode.
 *
 * param base FLASHC base pointer.
 * return return status.
 */
status_t FLASHC_ExitQuadContReadMode(FLASHC_Type *base)
{
    volatile uint32_t cnt = 0x00U;
    uint32_t temp         = 0x00U;
    uint32_t cacheMode;

    /*Backup cache mode. */
    cacheMode = (base->FCCR & FLASHC_FCCR_CACHE_EN_MASK) >> FLASHC_FCCR_CACHE_EN_SHIFT;
    /* Disable cache mode. */
    base->FCCR &= ~FLASHC_FCCR_CACHE_EN_MASK;

    /* clear the continuous read mode exit status bit. */
    base->FCSR = FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK;

    temp = base->FCCR;
    /* Set hardware command for exit from quad read mode. */
    base->FCCR = (temp & ~FLASHC_FCCR_CMD_TYPE_MASK) | FLASHC_FCCR_CMD_TYPE(kFLASHC_HardwareCmdExitFromQuad);

    while (cnt < 0x200000U)
    {
        if ((base->FCSR & FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK) == FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK)
        {
            /* clear the continuous read mode exit status bit. */
            base->FCSR = FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK;
            base->FCCR |= FLASHC_FCCR_CACHE_EN(cacheMode);
            return kStatus_Success;
        }

        cnt++;
    }

    /* Restore cache mode. */
    base->FCCR |= FLASHC_FCCR_CACHE_EN(cacheMode);

    return kStatus_Fail;
}

/*!
 * brief Enable QSPI pad.
 *
 * param base FLASHC base pointer.
 * return return status.
 */
status_t FLASHC_EnableQSPIPad(FLASHC_Type *base)
{
    status_t status      = kStatus_Fail;
    uint32_t currentMode = 0x00U;
    uint32_t exitMethod  = 0x00U;

    status = FLASHC_FlushCache(base);
    if (status != kStatus_Success)
    {
        return kStatus_Fail;
    }

    currentMode = (base->FCCR & FLASHC_FCCR_CMD_TYPE_MASK) >> FLASHC_FCCR_CMD_TYPE_SHIFT;

    switch (currentMode)
    {
        case kFLASHC_HardwareCmdFastReadDualOutput:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdFastReadDualIO:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdFastReadDualIOContinuous:
            exitMethod = kFLASHC_HardwareCmdExitFromDual;
            break;
        case kFLASHC_HardwareCmdFastReadQuadOutput:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdFastReadQuadIO:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdFastReadQuadIOContinuous:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdWordReadQuadIO:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdWordReadQuadIOContinuous:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdOctalWordReadQuadIO:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kFLASHC_HardwareCmdOctalWordReadQuadIOContinuous:
            exitMethod = kFLASHC_HardwareCmdExitFromQuad;
            break;
        default:
            assert(NULL);
            break;
    }

    if (exitMethod == (uint8_t)kFLASHC_HardwareCmdExitFromQuad)
    {
        status = FLASHC_ExitQuadContReadMode(base);
    }
    else if (exitMethod == (uint8_t)kFLASHC_HardwareCmdExitFromDual)
    {
        status = FLASHC_ExitDualContReadMode(base);
    }
    else
    {
        ; /* Intentional empty for MISRA C-2012 rule 15.7. */
    }

    if (status == kStatus_Success)
    {
        /* Select PAD to QSPI/ */
        base->FCCR &= ~FLASHC_FCCR_FLASHC_PAD_EN_MASK;
    }

    return status;
}

/*!
 * brief Enable FLASHC pad.
 *
 * param base FLASHC base pointer.
 * param mode FLASHC read mode.
 * param jedecID JEDEC ID.
 */
void FLASHC_EnableFLASHCPad(FLASHC_Type *base, flashc_hw_cmd_t mode, uint32_t jedecID)
{
    uint32_t temp = 0x00U;

    temp = base->FCCR;
    /* Select PAD to FLASHC */
    temp |= FLASHC_FCCR_FLASHC_PAD_EN_MASK;

    /* Check continuous read mode requirement is supported or not. */
    if (kFLASHC_HardwareCmdFastReadDualIOContinuous == mode)
    {
        switch (jedecID)
        {
            case 0xef4014:
            case 0xef4015:
            case 0xef4016:
            case 0xef4017:
            case 0xef4018:
            case 0xc84016:
            case 0xc86016:
            case 0xc84018:
                /*
                 * For continuous quad read mode, flash chip read mode bits
                 * [M7-0] needs to have 0x20h value (bit 4/5 set). This is
                 * limitation of flash controller in 88MW300. In addition to
                 * exit from continuous read mode, command 0xFFh is required.
                 * Please check flash part datasheet before updating this.
                 */
                break;
            default:
                mode = kFLASHC_HardwareCmdFastReadDualIO;
                break;
        }
    }

    /* Check continuous read mode requirement is supported or not. */
    if (kFLASHC_HardwareCmdFastReadQuadIOContinuous == mode)
    {
        switch (jedecID)
        {
            case 0xef4014:
            case 0xef4015:
            case 0xef4016:
            case 0xef4017:
            case 0xef4018:
            case 0xc84016:
            case 0xc86016:
            case 0xc84018:
                /*
                 * For continuous quad read mode, flash chip read mode bits
                 * [M7-0] needs to have 0x20h value (bit 4/5 set). This is
                 * limitation of flash controller in 88MW300. In addition to
                 * exit from continuous read mode, command 0xFFh is required.
                 * Please check flash part datasheet before updating this.
                 */
                break;
            default:
                mode = kFLASHC_HardwareCmdFastReadQuadIO;
                break;
        }
    }

    /* Set hardware command for FLASHC read mode. */
    base->FCCR = (temp & ~FLASHC_FCCR_CMD_TYPE_MASK) | FLASHC_FCCR_CMD_TYPE(mode);
}

/*!
 * brief Set quad mode read command.
 *
 * param base FLASHC base pointer.
 * param jedecID JEDEC ID.
 */
void FLASH_SetQuadModeReadCmd(FLASHC_Type *base, uint32_t jedecID)
{
    uint32_t temp = 0x00U;

    temp = base->FCCR;

    switch (jedecID)
    {
        case 0xef4014:
        case 0xef4015:
        case 0xef4016:
        case 0xef4017:
        case 0xef4018:
        case 0xc84016:
        case 0xc86016:
        case 0xc84018:
            /*
             * For continuous quad read mode, flash chip read mode bits
             * [M7-0] needs to have 0x20h value (bit 4/5 set). This is
             * limitation of flash controller in 88MW300. In addition to
             * exit from continuous read mode, command 0xFFh is required.
             * Please check flash part datasheet before updating this.
             */
            base->FCCR =
                (temp & ~FLASHC_FCCR_CMD_TYPE_MASK) | FLASHC_FCCR_CMD_TYPE(kFLASHC_HardwareCmdFastReadQuadIOContinuous);
            break;
        case 0xc84015:
        case 0xc22014:
        case 0xc22016:
        case 0xc22017:
        case 0xc22018:
        case 0xc22810:
        case 0xc22811:
        case 0xc22812:
        case 0xc22813:
        case 0xc22814:
        case 0xc22815:
        case 0xc22816:
        case 0xc22817:
        case 0xc22310:
        case 0xc22311:
        case 0xc22312:
        case 0xc22313:
        case 0xc22314:
        case 0xc22315:
            base->FCCR = (temp & ~FLASHC_FCCR_CMD_TYPE_MASK) | FLASHC_FCCR_CMD_TYPE(kFLASHC_HardwareCmdFastReadQuadIO);
            break;
        default:
            assert(NULL);
            break;
    }
}
