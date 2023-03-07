/***************************************************************************//**
 * @file
 * @brief Nor Quad Spi Controller BSP
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>
#include  <em_device.h>

#if (defined(RTOS_MODULE_FS_AVAIL) && defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL) \
  && defined(_SILICON_LABS_GECKO_INTERNAL_SDID_100))            // Only GG11 has been tested with QSPI.

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>

#include  <drivers/fs/include/fs_nor_quad_spi_drv.h>
#include  <fs/include/fs_nor.h>
#include  <fs/include/fs_nor_quad_spi.h>
#include  <fs/include/fs_nor_phy_drv.h>

#include  <em_cmu.h>
#include  <em_gpio.h>
#include  <em_qspi.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_Init(FS_NOR_QUAD_SPI_ISR_HANDLE_FNCT isr_fnct,
                                           void                            *p_drv_data);

static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_ClkCfg(void);

static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_IO_Cfg(void);

static CPU_INT32U BSP_FS_NOR_QuadSPI_ClkFreqGet(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const FS_NOR_QUAD_SPI_BSP_API QuadSPI_Ctrlr_BSP_API = {
  .Init = BSP_FS_NOR_QuadSPI_Init,
  .ClkCfg = BSP_FS_NOR_QuadSPI_ClkCfg,
  .IO_Cfg = BSP_FS_NOR_QuadSPI_IO_Cfg,
  .IntCfg = DEF_NULL,
  .ChipSelEn = DEF_NULL,
  .ChipSelDis = DEF_NULL,
  .ClkFreqGet = BSP_FS_NOR_QuadSPI_ClkFreqGet
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *
 * @note     (1) QSPI is mapped to two different memory regions, one in code space and one in system
 *               space. Each region can be used both to write to and read from external flash in direct
 *               and indirect modes. Code space is recommended for code execution and system space for
 *               data storage. Possible values are:
 *
 *               QSPI0_MEM_BASE          System space
 *               QSPI0_CODE_MEM_BASE     Code   space
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_NOR_QUAD_SPI_CTRLR_INFO BSP_FS_NOR_QuadSPICtrl_HwInfo = {
  .DrvApiPtr = (FS_NOR_QUAD_SPI_DRV_API *)&FS_NOR_QuadSpiDrvAPI_Silabs_EFM32GG11,
  .BspApiPtr = (FS_NOR_QUAD_SPI_BSP_API *)&QuadSPI_Ctrlr_BSP_API,
  .BaseAddr = QSPI0_BASE,
  .AlignReq = sizeof(CPU_ALIGN),
  .FlashMemMapStartAddr = QSPI0_MEM_BASE,                       // See Note #1.
  .BusWidth = FS_NOR_SERIAL_BUS_WIDTH_SINGLE_IO
};

const FS_NOR_QUAD_SPI_HW_INFO BSP_FS_NOR_MX25R_QuadSPI_HwInfo = {
  .PartHwInfo.PhyApiPtr = (CPU_INT32U)(&FS_NOR_PHY_MX25R_API),
  .PartHwInfo.ChipSelID = 0u,
  .CtrlrHwInfoPtr = &BSP_FS_NOR_QuadSPICtrl_HwInfo
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       BSP_FS_NOR_QuadSPI_Init()
 *
 * @brief    Initializes internal resources needed by the NOR QSPI BSP.
 *
 * @param    isr_fnct    Interrupt service routine to call when an interruption from the QSPI controller
 *                       occurs.
 *
 * @param    p_drv_data  Pointer to QSPI controller driver private data. This must be passed to the
 *                       interrupt service routine 'isr_fnct'.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_Init(FS_NOR_QUAD_SPI_ISR_HANDLE_FNCT isr_fnct,
                                           void                            *p_drv_data)
{
  PP_UNUSED_PARAM(isr_fnct);                                    // QSPI driver does not use interrupt for NOR xfers.
  PP_UNUSED_PARAM(p_drv_data);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                      BSP_FS_NOR_QuadSPI_ClkCfg()
 *
 * @brief    Initializes clock(s) needed by the QSPI controller.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_ClkCfg(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);                         // Enable the GPIO clock Peripheral.
  CMU_ClockEnable(cmuClock_QSPI0, true);                        // Enable the QSPI clock Peripheral.

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                      BSP_FS_NOR_QuadSPI_IO_Cfg()
 *
 * @brief    Initializes Input/Output needed by the QSPI controller.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_IO_Cfg(void)
{
  GPIO_PinModeSet(gpioPortG,
                  9,                                            // MX25R Chip Select (CS#): pin 9.
                  gpioModePushPull,                             // MX25R Input.
                  DEF_OFF);                                     // Initial setting: line is low = chip NOT selected.
  GPIO_PinModeSet(gpioPortG,
                  0,                                            // MX25R Clock Input (SCLK): pin 0.
                  gpioModePushPull,                             // MX25R Input.
                  DEF_OFF);                                     // Initial setting: line is low.
  GPIO_PinModeSet(gpioPortG,
                  1,                                            // MX25R Serial Data Input 0 (SIO0): pin 1.
                  gpioModePushPull,                             // MX25R Input/Output.
                  DEF_OFF);                                     // Initial setting: line is low.
  GPIO_PinModeSet(gpioPortG,
                  2,                                            // MX25R Serial Data Input 1 (SIO1): pin 2.
                  gpioModePushPull,                             // MX25R Input/Output.
                  DEF_OFF);                                     // Initial setting: line is low.
  GPIO_PinModeSet(gpioPortG,
                  3,                                            // MX25R Serial Data Input 2 (SIO2): pin 3.
                  gpioModePushPull,                             // MX25R Input/Output.
                  DEF_OFF);                                     // Initial setting: line is low.
  GPIO_PinModeSet(gpioPortG,
                  4,                                            // MX25R Serial Data Input 3 (SIO3): pin 4.
                  gpioModePushPull,                             // MX25R Input/Output.
                  DEF_OFF);                                     // Initial setting: line is low.

  GPIO_PinModeSet(gpioPortG,
                  13,                                           // Board QSPI power enable: pin 13.
                  gpioModePushPull,
                  DEF_ON);                                      // Initial setting: line is high = power enabled.

  //                                                               Cfg I/O Routing Location and Pin in QSPI controller.
  QSPI0->ROUTELOC0 = QSPI_ROUTELOC0_QSPILOC_LOC2;               // Location 2 used on EFM32G11 STK.
                                                                // Enable all required pins.
  QSPI0->ROUTEPEN = (QSPI_ROUTEPEN_SCLKPEN
                     | QSPI_ROUTEPEN_CS0PEN
                     | QSPI_ROUTEPEN_DQ0PEN
                     | QSPI_ROUTEPEN_DQ1PEN
                     | QSPI_ROUTEPEN_DQ2PEN
                     | QSPI_ROUTEPEN_DQ3PEN);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                    BSP_FS_NOR_QuadSPI_ClkFreqGet()
 *
 * @brief    Get input clock frequency of Quad SPI controller.
 *
 * @return   Input clock frequency, in hertz.
 *******************************************************************************************************/
static CPU_INT32U BSP_FS_NOR_QuadSPI_ClkFreqGet(void)
{
  CPU_INT32U clk_freq;

  //                                                               Get input clock of QSPI controller.
  clk_freq = CMU_ClockFreqGet(cmuClock_QSPI0);

  return (clk_freq);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL && RTOS_MODULE_FS_STORAGE_NOR_AVAIL
