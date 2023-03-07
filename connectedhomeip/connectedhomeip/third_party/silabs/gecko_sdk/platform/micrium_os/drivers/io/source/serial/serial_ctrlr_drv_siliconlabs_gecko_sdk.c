/***************************************************************************//**
 * @file
 * @brief IO - Serial Ctrlr Driver - Silicon Labs Gecko Sdk
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_IO_SERIAL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_def.h>
#include  <common/include/toolchains.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/include/rtos_opt_def.h>

#include  <drivers/io/include/serial_drv.h>
#include  <io/include/serial.h>
#include  <io/source/serial/serial_ctrlr_priv.h>

#include  <spidrv.h>

#define  RTOS_MODULE_CUR            RTOS_CFG_MODULE_IO

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------- DRIVER DATA --------------------
typedef struct serial_ctrlr_drv_data {
  SLIST_MEMBER        ListNode;                                 // Serial ctrlr drv data list node.

  SERIAL_HANDLE       SerHandle;                                // Serial ctrlr handle.
  CPU_INT08U          *RxBufPtr;                                // Ptr to app rx buffer.

  SPIDRV_HandleData_t Handle;                                   // Handle on SPI driver handle.
  SPIDRV_Init_t       InitCfg;                                  // Init cfgs for SPI drv.
} SERIAL_CTRLR_DRV_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SLIST_MEMBER *Serial_CtrlrDrvDataListHeadPtr = (SLIST_MEMBER *)-1u;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Serial_CtrlrDrvCallback(struct SPIDRV_HandleData *handle,
                                    Ecode_t                  transfer_status,
                                    int                      items_transferred);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           DRIVER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

static void Serial_CtrlrDrvInit(SERIAL_DRV    *p_ser_drv,
                                SERIAL_HANDLE handle,
                                MEM_SEG       *p_mem_seg,
                                RTOS_ERR      *p_err);

static void Serial_CtrlrDrvStart(SERIAL_DRV *p_ser_drv,
                                 CPU_INT08U mode,
                                 RTOS_ERR   *p_err);

static void Serial_CtrlrDrvStop(SERIAL_DRV *p_ser_drv,
                                RTOS_ERR   *p_err);

static CPU_BOOLEAN Serial_CtrlrDrvCfgChk(SERIAL_DRV       *p_ser_drv,
                                         const SERIAL_CFG *p_serial_cfg,
                                         RTOS_ERR         *p_err);

static void Serial_CtrlrDrvCfg(SERIAL_DRV       *p_ser_drv,
                               const SERIAL_CFG *p_serial_cfg,
                               RTOS_ERR         *p_err);

static void Serial_CtrlrDrvSlaveSel(SERIAL_DRV              *p_ser_drv,
                                    const SERIAL_SLAVE_INFO *p_slave_info,
                                    RTOS_ERR                *p_err);

static void Serial_CtrlrDrvSlaveDesel(SERIAL_DRV              *p_ser_drv,
                                      const SERIAL_SLAVE_INFO *p_slave_info,
                                      RTOS_ERR                *p_err);

static void Serial_CtrlrDrvRx(SERIAL_DRV *p_ser_drv,
                              CPU_INT08U *p_buf,
                              CPU_INT32U buf_len,
                              RTOS_ERR   *p_err);

static void Serial_CtrlrDrvTx(SERIAL_DRV       *p_ser_drv,
                              const CPU_INT08U *p_buf,
                              CPU_INT32U       buf_len,
                              RTOS_ERR         *p_err);

static void Serial_CtrlrDrvByteTx(SERIAL_DRV *p_ser_drv,
                                  CPU_INT08U byte,
                                  CPU_INT32U len,
                                  RTOS_ERR   *p_err);

static void Serial_CtrlrDrvAbort(SERIAL_DRV *p_ser_drv,
                                 CPU_INT08U dir,
                                 RTOS_ERR   *p_err);

static void Serial_CtrlrDrvIO_Ctrl(SERIAL_DRV *p_ser_drv,
                                   CPU_INT32S cmd,
                                   void       *p_arg,
                                   RTOS_ERR   *p_err);

static CPU_SIZE_T Serial_CtrlrDrvReqAlignGet(SERIAL_DRV *p_ser_drv);

/********************************************************************************************************
 *                                               API STRUCTURE
 *******************************************************************************************************/

const SERIAL_CTRLR_DRV_API Serial_CtrlrDrv_API_SiliconLabsGeckoSDK = {
  Serial_CtrlrDrvInit,
  Serial_CtrlrDrvStart,
  Serial_CtrlrDrvStop,
  Serial_CtrlrDrvCfgChk,
  Serial_CtrlrDrvCfg,
  Serial_CtrlrDrvSlaveSel,
  Serial_CtrlrDrvSlaveDesel,
  Serial_CtrlrDrvRx,
  Serial_CtrlrDrvTx,
  Serial_CtrlrDrvByteTx,
  Serial_CtrlrDrvAbort,
  Serial_CtrlrDrvIO_Ctrl,
  Serial_CtrlrDrvReqAlignGet
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION DEFINITIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvInit()
 *
 * @brief    Initializes serial bus driver.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    handle      Handle to serial controller.
 *
 * @param    p_mem_seg   Pointer to memory segment to use for data allocation.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INIT
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
static void Serial_CtrlrDrvInit(SERIAL_DRV    *p_ser_drv,
                                SERIAL_HANDLE handle,
                                MEM_SEG       *p_mem_seg,
                                RTOS_ERR      *p_err)
{
  CPU_BOOLEAN                success;
  const SERIAL_CTRLR_BSP_API *p_bsp_api = p_ser_drv->BSP_API_Ptr;
  SERIAL_CTRLR_DRV_DATA      *p_drv_data;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_CRITICAL(p_ser_drv->HW_InfoPtr->InfoExtPtr != DEF_NULL, RTOS_ERR_INVALID_CFG,; );

  CORE_ENTER_ATOMIC();
  if (Serial_CtrlrDrvDataListHeadPtr == (SLIST_MEMBER *)-1u) {
    SList_Init(&Serial_CtrlrDrvDataListHeadPtr);
  }
  CORE_EXIT_ATOMIC();

  p_drv_data = (SERIAL_CTRLR_DRV_DATA *)Mem_SegAlloc("IO - Silicon Labs Gecko SDK drv data",
                                                     p_mem_seg,
                                                     sizeof(SERIAL_CTRLR_DRV_DATA),
                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_drv_data->SerHandle = handle;

  //                                                               Initialize the BSP
  if ((p_bsp_api != DEF_NULL)
      && (p_bsp_api->Init != DEF_NULL)) {
    success = p_bsp_api->Init(DEF_NULL, p_ser_drv);
    if (!success) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
      return;
    }
  }

  p_ser_drv->DataPtr = (void *)p_drv_data;

  SList_Push(&Serial_CtrlrDrvDataListHeadPtr,
             &p_drv_data->ListNode);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvStart()
 *
 * @brief    Starts serial controller.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    mode        Mode in which the serial controller should be started.
 *                       SERIAL_CTRLR_MODE_SPI (only mode supported)
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/
static void Serial_CtrlrDrvStart(SERIAL_DRV *p_ser_drv,
                                 CPU_INT08U mode,
                                 RTOS_ERR   *p_err)
{
  const SERIAL_CTRLR_BSP_API           *p_bsp_api = p_ser_drv->BSP_API_Ptr;
  SERIAL_CTRLR_DRV_DATA                *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;
  SERIAL_CTRLR_HW_INFO_EXT_SILICONLABS *p_hw_info_silabs = (SERIAL_CTRLR_HW_INFO_EXT_SILICONLABS *)p_ser_drv->HW_InfoPtr->InfoExtPtr;

  RTOS_ASSERT_CRITICAL(mode == SERIAL_CTRLR_MODE_SPI, RTOS_ERR_NOT_SUPPORTED,; );

  if (p_bsp_api != DEF_NULL) {
    CPU_BOOLEAN success;

    if (p_bsp_api->PwrCfg() != DEF_NULL) {
      success = p_bsp_api->PwrCfg();
      if (!success) {
        goto end_err_io;
      }
    }

    if (p_bsp_api->IO_Cfg() != DEF_NULL) {
      success = p_bsp_api->IO_Cfg();
      if (!success) {
        goto end_err_io;
      }
    }

    if (p_bsp_api->ClkEn() != DEF_NULL) {
      success = p_bsp_api->ClkEn();
      if (!success) {
        goto end_err_io;
      }
    }

    if (p_bsp_api->IntCfg() != DEF_NULL) {
      success = p_bsp_api->IntCfg();
      if (!success) {
        goto end_err_io;
      }
    }

    if (p_bsp_api->Start() != DEF_NULL) {
      success = p_bsp_api->Start();
      if (!success) {
        goto end_err_io;
      }
    }
  }

  p_drv_data->RxBufPtr = DEF_NULL;

#if (_SILICON_LABS_32B_SERIES < 2)
#if defined(_USART_ROUTELOC0_MASK)
  p_drv_data->InitCfg.portLocationTx = p_hw_info_silabs->PortLocationTx;
  p_drv_data->InitCfg.portLocationRx = p_hw_info_silabs->PortLocationRx;
  p_drv_data->InitCfg.portLocationClk = p_hw_info_silabs->PortLocationClk;
  p_drv_data->InitCfg.portLocationCs = p_hw_info_silabs->PortLocationCs;
#else
  RTOS_ASSERT_CRITICAL(((p_hw_info_silabs->PortLocationTx == p_hw_info_silabs->PortLocationRx)
                        && (p_hw_info_silabs->PortLocationRx == p_hw_info_silabs->PortLocationClk)
                        && (p_hw_info_silabs->PortLocationClk == p_hw_info_silabs->PortLocationCs)),
                       RTOS_ERR_INVALID_CFG,; );

  p_drv_data->InitCfg.portLocation = p_hw_info_silabs->PortLocationTx;
#endif
#endif

  p_drv_data->InitCfg.bitOrder = spidrvBitOrderMsbFirst;
  p_drv_data->InitCfg.bitRate = 400000u;
  p_drv_data->InitCfg.clockMode = spidrvClockMode3;
  p_drv_data->InitCfg.csControl = spidrvCsControlApplication;
  p_drv_data->InitCfg.dummyTxValue = 0xFFFFFFFFu;
  p_drv_data->InitCfg.frameLength = 8u;
  p_drv_data->InitCfg.port = (USART_TypeDef *)p_ser_drv->HW_InfoPtr->BaseAddr;
  p_drv_data->InitCfg.slaveStartMode = spidrvSlaveStartImmediate;
  p_drv_data->InitCfg.type = spidrvMaster;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;

  end_err_io:
  RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvStop()
 *
 * @brief    Stops serial controller.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/
static void Serial_CtrlrDrvStop(SERIAL_DRV *p_ser_drv,
                                RTOS_ERR   *p_err)
{
  const SERIAL_CTRLR_BSP_API *p_bsp_api = p_ser_drv->BSP_API_Ptr;

  if ((p_bsp_api != DEF_NULL)
      && (p_bsp_api->Init != DEF_NULL)) {
    CPU_BOOLEAN success;

    success = p_bsp_api->Stop();
    if (!success) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvCfgChk()
 *
 * @brief    Validates configuration (ensures desired configurations are supported by
 *           driver/controller).
 *
 * @param    p_ser_drv       Pointer to serial bus driver structure.
 *
 * @param    p_serial_cfg    Pointer to serial configuration structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *
 * @return   Boolean indicating if configurations are supported or not.
 *
 *               DEF_VALID
 *               DEF_INVALID
 *
 * @note     (1) No easy way to validate the configurations. Let's assume they are valid.
 *******************************************************************************************************/
static CPU_BOOLEAN Serial_CtrlrDrvCfgChk(SERIAL_DRV       *p_ser_drv,
                                         const SERIAL_CFG *p_serial_cfg,
                                         RTOS_ERR         *p_err)
{
  PP_UNUSED_PARAM(p_ser_drv);
  PP_UNUSED_PARAM(p_serial_cfg);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (DEF_VALID);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvCfg()
 *
 * @brief    Configure serial controller.
 *
 * @param    p_ser_drv       Pointer to serial bus driver structure.
 *
 * @param    p_serial_cfg    Pointer to serial configuration structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
static void Serial_CtrlrDrvCfg(SERIAL_DRV       *p_ser_drv,
                               const SERIAL_CFG *p_serial_cfg,
                               RTOS_ERR         *p_err)
{
  SERIAL_CTRLR_DRV_DATA *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;

  p_drv_data->InitCfg.bitOrder = (p_serial_cfg->LSB_First) ? spidrvBitOrderLsbFirst : spidrvBitOrderMsbFirst;
  p_drv_data->InitCfg.bitRate = p_serial_cfg->Baudrate;
  p_drv_data->InitCfg.frameLength = p_serial_cfg->FrameSize;

  switch (p_serial_cfg->Mode) {
    case SERIAL_SPI_BUS_MODE_CPHA_BIT:
      p_drv_data->InitCfg.clockMode = spidrvClockMode1;
      break;

    case SERIAL_SPI_BUS_MODE_CPOL_BIT:
      p_drv_data->InitCfg.clockMode = spidrvClockMode2;
      break;

    case SERIAL_SPI_BUS_MODE_CPHA_BIT | SERIAL_SPI_BUS_MODE_CPOL_BIT:
      p_drv_data->InitCfg.clockMode = spidrvClockMode3;
      break;

    case DEF_BIT_NONE:
    default:
      p_drv_data->InitCfg.clockMode = spidrvClockMode0;
      break;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvSlaveSel()
 *
 * @brief    Selects given slave.
 *
 * @param    p_ser_drv       Pointer to serial bus driver structure.
 *
 * @param    p_slave_info    Pointer to slave information structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
static void Serial_CtrlrDrvSlaveSel(SERIAL_DRV              *p_ser_drv,
                                    const SERIAL_SLAVE_INFO *p_slave_info,
                                    RTOS_ERR                *p_err)
{
  const SERIAL_CTRLR_BSP_API *p_bsp_api = p_ser_drv->BSP_API_Ptr;
  SERIAL_CTRLR_DRV_DATA      *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;
  Ecode_t                    result;

  result = SPIDRV_Init(&p_drv_data->Handle,
                       &p_drv_data->InitCfg);
  if (result != ECODE_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  if ((p_bsp_api != DEF_NULL)
      && (p_bsp_api->SlaveSel != DEF_NULL)) {
    CPU_BOOLEAN success;

    success = p_bsp_api->SlaveSel(p_slave_info);
    if (!success) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       Serial_CtrlrDrvSlaveDesel()
 *
 * @brief    Deselects given slave.
 *
 * @param    p_ser_drv       Pointer to serial bus driver structure.
 *
 * @param    p_slave_info    Pointer to slave information structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
static void Serial_CtrlrDrvSlaveDesel(SERIAL_DRV              *p_ser_drv,
                                      const SERIAL_SLAVE_INFO *p_slave_info,
                                      RTOS_ERR                *p_err)
{
  const SERIAL_CTRLR_BSP_API *p_bsp_api = p_ser_drv->BSP_API_Ptr;
  SERIAL_CTRLR_DRV_DATA      *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;
  Ecode_t                    result;

  result = SPIDRV_DeInit(&p_drv_data->Handle);
  if (result != ECODE_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  if ((p_bsp_api != DEF_NULL)
      && (p_bsp_api->SlaveDesel != DEF_NULL)) {
    CPU_BOOLEAN success;

    success = p_bsp_api->SlaveDesel(p_slave_info);
    if (!success) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvRx()
 *
 * @brief    Starts data reception on serial bus.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    p_buf       Pointer to receive buffer.
 *
 * @param    buf_len     Length of receive buffer, in octets.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void Serial_CtrlrDrvRx(SERIAL_DRV *p_ser_drv,
                              CPU_INT08U *p_buf,
                              CPU_INT32U buf_len,
                              RTOS_ERR   *p_err)
{
  SERIAL_CTRLR_DRV_DATA *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(buf_len);
  PP_UNUSED_PARAM(p_err);

  CORE_ENTER_ATOMIC();
  p_drv_data->RxBufPtr = p_buf;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvTx()
 *
 * @brief    Starts data transmission on serial bus.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    p_buf       Pointer to transmit buffer.
 *
 * @param    buf_len     Length of transmit buffer, in octets.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *
 * @note     (1) Enabling the transmit interrupt will trigger a tx buffer empty interrupt right away,
 *               which will be used to start data transmission.
 *******************************************************************************************************/
static void Serial_CtrlrDrvTx(SERIAL_DRV       *p_ser_drv,
                              const CPU_INT08U *p_buf,
                              CPU_INT32U       buf_len,
                              RTOS_ERR         *p_err)
{
  SERIAL_CTRLR_DRV_DATA *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;
  Ecode_t               result;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (p_drv_data->RxBufPtr == DEF_NULL) {
    result = SPIDRV_MTransmit(&p_drv_data->Handle,
                              (const  void *)p_buf,
                              buf_len,
                              Serial_CtrlrDrvCallback);
  } else {
    result = SPIDRV_MTransfer(&p_drv_data->Handle,
                              (const  void *)p_buf,
                              (void *)p_drv_data->RxBufPtr,
                              buf_len,
                              Serial_CtrlrDrvCallback);
  }
  if (result != ECODE_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvByteTx()
 *
 * @brief    Transmit byte.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    byte        Byte to transmit.
 *
 * @param    len         Length of transmission (number of time the byte should be transmitted).
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
static void Serial_CtrlrDrvByteTx(SERIAL_DRV *p_ser_drv,
                                  CPU_INT08U byte,
                                  CPU_INT32U len,
                                  RTOS_ERR   *p_err)
{
  SERIAL_CTRLR_DRV_DATA *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;
  Ecode_t               result;

  if ((CPU_INT08U)p_drv_data->InitCfg.dummyTxValue != byte) {
    result = SPIDRV_DeInit(&p_drv_data->Handle);
    if (result != ECODE_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    p_drv_data->InitCfg.dummyTxValue = byte;

    result = SPIDRV_Init(&p_drv_data->Handle,
                         &p_drv_data->InitCfg);
    if (result != ECODE_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  result = SPIDRV_MReceive(&p_drv_data->Handle,
                           (void *)p_drv_data->RxBufPtr,
                           len,
                           Serial_CtrlrDrvCallback);
  if (result != ECODE_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvAbort()
 *
 * @brief    Abort transmission/reception on serial bus.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    dir         Bitmap of direction(s) to abort.
 *                       SERIAL_ABORT_DIR_RX
 *                       SERIAL_ABORT_DIR_TX
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *
 * @note     (1) RSPI controller is SPI only, so both directions are always aborted.
 *******************************************************************************************************/
static void Serial_CtrlrDrvAbort(SERIAL_DRV *p_ser_drv,
                                 CPU_INT08U dir,
                                 RTOS_ERR   *p_err)
{
  SERIAL_CTRLR_DRV_DATA *p_drv_data = (SERIAL_CTRLR_DRV_DATA *)p_ser_drv->DataPtr;
  Ecode_t               result;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(dir);

  result = SPIDRV_AbortTransfer(&p_drv_data->Handle);
  if (result != ECODE_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  CORE_ENTER_ATOMIC();
  p_drv_data->RxBufPtr = DEF_NULL;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvIO_Ctrl()
 *
 * @brief    Perform IO Ctrl.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @param    cmd         Command to execute.
 *
 * @param    p_arg       Pointer to IOCtrl data. May be input, output, or DEF_NULL depending on the
 *                       value of 'cmd'.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_SUPPORTED
 *******************************************************************************************************/
static void Serial_CtrlrDrvIO_Ctrl(SERIAL_DRV *p_ser_drv,
                                   CPU_INT32S cmd,
                                   void       *p_arg,
                                   RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_ser_drv);
  PP_UNUSED_PARAM(p_arg);

  switch (cmd) {
    case SERIAL_IOCTRL_CMD_LOOPBACK:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      break;
  }
}

/****************************************************************************************************//**
 *                                       Serial_CtrlrDrvReqAlignGet()
 *
 * @brief    Retrieves required data buffer alignment.
 *
 * @param    p_ser_drv   Pointer to serial bus driver structure.
 *
 * @return   Buffer alignment, in octets.
 *******************************************************************************************************/
static CPU_SIZE_T Serial_CtrlrDrvReqAlignGet(SERIAL_DRV *p_ser_drv)
{
  PP_UNUSED_PARAM(p_ser_drv);

  return (sizeof(CPU_ALIGN));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Serial_CtrlrDrvCallback()
 *
 * @brief    Completes SPI transfer.
 *
 * @param    handle              SPIDRV handle.
 *
 * @param    transfer_status     Transfer status.
 *
 * @param    items_transferred   Length of data transferred.
 *******************************************************************************************************/
static void Serial_CtrlrDrvCallback(struct SPIDRV_HandleData *handle,
                                    Ecode_t                  transfer_status,
                                    int                      items_transferred)
{
  SERIAL_CTRLR_DRV_DATA *p_drv_data = DEF_NULL;
  SERIAL_CTRLR_DRV_DATA *p_drv_data_temp;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(items_transferred);

  SLIST_FOR_EACH_ENTRY(Serial_CtrlrDrvDataListHeadPtr, p_drv_data_temp, SERIAL_CTRLR_DRV_DATA, ListNode) {
    if (&p_drv_data_temp->Handle == handle) {
      p_drv_data = p_drv_data_temp;
      break;
    }
  }

  if (p_drv_data != DEF_NULL) {
    RTOS_ERR err;

    RTOS_ERR_SET(err, (transfer_status == ECODE_OK) ? RTOS_ERR_NONE : RTOS_ERR_IO);

    Serial_EventTxCmpl(p_drv_data->SerHandle,
                       err);

    CORE_ENTER_ATOMIC();
    if (p_drv_data->RxBufPtr != DEF_NULL) {
      p_drv_data->RxBufPtr = DEF_NULL;
      CORE_EXIT_ATOMIC();

      Serial_EventRxCmpl(p_drv_data->SerHandle,
                         err);
    } else {
      CORE_EXIT_ATOMIC();
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SERIAL_AVAIL))
