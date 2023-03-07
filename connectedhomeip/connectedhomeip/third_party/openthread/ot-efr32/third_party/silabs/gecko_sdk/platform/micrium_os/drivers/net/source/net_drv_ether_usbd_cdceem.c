/***************************************************************************//**
 * @file
 * @brief Network Device Driver - Texas Instruments - Ethernet Media Access Controller (Emac)
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
 * @note     (1) This driver requires the uC/OS-USBD stack with the CDC-EEM class.
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

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))
#error Ethernet Driver requires USB Device module. Make sure it is part of your project \
  and that RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <drivers/net/include/net_drv_ether.h>
#include  <net/include/net_if_ether.h>
#include  <net/include/net.h>
#include  <net/include/net_util.h>
#include  <net/include/net_if.h>
#include  <net/source/tcpip/net_if_priv.h>

#include  <common/include/lib_math.h>
#include  <common/include/toolchains.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_path.h>

#include  <usb/include/device/usbd_cdc_eem.h>
#include  <usb/include/device/usbd_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *
 * Note(s) : (1) Device driver functions may be arbitrarily named.  However, it is recommended that device
 *               driver functions be named using the names provided below.  All driver function prototypes
 *               should be located within the driver C source file ('net_dev_&&&.c') & be declared as
 *               static functions to prevent name clashes with other network protocol suite device drivers.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ FNCT'S COMMON TO ALL DRV'S ------------
static void NetDev_Init(NET_IF   *p_if,
                        RTOS_ERR *p_err);

static void NetDev_Start(NET_IF   *p_if,
                         RTOS_ERR *p_err);

static void NetDev_Stop(NET_IF   *p_if,
                        RTOS_ERR *p_err);

static void NetDev_Rx(NET_IF     *p_if,
                      CPU_INT08U **p_data,
                      CPU_INT16U *size,
                      RTOS_ERR   *p_err);

static void NetDev_Tx(NET_IF     *p_if,
                      CPU_INT08U *p_data,
                      CPU_INT16U size,
                      RTOS_ERR   *p_err);

static void NetDev_AddrMulticastAdd(NET_IF     *p_if,
                                    CPU_INT08U *paddr_hw,
                                    CPU_INT08U addr_hw_len,
                                    RTOS_ERR   *p_err);

static void NetDev_AddrMulticastRemove(NET_IF     *p_if,
                                       CPU_INT08U *paddr_hw,
                                       CPU_INT08U addr_hw_len,
                                       RTOS_ERR   *p_err);

static void NetDev_IO_Ctrl(NET_IF     *p_if,
                           CPU_INT08U opt,
                           void       *p_data,
                           RTOS_ERR   *p_err);

/********************************************************************************************************
 *                                       USBD CDC-EEM DRIVER FNCTS
 *******************************************************************************************************/

static CPU_INT08U *NetDev_USBD_CDCEEM_RxBufGet(CPU_INT08U class_nbr,
                                               void       *p_arg,
                                               CPU_INT16U *p_buf_len);

static void NetDev_USBD_CDCEEM_RxBufRdy(CPU_INT08U class_nbr,
                                        void       *p_arg);

static void NetDev_USBD_CDCEEM_TxBufFree(CPU_INT08U class_nbr,
                                         void       *p_arg,
                                         CPU_INT08U *p_buf,
                                         CPU_INT16U buf_len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       NETWORK DEVICE DRIVER API
 *
 * Note(s) : (1) Device driver API structures are used by applications during calls to NetIF_Add().  This
 *               API structure allows higher layers to call specific device driver functions via function
 *               pointer instead of by name.  This enables the network protocol suite to compile & operate
 *               with multiple device drivers.
 *
 *           (2) In most cases, the API structure provided below SHOULD suffice for most device drivers
 *               exactly as is with the exception that the API structure's name which MUST be unique &
 *               SHOULD clearly identify the device being implemented.  For example, the Cirrus Logic
 *               CS8900A Ethernet controller's API structure should be named NetDev_API_CS8900A[].
 *
 *               The API structure MUST also be externally declared in the device driver header file
 *               ('net_dev_&&&.h') with the exact same name & type.
 ********************************************************************************************************
 *******************************************************************************************************/

const NET_DEV_API_ETHER NetDev_API_USBD_CDCEEM = {
  NetDev_Init,                                                  // Init/add
  NetDev_Start,                                                 // Start
  NetDev_Stop,                                                  // Stop
  NetDev_Rx,                                                    // Rx
  NetDev_Tx,                                                    // Tx
  NetDev_AddrMulticastAdd,                                      // Multicast addr add
  NetDev_AddrMulticastRemove,                                   // Multicast addr remove
  DEF_NULL,                                                     // ISR handler
  NetDev_IO_Ctrl,                                               // I/O ctrl
  DEF_NULL,                                                     // Phy reg rd
  DEF_NULL                                                      // Phy reg wr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           USBD CDC EEM DRIVER
 ********************************************************************************************************
 *******************************************************************************************************/

USBD_CDC_EEM_DRV NetDev_DrvUSBD_CDC_EEM = {
  NetDev_USBD_CDCEEM_RxBufGet,
  NetDev_USBD_CDCEEM_RxBufRdy,
  NetDev_USBD_CDCEEM_TxBufFree
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetDev_Init()
 *
 * @brief    Initializes driver.
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_err   Pointer to return error code.
 *******************************************************************************************************/
static void NetDev_Init(NET_IF   *p_if,
                        RTOS_ERR *p_err)
{
  NET_DEV_CFG_ETHER         *p_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;
  CPU_INT08U                class_nbr;
  USBD_CDC_EEM_INSTANCE_CFG inst_cfg;

  RTOS_ASSERT_DBG((p_cfg->CfgExtPtr != DEF_NULL), RTOS_ERR_INVALID_CFG,; );

  class_nbr = ((NET_DEV_CFG_USBD_CDC_EEM_EXT *)p_cfg->CfgExtPtr)->ClassNbr;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               Ensure we have valid dev cfg for CDC-EEM.
  if ((p_cfg->RxBufLargeSize < NET_IF_802x_FRAME_MAX_CRC_SIZE)
      || (p_cfg->TxBufLargeSize < NET_IF_802x_FRAME_MAX_CRC_SIZE)
      || (p_cfg->TxBufIxOffset != USBD_CDC_EEM_HDR_LEN)) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }
#endif

  inst_cfg.RxBufQSize = p_cfg->RxBufLargeNbr;
  inst_cfg.TxBufQSize = p_cfg->TxBufLargeNbr + p_cfg->TxBufSmallNbr;

  USBD_CDC_EEM_InstanceInit(class_nbr,                          // Configure class instance according to net drv needs.
                            &inst_cfg,
                            &NetDev_DrvUSBD_CDC_EEM,
                            (void *)p_if,
                            p_err);
}

/****************************************************************************************************//**
 *                                               NetDev_Start()
 *
 * @brief    (1) Start network interface hardware :
 *               - (a) Initialize transmit semaphore count
 *               - (b) Initialize hardware address registers
 *               - (c) Initialize receive and transmit descriptors
 *               - (d) Clear all pending interrupt sources
 *               - (e) Enable supported interrupts
 *               - (f) Enable the transmitter and receiver
 *               - (g) Start / Enable DMA if required
 *
 * @param    p_if    Pointer to a network interface.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *******************************************************************************************************/
static void NetDev_Start(NET_IF   *p_if,
                         RTOS_ERR *p_err)
{
  CPU_INT08U        hw_addr[NET_IF_ETHER_ADDR_SIZE];
  CPU_INT16U        tx_buf_qty;
  NET_DEV_CFG_ETHER *p_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;
  CPU_INT08U        class_nbr = ((NET_DEV_CFG_USBD_CDC_EEM_EXT *)p_cfg->CfgExtPtr)->ClassNbr;
  CPU_INT08U        hw_addr_len;
  CPU_BOOLEAN       hw_addr_cfg;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  USBD_CDC_EEM_Start(class_nbr,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------------- CFG HW ADDR --------------------
  hw_addr_cfg = DEF_NO;
  //                                                               Get  app-configured IF layer HW MAC address.
  hw_addr_len = sizeof(hw_addr);
  NetIF_AddrHW_GetHandler(p_if->Nbr, &hw_addr[0u], &hw_addr_len, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    hw_addr_cfg = NetIF_AddrHW_IsValidHandler(p_if->Nbr, &hw_addr[0u], &local_err);
  }

  if (hw_addr_cfg != DEF_YES) {                                 // Else get configured HW MAC address string, if any ...
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetASCII_Str_to_MAC(p_cfg->HW_AddrStr,                      // Check if configured HW MAC address format is valid.
                        &hw_addr[0u],
                        &local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      NetIF_AddrHW_SetHandler(p_if->Nbr,                        // Check if configured HW MAC address is valid.
                              &hw_addr[0u],                     // return error if invalid.
                              sizeof(hw_addr),
                              &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
        return;
      }
    } else {
      //                                                           Else attempt to get device's automatically loaded ...
      //                                                           ... HW MAC address, if any.
      RTOS_ERR_SET(local_err, RTOS_ERR_IO);
      return;
    }
  }

  //                                                               CDC EEM class can Q all buffer.
  tx_buf_qty = p_cfg->TxBufLargeNbr + p_cfg->TxBufSmallNbr;

  NetIF_DevCfgTxRdySignal(p_if, tx_buf_qty);
}

/****************************************************************************************************//**
 *                                               NetDev_Stop()
 *
 * @brief    (1) Shutdown network interface hardware :
 *               - (a) Disable the receiver and transmitter
 *               - (b) Disable receive and transmit interrupts
 *               - (c) Clear pending interrupt requests
 *               - (d) Free ALL receive descriptors (Return ownership to hardware)
 *               - (e) Deallocate ALL transmit buffers
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_err   Pointer to return error code.
 *******************************************************************************************************/
static void NetDev_Stop(NET_IF   *p_if,
                        RTOS_ERR *p_err)
{
  CPU_INT08U class_nbr = ((NET_DEV_CFG_USBD_CDC_EEM_EXT *)(((NET_DEV_CFG_ETHER *)p_if->Dev_Cfg)->CfgExtPtr))->ClassNbr;

  USBD_CDC_EEM_Stop(class_nbr, p_err);
}

/****************************************************************************************************//**
 *                                               NetDev_Rx()
 *
 * @brief    Gets rx data packet.
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_data  Pointer to pointer to received data area. The received data
 *                   area address should be returned to the stack by dereferencing
 *                   p_data as *p_data = (address of receive data area).
 *
 * @param    size    Pointer to size. The number of bytes received should be returned
 *                   to the stack by dereferencing size as *size = (number of bytes).
 *
 * @param    p_err   Pointer to return error code.
 *******************************************************************************************************/
static void NetDev_Rx(NET_IF     *p_if,
                      CPU_INT08U **p_data,
                      CPU_INT16U *size,
                      RTOS_ERR   *p_err)
{
  CPU_INT08U class_nbr = ((NET_DEV_CFG_USBD_CDC_EEM_EXT *)(((NET_DEV_CFG_ETHER *)p_if->Dev_Cfg)->CfgExtPtr))->ClassNbr;
  CPU_INT16U buf_len;

  *p_data = USBD_CDC_EEM_RxDataPktGet(class_nbr,                // Retrieve data packet from CDC EEM class.
                                      &buf_len,
                                      DEF_NULL,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               --------------------- CLR CRC ----------------------
  buf_len -= NET_IF_802x_FRAME_CRC_SIZE;                        // Remove CRC from buf as net stack doesn't expect it.

  //                                                               ------------------- ADD PADDING --------------------
  *size = DEF_MAX(buf_len, NET_IF_802x_FRAME_MIN_SIZE);         // Add padding if necessary.

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  if (*size > buf_len) {
    Mem_Clr(&((*p_data)[buf_len]),                              // Clr buffer content if padding added.
            *size - buf_len);
  }
#endif
}

/****************************************************************************************************//**
 *                                               NetDev_Tx()
 *
 * @brief    Transmits specified data.
 *
 * @param    p_if    Pointer to the interface requiring service.
 *
 * @param    p_data  Pointer to buffer that contains data to send.
 *
 * @param    size    Size of buffer in bytes.
 *
 * @param    p_err   Pointer to return error code.
 *******************************************************************************************************/
static void NetDev_Tx(NET_IF     *p_if,
                      CPU_INT08U *p_data,
                      CPU_INT16U size,
                      RTOS_ERR   *p_err)
{
  CPU_INT08U *p_buf = p_data - USBD_CDC_EEM_HDR_LEN;
  CPU_INT08U class_nbr = ((NET_DEV_CFG_USBD_CDC_EEM_EXT *)(((NET_DEV_CFG_ETHER *)p_if->Dev_Cfg)->CfgExtPtr))->ClassNbr;

  USBD_CDC_EEM_TxDataPktSubmit(class_nbr,
                               p_buf,
                               size,
                               DEF_NO,
                               p_err);
}

/****************************************************************************************************//**
 *                                           NetDev_AddrMulticastAdd()
 *
 * @brief    Configure hardware address filtering to accept specified hardware address.
 *
 * @param    p_if        Pointer to an Ethernet network interface.
 *
 * @param    paddr_hw    Pointer to hardware address.
 *
 * @param    addr_len    Length  of hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This functionality is not supported by CDC-EEM.
 *******************************************************************************************************/
static void NetDev_AddrMulticastAdd(NET_IF     *p_if,
                                    CPU_INT08U *paddr_hw,
                                    CPU_INT08U addr_hw_len,
                                    RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_if);
  PP_UNUSED_PARAM(paddr_hw);
  PP_UNUSED_PARAM(addr_hw_len);

  //                                                               CDC-EEM unable to perform address filtering.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       NetDev_AddrMulticastRemove()
 *
 * @brief    Configure hardware address filtering to reject specified hardware address.
 *
 * @param    p_if        Pointer to an Ethernet network interface..
 *
 * @param    paddr_hw    Pointer to hardware address.
 *
 * @param    addr_len    Length  of hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This functionality is not supported by CDC-EEM.
 *******************************************************************************************************/
static void NetDev_AddrMulticastRemove(NET_IF     *p_if,
                                       CPU_INT08U *paddr_hw,
                                       CPU_INT08U addr_hw_len,
                                       RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_if);
  PP_UNUSED_PARAM(paddr_hw);
  PP_UNUSED_PARAM(addr_hw_len);

  //                                                               CDC-EEM unable to perform address filtering.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               NetDev_IO_Ctrl()
 *
 * @brief    This function provides a mechanism for the Phy driver to update the MAC link
 *           and duplex settings, as well as a method for the application and link state
 *           timer to obtain the current link status.  Additional user specified driver
 *           functionality MAY be added if necessary.
 *
 * @param    p_if    Pointer to interface requiring service.
 *
 * @param    opt     Option code representing desired function to perform. The Network Protocol Suite
 *                   specifies the option codes below. Additional option codes may be defined by the
 *                   driver developer in the driver's header file.
 *                   NET_IF_IO_CTRL_LINK_STATE_GET
 *                   NET_IF_IO_CTRL_LINK_STATE_UPDATE
 *                   Driver defined operation codes MUST be defined starting from 20 or higher
 *                   to prevent clashing with the pre-defined operation code types. See the
 *                   device driver header file for more details.
 *
 * @param    p_data  Pointer to optional data for either sending or receiving additional function
 *                   arguments or return data.
 *
 * @param    p_err   Pointer to return error code
 *******************************************************************************************************/
static void NetDev_IO_Ctrl(NET_IF     *p_if,
                           CPU_INT08U opt,
                           void       *p_data,
                           RTOS_ERR   *p_err)
{
  CPU_BOOLEAN        is_conn;
  CPU_INT08U         class_nbr = ((NET_DEV_CFG_USBD_CDC_EEM_EXT *)(((NET_DEV_CFG_ETHER *)p_if->Dev_Cfg)->CfgExtPtr))->ClassNbr;
  CPU_INT08U         dev_nbr;
  USBD_DEV_SPD       dev_spd;
  NET_DEV_LINK_ETHER *plink_state;

  switch (opt) {
    case NET_IF_IO_CTRL_LINK_STATE_GET_INFO:                    // Retrieve spd/conn state from device conn status.
      plink_state = (NET_DEV_LINK_ETHER *)p_data;

      plink_state->Spd = NET_PHY_SPD_0;
      plink_state->Duplex = NET_PHY_DUPLEX_UNKNOWN;

      is_conn = USBD_CDC_EEM_IsConn(class_nbr);
      if (is_conn != DEF_YES) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        return;
      }

      dev_nbr = USBD_CDC_EEM_DevNbrGet(class_nbr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      dev_spd = USBD_DevSpdGet(dev_nbr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      plink_state->Duplex = NET_PHY_DUPLEX_AUTO;

      switch (dev_spd) {
        case USBD_DEV_SPD_HIGH:
          plink_state->Spd = NET_PHY_SPD_1000;
          break;

        case USBD_DEV_SPD_FULL:
        case USBD_DEV_SPD_LOW:
          plink_state->Spd = NET_PHY_SPD_10;
          break;

        case USBD_DEV_SPD_INVALID:
        default:
          break;
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case NET_IF_IO_CTRL_LINK_STATE_UPDATE:
      plink_state = (NET_DEV_LINK_ETHER *)p_data;

      switch (plink_state->Duplex) {                            // Update duplex setting on device.
        case NET_PHY_DUPLEX_FULL:
        case NET_PHY_DUPLEX_HALF:
          break;

        default:
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
      }

      switch (plink_state->Spd) {                               // Update speed setting on device.
        case NET_PHY_SPD_10:
        case NET_PHY_SPD_100:
          break;

        default:
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_SUPPORTED,; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       USBD CDC EEM DRIVER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetDev_USBD_CDCEEM_RxBufGet()
 *
 * @brief    Gets a free network rx buffer.
 *
 * @param    class_nbr   CDC EEM class number.
 *
 * @param    p_arg       Pointer to application specific argument.
 *
 * @param    p_buf_len   Pointer to variable that will receive the buffer length in bytes.
 *
 * @return   Pointer to retrieved rx buffer, if successful.
 *           Null pointer,                   otherwise.
 *******************************************************************************************************/
static CPU_INT08U *NetDev_USBD_CDCEEM_RxBufGet(CPU_INT08U class_nbr,
                                               void       *p_arg,
                                               CPU_INT16U *p_buf_len)
{
  CPU_INT08U *p_buf;
  NET_IF     *p_if = (NET_IF *)p_arg;
  RTOS_ERR   local_err;

  PP_UNUSED_PARAM(class_nbr);

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_buf = NetBuf_GetDataPtr(p_if,
                            NET_TRANSACTION_RX,
                            NET_IF_ETHER_FRAME_MAX_SIZE,
                            NET_IF_IX_RX,
                            DEF_NULL,
                            p_buf_len,
                            DEF_NULL,
                            &local_err);
  PP_UNUSED_PARAM(local_err);

  return (p_buf);
}

/****************************************************************************************************//**
 *                                       NetDev_USBD_CDCEEM_RxBufRdy()
 *
 * @brief    Signals that a rx buffer is ready.
 *
 * @param    class_nbr   CDC EEM class number.
 *
 * @param    p_arg       Pointer to application specific argument.
 *******************************************************************************************************/
static void NetDev_USBD_CDCEEM_RxBufRdy(CPU_INT08U class_nbr,
                                        void       *p_arg)
{
  NET_IF   *p_if = (NET_IF *)p_arg;
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  PP_UNUSED_PARAM(class_nbr);

  NetIF_RxQPost(p_if->Nbr, &local_err);

  PP_UNUSED_PARAM(local_err);
}

/****************************************************************************************************//**
 *                                       NetDev_USBD_CDCEEM_TxBufFree()
 *
 * @brief    Signals that a tx buffer can be freed.
 *
 * @param    class_nbr   CDC EEM class number.
 *
 * @param    p_arg       Pointer to application specific argument.
 *
 * @param    p_buf       Pointer to buffer to be freed.
 *
 * @param    buf_len     Buffer length in bytes.
 *******************************************************************************************************/
static void NetDev_USBD_CDCEEM_TxBufFree(CPU_INT08U class_nbr,
                                         void       *p_arg,
                                         CPU_INT08U *p_buf,
                                         CPU_INT16U buf_len)
{
  CPU_INT08U *p_data = p_buf + USBD_CDC_EEM_HDR_LEN;
  NET_IF     *p_if = (NET_IF *)p_arg;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  PP_UNUSED_PARAM(class_nbr);
  PP_UNUSED_PARAM(buf_len);

  NetIF_TxDeallocQPost(p_data, &local_err);
  NetIF_DevTxRdySignal(p_if);

  PP_UNUSED_PARAM(local_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_IF_ETHER_AVAIL
