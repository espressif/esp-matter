/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ssd1963.h"

/*******************************************************************************
 * Definitations
 ******************************************************************************/
#define SSD1963_ORIENTATION_MODE_MASK \
    (SSD1963_ADDR_MODE_PAGE_ADDR_ORDER | SSD1963_ADDR_MODE_PAG_COL_ADDR_ORDER | SSD1963_ADDR_MODE_COL_ADDR_ORDER)

#define SSD1963_FLIP_MODE_MASK (SSD1963_ADDR_MODE_FLIP_VERT | SSD1963_ADDR_MODE_FLIP_HORZ)

/* The PLL VCO clock must be in the range of (250MHz, 800MHz). */
#define SSD1963_VCO_MIN_HZ 250000000U
#define SSD1963_VCO_MAX_HZ 800000000U
#define SSD1963_PLL_MULTI_MIN 0x0U
#define SSD1963_PLL_MULTI_MAX 0xFFU
#define SSD1963_PLL_DIV_MIN 0x0U
#define SSD1963_PLL_DIV_MAX 0x1FU

/* The PLL output frequency will be configured to about 100MHz. */
#define SSD1963_PLL_FREQ_HZ 100000000U

/* The max value of LCDC_FPR to generate the lshift clock (pixel clock). */
#define SSD1963_LCDC_FPR_MAX 0xFFFFFU

#if (SSD1963_DATA_WITDH != 16) && (SSD1963_DATA_WITDH != 8)
#error Only support 8-bit or 16-bit data bus
#endif

#define SSD1963_DATA_WITDH_BYTE (SSD1963_DATA_WITDH / 8)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Use loop to delay.
 *
 * @param loops Number of the loops.
 */
static void SSD1963_Delay(uint32_t loops);

/*!
 * @brief Get the multiplier and divider setting for PLL.
 *
 * This function gets the multiplier and divider to generate PLL frequency at
 * about 100MHz. The actually PLL frequency is returned.
 *
 * @param multi The multiplier value.
 * @param div The divider value.
 * @param srcClock_Hz The external reference clock(XTAL or CLK) frequency in Hz.
 * @return Generated PLL frequency with the @p multi and @p div. If could not get
 * the desired PLL frequency, this function returns 0.
 */
static uint32_t SSD1963_GetPllDivider(uint8_t *multi, uint8_t *div, uint32_t srcClock_Hz);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void SSD1963_Delay(uint32_t loops)
{
    while (loops--)
    {
        __NOP();
    }
}

static uint32_t SSD1963_GetPllDivider(uint8_t *multi, uint8_t *div, uint32_t srcClock_Hz)
{
    uint32_t multiCur, divCur, pllFreqCur, vcoCur, diffCur;
    uint32_t multiCandidate   = 0U;
    uint32_t divCandidate     = 0U;
    uint32_t pllFreqCandidate = 0U;
    uint32_t diff             = 0xFFFFFFFFU;

    for (multiCur = SSD1963_PLL_MULTI_MIN; multiCur <= SSD1963_PLL_MULTI_MAX; multiCur++)
    {
        vcoCur = srcClock_Hz * (multiCur + 1U);

        /* VCO must be larger than SSD1963_VCO_MIN_HZ. */
        if (vcoCur <= SSD1963_VCO_MIN_HZ)
        {
            continue;
        }

        /* VCO must be smaller than SSD1963_VCO_MAX_HZ. */
        if (vcoCur >= SSD1963_VCO_MAX_HZ)
        {
            break;
        }

        divCur = ((vcoCur + (SSD1963_PLL_FREQ_HZ / 2U)) / SSD1963_PLL_FREQ_HZ) - 1U;

        /*
         * VCO frequency must be in the range of (250MHz, 800MHz). The desired
         * PLL output frequency is 100MHz, then the divCur here must be in the
         * range of (1, 8). In this case, it is not necessary to check whether
         * divCur is in the range of (0, 31). But for safty when the desired
         * PLL frequency is changed, here check the upper range.
         */
#if ((((SSD1963_VCO_MAX_HZ + (SSD1963_PLL_FREQ_HZ / 2U)) / SSD1963_PLL_FREQ_HZ) - 1U) > SSD1963_PLL_DIV_MAX)
        if (divCur > SSD1963_PLL_DIV_MAX)
        {
            divCur = SSD1963_PLL_DIV_MAX;
        }
#endif

        pllFreqCur = vcoCur / (divCur + 1);

        if (SSD1963_PLL_FREQ_HZ > pllFreqCur)
        {
            diffCur = SSD1963_PLL_FREQ_HZ - pllFreqCur;
        }
        else
        {
            diffCur = pllFreqCur - SSD1963_PLL_FREQ_HZ;
        }

        /* Find better multi and divider. */
        if (diff > diffCur)
        {
            diff             = diffCur;
            multiCandidate   = multiCur;
            divCandidate     = divCur;
            pllFreqCandidate = pllFreqCur;
        }
    }

    *multi = (uint8_t)multiCandidate;
    *div   = (uint8_t)divCandidate;

    return pllFreqCandidate;
}

status_t SSD1963_Init(ssd1963_handle_t *handle,
                      const ssd1963_config_t *config,
                      const dbi_xfer_ops_t *xferOps,
                      void *xferOpsData,
                      uint32_t srcClock_Hz)
{
    assert(handle);
    assert(config);

    uint8_t multi, div;
    uint32_t pllFreq_Hz;
    uint32_t fpr; /* Pixel clock = PLL clock * ((fpr + 1) / 2^20) */
#if (16 == SSD1963_DATA_WITDH)
    uint16_t commandParam[8];
#else
    uint8_t commandParam[8];
#endif
    uint16_t vt, vps, ht, hps;

    pllFreq_Hz = SSD1963_GetPllDivider(&multi, &div, srcClock_Hz);

    /* Could not set the PLL to desired frequency. */
    if (!pllFreq_Hz)
    {
        return kStatus_InvalidArgument;
    }

    fpr = (uint32_t)(((float)config->pclkFreq_Hz / (float)pllFreq_Hz) * (float)(1U << 20U));

    if ((fpr < 1) || (fpr > (SSD1963_LCDC_FPR_MAX + 1)))
    {
        return kStatus_InvalidArgument;
    }

    fpr--;

    /* Initialize the handle. */
    memset(handle, 0, sizeof(ssd1963_handle_t));

    handle->panelWidth  = config->panelWidth;
    handle->panelHeight = config->panelHeight;
    handle->xferOps     = xferOps;
    handle->xferOpsData = xferOpsData;

    /* Soft reset. */
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SOFT_RESET);
    SSD1963_Delay(50000);

    /* Setup the PLL. */
    /* Set the multiplier and divider. */
    commandParam[0] = multi;
    commandParam[1] = div | (1U << 5U);
    commandParam[2] = 1U << 2U;
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SET_PLL_MN);
    handle->xferOps->writeData(xferOpsData, commandParam, 3U * SSD1963_DATA_WITDH_BYTE);

    /* Enable PLL. */
    commandParam[0] = 0x01U;
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SET_PLL);
    handle->xferOps->writeData(xferOpsData, commandParam, 1U * SSD1963_DATA_WITDH_BYTE);

    /* Delay at least 100us, to wait for the PLL stable. */
    SSD1963_Delay(500);

    /* Use the PLL. */
    commandParam[0] = 0x03U;
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SET_PLL);
    handle->xferOps->writeData(xferOpsData, commandParam, 1U * SSD1963_DATA_WITDH_BYTE);

    /* Configure the pixel clock. */
    commandParam[0] = (fpr & 0xFF0000U) >> 16U;
    commandParam[1] = (fpr & 0xFF00U) >> 8U;
    commandParam[2] = (fpr & 0xFFU);
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SET_LSHIFT_FREQ);
    handle->xferOps->writeData(xferOpsData, commandParam, 3U * SSD1963_DATA_WITDH_BYTE);

    /* Configure LCD panel. */
    commandParam[0] = config->panelDataWidth | config->polarityFlags; /* Not enable FRC, dithering. */
    commandParam[1] = 0x20U;                                          /* TFT mode. */
    commandParam[2] = (config->panelWidth - 1) >> 8;
    commandParam[3] = (config->panelWidth - 1) & 0xFFU;
    commandParam[4] = (config->panelHeight - 1) >> 8;
    commandParam[5] = (config->panelHeight - 1) & 0xFFU;
    commandParam[6] = 0;
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SET_LCD_MODE);
    handle->xferOps->writeData(xferOpsData, commandParam, 7 * SSD1963_DATA_WITDH_BYTE);

    /* Horizontal period setting. */
    ht              = config->panelWidth + config->hsw + config->hfp + config->hbp;
    hps             = config->hsw + config->hbp;
    commandParam[0] = (ht - 1U) >> 8U;
    commandParam[1] = (ht - 1U) & 0xFFU;
    commandParam[2] = hps >> 8U;
    commandParam[3] = hps & 0xFFU;
    commandParam[4] = config->hsw - 1U;
    commandParam[5] = 0U;
    commandParam[6] = 0U;
    commandParam[7] = 0U;
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SET_HORI_PERIOD);
    handle->xferOps->writeData(xferOpsData, commandParam, 8 * SSD1963_DATA_WITDH_BYTE);

    /* Vertical period setting. */
    vt              = config->panelHeight + config->vsw + config->vfp + config->vbp;
    vps             = config->vsw + config->vbp;
    commandParam[0] = (vt - 1U) >> 8U;
    commandParam[1] = (vt - 1U) & 0xFFU;
    commandParam[2] = vps >> 8U;
    commandParam[3] = vps & 0xFFU;
    commandParam[4] = config->vsw - 1U;
    commandParam[5] = 0U;
    commandParam[6] = 0U;
    handle->xferOps->writeCommand(xferOpsData, SSD1963_SET_VERT_PERIOD);
    handle->xferOps->writeData(xferOpsData, commandParam, 7 * SSD1963_DATA_WITDH_BYTE);

    /* Pixel format. */
    SSD1963_SetPixelFormat(handle, config->pixelInterface);

    return kStatus_Success;
}

void SSD1963_SetMemoryDoneCallback(ssd1963_handle_t *handle, dbi_mem_done_callback_t callback, void *userData)
{
    assert(handle);

    handle->xferOps->setMemoryDoneCallback(handle->xferOpsData, callback, userData);
}

void SSD1963_Deinit(ssd1963_handle_t *handle)
{
    assert(handle);

    memset(handle, 0, sizeof(ssd1963_handle_t));
}

void SSD1963_StartDisplay(ssd1963_handle_t *handle)
{
    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_DISPLAY_ON);
}

void SSD1963_StopDisplay(ssd1963_handle_t *handle)
{
    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_DISPLAY_OFF);
}

void SSD1963_SetFlipMode(ssd1963_handle_t *handle, ssd1963_flip_mode_t mode)
{
#if (16 == SSD1963_DATA_WITDH)
    uint16_t newAddrMode;
#else
    uint8_t newAddrMode;
#endif

    newAddrMode = (handle->addrMode & ~SSD1963_FLIP_MODE_MASK) | mode;

    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_ADDRESS_MODE);
    handle->xferOps->writeData(handle->xferOpsData, &newAddrMode, 1 * SSD1963_DATA_WITDH_BYTE);

    handle->addrMode = newAddrMode;
}

void SSD1963_SetOrientationMode(ssd1963_handle_t *handle, ssd1963_orientation_mode_t mode)
{
#if (16 == SSD1963_DATA_WITDH)
    uint16_t newAddrMode;
#else
    uint8_t newAddrMode;
#endif

    newAddrMode = (handle->addrMode & ~SSD1963_ORIENTATION_MODE_MASK) | mode;

    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_ADDRESS_MODE);
    handle->xferOps->writeData(handle->xferOpsData, &newAddrMode, 1 * SSD1963_DATA_WITDH_BYTE);

    handle->addrMode = newAddrMode;
}

void SSD1963_SelectArea(ssd1963_handle_t *handle, uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY)
{
    uint16_t sc; /* Start of column number. */
    uint16_t ec; /* End of column number. */
    uint16_t sp; /* Start of page number. */
    uint16_t ep; /* End of page number. */
    ssd1963_orientation_mode_t mode;
#if (16 == SSD1963_DATA_WITDH)
    uint16_t commandParam[4]; /* Command parameters for set_page_address and set_column_address. */
#else
    uint8_t commandParam[4]; /* Command parameters for set_page_address and set_column_address. */
#endif

    mode = (ssd1963_orientation_mode_t)(handle->addrMode & SSD1963_ORIENTATION_MODE_MASK);

    switch (mode)
    {
        default:
        case kSSD1963_Orientation0:
            sp = startY;
            ep = endY;
            sc = startX;
            ec = endX;
            break;

        case kSSD1963_Orientation90:
            sp = handle->panelHeight - 1 - endX;
            ep = handle->panelHeight - 1 - startX;
            sc = startY;
            ec = endY;
            break;

        case kSSD1963_Orientation180:
            sp = handle->panelHeight - 1 - endY;
            ep = handle->panelHeight - 1 - startY;
            sc = handle->panelWidth - 1 - endX;
            ec = handle->panelWidth - 1 - startX;
            break;

        case kSSD1963_Orientation270:
            sp = startX;
            ep = endX;
            sc = handle->panelWidth - 1 - endY;
            ec = handle->panelWidth - 1 - startY;
            break;
    }

    /* Send the set_page_address command. */
    commandParam[0] = (sp & 0xFF00U) >> 8U;
    commandParam[1] = sp & 0xFFU;
    commandParam[2] = (ep & 0xFF00U) >> 8U;
    commandParam[3] = ep & 0xFFU;

    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_PAGE_ADDRESS);
    handle->xferOps->writeData(handle->xferOpsData, commandParam, 4 * SSD1963_DATA_WITDH_BYTE);

    /* Send the set_column_address command. */
    commandParam[0] = (sc & 0xFF00U) >> 8U;
    commandParam[1] = sc & 0xFFU;
    commandParam[2] = (ec & 0xFF00U) >> 8U;
    commandParam[3] = ec & 0xFFU;

    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_COLUMN_ADDRESS);
    handle->xferOps->writeData(handle->xferOpsData, commandParam, 4 * SSD1963_DATA_WITDH_BYTE);
}

#if (16 == SSD1963_DATA_WITDH)
void SSD1963_WritePixels(ssd1963_handle_t *handle, const uint16_t *pixels, uint32_t length)
{
    handle->xferOps->writeMemory(handle->xferOpsData, SSD1963_WRITE_MEMORY_START, (const uint8_t *)pixels, length * 2);
}

void SSD1963_ReadPixels(ssd1963_handle_t *handle, uint16_t *pixels, uint32_t length)
{
    handle->xferOps->readMemory(handle->xferOpsData, SSD1963_READ_MEMORY_START, (uint8_t *)pixels, length * 2);
}
#endif

void SSD1963_SetBackLight(ssd1963_handle_t *handle, uint8_t value)
{
#if (16 == SSD1963_DATA_WITDH)
    uint16_t commandParam[] = {0x06U, value, 0x01U, 0xFFU, 0x00U, 0x01U};
#else
    uint8_t commandParam[] = {0x06U, value, 0x01U, 0xFFU, 0x00U, 0x01U};
#endif

    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_PWM_CONF);
    handle->xferOps->writeData(handle->xferOpsData, commandParam, sizeof(commandParam));
}

void SSD1963_EnableTearEffect(ssd1963_handle_t *handle, bool enable)
{
    uint16_t regVal = 0;

    if (enable)
    {
        handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_TEAR_ON);
        handle->xferOps->writeData(handle->xferOpsData, &regVal, SSD1963_DATA_WITDH_BYTE);
    }
    else
    {
        handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_TEAR_OFF);
    }
}

void SSD1963_SetPixelFormat(ssd1963_handle_t *handle, ssd1963_pixel_interface_t pixelFormat)
{
#if (16 == SSD1963_DATA_WITDH)
    uint16_t commandParam[1];
#else
    uint8_t commandParam[1];
#endif

    /* Data interface. */
#if (8 == SSD1963_DATA_WITDH)
    commandParam[0] = 0;
#else
    commandParam[0] = 3;
#endif
    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_PIXEL_DATA_INTERFACE);
    handle->xferOps->writeData(handle->xferOpsData, commandParam, 1 * SSD1963_DATA_WITDH_BYTE);

    /* Address mode. */
    handle->addrMode &= ~SSD1963_ADDR_MODE_BGR;
#if (8 == SSD1963_DATA_WITDH)
    if (kSSD1963_RGB888 == pixelFormat)
    {
        handle->addrMode |= SSD1963_ADDR_MODE_BGR;
    }
#else
    if (kSSD1963_BGR565 == pixelFormat)
    {
        handle->addrMode |= SSD1963_ADDR_MODE_BGR;
    }
#endif

    commandParam[0] = handle->addrMode;
    handle->xferOps->writeCommand(handle->xferOpsData, SSD1963_SET_ADDRESS_MODE);
    handle->xferOps->writeData(handle->xferOpsData, commandParam, 1 * SSD1963_DATA_WITDH_BYTE);
}

void SSD1963_ReadMemory(ssd1963_handle_t *handle, uint8_t *data, uint32_t length)
{
    handle->xferOps->readMemory(handle->xferOpsData, SSD1963_READ_MEMORY_START, data, length);
}

void SSD1963_WriteMemory(ssd1963_handle_t *handle, const uint8_t *data, uint32_t length)
{
    handle->xferOps->writeMemory(handle->xferOpsData, SSD1963_WRITE_MEMORY_START, data, length);
}
