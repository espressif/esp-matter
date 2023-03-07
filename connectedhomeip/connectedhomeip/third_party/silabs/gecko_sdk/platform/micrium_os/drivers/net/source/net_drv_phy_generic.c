/***************************************************************************//**
 * @file
 * @brief Network Device Driver - Generic Ethernet Phy
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
 * @note     (1) The (R)MII interface port is assumed to be part of the host EMAC.  Therefore, (R)MII
 *               reads/writes MUST be performed through the network device API interface via calls to
 *               function pointers 'Phy_RegRd()' & 'Phy_RegWr()'.
 *
 * @note     (2) Interrupt support is Phy specific, therefore the generic Phy driver does NOT support
 *               interrupts.  However, interrupt support is easily added to the generic Phy driver &
 *               thus the ISR handler has been prototyped and & populated within the function pointer
 *               structure for example purposes.
 *
 * @note     (3) Does NOT support 1000Mbps Phy.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error Ethernet Driver requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net.h>
#include  <net/include/net_if.h>
#include  <net/include/net_if_ether.h>
#include  <net/source/tcpip/net_if_priv.h>
#include  <net/source/tcpip/net_if_ether_priv.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

#define  NET_PHY_ADDR_MAX                     31u               // 5 bit Phy address max value.

#define  NET_PHY_INIT_AUTO_NEG_RETRIES         3u               // Attempt Auto-Negotiation x times
#define  NET_PHY_INIT_RESET_RETRIES            3u               // Check for successful reset x times

/********************************************************************************************************
 *                                       REGISTER DEFINES
 *******************************************************************************************************/

//                                                                 -------------- GENERIC MII REGISTERS ---------------
#define  PHY_BMCR                           0x00u               // Basic mode control reg.
#define  PHY_BMSR                           0x01u               // Basic mode status reg.
#define  PHY_PHYSID1                        0x02u               // PHYS ID 1 reg.
#define  PHY_PHYSID2                        0x03u               // PHYS ID 2 reg.
#define  PHY_ANAR                           0x04u               // Advertisement control reg.
#define  PHY_ANLPAR                         0x05u               // Link partner ability reg.
#define  PHY_ANER                           0x06u               // Expansion reg.
#define  PHY_ANNPTR                         0x07u               // Next page transmit reg.

/********************************************************************************************************
 *                                           REGISTER BITS
 *******************************************************************************************************/

//                                                                 -------------- PHY_BMCR REGISTER BITS --------------
#define  BMCR_RESV                        0x007Fu               // Unused...
#define  BMCR_CTST                    DEF_BIT_07                // Collision test.
#define  BMCR_FULLDPLX                DEF_BIT_08                // Full duplex.
#define  BMCR_ANRESTART               DEF_BIT_09                // Auto negotiation restart.
#define  BMCR_ISOLATE                 DEF_BIT_10                // Disconnect Phy from MII.
#define  BMCR_PDOWN                   DEF_BIT_11                // Power down.
#define  BMCR_ANENABLE                DEF_BIT_12                // Enable auto negotiation.
#define  BMCR_SPEED100                DEF_BIT_13                // Select 100Mbps.
#define  BMCR_LOOPBACK                DEF_BIT_14                // TXD loopback bits.
#define  BMCR_RESET                   DEF_BIT_15                // Reset.

//                                                                 -------------- PHY_BMSR REGISTER BITS --------------
#define  BMSR_ERCAP                   DEF_BIT_00                // Ext-reg capability.
#define  BMSR_JCD                     DEF_BIT_01                // Jabber detected.
#define  BMSR_LSTATUS                 DEF_BIT_02                // Link status.
#define  BMSR_ANEGCAPABLE             DEF_BIT_03                // Able to do auto-negotiation.
#define  BMSR_RFAULT                  DEF_BIT_04                // Remote fault detected.
#define  BMSR_ANEGCOMPLETE            DEF_BIT_05                // Auto-negotiation complete.
#define  BMSR_RESV                        0x07C0u               // Unused...
#define  BMSR_10HALF                  DEF_BIT_11                // Can do 10mbps, half-duplex.
#define  BMSR_10FULL                  DEF_BIT_12                // Can do 10mbps, full-duplex.
#define  BMSR_100HALF                 DEF_BIT_13                // Can do 100mbps, half-duplex.
#define  BMSR_100FULL                 DEF_BIT_14                // Can do 100mbps, full-duplex.
#define  BMSR_100BASE4                DEF_BIT_15                // Can do 100mbps, 4k packets.

//                                                                 -------------- PHY_ANAR REGISTER BITS --------------
#define  ANAR_SLCT                        0x001Fu               // Selector bits.
#define  ANAR_CSMA                    DEF_BIT_04                // Only selector supported.
#define  ANAR_10HALF                  DEF_BIT_05                // Try for 10mbps half-duplex.
#define  ANAR_10FULL                  DEF_BIT_06                // Try for 10mbps full-duplex.
#define  ANAR_100HALF                 DEF_BIT_07                // Try for 100mbps half-duplex.
#define  ANAR_100FULL                 DEF_BIT_08                // Try for 100mbps full-duplex.
#define  ANAR_100BASE4                DEF_BIT_09                // Try for 100mbps 4k packets.
#define  ANAR_RESV                        0x1C00u               // Unused...
#define  ANAR_RFAULT                  DEF_BIT_13                // Say we can detect faults.
#define  ANAR_LPACK                   DEF_BIT_14                // Ack link partners response.
#define  ANAR_NPAGE                   DEF_BIT_15                // Next page bit.

#define  ANAR_FULL              (ANAR_100FULL  | ANAR_10FULL  | ANAR_CSMA)
#define  ANAR_ALL               (ANAR_100BASE4 | ANAR_100FULL | ANAR_10FULL | ANAR_100HALF | ANAR_10HALF)

//                                                                 ------------- PHY_ANLPAR REGISTER BITS -------------
#define  ANLPAR_SLCT                      0x001Fu               // Same as advertise selector.
#define  ANLPAR_10HALF                DEF_BIT_05                // Can do 10mbps half-duplex.
#define  ANLPAR_10FULL                DEF_BIT_06                // Can do 10mbps full-duplex.
#define  ANLPAR_100HALF               DEF_BIT_07                // Can do 100mbps half-duplex.
#define  ANLPAR_100FULL               DEF_BIT_08                // Can do 100mbps full-duplex.
#define  ANLPAR_100BASE4              DEF_BIT_09                // Can do 100mbps 4k packets.
#define  ANLPAR_RESV                      0x1C00u               // Unused...
#define  ANLPAR_RFAULT                DEF_BIT_13                // Link partner faulted.
#define  ANLPAR_LPACK                 DEF_BIT_14                // Link partner acked us.
#define  ANLPAR_NPAGE                 DEF_BIT_15                // Next page bit.

#define  ANLPAR_DUPLEX          (ANLPAR_10FULL  | ANLPAR_100FULL)
#define  ANLPAR_100             (ANLPAR_100FULL | ANLPAR_100HALF | ANLPAR_100BASE4)

//                                                                 -------------- PHY_ANER REGISTER BITS --------------
#define  ANER_NWAY                    DEF_BIT_00                // Can do N-way auto-negotiation.
#define  ANER_LCWP                    DEF_BIT_01                // Got new RX page code word.
#define  ANER_ENABLENPAGE             DEF_BIT_02                // This enables npage words.
#define  ANER_NPCAPABLE               DEF_BIT_03                // Link partner supports npage.
#define  ANER_MFAULTS                 DEF_BIT_04                // Multiple faults detected.
#define  ANER_RESV                        0xFFE0u               // Unused...

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *
 * Note(s) : (1) Physical layer driver functions may be arbitrarily named.  However, it is recommended that
 *               physical layer driver functions be named using the names provided below.  All driver function
 *               prototypes should be located within the driver C source file ('net_phy_&&&.c') & be declared
 *               as static functions to prevent name clashes with other network protocol suite physical layer
 *               drivers.
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetPhy_Init(NET_IF   *p_if,
                        RTOS_ERR *p_err);

static void NetPhy_EnDis(NET_IF      *p_if,
                         CPU_BOOLEAN en,
                         RTOS_ERR    *p_err);

static void NetPhy_LinkStateGet(NET_IF             *p_if,
                                NET_DEV_LINK_ETHER *p_link_state,
                                RTOS_ERR           *p_err);

static void NetPhy_LinkStateSet(NET_IF             *p_if,
                                NET_DEV_LINK_ETHER *p_link_state,
                                RTOS_ERR           *p_err);

static void NetPhy_AutoNegStart(NET_IF   *p_if,
                                RTOS_ERR *p_err);

static void NetPhy_AddrProbe(NET_IF   *p_if,
                             RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                   NETWORK PHYSICAL LAYER DRIVER API
 *
 * Note(s) : (1) Physical layer driver API structures are used by applications during calls to NetIF_Add().
 *               This API structure allows higher layers to call specific physical layer driver functions
 *               via function pointer instead of by name.  This enables the network protocol suite to
 *               compile & operate with multiple physical layer drivers.
 *
 *           (2) In most cases, the API structure provided below SHOULD suffice for most physical layer
 *               drivers exactly as is with the exception that the API structure's name which MUST be
 *               unique & SHOULD clearly identify the physical layer being implemented.  For example,
 *               the AMD 79C874's API structure should be named NetPhy_API_AM79C874[].
 *
 *               The API structure MUST also be externally declared in the physical layer drivers header
 *               file ('net_drv_phy.h') with the exact same name & type.
 ********************************************************************************************************
 *******************************************************************************************************/

const NET_PHY_API_ETHER NetPhy_API_Generic = {                  // Generic phy API fnct ptrs :
  .Init = &NetPhy_Init,
  .EnDis = &NetPhy_EnDis,
  .LinkStateGet = &NetPhy_LinkStateGet,
  .LinkStateSet = &NetPhy_LinkStateSet,
  .ISR_Handler = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetPhy_Init()
 *
 * @brief    Initialize Ethernet physical layer.
 *
 * @param    p_if    Pointer to interface to initialize Phy.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Assumes the MDI port as already been enabled for the Phy.
 *
 * @note     (2) Phy initialization occurs each time the interface is started.
 *               See 'net_if.c  NetIF_Start()'.
 *******************************************************************************************************/
static void NetPhy_Init(NET_IF   *p_if,
                        RTOS_ERR *p_err)
{
  NET_DEV_API_ETHER *p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  NET_PHY_CFG_ETHER *p_phy_cfg = (NET_PHY_CFG_ETHER *)p_if->Ext_Cfg;
  NET_IF_DATA_ETHER *p_if_data = (NET_IF_DATA_ETHER *)p_if->IF_Data;
  CPU_INT16U        retries = NET_PHY_INIT_RESET_RETRIES;
  CPU_INT08U        phy_addr = p_phy_cfg->BusAddr;
  CPU_INT16U        reg_val;

  if (phy_addr == NET_PHY_ADDR_AUTO) {                          // Automatic detection of Phy address enabled.
    NetPhy_AddrProbe(p_if, p_err);                              // Attempt to automatically determine Phy addr.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    phy_addr = p_if_data->Phy_Addr;
  } else {
    p_if_data->Phy_Addr = phy_addr;                             // Set Phy addr to cfg'd val.
  }

  //                                                               -------------------- RESET PHY ---------------------
  //                                                               Reset Phy.
  p_dev_api->Phy_RegWr(p_if, phy_addr, PHY_BMCR, BMCR_RESET, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Rd ctrl reg, get reset bit.
  p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMCR, &reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  reg_val &= BMCR_RESET;                                        // Mask out reset status bit.

  while ((reg_val == BMCR_RESET) && (retries > 0u)) {           // Wait for reset to complete.
    KAL_Dly(2u);

    p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMCR, &reg_val, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    reg_val &= BMCR_RESET;
    retries--;
  }

  if (retries == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
    return;
  }
}

/****************************************************************************************************//**
 *                                               NetPhy_EnDis()
 *
 * @brief    Enable/disable the Phy.
 *
 * @param    p_if    Pointer to interface to enable/disable Phy.
 *
 * @param    en      Enable option :
 *                       - DEF_ENABLED   Enable  Phy
 *                       - DEF_DISABLED  Disable Phy
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetPhy_EnDis(NET_IF      *p_if,
                         CPU_BOOLEAN en,
                         RTOS_ERR    *p_err)
{
  NET_DEV_API_ETHER *p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  NET_IF_DATA_ETHER *p_if_data = (NET_IF_DATA_ETHER *)p_if->IF_Data;
  CPU_INT08U        phy_addr = p_if_data->Phy_Addr;
  CPU_INT16U        reg_val;

  //                                                               Get cur ctrl reg val.
  p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMCR, &reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  switch (en) {
    case DEF_DISABLED:
      reg_val |= BMCR_PDOWN;                                    // Dis Phy.
      break;

    case DEF_ENABLED:
    default:
      reg_val &= ~BMCR_PDOWN;                                   // En  Phy.
      break;
  }
  //                                                               Pwr up / down the Phy.
  p_dev_api->Phy_RegWr(p_if, phy_addr, PHY_BMCR, reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           NetPhy_LinkStateGet()
 *
 * @brief    Get current Phy link state (speed & duplex).
 *
 * @param    p_if            Pointer to interface to get link state.
 *
 * @param    p_link_state    Pointer to structure that will receive the link state.
 *
 * @param    p_err           Pointer to variable  that will receive the return error code from this function.
 *
 * @note     (1) Some Phy's have the link status field latched in the BMSR register.  The link status
 *               remains low after a temporary link failure until it is read. To retrieve the current
 *               link status, BMSR must be read twice.
 *
 * @note     (2) Current link state should be obtained by calling this function through the NetIF layer.
 *               See 'net_if.c  NetIF_IO_Ctrl()'.
 *******************************************************************************************************/
static void NetPhy_LinkStateGet(NET_IF             *p_if,
                                NET_DEV_LINK_ETHER *p_link_state,
                                RTOS_ERR           *p_err)
{
  NET_DEV_API_ETHER *p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  NET_IF_DATA_ETHER *p_if_data = (NET_IF_DATA_ETHER *)p_if->IF_Data;
  CPU_INT08U        phy_addr = p_if_data->Phy_Addr;
  CPU_INT16U        reg_val;
  CPU_INT16U        link_self;
  CPU_INT16U        link_partner;

  p_link_state->Spd = NET_PHY_SPD_0;
  p_link_state->Duplex = NET_PHY_DUPLEX_UNKNOWN;

  //                                                               ------------- OBTAIN CUR LINK STATUS  --------------
  p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMSR, &link_self, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               Rd BMSR twice (see Note #1).
  p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMSR, &link_self, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((link_self & BMSR_LSTATUS) == 0u) {                       // Chk if link down.
    return;
  }
  //                                                               ------------- DETERMINE SPD AND DUPLEX -------------
  //                                                               Obtain AN settings.
  p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMCR, &reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((reg_val & BMCR_ANENABLE) == 0u) {                        // IF AN disabled.
    if ((reg_val & BMCR_SPEED100) == 0u) {                      // Determine spd.
      p_link_state->Spd = NET_PHY_SPD_10;
    } else {
      p_link_state->Spd = NET_PHY_SPD_100;
    }

    if ((reg_val & BMCR_FULLDPLX) == 0u) {                      // Determine duplex.
      p_link_state->Duplex = NET_PHY_DUPLEX_HALF;
    } else {
      p_link_state->Duplex = NET_PHY_DUPLEX_FULL;
    }
  } else {
    //                                                             AN enabled. Get self link capabilities.
    p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_ANAR, &link_self, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    //                                                             Get link partner link capabilities.
    p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_ANLPAR, &link_partner, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    link_partner &= (ANLPAR_100BASE4                            // Preserve link status bits.
                     | ANLPAR_100FULL
                     | ANLPAR_100HALF
                     | ANLPAR_10FULL
                     | ANLPAR_10HALF);

    link_self &= link_partner;                                  // Match self capabilties to partner capabilities.

    if (link_self >= ANLPAR_100FULL) {                          // Determine most likely link state.
      p_link_state->Spd = NET_PHY_SPD_100;
      p_link_state->Duplex = NET_PHY_DUPLEX_FULL;
    } else if (link_self >= ANLPAR_100HALF) {
      p_link_state->Spd = NET_PHY_SPD_100;
      p_link_state->Duplex = NET_PHY_DUPLEX_HALF;
    } else if (link_self >= ANLPAR_10FULL) {
      p_link_state->Spd = NET_PHY_SPD_10;
      p_link_state->Duplex = NET_PHY_DUPLEX_FULL;
    } else {
      p_link_state->Spd = NET_PHY_SPD_10;
      p_link_state->Duplex = NET_PHY_DUPLEX_HALF;
    }
  }

  //                                                               Link established, update MAC settings.
  p_dev_api->IO_Ctrl(p_if, NET_IF_IO_CTRL_LINK_STATE_UPDATE, p_link_state, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           NetPhy_LinkStateSet()
 *
 * @brief    Set current Phy link state (speed & duplex).
 *
 * @param    p_if            Pointer to interface to get link state.
 *
 * @param    p_link_state    Pointer to structure that will contain the desired link state.
 *
 * @param    p_err           Pointer to variable  that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetPhy_LinkStateSet(NET_IF             *p_if,
                                NET_DEV_LINK_ETHER *p_link_state,
                                RTOS_ERR           *p_err)
{
  NET_DEV_API_ETHER *p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  NET_IF_DATA_ETHER *p_if_data = (NET_IF_DATA_ETHER *)p_if->IF_Data;
  CPU_INT08U        phy_addr = p_if_data->Phy_Addr;
  CPU_INT16U        reg_val;
  CPU_INT16U        spd;
  CPU_INT08U        duplex;

  spd = p_link_state->Spd;
  duplex = p_link_state->Duplex;

  if (((spd != NET_PHY_SPD_10)                                  // Enable AN if cfg invalid or any member set to AUTO.
       && (spd != NET_PHY_SPD_100))
      || ((duplex != NET_PHY_DUPLEX_HALF)
          && (duplex != NET_PHY_DUPLEX_FULL))) {
    NetPhy_AutoNegStart(p_if, p_err);
    return;
  }

  p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMCR, &reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  reg_val &= ~BMCR_ANENABLE;                                    // Clr AN enable bit.

  switch (spd) {                                                // Set spd.
    case NET_PHY_SPD_10:
      reg_val &= ~BMCR_SPEED100;
      break;

    case NET_PHY_SPD_100:
      reg_val |= BMCR_SPEED100;
      break;

    default:
      break;
  }

  switch (duplex) {                                             // Set duplex.
    case NET_PHY_DUPLEX_HALF:
      reg_val &= ~BMCR_FULLDPLX;
      break;

    case NET_PHY_DUPLEX_FULL:
      reg_val |= BMCR_FULLDPLX;
      break;

    default:
      break;
  }
  //                                                               Cfg Phy.
  p_dev_api->Phy_RegWr(p_if, phy_addr, PHY_BMCR, reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           NetPhy_AutoNegStart()
 *
 * @brief    Start the Auto-Negotiation process.
 *
 * @param    p_if    Pointer to interface to start auto-negotiation.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetPhy_AutoNegStart(NET_IF   *p_if,
                                RTOS_ERR *p_err)
{
  NET_DEV_API_ETHER *p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  NET_IF_DATA_ETHER *p_if_data = (NET_IF_DATA_ETHER *)p_if->IF_Data;
  CPU_INT08U        phy_addr = p_if_data->Phy_Addr;
  CPU_INT16U        reg_val;

  p_dev_api->Phy_RegRd(p_if, phy_addr, PHY_BMCR, &reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  reg_val |= BMCR_ANENABLE
             | BMCR_ANRESTART;
  //                                                               Restart Auto-Negotiation.
  p_dev_api->Phy_RegWr(p_if, phy_addr, PHY_BMCR, reg_val, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           NetPhy_AddrProbe()
 *
 * @brief    Automatically detect Phy bus address.
 *
 * @param    p_if    Pointer to interface to probe.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Assumes the MDI port has already been initialized for the Phy.
 *******************************************************************************************************/
static void NetPhy_AddrProbe(NET_IF   *p_if,
                             RTOS_ERR *p_err)
{
  NET_DEV_API_ETHER *p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  NET_IF_DATA_ETHER *p_if_data = (NET_IF_DATA_ETHER *)p_if->IF_Data;
  CPU_INT16U        reg_id1;
  CPU_INT16U        reg_id2;
  CPU_INT08U        i;

  for (i = 0u; i <= NET_PHY_ADDR_MAX; i++) {
    //                                                             Get Phy ID #1 reg val.
    p_dev_api->Phy_RegRd(p_if, i, PHY_PHYSID1, &reg_id1, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      continue;
    }
    //                                                             Get Phy ID #2 reg val.
    p_dev_api->Phy_RegRd(p_if, i, PHY_PHYSID2, &reg_id2, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      continue;
    }

    if (((reg_id1 == 0x0000u) && (reg_id2 == 0x0000u))
        || ((reg_id1 == 0x3FFFu) && (reg_id2 == 0x0000u))
        || ((reg_id1 == 0x0000u) && (reg_id2 == 0x3FFFu))
        || ((reg_id1 == 0x3FFFu) && (reg_id2 == 0x3FFFu))
        || ((reg_id1 == 0xFFFFu) && (reg_id2 == 0x0000u))
        || ((reg_id1 == 0x0000u) && (reg_id2 == 0xFFFFu))
        || ((reg_id1 == 0x3FFFu) && (reg_id2 == 0xFFFFu))
        || ((reg_id1 == 0xFFFFu) && (reg_id2 == 0xFFFFu))) {
      continue;
    } else {
      break;
    }
  }

  RTOS_ASSERT_CRITICAL_ERR_SET((i < NET_PHY_ADDR_MAX), *p_err, RTOS_ERR_NOT_FOUND,; );

  p_if_data->Phy_Addr = i;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_IF_ETHER_AVAIL
