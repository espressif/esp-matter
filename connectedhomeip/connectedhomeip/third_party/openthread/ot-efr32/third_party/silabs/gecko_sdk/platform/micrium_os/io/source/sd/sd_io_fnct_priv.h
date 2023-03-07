/***************************************************************************//**
 * @file
 * @brief Sd IO Declarations
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

#ifndef  _SD_IO_FNCT_PRIV_H_
#define  _SD_IO_FNCT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/include/sd.h>
#include  <io/source/sd/sd_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SD_IO_FNCT_0                                               0u

#define  SD_IO_TPL_ADDR_INVALID                                     DEF_INT_32U_MAX_VAL

/********************************************************************************************************
 *                               STANDARD SD IO FUNCTION INTERFACE CODE
 *******************************************************************************************************/

#define  SD_IO_FNCT_IF_CODE_NONE_STD                                0u
#define  SD_IO_FNCT_IF_CODE_STD_UART                                1u
#define  SD_IO_FNCT_IF_CODE_BLUETOOTH_TYPE_A                        2u
#define  SD_IO_FNCT_IF_CODE_BLUETOOTH_TYPE_B                        3u
#define  SD_IO_FNCT_IF_CODE_GPS                                     4u
#define  SD_IO_FNCT_IF_CODE_CAMERA                                  5u
#define  SD_IO_FNCT_IF_CODE_PHS                                     6u
#define  SD_IO_FNCT_IF_CODE_WLAN                                    7u
#define  SD_IO_FNCT_IF_CODE_SDIO_ATA                                8u
#define  SD_IO_FNCT_IF_CODE_SEE_EXT                                 15u

/********************************************************************************************************
 *                                           SD IO FBR DEFINES
 *
 * Note(s) : (1) Function Basic Registers (FBR) are described in "SD Specifications: SDIO Simplified
 *               Specification", version 3.00, section 6.10.
 *******************************************************************************************************/

#define  SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr)                        ((fnct_nbr) * 0x0100u)
#define  SD_IO_FNCT_FBR_CSA_IF_CODE_ADDR(fnct_nbr)                 (SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr) + 0x00u)
#define  SD_IO_FNCT_FBR_EXT_IF_CODE_ADDR(fnct_nbr)                 (SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr) + 0x01u)
#define  SD_IO_FNCT_FBR_PWR_SEL_ADDR(fnct_nbr)                     (SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr) + 0x02u)
#define  SD_IO_FNCT_FBR_CIS_PTR_ADDR(fnct_nbr)                     (SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr) + 0x09u)
#define  SD_IO_FNCT_FBR_CSA_PTR_ADDR(fnct_nbr)                     (SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr) + 0x0Cu)
#define  SD_IO_FNCT_FBR_CSA_DATA_ACCESS_WINDOW_ADDR(fnct_nbr)      (SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr) + 0x0Fu)
#define  SD_IO_FNCT_FBR_IO_BLK_SIZE_ADDR(fnct_nbr)                 (SD_IO_FNCT_FBR_BASE_ADDR(fnct_nbr) + 0x10u)

#define  SD_IO_FNCT_FBR_IF_CODE_MSK                                 DEF_BIT_FIELD(4u, 0u)
#define  SD_IO_FNCT_FBR_CSA_SUPPORT                                 DEF_BIT_06
#define  SD_IO_FNCT_FBR_CSA_EN                                      DEF_BIT_07

#define  SD_IO_CIS_ADDR                                             0x1000u

/********************************************************************************************************
 *                                       STANDARD TUPLES DEFINE
 *
 * Note(s) : (1) Standard SDIO tuples are described in "SD Specifications: SDIO Simplified
 *               Specification", version 3.00, section 16.
 *******************************************************************************************************/

//                                                                 ----------------- SDIO TUPLE CODES -----------------
#define  SD_IO_TPL_CODE_NULL                0x00u
#define  SD_IO_TPL_CODE_CHECKSUM            0x10u
#define  SD_IO_TPL_CODE_VERS1               0x15u
#define  SD_IO_TPL_CODE_ALTSTR              0x16u
#define  SD_IO_TPL_CODE_MANFID              0x20u
#define  SD_IO_TPL_CODE_FUNCID              0x21u
#define  SD_IO_TPL_CODE_FUNCE               0x22u
#define  SD_IO_TPL_CODE_SDIO_STD            0x91u
#define  SD_IO_TPL_CODE_SDIO_EXT            0x92u
#define  SD_IO_TPL_CODE_SDIO_END            0xFFu

//                                                                 --------------- STANDARD TUPLE BYTES ---------------
#define  SD_IO_TPL_STD_BYTE_CODE            0x00u
#define  SD_IO_TPL_STD_BYTE_LINK            0x01u

//                                                                 ---------------- MANFID TUPLE BYTES ----------------
#define  SD_IO_TPL_MID_BYTE_MANF            0x02u
#define  SD_IO_TPL_MID_BYTE_CARD            0x04u

//                                                                 ---------------- FUNCID TUPLE BYTES ----------------
#define  SD_IO_TPL_FID_BYTE_FUNCTION        0x02u
#define  SD_IO_TPL_FID_BYTE_SYSINIT         0x03u

//                                                                 ----------------- FUNCE TUPLE BYTES ----------------
#define  SD_IO_TPL_FE_BYTE_TYPE             0x02u

#define  SD_IO_TPL_FE_TYPE_FUNC0            0x00u
#define  SD_IO_TPL_FE_TYPE_FUNC1_7          0x01u

//                                                                 FUNCE (fnct 0) tuple bytes.
#define  SD_IO_TPL_FE_BYTE_FN0_BLK_SIZE     0x03u
#define  SD_IO_TPL_FE_BYTE_MAX_TRAN_SPEED   0x05u

//                                                                 FUNCE (fnct 1-7) tuple bytes.
#define  SD_IO_TPL_FE_BYTE_FUNC_INFO        0x03u
#define  SD_IO_TPL_FE_BYTE_CARD_PSN         0x05u
#define  SD_IO_TPL_FE_BYTE_CSA_SIZE         0x09u
#define  SD_IO_TPL_FE_BYTE_CSA_PROPERTY     0x0Du
#define  SD_IO_TPL_FE_BYTE_MAX_BLK_SIZE     0x0Eu
#define  SD_IO_TPL_FE_BYTE_OCR              0x10u
#define  SD_IO_TPL_FE_BYTE_OP_MIN_PWR       0x14u
#define  SD_IO_TPL_FE_BYTE_OP_AVG_PWR       0x15u
#define  SD_IO_TPL_FE_BYTE_OP_MAX_PWR       0x16u
#define  SD_IO_TPL_FE_BYTE_SB_MIN_PWR       0x17u
#define  SD_IO_TPL_FE_BYTE_SB_AVG_PWR       0x18u
#define  SD_IO_TPL_FE_BYTE_SB_MAX_PWR       0x19u
#define  SD_IO_TPL_FE_BYTE_MIN_BW           0x1Au
#define  SD_IO_TPL_FE_BYTE_OPT_BW           0x1Cu
#define  SD_IO_TPL_FE_BYTE_EN_TIMEOUT_VAL   0x1Eu
#define  SD_IO_TPL_FE_BYTE_SP_AVG_PWR_33V   0x20u
#define  SD_IO_TPL_FE_BYTE_SP_MAX_PWR_33V   0x22u
#define  SD_IO_TPL_FE_BYTE_HP_AVG_PWR_33V   0x24u
#define  SD_IO_TPL_FE_BYTE_HP_MAX_PWR_33V   0x26u
#define  SD_IO_TPL_FE_BYTE_LP_AVG_PWR_33V   0x28u
#define  SD_IO_TPL_FE_BYTE_LP_MAX_PWR_33V   0x2Au

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                       SD IO function driver API
 *******************************************************************************************************/

typedef struct sd_io_fnct_drv_api {
  //                                                               Probe function driver.
  CPU_BOOLEAN (*CardFnctProbe)(SD_BUS_HANDLE  bus_handle,
                               SD_FNCT_HANDLE fnct_handle,
                               CPU_INT08U     sdio_fnct_if_code,
                               void           **pp_fnct_data);

  //                                                               Notifies function is ready for communication.
  void (*CardFnctConn)(SD_FNCT_HANDLE fnct_handle,
                       void           *p_fnct_data);

  //                                                               Notifies function trigerred an interruption.
  void (*CardFnctInt)(SD_FNCT_HANDLE fnct_handle,
                      void           *p_fnct_data);

  //                                                               Notifies function was disconnected.
  void (*CardFnctDisconn)(SD_FNCT_HANDLE fnct_handle,
                          void           *p_fnct_data);
} SD_IO_FNCT_DRV_API;

/*
 ********************************************************************************************************
 *                                       SD IO function driver entry
 *******************************************************************************************************/

struct sd_io_fnct_drv_entry {
  const SD_IO_FNCT_DRV_API    *FnctDrvAPI_Ptr;                  ///< Pointer to function driver API.
  struct sd_io_fnct_drv_entry *NextPtr;                         ///< Pointer to next entry in function drivers list.
};

/********************************************************************************************************
 *                                           ISR HANDLER FUNCTION
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   SPI MODULE CONFIGURATION STRUCT
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

void SD_IO_FnctDrvReg(const SD_IO_FNCT_DRV_API *p_fnct_drv_api,
                      RTOS_ERR                 *p_err);

CPU_INT32U SD_IO_FnctTplAddrGet(SD_BUS_HANDLE  bus_handle,
                                SD_FNCT_HANDLE fnct_handle,
                                CPU_INT08U     tpl_code,
                                RTOS_ERR       *p_err);

CPU_INT08U SD_IO_FnctRdByte(SD_BUS_HANDLE  bus_handle,
                            SD_FNCT_HANDLE fnct_handle,
                            CPU_INT32U     reg_addr,
                            RTOS_ERR       *p_err);

void SD_IO_FnctWrByte(SD_BUS_HANDLE  bus_handle,
                      SD_FNCT_HANDLE fnct_handle,
                      CPU_INT32U     reg_addr,
                      CPU_INT08U     byte,
                      RTOS_ERR       *p_err);

void SD_IO_FnctRd(SD_BUS_HANDLE  bus_handle,
                  SD_FNCT_HANDLE fnct_handle,
                  CPU_INT32U     reg_addr,
                  CPU_INT08U     *p_buf,
                  CPU_INT16U     buf_len,
                  CPU_BOOLEAN    fixed_addr,
                  RTOS_ERR       *p_err);

void SD_IO_FnctWr(SD_BUS_HANDLE  bus_handle,
                  SD_FNCT_HANDLE fnct_handle,
                  CPU_INT32U     reg_addr,
                  CPU_INT08U     *p_buf,
                  CPU_INT16U     buf_len,
                  CPU_BOOLEAN    fixed_addr,
                  RTOS_ERR       *p_err);

void SD_IO_FnctRdBlk(SD_BUS_HANDLE  bus_handle,
                     SD_FNCT_HANDLE fnct_handle,
                     CPU_INT32U     reg_addr,
                     CPU_INT08U     *p_buf,
                     CPU_INT16U     blk_nbr,
                     CPU_BOOLEAN    fixed_addr,
                     RTOS_ERR       *p_err);

void SD_IO_FnctWrBlk(SD_BUS_HANDLE  bus_handle,
                     SD_FNCT_HANDLE fnct_handle,
                     CPU_INT32U     reg_addr,
                     CPU_INT08U     *p_buf,
                     CPU_INT16U     blk_nbr,
                     CPU_BOOLEAN    fixed_addr,
                     RTOS_ERR       *p_err);

void SD_IO_FnctRdAsync(SD_BUS_HANDLE  bus_handle,
                       SD_FNCT_HANDLE fnct_handle,
                       CPU_INT32U     reg_addr,
                       CPU_INT08U     *p_buf,
                       CPU_INT16U     buf_len,
                       CPU_BOOLEAN    fixed_addr,
                       SD_ASYNC_FNCT  async_fnct,
                       void           *p_async_data,
                       RTOS_ERR       *p_err);

void SD_IO_FnctWrAsync(SD_BUS_HANDLE  bus_handle,
                       SD_FNCT_HANDLE fnct_handle,
                       CPU_INT32U     reg_addr,
                       CPU_INT08U     *p_buf,
                       CPU_INT16U     buf_len,
                       CPU_BOOLEAN    fixed_addr,
                       SD_ASYNC_FNCT  async_fnct,
                       void           *p_async_data,
                       RTOS_ERR       *p_err);

void SD_IO_FnctRdBlkAsync(SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          CPU_INT32U     reg_addr,
                          CPU_INT08U     *p_buf,
                          CPU_INT16U     blk_nbr,
                          CPU_BOOLEAN    fixed_addr,
                          SD_ASYNC_FNCT  async_fnct,
                          void           *p_async_data,
                          RTOS_ERR       *p_err);

void SD_IO_FnctWrBlkAsync(SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          CPU_INT32U     reg_addr,
                          CPU_INT08U     *p_buf,
                          CPU_INT16U     blk_nbr,
                          CPU_BOOLEAN    fixed_addr,
                          SD_ASYNC_FNCT  async_fnct,
                          void           *p_async_data,
                          RTOS_ERR       *p_err);

CPU_INT16U SD_IO_FnctBlkSizeGet(SD_BUS_HANDLE  bus_handle,
                                SD_FNCT_HANDLE fnct_handle,
                                RTOS_ERR       *p_err);

void SD_IO_FnctBlkSizeSet(SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          CPU_INT16U     blk_size,
                          RTOS_ERR       *p_err);

void SD_IO_FnctIntEnDis(SD_BUS_HANDLE  bus_handle,
                        SD_FNCT_HANDLE fnct_handle,
                        CPU_BOOLEAN    enable,
                        RTOS_ERR       *p_err);

CPU_BOOLEAN SD_IO_IsBlkOperSupported(SD_BUS_HANDLE bus_handle,
                                     RTOS_ERR      *p_err);

#ifdef  __cplusplus
}
#endif

#endif // _SD_IO_FNCT_PRIV_H_
