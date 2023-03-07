/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_power.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.power"
#endif

typedef struct _power_nvic_context
{
    uint32_t PriorityGroup;
    uint32_t ISER[2];
    uint8_t IP[64];
    uint8_t SHP[12];
    uint32_t ICSR;
    uint32_t VTOR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint32_t SHCSR;
    uint32_t MMFAR;
    uint32_t BFAR;
    uint32_t CPACR;
} power_nvic_context_t;

typedef struct _power_systick_context
{
    uint32_t CTRL;
    uint32_t LOAD;
} power_systick_context_t;

typedef struct _power_clock_context
{
    uint32_t CLK_SRC;
    uint32_t WLAN_CTRL;
    uint32_t SFLL_CTRL0;
    uint32_t SFLL_CTRL1;
    uint32_t AUPLL_CTRL0;
    uint32_t AUPLL_CTRL1;
    uint32_t UART_CLK_SEL;
    uint32_t GPT0_CTRL;
    uint32_t GPT1_CTRL;
    uint32_t GPT2_CTRL;
    uint32_t GPT3_CTRL;
    uint32_t PERI_CLK_SRC;
    uint32_t CAU_CLK_SEL;
    uint32_t UART_FAST_CLK_DIV;
    uint32_t UART_SLOW_CLK_DIV;
    uint32_t MCU_CORE_CLK_DIV;
    uint32_t PERI0_CLK_DIV;
    uint32_t PERI1_CLK_DIV;
    uint32_t PERI2_CLK_DIV;
    uint32_t PERI3_CTRL;
} power_clock_context_t;

typedef struct _power_flash_context
{
    /* FLASHC */
    uint32_t FCCR;
    uint32_t FCTR;
    uint32_t FCACR;
    uint32_t FAOFFR;
    /* QSPI */
    uint32_t CONF;
    uint32_t TIMING;
} power_flash_context_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
static power_nvic_context_t s_nvicContext;
static power_systick_context_t s_systickContext;
static power_clock_context_t s_clockContext;
static power_flash_context_t s_flashContext;
static uint32_t s_ioPadPwrCfg;
#if defined(__ICCARM__)
uint32_t *pm3_entryaddr;
#elif defined(__GNUC__)
__attribute__((used)) uint32_t *pm3_entryaddr;
#else
#error unsupported compiler
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void POWER_SaveNvicState(void)
{
    uint32_t i;
    uint32_t irqRegs;
    uint32_t irqNum;

    irqRegs = (SCnSCB->ICTR & SCnSCB_ICTR_INTLINESNUM_Msk) + 1;
    irqNum  = irqRegs * 32;

    assert(irqRegs <= ARRAY_SIZE(s_nvicContext.ISER));
    assert(irqNum <= ARRAY_SIZE(s_nvicContext.IP));

    s_nvicContext.PriorityGroup = NVIC_GetPriorityGrouping();

    for (i = 0; i < irqRegs; i++)
    {
        s_nvicContext.ISER[i] = NVIC->ISER[i];
    }

    for (i = 0; i < irqNum; i++)
    {
        s_nvicContext.IP[i] = NVIC->IP[i];
    }

    /* Save SCB configuration */
    s_nvicContext.ICSR  = SCB->ICSR;
    s_nvicContext.VTOR  = SCB->VTOR;
    s_nvicContext.AIRCR = SCB->AIRCR;
    s_nvicContext.SCR   = SCB->SCR;
    s_nvicContext.CCR   = SCB->CCR;

    s_nvicContext.SHCSR = SCB->SHCSR;
    s_nvicContext.MMFAR = SCB->MMFAR;
    s_nvicContext.BFAR  = SCB->BFAR;
    s_nvicContext.CPACR = SCB->CPACR;

    s_nvicContext.SHP[0]  = SCB->SHP[0];  /* MemManage */
    s_nvicContext.SHP[1]  = SCB->SHP[1];  /* BusFault */
    s_nvicContext.SHP[2]  = SCB->SHP[2];  /* UsageFault */
    s_nvicContext.SHP[7]  = SCB->SHP[7];  /* SVCall */
    s_nvicContext.SHP[8]  = SCB->SHP[8];  /* DebugMonitor */
    s_nvicContext.SHP[10] = SCB->SHP[10]; /* PendSV */
    s_nvicContext.SHP[11] = SCB->SHP[11]; /* SysTick */
}

static void POWER_RestoreNvicState(void)
{
    uint32_t i;
    uint32_t irqRegs;
    uint32_t irqNum;

    irqRegs = (SCnSCB->ICTR & SCnSCB_ICTR_INTLINESNUM_Msk) + 1;
    irqNum  = irqRegs * 32;

    NVIC_SetPriorityGrouping(s_nvicContext.PriorityGroup);

    for (i = 0; i < irqRegs; i++)
    {
        NVIC->ISER[i] = s_nvicContext.ISER[i];
    }

    for (i = 0; i < irqNum; i++)
    {
        NVIC->IP[i] = s_nvicContext.IP[i];
    }

    /* Restore SCB configuration */
    SCB->ICSR  = s_nvicContext.ICSR;
    SCB->VTOR  = s_nvicContext.VTOR;
    SCB->AIRCR = s_nvicContext.AIRCR;
    SCB->SCR   = s_nvicContext.SCR;
    SCB->CCR   = s_nvicContext.CCR;

    SCB->SHCSR = s_nvicContext.SHCSR;
    SCB->MMFAR = s_nvicContext.MMFAR;
    SCB->BFAR  = s_nvicContext.BFAR;
    SCB->CPACR = s_nvicContext.CPACR;

    SCB->SHP[0]  = s_nvicContext.SHP[0];  /* MemManage */
    SCB->SHP[1]  = s_nvicContext.SHP[1];  /* BusFault */
    SCB->SHP[2]  = s_nvicContext.SHP[2];  /* UsageFault */
    SCB->SHP[7]  = s_nvicContext.SHP[7];  /* SVCall */
    SCB->SHP[8]  = s_nvicContext.SHP[8];  /* DebugMonitor */
    SCB->SHP[10] = s_nvicContext.SHP[10]; /* PendSV */
    SCB->SHP[11] = s_nvicContext.SHP[11]; /* SysTick */
}

/**
 * @brief   Power on IO domain pad regulator.
 * @param   domain : IO domain to power on.
 */
void POWER_PowerOnVddioPad(power_vddio_t domain)
{
    if (domain == kPOWER_VddIoAon)
    {
        PMU->IO_PAD_PWR_CFG |= PMU_IO_PAD_PWR_CFG_GPIO_AON_PDB_MASK;
    }
    else
    {
        assert(domain <= kPOWER_VddIo3);
        /* Both pad regulator and IO domain powered on */
        PMU->IO_PAD_PWR_CFG |= ((1UL << (PMU_IO_PAD_PWR_CFG_GPIO0_LOW_VDDB_SHIFT + (uint32_t)domain)) |
                                (1UL << (PMU_IO_PAD_PWR_CFG_GPIO0_PDB_SHIFT + (uint32_t)domain)));
    }
}

/**
 * @brief   Power off IO domain pad regulator.
 * @param   domain : IO domain to power off.
 */
void POWER_PowerOffVddioPad(power_vddio_t domain)
{
    if (domain == kPOWER_VddIoAon)
    {
        PMU->IO_PAD_PWR_CFG &= ~PMU_IO_PAD_PWR_CFG_GPIO_AON_PDB_MASK;
    }
    else
    {
        assert(domain <= kPOWER_VddIo3);
        PMU->IO_PAD_PWR_CFG &= ~(1UL << (PMU_IO_PAD_PWR_CFG_GPIO0_PDB_SHIFT + (uint32_t)domain));
    }
}

/**
 * @brief   Set IO domain pad voltage.
 * @param   domain : IO domain.
 * @param   volt   : Voltage level to be set.
 */
void POWER_SetVddioPadVolt(power_vddio_t domain, power_vddio_volt_t volt)
{
    uint32_t regVal = (uint32_t)volt;

    if (domain == kPOWER_VddIoAon)
    {
        regVal = ((regVal >> 3U) | (regVal & 0x1U)) << PMU_IO_PAD_PWR_CFG_GPIO_AON_V25_SHIFT;
        PMU->IO_PAD_PWR_CFG =
            (PMU->IO_PAD_PWR_CFG & ~(PMU_IO_PAD_PWR_CFG_GPIO_AON_V18_MASK | PMU_IO_PAD_PWR_CFG_GPIO_AON_V25_MASK)) |
            regVal;
    }
    else
    {
        assert(domain <= kPOWER_VddIo3);

        regVal <<= (PMU_IO_PAD_PWR_CFG_GPIO0_V25_SHIFT + (uint32_t)domain);
        PMU->IO_PAD_PWR_CFG =
            (PMU->IO_PAD_PWR_CFG &
             ~(((uint32_t)PMU_IO_PAD_PWR_CFG_GPIO0_V18_MASK | (uint32_t)PMU_IO_PAD_PWR_CFG_GPIO0_V25_MASK)
               << (uint32_t)domain)) |
            regVal;
    }
}

/**
 * @brief   Attach GPIO interrupt to NVIC vector table.
 *          Since 2 GPIO pins shares 1 IRQ number. Attaching one GPIO IRQ implicits
 *          detaching the sibling GPIO IRQ.
 * @param   pin    : GPIO index.
 */
void POWER_AttachGpioIrq(uint32_t pin)
{
    uint32_t bit;
    uint32_t value;

    assert(pin < 50U);

    bit   = pin / 2U;
    value = (pin & 1U) ^ 1U;

    PMU->EXT_SEL_REG0 = (PMU->EXT_SEL_REG0 & ~(1UL << bit)) | (value << bit);
}

/**
 * @brief   Enable AON Domain Comparator.
 * @param   config : AON Comparator config data.
 */
void POWER_EnableAonComp(const power_aon_comp_config_t *config)
{
    assert(config);

    PMU->PMIP_CMP_CTRL =
        (PMU->PMIP_CMP_CTRL & ~(PMU_PMIP_CMP_CTRL_COMP_REF_SEL_MASK | PMU_PMIP_CMP_CTRL_COMP_DIFF_EN_MASK |
                                PMU_PMIP_CMP_CTRL_COMP_HYST_MASK)) |
        PMU_PMIP_CMP_CTRL_COMP_REF_SEL(config->ref) | PMU_PMIP_CMP_CTRL_COMP_DIFF_EN(config->mode) |
        PMU_PMIP_CMP_CTRL_COMP_HYST(config->hyst);
    PMU->PMIP_CMP_CTRL |= PMU_PMIP_CMP_CTRL_COMP_EN_MASK;

    while ((PMU->PMIP_CMP_CTRL & PMU_PMIP_CMP_CTRL_COMP_RDY_MASK) == 0U)
    {
    }
}

/**
 * @brief   Disable AON Domain Comparator.
 */
void POWER_DisableAonComp(void)
{
    PMU->PMIP_CMP_CTRL &= ~PMU_PMIP_CMP_CTRL_COMP_EN_MASK;
}

/**
 * @brief   Get AON Domain Comparator result.
 * @return  AON Comparator output value
 */
uint32_t POWER_GetAonCompValue(void)
{
    return (PMU->PMIP_CMP_CTRL & PMU_PMIP_CMP_CTRL_COMP_OUT_MASK) >> PMU_PMIP_CMP_CTRL_COMP_OUT_SHIFT;
}

/**
 * @brief   Set sleep mode on idle.
 * @param   mode : 0 ~ 4 stands for PM0 ~ PM4.
 */
void POWER_SetSleepMode(uint32_t mode)
{
    assert(mode <= 4U);

    if (mode == 0U)
    {
        mode = 1U; /* PM0/PM1 is same */
    }
    /* set PMU basic mode */
    PMU->PWR_MODE = PMU_PWR_MODE_PWR_MODE(mode - 1);

    /* select deepsleep or not */
    if (mode == 1U)
    {
        SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    }
    else
    {
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    }
}

/* Remove flash from continuous read mode and disable flash controller */
static void FLASHC_Disable(void)
{
    uint32_t cacheMode;

    if ((FLASHC->FCCR & FLASHC_FCCR_FLASHC_PAD_EN_MASK) != 0U)
    {
        cacheMode = FLASHC->FCCR & FLASHC_FCCR_CACHE_EN_MASK;
        FLASHC->FCCR &= ~FLASHC_FCCR_CACHE_EN_MASK;

        /* Check if continuous read mode is enabled */
        if ((FLASHC->FCCR & FLASHC_FCCR_CMD_TYPE_MASK) == FLASHC_FCCR_CMD_TYPE(7U))
        {
            FLASHC->FCSR = FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK;
            FLASHC->FCCR = (FLASHC->FCCR & ~FLASHC_FCCR_CMD_TYPE_MASK) | FLASHC_FCCR_CMD_TYPE(13U);
            while ((FLASHC->FCSR & FLASHC_FCSR_CONT_RD_MD_EXIT_DONE_MASK) == 0)
            {
            }
        }
        FLASHC->FCCR |= cacheMode;
        PMU->LOW_PWR_CTRL &= ~PMU_LOW_PWR_CTRL_CACHE_LINE_FLUSH_MASK;
        FLASHC->FCCR &= ~FLASHC_FCCR_FLASHC_PAD_EN_MASK;
    }
}

static void FLASHC_Enable(void)
{
    FLASHC->FCCR &= ~FLASHC_FCCR_CACHE_LINE_FLUSH_MASK;
    PMU->LOW_PWR_CTRL |= PMU_LOW_PWR_CTRL_CACHE_LINE_FLUSH_MASK;
}

static void FLASH_PowerDown(bool powerDown)
{
    /* Clear QSPI1 FIFO */
    QSPI->CONF |= QSPI_CONF_FIFO_FLUSH_MASK;
    while ((QSPI->CONF & QSPI_CONF_FIFO_FLUSH_MASK) != 0U)
    {
    }

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI->HDRCNT = 0x1U;
    /* Set data in counter */
    QSPI->DINCNT = 0U;

    if (powerDown)
    {
        /* Set instruction */
        QSPI->INSTR = 0xB9U;
    }
    else
    {
        /* Set instruction */
        QSPI->INSTR = 0xABU;
    }

    /* Set QSPI1 write */
    QSPI->CNTL |= QSPI_CNTL_SS_EN_MASK;
    QSPI->CONF |= QSPI_CONF_RW_EN_MASK;
    QSPI->CONF |= QSPI_CONF_XFER_START_MASK;

    /* Stop QSPI1 transfer */
    while ((QSPI->CNTL & (QSPI_CNTL_XFER_RDY_MASK | QSPI_CNTL_WFIFO_EMPTY_MASK)) !=
           (QSPI_CNTL_XFER_RDY_MASK | QSPI_CNTL_WFIFO_EMPTY_MASK))
    {
    }
    QSPI->CONF |= QSPI_CONF_XFER_STOP_MASK;
    while ((QSPI->CONF & QSPI_CONF_XFER_START_MASK) == QSPI_CONF_XFER_START_MASK)
    {
    }
    QSPI->CNTL &= ~QSPI_CNTL_SS_EN_MASK;
}

static void POWER_SwitchOffIo(uint32_t mode, uint32_t excludeIo)
{
    s_ioPadPwrCfg = PMU->IO_PAD_PWR_CFG;

    /* Turn OFF different power domains. */
    /* This is done to make wakeup from PM2
     * uisng Ext Pin 0 and Ext Pin 1.
     * Please note that PM3 wakeup does
     * not work after this change.
     */
    PMU->IO_PAD_PWR_CFG &= ~PMU_IO_PAD_PWR_CFG_GPIO_AON_PDB_MASK;
    if (mode == 2U)
    {
        PMU->IO_PAD_PWR_CFG &= ~((~excludeIo & 0xFU) << PMU_IO_PAD_PWR_CFG_GPIO0_LOW_VDDB_SHIFT);
    }
    else
    {
        PMU->IO_PAD_PWR_CFG &= ~(0xFUL << PMU_IO_PAD_PWR_CFG_GPIO0_LOW_VDDB_SHIFT);
    }
}

static void POWER_RestoreIo(void)
{
    PMU->IO_PAD_PWR_CFG = s_ioPadPwrCfg;
}

static void POWER_SaveDeviceState(uint32_t mode)
{
    s_clockContext.WLAN_CTRL   = PMU->WLAN_CTRL;
    s_clockContext.SFLL_CTRL0  = PMU->SFLL_CTRL0;
    s_clockContext.SFLL_CTRL1  = PMU->SFLL_CTRL1;
    s_clockContext.AUPLL_CTRL0 = PMU->AUPLL_CTRL0;
    s_clockContext.AUPLL_CTRL1 = PMU->AUPLL_CTRL1;
    s_clockContext.CLK_SRC     = PMU->CLK_SRC;

    if (mode == 3U)
    {
        /* Save Flash controller configuration
         * This needs to be restored on exit from PM3
         */
        s_flashContext.FCCR   = FLASHC->FCCR;
        s_flashContext.FCTR   = FLASHC->FCTR;
        s_flashContext.FCACR  = FLASHC->FCACR;
        s_flashContext.FAOFFR = FLASHC->FAOFFR;
        /* Save QSPI configuration */
        s_flashContext.CONF   = QSPI->CONF;
        s_flashContext.TIMING = QSPI->TIMING;
        /* Save clock source and div setting for peripherals */
        s_clockContext.UART_CLK_SEL      = PMU->UART_CLK_SEL;
        s_clockContext.GPT0_CTRL         = PMU->GPT0_CTRL;
        s_clockContext.GPT1_CTRL         = PMU->GPT1_CTRL;
        s_clockContext.GPT2_CTRL         = PMU->GPT2_CTRL;
        s_clockContext.GPT3_CTRL         = PMU->GPT3_CTRL;
        s_clockContext.PERI_CLK_SRC      = PMU->PERI_CLK_SRC;
        s_clockContext.CAU_CLK_SEL       = PMU->CAU_CLK_SEL;
        s_clockContext.UART_FAST_CLK_DIV = PMU->UART_FAST_CLK_DIV;
        s_clockContext.UART_SLOW_CLK_DIV = PMU->UART_SLOW_CLK_DIV;
        s_clockContext.MCU_CORE_CLK_DIV  = PMU->MCU_CORE_CLK_DIV;
        s_clockContext.PERI0_CLK_DIV     = PMU->PERI0_CLK_DIV;
        s_clockContext.PERI1_CLK_DIV     = PMU->PERI1_CLK_DIV;
        s_clockContext.PERI2_CLK_DIV     = PMU->PERI2_CLK_DIV;
        s_clockContext.PERI3_CTRL        = PMU->PERI3_CTRL;
    }
}

static void POWER_RestoreDeviceState(uint32_t mode)
{
    uint32_t refClk;

    if (mode == 3U)
    {
        /* Restore clock source and div setting for peripherals */
        PMU->UART_CLK_SEL      = s_clockContext.UART_CLK_SEL;
        PMU->GPT0_CTRL         = s_clockContext.GPT0_CTRL;
        PMU->GPT1_CTRL         = s_clockContext.GPT1_CTRL;
        PMU->GPT2_CTRL         = s_clockContext.GPT2_CTRL;
        PMU->GPT3_CTRL         = s_clockContext.GPT3_CTRL;
        PMU->PERI_CLK_SRC      = s_clockContext.PERI_CLK_SRC;
        PMU->CAU_CLK_SEL       = s_clockContext.CAU_CLK_SEL;
        PMU->UART_FAST_CLK_DIV = s_clockContext.UART_FAST_CLK_DIV;
        PMU->UART_SLOW_CLK_DIV = s_clockContext.UART_SLOW_CLK_DIV;
        PMU->MCU_CORE_CLK_DIV  = s_clockContext.MCU_CORE_CLK_DIV;
        PMU->PERI0_CLK_DIV     = s_clockContext.PERI0_CLK_DIV;
        PMU->PERI1_CLK_DIV     = s_clockContext.PERI1_CLK_DIV;
        PMU->PERI2_CLK_DIV     = s_clockContext.PERI2_CLK_DIV;
        PMU->PERI3_CTRL        = s_clockContext.PERI3_CTRL;
        /* Restore Flash controller configuration. */
        FLASHC->FCTR   = s_flashContext.FCTR;
        FLASHC->FCACR  = s_flashContext.FCACR;
        FLASHC->FAOFFR = s_flashContext.FAOFFR;
        FLASHC->FCCR   = s_flashContext.FCCR;
        /* Restore QSPI configuration */
        QSPI->CONF   = s_flashContext.CONF;
        QSPI->TIMING = s_flashContext.TIMING;
    }

    refClk         = s_clockContext.WLAN_CTRL & (7UL << PMU_WLAN_CTRL_REFCLK_SYS_REQ_SHIFT);
    PMU->WLAN_CTRL = s_clockContext.WLAN_CTRL;
    /* Wait reference clock ready */
    while ((PMU->WLAN_CTRL & (refClk << 3U)) != (refClk << 3U))
    {
    }

    PMU->SFLL_CTRL1 = s_clockContext.SFLL_CTRL1;
    PMU->SFLL_CTRL0 = s_clockContext.SFLL_CTRL0;
    if ((s_clockContext.SFLL_CTRL0 & PMU_SFLL_CTRL0_SFLL_PU_MASK) != 0U)
    {
        /* Wait PLL ready */
        while ((PMU->CLK_RDY & PMU_CLK_RDY_PLL_CLK_RDY_MASK) == 0U)
        {
        }
    }

    PMU->AUPLL_CTRL1 = s_clockContext.AUPLL_CTRL1;
    PMU->AUPLL_CTRL0 = s_clockContext.AUPLL_CTRL0;
    if ((s_clockContext.AUPLL_CTRL0 & PMU_AUPLL_CTRL0_PU_MASK) != 0U)
    {
        /* Wait PLL ready */
        while ((PMU->CLK_RDY & PMU_CLK_RDY_PLL_AUDIO_RDY_MASK) == 0U)
        {
        }
    }
    if ((s_clockContext.CLK_SRC & PMU_CLK_SRC_SYS_CLK_SEL_MASK) == (uint32_t)kCLOCK_SysClkSrcMainXtal)
    {
        PMU->CLK_SRC = PMU_CLK_SRC_SYS_CLK_SEL(kCLOCK_SysClkSrcRC32M_3);
    }
    PMU->CLK_SRC = s_clockContext.CLK_SRC;
}

/* Prepare to go to low power
 *  Change clock source to RC32M
 *   Switch off PLLs, XTAL
 *  Set Deep sleep bit in SRC register
 *  Initiate state change
 */
static void POWER_PrePowerMode(uint32_t mode, uint32_t excludeIo)
{
    assert((mode >= 1U) && (mode <= 4U));
    /* Turn off Systick to avoid interrupt
     *  when entering low power state
     */
    s_systickContext.CTRL = SysTick->CTRL;
    s_systickContext.LOAD = SysTick->LOAD;
    SysTick->CTRL         = 0;
    SysTick->LOAD         = 0;

    if (mode >= 2U)
    {
        POWER_SaveDeviceState(mode);

        if (mode == 3U)
        {
            /* On exit from PM3 bootrom jumps to address
             * in pm3_entryaddr in NVRAM.
             */
            pm3_entryaddr = (uint32_t *)0x480C0000;
            POWER_SaveNvicState();
            FLASHC_Disable();
        }

        /* Switch clock source to RC 32Mhz */
        PMU->CLK_SRC |= 1U;

        /* Disable xtal */
        PMU->WLAN_CTRL &= ~(PMU_WLAN_CTRL_REFCLK_SYS_REQ_MASK | PMU_WLAN_CTRL_REFCLK_AUD_REQ_MASK |
                            PMU_WLAN_CTRL_REFCLK_USB_REQ_MASK);

        /* Power down analog unit */
        if (mode == 2U)
        {
            PMU->ANA_GRP_CTRL0 &= ~PMU_ANA_GRP_CTRL0_PU_MASK;
        }

        /* Disable Audio PLL */
        PMU->AUPLL_CTRL0 &= ~PMU_AUPLL_CTRL0_PU_MASK;

        /* Disable System PLL */
        PMU->SFLL_CTRL0 &= ~PMU_SFLL_CTRL0_SFLL_PU_MASK;

        FLASH_PowerDown(true);

        POWER_SwitchOffIo(mode, excludeIo);
    }

    POWER_SetSleepMode(mode);
}

static void POWER_PostPowerMode(uint32_t mode)
{
    assert((mode >= 1U) && (mode <= 4U));

    POWER_SetSleepMode(1U);

    if (mode >= 2U)
    {
        if (mode == 2U)
        {
            PMU->ANA_GRP_CTRL0 |= PMU_ANA_GRP_CTRL0_PU_MASK;
        }

        POWER_RestoreIo();

        FLASH_PowerDown(false);

        POWER_RestoreDeviceState(mode);

        if (mode == 3U)
        {
            POWER_RestoreNvicState();
            FLASHC_Enable();
        }
    }

    SysTick->CTRL = s_systickContext.CTRL;
    SysTick->LOAD = s_systickContext.LOAD;
}

#if defined(__GNUC__)
void asm_mcu_pm3()
{
    /* Address: 0x480C0008 is the address in NVRAM which holds address
     * where control returns after exit from PM3*/
    /* All general purpose registers and special registers
     *  are saved by pushing them on current thread's stack
     *  (psp is being used as sp) and finally SP is saved in NVRAM location*/

    __asm volatile(
        "push {r1}\n"
        "mrs r1 , msp\n"
        "push {r1}\n"
        "mrs r1 , basepri\n"
        "push {r1}\n"
        "mrs r1 , primask\n"
        "push {r1}\n"
        "mrs r1 , faultmask\n"
        "push  {r1}\n"
        "mrs r1 ,  control\n"
        "push {r1}\n"
        "push {r0-r12}\n"
        "push {lr}\n"
        "ldr r0 , =0x480C0040\n"
        "str sp , [r0]\n"
        "ldr r0 , =pm3_entryaddr\n"
        "mov r1 , pc\n"
        "add r1 , r1 , #20\n"
        "ldr r2 , [r0]\n"
        "str r1 , [r2]\n");
    /*
     * Execute WFI to generate a state change
     * and system is in an unresponsive state
     * press wakeup key to get it out of standby
     * If time_to_standby is set to valid value
     * RTC is programmed and RTC generates
     * a wakeup signal.
     */
    __asm volatile("wfi");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    /* When system exits PM3 all registers need to be
     * restored as they are lost.
     * After exit from PM3, control register is 0
     * This indicates that msp is being used as sp
     * psp is populated with saved_sp_addr value.
     * When control register is popped, system starts using
     * psp as sp and thread stack is now accessible.
     */

    /*
     * When MCU enters PM3 all Core registers
     * r0-r12
     * lr
     * control
     * primask
     * faultmask
     * basepri
     * psp
     * msp
     * are lost (ZERO) as MCU power is tuned off
     * On wakeup from PM3, this piece of code restores
     * these registers which were saved before entry.
     * The location of saving this register was on stack
     * This stack was addressed as "sp" -> "psp".
     * On exit "control" register becomes zero.
     * Bit 1 of control register determines which register
     * of "msp" or "psp" is mapped to "sp".
     * If "control" register bit 1 is ZERO then use "msp"
     * mapped to "sp" and restore the registers by popping them.
     * If "control" register bit 1 is not ZERO then use "psp"
     * mapped to "sp" and restore the registers by popping them.
     *
     */
    __asm volatile(
        "mrs r0 , control\n"
        "and r0 , #0x2\n"
        "cmp r0 , #0x2\n"
        "beq.w psp_lbl\n"
        "ldr r0 , =0x480C0040\n"
        "ldr sp , [r0]\n"
        "pop {lr}\n"
        "ldr r1 , [r0]\n"
        "msr psp , r1\n"
        "pop { r0-r12}\n"
        "mov r1 , sp\n"
        "add r1 , r1, #4\n"
        "msr psp , r1\n"
        "pop {r1}\n"
        "msr control , r1\n"
        "pop {r1}\n"
        "msr faultmask , r1\n"
        "pop {r1}\n"
        "msr primask , r1\n"
        "pop {r1}\n"
        "msr basepri , r1\n"
        "pop {r1}\n"
        "msr msp , r1\n"
        "pop {r1}\n"
        "b.w exit_lbl\n"
        "psp_lbl:\n"
        "pop {lr}\n"
        "pop {r0-r12}\n"
        "pop {r1}\n"
        "pop {r1}\n"
        "msr faultmask , r1\n"
        "pop {r1}\n"
        "msr primask , r1\n"
        "pop {r1}\n"
        "msr basepri , r1\n"
        "pop {r1}\n"
        "msr msp , r1\n"
        "pop {r1}\n"
        "exit_lbl:\n");
    /* In the assembly code above adding 4 in r1 is done to
     * populate "psp" before "control" register is popped.
     * This is needed as "control" register has bit 1 set to 1
     * and it indicates that "sp" will be mapped to "psp" after
     * the instruction that pops "control" register.
     */
}
#elif defined(__ICCARM__)
void asm_mcu_pm3()
{
    __asm volatile("push {r0-r12}");
    /* Address: 0x480C0008 is the address in NVRAM which holds address
     * where control returns after exit from PM3*/
    /* All general purpose registers and special registers
     *  are saved by pushing them on current thread's stack
     *  (psp is being used as sp) and finally SP is saved in NVRAM location*/

    /* Important Note:
     * IAR tool chain does not allow following syntax
     *
     * LDR Rx, =variable_name
     * or
     * LDR Ry, =0x10000000
     *
     * To overcome this issue a workaround is done to
     * Load 0x480C0040 in a register using
     * following instructions
     *
     * MOV, LSL and ADD
     *
     * 0x480C0040 will hold Stack Pointer
     * before entering PM3
     */

    __asm volatile("mov r5, %0\n" : : "r"(&pm3_entryaddr));

    __asm volatile(
        "push {r1}\n"
        "mrs r1 , msp\n"
        "push {r1}\n"
        "mrs r1 , basepri\n"
        "push {r1}\n"
        "mrs r1 , primask\n"
        "push {r1}\n"
        "mrs r1 , faultmask\n"
        "push  {r1}\n"
        "mrs r1 ,  control\n"
        "push {r1}\n"
        "push {r0-r12}\n"
        "push {lr}\n"
        "mov r0 , #0x480C\n"
        "lsl r0 , r0 , #16\n"
        "add r0 , r0 , #0x0040\n"
        "str sp , [r0]\n"
        "mov r0 , r5\n"
        "mov r1 , pc\n"
        "add r1 , r1 , #20\n"
        "ldr r2 , [r0]\n"
        "str r1 , [r2]\n");
    /*
     * Execute WFI to generate a state change
     * and system is in an unresponsive state
     * press wakeup key to get it out of standby
     * If time_to_standby is set to valid value
     * RTC is programmed and RTC generates
     * a wakeup signal.
     */
    __asm volatile("wfi");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    /* When system exits PM3 all registers need to be
     * restored as they are lost.
     * After exit from PM3, control register is 0
     * This indicates that msp is being used as sp
     * psp is populated with saved_sp_addr value.
     * When control register is popped, system starts using
     * psp as sp and thread stack is now accessible.
     */

    /*
     * When MCU enters PM3 all Core registers
     * r0-r12
     * lr
     * control
     * primask
     * faultmask
     * basepri
     * psp
     * msp
     * are lost (ZERO) as MCU power is tuned off
     * On wakeup from PM3, this piece of code restores
     * these registers which were saved before entry.
     * The location of saving this register was on stack
     * This stack was addressed as "sp" -> "psp".
     * On exit "control" register becomes zero.
     * Bit 1 of control register determines which register
     * of "msp" or "psp" is mapped to "sp".
     * If "control" register bit 1 is ZERO then use "msp"
     * mapped to "sp" and restore the registers by popping them.
     * If "control" register bit 1 is not ZERO then use "psp"
     * mapped to "sp" and restore the registers by popping them.
     *
     */
    __asm volatile(
        "mrs r0 , control\n"
        "and r0 , r0 , #0x2\n"
        "cmp r0 , #0x2\n"
        "beq.w psp_lbl\n"
        "mov r0 , #0x480C\n"
        "lsl r0 , r0 , #16\n"
        "add r0 , r0 , #0x0040\n"
        "ldr sp , [r0]\n"
        "pop {lr}\n"
        "ldr r1 , [r0]\n"
        "msr psp , r1\n"
        "pop { r0-r12}\n"
        "mov r1 , sp\n"
        "add r1 , r1, #4\n"
        "msr psp , r1\n"
        "pop {r1}\n"
        "msr control , r1\n"
        "pop {r1}\n"
        "msr faultmask , r1\n"
        "pop {r1}\n"
        "msr primask , r1\n"
        "pop {r1}\n"
        "msr basepri , r1\n"
        "pop {r1}\n"
        "msr msp , r1\n"
        "pop {r1}\n"
        "b.w exit_lbl\n"
        "psp_lbl:\n"
        "pop {lr}\n"
        "pop {r0-r12}\n"
        "pop {r1}\n"
        "pop {r1}\n"
        "msr faultmask , r1\n"
        "pop {r1}\n"
        "msr primask , r1\n"
        "pop {r1}\n"
        "msr basepri , r1\n"
        "pop {r1}\n"
        "msr msp , r1\n"
        "pop {r1}\n"
        "exit_lbl:\n");
    __asm volatile("pop {r0-r12}");
    /* In the assembly code above adding 4 in r1 is done to
     * populate "psp" before "control" register is popped.
     * This is needed as "control" register has bit 1 set to 1
     * and it indicates that "sp" will be mapped to "psp" after
     * the instruction that pops "control" register.
     */
}
#else
#error compiler not supported
#endif

static void ps_wait(int cycle)
{
    __asm volatile(
        "loop:\n"
        "nop\n"
        "SUB r0 , r0 , #1\n"
        "CMP r0 , #0\n"
        "BNE.W loop\n");
}

void POWER_EnterPowerMode(uint32_t mode, uint32_t excludeIo)
{
    assert(mode <= 4U);

    if (mode >= 1U)
    {
        POWER_PrePowerMode(mode, excludeIo);
        if (mode >= 2U)
        {
            ps_wait(10);
        }
        if (mode == 3U)
        {
            asm_mcu_pm3();
        }
        else
        {
            __WFI();
        }
        POWER_PostPowerMode(mode);
    }
}

void POWER_TrimBandgapRefVoltage(power_bandgap_ref_voltage_t refVoltage)
{
    uint32_t tmp32;

    tmp32 = BG->CTRL;
    tmp32 &= ~BG_CTRL_RES_TRIM_MASK;
    tmp32 |= BG_CTRL_RES_TRIM(refVoltage);
    BG->CTRL = tmp32;
}
