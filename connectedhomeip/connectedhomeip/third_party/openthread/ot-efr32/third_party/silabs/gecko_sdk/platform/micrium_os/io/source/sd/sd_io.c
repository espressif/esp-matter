/***************************************************************************//**
 * @file
 * @brief Sd IO Core
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

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <io/include/sd.h>

#include  <io/source/sd/sd_priv.h>
#include  <io/source/sd/sd_io_fnct_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (IO, SD)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO

#define  SD_IO_FNCT_EN_TIMEOUT_DLY_UNIT_MS  10u

#define  SD_IO_CMD5_RETRY_CNT               20u
#define  SD_IO_CMD5_RETRY_DLY_MS            50u

#define  SD_IO_FNCT_BLK_XFER_NBR_MAX        511u

#define  SD_IO_FNCT_BLK_SIZE_LEN_MIN        1u
#define  SD_IO_FNCT_BLK_SIZE_LEN_MAX        2048u

#define  SD_IO_FNCT_MULTI_BYTE_LEN_MIN      1u
#define  SD_IO_FNCT_MULTI_BYTE_LEN_MAX      512u

/********************************************************************************************************
 *                                           CCCR ADDR AND MASKS
 *******************************************************************************************************/

//                                                                 --------------------- REG ADDR ---------------------
#define  SD_IO_CCCR_SDIO_REV_ADDR                   0x00u
#define  SD_IO_CCCR_SD_SPEC_REV_ADDR                0x01u
#define  SD_IO_CCCR_IO_FNCT_EN_ADDR                 0x02u
#define  SD_IO_CCCR_IO_FNCT_RDY_ADDR                0x03u
#define  SD_IO_CCCR_INT_EN_ADDR                     0x04u
#define  SD_IO_CCCR_INT_PEND_ADDR                   0x05u
#define  SD_IO_CCCR_IO_ABORT_ADDR                   0x06u
#define  SD_IO_CCCR_BUS_IF_CTRL_ADDR                0x07u
#define  SD_IO_CCCR_CARD_CAP_ADDR                   0x08u
#define  SD_IO_CCCR_COM_CIS_PTR_ADDR                0x09u
#define  SD_IO_CCCR_BUS_SUSP_ADDR                   0x0Cu
#define  SD_IO_CCCR_FNCT_SEL_ADDR                   0x0Du
#define  SD_IO_CCCR_EXEC_FLAGS_ADDR                 0x0Eu
#define  SD_IO_CCCR_RDY_FLAGS_ADDR                  0x0Fu
#define  SD_IO_CCCR_FN0_BLK_SIZE_ADDR               0x10u
#define  SD_IO_CCCR_PWR_CTRL_ADDR                   0x12u
#define  SD_IO_CCCR_BUS_SPD_SEL_ADDR                0x13u

//                                                                 ------------------ CCCR SDIO REV -------------------
#define  SD_IO_CCCR_SDIO_REV_CCCR_MSK               DEF_BIT_FIELD(4u, 0u)
#define  SD_IO_CCCR_SDIO_REV_CCCR_1_00              0u
#define  SD_IO_CCCR_SDIO_REV_CCCR_1_10              1u
#define  SD_IO_CCCR_SDIO_REV_CCCR_2_00              2u
#define  SD_IO_CCCR_SDIO_REV_CCCR_3_00              3u

#define  SD_IO_CCCR_SDIO_REV_SDIO_MSK               DEF_BIT_FIELD(4u, 4u)
#define  SD_IO_CCCR_SDIO_REV_SDIO_1_00              0u
#define  SD_IO_CCCR_SDIO_REV_SDIO_1_10              1u
#define  SD_IO_CCCR_SDIO_REV_SDIO_1_20              2u
#define  SD_IO_CCCR_SDIO_REV_SDIO_2_00              3u
#define  SD_IO_CCCR_SDIO_REV_SDIO_3_00              4u

//                                                                 ------------------- SD SPEC REV --------------------
#define  SD_IO_CCCR_SD_SPEC_REV_MSK                 DEF_BIT_FIELD(4u, 0u)
#define  SD_IO_CCCR_SD_SPEC_REV_1_01                0u
#define  SD_IO_CCCR_SD_SPEC_REV_1_10                1u
#define  SD_IO_CCCR_SD_SPEC_REV_2_00                2u
#define  SD_IO_CCCR_SD_SPEC_REV_3_0x                3u

//                                                                 ------------ IO EN & RDY, INT EN & PEND ------------
#define  SD_IO_CCCR_IO_FNCT_BIT(fnct_nbr)           DEF_BIT(fnct_nbr)
#define  SD_IO_CCCR_INT_EN_MASTER                   DEF_BIT_00

//                                                                 --------------------- IO ABORT ---------------------
#define  SD_IO_CCCR_IO_ABORT_AS_FNCT_MSK            DEF_BIT_FIELD(3u, 0u)
#define  SD_IO_CCCR_IO_ABORT_RES                    DEF_BIT_03

//                                                                 ------------------- BUS IF CTRL --------------------
#define  SD_IO_CCCR_BUS_IF_CTRL_CD_DIS              DEF_BIT_07
#define  SD_IO_CCCR_BUS_IF_CTRL_SCSI                DEF_BIT_06
#define  SD_IO_CCCR_BUS_IF_CTRL_ECSI                DEF_BIT_05
#define  SD_IO_CCCR_BUS_IF_CTRL_S8B                 DEF_BIT_02
#define  SD_IO_CCCR_BUS_IF_CTRL_BUS_WIDTH_MSK      (DEF_BIT_01 | DEF_BIT_00)
#define  SD_IO_CCCR_BUS_IF_CTRL_BUS_WIDTH_1B        0u
#define  SD_IO_CCCR_BUS_IF_CTRL_BUS_WIDTH_4B        2u
#define  SD_IO_CCCR_BUS_IF_CTRL_BUS_WIDTH_8B        3u

//                                                                 ----------------- CARD CAPABILITY ------------------
#define  SD_IO_CCCR_CARD_CAP_4BLS                   DEF_BIT_07
#define  SD_IO_CCCR_CARD_CAP_LSC                    DEF_BIT_06
#define  SD_IO_CCCR_CARD_CAP_E4MI                   DEF_BIT_05
#define  SD_IO_CCCR_CARD_CAP_S4MI                   DEF_BIT_04
#define  SD_IO_CCCR_CARD_CAP_SBS                    DEF_BIT_03
#define  SD_IO_CCCR_CARD_CAP_SRW                    DEF_BIT_02
#define  SD_IO_CCCR_CARD_CAP_SMB                    DEF_BIT_01
#define  SD_IO_CCCR_CARD_CAP_SDC                    DEF_BIT_00

//                                                                 ----------------- BUS SPEED SELECT -----------------
#define  SD_IO_CCCR_BUS_SPD_SEL_SHS                 DEF_BIT_00
#define  SD_IO_CCCR_BUS_SPD_SEL_EHS                 DEF_BIT_01
#define  SD_IO_CCCR_BUS_SPD_SEL_BSS_MSK             DEF_BIT_FIELD(3u, 1u)

#define  SD_IO_CCCR_BUS_SPD_SEL_BSS_SDR12           0u
#define  SD_IO_CCCR_BUS_SPD_SEL_BSS_SDR25           1u
#define  SD_IO_CCCR_BUS_SPD_SEL_BSS_SDR50           2u
#define  SD_IO_CCCR_BUS_SPD_SEL_BSS_SDR104          3u
#define  SD_IO_CCCR_BUS_SPD_SEL_BSS_DDR50           4u

#define  SD_IO_SYNC_XFER_TIMEOUT_MS                 5000u

#define  SD_CMD53_ARG_SET(reg_addr, fnct_nbr, size, op_code, write, blk_mode)  (DEF_BIT_FIELD_ENC(reg_addr, SD_CMD53_ARG_REG_ADDR_MSK)                      \
                                                                                | DEF_BIT_FIELD_ENC(fnct_nbr, SD_CMD53_ARG_FNCT_NBR_MSK)                    \
                                                                                | DEF_BIT_FIELD_ENC(size, SD_CMD53_ARG_BYTE_BLK_CNT_MSK)                    \
                                                                                | DEF_BIT_FIELD_ENC(((op_code) == DEF_YES) ? 1u : 0u, SD_CMD53_ARG_OP_CODE) \
                                                                                | DEF_BIT_FIELD_ENC(((write) == DEF_YES) ? 1u : 0u, SD_CMD53_ARG_RW_FLAG)   \
                                                                                | DEF_BIT_FIELD_ENC(((blk_mode) == DEF_YES) ? 1u : 0u, SD_CMD53_ARG_BLK_MODE))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern SD *SD_Ptr;

extern SD_INIT_CFG SD_InitCfg;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                           SDIO function data
 *******************************************************************************************************/

struct  sd_io_fnct {
  SD_IO_FNCT_DRV_API *DrvAPI_Ptr;                               ///< Pointer to function driver API.
  void               *DrvDataPtr;                               ///< Pointer to function driver data.

  CPU_INT32U         CIS_Pointer;                               ///< Card Information Structure start pointer.
  CPU_INT32U         TplFunceAddr;                              ///< Address of FUNCE tuple for this function.

  CPU_INT16U         MaxBlkSize;                                ///< Maximum block size, in bytes.
  CPU_INT16U         BlkSize;                                   ///< Current block size, in bytes.

  SD_FNCT_HANDLE     Handle;                                    ///< Handle to SD IO function.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT08U SD_IO_RdDirect(SD_BUS_HANDLE bus_handle,
                                 CPU_INT08U    fnct_nbr,
                                 CPU_INT32U    reg_addr,
                                 RTOS_ERR      *p_err);

static void SD_IO_WrDirect(SD_BUS_HANDLE bus_handle,
                           CPU_INT08U    fnct_nbr,
                           CPU_INT32U    reg_addr,
                           CPU_INT08U    wr_data,
                           RTOS_ERR      *p_err);

static CPU_INT16U SD_IO_RdDirect16(SD_BUS_HANDLE bus_handle,
                                   CPU_INT08U    fnct_nbr,
                                   CPU_INT32U    reg_addr_base,
                                   RTOS_ERR      *p_err);

static CPU_INT32U SD_IO_RdDirect24(SD_BUS_HANDLE bus_handle,
                                   CPU_INT08U    fnct_nbr,
                                   CPU_INT32U    reg_addr_base,
                                   RTOS_ERR      *p_err);

static void SD_IO_WrDirect16(SD_BUS_HANDLE bus_handle,
                             CPU_INT08U    fnct_nbr,
                             CPU_INT32U    reg_addr_base,
                             CPU_INT16U    wr_data,
                             RTOS_ERR      *p_err);

static CPU_INT32U SD_IO_TplAddrget(SD_BUS_HANDLE bus_handle,
                                   CPU_INT08U    fnct_nbr,
                                   CPU_INT08U    tpl_code,
                                   RTOS_ERR      *p_err);

static void SD_IO_CardFnctsEn(SD_BUS_HANDLE bus_handle,
                              CPU_INT08U    fnct_en_msk,
                              RTOS_ERR      *p_err);

static void SD_IO_CardFnctDisconn(SD_BUS_HANDLE bus_handle,
                                  SD_IO_FNCT    *p_fnct,
                                  RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SD_IO_FnctDrvReg()
 *
 * @brief    Registers a SD IO function driver.
 *
 * @param    p_fnct_drv_api  Pointer to function driver API.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                             from this function:
 *                                 - RTOS_ERR_NONE
 *                                 - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void SD_IO_FnctDrvReg(const SD_IO_FNCT_DRV_API *p_fnct_drv_api,
                      RTOS_ERR                 *p_err)
{
  SD_IO_FNCT_DRV_ENTRY *p_entry;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_fnct_drv_api != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_entry = (SD_IO_FNCT_DRV_ENTRY *)Mem_SegAlloc("SD - IO fnct drv entry",
                                                 SD_InitCfg.MemSegPtr,
                                                 sizeof(SD_IO_FNCT_DRV_ENTRY),
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  p_entry->FnctDrvAPI_Ptr = p_fnct_drv_api;

  CORE_ENTER_ATOMIC();
  p_entry->NextPtr = SD_Ptr->IO_FnctDrvEntryHeadPtr;
  SD_Ptr->IO_FnctDrvEntryHeadPtr = p_entry;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       SD_IO_FnctTplAddrGet()
 *
 * @brief    Retrieves address of tuple of given code.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SDIO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    tpl_code        Tuple code.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                             from this function:
 *                                 - RTOS_ERR_NONE
 *                                 - RTOS_ERR_NOT_AVAIL
 *                                 - RTOS_ERR_WOULD_OVF
 *                                 - RTOS_ERR_OS_OBJ_DEL
 *                                 - RTOS_ERR_WOULD_BLOCK
 *                                 - RTOS_ERR_IS_OWNER
 *                                 - RTOS_ERR_OS_SCHED_LOCKED
 *                                 - RTOS_ERR_ABORT
 *                                 - RTOS_ERR_TIMEOUT
 *                                 - RTOS_ERR_IO
 *                                 - RTOS_ERR_IO_TIMEOUT
 *
 * @return    Tuple address.
 *******************************************************************************************************/
CPU_INT32U SD_IO_FnctTplAddrGet(SD_BUS_HANDLE  bus_handle,
                                SD_FNCT_HANDLE fnct_handle,
                                CPU_INT08U     tpl_code,
                                RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT32U tpl_addr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (0u);
  }

  tpl_addr = SD_IO_TplAddrget(bus_handle,
                              fnct_nbr,
                              tpl_code,
                              p_err);

  SD_BusUnlock(bus_handle);

  return (tpl_addr);
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctRdByte()
 *
 * @brief    Reads a byte from an SD IO function's registers.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                             from this function:
 *                                 - RTOS_ERR_NONE
 *                                 - RTOS_ERR_NOT_AVAIL
 *                                 - RTOS_ERR_WOULD_OVF
 *                                 - RTOS_ERR_OS_OBJ_DEL
 *                                 - RTOS_ERR_WOULD_BLOCK
 *                                 - RTOS_ERR_IS_OWNER
 *                                 - RTOS_ERR_OS_SCHED_LOCKED
 *                                 - RTOS_ERR_ABORT
 *                                 - RTOS_ERR_TIMEOUT
 *                                 - RTOS_ERR_IO
 *                                 - RTOS_ERR_IO_TIMEOUT
 *
 * @return    Byte read at given register address.
 *******************************************************************************************************/
CPU_INT08U SD_IO_FnctRdByte(SD_BUS_HANDLE  bus_handle,
                            SD_FNCT_HANDLE fnct_handle,
                            CPU_INT32U     reg_addr,
                            RTOS_ERR       *p_err)
{
  CPU_INT08U byte;
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (0u);
  }

  byte = SD_IO_RdDirect(bus_handle,
                        fnct_nbr,
                        reg_addr,
                        p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif

  SD_BusUnlock(bus_handle);

  return (byte);
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctWrByte()
 *
 * @brief    Reads a byte from an SD IO function's registers.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    byte            Byte to write.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                             from this function:
 *                                 - RTOS_ERR_NONE
 *                                 - RTOS_ERR_NOT_AVAIL
 *                                 - RTOS_ERR_WOULD_OVF
 *                                 - RTOS_ERR_OS_OBJ_DEL
 *                                 - RTOS_ERR_WOULD_BLOCK
 *                                 - RTOS_ERR_IS_OWNER
 *                                 - RTOS_ERR_OS_SCHED_LOCKED
 *                                 - RTOS_ERR_ABORT
 *                                 - RTOS_ERR_TIMEOUT
 *                                 - RTOS_ERR_IO
 *                                 - RTOS_ERR_IO_TIMEOUT
 *******************************************************************************************************/
void SD_IO_FnctWrByte(SD_BUS_HANDLE  bus_handle,
                      SD_FNCT_HANDLE fnct_handle,
                      CPU_INT32U     reg_addr,
                      CPU_INT08U     byte,
                      RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  SD_IO_WrDirect(bus_handle,
                 fnct_nbr,
                 reg_addr,
                 byte,
                 p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif

  SD_BusUnlock(bus_handle);
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctRd()
 *
 * @brief    Reads multiple bytes starting at given register address.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that will receive the read data.
 *
 * @param    buf_len         Length of buffer, in octets.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) According to "SDIO Simplified Specification", version 3.00, section 5.3 (Table 5-3), a
 *               byte cnt of 0 means 512 bytes to transfer.
 *
 * @note     (2) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctRd(SD_BUS_HANDLE  bus_handle,
                  SD_FNCT_HANDLE fnct_handle,
                  CPU_INT32U     reg_addr,
                  CPU_INT08U     *p_buf,
                  CPU_INT16U     buf_len,
                  CPU_BOOLEAN    fixed_addr,
                  RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U max_blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             (buf_len != 512u) ? buf_len : 0u,  // See note 1.
                             !fixed_addr,
                             DEF_NO,
                             DEF_NO);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  max_blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->MaxBlkSize
                 : bus_handle->IO_Fnct0MaxBlkSize;

  SD_BusUnlock(bus_handle);

  if ((buf_len < SD_IO_FNCT_MULTI_BYTE_LEN_MIN)
      || (buf_len > SD_IO_FNCT_MULTI_BYTE_LEN_MAX)
      || (buf_len > max_blk_size)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  SD_BusSyncXferExec(bus_handle,
                     fnct_handle,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_YES,
                     p_buf,
                     1u,
                     buf_len,
                     SD_IO_SYNC_XFER_TIMEOUT_MS,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctWr()
 *
 * @brief    Writes multiple bytes starting at given register address.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that contains the data to transmit.
 *
 * @param    buf_len         Length of buffer, in octets.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) According to "SDIO Simplified Specification", version 3.00, section 5.3 (Table 5-3), a
 *               byte cnt of 0 means 512 bytes to transfer.
 *
 * @note     (2) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctWr(SD_BUS_HANDLE  bus_handle,
                  SD_FNCT_HANDLE fnct_handle,
                  CPU_INT32U     reg_addr,
                  CPU_INT08U     *p_buf,
                  CPU_INT16U     buf_len,
                  CPU_BOOLEAN    fixed_addr,
                  RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U max_blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             (buf_len != 512u) ? buf_len : 0u,  // See note 1.
                             !fixed_addr,
                             DEF_YES,
                             DEF_NO);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  max_blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->MaxBlkSize
                 : bus_handle->IO_Fnct0MaxBlkSize;

  SD_BusUnlock(bus_handle);

  if ((buf_len < SD_IO_FNCT_MULTI_BYTE_LEN_MIN)
      || (buf_len > SD_IO_FNCT_MULTI_BYTE_LEN_MAX)
      || (buf_len > max_blk_size)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  SD_BusSyncXferExec(bus_handle,
                     fnct_handle,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_NO,
                     p_buf,
                     1u,
                     buf_len,
                     SD_IO_SYNC_XFER_TIMEOUT_MS,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctRdBlk()
 *
 * @brief    Reads a given number of blocks on SDIO function.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that will receive the read data.
 *
 * @param    blk_nbr         Number of blocks to read.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctRdBlk(SD_BUS_HANDLE  bus_handle,
                     SD_FNCT_HANDLE fnct_handle,
                     CPU_INT32U     reg_addr,
                     CPU_INT08U     *p_buf,
                     CPU_INT16U     blk_nbr,
                     CPU_BOOLEAN    fixed_addr,
                     RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             blk_nbr,
                             !fixed_addr,
                             DEF_NO,
                             DEF_YES);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->BlkSize
             : bus_handle->IO_Fnct0BlkSize;

  SD_BusUnlock(bus_handle);

  SD_BusSyncXferExec(bus_handle,
                     fnct_handle,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_YES,
                     p_buf,
                     blk_nbr,
                     blk_size,
                     SD_IO_SYNC_XFER_TIMEOUT_MS,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctWrBlk()
 *
 * @brief    Writes a given number of blocks on SDIO function.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that contains data to transmit.
 *
 * @param    blk_nbr         Number of blocks to write.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctWrBlk(SD_BUS_HANDLE  bus_handle,
                     SD_FNCT_HANDLE fnct_handle,
                     CPU_INT32U     reg_addr,
                     CPU_INT08U     *p_buf,
                     CPU_INT16U     blk_nbr,
                     CPU_BOOLEAN    fixed_addr,
                     RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             blk_nbr,
                             !fixed_addr,
                             DEF_YES,
                             DEF_YES);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->BlkSize
             : bus_handle->IO_Fnct0BlkSize;

  SD_BusUnlock(bus_handle);

  SD_BusSyncXferExec(bus_handle,
                     fnct_handle,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_NO,
                     p_buf,
                     blk_nbr,
                     blk_size,
                     SD_IO_SYNC_XFER_TIMEOUT_MS,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctRdAsync()
 *
 * @brief    Reads multiple bytes starting at given register address. Function is non-blocking.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that will receive the read data.
 *
 * @param    buf_len         Length of buffer, in octets.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_data    Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) According to "SDIO Simplified Specification", version 3.00, section 5.3 (Table 5-3), a
 *               byte cnt of 0 means 512 bytes to transfer.
 *
 * @note     (2) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctRdAsync(SD_BUS_HANDLE  bus_handle,
                       SD_FNCT_HANDLE fnct_handle,
                       CPU_INT32U     reg_addr,
                       CPU_INT08U     *p_buf,
                       CPU_INT16U     buf_len,
                       CPU_BOOLEAN    fixed_addr,
                       SD_ASYNC_FNCT  async_fnct,
                       void           *p_async_data,
                       RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U max_blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             (buf_len != 512u) ? buf_len : 0u,  // See note 1.
                             !fixed_addr,
                             DEF_NO,
                             DEF_NO);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  max_blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->MaxBlkSize
                 : bus_handle->IO_Fnct0MaxBlkSize;

  SD_BusUnlock(bus_handle);

  if ((buf_len < SD_IO_FNCT_MULTI_BYTE_LEN_MIN)
      || (buf_len > SD_IO_FNCT_MULTI_BYTE_LEN_MAX)
      || (buf_len > max_blk_size)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  SD_BusAsyncXferAdd(bus_handle,
                     fnct_handle,
                     async_fnct,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_YES,
                     p_buf,
                     1u,
                     buf_len,
                     p_async_data,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctWrAsync()
 *
 * @brief    Writes multiple bytes starting at given register address. Function is non-blocking.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that contains the data to transmit.
 *
 * @param    buf_len         Length of buffer, in octets.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_data    Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) According to "SDIO Simplified Specification", version 3.00, section 5.3 (Table 5-3), a
 *               byte cnt of 0 means 512 bytes to transfer.
 *
 * @note     (2) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctWrAsync(SD_BUS_HANDLE  bus_handle,
                       SD_FNCT_HANDLE fnct_handle,
                       CPU_INT32U     reg_addr,
                       CPU_INT08U     *p_buf,
                       CPU_INT16U     buf_len,
                       CPU_BOOLEAN    fixed_addr,
                       SD_ASYNC_FNCT  async_fnct,
                       void           *p_async_data,
                       RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U max_blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             (buf_len != 512u) ? buf_len : 0u,  // See note 1.
                             !fixed_addr,
                             DEF_YES,
                             DEF_NO);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  max_blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->MaxBlkSize
                 : bus_handle->IO_Fnct0MaxBlkSize;

  SD_BusUnlock(bus_handle);

  if ((buf_len < SD_IO_FNCT_MULTI_BYTE_LEN_MIN)
      || (buf_len > SD_IO_FNCT_MULTI_BYTE_LEN_MAX)
      || (buf_len > max_blk_size)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  SD_BusAsyncXferAdd(bus_handle,
                     fnct_handle,
                     async_fnct,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_NO,
                     p_buf,
                     1u,
                     buf_len,
                     p_async_data,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                       SD_IO_FnctRdBlkAsync()
 *
 * @brief    Reads a given number of blocks on SDIO function. Function is non-blocking.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that will receive the read data.
 *
 * @param    blk_nbr         Number of blocks to read.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_data    Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctRdBlkAsync(SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          CPU_INT32U     reg_addr,
                          CPU_INT08U     *p_buf,
                          CPU_INT16U     blk_nbr,
                          CPU_BOOLEAN    fixed_addr,
                          SD_ASYNC_FNCT  async_fnct,
                          void           *p_async_data,
                          RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET(((blk_nbr > 0u) && (blk_nbr <= SD_IO_FNCT_BLK_XFER_NBR_MAX)), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             blk_nbr,
                             !fixed_addr,
                             DEF_NO,
                             DEF_YES);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->BlkSize
             : bus_handle->IO_Fnct0BlkSize;

  SD_BusUnlock(bus_handle);

  SD_BusAsyncXferAdd(bus_handle,
                     fnct_handle,
                     async_fnct,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_YES,
                     p_buf,
                     blk_nbr,
                     blk_size,
                     p_async_data,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                       SD_IO_FnctWrBlkAsync()
 *
 * @brief    Writes a given number of blocks on SDIO function. Function is non-blocking.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr        Register address.
 *
 * @param    p_buf           Pointer to buffer that contains data to transmit.
 *
 * @param    blk_nbr         Number of blocks to write.
 *
 * @param    fixed_addr      Flag that indicates if the address should be incremented or not.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_data    Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *
 * @note     (1) This function CANNOT be called from the SD IO function driver callbacks context.
 *******************************************************************************************************/
void SD_IO_FnctWrBlkAsync(SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          CPU_INT32U     reg_addr,
                          CPU_INT08U     *p_buf,
                          CPU_INT16U     blk_nbr,
                          CPU_BOOLEAN    fixed_addr,
                          SD_ASYNC_FNCT  async_fnct,
                          void           *p_async_data,
                          RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U blk_size;
  CPU_INT32U cmd_arg = DEF_BIT_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET(((blk_nbr > 0u) && (blk_nbr <= SD_IO_FNCT_BLK_XFER_NBR_MAX)), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  cmd_arg = SD_CMD53_ARG_SET(reg_addr,
                             fnct_nbr,
                             blk_nbr,
                             !fixed_addr,
                             DEF_YES,
                             DEF_YES);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->BlkSize
             : bus_handle->IO_Fnct0BlkSize;

  SD_BusUnlock(bus_handle);

  SD_BusAsyncXferAdd(bus_handle,
                     fnct_handle,
                     async_fnct,
                     SD_CMD_IO_RW_EXTENDED,
                     cmd_arg,
                     DEF_NO,
                     p_buf,
                     blk_nbr,
                     blk_size,
                     p_async_data,
                     p_err);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if ((bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING)
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
    SD_BusCardRemoveEvent(bus_handle);                          // Polling mode: failed xfer considered as card removed.
  }
#endif
}

/****************************************************************************************************//**
 *                                       SD_IO_FnctBlkSizeGet()
 *
 * @brief    Gets SDIO block size for given function.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_NOT_SUPPORTED
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return    Block size, in octets.
 *******************************************************************************************************/
CPU_INT16U SD_IO_FnctBlkSizeGet(SD_BUS_HANDLE  bus_handle,
                                SD_FNCT_HANDLE fnct_handle,
                                RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U blk_size = 0u;
  CPU_INT16U reg_addr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (0u);
  }

  if (!bus_handle->IO_BlkOperSupported) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto err_unlock;
  }

  reg_addr = (fnct_handle != SD_IO_FNCT_0) ? SD_IO_FNCT_FBR_IO_BLK_SIZE_ADDR(fnct_nbr)
             : SD_IO_CCCR_FN0_BLK_SIZE_ADDR;

  blk_size = SD_IO_RdDirect16(bus_handle,
                              SD_IO_FNCT_0,
                              reg_addr,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Reading block size -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }

err_unlock:
  SD_BusUnlock(bus_handle);

  return (blk_size);
}

/****************************************************************************************************//**
 *                                       SD_IO_FnctBlkSizeSet()
 *
 * @brief    Sets SDIO block size for given function.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function.
 *
 * @param    blk_size        Block size to set, in octets.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_NOT_SUPPORTED
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_IO_TIMEOUT
 *******************************************************************************************************/
void SD_IO_FnctBlkSizeSet(SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          CPU_INT16U     blk_size,
                          RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT16U max_blk_size;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  max_blk_size = (fnct_handle != SD_IO_FNCT_0) ? bus_handle->IO_FnctTbl[fnct_nbr - 1u]->MaxBlkSize
                 : bus_handle->IO_Fnct0MaxBlkSize;

  if ((blk_size < SD_IO_FNCT_BLK_SIZE_LEN_MIN)
      || (blk_size > SD_IO_FNCT_BLK_SIZE_LEN_MAX)
      || (blk_size > max_blk_size)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  if (!bus_handle->IO_BlkOperSupported) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto err_unlock;
  }

  if (fnct_handle != SD_IO_FNCT_0) {
    SD_IO_WrDirect16(bus_handle,
                     SD_IO_FNCT_0,
                     SD_IO_FNCT_FBR_IO_BLK_SIZE_ADDR(fnct_nbr),
                     blk_size,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      bus_handle->IO_FnctTbl[fnct_nbr - 1u]->BlkSize = blk_size;
    }
  } else {
    SD_IO_WrDirect16(bus_handle,
                     SD_IO_FNCT_0,
                     SD_IO_CCCR_FN0_BLK_SIZE_ADDR,
                     blk_size,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      bus_handle->IO_Fnct0BlkSize = blk_size;
    }
  }

err_unlock:
  SD_BusUnlock(bus_handle);
}

/****************************************************************************************************//**
 *                                           SD_IO_FnctIntEnDis()
 *
 * @brief    Enable/Disable the interrupt for the specified SD IO function.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD IO function. SD_IO_FNCT_0 if CIA.
 *
 * @param    enable          DEF_YES, to enable the interrupt for the specified function.
 *                             DEF_NO, to disable the interrupt for the specified function.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
void SD_IO_FnctIntEnDis(SD_BUS_HANDLE  bus_handle,
                        SD_FNCT_HANDLE fnct_handle,
                        CPU_BOOLEAN    enable,
                        RTOS_ERR       *p_err)
{
  CPU_INT08U fnct_nbr = (CPU_INT08U)fnct_handle;
  CPU_INT08U reg_int_en;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr <= bus_handle->IO_FnctNbr), *p_err, RTOS_ERR_INVALID_HANDLE,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct_nbr != SD_IO_FNCT_0), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  //                                                               Read Int Enable Register.
  reg_int_en = SD_IO_RdDirect(bus_handle,
                              SD_IO_FNCT_0,
                              SD_IO_CCCR_INT_EN_ADDR,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (enable == DEF_YES) {
    //                                                             Set Master bit + Function Int bit.
    DEF_BIT_SET(reg_int_en, SD_IO_CCCR_INT_EN_MASTER);
    DEF_BIT_SET(reg_int_en, SD_IO_CCCR_IO_FNCT_BIT(fnct_nbr));
  } else {
    //                                                             Clear Function Int bit.
    DEF_BIT_CLR(reg_int_en, SD_IO_CCCR_IO_FNCT_BIT(fnct_nbr));
  }

  //                                                               Write to Int Enable Register.
  SD_IO_WrDirect(bus_handle,
                 SD_IO_FNCT_0,
                 SD_IO_CCCR_INT_EN_ADDR,
                 reg_int_en,
                 p_err);

exit_release:
  SD_BusUnlock(bus_handle);
}

/****************************************************************************************************//**
 *                                       SD_IO_IsBlkOperSupported()
 *
 * @brief    Indicates if SDIO card support block mode.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   DEF_YES, if SDIO card supports block mode.
 *           DEF_NO,  if SDIO card does not support block mode.
 *******************************************************************************************************/
CPU_BOOLEAN SD_IO_IsBlkOperSupported(SD_BUS_HANDLE bus_handle,
                                     RTOS_ERR      *p_err)
{
  CPU_BOOLEAN blk_oper_supported = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((bus_handle != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (blk_oper_supported);
  }

  blk_oper_supported = bus_handle->IO_BlkOperSupported;

  SD_BusUnlock(bus_handle);

  return (blk_oper_supported);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SD_IO_Init()
 *
 * @brief    Initializes IO part of SD module.
 *
 * @param    p_err   Error pointer..
 *******************************************************************************************************/
void SD_IO_Init(RTOS_ERR *p_err)
{
  SD_Ptr->IO_FnctDrvEntryHeadPtr = DEF_NULL;

  Mem_DynPoolCreate("SD - IO Fnct Pool",
                    &SD_Ptr->IO_FnctPool,
                    SD_InitCfg.MemSegPtr,
                    sizeof(SD_IO_FNCT),
                    sizeof(CPU_ALIGN),
                    (SD_InitCfg.IO_FnctQtyTot == LIB_MEM_BLK_QTY_UNLIMITED) ? 0u : SD_InitCfg.IO_FnctQtyTot,
                    SD_InitCfg.IO_FnctQtyTot,
                    p_err);
}

/****************************************************************************************************//**
 *                                           SD_IO_CardInit()
 *
 * @brief    Initializes the IO portion of an SD card.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) This function implements the basic SD IO initialization which consists on issuing CMD5 as
 *               described in "SD Specifications: SDIO Simplified Specification", version 3.00,
 *               section 3.1.2.
 *******************************************************************************************************/
void SD_IO_CardInit(SD_BUS_HANDLE bus_handle,
                    RTOS_ERR      *p_err)
{
  CPU_INT08U     retry_cnt;
  CPU_INT32U     cmd_arg;
  CPU_INT32U     ocr;
  SD_CMD_R4_DATA r4_resp;
  SD_BUS_DRV_API *p_bus_drv_api = (SD_BUS_DRV_API *)bus_handle->BusDrvApiPtr;

  bus_handle->IO_FnctNbr = 0u;
  bus_handle->IO_BlkOperSupported = DEF_NO;
  bus_handle->IO_SpecVer = SD_IO_CCCR_SDIO_REV_CCCR_1_00;
  bus_handle->IO_CCCR_Ver = SD_IO_CCCR_SDIO_REV_SDIO_1_00;

  //                                                               Issue CMD5 with no arg first.
  p_bus_drv_api->CmdR4Exec(bus_handle->BusDrvDataPtr,
                           bus_handle->CardType,
                           SD_CMD_IO_SEND_OP_COND,
                           0u,
                           &r4_resp,
                           p_err);
  if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO_TIMEOUT)        // Card is not SDIO.
      || (r4_resp.IO_FnctNbr == 0u)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Issue CMD5 again with valid OCR.
  ocr = r4_resp.IO_OCR & bus_handle->Capabilities.Host.OCR;
  if (ocr == DEF_BIT_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }

  cmd_arg = ocr;
  retry_cnt = SD_IO_CMD5_RETRY_CNT;

  //                                                               If host and Card supports 1.8V, request switch.
  if ((DEF_BIT_IS_SET(bus_handle->Capabilities.Host.Capabilities, SD_CAP_BUS_SIGNAL_VOLT_1_8))
      && (DEF_BIT_FIELD_RD(bus_handle->Capabilities.Card.Capabilities, SD_CAP_BUS_SIGNAL_VOLT_MASK) == (SD_CAP_BUS_SIGNAL_VOLT_1_8
                                                                                                        | SD_CAP_BUS_SIGNAL_VOLT_3_3))) {
    DEF_BIT_SET(cmd_arg, SD_CMD5_ARG_S18R);
  }

  do {
    p_bus_drv_api->CmdR4Exec(bus_handle->BusDrvDataPtr,
                             bus_handle->CardType,
                             SD_CMD_IO_SEND_OP_COND,
                             cmd_arg,
                             &r4_resp,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    if (r4_resp.CardRdy == DEF_NO) {
      KAL_Dly(SD_IO_CMD5_RETRY_DLY_MS);
    }

    retry_cnt--;
  } while ((retry_cnt > 0u)
           && (r4_resp.CardRdy == DEF_NO));

  if (r4_resp.CardRdy == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  bus_handle->IO_FnctNbr = r4_resp.IO_FnctNbr;
  bus_handle->CardSupports1_8v = r4_resp.Switch18_Accepted;
}

/****************************************************************************************************//**
 *                                           SD_IO_CardFnctsInit()
 *
 * @brief    Initializes SD IO function(s) and assign them a function driver.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void SD_IO_CardFnctsInit(SD_BUS_HANDLE bus_handle,
                         RTOS_ERR      *p_err)
{
  CPU_BOOLEAN    card_is_ls;
  CPU_INT08U     fnct_cnt;
  CPU_INT08U     fnct_en_msk = DEF_BIT_NONE;
  CPU_INT08U     reg_value;
  CPU_INT32U     tpl_addr;
  SD_BUS_DRV_API *p_bus_drv_api = (SD_BUS_DRV_API *)bus_handle->BusDrvApiPtr;

  //                                                               Read CIA (CCCR and FBR, CIS).
  reg_value = SD_IO_RdDirect(bus_handle,
                             SD_IO_FNCT_0,
                             SD_IO_CCCR_SDIO_REV_ADDR,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bus_handle->IO_SpecVer = DEF_BIT_FIELD_RD(reg_value, SD_IO_CCCR_SDIO_REV_SDIO_MSK);
  bus_handle->IO_CCCR_Ver = DEF_BIT_FIELD_RD(reg_value, SD_IO_CCCR_SDIO_REV_CCCR_MSK);

  //                                                               Read Common CIS Pointer.
  bus_handle->IO_CIS_ComPointer = SD_IO_RdDirect24(bus_handle,
                                                   SD_IO_FNCT_0,
                                                   SD_IO_CCCR_COM_CIS_PTR_ADDR,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Read card capability register.
  reg_value = SD_IO_RdDirect(bus_handle,
                             SD_IO_FNCT_0,
                             SD_IO_CCCR_CARD_CAP_ADDR,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bus_handle->IO_BlkOperSupported = DEF_BIT_IS_SET(reg_value, SD_IO_CCCR_CARD_CAP_SMB);
  card_is_ls = DEF_BIT_IS_SET(reg_value, SD_IO_CCCR_CARD_CAP_LSC);

  if (bus_handle->IO_BlkOperSupported) {
    //                                                             Retrieve current blk size for fnct 0.
    bus_handle->IO_Fnct0BlkSize = SD_IO_RdDirect16(bus_handle,
                                                   SD_IO_FNCT_0,
                                                   SD_IO_CCCR_FN0_BLK_SIZE_ADDR,
                                                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {
    bus_handle->IO_Fnct0BlkSize = 0u;
  }

  //                                                               Retrieve max blk size for fnct 0.
  tpl_addr = SD_IO_TplAddrget(bus_handle,
                              SD_IO_FNCT_0,
                              SD_IO_TPL_CODE_FUNCE,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bus_handle->IO_Fnct0MaxBlkSize = SD_IO_RdDirect16(bus_handle,
                                                    SD_IO_FNCT_0,
                                                    tpl_addr + SD_IO_TPL_FE_BYTE_FN0_BLK_SIZE,
                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Read card bus speed select register.
  reg_value = SD_IO_RdDirect(bus_handle,
                             SD_IO_FNCT_0,
                             SD_IO_CCCR_BUS_SPD_SEL_ADDR,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(reg_value, SD_IO_CCCR_BUS_SPD_SEL_SHS)) {
    DEF_BIT_SET(reg_value, SD_IO_CCCR_BUS_SPD_SEL_EHS);

    SD_IO_WrDirect(bus_handle,
                   SD_IO_FNCT_0,
                   SD_IO_CCCR_BUS_SPD_SEL_ADDR,
                   reg_value,
                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    KAL_Dly(1u);                                                // Must wait 8 clocks before switching freq.

    p_bus_drv_api->ClkFreqSet(bus_handle->BusDrvDataPtr,
                              50000000u,
                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else if (!card_is_ls) {
    //                                                             Card is Full-Speed.
    p_bus_drv_api->ClkFreqSet(bus_handle->BusDrvDataPtr,
                              25000000u,
                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {
    //                                                             Card is Low-Speed.
    //                                                             MICRIUM-692 Check CISTPL_FUNCE (for fnct 0) for more info on freq supported. For now, leave it to 400 khz.
  }

  for (fnct_cnt = 0u; fnct_cnt < bus_handle->IO_FnctNbr; fnct_cnt++) {
    CPU_INT08U           fnct_nbr = fnct_cnt + 1u;
    CPU_INT08U           fnct_if_code;
    SD_IO_FNCT_DRV_ENTRY *p_entry = SD_Ptr->IO_FnctDrvEntryHeadPtr;
    SD_IO_FNCT_DRV_API   *p_drv_api = DEF_NULL;
    void                 *p_fnct_drv_data;
    SD_IO_FNCT           *p_fnct = DEF_NULL;
    SD_EVENT_FNCTS       *p_event_fncts = SD_InitCfg.EventFnctsPtr;
    RTOS_ERR             err_fnct;

    p_fnct = (SD_IO_FNCT *)Mem_DynPoolBlkGet(&SD_Ptr->IO_FnctPool,
                                             &err_fnct);
    if (RTOS_ERR_CODE_GET(err_fnct) != RTOS_ERR_NONE) {
      RTOS_ERR_CONTEXT_REFRESH(err_fnct);
      goto fnct_conn_fail;
    }

    bus_handle->IO_FnctTbl[fnct_cnt] = p_fnct;

    p_fnct->Handle = (SD_FNCT_HANDLE)fnct_nbr;
    p_fnct->DrvAPI_Ptr = DEF_NULL;
    p_fnct->DrvDataPtr = DEF_NULL;

    //                                                             Read function interface code.
    reg_value = SD_IO_RdDirect(bus_handle,
                               SD_IO_FNCT_0,
                               SD_IO_FNCT_FBR_CSA_IF_CODE_ADDR(fnct_nbr),
                               &err_fnct);
    if (RTOS_ERR_CODE_GET(err_fnct) != RTOS_ERR_NONE) {
      goto fnct_conn_fail;
    }

    fnct_if_code = DEF_BIT_FIELD_RD(reg_value, SD_IO_FNCT_FBR_IF_CODE_MSK);

    if (fnct_if_code == SD_IO_FNCT_IF_CODE_SEE_EXT) {
      fnct_if_code = SD_IO_RdDirect(bus_handle,
                                    SD_IO_FNCT_0,
                                    SD_IO_FNCT_FBR_EXT_IF_CODE_ADDR(fnct_nbr),
                                    &err_fnct);
      if (RTOS_ERR_CODE_GET(err_fnct) != RTOS_ERR_NONE) {
        goto fnct_conn_fail;
      }
    }

    //                                                             Read function CIS Pointer.
    p_fnct->CIS_Pointer = SD_IO_RdDirect24(bus_handle,
                                           SD_IO_FNCT_0,
                                           SD_IO_FNCT_FBR_CIS_PTR_ADDR(fnct_nbr),
                                           &err_fnct);
    if (RTOS_ERR_CODE_GET(err_fnct) != RTOS_ERR_NONE) {
      goto fnct_conn_fail;
    }

    //                                                             Retrieve current function block size.
    p_fnct->BlkSize = SD_IO_RdDirect16(bus_handle,
                                       SD_IO_FNCT_0,
                                       SD_IO_FNCT_FBR_IO_BLK_SIZE_ADDR(fnct_nbr),
                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             Retrieve address of FUNCE tuple.
    p_fnct->TplFunceAddr = SD_IO_TplAddrget(bus_handle,
                                            fnct_nbr,
                                            SD_IO_TPL_CODE_FUNCE,
                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // If err, ignore timeout. Worst case fnct won't be en.
      LOG_ERR(("Reading function tuple FUNCE addr -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
      goto fnct_conn_fail;
    }

    p_fnct->MaxBlkSize = SD_IO_RdDirect16(bus_handle,
                                          SD_IO_FNCT_0,
                                          p_fnct->TplFunceAddr + SD_IO_TPL_FE_BYTE_MAX_BLK_SIZE,
                                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto fnct_conn_fail;
    }

    while (p_entry != DEF_NULL) {
      CPU_BOOLEAN        probe_ok;
      SD_IO_FNCT_DRV_API *p_drv_api_temp = (SD_IO_FNCT_DRV_API *)p_entry->FnctDrvAPI_Ptr;

      probe_ok = p_drv_api_temp->CardFnctProbe(bus_handle,
                                               (SD_FNCT_HANDLE)fnct_nbr,
                                               fnct_if_code,
                                               &p_fnct_drv_data);
      if (probe_ok) {
        p_drv_api = p_drv_api_temp;
        break;
      }

      p_entry = p_entry->NextPtr;
    }

    if (p_drv_api == DEF_NULL) {
      RTOS_ERR_SET(err_fnct, RTOS_ERR_FNCT_DRV_NOT_FOUND);
      goto fnct_conn_fail;
    }

    p_fnct->DrvAPI_Ptr = p_drv_api;
    p_fnct->DrvDataPtr = p_fnct_drv_data;

    DEF_BIT_SET(fnct_en_msk, SD_IO_CCCR_IO_FNCT_BIT(fnct_nbr));

    continue;

fnct_conn_fail:
    if (p_fnct != DEF_NULL) {
      RTOS_ERR err_disconn;

      SD_IO_CardFnctDisconn(bus_handle,
                            p_fnct,
                            &err_disconn);
      if (RTOS_ERR_CODE_GET(err_disconn) != RTOS_ERR_NONE) {
        LOG_ERR(("Disconnecting SD IO fnct -> ", RTOS_ERR_LOG_ARG_GET(err_disconn)));
      }

      bus_handle->IO_FnctTbl[fnct_cnt] = DEF_NULL;
    }

    if ((p_event_fncts != DEF_NULL)                             // Notify application of IO function init failure.
        && (p_event_fncts->IO_FnctConnFail != DEF_NULL)) {
      p_event_fncts->IO_FnctConnFail(bus_handle,
                                     fnct_nbr,
                                     err_fnct);
    }
  }

  if (fnct_en_msk != DEF_BIT_NONE) {
    SD_IO_CardFnctsEn(bus_handle,
                      fnct_en_msk,
                      p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
  }

  //                                                               Enable Card Interrupt.
  p_bus_drv_api->CardIntEnDis(bus_handle->BusDrvDataPtr,
                              DEF_YES);
}

/****************************************************************************************************//**
 *                                           SD_IO_CardRem()
 *
 * @brief    Frees resources used by SD IO module for given card and notifies function drivers of card
 *           removal.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void SD_IO_CardRem(SD_BUS_HANDLE bus_handle,
                   RTOS_ERR      *p_err)
{
  CPU_INT08U fnct_cnt;

  for (fnct_cnt = 0u; fnct_cnt < bus_handle->IO_FnctNbr; fnct_cnt++) {
    SD_IO_FNCT *p_fnct = bus_handle->IO_FnctTbl[fnct_cnt];

    if (p_fnct != DEF_NULL) {
      SD_IO_CardFnctDisconn(bus_handle,
                            p_fnct,
                            p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      bus_handle->IO_FnctTbl[fnct_cnt] = DEF_NULL;
    }
  }
}

/****************************************************************************************************//**
 *                                       SD_IO_CardBusWidthCapGet()
 *
 * @brief    Retrieves the bus widths that are supported by the SDIO Card.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Flags indicating the bus widths supported by the card.
 *******************************************************************************************************/
SD_CAP_BITMAP SD_IO_CardBusWidthCapGet(SD_BUS_HANDLE bus_handle,
                                       RTOS_ERR      *p_err)
{
  CPU_INT08U    reg_value;
  SD_CAP_BITMAP bus_width_supported = 0u;
  CPU_BOOLEAN   card_is_ls;
  CPU_BOOLEAN   card_4b_support;
  CPU_BOOLEAN   card_8b_support;

  reg_value = SD_IO_RdDirect(bus_handle,
                             SD_IO_FNCT_0,
                             SD_IO_CCCR_CARD_CAP_ADDR,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  card_is_ls = DEF_BIT_IS_SET(reg_value, SD_IO_CCCR_CARD_CAP_LSC);
  card_4b_support = DEF_BIT_IS_SET(reg_value, SD_IO_CCCR_CARD_CAP_4BLS);

  reg_value = SD_IO_RdDirect(bus_handle,
                             SD_IO_FNCT_0,
                             SD_IO_CCCR_BUS_IF_CTRL_ADDR,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  card_8b_support = DEF_BIT_IS_SET(reg_value, SD_IO_CCCR_BUS_IF_CTRL_S8B);

  if (card_8b_support == DEF_YES) {
    DEF_BIT_SET(bus_width_supported, SD_CAP_BUS_WIDTH_8_BIT);
  }

  if ((card_is_ls == DEF_YES && card_4b_support == DEF_YES)
      || (card_is_ls == DEF_NO)) {
    DEF_BIT_SET(bus_width_supported, SD_CAP_BUS_WIDTH_4_BIT);
  }

  DEF_BIT_SET(bus_width_supported, SD_CAP_BUS_WIDTH_1_BIT);

  return (bus_width_supported);
}

/****************************************************************************************************//**
 *                                           SD_IO_CardBusWidthSet()
 *
 * @brief    Sets the bus width to use in the SDIO Card.
 *
 * @param    bus_handle              Handle to SD bus.
 *
 * @param    bus_width_supported     Bus width value.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/
void SD_IO_CardBusWidthSet(SD_BUS_HANDLE bus_handle,
                           CPU_INT08U    bus_width_supported,
                           RTOS_ERR      *p_err)
{
  CPU_INT08U reg_value;

  reg_value = SD_IO_RdDirect(bus_handle,
                             SD_IO_FNCT_0,
                             SD_IO_CCCR_BUS_IF_CTRL_ADDR,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  switch (bus_width_supported) {
    case 1u:
      DEF_BIT_CLR(reg_value, (CPU_INT08U)SD_IO_CCCR_BUS_IF_CTRL_BUS_WIDTH_MSK);
      break;

    case 4u:
      reg_value |= SD_IO_CCCR_BUS_IF_CTRL_BUS_WIDTH_4B;
      break;

    case 8u:
      reg_value |= SD_IO_CCCR_BUS_IF_CTRL_BUS_WIDTH_8B;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  SD_IO_WrDirect(bus_handle,
                 SD_IO_FNCT_0,
                 SD_IO_CCCR_BUS_IF_CTRL_ADDR,
                 reg_value,
                 p_err);
}

/****************************************************************************************************//**
 *                                           SD_IO_CardIntHandle()
 *
 * @brief    Handles received Card Interrupt. Call SD IO Function Drivers with pending interrupt.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void SD_IO_CardIntHandle(SD_BUS_HANDLE bus_handle,
                         RTOS_ERR      *p_err)
{
  CPU_INT08U           reg_int_en;
  CPU_INT08U           reg_int_pend;
  CPU_BOOLEAN          master_int_en;
  CPU_INT08U           fnct_int_msk_temp;

  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  //                                                               Read Int Enable Register.
  reg_int_en = SD_IO_RdDirect(bus_handle,
                              SD_IO_FNCT_0,
                              SD_IO_CCCR_INT_EN_ADDR,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               Validate that the Master Int bit is not disabled.
  master_int_en = DEF_BIT_IS_SET(reg_int_en, SD_IO_CCCR_INT_EN_MASTER);
  if (master_int_en != DEF_ENABLED) {
    goto exit_release;
  }

  //                                                               Read Int Pending Register.
  reg_int_pend = SD_IO_RdDirect(bus_handle,
                                SD_IO_FNCT_0,
                                SD_IO_CCCR_INT_PEND_ADDR,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               Call Function Driver for each funct with active int.
  fnct_int_msk_temp = reg_int_pend;

  while (fnct_int_msk_temp != DEF_BIT_NONE) {
    SD_IO_FNCT *p_fnct;
    CPU_INT08U fnct_nbr;

    fnct_nbr = CPU_CntTrailZeros08(fnct_int_msk_temp);
    p_fnct = bus_handle->IO_FnctTbl[fnct_nbr - 1];

    if (DEF_BIT_IS_SET(reg_int_en, SD_IO_CCCR_IO_FNCT_BIT(fnct_nbr))) {
      p_fnct->DrvAPI_Ptr->CardFnctInt(p_fnct->Handle,
                                      p_fnct->DrvDataPtr);
    }

    DEF_BIT_CLR(fnct_int_msk_temp, DEF_BIT(fnct_nbr));
  }
  //                                                               Re-enabling the Card Interrupt is the responsibility of the application.
exit_release:
  SD_BusUnlock(bus_handle);
}

/****************************************************************************************************//**
 *                                       SD_IO_CardIntMasterEnDis()
 *
 * @brief    Enables/Disables the Card Interrupt in the SD Card by setting the Interrupt Master Bit.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    enable      DEF_YES, to enable Card interrupt inside the SD Card.
 *                       DEF_NO, to disable Card interrupt inside the SD Card.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void SD_IO_CardIntMasterEnDis(SD_BUS_HANDLE bus_handle,
                              CPU_BOOLEAN   enable,
                              RTOS_ERR      *p_err)
{
  CPU_INT08U reg_int_en;

  //                                                               Read Int Enable Register.
  reg_int_en = SD_IO_RdDirect(bus_handle,
                              SD_IO_FNCT_0,
                              SD_IO_CCCR_INT_EN_ADDR,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Set/Clear Master Interrupt flag.
  if (enable == DEF_YES) {
    DEF_BIT_SET(reg_int_en, SD_IO_CCCR_INT_EN_MASTER);
  } else {
    DEF_BIT_CLR(reg_int_en, (CPU_INT08U)SD_IO_CCCR_INT_EN_MASTER);
  }

  //                                                               Write Int Enable Register.
  SD_IO_WrDirect(bus_handle,
                 SD_IO_FNCT_0,
                 SD_IO_CCCR_INT_EN_ADDR,
                 reg_int_en,
                 p_err);
}

/****************************************************************************************************//**
 *                                       SD_IO_DataXferSubmit()
 *
 * @brief    Submits a SDIO data transfer.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    dir_is_rd       Flag that indicates if transfer is read.
 *
 * @param    cmd_arg         Arguments of the command.
 *
 * @param    p_buf           Pointer to buffer containing data to transfer or that will receive the read
 *                             data.
 *
 * @param    blk_qty         Quantity of blocks to transfer.
 *
 * @param    blk_len         Length of the blocks, in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @return   SD status.
 *
 * @note     The bus must be locked by the caller function.
 *******************************************************************************************************/
SD_STATUS SD_IO_DataXferSubmit(SD_BUS_HANDLE bus_handle,
                               CPU_BOOLEAN   dir_is_rd,
                               CPU_INT32U    cmd_arg,
                               CPU_INT08U    *p_buf,
                               CPU_INT16U    blk_qty,
                               CPU_INT16U    blk_len,
                               RTOS_ERR      *p_err)
{
  SD_STATUS            status = DEF_BIT_NONE;
  const SD_BUS_DRV_API *p_bus_drv_api = bus_handle->BusDrvApiPtr;

  p_bus_drv_api->DataXferPrepare(bus_handle->BusDrvDataPtr,
                                 bus_handle->CardType,
                                 p_buf,
                                 blk_qty,
                                 blk_len,
                                 dir_is_rd,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (status);
  }

  status = p_bus_drv_api->CmdR5Exec(bus_handle->BusDrvDataPtr,
                                    bus_handle->CardType,
                                    SD_CMD_IO_RW_EXTENDED,
                                    cmd_arg,
                                    DEF_NULL,
                                    p_err);

  return (status);
}

/****************************************************************************************************//**
 *                                           SD_IO_CardIntEvent()
 *
 * @brief    Report card interrupt event.
 *
 * @param    bus_handle      Handle to SD bus.
 *******************************************************************************************************/
void SD_IO_CardIntEvent(SD_BUS_HANDLE bus_handle)
{
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  SD_CoreEventPost(bus_handle,
                   SD_EVENT_TYPE_CARD_IO_INT,
                   err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SD_IO_RdDirect()
 *
 * @brief    Read a byte at given register address using CMD52.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    fnct_nbr    Function number. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr    Register address.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Read byte.
 *******************************************************************************************************/
static CPU_INT08U SD_IO_RdDirect(SD_BUS_HANDLE bus_handle,
                                 CPU_INT08U    fnct_nbr,
                                 CPU_INT32U    reg_addr,
                                 RTOS_ERR      *p_err)
{
  const SD_BUS_DRV_API *p_bus_drv_api = bus_handle->BusDrvApiPtr;
  CPU_INT32U           cmd_arg = DEF_BIT_NONE;
  CPU_INT08U           rd_data = 0u;

  DEF_BIT_FIELD_WR(cmd_arg, reg_addr, SD_CMD52_ARG_REG_ADDR_MSK);
  DEF_BIT_FIELD_WR(cmd_arg, fnct_nbr, SD_CMD52_ARG_FNCT_NBR_MSK);

  (void)p_bus_drv_api->CmdR5Exec(bus_handle->BusDrvDataPtr,
                                 bus_handle->CardType,
                                 SD_CMD_IO_RW_DIRECT,
                                 cmd_arg,
                                 &rd_data,
                                 p_err);

  return (rd_data);
}

/****************************************************************************************************//**
 *                                           SD_IO_WrDirect()
 *
 * @brief    Writes a byte at given register address using CMD52.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    fnct_nbr    Function number. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr    Register address.
 *
 * @param    wr_data     Byte to write.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) SD IO CMD 52 offers the possibility to perform a read after write to ensure the written
 *               byte is correct. This function always issues the command with this option enabled and
 *               ensures the read byte matches the byte that was written.
 *******************************************************************************************************/
static void SD_IO_WrDirect(SD_BUS_HANDLE bus_handle,
                           CPU_INT08U    fnct_nbr,
                           CPU_INT32U    reg_addr,
                           CPU_INT08U    wr_data,
                           RTOS_ERR      *p_err)
{
  const SD_BUS_DRV_API *p_bus_drv_api = bus_handle->BusDrvApiPtr;
  CPU_INT32U           cmd_arg = DEF_BIT_NONE;
  CPU_INT08U           rd_data = 0u;

  DEF_BIT_FIELD_WR(cmd_arg, reg_addr, SD_CMD52_ARG_REG_ADDR_MSK);
  DEF_BIT_FIELD_WR(cmd_arg, wr_data, SD_CMD52_ARG_WR_DATA_MSK);
  DEF_BIT_FIELD_WR(cmd_arg, fnct_nbr, SD_CMD52_ARG_FNCT_NBR_MSK);
  DEF_BIT_SET(cmd_arg, SD_CMD52_ARG_RW_FLAG);
  DEF_BIT_SET(cmd_arg, SD_CMD52_ARG_RAW_FLAG);                  // Perform a Read After Write. See note (1).

  (void)p_bus_drv_api->CmdR5Exec(bus_handle->BusDrvDataPtr,
                                 bus_handle->CardType,
                                 SD_CMD_IO_RW_DIRECT,
                                 cmd_arg,
                                 &rd_data,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (wr_data != rd_data) {                                     // Ensures written and read data matches. See note (1).
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                           SD_IO_RdDirect16()
 *
 * @brief    Reads a 16 bit value from registers using CMD52.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_nbr        Function number. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr_base   Register base address.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Read value.
 *******************************************************************************************************/
static CPU_INT16U SD_IO_RdDirect16(SD_BUS_HANDLE bus_handle,
                                   CPU_INT08U    fnct_nbr,
                                   CPU_INT32U    reg_addr_base,
                                   RTOS_ERR      *p_err)
{
  CPU_INT08U cnt;
  CPU_INT08U reg_value;
  CPU_INT16U value;
  CPU_INT16U value_tmp = 0u;

  for (cnt = 0u; cnt < 2u; cnt++) {
    reg_value = SD_IO_RdDirect(bus_handle,
                               fnct_nbr,
                               reg_addr_base + cnt,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    value_tmp |= (reg_value << (cnt * 8u));
  }

  MEM_VAL_SET_INT16U(&value, value_tmp);

  return (value);
}

/****************************************************************************************************//**
 *                                           SD_IO_RdDirect24()
 *
 * @brief    Reads a 24 bit value from registers using CMD52.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_nbr        Function number. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr_base   Register base address.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Read value.
 *******************************************************************************************************/
static CPU_INT32U SD_IO_RdDirect24(SD_BUS_HANDLE bus_handle,
                                   CPU_INT08U    fnct_nbr,
                                   CPU_INT32U    reg_addr_base,
                                   RTOS_ERR      *p_err)
{
  CPU_INT08U cnt;
  CPU_INT08U reg_value;
  CPU_INT32U value;
  CPU_INT32U value_tmp = 0u;

  for (cnt = 0u; cnt < 3u; cnt++) {
    reg_value = SD_IO_RdDirect(bus_handle,
                               fnct_nbr,
                               reg_addr_base + cnt,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    value_tmp |= (reg_value << (cnt * 8u));
  }

  MEM_VAL_SET_INT32U(&value, value_tmp);

  return (value);
}

/****************************************************************************************************//**
 *                                           SD_IO_WrDirect16()
 *
 * @brief    Writes a 16 bit value to registers using CMD52.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_nbr        Function number. SD_IO_FNCT_0 if CIA.
 *
 * @param    reg_addr_base   Register base address.
 *
 * @param    wr_data         Data to write.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void SD_IO_WrDirect16(SD_BUS_HANDLE bus_handle,
                             CPU_INT08U    fnct_nbr,
                             CPU_INT32U    reg_addr_base,
                             CPU_INT16U    wr_data,
                             RTOS_ERR      *p_err)
{
  CPU_INT08U cnt;
  CPU_INT08U reg_value;

  for (cnt = 0u; cnt < 2u; cnt++) {
    reg_value = (wr_data >> (cnt * 8u)) & DEF_INT_08_MASK;

    SD_IO_WrDirect(bus_handle,
                   fnct_nbr,
                   reg_addr_base + cnt,
                   reg_value,
                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           SD_IO_TplAddrget()
 *
 * @brief    Retrieves address of tuple of given code.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_nbr        Function number. SD_IO_FNCT_0 if CIA.
 *
 * @param    tpl_code        Tuple code.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Tuple address.
 *******************************************************************************************************/
static CPU_INT32U SD_IO_TplAddrget(SD_BUS_HANDLE bus_handle,
                                   CPU_INT08U    fnct_nbr,
                                   CPU_INT08U    tpl_code,
                                   RTOS_ERR      *p_err)
{
  CPU_INT08U tpl_code_cur;
  CPU_INT32U addr;

  if (fnct_nbr == SD_IO_FNCT_0) {
    addr = bus_handle->IO_CIS_ComPointer;
  } else {
    addr = bus_handle->IO_FnctTbl[fnct_nbr - 1u]->CIS_Pointer;
  }

  do {
    //                                                             Retrieve current tuple code.
    tpl_code_cur = SD_IO_RdDirect(bus_handle,
                                  SD_IO_FNCT_0,
                                  addr + SD_IO_TPL_STD_BYTE_CODE,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (SD_IO_TPL_ADDR_INVALID);
    }

    if ((tpl_code_cur != tpl_code)
        && (tpl_code_cur != 0xFF)) {                            // 0xFF = End of tuple chain.
      CPU_INT08U tpl_link;

      //                                                           Must go to next tuple. Retrieve current tuple link.
      tpl_link = SD_IO_RdDirect(bus_handle,
                                SD_IO_FNCT_0,
                                addr + SD_IO_TPL_STD_BYTE_LINK,
                                p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (SD_IO_TPL_ADDR_INVALID);
      }

      addr = addr + tpl_link + 2u;
    } else if (tpl_code_cur == 0xFF) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
      return (SD_IO_TPL_ADDR_INVALID);
    }
  } while (tpl_code_cur != tpl_code);

  return (addr);
}

/****************************************************************************************************//**
 *                                           SD_IO_CardFnctsEn()
 *
 * @brief    Enable given SD IO functions.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_en_msk     Mask of functions to enable.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) If CCCR version >= 1.10, each function provides an enable timeout (unit is 10 ms). This
 *               function retrieves the timeout value for all the functions and uses the longest one.
 *               If CCCR version == 1, then an arbitrary timeout is used.
 *
 * @note     (2) Poll the function ready field at regular interval until all functions are enabled, or
 *               the timeout is reached.
 *******************************************************************************************************/
static void SD_IO_CardFnctsEn(SD_BUS_HANDLE bus_handle,
                              CPU_INT08U    fnct_en_msk,
                              RTOS_ERR      *p_err)
{
  CPU_INT16U timeout = 0u;
  CPU_INT08U fnct_rdy_msk = DEF_BIT_NONE;
  CPU_INT08U fnct_en_msk_temp = fnct_en_msk;

  SD_IO_WrDirect(bus_handle,                                    // Enable all functions.
                 SD_IO_FNCT_0,
                 SD_IO_CCCR_IO_FNCT_EN_ADDR,
                 fnct_en_msk,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               See note (1).
  if (bus_handle->IO_CCCR_Ver >= SD_IO_CCCR_SD_SPEC_REV_1_10) {
    CPU_INT08U fnct_rdy_msk_temp = fnct_en_msk;

    while (fnct_rdy_msk_temp != DEF_BIT_NONE) {
      CPU_INT08U fnct_nbr;
      CPU_INT16U fnct_en_timeout;
      SD_IO_FNCT *p_fnct;

      fnct_nbr = CPU_CntTrailZeros08(fnct_rdy_msk_temp);
      p_fnct = bus_handle->IO_FnctTbl[fnct_nbr - 1u];

      fnct_en_timeout = SD_IO_RdDirect16(bus_handle,            // Retrieve fnct timeout value.
                                         SD_IO_FNCT_0,
                                         p_fnct->TplFunceAddr + SD_IO_TPL_FE_BYTE_EN_TIMEOUT_VAL,
                                         p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {         // If err, ignore timeout. Worst case fnct won't be en.
        LOG_ERR(("Reading function en timeout -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
        return;
      }

      timeout = DEF_MAX(timeout, fnct_en_timeout);              // Find fnct that has longest timeout.

      DEF_BIT_CLR(fnct_rdy_msk_temp, DEF_BIT(fnct_nbr));
    }
  } else {
    timeout = 10u;                                              // Use arbitrary timeout.
  }

  do {                                                          // See note (2).
    fnct_rdy_msk = SD_IO_RdDirect(bus_handle,                   // Read IO ready field.
                                  SD_IO_FNCT_0,
                                  SD_IO_CCCR_IO_FNCT_RDY_ADDR,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Reading function ready field -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    if ((fnct_rdy_msk & fnct_en_msk) != fnct_en_msk) {
      KAL_Dly(SD_IO_FNCT_EN_TIMEOUT_DLY_UNIT_MS);
    }

    if (timeout > 0u) {
      timeout--;
    }
  } while ( (timeout > 0u)
            && ((fnct_rdy_msk & fnct_en_msk) != fnct_en_msk));

  while (fnct_en_msk_temp != DEF_BIT_NONE) {
    CPU_INT08U     fnct_nbr;
    SD_EVENT_FNCTS *p_event_fncts = SD_InitCfg.EventFnctsPtr;
    SD_IO_FNCT     *p_fnct;

    fnct_nbr = CPU_CntTrailZeros08(fnct_en_msk_temp);
    p_fnct = bus_handle->IO_FnctTbl[fnct_nbr - 1u];

    if (DEF_BIT_IS_SET(fnct_rdy_msk, SD_IO_CCCR_IO_FNCT_BIT(fnct_nbr))) {
      //                                                           IO function ready. Notify function driver.
      p_fnct->DrvAPI_Ptr->CardFnctConn(p_fnct->Handle,
                                       p_fnct->DrvDataPtr);
    } else {
      RTOS_ERR err_local;

      RTOS_ERR_SET(err_local, RTOS_ERR_NONE);
      //                                                           IO function init failed.
      SD_IO_CardFnctDisconn(bus_handle,
                            p_fnct,
                            &err_local);
      LOG_ERR(("Disconnecting a SD IO fnct -> ", RTOS_ERR_LOG_ARG_GET(err_local)));

      bus_handle->IO_FnctTbl[fnct_nbr - 1u] = DEF_NULL;

      if ((p_event_fncts != DEF_NULL)                           // Notify application of IO function enable failure.
          && (p_event_fncts->IO_FnctConnFail != DEF_NULL)) {
        RTOS_ERR err_en;

        RTOS_ERR_SET(err_en, RTOS_ERR_NOT_READY);

        p_event_fncts->IO_FnctConnFail(bus_handle,
                                       fnct_nbr,
                                       err_en);
      }
    }

    DEF_BIT_CLR(fnct_en_msk_temp, DEF_BIT(fnct_nbr));
  }

  if (fnct_rdy_msk == DEF_BIT_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
  }
}

/****************************************************************************************************//**
 *                                       SD_IO_CardFnctDisconn()
 *
 * @brief    Disconnect given SD IO function.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_fnct          Pointer to function's data structure.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void SD_IO_CardFnctDisconn(SD_BUS_HANDLE bus_handle,
                                  SD_IO_FNCT    *p_fnct,
                                  RTOS_ERR      *p_err)
{
  SD_IO_FNCT_DRV_API *p_fnct_drv_api = p_fnct->DrvAPI_Ptr;

  PP_UNUSED_PARAM(bus_handle);

  if (p_fnct_drv_api != DEF_NULL) {
    p_fnct_drv_api->CardFnctDisconn(p_fnct->Handle,
                                    p_fnct->DrvDataPtr);
  }

  Mem_DynPoolBlkFree(&SD_Ptr->IO_FnctPool,
                     (void *)p_fnct,
                     p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SD_AVAIL))
