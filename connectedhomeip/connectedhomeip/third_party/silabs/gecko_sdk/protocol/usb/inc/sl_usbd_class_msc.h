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
 * @defgroup USBD_MSC USB Device MSC API
 * @ingroup USBD
 * @brief   USB Device MSC API
 *
 * @addtogroup USBD_MSC
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SL_USBD_CLASS_MSC_H
#define  SL_USBD_CLASS_MSC_H

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

#include "sli_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SL_USBD_MSC_LU_NBR_INVALID                        255u

/********************************************************************************************************
 *                                           SUBCLASS CODES
 *
 * Note(s) : (1) See 'USB Mass Storage Class Specification Overview', Revision 1.2, Section 2
 *******************************************************************************************************/

#define  SL_USBD_MSC_SUBCLASS_CODE_RBC                      0x01
#define  SL_USBD_MSC_SUBCLASS_CODE_SFF_8020i                0x02
#define  SL_USBD_MSC_SUBCLASS_CODE_MMC_2                    0x02
#define  SL_USBD_MSC_SUBCLASS_CODE_QIC_157                  0x03
#define  SL_USBD_MSC_SUBCLASS_CODE_UFI                      0x04
#define  SL_USBD_MSC_SUBCLASS_CODE_SFF_8070i                0x05
#define  SL_USBD_MSC_SUBCLASS_CODE_SCSI                     0x06

/********************************************************************************************************
 *                                           PROTOCOL CODES
 *
 * Note(s) : (1) See 'USB Mass Storage Class Specification Overview', Revision 1.2, Section 3
 *******************************************************************************************************/

#define  SL_USBD_MSC_PROTOCOL_CODE_CTRL_BULK_INTR_CMD_INTR  0x00
#define  SL_USBD_MSC_PROTOCOL_CODE_CTRL_BULK_INTR           0x01
#define  SL_USBD_MSC_PROTOCOL_CODE_BULK_ONLY                0x50

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/// MSC subclass driver
typedef const struct {
  sl_status_t (*enable)(uint8_t   class_nbr,
                        void      *p_subclass_arg);

  sl_status_t (*disable)(uint8_t   class_nbr,
                         void      *p_subclass_arg);

  sl_status_t (*process_command)(uint8_t        class_nbr,
                                 void           *p_lun_data,
                                 const uint8_t  *p_cbwcb,
                                 uint8_t        *p_data_buf,
                                 uint32_t       *p_resp_len,
                                 uint8_t        *p_data_dir,
                                 void           *p_subclass_arg);

  sl_status_t (*read_data)(uint8_t   class_nbr,
                           void      *p_lun_data,
                           uint8_t   command,
                           uint8_t   *p_data_buf,
                           uint32_t  data_len,
                           void      *p_subclass_arg);

  sl_status_t (*write_data)(uint8_t   class_nbr,
                            void      *p_lun_data,
                            uint8_t   command,
                            void      *p_data_buf,
                            uint32_t  data_len,
                            void      *p_subclass_arg);
} sl_usbd_msc_subclass_driver_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************
 *                                               MSC FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 * @brief    Initialize internal structures and variables used by the Mass Storage Class
 *           Bulk Only Transport.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_init(void);

/****************************************************************************************************//**
 * @brief    Add a new instance of the Mass Storage Class.
 *
 * @param    subclass             MSC subclass code.
 *
 * @param    p_subclass_drv       Pointer to data structure of functions that implement the subclass driver.
 *
 * @param    msc_task_stack_size  The USB stack creates a task to handle the events of each MSC instance.
 *                                This parameter specifies the stack size to be used for the newly created
 *                                task for this instance.
 *
 * @param    msc_task_priority    The USB stack creates a task to handle the events of each MSC instance.
 *                                This parameter specifies the CMSIS priority of that task.
 *
 * @param    p_subclass_arg       An argument that will be passed to the driver when driver functions are called.
 *
 * @param    p_class_nbr          Pointer to a variable that will receive the instance number for the created
 *                                class instance, if no errors are returned. Otherwise, the variable is set to
 *                                SL_USBD_CLASS_NBR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_create_instance(uint8_t                       subclass,
                                        uint32_t                      msc_task_stack_size,
                                        osPriority_t                  msc_task_priority,
                                        sl_usbd_msc_subclass_driver_t *p_subclass_drv,
                                        void                          *p_subclass_arg,
                                        uint8_t                       *p_class_nbr);

/****************************************************************************************************//**
 * @brief    Add an existing MSC instance to the specified configuration.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    config_nbr  Configuration index to which to add the existing MSC interface.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) sl_usbd_msc_add_to_configuration() adds an Interface descriptor and its associated Endpoint
 *               descriptor(s) to the Configuration descriptor. One call to sl_usbd_msc_add_to_configuration() builds
 *               the Configuration descriptor corresponding to an MSC device with the following format:
 *               @verbatim
 *               Configuration Descriptor
 *               |-- Interface Descriptor (MSC)
 *               |-- Endpoint Descriptor (Bulk OUT)
 *               |-- Endpoint Descriptor (Bulk IN)
 *               @endverbatim
 *               If sl_usbd_msc_add_to_configuration() is called several times from the application, it creates multiple
 *               instances and configurations. For example, the following architecture could be created
 *               for a high-speed device:
 *               @verbatim
 *               High-speed
 *               |-- Configuration 0
 *               |-- Interface 0 (MSC 0)
 *               |-- Configuration 1
 *               |-- Interface 0 (MSC 0)
 *               |-- Interface 1 (MSC 1)
 *               @endverbatim
 *               In this example, there are two instances of MSC: 'MSC 0' and 'MSC 1', and two possible
 *               configurations for the device: 'Configuration 0' and 'Configuration 1'. 'Configuration 1'
 *               is composed of two interfaces.
 *               @n
 *               Each class instance has an association with one of the interfaces. If 'Configuration 1'
 *               is activated by the host, it allows the host to access two different functionalities
 *               offered by the device.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_add_to_configuration(uint8_t  class_nbr,
                                             uint8_t  config_nbr);

/****************************************************************************************************//**
 * @brief    Add an MSC logical unit to the MSC interface.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    p_lu_data   Pointer to the logical unit data to store in LU data structure.
 *
 * @param    p_lu_nbr    Pointer to a variable that will receive Logical Unit Number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_lun_add(uint8_t         class_nbr,
                                void            *p_lu_data,
                                uint8_t         *p_lu_nbr);

/****************************************************************************************************//**
 * @brief    Get the MSC enable state of the device.
 *
 * @param    class_nbr    MSC instance number.
 *
 * @param    p_enabled  Pointer to a boolean variable that will receive enable state.
 *                        The variable is set to true if MSC class is enabled.
 *                        The variable is set to false if the MSC class is not enabled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_msc_is_enabled(uint8_t  class_nbr,
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
