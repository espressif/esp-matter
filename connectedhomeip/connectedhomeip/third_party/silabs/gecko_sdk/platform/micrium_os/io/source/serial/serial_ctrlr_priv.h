/***************************************************************************//**
 * @file
 * @brief IO Serial Controller Declarations
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

#ifndef  _SERIAL_CTRLR_PRIV_H_
#define  _SERIAL_CTRLR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/include/serial.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SERIAL_ABORT_DIR_RX   DEF_BIT_00
#define  SERIAL_ABORT_DIR_TX   DEF_BIT_01

#define  SERIAL_IOCTRL_CMD_LOOPBACK        1
#define  SERIAL_IOCTRL_CMD_HW_FLOW_CTRL    2
#define  SERIAL_IOCTRL_CMD_BREAK_SIGNAL    3
#define  SERIAL_IOCTRL_CMD_DTR             4
#define  SERIAL_IOCTRL_CMD_RTS             5

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ SERIAL PARITY ------------------
typedef enum serial_parity {
  SERIAL_PARITY_NONE,
  SERIAL_PARITY_ODD,
  SERIAL_PARITY_EVEN,
  SERIAL_PARITY_MARK,
  SERIAL_PARITY_SPACE
} SERIAL_PARITY;

//                                                                 ----------------- SERIAL STOP BIT -----------------
typedef enum serial_stop_bit {
  SERIAL_STOP_BIT_NONE,
  SERIAL_STOP_BIT_1,
  SERIAL_STOP_BIT_1_5,
  SERIAL_STOP_BIT_2
} SERIAL_STOP_BIT;

//                                                                 ----------------- SERIAL CFG STRUCT ----------------
typedef struct serial_cfg {
  CPU_INT32U      Baudrate;                                     // Baudrate, in bps.
  CPU_INT08U      FrameSize;                                    // Transfer frame size  (in bits).
  SERIAL_PARITY   ParityMode;                                   // Parity mode.
  SERIAL_STOP_BIT StopBit;                                      // Number of stop bits.
  CPU_INT08U      Mode;                                         // CPHA (bit 0) and CPOL (bit 1)
  CPU_BOOLEAN     LSB_First;                                    // Data output bit order
} SERIAL_CFG;

typedef struct serial_handle *  SERIAL_HANDLE;

/********************************************************************************************************
 *                                           ISR HANDLER FUNCTION
 *******************************************************************************************************/

struct serial_ctrlr_drv_api {
  //                                                               Init driver
  void (*Init)       (SERIAL_DRV    *p_ser_drv,
                      SERIAL_HANDLE handle,
                      MEM_SEG       *p_mem_seg,
                      RTOS_ERR      *p_err);

  //                                                               Starts SPI hardware
  void (*Start)      (SERIAL_DRV *p_ser_drv,
                      CPU_INT08U mode,
                      RTOS_ERR   *p_err);

  //                                                               Stops SPI hardware
  void (*Stop)       (SERIAL_DRV *p_ser_drv,
                      RTOS_ERR   *p_err);

  //                                                               Validate ctrlr supports cfg.
  CPU_BOOLEAN (*CfgChk)     (SERIAL_DRV       *p_ser_drv,
                             const SERIAL_CFG *p_serial_cfg,
                             RTOS_ERR         *p_err);

  //                                                               Configure SPI bus parameters
  void (*Cfg)        (SERIAL_DRV       *p_ser_drv,
                      const SERIAL_CFG *p_serial_cfg,
                      RTOS_ERR         *p_err);

  //                                                               Enables slave-select
  void (*SlaveSel)   (SERIAL_DRV              *p_ser_drv,
                      const SERIAL_SLAVE_INFO *p_slave_info,
                      RTOS_ERR                *p_err);

  //                                                               Disables slave-select
  void (*SlaveDesel) (SERIAL_DRV              *p_ser_drv,
                      const SERIAL_SLAVE_INFO *p_slave_info,
                      RTOS_ERR                *p_err);

  //                                                               Transfers data over the SPI bus
  void (*Rx)         (SERIAL_DRV *p_ser_drv,
                      CPU_INT08U *p_buf,
                      CPU_INT32U buf_len,
                      RTOS_ERR   *p_err);

  //                                                               Transfers data over the SPI bus
  void (*Tx)         (SERIAL_DRV       *p_ser_drv,
                      const CPU_INT08U *p_buf,
                      CPU_INT32U       buf_len,
                      RTOS_ERR         *p_err);

  //                                                               Transmits single byte 'len' times.
  void (*ByteTx)     (SERIAL_DRV *p_ser_drv,
                      CPU_INT08U byte,
                      CPU_INT32U len,
                      RTOS_ERR   *p_err);

  //                                                               Transfers data over the SPI bus
  void (*Abort)      (SERIAL_DRV *p_ser_drv,
                      CPU_INT08U dir,
                      RTOS_ERR   *p_err);

  //                                                               Handles driver-specific I/O control commands
  void (*IO_Ctrl)    (SERIAL_DRV *p_ser_drv,
                      CPU_INT32S cmd,
                      void       *p_arg,
                      RTOS_ERR   *p_err);

  //                                                               Retrieves required buffer alignment.
  CPU_SIZE_T (*ReqAlignGet)(SERIAL_DRV *p_ser_drv);
};

//                                                                 ----------------- SERIAL DRV STRUCT ----------------
struct serial_drv {
  void                       *DataPtr;

  const SERIAL_CTRLR_HW_INFO *HW_InfoPtr;
  const SERIAL_CTRLR_DRV_API *DrvAPI_Ptr;
  const SERIAL_CTRLR_BSP_API *BSP_API_Ptr;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void Serial_EventRxCmpl(SERIAL_HANDLE handle,
                        RTOS_ERR      err);

void Serial_EventTxCmpl(SERIAL_HANDLE handle,
                        RTOS_ERR      err);

#endif
