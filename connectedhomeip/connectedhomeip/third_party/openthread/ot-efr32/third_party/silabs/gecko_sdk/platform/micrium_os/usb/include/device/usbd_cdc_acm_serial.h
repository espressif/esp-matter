/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (CDC)
 *        Abstract Control Model (ACM) - Serial Emulation
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
 * @defgroup USBD_ACM USB Device ACM API
 * @ingroup USBD
 * @brief   USB Device ACM API
 *
 * @addtogroup USBD_ACM
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_CDC_ACM_SERIAL_H_
#define  _USBD_CDC_ACM_SERIAL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_cdc.h>
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBD_ACM_SERIAL_NBR_NONE            DEF_INT_08U_MAX_VAL

/********************************************************************************************************
 *                                           PORT SETTINGS DEFINES
 *******************************************************************************************************/

#define  USBD_ACM_SERIAL_PARITY_NONE                       0u
#define  USBD_ACM_SERIAL_PARITY_ODD                        1u
#define  USBD_ACM_SERIAL_PARITY_EVEN                       2u
#define  USBD_ACM_SERIAL_PARITY_MARK                       3u
#define  USBD_ACM_SERIAL_PARITY_SPACE                      4u

#define  USBD_ACM_SERIAL_STOP_BIT_1                        0u
#define  USBD_ACM_SERIAL_STOP_BIT_1_5                      1u
#define  USBD_ACM_SERIAL_STOP_BIT_2                        2u

/********************************************************************************************************
 *                                       LINE EVENTS FLAGS DEFINES
 *******************************************************************************************************/

#define  USBD_ACM_SERIAL_CTRL_BREAK                   DEF_BIT_00
#define  USBD_ACM_SERIAL_CTRL_RTS                     DEF_BIT_01
#define  USBD_ACM_SERIAL_CTRL_DTR                     DEF_BIT_02

#define  USBD_ACM_SERIAL_STATE_DCD                    DEF_BIT_00
#define  USBD_ACM_SERIAL_STATE_DSR                    DEF_BIT_01
#define  USBD_ACM_SERIAL_STATE_BREAK                  DEF_BIT_02
#define  USBD_ACM_SERIAL_STATE_RING                   DEF_BIT_03
#define  USBD_ACM_SERIAL_STATE_FRAMING                DEF_BIT_04
#define  USBD_ACM_SERIAL_STATE_PARITY                 DEF_BIT_05
#define  USBD_ACM_SERIAL_STATE_OVERUN                 DEF_BIT_06

/********************************************************************************************************
 *                                       CALL MANAGEMENT CAPABILITIES
 *
 * Note(s) : (1) See 'USB, Communications Class, Subclass Specification for PSTN Devices, Revision 1.2,
 *               February 9 2007', section '5.3.1 Call Management Functional Descriptor' for more details
 *               about the Call Management capabilities.
 *******************************************************************************************************/

#define  USBD_ACM_SERIAL_CALL_MGMT_DEV                DEF_BIT_00
#define  USBD_ACM_SERIAL_CALL_MGMT_DATA_CCI_DCI       DEF_BIT_01
#define  USBD_ACM_SERIAL_CALL_MGMT_DATA_OVER_DCI     (DEF_BIT_01 | DEF_BIT_00)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SUBCLASS CFG STRUCT
 * @{
 *******************************************************************************************************/

typedef struct usbd_cdc_acm_serial_init_cfg {                   // -------- CDC ACM SERIAL INIT CONFIGURATIONS --------
  CPU_SIZE_T BufAlignOctets;                                    ///< Required buffer alignment in octets.

  MEM_SEG    *MemSegPtr;                                        ///< Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG    *MemSegBufPtr;                                     ///< Ptr to mem segment to use when allocating data buf.
} USBD_CDC_ACM_SERIAL_INIT_CFG;

///< @}

/*
 ********************************************************************************************************
 *                                           LINE CODING DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_acm_serial_line_coding {
  CPU_INT32U BaudRate;
  CPU_INT08U Parity;
  CPU_INT08U StopBits;
  CPU_INT08U DataBits;
} USBD_ACM_SERIAL_LINE_CODING;

/********************************************************************************************************
 *                                   LINE CTRL CHANGE CALLBACK DATA TYPE
 *******************************************************************************************************/

typedef void (*USBD_ACM_SERIAL_LINE_CTRL_CHNGD) (CPU_INT08U subclass_nbr,
                                                 CPU_INT08U event,
                                                 CPU_INT08U event_chngd,
                                                 void       *p_arg);

/********************************************************************************************************
 *                                   LINE CODING CHANGE CALLBACK DATA TYPE
 *******************************************************************************************************/

typedef CPU_BOOLEAN (*USBD_ACM_SERIAL_LINE_CODING_CHNGD) (CPU_INT08U                  subclass_nbr,
                                                          USBD_ACM_SERIAL_LINE_CODING *p_line_coding,
                                                          void                        *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 USBD CDC ACM serial subclass dflt configurations.
extern const USBD_CDC_ACM_SERIAL_INIT_CFG USBD_CDC_ACM_SerialInitCfgDflt;

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
void USBD_ACM_SerialConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBD_ACM_SerialConfigureMemSeg(MEM_SEG *p_mem_seg,
                                    MEM_SEG *p_mem_seg_buf);
#endif

/********************************************************************************************************
 *                                               MSC FUNCTIONS
 *******************************************************************************************************/

void USBD_ACM_SerialInit(CPU_INT08U subclass_instance_qty,
                         RTOS_ERR   *p_err);

CPU_INT08U USBD_ACM_SerialAdd(CPU_INT16U line_state_interval,
                              CPU_INT16U call_mgmt_capabilities,
                              RTOS_ERR   *p_err);

CPU_BOOLEAN USBD_ACM_SerialConfigAdd(CPU_INT08U subclass_nbr,
                                     CPU_INT08U dev_nbr,
                                     CPU_INT08U cfg_nbr,
                                     RTOS_ERR   *p_err);

CPU_BOOLEAN USBD_ACM_SerialIsConn(CPU_INT08U subclass_nbr);

CPU_INT32U USBD_ACM_SerialRx(CPU_INT08U subclass_nbr,
                             CPU_INT08U *p_buf,
                             CPU_INT32U buf_len,
                             CPU_INT16U timeout,
                             RTOS_ERR   *p_err);

CPU_INT32U USBD_ACM_SerialTx(CPU_INT08U subclass_nbr,
                             CPU_INT08U *p_buf,
                             CPU_INT32U buf_len,
                             CPU_INT16U timeout,
                             RTOS_ERR   *p_err);

#if 0
CPU_INT32U USBD_ACM_SerialRxAsync(CPU_INT08U            subclass_nbr,
                                  CPU_INT08U            *p_buf,
                                  CPU_INT32U            buf_len,
                                  USBD_ACM_SERIAL_ASYNC async,
                                  void                  *p_async_arg,
                                  RTOS_ERR              *p_err);

CPU_INT32U USBD_ACM_SerialTxAsync(CPU_INT08U            subclass_nbr,
                                  CPU_INT08U            *p_buf,
                                  CPU_INT32U            buf_len,
                                  USBD_ACM_SERIAL_ASYNC async,
                                  void                  *p_async_arg,
                                  RTOS_ERR              *p_err);
#endif

CPU_INT08U USBD_ACM_SerialLineCtrlGet(CPU_INT08U subclass_nbr,
                                      RTOS_ERR   *p_err);

void USBD_ACM_SerialLineCtrlReg(CPU_INT08U                      subclass_nbr,
                                USBD_ACM_SERIAL_LINE_CTRL_CHNGD line_ctrl_chngd,
                                void                            *p_arg,
                                RTOS_ERR                        *p_err);

void USBD_ACM_SerialLineCodingGet(CPU_INT08U                  subclass_nbr,
                                  USBD_ACM_SERIAL_LINE_CODING *p_line_coding,
                                  RTOS_ERR                    *p_err);

void USBD_ACM_SerialLineCodingSet(CPU_INT08U                  subclass_nbr,
                                  USBD_ACM_SERIAL_LINE_CODING *p_line_coding,
                                  RTOS_ERR                    *p_err);

void USBD_ACM_SerialLineCodingReg(CPU_INT08U                        subclass_nbr,
                                  USBD_ACM_SERIAL_LINE_CODING_CHNGD line_coding_chngd,
                                  void                              *p_arg,
                                  RTOS_ERR                          *p_err);

void USBD_ACM_SerialLineStateSet(CPU_INT08U subclass_nbr,
                                 CPU_INT08U events,
                                 RTOS_ERR   *p_err);

void USBD_ACM_SerialLineStateClr(CPU_INT08U subclass_nbr,
                                 CPU_INT08U events,
                                 RTOS_ERR   *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
