/***************************************************************************//**
 * @file
 * @brief IO - Sd Card Core
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

#if (defined(RTOS_MODULE_IO_SD_AVAIL))

#if (!defined(RTOS_MODULE_IO_AVAIL))

#error IO SD module requires IO module. Make sure it is part of your project and that \
  RTOS_MODULE_IO_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <io/source/sd/sd_priv.h>
#include  <io/source/sd/sd_card_ctrlr_priv.h>

#include  <io/include/sd.h>
#include  <io/include/sd_card.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                            (IO, SD)
#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_IO

#define  SD_CARD_RESP_BUF_LEN                  17u              // Largest resp possible is 136 bit long (R2).

#define  SD_CARD_CMD_END_WAIT_TIMEOUT_MS     5000u

#define  SD_CARD_CUR_STATE_IDLE                 0u
#define  SD_CARD_CUR_STATE_READY                1u
#define  SD_CARD_CUR_STATE_IDENT                2u
#define  SD_CARD_CUR_STATE_STBY                 3u
#define  SD_CARD_CUR_STATE_TRAN                 4u
#define  SD_CARD_CUR_STATE_DATA                 5u
#define  SD_CARD_CUR_STATE_RCV                  6u
#define  SD_CARD_CUR_STATE_PRG                  7u
#define  SD_CARD_CUR_STATE_DIS                  8u

#define  SD_CARD_CMD7_ARG_RCA_MSK               DEF_BIT_FIELD(16u, 16u)

/********************************************************************************************************
 *                                   NORMAL (R1) RESPONSE BIT DEFINES
 *******************************************************************************************************/

#define  SD_CARD_R1_AKE_SEQ_ERROR               DEF_BIT_03
#define  SD_CARD_R1_APP_CMD                     DEF_BIT_05
#define  SD_CARD_R1_READY_FOR_DATA              DEF_BIT_08
#define  SD_CARD_R1_ERASE_RESET                 DEF_BIT_13
#define  SD_CARD_R1_CARD_ECC_DISABLED           DEF_BIT_14
#define  SD_CARD_R1_WP_ERASE_SKIP               DEF_BIT_15
#define  SD_CARD_R1_CSD_OVERWRITE               DEF_BIT_16
#define  SD_CARD_R1_ERROR                       DEF_BIT_19
#define  SD_CARD_R1_CC_ERROR                    DEF_BIT_20
#define  SD_CARD_R1_CARD_ECC_FAILED             DEF_BIT_21
#define  SD_CARD_R1_ILLEGAL_COMMAND             DEF_BIT_22
#define  SD_CARD_R1_COM_CRC_ERROR               DEF_BIT_23
#define  SD_CARD_R1_LOCK_UNLOCK_FAILED          DEF_BIT_24
#define  SD_CARD_R1_CARD_IS_LOCKED              DEF_BIT_25
#define  SD_CARD_R1_WP_VIOLATION                DEF_BIT_26
#define  SD_CARD_R1_ERASE_PARAM                 DEF_BIT_27
#define  SD_CARD_R1_ERASE_SEQ_ERROR             DEF_BIT_28
#define  SD_CARD_R1_BLOCK_LEN_ERROR             DEF_BIT_29
#define  SD_CARD_R1_ADDRESS_ERROR               DEF_BIT_30
#define  SD_CARD_R1_OUT_OF_RANGE                DEF_BIT_31

#define  SD_CARD_R1_CUR_STATE_MASK              DEF_BIT_FIELD(4u, 9u)

#define  SD_CARD_R1_ERR_ANY_MSK                 DEF_BIT_FIELD(13u, 19u)

/********************************************************************************************************
 *                               IO SEND OP COND (R4) RESPONSE BIT DEFINES
 *******************************************************************************************************/

#define  SD_CARD_R4_IO_OCR_MSK                  DEF_BIT_FIELD(24u, 0u)
#define  SD_CARD_R4_S18A                        DEF_BIT_24
#define  SD_CARD_R4_MEM_PRES                    DEF_BIT_27
#define  SD_CARD_R4_NBR_IO_FNCT_MSK             DEF_BIT_FIELD(3u, 28u)
#define  SD_CARD_R4_C                           DEF_BIT_31

/********************************************************************************************************
 *                                   IO RW (R5) RESPONSE BIT DEFINES
 *******************************************************************************************************/

#define  SD_CARD_R5_COM_CRC_ERR                 DEF_BIT_07
#define  SD_CARD_R5_ILLEGAL_CMD                 DEF_BIT_06
#define  SD_CARD_R5_IO_CUR_STATE_MSK           (DEF_BIT_05 | DEF_BIT_04)
#define  SD_CARD_R5_IO_CUR_STATE_DIS            0u
#define  SD_CARD_R5_IO_CUR_STATE_CMD            1u
#define  SD_CARD_R5_IO_CUR_STATE_TRN            2u
#define  SD_CARD_R5_IO_CUR_STATE_RFU            3u
#define  SD_CARD_R5_ERR                         DEF_BIT_03
#define  SD_CARD_R5_FNCT_NBR_ERR                DEF_BIT_01
#define  SD_CARD_R5_OUT_OF_RANGE                DEF_BIT_00

#define  SD_CARD_R5_ERR_ANY_MSK                (SD_CARD_R5_COM_CRC_ERR | SD_CARD_R5_ILLEGAL_CMD \
                                                | SD_CARD_R5_ERR | SD_CARD_R5_FNCT_NBR_ERR | SD_CARD_R5_OUT_OF_RANGE)

/********************************************************************************************************
 *                               PUBLISHED RCA (R6) RESPONSE BIT DEFINES
 *******************************************************************************************************/

#define  SD_CARD_R6_ARG_FIELD_STATUS_MSK                DEF_BIT_FIELD(16u, 0u)
#define  SD_CARD_R6_ARG_FIELD_STATUS_COM_CRC_ERR        DEF_BIT_15
#define  SD_CARD_R6_ARG_FIELD_STATUS_ILLEGAL_CMD        DEF_BIT_14
#define  SD_CARD_R6_ARG_FIELD_STATUS_ERR                DEF_BIT_13
#define  SD_CARD_R6_ARG_FIELD_STATUS_READY_FOR_DATA     SD_CARD_R1_READY_FOR_DATA
#define  SD_CARD_R6_ARG_FIELD_STATUS_APP_CMD            SD_CARD_R1_APP_CMD
#define  SD_CARD_R6_ARG_FIELD_STATUS_AKE_SEQ_ERROR      SD_CARD_R1_AKE_SEQ_ERROR

#define  SD_CARD_R6_ARG_FIELD_STATUS_ERR_ANY_MSK       (SD_CARD_R6_ARG_FIELD_STATUS_COM_CRC_ERR   \
                                                        | SD_CARD_R6_ARG_FIELD_STATUS_ILLEGAL_CMD \
                                                        | SD_CARD_R6_ARG_FIELD_STATUS_ERR)

#define  SD_CARD_R6_ARG_FIELD_STATUS_STATE_MSK          SD_CARD_R1_CUR_STATE_MASK

#define  SD_CARD_R6_ARG_FIELD_RCA_MSK                   DEF_BIT_FIELD(16u, 16u)

/********************************************************************************************************
 *                                   SD CARD DATA LINES LEVEL DEFINES
 *******************************************************************************************************/

#define  SD_CARD_DATA_LINES_LVL_DAT0        DEF_BIT_00
#define  SD_CARD_DATA_LINES_LVL_DAT1        DEF_BIT_01
#define  SD_CARD_DATA_LINES_LVL_DAT2        DEF_BIT_02
#define  SD_CARD_DATA_LINES_LVL_DAT3        DEF_BIT_03

#define  SD_CARD_DATA_LINES_LVL_ALL_DOWN    0x0
#define  SD_CARD_DATA_LINES_LVL_ALL_UP      0xF

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const SD_CARD_BUS_HANDLE SD_CardBusHandleNull = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern SD_INIT_CFG SD_InitCfg;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                           SD Card bus data
 *******************************************************************************************************/

struct  sd_card_bus {
  SD_CARD_DRV    Drv;                                           ///< SD Card driver structure.

  SD_BUS_HANDLE  BusHandle;                                     ///< Handle to SD Bus.

  CPU_INT08U     *RespBufPtr;                                   ///< Pointer to buffer that contains CMD responses.
  CPU_INT16U     RCA;                                           ///< Card's Relative Card Address.

  RTOS_ERR       CmdErr;                                        ///< Error from last executed command.
  KAL_SEM_HANDLE CmdCmplSem;                                    ///< Semaphore that notifies command completion.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               CMD FLAG(S) <-> CMD RESP TYPE LOOKUP TABLE
 *******************************************************************************************************/

static const CPU_INT16U SD_Card_CmdFlag[] = {
  SD_CARD_CMD_FLAG_NONE,
  SD_CARD_CMD_FLAG_NONE,
  (SD_CARD_CMD_FLAG_IX_VALID | SD_CARD_CMD_FLAG_CRC_VALID | SD_CARD_CMD_FLAG_RESP),
  (SD_CARD_CMD_FLAG_IX_VALID | SD_CARD_CMD_FLAG_CRC_VALID | SD_CARD_CMD_FLAG_BUSY | SD_CARD_CMD_FLAG_RESP),
  (SD_CARD_CMD_FLAG_CRC_VALID | SD_CARD_CMD_FLAG_RESP | SD_CARD_CMD_FLAG_RESP_LONG),
  SD_CARD_CMD_FLAG_RESP,
  SD_CARD_CMD_FLAG_RESP,
  (SD_CARD_CMD_FLAG_IX_VALID | SD_CARD_CMD_FLAG_CRC_VALID | SD_CARD_CMD_FLAG_RESP),
  (SD_CARD_CMD_FLAG_IX_VALID | SD_CARD_CMD_FLAG_CRC_VALID | SD_CARD_CMD_FLAG_BUSY | SD_CARD_CMD_FLAG_RESP | SD_CARD_CMD_FLAG_RESP_LONG),
  (SD_CARD_CMD_FLAG_IX_VALID | SD_CARD_CMD_FLAG_CRC_VALID | SD_CARD_CMD_FLAG_RESP),
  (SD_CARD_CMD_FLAG_IX_VALID | SD_CARD_CMD_FLAG_CRC_VALID | SD_CARD_CMD_FLAG_RESP)
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           BUS DRIVER FUNCTIONS
 *******************************************************************************************************/

static void *SD_BusDrvInit(SD_BUS_HANDLE bus_handle,
                           void          *p_drv_info,
                           RTOS_ERR      *p_err);

static SD_CAPABILITIES SD_BusDrvInitHW(void     *p_bus_drv_data,
                                       RTOS_ERR *p_err);

static void SD_BusDrvStart(void     *p_bus_drv_data,
                           RTOS_ERR *p_err);

static void SD_BusDrvStop(void     *p_bus_drv_data,
                          RTOS_ERR *p_err);

static CPU_SIZE_T SD_BusDrvAlignReqGet(void     *p_bus_drv_data,
                                       RTOS_ERR *p_err);

static void SD_BusDrvClkFreqSet(void       *p_bus_drv_data,
                                CPU_INT32U freq,
                                RTOS_ERR   *p_err);

static void SD_BusDrvBusSupplyVoltSet(void             *p_bus_drv_data,
                                      SD_CARD_BUS_VOLT volt,
                                      RTOS_ERR         *p_err);

static void SD_BusDrvBusSignalVoltInit(void     *p_bus_drv_data,
                                       RTOS_ERR *p_err);

static void SD_BusDrvBusWidthSet(void       *p_bus_drv_data,
                                 CPU_INT08U width,
                                 RTOS_ERR   *p_err);

static void SD_BusDrvBusSignalVoltSwitch(void     *p_bus_drv_data,
                                         RTOS_ERR *p_err);

static SD_STATUS SD_BusDrvCmdR1Exec(void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    CPU_INT08U  cmd_nbr,
                                    CPU_INT32U  arg,
                                    RTOS_ERR    *p_err);

static SD_STATUS SD_BusDrvCmd3Exec(void        *p_bus_drv_data,
                                   SD_CARDTYPE card_type,
                                   RTOS_ERR    *p_err);

static SD_STATUS SD_BusDrvCmd7Exec(void        *p_bus_drv_data,
                                   SD_CARDTYPE card_type,
                                   CPU_BOOLEAN select,
                                   RTOS_ERR    *p_err);

static void SD_BusDrvCmdR3Exec(void        *p_bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  cmd_nbr,
                               CPU_INT32U  arg,
                               CPU_INT32U  *p_r3_resp,
                               RTOS_ERR    *p_err);

static void SD_BusDrvCmdR4Exec(void           *p_bus_drv_data,
                               SD_CARDTYPE    card_type,
                               CPU_INT08U     cmd_nbr,
                               CPU_INT32U     arg,
                               SD_CMD_R4_DATA *p_r4_resp,
                               RTOS_ERR       *p_err);

static SD_STATUS SD_BusDrvCmdR5Exec(void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    CPU_INT08U  cmd_nbr,
                                    CPU_INT32U  arg,
                                    CPU_INT08U  *p_rw_data,
                                    RTOS_ERR    *p_err);

static void SD_BusDrvCmdR7Exec(void           *p_bus_drv_data,
                               SD_CARDTYPE    card_type,
                               CPU_INT08U     cmd_nbr,
                               CPU_INT32U     arg,
                               SD_CMD_R7_DATA *p_r7_resp,
                               RTOS_ERR       *p_err);

static void SD_BusDrvCmdNoRespExec(void        *p_bus_drv_data,
                                   SD_CARDTYPE card_type,
                                   CPU_INT08U  cmd_nbr,
                                   CPU_INT32U  arg,
                                   RTOS_ERR    *p_err);

static void SD_BusDrvDataXferPrepare(void        *p_bus_drv_data,
                                     SD_CARDTYPE card_type,
                                     CPU_INT08U  *p_buf,
                                     CPU_INT32U  blk_nbr,
                                     CPU_INT32U  blk_len,
                                     CPU_BOOLEAN dir_is_rd,
                                     RTOS_ERR    *p_err);

static void SD_BusDrvDataXferCmpl(void        *p_bus_drv_data,
                                  SD_CARDTYPE card_type,
                                  CPU_INT08U  *p_buf,
                                  CPU_INT32U  blk_nbr,
                                  CPU_INT32U  blk_len,
                                  CPU_BOOLEAN dir_is_rd,
                                  RTOS_ERR    *p_err);

static void SD_CtrlrDrvCardIntEnDis(void        *p_bus_drv_data,
                                    CPU_BOOLEAN enable);

/********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 *******************************************************************************************************/

static void SD_CardCmdExec(SD_CARD_BUS_HANDLE sd_card_bus_handle,
                           SD_CARDTYPE        card_type,
                           CPU_INT08U         cmd_nbr,
                           CPU_INT32U         arg,
                           SD_CARD_RESP_TYPE  resp_type,
                           RTOS_ERR           *p_err);

static void SD_CardCmdEndWait(SD_CARD_BUS_HANDLE sd_card_bus_handle,
                              RTOS_ERR           *p_err);

/********************************************************************************************************
 *                                           BUS DRIVER API
 *******************************************************************************************************/

const SD_BUS_DRV_API SD_CardBusDrvAPI = {
  .Init = SD_BusDrvInit,
  .InitHW = SD_BusDrvInitHW,
  .Start = SD_BusDrvStart,
  .Stop = SD_BusDrvStop,
  .AlignReqGet = SD_BusDrvAlignReqGet,
  .ClkFreqSet = SD_BusDrvClkFreqSet,
  .BusSupplyVoltSet = SD_BusDrvBusSupplyVoltSet,
  .BusSignalVoltInit = SD_BusDrvBusSignalVoltInit,
  .BusWidthSet = SD_BusDrvBusWidthSet,
  .BusSignalVoltSwitch = SD_BusDrvBusSignalVoltSwitch,
  .CmdR1Exec = SD_BusDrvCmdR1Exec,
  .Cmd3Exec = SD_BusDrvCmd3Exec,
  .Cmd7Exec = SD_BusDrvCmd7Exec,
  .CmdR3Exec = SD_BusDrvCmdR3Exec,
  .CmdR4Exec = SD_BusDrvCmdR4Exec,
  .CmdR5Exec = SD_BusDrvCmdR5Exec,
  .CmdR7Exec = SD_BusDrvCmdR7Exec,
  .CmdNoRespExec = SD_BusDrvCmdNoRespExec,
  .DataXferPrepare = SD_BusDrvDataXferPrepare,
  .DataXferCmpl = SD_BusDrvDataXferCmpl,
  .CardIntEnDis = SD_CtrlrDrvCardIntEnDis
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       SD_CardEventCardDetect()
 *
 * @brief      Notifies a card was inserted.
 *
 * @param      bus_handle      Handle to SD card bus.
 *******************************************************************************************************/
void SD_CardEventCardDetect(SD_CARD_BUS_HANDLE bus_handle)
{
  SD_BusCardDetectEvent(bus_handle->BusHandle);
}

/****************************************************************************************************//**
 *                                      SD_CardEventCardRemoval()
 *
 * @brief    Notifies a card was removed.
 *
 * @param    bus_handle      Handle to SD card bus.
 *******************************************************************************************************/
void SD_CardEventCardRemove(SD_CARD_BUS_HANDLE bus_handle)
{
  SD_BusCardRemoveEvent(bus_handle->BusHandle);
}

/****************************************************************************************************//**
 *                                         SD_CardEventCmdCmpl()
 *
 * @brief      Notifies of command completion.
 *
 * @param      bus_handle      Handle to SD card bus.
 *
 * @param      err             Error reported by driver.
 *******************************************************************************************************/
void SD_CardEventCmdCmpl(SD_CARD_BUS_HANDLE bus_handle,
                         RTOS_ERR           err)
{
  RTOS_ERR local_err;

  RTOS_ERR_COPY(bus_handle->CmdErr, err);

  KAL_SemPost(bus_handle->CmdCmplSem,
              KAL_OPT_POST_NONE,
              &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Posting CMD cmpl sem -> ", RTOS_ERR_LOG_ARG_GET(local_err)));
  }
}

/****************************************************************************************************//**
 *                                       SD_CardEventDataXferCmpl()
 *
 * @brief      Notifies of data transfer completion.
 *
 * @param      bus_handle      Handle to SD card bus.
 *
 * @param      err             Error reported by driver.
 *******************************************************************************************************/
void SD_CardEventDataXferCmpl(SD_CARD_BUS_HANDLE bus_handle,
                              RTOS_ERR           err)
{
  SD_AsyncEventPost(bus_handle->BusHandle,
                    SD_EVENT_TYPE_DATA_XFER_CMPL,
                    err);
}

/****************************************************************************************************//**
 *                                       SD_CardEventCardInt()
 *
 * @brief      Notifies a card interrupt occurred.
 *
 * @param      bus_handle      Handle to SD card bus.
 *******************************************************************************************************/
void SD_CardEventCardInt(SD_CARD_BUS_HANDLE bus_handle)
{
  SD_IO_CardIntEvent(bus_handle->BusHandle);
}

/********************************************************************************************************
 *                                           BUS DRV FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SD_BusDrvInit()
 *
 * @brief    Initializes bus driver.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_drv_info  Pointer to driver information retrieved from platform manager.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to bus driver data.
 *******************************************************************************************************/
static void *SD_BusDrvInit(SD_BUS_HANDLE bus_handle,
                           void          *p_drv_info,
                           RTOS_ERR      *p_err)
{
  CPU_SIZE_T                  buf_align;
  SD_CARD_CTRLR_DRV_INFO      *p_sd_card_info = (SD_CARD_CTRLR_DRV_INFO *)p_drv_info;
  const SD_CARD_CTRLR_DRV_API *p_drv_api = p_sd_card_info->DrvAPI_Ptr;
  SD_CARD_BUS_HANDLE          sd_card_bus_handle;

  sd_card_bus_handle = (SD_CARD_BUS_HANDLE)Mem_SegAlloc("IO - SD Card Bus",
                                                        SD_InitCfg.MemSegPtr,
                                                        sizeof(struct  sd_card_bus),
                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (DEF_NULL);
  }

  sd_card_bus_handle->BusHandle = bus_handle;
  sd_card_bus_handle->Drv.DataPtr = DEF_NULL;
  sd_card_bus_handle->Drv.HW_InfoPtr = &p_sd_card_info->HW_Info;
  sd_card_bus_handle->Drv.DrvAPI_Ptr = p_sd_card_info->DrvAPI_Ptr;
  sd_card_bus_handle->Drv.BSP_API_Ptr = p_sd_card_info->BSP_API_Ptr;
  bus_handle->CardDetectMode = p_sd_card_info->HW_Info.CardDetectMode;

  p_drv_api->Init(&sd_card_bus_handle->Drv,
                  sd_card_bus_handle,
                  SD_InitCfg.MemSegPtr,
                  p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  if (p_drv_api->ReqAlignGet != DEF_NULL) {
    buf_align = p_drv_api->ReqAlignGet(&sd_card_bus_handle->Drv);
  } else {
    buf_align = LIB_MEM_BUF_ALIGN_AUTO;
  }

  //                                                               Allocate buffer for SD CMD response.
  sd_card_bus_handle->RespBufPtr = (CPU_INT08U *)Mem_SegAllocHW("IO - SD Card Resp buffer",
                                                                SD_InitCfg.MemSegBufPtr,
                                                                SD_CARD_RESP_BUF_LEN,
                                                                buf_align,
                                                                DEF_NULL,
                                                                p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  sd_card_bus_handle->CmdCmplSem = KAL_SemCreate("IO - SD Cmd Cmpl Sem",
                                                 DEF_NULL,
                                                 p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  return ((void *)sd_card_bus_handle);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvInitHW()
 *
 * @brief    Initializes SD host controller.
 *
 * @param    p_bus_drv_data  Pointer to bus driver data.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Structure containing the SD host controller capabilities.
 *
 * @note     (1) As per "SD Specifications: Physical Layer specification" the default frequency of an SD
 *               card after a reset must be 400 kHz.
 *******************************************************************************************************/
static SD_CAPABILITIES SD_BusDrvInitHW(void     *p_bus_drv_data,
                                       RTOS_ERR *p_err)
{
  SD_CAPABILITIES             capabilities = { 0u };
  SD_CARD_BUS_HANDLE          sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API       *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  const SD_CARD_CTRLR_HW_INFO *p_hw_info = sd_card_bus_handle->Drv.HW_InfoPtr;

  p_api->InitHW(&sd_card_bus_handle->Drv,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (capabilities);
  }

  if (p_api->CapabilitiesGet != DEF_NULL) {
    capabilities.Host = p_api->CapabilitiesGet(&sd_card_bus_handle->Drv);
  }

  switch (p_hw_info->CardSignalVolt) {
    case SD_CARD_BUS_SIGNAL_VOLT_AUTO:
      capabilities.Card.Capabilities = SD_CAP_BUS_SIGNAL_VOLT_3_3 | SD_CAP_BUS_SIGNAL_VOLT_1_8;
      break;

    case SD_CARD_BUS_SIGNAL_VOLT_3_3:
      capabilities.Card.Capabilities = SD_CAP_BUS_SIGNAL_VOLT_3_3;
      break;

    case SD_CARD_BUS_SIGNAL_VOLT_1_8:
      capabilities.Card.Capabilities = SD_CAP_BUS_SIGNAL_VOLT_1_8;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, capabilities);
  }

  return (capabilities);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvStart()
 *
 * @brief      Starts SD bus driver.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvStart(void     *p_bus_drv_data,
                           RTOS_ERR *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  p_api->Start(&sd_card_bus_handle->Drv,
               p_err);
}

/****************************************************************************************************//**
 *                                               SD_BusDrvStop()
 *
 * @brief      Stops SD bus driver.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvStop(void     *p_bus_drv_data,
                          RTOS_ERR *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  if (p_api->Stop != DEF_NULL) {
    p_api->Stop(&sd_card_bus_handle->Drv,
                p_err);
  }
}

/****************************************************************************************************//**
 *                                       SD_BusDrvAlignReqGet()
 *
 * @brief    Gets bus driver buffer alignment requirement.
 *
 * @param    p_bus_drv_data  Pointer to bus driver data.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Required buffer alignment, in bytes.
 *******************************************************************************************************/
static CPU_SIZE_T SD_BusDrvAlignReqGet(void     *p_bus_drv_data,
                                       RTOS_ERR *p_err)
{
  CPU_SIZE_T            align = 0u;
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  if (p_api->ReqAlignGet != DEF_NULL) {
    align = p_api->ReqAlignGet(&sd_card_bus_handle->Drv);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
  }

  return (align);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvClkFreqSet()
 *
 * @brief      Sets SD bus clock frequency.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      freq            Frequency to set, in hertz.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvClkFreqSet(void       *p_bus_drv_data,
                                CPU_INT32U freq,
                                RTOS_ERR   *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  p_api->ClkFreqSet(&sd_card_bus_handle->Drv,
                    freq,
                    p_err);
}

/****************************************************************************************************//**
 *                                       SD_BusDrvBusSupplyVoltSet()
 *
 * @brief      Sets SD bus supply voltage.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      volt            Bus supply voltage to set.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvBusSupplyVoltSet(void             *p_bus_drv_data,
                                      SD_CARD_BUS_VOLT volt,
                                      RTOS_ERR         *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  p_api->BusSupplyVoltSet(&sd_card_bus_handle->Drv,
                          volt,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                       SD_BusDrvBusSignalVoltInit()
 *
 * @brief      Initialize SD bus signaling voltage.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvBusSignalVoltInit(void     *p_bus_drv_data,
                                       RTOS_ERR *p_err)
{
  SD_CARD_BUS_HANDLE          sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API       *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  const SD_CARD_CTRLR_HW_INFO *p_hw_info = sd_card_bus_handle->Drv.HW_InfoPtr;
  SD_BUS_HANDLE               bus_handle = sd_card_bus_handle->BusHandle;
  SD_CARD_BUS_SIGNAL_VOLT     volt_to_set = SD_CARD_BUS_SIGNAL_VOLT_AUTO;
  SD_CARD_BUS_SIGNAL_VOLT     volt_result;

  //                                                               If HC doesn't support switch to 1.8v, exit.
  if (!(DEF_BIT_IS_SET(bus_handle->Capabilities.Host.Capabilities, SD_CAP_BUS_SIGNAL_VOLT_1_8))) {
    RTOS_ASSERT_DBG_ERR_SET((p_hw_info->CardSignalVolt != SD_CARD_BUS_SIGNAL_VOLT_1_8), *p_err, RTOS_ERR_INVALID_CFG,; )
    return;
  }

  //                                                               Find the bus signaling voltage to start with.
  switch (p_hw_info->CardSignalVolt) {
    case SD_CARD_BUS_SIGNAL_VOLT_1_8:
    case SD_CARD_BUS_SIGNAL_VOLT_3_3:
      volt_to_set = p_hw_info->CardSignalVolt;
      break;

    case SD_CARD_BUS_SIGNAL_VOLT_AUTO:
      volt_to_set = SD_CARD_BUS_SIGNAL_VOLT_3_3;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               Set the signaling voltage in the HC.
  p_api->BusSignalVoltSet(&sd_card_bus_handle->Drv,
                          volt_to_set,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Wait 5ms for the voltage signaling to stabilize.
  KAL_Dly(5);

  //                                                               Validate that the voltage switch worked.
  volt_result = p_api->BusSignalVoltGet(&sd_card_bus_handle->Drv,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (volt_result != volt_to_set) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                       SD_BusDrvBusWidthSet()
 *
 * @brief      Sets SD bus width.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      width           Width of bus, in number of bits.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvBusWidthSet(void       *p_bus_drv_data,
                                 CPU_INT08U width,
                                 RTOS_ERR   *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  p_api->BusWidthSet(&sd_card_bus_handle->Drv,
                     width,
                     p_err);
}

/****************************************************************************************************//**
 *                                   SD_BusDrvBusSignalVoltSwitch()
 *
 * @brief      Starts the bus Signal Voltage Switch Sequence to use 1.8 volt for the signal lines.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvBusSignalVoltSwitch(void     *p_bus_drv_data,
                                         RTOS_ERR *p_err)
{
  SD_CARD_BUS_HANDLE      sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API   *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  SD_BUS_HANDLE           bus_handle = sd_card_bus_handle->BusHandle;
  CPU_INT08U              data_lvl_bitmap = 0;
  CPU_INT16U              timeout = 0;
  SD_CARD_BUS_SIGNAL_VOLT volt;

  //                                                               If SD Card/HC doesn't support switch to 1.8v, exit.
  if (!(DEF_BIT_IS_SET(bus_handle->Capabilities.Host.Capabilities, SD_CAP_BUS_SIGNAL_VOLT_1_8))
      || !(bus_handle->CardSupports1_8v)) {
    return;
  }

  //                                                               Send CMD11 to card to start Voltage Switch Sequence.
  (void)SD_BusDrvCmdR1Exec(p_bus_drv_data,
                           bus_handle->CardType,
                           SD_CMD_VOLTAGE_SWITCH,
                           DEF_BIT_NONE,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Stop providing SD clock to the card.
  p_api->ClkFreqSet(&sd_card_bus_handle->Drv,
                    0,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Check DAT[3:0] level. Must be 0000b to continue.
  timeout = DEF_INT_16U_MAX_VAL;
  do {
    data_lvl_bitmap = p_api->DataLinesLvlGet(&sd_card_bus_handle->Drv,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    timeout--;
  } while ((data_lvl_bitmap != SD_CARD_DATA_LINES_LVL_ALL_DOWN)
           && (timeout > 0));

  if (timeout == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  //                                                               Set 1.8V Signal Enable in the Host Controller reg.
  p_api->BusSignalVoltSet(&sd_card_bus_handle->Drv,
                          SD_CARD_BUS_SIGNAL_VOLT_1_8,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Wait 5 ms.
  KAL_Dly(5);

  //                                                               Check that 1.8V Signal Enable was not cleared by HC.
  volt = p_api->BusSignalVoltGet(&sd_card_bus_handle->Drv,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (volt != SD_CARD_BUS_SIGNAL_VOLT_1_8) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  //                                                               Provide SD Clock to the card again.
  p_api->ClkFreqSet(&sd_card_bus_handle->Drv,
                    SD_FREQ_DFLT_HZ,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Wait 1 ms.
  KAL_Dly(1);

  //                                                               Check DAT[3:0] level. At least one line must be high.
  timeout = DEF_INT_16U_MAX_VAL;
  do {
    data_lvl_bitmap = p_api->DataLinesLvlGet(&sd_card_bus_handle->Drv,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    timeout--;
  } while ((data_lvl_bitmap == SD_CARD_DATA_LINES_LVL_ALL_DOWN)
           && (timeout > 0));

  if (timeout == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                           SD_BusDrvCmdR1Exec()
 *
 * @brief    Executes a command that returns a response of type R1 (normal).
 *
 * @param    p_bus_drv_data  Pointer to bus driver data.
 *
 * @param    card_type       Type of card.
 *
 * @param    cmd_nbr         Command number.
 *
 * @param    arg             Command's argument.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Current SD card status.
 *******************************************************************************************************/
static SD_STATUS SD_BusDrvCmdR1Exec(void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    CPU_INT08U  cmd_nbr,
                                    CPU_INT32U  arg,
                                    RTOS_ERR    *p_err)
{
  CPU_INT08U            cur_state;
  CPU_INT32U            r1_resp;
  SD_CARD_RESP_TYPE     resp_type = SD_CARD_RESP_TYPE_R1;
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  SD_STATUS             card_status = DEF_BIT_NONE;

  if ((cmd_nbr == SD_CMD_IO_SEND_OP_COND)
      || (cmd_nbr == SD_CMD_SWITCH_FUNC)
      || (cmd_nbr == SD_CMD_SEL_DESEL_CARD)
      || (cmd_nbr == SD_CMD_STOP_TRANSMISSION)
      || (cmd_nbr == SD_CMD_SET_WRITE_PROT)
      || (cmd_nbr == SD_CMD_CLR_WRITE_PROT)
      || (cmd_nbr == SD_CMD_ERASE)) {
    resp_type = SD_CARD_RESP_TYPE_R1B;
  }

  //                                                               ----------------- EXECUTE COMMAND ------------------
  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 cmd_nbr,
                 arg,
                 resp_type,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  //                                                               --------------- RD & PARSE RESPONSE ----------------
  p_api->CmdRespGet(&sd_card_bus_handle->Drv,
                    sd_card_bus_handle->RespBufPtr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  r1_resp = ((CPU_INT32U)*sd_card_bus_handle->RespBufPtr);
  cur_state = DEF_BIT_FIELD_RD(r1_resp, SD_CARD_R1_CUR_STATE_MASK);

  switch (cur_state) {
    case SD_CARD_CUR_STATE_IDLE:
      DEF_BIT_SET(card_status, SD_STATUS_IDLE);
      break;

    case SD_CARD_CUR_STATE_READY:
    case SD_CARD_CUR_STATE_IDENT:
    case SD_CARD_CUR_STATE_STBY:
    case SD_CARD_CUR_STATE_TRAN:
    case SD_CARD_CUR_STATE_DATA:
    case SD_CARD_CUR_STATE_RCV:
    case SD_CARD_CUR_STATE_PRG:
    case SD_CARD_CUR_STATE_DIS:
    default:
      break;
  }

  if (DEF_BIT_IS_SET(r1_resp, SD_CARD_R1_ERASE_RESET)) {
    DEF_BIT_SET(card_status, SD_STATUS_ERASE_RESET);
  }

  if (DEF_BIT_IS_SET(r1_resp, SD_CARD_R1_ILLEGAL_COMMAND)) {
    DEF_BIT_SET(card_status, SD_STATUS_ILLEGAL_CMD);
  }

  RTOS_ERR_SET(*p_err, DEF_BIT_IS_SET_ANY(r1_resp, SD_CARD_R1_ERR_ANY_MSK) ? RTOS_ERR_IO : RTOS_ERR_NONE);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  return (card_status);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvCmdR3Exec()
 *
 * @brief      Executes a command that returns a response of type R3.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      card_type       Type of card.
 *
 * @param      cmd_nbr         Command number.
 *
 * @param      arg             Command's argument.
 *
 * @param      p_r3_resp       Pointer to variable that will receive content of response.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvCmdR3Exec(void        *p_bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  cmd_nbr,
                               CPU_INT32U  arg,
                               CPU_INT32U  *p_r3_resp,
                               RTOS_ERR    *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  //                                                               ----------------- EXECUTE COMMAND ------------------
  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 cmd_nbr,
                 arg,
                 SD_CARD_RESP_TYPE_R3,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               --------------- RD & PARSE RESPONSE ----------------
  p_api->CmdRespGet(&sd_card_bus_handle->Drv,
                    sd_card_bus_handle->RespBufPtr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  *p_r3_resp = *((CPU_INT32U *)sd_card_bus_handle->RespBufPtr);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvCmdR4Exec()
 *
 * @brief      Executes a command that returns a response of type R4.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      card_type       Type of card.
 *
 * @param      cmd_nbr         Command number.
 *
 * @param      arg             Command's argument.
 *
 * @param      p_r4_resp       Pointer to structure that will receive content of response.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvCmdR4Exec(void           *p_bus_drv_data,
                               SD_CARDTYPE    card_type,
                               CPU_INT08U     cmd_nbr,
                               CPU_INT32U     arg,
                               SD_CMD_R4_DATA *p_r4_resp,
                               RTOS_ERR       *p_err)
{
  CPU_INT32U            r4_resp_data;
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  //                                                               ----------------- EXECUTE COMMAND ------------------
  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 cmd_nbr,
                 arg,
                 SD_CARD_RESP_TYPE_R4,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               --------------- RD & PARSE RESPONSE ----------------
  p_api->CmdRespGet(&sd_card_bus_handle->Drv,
                    sd_card_bus_handle->RespBufPtr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  r4_resp_data = *((CPU_INT32U *)sd_card_bus_handle->RespBufPtr);

  p_r4_resp->IO_OCR = DEF_BIT_FIELD_RD(r4_resp_data, SD_CARD_R4_IO_OCR_MSK);
  p_r4_resp->MemPresent = DEF_BIT_IS_SET(r4_resp_data, SD_CARD_R4_MEM_PRES);
  p_r4_resp->IO_FnctNbr = DEF_BIT_FIELD_RD(r4_resp_data, SD_CARD_R4_NBR_IO_FNCT_MSK);
  p_r4_resp->CardRdy = DEF_BIT_IS_SET(r4_resp_data, SD_CARD_R4_C);
  p_r4_resp->Switch18_Accepted = DEF_BIT_IS_SET(r4_resp_data, SD_CARD_R4_S18A);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvCmd3Exec()
 *
 * @brief    Executes command 3 (SEND_RELATIVE_ADDR).
 *
 * @param    p_bus_drv_data  Pointer to bus driver data.
 *
 * @param    card_type       Type of card.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Current SD card status.
 *
 * @note     (1) This command has its own driver function since it is a SD card mode specific command.
 *
 * @note     (2) The Relative Card Address (RCA) is never returned to the core layer. This is entirely
 *               handled by this driver.
 *******************************************************************************************************/
static SD_STATUS SD_BusDrvCmd3Exec(void        *p_bus_drv_data,
                                   SD_CARDTYPE card_type,
                                   RTOS_ERR    *p_err)
{
  CPU_INT08U            cur_state;
  CPU_INT32U            arg_field;
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  SD_STATUS             card_status = DEF_BIT_NONE;

  //                                                               ----------------- EXECUTE COMMAND ------------------
  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 SD_CMD_SEND_RELATIVE_ADDR,
                 DEF_BIT_NONE,
                 SD_CARD_RESP_TYPE_R6,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  //                                                               --------------- RD & PARSE RESPONSE ----------------
  p_api->CmdRespGet(&sd_card_bus_handle->Drv,
                    sd_card_bus_handle->RespBufPtr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  arg_field = *((CPU_INT32U *)sd_card_bus_handle->RespBufPtr);

  sd_card_bus_handle->RCA = DEF_BIT_FIELD_RD(arg_field,         // See note (2).
                                             SD_CARD_R6_ARG_FIELD_RCA_MSK);
  cur_state = DEF_BIT_FIELD_RD(arg_field,
                               SD_CARD_R6_ARG_FIELD_STATUS_STATE_MSK);

  if (sd_card_bus_handle->RCA == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return ((SD_STATUS)0u);
  }

  switch (cur_state) {
    case SD_CARD_CUR_STATE_IDLE:
      DEF_BIT_SET(card_status, SD_STATUS_IDLE);
      break;

    case SD_CARD_CUR_STATE_READY:
    case SD_CARD_CUR_STATE_IDENT:
    case SD_CARD_CUR_STATE_STBY:
    case SD_CARD_CUR_STATE_TRAN:
    case SD_CARD_CUR_STATE_DATA:
    case SD_CARD_CUR_STATE_RCV:
    case SD_CARD_CUR_STATE_PRG:
    case SD_CARD_CUR_STATE_DIS:
    default:
      break;
  }

  if (DEF_BIT_IS_SET(arg_field, SD_CARD_R6_ARG_FIELD_STATUS_ILLEGAL_CMD)) {
    DEF_BIT_SET(card_status, SD_STATUS_ILLEGAL_CMD);
  }

  RTOS_ERR_SET(*p_err, DEF_BIT_IS_SET_ANY(arg_field, SD_CARD_R6_ARG_FIELD_STATUS_ERR_ANY_MSK) ? RTOS_ERR_IO : RTOS_ERR_NONE);

  return (card_status);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvCmd7Exec()
 *
 * @brief    Executes command 7 (SELECT/DESELECT_CARD) and uses the address previously retrieved via
 *           command 3.
 *
 * @param    p_bus_drv_data  Pointer to bus driver data.
 *
 * @param    card_type       Type of card.
 *
 * @param    select          Boolean that indicates if card is selected or de-selected.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Current SD card status.
 *
 * @note     (1) This command has its own driver function since it is a SD card mode specific command.
 *******************************************************************************************************/
static SD_STATUS SD_BusDrvCmd7Exec(void        *p_bus_drv_data,
                                   SD_CARDTYPE card_type,
                                   CPU_BOOLEAN select,
                                   RTOS_ERR    *p_err)
{
  CPU_INT08U            cur_state;
  CPU_INT32U            r1_resp;
  CPU_INT32U            cmd_arg = 0u;
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  SD_STATUS             card_status = DEF_BIT_NONE;

  //                                                               ----------------- EXECUTE COMMAND ------------------
  if (select) {
    DEF_BIT_FIELD_WR(cmd_arg,
                     sd_card_bus_handle->RCA,
                     SD_CARD_CMD7_ARG_RCA_MSK);
  }

  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 SD_CMD_SEL_DESEL_CARD,
                 cmd_arg,
                 SD_CARD_RESP_TYPE_R1B,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  //                                                               --------------- RD & PARSE RESPONSE ----------------
  p_api->CmdRespGet(&sd_card_bus_handle->Drv,
                    sd_card_bus_handle->RespBufPtr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  r1_resp = ((CPU_INT32U)*sd_card_bus_handle->RespBufPtr);
  cur_state = DEF_BIT_FIELD_RD(r1_resp, SD_CARD_R1_CUR_STATE_MASK);

  switch (cur_state) {
    case SD_CARD_CUR_STATE_IDLE:
      DEF_BIT_SET(card_status, SD_STATUS_IDLE);
      break;

    case SD_CARD_CUR_STATE_READY:
    case SD_CARD_CUR_STATE_IDENT:
    case SD_CARD_CUR_STATE_STBY:
    case SD_CARD_CUR_STATE_TRAN:
    case SD_CARD_CUR_STATE_DATA:
    case SD_CARD_CUR_STATE_RCV:
    case SD_CARD_CUR_STATE_PRG:
    case SD_CARD_CUR_STATE_DIS:
    default:
      break;
  }

  if (DEF_BIT_IS_SET(r1_resp, SD_CARD_R1_ERASE_RESET)) {
    DEF_BIT_SET(card_status, SD_STATUS_ERASE_RESET);
  }

  if (DEF_BIT_IS_SET(r1_resp, SD_CARD_R1_ILLEGAL_COMMAND)) {
    DEF_BIT_SET(card_status, SD_STATUS_ILLEGAL_CMD);
  }

  RTOS_ERR_SET(*p_err, DEF_BIT_IS_SET_ANY(r1_resp, SD_CARD_R1_ERR_ANY_MSK) ? RTOS_ERR_IO : RTOS_ERR_NONE);

  return (card_status);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvCmdR5Exec()
 *
 * @brief    Executes a command that returns a response of type R5.
 *
 * @param    p_bus_drv_data  Pointer to bus driver data.
 *
 * @param    card_type       Type of card.
 *
 * @param    cmd_nbr         Command number.
 *
 * @param    arg             Command's argument.
 *
 * @param    p_rw_data       Pointer to data to write, if write command, or buffer that will contain read
 *                             data, if read command.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Current SD card status.
 *******************************************************************************************************/
static SD_STATUS SD_BusDrvCmdR5Exec(void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    CPU_INT08U  cmd_nbr,
                                    CPU_INT32U  arg,
                                    CPU_INT08U  *p_rw_data,
                                    RTOS_ERR    *p_err)
{
  CPU_INT08U            r5_flags;
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  SD_STATUS             card_status = DEF_BIT_NONE;

  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 cmd_nbr,
                 arg,
                 SD_CARD_RESP_TYPE_R5,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  p_api->CmdRespGet(&sd_card_bus_handle->Drv,
                    sd_card_bus_handle->RespBufPtr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (card_status);
  }

  if (p_rw_data != DEF_NULL) {
    *p_rw_data = sd_card_bus_handle->RespBufPtr[0u];
  }

  r5_flags = sd_card_bus_handle->RespBufPtr[1u];

  if (DEF_BIT_IS_SET(r5_flags, SD_CARD_R5_ILLEGAL_CMD)) {
    DEF_BIT_SET(card_status, SD_STATUS_ILLEGAL_CMD);
  }

  if (DEF_BIT_IS_SET_ANY(r5_flags, SD_CARD_R5_ERR_ANY_MSK)) {
    LOG_ERR(("Error in R5 response: ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_IO)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }

  return (card_status);
}

/****************************************************************************************************//**
 *                                           SD_BusDrvCmdR7Exec()
 *
 * @brief      Executes a command that returns a response of type R7.
 *
 * @param      p_bus_drv_data      Pointer to bus driver data.
 *
 * @param      card_type       Type of card.
 *
 * @param      cmd_nbr         Command number.
 *
 * @param      arg             Command's argument.
 *
 * @param      p_r7_resp       Pointer strcture that will contain R7 response fields.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvCmdR7Exec(void           *p_bus_drv_data,
                               SD_CARDTYPE    card_type,
                               CPU_INT08U     cmd_nbr,
                               CPU_INT32U     arg,
                               SD_CMD_R7_DATA *p_r7_resp,
                               RTOS_ERR       *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 cmd_nbr,
                 arg,
                 SD_CARD_RESP_TYPE_R7,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_api->CmdRespGet(&sd_card_bus_handle->Drv,
                    sd_card_bus_handle->RespBufPtr,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_r7_resp->ChkPatternEcho = sd_card_bus_handle->RespBufPtr[0u];
  p_r7_resp->VoltageAccepted = (sd_card_bus_handle->RespBufPtr[1u] & 0x0Fu);
}

/****************************************************************************************************//**
 *                                       SD_BusDrvCmdNoRespExec()
 *
 * @brief      Executes a command that does not return a response.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      card_type       Type of card.
 *
 * @param      cmd_nbr         Command number.
 *
 * @param      arg             Command's argument.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvCmdNoRespExec(void        *p_bus_drv_data,
                                   SD_CARDTYPE card_type,
                                   CPU_INT08U  cmd_nbr,
                                   CPU_INT32U  arg,
                                   RTOS_ERR    *p_err)
{
  SD_CARD_BUS_HANDLE sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;

  SD_CardCmdExec(sd_card_bus_handle,
                 card_type,
                 cmd_nbr,
                 arg,
                 SD_CARD_RESP_TYPE_NONE,
                 p_err);
}

/****************************************************************************************************//**
 *                                       SD_BusDrvDataXferPrepare()
 *
 * @brief      Prepare buffer for upcoming data transfer.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      card_type       Type of card.
 *
 * @param      p_buf           Pointer to buffer that contains data to transmit or will receives the data.
 *
 * @param      blk_nbr         Number of blocks to transfer.
 *
 * @param      blk_len         Length of a block, in octets.
 *
 * @param      dir_is_rd       Flag that indicates if data transfer is read or write.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvDataXferPrepare(void        *p_bus_drv_data,
                                     SD_CARDTYPE card_type,
                                     CPU_INT08U  *p_buf,
                                     CPU_INT32U  blk_nbr,
                                     CPU_INT32U  blk_len,
                                     CPU_BOOLEAN dir_is_rd,
                                     RTOS_ERR    *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  PP_UNUSED_PARAM(card_type);

  if (p_api->DataBufSubmit != DEF_NULL) {
    p_api->DataBufSubmit(&sd_card_bus_handle->Drv,
                         p_buf,
                         blk_nbr,
                         blk_len,
                         dir_is_rd,
                         p_err);
  }
}

/****************************************************************************************************//**
 *                                       SD_BusDrvDataXferCmpl()
 *
 * @brief      Completes data transfer.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      card_type       Type of card.
 *
 * @param      p_buf           Pointer to buffer that contains data to transmit or will receives the data.
 *
 * @param      blk_nbr         Number of blocks to transfer.
 *
 * @param      blk_len         Length of a block, in octets.
 *
 * @param      dir_is_rd       Flag that indicates if data transfer is read or write.
 *
 * @param      p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusDrvDataXferCmpl(void        *p_bus_drv_data,
                                  SD_CARDTYPE card_type,
                                  CPU_INT08U  *p_buf,
                                  CPU_INT32U  blk_nbr,
                                  CPU_INT32U  blk_len,
                                  CPU_BOOLEAN dir_is_rd,
                                  RTOS_ERR    *p_err)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  PP_UNUSED_PARAM(card_type);

  if (p_api->DataXferCmpl != DEF_NULL) {
    p_api->DataXferCmpl(&sd_card_bus_handle->Drv,
                        p_buf,
                        blk_nbr,
                        blk_len,
                        dir_is_rd,
                        p_err);
  }
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvCardIntEnDis()
 *
 * @brief      Enables/Disables the Card Interrupt in the Host Controller.
 *
 * @param      p_bus_drv_data  Pointer to bus driver data.
 *
 * @param      enable          DEF_YES, to enable Card Interrupt.
 *                             DEF_NO, to disable Card Interrupt.
 *******************************************************************************************************/
static void SD_CtrlrDrvCardIntEnDis(void        *p_bus_drv_data,
                                    CPU_BOOLEAN enable)
{
  SD_CARD_BUS_HANDLE    sd_card_bus_handle = (SD_CARD_BUS_HANDLE)p_bus_drv_data;
  SD_CARD_CTRLR_DRV_API *p_api_ptr = sd_card_bus_handle->Drv.DrvAPI_Ptr;

  p_api_ptr->CardIntEnDis(&sd_card_bus_handle->Drv, enable);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SD_CardCmdExec()
 *
 * @brief      Executes a command.
 *
 * @param      sd_card_bus_handle  Handle to SD card bus driver.
 *
 * @param      card_type           Type of card.
 *
 * @param      cmd_nbr             Command number.
 *
 * @param      arg                 Command argument.
 *
 * @param      resp_type           Command's response type.
 *
 * @param      p_err               Error pointer.
 *******************************************************************************************************/
static void SD_CardCmdExec(SD_CARD_BUS_HANDLE sd_card_bus_handle,
                           SD_CARDTYPE        card_type,
                           CPU_INT08U         cmd_nbr,
                           CPU_INT32U         arg,
                           SD_CARD_RESP_TYPE  resp_type,
                           RTOS_ERR           *p_err)
{
  CPU_INT08U            flags;
  SD_CARD_CTRLR_DRV_API *p_api = sd_card_bus_handle->Drv.DrvAPI_Ptr;
  SD_CARD_CMD           sd_cmd;
  SD_CARD_DATA_TYPE     data_type;
  SD_CARD_DATA_DIR      data_dir;

  PP_UNUSED_PARAM(card_type);

  flags = SD_Card_CmdFlag[resp_type];

  switch (cmd_nbr) {                                            // ------------------- GET DATA DIR -------------------
    case SD_CMD_READ_SINGLE_BLOCK:
    case SD_CMD_READ_MULTIPLE_BLOCK:
    case SD_CMD_BUSTEST_R:
      data_dir = SD_CARD_DATA_DIR_CARD_TO_HOST;
      break;

    case SD_CMD_WRITE_DAT_UNTIL_STOP:
    case SD_CMD_WRITE_BLOCK:
    case SD_CMD_WRITE_MULTIPLE_BLOCK:
    case SD_CMD_BUSTEST_W:
      data_dir = SD_CARD_DATA_DIR_HOST_TO_CARD;
      break;

    case SD_CMD_STOP_TRANSMISSION:
      data_dir = SD_CARD_DATA_DIR_NONE;
      flags |= SD_CARD_CMD_FLAG_DATA_ABORT;
      break;

    case SD_CMD_IO_RW_EXTENDED:
      data_dir = (DEF_BIT_IS_SET(arg, SD_CMD53_ARG_RW_FLAG)) ? SD_CARD_DATA_DIR_HOST_TO_CARD
                 : SD_CARD_DATA_DIR_CARD_TO_HOST;
      break;

    default:
      data_dir = SD_CARD_DATA_DIR_NONE;
      break;
  }

  switch (cmd_nbr) {                                            // ------------------- GET DATA TYPE ------------------
    case SD_CMD_WRITE_DAT_UNTIL_STOP:
      data_type = SD_CARD_DATA_TYPE_STREAM;
      break;

    case SD_CMD_READ_SINGLE_BLOCK:
    case SD_CMD_WRITE_BLOCK:
    case SD_CMD_BUSTEST_R:
    case SD_CMD_BUSTEST_W:
      data_type = SD_CARD_DATA_TYPE_SINGLE_BLOCK;
      break;

    case SD_CMD_READ_MULTIPLE_BLOCK:
    case SD_CMD_WRITE_MULTIPLE_BLOCK:
    case SD_CMD_IO_RW_EXTENDED:
      data_type = SD_CARD_DATA_TYPE_MULTI_BLOCK;
      break;

    default:
      data_type = SD_CARD_DATA_TYPE_NONE;
      break;
  }

  RTOS_ERR_SET(sd_card_bus_handle->CmdErr, RTOS_ERR_NONE);

  sd_cmd.Cmd = cmd_nbr;
  sd_cmd.Arg = arg;
  sd_cmd.DataType = data_type;
  sd_cmd.DataDir = data_dir;
  sd_cmd.Flags = flags;
  sd_cmd.RespType = resp_type;

  p_api->CmdStart(&sd_card_bus_handle->Drv,
                  &sd_cmd,
                  sd_card_bus_handle->RespBufPtr,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO_TIMEOUT)
        && (p_api->CtrlrReset != DEF_NULL)) {
      RTOS_ERR err_local;

      RTOS_ERR_SET(err_local, RTOS_ERR_NONE);
      //                                                           Reset CMD line to allow next command.
      p_api->CtrlrReset(&sd_card_bus_handle->Drv,
                        SD_CARD_CTRLR_RESET_LVL_CMD,
                        &err_local);
      if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
        LOG_ERR(("Reset host controller -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
      }
    }

    return;
  }

  SD_CardCmdEndWait(sd_card_bus_handle, p_err);
  if (((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO)
       || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT)
       || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO_TIMEOUT))
      && (p_api->CtrlrReset != DEF_NULL)) {
    RTOS_ERR err_local;

    RTOS_ERR_SET(err_local, RTOS_ERR_NONE);
    p_api->CtrlrReset(&sd_card_bus_handle->Drv,
                      SD_CARD_CTRLR_RESET_LVL_CMD,
                      &err_local);
    if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
      LOG_ERR(("Reset host controller -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
    }
  }
}

/****************************************************************************************************//**
 *                                           SD_CardCmdEndWait()
 *
 * @brief      Pends until command is completed.
 *
 * @param      sd_card_bus_handle  Handle to SD card bus driver.
 *
 * @param      p_err               Error pointer.
 *******************************************************************************************************/
static void SD_CardCmdEndWait(SD_CARD_BUS_HANDLE sd_card_bus_handle,
                              RTOS_ERR           *p_err)
{
  KAL_SemPend(sd_card_bus_handle->CmdCmplSem,
              KAL_OPT_PEND_NONE,
              SD_CARD_CMD_END_WAIT_TIMEOUT_MS,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  if (RTOS_ERR_CODE_GET(sd_card_bus_handle->CmdErr) != RTOS_ERR_NONE) {
    RTOS_ERR_COPY(*p_err, sd_card_bus_handle->CmdErr);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SD_AVAIL))
