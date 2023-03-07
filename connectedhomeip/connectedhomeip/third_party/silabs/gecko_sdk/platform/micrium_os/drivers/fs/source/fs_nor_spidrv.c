/***************************************************************************//**
 * @file
 * @brief File System - EFM32-EFR32 SPIDRV Controller Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <rtos_description.h>
#include <drivers/fs/include/fs_nor_spidrv.h>
#include <fs/include/fs_nor.h>
#include <fs/include/fs_nor_phy_drv.h>
#include <common/source/kal/kal_priv.h>
#include "spidrv.h"
#include "em_gpio.h"

//                                                                Configuration file
#include "sl_mx25_flash_shutdown_usart_config.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------- DRIVER DATA --------------------
typedef  struct spi_data {
  FS_NOR_SPIDRV_CTRLR_INFO    *SPI_CtrlrInfoPtr;                 // Pointer to SPI controller info.
  SPIDRV_HandleData_t         SPIDRV_Handle;                    // SPIDRV handle.
} SPI_DATA;

/********************************************************************************************************
 *                                DRIVER INTERFACE FUNCTION PROTOTYPES
 *******************************************************************************************************/

static void *FS_NOR_SPIDRV_Add(const FS_NOR_SPIDRV_CTRLR_INFO *p_ctrlr_info,
                               const FS_NOR_SPIDRV_SLAVE_INFO *p_slave_info,
                               MEM_SEG                        *p_seg,
                               RTOS_ERR                       *p_err);

static void FS_NOR_SPIDRV_Start(void     *p_drv_data,
                                RTOS_ERR *p_err);

static void FS_NOR_SPIDRV_Stop(void     *p_drv_data,
                               RTOS_ERR *p_err);

static void FS_NOR_SPIDRV_ClkSet(void       *p_drv_data,
                                 CPU_INT32U clk,
                                 RTOS_ERR   *p_err);

static void FS_NOR_SPIDRV_CmdSend(void                           *p_drv_data,
                                  const FS_NOR_SPIDRV_CMD_DESC   *p_cmd,
                                  CPU_INT08U                     addr_tbl[],
                                  CPU_INT08U                     inter_data[],
                                  CPU_INT08U                     inter_cycles,
                                  void                           *p_xfer_data,
                                  CPU_INT32U                     xfer_size,
                                  RTOS_ERR                       *p_err);

static CPU_SIZE_T FS_NOR_SPIDRV_AlignReqGet(void     *p_drv_data,
                                            RTOS_ERR *p_err);

/********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_Transmit(SPIDRV_Handle_t handle,
                                   const void      *buffer,
                                   int             count,
                                   RTOS_ERR        *p_err);

static void FS_NOR_SPIDRV_Receive(SPIDRV_Handle_t handle,
                                  void            *buffer,
                                  int             count,
                                  RTOS_ERR        *p_err);

static void FS_NOR_SPIDRV_Callback(SPIDRV_Handle_t handle,
                                   Ecode_t         transferStatus,
                                   int             itemsTransferred);

/********************************************************************************************************
 *                            STATIC VARIABLES
 *******************************************************************************************************/
static SPIDRV_Init_t FS_NOR_SPIDRV_init_mx25 =  {
  SL_MX25_FLASH_SHUTDOWN_PERIPHERAL,  /* USART port                       */
  SL_MX25_FLASH_SHUTDOWN_TX_PORT,     /* USART Tx port location number    */
  SL_MX25_FLASH_SHUTDOWN_RX_PORT,     /* USART Rx port location number    */
  SL_MX25_FLASH_SHUTDOWN_CLK_PORT,    /* USART Clk port location number   */
  SL_MX25_FLASH_SHUTDOWN_CS_PORT,     /* USART Cs port location number    */
  SL_MX25_FLASH_SHUTDOWN_TX_PIN,      /* USART Tx port location number    */
  SL_MX25_FLASH_SHUTDOWN_RX_PIN,      /* USART Rx port location number    */
  SL_MX25_FLASH_SHUTDOWN_CLK_PIN,     /* USART Clk pin location number    */
  SL_MX25_FLASH_SHUTDOWN_CS_PIN,      /* USART Cs pin location number     */
  1000000,                            /* Bitrate                          */
  8,                                  /* Frame length                     */
  0xFF,                               /* Dummy tx value for rx only funcs */
  spidrvMaster,                       /* SPI mode                         */
  spidrvBitOrderMsbFirst,             /* Bit order on bus                 */
  spidrvClockMode0,                   /* SPI clock/phase mode             */
  spidrvCsControlApplication,         /* CS controlled by the driver      */
  spidrvSlaveStartImmediate           /* Slave start transfers immediately*/
};

static KAL_SEM_HANDLE FS_NOR_SPIDRV_semaphore;

/********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *******************************************************************************************************/

const FS_NOR_SPIDRV_DRV_API FS_NOR_SpiDrvAPI = {
  .Add = FS_NOR_SPIDRV_Add,
  .Start = FS_NOR_SPIDRV_Start,
  .Stop = FS_NOR_SPIDRV_Stop,
  .ClkSet = FS_NOR_SPIDRV_ClkSet,
  .DTR_Set = DEF_NULL,
  .FlashSizeSet = DEF_NULL,
  .CmdSend = FS_NOR_SPIDRV_CmdSend,
  .WaitWhileBusy = DEF_NULL,
  .AlignReqGet = FS_NOR_SPIDRV_AlignReqGet,
  .XipBitSet = DEF_NULL,
  .XipCfg = DEF_NULL
};

const FS_NOR_SPIDRV_CTRLR_INFO BSP_FS_NOR_SPICtrl_HwInfo = {
  .DrvApiPtr = (FS_NOR_SPIDRV_DRV_API *)&FS_NOR_SpiDrvAPI,
  .BspApiPtr = DEF_NULL,
  .BaseAddr = DEF_NULL,
  .AlignReq = sizeof(CPU_ALIGN),
  .FlashMemMapStartAddr = FS_NOR_PHY_FLASH_MEM_MAP_NONE,
  .BusWidth = FS_NOR_SERIAL_BUS_WIDTH_SINGLE_IO,
};

const FS_NOR_SPIDRV_HW_INFO BSP_FS_NOR_MX25R_SPI_HwInfo = {
  .PartHwInfo.PhyApiPtr = (CPU_INT32U)(&FS_NOR_PHY_MX25R_API),
  .PartHwInfo.ChipSelID = 0u,
  .CtrlrHwInfoPtr = &BSP_FS_NOR_SPICtrl_HwInfo
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          FS_NOR_SPI_Add()
 *
 * @brief   Add a SPI controller driver instance.
 *
 * @param   p_ctrlr_info    Pointer to a controller information structure.
 *
 * @param   p_slave_info    Pointer to a slave information structure.
 *
 * @param   p_seg           Pointer to a memory segment where to allocate the controller driver instance.
 *
 * @param   p_err           Error pointer.
 *
 * @return  Pointer to driver-specific data, if NO error(s).
 *          Null pointer,                    otherwise.
 *******************************************************************************************************/
static void *FS_NOR_SPIDRV_Add(const FS_NOR_SPIDRV_CTRLR_INFO *p_ctrlr_info,
                               const FS_NOR_SPIDRV_SLAVE_INFO *p_slave_info,
                               MEM_SEG                        *p_seg,
                               RTOS_ERR                       *p_err)
{
  SPI_DATA *p_spi_drv_data = DEF_NULL;
  PP_UNUSED_PARAM(p_slave_info);
  PP_UNUSED_PARAM(p_seg);

  //                                                               Create semaphore for transfer status
  FS_NOR_SPIDRV_semaphore = KAL_SemCreate("NOR SPIDRV semaphore",
                                          DEF_NULL,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (DEF_NULL);
  }

  p_spi_drv_data = (SPI_DATA *)Mem_SegAlloc(DEF_NULL,
                                            p_seg,
                                            sizeof(SPI_DATA),
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (DEF_NULL);
  }
  Mem_Clr(p_spi_drv_data, sizeof(SPI_DATA));

  //                                                               Save SPI controller info for later usage by drv.
  p_spi_drv_data->SPI_CtrlrInfoPtr = (FS_NOR_SPIDRV_CTRLR_INFO *)p_ctrlr_info;

  return ((void *)p_spi_drv_data);
}

/****************************************************************************************************//**
 *                                        FS_NOR_SPI_CmdSend()
 *
 * @brief   Send a command.
 *
 * @param   p_drv_data      Pointer to driver-specific data.
 *
 * @param   p_cmd           Pointer to a command descriptor.
 *
 * @param   addr_tbl        Source / Destination address table.
 *
 * @param   inter_data      Inter data table.
 *
 * @param   inter_cycles    Inter cycle cnt.
 *
 * @param   p_xfer_data     Pointer to a buffer that contains data to be written or that receives
 *                          data to be read.
 *
 * @param   xfer_size       Number of octets to be read / written.
 *
 * @param   p_err           Error pointer.
 *
 * @note    (1) The NOR command can have one of the following formats:
 *
 *              - (a) OPCODE (single operation such as reset, write enable)
 *              - (b) OPCODE + DATA (flash register read/write access)
 *              - (c) OPCODE + ADDRESS (erase operation)
 *              - (d) OPCODE + ADDRESS + DATA (memory read/write access in indirect/direct mode)
 *
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_CmdSend(void                           *p_drv_data,
                                  const FS_NOR_SPIDRV_CMD_DESC   *p_cmd,
                                  CPU_INT08U                     addr_tbl[],
                                  CPU_INT08U                     inter_data[],
                                  CPU_INT08U                     inter_cycles,
                                  void                           *p_xfer_data,
                                  CPU_INT32U                     xfer_size,
                                  RTOS_ERR                       *p_err)
{
  CPU_INT08U      *p_buffer = (CPU_INT08U *)p_xfer_data;
  SPIDRV_Handle_t spidrv_handle = (SPIDRV_Handle_t)&((SPI_DATA *)p_drv_data)->SPIDRV_Handle;
  CPU_INT32U      remaining;
  CPU_INT32U      chunk;

  if (p_cmd->Form.MultiIO_Quad == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  //                                                               Enable Chip
  GPIO_PinOutClear(spidrv_handle->portCs,
                   spidrv_handle->pinCs);
  //                                                               Send OPCODE
  FS_NOR_SPIDRV_Transmit(spidrv_handle, &(p_cmd->Opcode), sizeof(p_cmd->Opcode), p_err);

  if (p_cmd->Form.HasAddr == DEF_TRUE) {                          // Check if Flash address must be sent
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      FS_NOR_SPIDRV_Transmit(spidrv_handle, &addr_tbl[2u], sizeof(addr_tbl[2u]), p_err);
    }
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      FS_NOR_SPIDRV_Transmit(spidrv_handle, &addr_tbl[1u], sizeof(addr_tbl[1u]), p_err);
    }
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      FS_NOR_SPIDRV_Transmit(spidrv_handle, &addr_tbl[0u], sizeof(addr_tbl[0u]), p_err);
    }
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      if (inter_cycles != DEF_NULL) {                             // Send dummy byte before FREAD begins.
        FS_NOR_SPIDRV_Transmit(spidrv_handle, &inter_data[0u], sizeof(inter_data[0u]), p_err);
      }
    }
  }

  remaining = xfer_size;
  while ((remaining > 0) && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
    //                                                             limit amount of data sent per transfer
    chunk = (remaining > DMADRV_MAX_XFER_COUNT) ? DMADRV_MAX_XFER_COUNT : remaining;
    if (p_cmd->Form.IsWr == DEF_YES) {                          // Write DATA
      FS_NOR_SPIDRV_Transmit(spidrv_handle, p_buffer, chunk, p_err);
    } else {                                                    // Read DATA
      FS_NOR_SPIDRV_Receive(spidrv_handle, p_buffer, chunk, p_err);
    }
    p_buffer = p_buffer + chunk;
    remaining = remaining - chunk;
  }

  //                                                              Disable Chip
  GPIO_PinOutSet(spidrv_handle->portCs,
                 spidrv_handle->pinCs);
}

/****************************************************************************************************//**
 *                                         FS_NOR_SPI_ClkSet()
 *
 * @brief   Set serial clock frequency outputted by the SPI controller to the flash device.
 *
 * @param   p_drv_data    Pointer to driver-specific data.
 *
 * @param   clk           Serial clock frequency.
 *
 * @param   p_err         Error pointer.
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_ClkSet(void       *p_drv_data,
                                 CPU_INT32U clk,
                                 RTOS_ERR   *p_err)
{
  SPIDRV_Handle_t spidrv_handle = (SPIDRV_Handle_t)&((SPI_DATA *)p_drv_data)->SPIDRV_Handle;

  if (ECODE_EMDRV_SPIDRV_OK != SPIDRV_SetBitrate(spidrv_handle, clk)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                         FS_NOR_SPIDRV_Start()
 *
 * @brief   Start controller operation.
 *
 * @param   p_drv_data      Pointer to driver-specific data.
 *
 * @param   p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_Start(void     *p_drv_data,
                                RTOS_ERR *p_err)
{
  SPIDRV_Handle_t spidrv_handle = (SPIDRV_Handle_t)&((SPI_DATA *)p_drv_data)->SPIDRV_Handle;

  //                                                              Initialize SPIDRV
  if (SPIDRV_Init(spidrv_handle, &FS_NOR_SPIDRV_init_mx25) != ECODE_EMDRV_SPIDRV_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  //                                                              Configure CS
  GPIO_PinModeSet((GPIO_Port_TypeDef)spidrv_handle->portCs, spidrv_handle->pinCs,
                  gpioModePushPull, 1);
}

/****************************************************************************************************//**
 *                                          FS_NOR_SPIDRV_Stop()
 *
 * @brief   Stop controller operation.
 *
 * @param   p_drv_data     Pointer to driver-specific data.
 *
 * @param   p_err          Error pointer.
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_Stop(void     *p_drv_data,
                               RTOS_ERR *p_err)
{
  SPIDRV_Handle_t spidrv_handle = (SPIDRV_Handle_t)&((SPI_DATA *)p_drv_data)->SPIDRV_Handle;

  //                                                              DeInitialize SPIDRV
  if (SPIDRV_DeInit(spidrv_handle) != ECODE_EMDRV_SPIDRV_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }

  //                                                              Disable CS
  GPIO_PinModeSet((GPIO_Port_TypeDef)spidrv_handle->portCs, spidrv_handle->pinCs,
                  gpioModeDisabled, 0);
}

/****************************************************************************************************//**
 *                                     FS_NOR_SPI_AlignReqGet()
 *
 * @brief   Get buffer alignment requirement of the controller.
 *
 * @param   p_drv_data      Pointer to driver-specific data.
 *
 * @param   p_err           Error pointer.
 *
 * @return  buffer alignment requirement in octets.
 *
 *******************************************************************************************************/
static CPU_SIZE_T FS_NOR_SPIDRV_AlignReqGet(void     *p_drv_data,
                                            RTOS_ERR *p_err)
{
  SPI_DATA *p_spi_drv_data = (SPI_DATA *)p_drv_data;
  PP_UNUSED_PARAM(p_drv_data);
  PP_UNUSED_PARAM(p_err);

  return (p_spi_drv_data->SPI_CtrlrInfoPtr->AlignReq);
}

/****************************************************************************************************//**
 *                                     FS_NOR_SPIDRV_Transmit()
 *
 * @brief   Transmit spidrv data.
 *
 * @param   handle            SPIDRV handle.
 *
 * @param   buffer            Buffer with data to transmit
 *
 * @param   count             Size of buffer
 *
 * @param   p_err             Error pointer
 *
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_Transmit(SPIDRV_Handle_t handle,
                                   const void *buffer,
                                   int count,
                                   RTOS_ERR *p_err)
{
  if (SPIDRV_MTransmit(handle, buffer, count, FS_NOR_SPIDRV_Callback) == ECODE_EMDRV_SPIDRV_OK) {
    KAL_SemPend(FS_NOR_SPIDRV_semaphore, KAL_OPT_PEND_BLOCKING, 0, p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                     FS_NOR_SPIDRV_Receive()
 *
 * @brief   Receive spidrv data.
 *
 * @param   handle            SPIDRV handle.
 *
 * @param   buffer            Buffer to receive data
 *
 * @param   count             Buffer size
 *
 * @param   p_err             Error pointer
 *
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_Receive(SPIDRV_Handle_t handle,
                                  void *buffer,
                                  int count,
                                  RTOS_ERR *p_err)
{
  if (SPIDRV_MReceive(handle, buffer, count, FS_NOR_SPIDRV_Callback) == ECODE_EMDRV_SPIDRV_OK) {
    KAL_SemPend(FS_NOR_SPIDRV_semaphore, KAL_OPT_PEND_BLOCKING, 0, p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                     FS_NOR_SPIDRV_Callback()
 *
 * @brief   Callback for spidrv transfers.
 *
 * @param   handle            SPIDRV handle.
 *
 * @param   transfer_status   Status of current transfer.
 *
 * @param   items_transfered Number of bytes transfered.
 *
 *******************************************************************************************************/
static void FS_NOR_SPIDRV_Callback(SPIDRV_Handle_t handle,
                                   Ecode_t transfer_status,
                                   int items_transfered)
{
  RTOS_ERR p_err;
  PP_UNUSED_PARAM(handle);
  PP_UNUSED_PARAM(transfer_status);
  PP_UNUSED_PARAM(items_transfered);

  if (handle->state != spidrvStateTransferring) {
    KAL_SemPost(FS_NOR_SPIDRV_semaphore, KAL_OPT_POST_NONE, &p_err);
  }
}
