/***************************************************************************//**
 * @file
 * @brief USB Device MSC Class
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
 * @defgroup USBD_MSC_SCSI USB Device MSC SCSI API
 * @ingroup USBD
 * @brief   USB Device MSC SCSI API
 *
 * @addtogroup USBD_MSC_SCSI
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SL_USBD_CLASS_MSC_SCSI_H
#define  SL_USBD_CLASS_MSC_SCSI_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sl_bit.h"
#include "sl_enum.h"
#include "sl_status.h"

#include "cmsis_os2.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_msc.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/// SCSI LBA Types
#if (SL_USBD_MSC_SCSI_64_BIT_LBA_EN == 1)
typedef uint64_t sl_usbd_msc_scsi_lba_t;
typedef uint64_t sl_usbd_msc_scsi_lb_qty_t;
#else
typedef uint32_t sl_usbd_msc_scsi_lb_qty_t;
typedef uint32_t sl_usbd_msc_scsi_lba_t;
#endif

/// SCSI Callback Functions
typedef const struct {
  void (*enable)(uint8_t class_nbr);

  void (*disable)(uint8_t class_nbr);

  void (*host_eject)(uint8_t class_nbr,
                     uint8_t lu_nbr);
} sl_usbd_msc_scsi_callbacks_t;

// SCSI Data Structures
typedef struct sl_usbd_msc_scsi_lun_api       sl_usbd_msc_scsi_lun_api_t;
typedef struct sl_usbd_msc_scsi_lun_info      sl_usbd_msc_scsi_lun_info_t;
typedef struct sl_usbd_msc_scsi_lun           sl_usbd_msc_scsi_lun_t;

/// SCSI Logical Unit Driver API
struct sl_usbd_msc_scsi_lun_api {
  sl_status_t (*init) (sl_usbd_msc_scsi_lun_t *p_scsi_lun);             ///< Initialize storage driver.

  sl_status_t (*open) (sl_usbd_msc_scsi_lun_t *p_scsi_lun);             ///< Request exclusive access to media.

  sl_status_t (*close) (sl_usbd_msc_scsi_lun_t *p_scsi_lun);            ///< Release exclusive access to media.

  sl_status_t (*get_capacity)(sl_usbd_msc_scsi_lun_t    *p_scsi_lun,
                              sl_usbd_msc_scsi_lb_qty_t *p_nbr_blks,
                              uint32_t                  *p_blk_size);   ///< Retrieve storage media capacity.

  sl_status_t (*read) (sl_usbd_msc_scsi_lun_t  *p_scsi_lun,
                       sl_usbd_msc_scsi_lba_t  blk_addr,
                       uint32_t                nbr_blks,
                       uint8_t                 *p_data_buf);            ///< Read data from storage media.

  sl_status_t (*write) (sl_usbd_msc_scsi_lun_t  *p_scsi_lun,
                        sl_usbd_msc_scsi_lba_t  blk_addr,
                        uint32_t                nbr_blks,
                        uint8_t                 *p_data_buf);           ///< Write data to storage media.
};

/// SCSI Logical Unit Info
struct sl_usbd_msc_scsi_lun_info {
  sl_usbd_msc_scsi_lun_api_t *scsi_lun_api_ptr;
  const char                 *vendor_id_ptr;
  const char                 *product_id_ptr;
  uint32_t                    product_revision_level;
  bool                        is_read_only;
};

/// SCSI Logical Unit
struct sl_usbd_msc_scsi_lun {
  uint8_t                     lun_index;                              ///< Index of this logical unit in the global SCSI lun table.

  const char                  *vendor_id_str;                         ///< Dev vendor info.
  const char                  *product_id_str;                        ///< Dev prod ID.
  uint32_t                    product_revision_level;                 ///< Revision level of product.
  bool                        is_read_only;                           ///< write protected or not.
  bool                        is_removable;                           ///< Flag that indicates if logical unit is removeable.

  uint8_t                     class_nbr;                              ///< The class number of the MSC instance.
  uint8_t                     lu_nbr;                                 ///< Logical unit nbr inside the class instance.

  sl_usbd_msc_scsi_lun_api_t  *scsi_lun_api_ptr;                      ///< Pointer to logical unit's driver API.
  void                        *data_ptr;                              ///< Pointer to storage driver's data.

  sl_usbd_msc_scsi_lb_qty_t   blocks_nbr;                             ///< Quantity of blocks.
  uint32_t                    block_size;                             ///< Size of blocks.

  sl_usbd_msc_scsi_lba_t      lb_addr;                                ///< Logical Blk Addr.
  uint32_t                    lb_count;                               ///< Nbr of mem blks.
  uint8_t                     sense_key;                              ///< Sense key describing an err or exception cond.
  uint8_t                     asc;                                    ///< Additional Sense Code describing sense key in detail.

  bool                        opened_flag;                            ///< Flag indicating logical unit locked or not.
  bool                        present_flag;                           ///< Flag indicating logical unit is present or not.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               MSC FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 * @brief    Initialize internal tables, variables and the storage layer.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_init(void);

/****************************************************************************************************//**
 * @brief    Add a new instance of SCSI Mass Storage Class.
 *
 * @param    msc_task_stack_size  The USB stack creates a task to handle the events of each MSC instance.
 *                                This parameter specifies the stack size to be used for the newly created
 *                                task for this instance.
 *
 * @param    msc_task_priority    The USB stack creates a task to handle the events of each MSC instance.
 *                                This parameter specifies the CMSIS priority of that task.
 *
 * @param    p_scsi_callbacks     Pointer to MSC SCSI callback structure.
 *                                [Content MUST be persistent]
 *
 * @param    p_class_nbr          Pointer to a variable that will receive the instance number for the created
 *                                class instance, if no errors are returned. Otherwise, the variable is set to
 *                                SL_USBD_CLASS_NBR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_create_instance(uint32_t                      msc_task_stack_size,
                                             osPriority_t                  msc_task_priority,
                                             sl_usbd_msc_scsi_callbacks_t *p_scsi_callbacks,
                                             uint8_t                      *p_class_nbr);

/****************************************************************************************************//**
 * @brief    Add an existing MSC SCSI instance to the specified configuration.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    config_nbr  Configuration index to which to add the existing MSC interface.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_add_to_configuration(uint8_t  class_nbr,
                                                  uint8_t  config_nbr);

/****************************************************************************************************//**
 * @brief    Initialize the specified logical unit.
 *
 * @param    class_nbr           MSC SCSI instance number.
 *
 * @param    p_lu_info           Pointer to logical unit information structure.
 *
 * @param    p_scsi_lun_ptr      Pointer to a void pointer that will be set to point to SCSI-related logical unit data.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_add(uint8_t                           class_nbr,
                                     sl_usbd_msc_scsi_lun_info_t       *p_lu_info,
                                     sl_usbd_msc_scsi_lun_t            **p_scsi_lun_ptr);

/****************************************************************************************************//**
 * @brief    Attach a storage medium to the given SCSI logical unit.
 *
 * @param    p_scsi_lun  Pointer to Logical Unit structure returned by sl_usbd_msc_scsi_lun_add().
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the logical unit has been ejected from the host, calling sl_usbd_msc_scsi_lun_detach() and
 *               sl_usbd_msc_scsi_lun_attach() will make it re-appear.
 *
 * @note     (2) Use this function with care as ejecting a logical unit in the middle of a
 *               transfer may corrupt the file system.
 *               The only scenarios where the usage of this function is safe are:
 *                   - (a) Host: Read, Embedded app: Read
 *                   - (b) Host: Read, Embedded app: Write
 *               The following scenarios are not considered safe. Use at your own risk.
 *                   - (a) Host: Write, Embedded app: Read
 *                   - (b) Host: Write, Embedded app: Write
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_attach(sl_usbd_msc_scsi_lun_t *p_scsi_lun);

/****************************************************************************************************//**
 * @brief    Detach a storage medium from the given SCSI logical unit.
 *
 * @param    p_scsi_lun  Pointer to Logical Unit structure returned by sl_usbd_msc_scsi_lun_add().
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) After a call to this function, the media will be available to the embedded application.
 *               The standard file API can be used.
 *
 * @note     (2) Use this function with care as ejecting a logical unit in the middle of a
 *               transfer may corrupt the file system.
 *               The only scenarios where the usage of this function is considered safe are:
 *                   - (a) Host: Read, Embedded app: Read
 *                   - (b) Host: Read, Embedded app: Write
 *               The following scenarios are not considered safe. Use at your own risk.
 *                   - (a) Host: Write, Embedded app: Read
 *                   - (b) Host: Write, Embedded app: Write
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_detach(sl_usbd_msc_scsi_lun_t *p_scsi_lun);

/****************************************************************************************************//**
 * @brief    Retrieve capacity of logical unit.
 *
 * @param    p_scsi_lun  Pointer to Logical Unit structure returned by sl_usbd_msc_scsi_lun_add().
 *
 * @param    p_lb_qty    Pointer to a variable that will receive LB quantity.
 *
 * @param    p_blk_size  Pointer to a variable that will receive block size.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_lun_get_capacity(sl_usbd_msc_scsi_lun_t     *p_scsi_lun,
                                              sl_usbd_msc_scsi_lb_qty_t  *p_lb_qty,
                                              uint32_t                   *p_blk_size);

/****************************************************************************************************//**
 * @brief    Get the MSC enable state of the SCSI interface.
 *
 * @param    class_nbr    MSC SCSI instance number.
 *
 * @param    p_enabled    Pointer to a boolean variable that will receive enable state.
 *                        The variable is set to true if MSC class is enabled.
 *                        The variable is set to false  if the MSC class is not enabled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_scsi_is_enabled(uint8_t  class_nbr,
                                        bool     *p_enabled);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
