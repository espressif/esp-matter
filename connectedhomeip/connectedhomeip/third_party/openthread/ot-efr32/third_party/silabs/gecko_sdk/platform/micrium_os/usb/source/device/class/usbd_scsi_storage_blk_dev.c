/***************************************************************************//**
 * @file
 * @brief USB Device Msc Class Storage Driver - Block Device
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device MSC class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 *                                               INCLUDE FILES
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#define    RTOS_MODULE_CUR                      RTOS_CFG_MODULE_USBD

#include  "usbd_scsi_storage_priv.h"
#include   "../../../include/device/usbd_scsi_storage_blk_dev.h"

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <fs/include/fs_core.h>
#include  <fs/include/fs_blk_dev.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>

/********************************************************************************************************
 *                                               LOCAL DEFINES
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, MSD)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 *                                               LOCAL CONSTANTS
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               LOCAL DATA TYPES
 *******************************************************************************************************/

//                                                                 ----------- STORAGE DRIVER INTERNAL DATA -----------
typedef struct usbd_scsi_storage_data {
  FS_BLK_DEV_HANDLE BlkDevHandle;
} USBD_SCSI_STORAGE_DATA;

/********************************************************************************************************
 *                                               LOCAL TABLES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

static void USBD_SCSI_StorageInit(MEM_SEG               *p_mem_seg,
                                  USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                  RTOS_ERR              *p_err);

static void USBD_SCSI_StorageOpen(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                  CPU_CHAR              *media_name,
                                  RTOS_ERR              *p_err);

static void USBD_SCSI_StorageClose(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                   RTOS_ERR              *p_err);

static CPU_BOOLEAN USBD_SCSI_StorageCapacityGet(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                                USBD_SCSI_LB_QTY      *p_nbr_blks,
                                                CPU_INT32U            *p_blk_size);

static CPU_BOOLEAN USBD_SCSI_StorageRd(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                       USBD_SCSI_LBA         blk_addr,
                                       CPU_INT32U            nbr_blks,
                                       CPU_INT08U            *p_data_buf);

static CPU_BOOLEAN USBD_SCSI_StorageWr(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                       USBD_SCSI_LBA         blk_addr,
                                       CPU_INT32U            nbr_blks,
                                       CPU_INT08U            *p_data_buf);

/********************************************************************************************************
 *                                               DRIVER API
 *******************************************************************************************************/

USBD_SCSI_STORAGE_API USBD_SCSI_StorageBlkDevAPI = {
  USBD_SCSI_StorageInit,
  USBD_SCSI_StorageOpen,
  USBD_SCSI_StorageClose,
  USBD_SCSI_StorageCapacityGet,
  USBD_SCSI_StorageRd,
  USBD_SCSI_StorageWr
};

/********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_SCSI_StorageInit()
 *
 * @brief    Initialize storage medium.
 *
 * @param    p_mem_seg       Pointer to data memory segment.
 *
 * @param    p_storage_lun   Pointer to logical unit storage structure.
 *
 * @param    p_storage_cfg   Pointer to storage driver LU configurations structure.
 *
 * @param    p_err           Pointer to variable that will receive error code from this function.
 *******************************************************************************************************/
static void USBD_SCSI_StorageInit(MEM_SEG               *p_mem_seg,
                                  USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                  RTOS_ERR              *p_err)
{
  USBD_SCSI_STORAGE_DATA *p_data;

  p_data = (USBD_SCSI_STORAGE_DATA *)Mem_SegAlloc("USBD - SCSI Storage FS drv data",
                                                  p_mem_seg,
                                                  sizeof(USBD_SCSI_STORAGE_DATA),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_data->BlkDevHandle = FSBlkDev_NullHandle;
  p_storage_lun->DataPtr = (void *)p_data;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBD_SCSI_StorageOpen()
 *
 * @brief    Open storage medium.
 *
 * @param    p_storage_lun   Pointer to logical unit storage structure.
 *
 * @param    media_name      Storage media name.
 *
 * @param    p_err           Pointer to error variable.
 *******************************************************************************************************/
static void USBD_SCSI_StorageOpen(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                  CPU_CHAR              *media_name,
                                  RTOS_ERR              *p_err)
{
  FS_MEDIA_HANDLE media_handle;

  USBD_SCSI_STORAGE_DATA *p_data = (USBD_SCSI_STORAGE_DATA *)p_storage_lun->DataPtr;
  media_handle = FSMedia_Get((const CPU_CHAR *)media_name);

  p_data->BlkDevHandle = FSBlkDev_Open(media_handle,
                                       p_err);
}

/****************************************************************************************************//**
 *                                           USBD_SCSI_StorageClose()
 *
 * @brief    Close storage medium.
 *
 * @param    p_storage_lun   Pointer to logical unit storage structure.
 *
 * @param    p_err           Pointer to error variable.
 *******************************************************************************************************/
static void USBD_SCSI_StorageClose(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                   RTOS_ERR              *p_err)
{
  USBD_SCSI_STORAGE_DATA *p_data = (USBD_SCSI_STORAGE_DATA *)p_storage_lun->DataPtr;

  FSBlkDev_Close(p_data->BlkDevHandle,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_data->BlkDevHandle = FSBlkDev_NullHandle;
  }
}

/****************************************************************************************************//**
 *                                       USBD_SCSI_StorageCapacityGet()
 *
 * @brief    Get storage medium's capacity.
 *
 * @param    p_storage_lun   Pointer to logical unit storage structure.
 *
 * @param    p_nbr_blks      Pointer to variable that will receive the number of logical blocks.
 *
 * @param    p_blk_size      Pointer to variable that will receive the size of each block, in bytes.
 *
 * @return   DEF_OK, if operation successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_SCSI_StorageCapacityGet(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                                USBD_SCSI_LB_QTY      *p_nbr_blks,
                                                CPU_INT32U            *p_blk_size)
{
  RTOS_ERR               err;
  USBD_SCSI_STORAGE_DATA *p_data = (USBD_SCSI_STORAGE_DATA *)p_storage_lun->DataPtr;

  *p_nbr_blks = FSBlkDev_LbCntGet(p_data->BlkDevHandle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  *p_blk_size = FSBlkDev_LbSizeGet(p_data->BlkDevHandle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                               USBD_StorageRd()
 *
 * @brief    Read data from the storage medium.
 *
 * @param    p_storage_lun   Pointer to logical unit storage structure.
 *
 * @param    blk_addr        Logical Block Address (LBA) of starting read block.
 *
 * @param    nbr_blks        Number of logical blocks to read.
 *
 * @param    p_data_buf      Pointer to buffer in which data will be stored.
 *
 * @return   DEF_OK, if operation successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_SCSI_StorageRd(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                       USBD_SCSI_LBA         blk_addr,
                                       CPU_INT32U            nbr_blks,
                                       CPU_INT08U            *p_data_buf)
{
  CPU_BOOLEAN            ok = DEF_OK;
  USBD_SCSI_STORAGE_DATA *p_data = (USBD_SCSI_STORAGE_DATA *)p_storage_lun->DataPtr;
  RTOS_ERR               err;

  FSBlkDev_Rd(p_data->BlkDevHandle,
              p_data_buf,
              blk_addr,
              nbr_blks,
              &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    ok = DEF_FAIL;
  }

  return (ok);
}

/****************************************************************************************************//**
 *                                               USBD_StorageWr()
 *
 * @brief    Write data to the storage medium.
 *
 * @param    p_storage_lun   Pointer to logical unit storage structure.
 *
 * @param    blk_addr        Logical Block Address (LBA) of starting write block.
 *
 * @param    nbr_blks        Number of logical blocks to write.
 *
 * @param    p-data_buf     Pointer to buffer in which data is stored.
 *
 * @return   DEF_OK, if operation successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBD_SCSI_StorageWr(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                                       USBD_SCSI_LBA         blk_addr,
                                       CPU_INT32U            nbr_blks,
                                       CPU_INT08U            *p_data_buf)
{
  CPU_BOOLEAN            ok = DEF_OK;
  USBD_SCSI_STORAGE_DATA *p_data = (USBD_SCSI_STORAGE_DATA *)p_storage_lun->DataPtr;
  RTOS_ERR               err;

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  FSBlkDev_Wr(p_data->BlkDevHandle,
              p_data_buf,
              blk_addr,
              nbr_blks,
              &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    ok = DEF_FAIL;
  }
#else
  return (DEF_FAIL);
#endif

  return (ok);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))
