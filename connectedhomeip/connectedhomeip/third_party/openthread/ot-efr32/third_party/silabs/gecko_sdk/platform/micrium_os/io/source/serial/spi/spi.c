/***************************************************************************//**
 * @file
 * @brief IO - Spi Core
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

#if (defined(RTOS_MODULE_IO_SERIAL_SPI_AVAIL))

#if (!defined(RTOS_MODULE_IO_SERIAL_AVAIL))

#error IO SPI module requires IO Serial module. Make sure it is part of your project and that \
  RTOS_MODULE_IO_SERIAL_AVAIL is defined in rtos_description.h.

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

#include  <io/source/serial/serial_priv.h>
#include  <io/source/serial/spi/spi_priv.h>

#include  <io/include/spi.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO

#define  SPI_INIT_CFG_DFLT                  {   \
    .MemSegPtr = DEF_NULL,                      \
    .SlaveHandleQty = LIB_MEM_BLK_QTY_UNLIMITED \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const SPI_INIT_CFG SPI_InitCfgDflt = SPI_INIT_CFG_DFLT;
SPI_INIT_CFG       SPI_InitCfg = SPI_INIT_CFG_DFLT;
#endif

SPI *SPI_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        SPI_ConfigureMemSeg()
 *
 * @brief    Sets the memory segment where file system internal data structures will be allocated.
 *
 * @param    p_seg   Pointer to a memory segment.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SPI_ConfigureMemSeg(MEM_SEG *p_seg)
{
  SPI_InitCfg.MemSegPtr = p_seg;
}
#endif

/****************************************************************************************************//**
 *                                       SPI_ConfigureSlaveHandleQty()
 *
 * @brief    Configure the maximum number of SPI slave handles for all the SPI busses.
 *
 * @param    handle_qty  Max number of SPI slave handles.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SPI_ConfigureSlaveHandleQty(CPU_SIZE_T handle_qty)
{
  SPI_InitCfg.SlaveHandleQty = handle_qty;
}
#endif

/****************************************************************************************************//**
 *                                               SPI_Init()
 *
 * @brief    Initializes the SPI module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @note     (1) This function is NOT thread-safe and should be called ONCE during system
 *               initialization, before multi-threading has begun.
 *
 * @note     (2) This function will not return an error -- any error conditions will trigger a failed
 *               assertion.
 *******************************************************************************************************/
void SPI_Init(RTOS_ERR *p_err)
{
  CPU_BOOLEAN kal_complete;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Assert that the system has all necessary features.
  kal_complete = KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_DEL, KAL_OPT_NONE);
  RTOS_ASSERT_CRITICAL(kal_complete, RTOS_ERR_NOT_SUPPORTED,; );

  Serial_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SPI_Ptr = (SPI *)Mem_SegAlloc("IO - SPI root struct",
                                SPI_InitCfg.MemSegPtr,
                                sizeof(SPI),
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SList_Init(&SPI_Ptr->BusHandleListHeadPtr);

  //                                                               Create memory pool for handles.
  Mem_DynPoolCreate("IO - SPI slave handles",
                    &SPI_Ptr->SlaveHandleMemPool,
                    SPI_InitCfg.MemSegPtr,
                    sizeof(struct spi_slave_handle),
                    sizeof(CPU_ALIGN),
                    (SPI_InitCfg.SlaveHandleQty == LIB_MEM_BLK_QTY_UNLIMITED) ? 0u : SPI_InitCfg.SlaveHandleQty,
                    SPI_InitCfg.SlaveHandleQty,
                    p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SERIAL_SPI_AVAIL))
