/***************************************************************************//**
 * @file
 * @brief USB Device - USB Msc Class - Private Header
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SLI_USBD_CLASS_MSC_H
#define  SLI_USBD_CLASS_MSC_H

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
 *                                                DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SLI_USBD_MSC_LEN_CBW                          31
#define  SLI_USBD_MSC_LEN_CSW                          13

// Size is in 4 bytes element
#define  SLI_USBD_MSC_CBW_BUFFER_SIZE                  (SLI_USBD_ROUND_INC_UP_PWR2(SLI_USBD_MSC_LEN_CBW, 4) / 4)
#define  SLI_USBD_MSC_CSW_BUFFER_SIZE                  (SLI_USBD_ROUND_INC_UP_PWR2(SLI_USBD_MSC_LEN_CSW, 4) / 4)
#define  SLI_USBD_MSC_DATA_BUFFER_SIZE                 (SLI_USBD_ROUND_INC_UP_PWR2(SL_USBD_MSC_DATA_BUFFER_SIZE, 4) / 4)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   COMMAND BLOCK WRAPPER DATA TYPE
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.1.
 *
 *           (2) The 'flags' field is a bit-mapped datum with three subfields :
 *
 *               (a) Bit  7  : Data transfer direction :
 *
 *                   (1) 0 = Data-out from host   to device.
 *                   (2) 1 = Data-in  from device to host.
 *
 *               (b) Bit  6  : Obsolete.  Should be set to zero.
 *               (c) Bits 5-0: Reserved.  Should be set to zero.
 *******************************************************************************************************/

typedef struct sli_usbd_msc_cbw {
  uint32_t signature;                                         ///< Sig that helps identify this data pkt as cbw.
  uint32_t tag;                                               ///< A cmd block tag sent by the host.
  uint32_t data_transfer_length;                              ///< Nbr of bytes of data that host expects to xfer.
  uint8_t flags;                                              ///< flags (see Notes #2).
  uint8_t lun;                                                ///< lun to which the cmd blk is being sent.
  uint8_t cbw_length;                                         ///< Length of the CBWB in bytes.
  uint8_t cb[16];                                             ///< Cmd blk to be executed by the dev.
} sli_usbd_msc_cbw_t;

/***************************************************************************************************//**
*                                   COMMAND STATUS WRAPPER DATA TYPE
*
* Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.2.
*******************************************************************************************************/

typedef struct sli_usbd_msc_csw {
  uint32_t signature;                                         ///< Sig that helps identify this data pkt as csw.
  uint32_t tag;                                               ///< Dev shall set this to the value in cbw's tag.
  uint32_t data_residue;                                      ///< Difference between expected & actual nbr data bytes.
  uint8_t status;                                             ///< Indicates the success or failure of the cmd.
} sli_usbd_msc_csw_t;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct sli_usbd_msc_ctrl sli_usbd_msc_ctrl_t;

/********************************************************************************************************
 *                                               MSC STATES
 *******************************************************************************************************/

typedef enum sli_usbd_msc_state {
  SLI_USBD_MSC_STATE_NONE = 0,
  SLI_USBD_MSC_STATE_INIT,
  SLI_USBD_MSC_STATE_CONFIG
} sli_usbd_msc_state_t;

typedef enum sli_usbd_msc_comm_state {
  SLI_USBD_MSC_COMM_STATE_NONE = 0,
  SLI_USBD_MSC_COMM_STATE_CBW,
  SLI_USBD_MSC_COMM_STATE_DATA,
  SLI_USBD_MSC_COMM_STATE_CSW,
  SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL,
  SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL,
  SLI_USBD_MSC_COMM_STATE_RESET_RECOVERY,
  SLI_USBD_MSC_COMM_STATE_BULK_IN_STALL,
  SLI_USBD_MSC_COMM_STATE_BULK_OUT_STALL
} sli_usbd_msc_comm_state_t;

/********************************************************************************************************
 *                                       MSC EP REQUIREMENTS DATA TYPE
 *******************************************************************************************************/

/// MSC Communication Information
typedef struct sli_usbd_msc_comm {
  sli_usbd_msc_ctrl_t       *ctrl_ptr;                              ///< Ptr to ctrl information.
  uint8_t                   data_Bulk_in_ep_addr;
  uint8_t                   data_Bulk_out_ep_addr;
  sli_usbd_msc_comm_state_t next_comm_state;                        ///< Next comm state of the MSC device.
  sli_usbd_msc_cbw_t        cbw;                                    ///< Cmd blk wrapper (cbw).
  sli_usbd_msc_csw_t        csw;                                    ///< Cmd status word (csw).
  bool                      stall;                                  ///< Used to stall the endpoints.
  uint32_t                  bytes_to_xfer;                          ///< Current bytes to xfer during data xfer stage.
  void                      *wr_buffer_ptr;                         ///< Ptr to the buf used to wr to device.
  uint32_t                  wr_buffer_length;                       ///< buf len used to wr to device.
} sli_usbd_msc_comm_t;

/// MSC Control Information
struct sli_usbd_msc_ctrl {
  uint32_t       cbw_buffer[SLI_USBD_MSC_CBW_BUFFER_SIZE];          ///< Buf to rx Cmd Blk  Wrapper.
  uint32_t       csw_buffer[SLI_USBD_MSC_CSW_BUFFER_SIZE];          ///< Buf to send Cmd status Wrapper.
  uint32_t       data_buffer[SLI_USBD_MSC_DATA_BUFFER_SIZE];        ///< Buf to handle data stage.
  uint8_t        ctrl_status_buffer;                                ///< Buf used for ctrl status xfers.
  uint8_t        class_nbr;                                         ///< MSC class instance nbr.
  uint8_t        subclass_code;                                     ///< MSC subclass code.
  uint8_t        subclass_protocol;                                 ///< MSC subclass protocol.
  sl_usbd_msc_subclass_driver_t *subclass_driver;                   ///< MSC subclass driver functions.
  void                          *subclass_arg;                      ///< MSC subclass argument.
  sli_usbd_msc_state_t state;                                       ///< MSC dev state.
  uint8_t        max_lun;                                           ///< Max logical unit number (lun).
  void           *lun_data_ptr_table[SL_USBD_MSC_LUN_QUANTITY];     ///< Array of struct pointing to lun's
  sli_usbd_msc_comm_t  *comm_ptr;                                   ///< MSC comm info ptr.
};

/// MSC Class Root Struct
typedef struct sli_usbd_msc {
  sli_usbd_msc_ctrl_t ctrl_table[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY];    ///< MSC instance array.
  uint8_t             next_ctrl_nbr;
  sli_usbd_msc_comm_t comm_table[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY * SL_USBD_MSC_CONFIGURATION_QUANTITY];    ///< MSC comm array.
  uint16_t            next_comm_nbr;
  uint8_t             class_instance_qty;                                 ///< Quantity of class instance.
  uint8_t             lun_qty;                                            ///< Quantity of logical unit per class instance.
} sli_usbd_msc_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern sli_usbd_msc_t *usbd_msc_ptr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           sli_usbd_msc_os_init()
 *
 * @brief    Initialize MSC OS interface.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_init(void);

/****************************************************************************************************//**
 *                                           sli_usbd_msc_os_create_task()
 *
 * @brief    Create task for MSC class instance.
 *
 * @param    class_nbr  Class instance number.
 *
 * @param    msc_task_stack_size  The USB stack creates a task to handle the events of each MSC instance.
 *                                This parameter specifies the stack size to be used for the newly created
 *                                task for this instance.
 *
 * @param    msc_task_priority    The USB stack creates a task to handle the events of each MSC instance.
 *                                This parameter specifies the CMSIS priority of that task.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_create_task(uint8_t       class_nbr,
                                        uint32_t      msc_task_stack_size,
                                        osPriority_t  msc_task_priority);

/****************************************************************************************************//**
 *                                       sli_usbd_msc_os_post_comm_signal()
 *
 * @brief    Post a semaphore used for MSC communication.
 *
 * @param    class_nbr   MSC instance class number
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_post_comm_signal(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                       sli_usbd_msc_os_pend_comm_signal()
 *
 * @brief    Wait on a semaphore to become available for MSC communication.
 *
 * @param    class_nbr   MSC instance class number
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_pend_comm_signal(uint8_t  class_nbr,
                                             uint32_t timeout);

/****************************************************************************************************//**
 *                                       sli_usbd_msc_os_post_connect_signal()
 *
 * @brief    Post a semaphore used for MSC enumeration.
 *
 * @param    class_nbr   MSC instance class number
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_post_connect_signal(uint8_t class_nbr);

/****************************************************************************************************//**
 *                                       sli_usbd_msc_os_pend_connect_signal()
 *
 * @brief    Wait on a semaphore to become available for MSC enumeration.
 *
 * @param    class_nbr   MSC instance class number
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_pend_connect_signal(uint8_t  class_nbr,
                                                uint32_t timeout);

/****************************************************************************************************//**
 *                                     sli_usbd_msc_os_acquire_lun_lock()
 *
 * @brief    Acquire logical unit's state lock.
 *
 * @param    lu_ix  Index of the logical unit inside the lun table.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_acquire_lun_lock(uint8_t lu_ix);

/****************************************************************************************************//**
 *                                     sli_usbd_msc_os_release_lun_lock()
 *
 * @brief    Release logical unit's state lock.
 *
 * @param    lu_ix  Index of the logical unit inside the lun table.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_release_lun_lock(uint8_t lu_ix);

/****************************************************************************************************//***
 *                                     sli_usbd_msc_task_handler()
 *
 * @brief    This function is used to handle MSC transfers.
 *
 * @param    class_nbr   MSC instance number.
 *******************************************************************************************************/
void sli_usbd_msc_task_handler(uint8_t class_nbr);

/****************************************************************************************************//**
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
