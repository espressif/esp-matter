/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (CDC) -
 *        Ethernet Emulation Model (EEM)
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
 * @defgroup USBD_CDCEEM USB Device CDC EEM API
 * @ingroup USBD
 * @brief   USB Device CDC EEM API
 *
 * @addtogroup USBD_CDCEEM
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_CDC_EEM_H_
#define  _USBD_CDC_EEM_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

#ifdef  RTOS_MODULE_NET_AVAIL
#include  <net/include/net_type.h>
#include  <net/include/net_if.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBD_CDC_EEM_HDR_LEN                              2u   // CDC EEM data header length.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   CDC EEM CLASS CONFIGURATION STRUCTURE
 * @{
 *******************************************************************************************************/

typedef struct usbd_cdc_eem_qty_cfg {
  CPU_INT08U ClassInstanceQty;                                  ///< Quantity of CDC-EEM class instances.
  CPU_INT08U ConfigQty;                                         ///< Qty of configs in which a class instance can be seen.
} USBD_CDC_EEM_QTY_CFG;

typedef struct usbd_cdc_eem_init_cfg {                          // ----------- CDC EEM INIT CONFIGURATIONS ------------
  CPU_SIZE_T BufAlignOctets;                                    ///< Buffers alignement in octets.

  CPU_INT32U RxBufLen;                                          ///< Length of receive buffer(s) in octets.
  CPU_INT08U RxBufQty;                                          ///< Qty of rx buffers per class instance (normally 1).
  CPU_INT16U EchoBufLen;                                        ///< Length of buffer used for echo requests in octets.

  MEM_SEG    *MemSegPtr;                                        ///< Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG    *MemSegBufPtr;                                     ///< Ptr to mem segment to use when allocating data buf.
} USBD_CDC_EEM_INIT_CFG;

///< @}

/*
 ********************************************************************************************************
 *                           CDC EEM CLASS INSTANCE CONFIGURATION STRUCTURE
 *******************************************************************************************************/

typedef struct usbd_cdc_eem_instance_cfg {
  CPU_INT08U RxBufQSize;                                        ///< Size of rx buffer Q.
  CPU_INT08U TxBufQSize;                                        ///< Size of tx buffer Q.
} USBD_CDC_EEM_INSTANCE_CFG;

/********************************************************************************************************
 *                                           CDC EEM DRIVER
 *******************************************************************************************************/

typedef const struct usbd_cdc_eem_drv {
  //                                                               Retrieve a Rx buffer.
  CPU_INT08U *(*RxBufGet)(CPU_INT08U class_nbr,
                          void       *p_arg,
                          CPU_INT16U *p_buf_len);

  //                                                               Signal that a rx buffer is ready.
  void (*RxBufRdy)(CPU_INT08U class_nbr,
                   void       *p_arg);

  //                                                               Free a tx buffer.
  void (*TxBufFree)(CPU_INT08U class_nbr,
                    void       *p_arg,
                    CPU_INT08U *p_buf,
                    CPU_INT16U buf_len);
} USBD_CDC_EEM_DRV;

/****************************************************************************************************//**
 *                                   CDC EEM NETWORK INTERFACE ETHER CFG
 *
 * @note     (1) This configuration structure is only available if µC/OS-Net Ethernet module is present in
 *               your project.
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_NET_AVAIL
typedef struct usbd_cdc_eem_net_if_ether_cfg {
  NET_BUF_QTY  RxBufQty;                                        ///< Nbr   of dev rx large buf data areas.
  NET_BUF_QTY  TxBufQty;                                        ///< Nbr   of dev tx large buf data areas.
  NET_BUF_SIZE TxBufSmallLen;                                   ///< Size  of dev tx small buf data areas (in octets).
  NET_BUF_QTY  TxBufSmallQty;                                   ///< Nbr   of dev tx small buf data areas.
  CPU_CHAR     HW_AddrStr[NET_IF_802x_ADDR_SIZE_STR];           ///< Ether IF's dev hw addr str.
} USBD_CDC_EEM_NET_IF_ETHER_CFG;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBD_CDC_EEM_INIT_CFG USBD_CDC_EEM_InitCfgDflt;    // USBD CDC EEM class dflt configurations.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_CDC_EEM_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBD_CDC_EEM_ConfigureRxBuf(CPU_INT08U rx_buf_qty,
                                 CPU_INT32U rx_buf_len);

void USBD_CDC_EEM_ConfigureEchoBufLen(CPU_INT16U echo_buf_len);

void USBD_CDC_EEM_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                                  MEM_SEG *p_mem_seg_buf);
#endif

/********************************************************************************************************
 *                                           CDC EEM FUNCTIONS
 *******************************************************************************************************/

void USBD_CDC_EEM_Init(USBD_CDC_EEM_QTY_CFG *p_qty_cfg,
                       RTOS_ERR             *p_err);

#ifdef  RTOS_MODULE_NET_AVAIL
CPU_INT08U USBD_CDC_EEM_NetIF_Reg(CPU_CHAR                      **p_name,
                                  USBD_CDC_EEM_NET_IF_ETHER_CFG *p_net_if_ether_cfg,
                                  RTOS_ERR                      *p_err);
#endif

CPU_INT08U USBD_CDC_EEM_Add(RTOS_ERR *p_err);

void USBD_CDC_EEM_ConfigAdd(CPU_INT08U     class_nbr,
                            CPU_INT08U     dev_nbr,
                            CPU_INT08U     config_nbr,
                            const CPU_CHAR *p_if_name,
                            RTOS_ERR       *p_err);

CPU_BOOLEAN USBD_CDC_EEM_IsConn(CPU_INT08U class_nbr);

void USBD_CDC_EEM_InstanceInit(CPU_INT08U                class_nbr,
                               USBD_CDC_EEM_INSTANCE_CFG *p_cfg,
                               USBD_CDC_EEM_DRV          *p_cdc_eem_drv,
                               void                      *p_arg,
                               RTOS_ERR                  *p_err);

void USBD_CDC_EEM_Start(CPU_INT08U class_nbr,
                        RTOS_ERR   *p_err);

void USBD_CDC_EEM_Stop(CPU_INT08U class_nbr,
                       RTOS_ERR   *p_err);

CPU_INT08U USBD_CDC_EEM_DevNbrGet(CPU_INT08U class_nbr,
                                  RTOS_ERR   *p_err);

CPU_INT08U *USBD_CDC_EEM_RxDataPktGet(CPU_INT08U  class_nbr,
                                      CPU_INT16U  *p_rx_len,
                                      CPU_BOOLEAN *p_crc_computed,
                                      RTOS_ERR    *p_err);

void USBD_CDC_EEM_TxDataPktSubmit(CPU_INT08U  class_nbr,
                                  CPU_INT08U  *p_buf,
                                  CPU_INT32U  buf_len,
                                  CPU_BOOLEAN crc_computed,
                                  RTOS_ERR    *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
