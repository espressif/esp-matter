/***************************************************************************//**
 * @file
 * @brief Network Ethernet BSP
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL) && defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))
#include  <net/include/net_def.h>
#include  <net/include/net_type.h>
#include  <net/include/net_if.h>
#include  <net/include/net_if_ether.h>
#include  <drivers/net/include/net_drv_ether.h>
#include  <drivers/net/include/net_drv_phy.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>

#include  <kernel/include/os.h>

#include  "em_cmu.h"
#include  "em_device.h"
#include  "em_gpio.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define BSP_ETH_PHY_MODE_CONFIG            NET_PHY_BUS_MODE_RMII

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void BSP_NetEther_CfgClk(NET_IF   *p_if,
                                RTOS_ERR *p_err);

static void BSP_NetEther_CfgIntCtrl(NET_IF   *p_if,
                                    RTOS_ERR *p_err);

static void BSP_NetEther_CfgGPIO(NET_IF   *p_if,
                                 RTOS_ERR *p_err);

static CPU_INT32U BSP_NetEther_ClkFreqGet(NET_IF   *p_if,
                                          RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                         INTERFACE NUMBER
 *
 * Note(s) : (1) (a) Each network controller maps to a unique network interface number.
 *
 *               (b) Network interface number variables SHOULD be initialized to 'NET_IF_NBR_NONE'.
 *******************************************************************************************************/

static NET_IF_NBR BSP_NetEther_IF_Nbr = NET_IF_NBR_NONE;

/********************************************************************************************************
 *                                  NETWORK CONTROLLER BSP INTERFACE
 *
 * Note(s) : (1) Board-support package (BSP) interface structures are used by the controller driver
 *               to call specific controller' BSP functions via function pointer instead of by name. This
 *               enables the network protocol suite to compile & operate with multiple instances of
 *               multiple controllers & drivers.
 *******************************************************************************************************/

const NET_DEV_BSP_ETHER BSP_NetEther_BSP_API = {
  .CfgClk = BSP_NetEther_CfgClk,
  .CfgIntCtrl = BSP_NetEther_CfgIntCtrl,
  .CfgGPIO = BSP_NetEther_CfgGPIO,
  .ClkFreqGet = BSP_NetEther_ClkFreqGet
};

/********************************************************************************************************
 *                                  NETWORK CONTROLLER CONFIGURATION
 *
 * Note(s) : (1) (a) Buffer & memory sizes & alignments configured in number of octets.
 *               (b) Data bus size configured in number of bits.
 *
 *           (2) (a) All network buffer data area sizes MUST be configured greater than or equal to
 *                   NET_BUF_DATA_SIZE_MIN.
 *               (b) Large transmit buffer data area sizes MUST be configured greater than or equal to
 *                   small transmit buffer data area sizes.
 *               (c) Small transmit buffer data area sizes MAY need to be configured greater than or
 *                   equal to the specific interface's minimum packet size.
 *
 *           (3) (a) MUST configure at least one (1) large receive  buffer.
 *               (b) MUST configure at least one (1) transmit buffer, however, zero (0) large OR
 *                   zero (0) small transmit buffers MAY be configured.
 *
 *           (4) Some processors or controllers may be more efficient & may even REQUIRE that buffer data
 *               areas align to specific CPU-word/octet address boundaries in order to successfully
 *               read/write data from/to controller. Therefore, it is recommended to align controllers'
 *               buffer data areas to the processor's or controller's data bus width.
 *
 *           (5) Positive offset from base receive/transmit index, if required by controller (or driver) :
 *
 *               (a) (1) Some controller's may receive or buffer additional octets prior to the actual
 *                       received packet. Thus an offset may be required to ignore these additional octets :
 *
 *                       (A) If a controller does NOT receive or buffer any  additional octets prior to
 *                           received packets, then the default offset of '0' SHOULD be configured.
 *
 *                       (B) However, if a controller does receive or buffer additional octets prior to
 *                           received packets, then configure the controller's receive offset with the
 *                           number of additional octets.
 *
 *                   (2) Some controllers/drivers may require additional octets prior to the actual
 *                       transmit packet. Thus an offset may be required to reserve additional octets :
 *
 *                       (A) If a controller/driver does NOT require any  additional octets prior to
 *                           transmit packets, then the default offset of '0' SHOULD be configured.
 *
 *                       (B) However, if a controller/driver does require additional octets prior to
 *                           transmit packets, then configure the controller's transmit offset with the
 *                           number of additional octets.
 *
 *               (b) Since each network buffer data area allocates additional octets for its configured
 *                   offset(s), the network buffer data area size does NOT need to be increased by the
 *                   number of additional offset octets.
 *
 *           (6) Flags to configure (optional) controller features; bit-field flags logically OR'd :
 *
 *               (a) NET_DEV_CFG_FLAG_NONE           No device configuration flags selected.
 *
 *               (b) NET_DEV_CFG_FLAG_SWAP_OCTETS    Swap data octets [i.e. swap data words' high-order
 *                                                       octet(s) with data words' low-order octet(s),
 *                                                       & vice-versa] if required by device-to-CPU data
 *                                                       bus wiring &/or CPU endian word order.
 *
 *           (7) Network controllers with receive descriptors MUST configure the number of receive buffers
 *               greater than the number of receive descriptors.
 *******************************************************************************************************/

//                                                                 Modify structure according to your application needs and controller particularities.
const NET_DEV_CFG_ETHER BSP_NetEther_CtrlrCfg = {
  .RxBufPoolType = NET_IF_MEM_TYPE_MAIN,                        // Desired receive buffer memory pool type :
                                                                // NET_IF_MEM_TYPE_MAIN : buffers allocated from main memory
                                                                // NET_IF_MEM_TYPE_DEDICATED : bufs alloc from dedicated memory
  .RxBufLargeSize = 1520u,                                      // Desired size of large receive buffers (in octets) [see Note #2].
  .RxBufLargeNbr = 10u,                                         // Desired number of large receive buffers [see Note #3a].
  .RxBufAlignOctets = 32u,                                      // Desired alignment of receive buffers (in octets) [see Note #4].
  .RxBufIxOffset = 0u,                                          // Desired offset from base receive index, if needed (in octets) [see Note #5a1].

  .TxBufPoolType = NET_IF_MEM_TYPE_MAIN,                        // Desired transmit buffer memory pool type :
                                                                // NET_IF_MEM_TYPE_MAIN : buffers allocated from main memory
                                                                // NET_IF_MEM_TYPE_DEDICATED : bufs alloc from dedicated memory
  .TxBufLargeSize = 1520u,                                      // Desired size of large transmit buffers (in octets) [see Note #2].
  .TxBufLargeNbr = 8u,                                          // Desired number of large transmit buffers [see Note #3b].
  .TxBufSmallSize = 60u,                                        // Desired size of small transmit buffers (in octets) [see Note #2].
  .TxBufSmallNbr = 4u,                                          // Desired number of small transmit buffers [see Note #3b].
  .TxBufAlignOctets = 16u,                                      // Desired alignment of transmit buffers (in octets) [see Note #4].
  .TxBufIxOffset = 0u,                                          // Desired offset from base transmit index, if needed (in octets) [see Note #5a2].

  .MemAddr = 0x00000000u,                                       // Base address of dedicated memory, if available.
  .MemSize = 0xFFFFu,                                           // Size of dedicated memory, if available (in octets).

  .Flags = NET_DEV_CFG_FLAG_NONE,                               // Desired option flags, if any (see Note #6).

  .RxDescNbr = 5u,                                              // Desired number of controller's receive descriptors (see Note #7).
  .TxDescNbr = 12u,                                             // Desired number of controller's transmit descriptors.

  .BaseAddr = ETH_BASE,                                         // Base address of controller's hardware/registers.

  .DataBusSizeNbrBits = 0u,                                     // Size of controller's data bus (in bits), if available.

  .HW_AddrStr = { DEF_NULL },                                   // Desired MAC hardware address; may be NULL address or string ...
                                                                // ... if  address configured or set at run-time.

  .CfgExtPtr = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                      NETWORK PHY CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

const NET_PHY_CFG_ETHER BSP_NetEther_PhyCfg = {
  .BusAddr = NET_PHY_ADDR_AUTO,                                 // Phy bus address.
  .BusMode = BSP_ETH_PHY_MODE_CONFIG,                           // Phy bus mode.
  .Type = NET_PHY_TYPE_EXT,                                     // Phy type.
  .Spd = NET_PHY_SPD_AUTO,                                      // Auto-Negotiation determines link speed.
  .Duplex = NET_PHY_DUPLEX_AUTO,                                // Auto-Negotiation determines link duplex.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   NETWORK CONTROLLER HARDWARE INFO
 *
 * Note(s) : (1) This structure is used to register the controller with the Platform Manager.
 *               See BSP_Init() in bsp.c
 *******************************************************************************************************/

//                                                                 Structure for registration of the network controller to the Platform Manager.
const NET_IF_ETHER_HW_INFO BSP_NetEther_HwInfo = {
  .DrvAPI_Ptr = &NetDev_API_GEM,                                // Insert ptr to the ctrlr driver API structure.
  .BSP_API_Ptr = &BSP_NetEther_BSP_API,                         // PHY driver API function pointers
  .DevCfgPtr = &BSP_NetEther_CtrlrCfg,                          // Controller driver configuration structure
  .PHY_API_Ptr = &NetPhy_API_Generic,                           // Insert ptr to the Phy driver API structure.
  .PHY_CfgPtr = &BSP_NetEther_PhyCfg                            // PHY driver configuration structure
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           BSP_NetEther_CfgClk()
 *
 * Description : Configure clocks for the specified interface/controller.
 *
 * Argument(s) : p_if        Pointer to network interface to configure.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_NetEther_CfgClk(NET_IF   *p_if,
                                RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_if);

  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ETH, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  //                                                               Make sure that HFXO is enabled and ready
  RTOS_ASSERT_CRITICAL(((CMU->STATUS & CMU_STATUS_HFXORDY) != 0), RTOS_ERR_INVALID_CFG,; );

#if (BSP_ETH_PHY_MODE_CONFIG == NET_PHY_BUS_MODE_RMII)
  //                                                               Send 50 MHz HFXO to PHY via PD10
  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 0);
  CMU->CTRL |= CMU_CTRL_CLKOUTSEL2_HFXO;
  CMU->ROUTELOC0 = CMU_ROUTELOC0_CLKOUT2LOC_LOC5;
  CMU->ROUTEPEN |= CMU_ROUTEPEN_CLKOUT2PEN;
  ETH->CTRL = ETH_CTRL_GBLCLKEN | ETH_CTRL_MIISEL_RMII;
#elif (BSP_ETH_PHY_MODE_CONFIG == NET_PHY_BUS_MODE_MII)
  //                                                               Send 25 MHz clock (HFXO/2) to PHY via PD10
  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 0);
  CMU->CTRL |= CMU_CTRL_CLKOUTSEL2_HFXODIV2Q;
  CMU->ROUTELOC0 = CMU_ROUTELOC0_CLKOUT2LOC_LOC5;
  CMU->ROUTEPEN |= CMU_ROUTEPEN_CLKOUT2PEN;
  ETH->CTRL = ETH_CTRL_GBLCLKEN | ETH_CTRL_MIISEL_MII;
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/********************************************************************************************************
 *                                         BSP_NetEther_CfgIntCtrl()
 *
 * Description : Configure interrupt controller for the specified interface/controller.
 *
 * Argument(s) : p_if        Pointer to network interface to configure.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) Phy interrupts NOT supported when using the Generic Ethernet Phy driver.
 *******************************************************************************************************/
static void BSP_NetEther_CfgIntCtrl(NET_IF   *p_if,
                                    RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  BSP_NetEther_IF_Nbr = p_if->Nbr;                              // Save the interface number for this controller.

  NVIC_EnableIRQ(ETH_IRQn);
}

/********************************************************************************************************
 *                                          BSP_NetEther_CfgGPIO()
 *
 * Description : Configure general-purpose I/O (GPIO) for the specified interface/controller.
 *
 * Argument(s) : p_if        Pointer to network interface to configure.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_NetEther_CfgGPIO(NET_IF   *p_if,
                                 RTOS_ERR *p_err)
{
  RTOS_ERR err;
  PP_UNUSED_PARAM(p_if);

#if (BSP_ETH_PHY_MODE_CONFIG == NET_PHY_BUS_MODE_RMII)
  //                                                               Setup the RMII pins
  GPIO_PinModeSet(gpioPortD, 11, gpioModeInput, 0);             // CRS_DV
  GPIO_PinModeSet(gpioPortF, 7, gpioModePushPull, 0);           // TXD0
  GPIO_PinModeSet(gpioPortF, 6, gpioModePushPull, 0);           // TXD1
  GPIO_PinModeSet(gpioPortF, 8, gpioModePushPull, 0);           // TX_EN
  GPIO_PinModeSet(gpioPortD, 9, gpioModeInput, 0);              // RXD0
  GPIO_PinModeSet(gpioPortF, 9, gpioModeInput, 0);              // RXD1
  GPIO_PinModeSet(gpioPortD, 12, gpioModeInput, 0);             // RX_ER

  //                                                               Setup route locations and enable pins
  ETH->ROUTELOC1 = (1 << _ETH_ROUTELOC1_RMIILOC_SHIFT)
                   | (1 << _ETH_ROUTELOC1_MDIOLOC_SHIFT);
  ETH->ROUTEPEN = ETH_ROUTEPEN_RMIIPEN | ETH_ROUTEPEN_MDIOPEN;
  ETH->ROUTEPEN = ETH_ROUTEPEN_RMIIPEN | ETH_ROUTEPEN_MDIOPEN;

#elif (BSP_ETH_PHY_MODE_CONFIG == NET_PHY_BUS_MODE_MII)
  //                                                               Setup the MII pins
  GPIO_PinModeSet(gpioPortB, 2, gpioModeInput, 0);              // COL
  GPIO_PinModeSet(gpioPortB, 1, gpioModeInput, 0);              // CRS
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);           // TX_ER
  GPIO_PinModeSet(gpioPortA, 4, gpioModePushPull, 0);           // TXD0
  GPIO_PinModeSet(gpioPortA, 3, gpioModePushPull, 0);           // TXD1
  GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 0);           // TXD2
  GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);           // TXD3
  GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 0);           // TX_EN
  GPIO_PinModeSet(gpioPortA, 0, gpioModeInput, 0);              // TX_CLK
  GPIO_PinModeSet(gpioPortE, 12, gpioModeInput, 0);             // RXD0
  GPIO_PinModeSet(gpioPortE, 13, gpioModeInput, 0);             // RXD1
  GPIO_PinModeSet(gpioPortE, 14, gpioModeInput, 0);             // RXD2
  GPIO_PinModeSet(gpioPortE, 15, gpioModeInput, 0);             // RXD3
  GPIO_PinModeSet(gpioPortE, 10, gpioModeInput, 0);             // RX_ER
  GPIO_PinModeSet(gpioPortA, 15, gpioModeInput, 0);             // RX_CLK
  GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 0);             // RX_DV

  //                                                               Setup route locations and enable pins
  ETH->ROUTELOC0 = (0 << _ETH_ROUTELOC0_MIICOLLOC_SHIFT)
                   | (0 << _ETH_ROUTELOC0_MIICRSLOC_SHIFT)
                   | (0 << _ETH_ROUTELOC0_MIIRXLOC_SHIFT)
                   | (0 << _ETH_ROUTELOC0_MIITXLOC_SHIFT);
  ETH->ROUTELOC1 = (1 << _ETH_ROUTELOC1_MDIOLOC_SHIFT);
  ETH->ROUTEPEN = ETH_ROUTEPEN_MIIPEN
                  | ETH_ROUTEPEN_MIIRXERPEN
                  | ETH_ROUTEPEN_MIITXERPEN
                  | ETH_ROUTEPEN_MDIOPEN;

#endif

  //                                                               Setup the MDIO pins
  GPIO_PinModeSet(gpioPortD, 13, gpioModePushPull, 0);          // MDIO
  GPIO_PinModeSet(gpioPortD, 14, gpioModePushPull, 0);          // MDC

  //                                                               Enable the PHY on the STK
  GPIO_PinModeSet(gpioPortI, 10, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortH, 7, gpioModePushPull, 1);

  /* PHY address detection is being done early in the initialization sequence
   * so we must wait for the PHY to be ready to answer to MDIO requests. */
  OSTimeDly(30, OS_OPT_TIME_DLY, &err);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/********************************************************************************************************
 *                                         BSP_NetEther_ClkFreqGet()
 *
 * Description : Get controller clock frequency.
 *
 * Argument(s) : p_if        Pointer to network interface to get clock frequency.
 *
 *               p_err       pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : MAC controller clock divider frequency (in Hz).
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_INT32U BSP_NetEther_ClkFreqGet(NET_IF   *p_if,
                                          RTOS_ERR *p_err)
{
  CPU_INT32U clk_freq;

  PP_UNUSED_PARAM(p_if);
  PP_UNUSED_PARAM(p_err);

  clk_freq = 0;
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (clk_freq);
}

/********************************************************************************************************
 *                                           ETH_IRQHandler()
 *
 * Description : BSP-level ISR handler for controller receive & transmit interrupts.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) Each controller interrupt, or set of interrupts, MUST be handled by a unique
 *                   BSP-level ISR handler which maps each specific controller interrupt to its
 *                   corresponding network interface ISR handler.
 *******************************************************************************************************/
void ETH_IRQHandler(void)
{
  RTOS_ERR err;

  OSIntEnter();
  NetIF_ISR_Handler(BSP_NetEther_IF_Nbr, NET_DEV_ISR_TYPE_UNKNOWN, &err);
  OSIntExit();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of RTOS_MODULE_NET_IF_ETHER_AVAIL
