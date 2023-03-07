/***************************************************************************//**
 * @file
 * @brief IO Serial Api
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

/****************************************************************************************************//**
 * @defgroup IO_SERIAL Serial API
 * @ingroup  IO
 * @brief      Serial API
 *
 * @addtogroup IO_SERIAL
 * @{
 ********************************************************************************************************
 */
#ifndef  _SERIAL_H_
#define  _SERIAL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------------- SERIAL CTRLR MODES ----------------
#define  SERIAL_CTRLR_MODE_NONE    DEF_BIT_NONE
#define  SERIAL_CTRLR_MODE_UART    DEF_BIT_00
#define  SERIAL_CTRLR_MODE_SPI     DEF_BIT_01
#define  SERIAL_CTRLR_MODE_I2C     DEF_BIT_02

/********************************************************************************************************
 *                                       SERIAL BUS MODE MACROS
 *
 * Note(s) : (1) The SPI bus mode setting determines the clock phase (CPHA) and clock polarity (CPOL)
 *               settings for SPI transfers. The clock phase setting determines when data are output and
 *               sampled, while the clock polarity setting determines whether the clock signal starts out
 *               high (1) or low (0).
 *
 *               CPHA: The least-significant bit of SPI_BUS_CFG.Mode represents the CPHA setting. If
 *                       CPHA = 0, then data are sampled on the clock's leading edge (e.g., the first clock
 *                       transition), and output on the trailing edge. If CPHA = 1, then this is
 *                       reversed -- data are output on the leading edge of the clock and sampled on the
 *                       trailing edge.
 *
 *               CPOL: The second to the least-significant bit represents the CPOL setting. This simply
 *                       determines whether the serial clock (SCLK) starts out low (CPOL = 0) or high
 *                       (CPOL = 1).
 *******************************************************************************************************/

#define  SERIAL_SPI_BUS_MODE_CPHA_BIT          DEF_BIT_00
#define  SERIAL_SPI_BUS_MODE_CPOL_BIT          DEF_BIT_01

#define  SERIAL_SPI_BUS_MODE_CPHA_GET(mode)    (DEF_BIT_IS_SET((mode), SERIAL_SPI_BUS_MODE_CPHA_BIT))
#define  SERIAL_SPI_BUS_MODE_CPOL_GET(mode)    (DEF_BIT_IS_SET((mode), SERIAL_SPI_BUS_MODE_CPOL_BIT))

/****************************************************************************************************//**
 *                                           IO_SERIAL_CTRLR_REG()
 *
 * @brief    Registers a IO serial controller to the platform manager.
 *
 * @param    name        Unique name for the IO serial controller. It is recommended to follow the
 *                       standard "serX", "spiX", "i2cX", "uartX" etc. where X is a digit.
 *
 * @param    p_drv_info  Pointer to the serial driver hardware information structure of type
 *                       SERIAL_CTRLR_DRV_INFO.
 *
 * @note     (1) This macro should normally be called from the BSP.
 *******************************************************************************************************/

#define  IO_SERIAL_CTRLR_REG(name, p_drv_info)                       \
  do {                                                               \
    if ((p_drv_info)->DrvAPI_Ptr != DEF_NULL) {                      \
      static const struct _serial_ctrlr_pm_item _item = {            \
        .Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_IO_SERIAL_CTRLR, \
        .Item.StrID = (name),                                        \
        .DrvInfoPtr = (const  SERIAL_CTRLR_DRV_INFO *)(p_drv_info)   \
      };                                                             \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_item.Item);      \
    }                                                                \
  } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                       SERIAL ISR SOURCE ENUM
 *******************************************************************************************************/
typedef enum serial_ctrlr_isr_src {
  SERIAL_CTRLR_ISR_SRC_NONE,                                    ///< Source of ISR retrieved from reg in driver.
  SERIAL_CTRLR_ISR_SRC_RX,                                      ///< Rx cmpl ISR.
  SERIAL_CTRLR_ISR_SRC_TX,                                      ///< Tx cmpl ISR.
  SERIAL_CTRLR_ISR_SRC_ERR,                                     ///< Serial bus error ISR.
  SERIAL_CTRLR_ISR_SRC_DMA_CMPL_RX,                             ///< Rx DMA xfer cmpl ISR.
  SERIAL_CTRLR_ISR_SRC_DMA_CMPL_TX,                             ///< Tx DMA xfer cmpl ISR.
} SERIAL_CTRLR_ISR_SRC;

/*
 ********************************************************************************************************
 *                                       SERIAL SLAVE INFO STRUCT
 *******************************************************************************************************/
typedef struct serial_slave_info {
  CPU_INT16U  Addr;                                             ///< Slave address.
  CPU_BOOLEAN ActiveLow;                                        ///< Flag that indicates if slave is active low or high.
} SERIAL_SLAVE_INFO;

/*
 ********************************************************************************************************
 *                                       SERIAL MODULE INIT CFG
 *******************************************************************************************************/

typedef struct serial_init_cfg {
  CPU_SIZE_T HandleQty;                                         ///< Max quantity of serial handle.
  MEM_SEG    *MemSegPtr;                                        ///< Ptr to mem segment from where to alloc internal data.
} SERIAL_INIT_CFG;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct serial_drv SERIAL_DRV;
typedef const struct serial_ctrlr_drv_api SERIAL_CTRLR_DRV_API;

/********************************************************************************************************
 *                                           ISR HANDLER FUNCTION
 *******************************************************************************************************/

typedef void (*SERIAL_CTRLR_ISR_HANDLE_FNCT)(SERIAL_DRV           *p_ser_drv,
                                             SERIAL_CTRLR_ISR_SRC isr_src);

/********************************************************************************************************
 *                                           DMA BSP API STRUCT
 *
 * TODO: Review API!
 *******************************************************************************************************/

typedef struct serial_ctrlr_bsp_dma_api {
  CPU_BOOLEAN (*CopyStart)(CPU_BOOLEAN is_rd,
                           CPU_INT08U  *buf_ptr,
                           CPU_REG32   *fifo_addr,
                           CPU_INT32U  xfer_len);

  CPU_INT08U (*ChStatusGet)(void);

  void (*ChStatusClr)(void);
} SERIAL_CTRLR_BSP_DMA_API;

/********************************************************************************************************
 *                                           BSP API STRUCT
 *******************************************************************************************************/

typedef struct serial_ctrlr_bsp_api {
  //                                                               Init BSP.
  CPU_BOOLEAN (*Init)(SERIAL_CTRLR_ISR_HANDLE_FNCT isr_fnct,
                      SERIAL_DRV                   *p_ser_drv);

  //                                                               Enable serial controller clock.
  CPU_BOOLEAN (*ClkEn)(void);

  //                                                               Init IO pin multiplexing.
  CPU_BOOLEAN (*IO_Cfg)(void);

  //                                                               Init interrupt control.
  CPU_BOOLEAN (*IntCfg)(void);

  //                                                               Init power control.
  CPU_BOOLEAN (*PwrCfg)(void);

  //                                                               Start communication.
  CPU_BOOLEAN (*Start)(void);

  //                                                               Stop communication.
  CPU_BOOLEAN (*Stop)(void);

  //                                                               Enable given slave select pin.
  CPU_BOOLEAN (*SlaveSel)(const SERIAL_SLAVE_INFO *p_slave_info);

  //                                                               Disable given slave select pin.
  CPU_BOOLEAN (*SlaveDesel)(const SERIAL_SLAVE_INFO *p_slave_info);

  //                                                               Retrieve serial controller clock frequency.
  CPU_INT32U (*ClkFreqGet)(void);

  //                                                               Ptr to DMA BSP API struct. DEF_NULL if no support.
  SERIAL_CTRLR_BSP_DMA_API *DMA_API_Ptr;

  //                                                               Pointer to extended BSP API. Refer to driver doc.
  void                     *BSP_API_ExtPtr;
} SERIAL_CTRLR_BSP_API;

/*
 ********************************************************************************************************
 *                                           SERIAL CTRLR HW INFO
 *******************************************************************************************************/

typedef struct serial_ctrlr_hw_info {
  CPU_INT08U SupportedMode;                                     ///< Bitmap of supported mode(s).
  CPU_ADDR   BaseAddr;                                          ///< Ctrlr's registers base address.
  const void *InfoExtPtr;                                       ///< Extended (drv specific) hardware info.
} SERIAL_CTRLR_HW_INFO;

/*
 ********************************************************************************************************
 *                                           SERIAL CTRLR INFO
 *******************************************************************************************************/

typedef struct serial_ctrlr_drv_info {
  SERIAL_CTRLR_HW_INFO       HW_Info;                           ///< HW info structure.
  const SERIAL_CTRLR_BSP_API *BSP_API_Ptr;                      ///< Pointer to BSP API struct.
  const SERIAL_CTRLR_DRV_API *DrvAPI_Ptr;                       ///< Pointer to driver API struct.
} SERIAL_CTRLR_DRV_INFO;

/****************************************************************************************************//**
 *                               SERIAL CONTROLLER PLATFORM MANAGER ITEM
 *
 * @note     (1) This structure should ONLY be instantiated via the macro IO_SERIAL_CTRLR_REG().
 *******************************************************************************************************/

struct _serial_ctrlr_pm_item {
  PLATFORM_MGR_ITEM           Item;
  const SERIAL_CTRLR_DRV_INFO *DrvInfoPtr;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Serial_ConfigureMemSeg(MEM_SEG *p_seg);
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Serial_ConfigureHandleQty(CPU_SIZE_T handle_qty);
#endif

CPU_SIZE_T Serial_CtrlrReqAlignGet(const CPU_CHAR *name);

#ifdef  __cplusplus
}
#endif

#endif

///< @}
