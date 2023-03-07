/***************************************************************************//**
 * @file
 * @brief USB Device Vendor Class
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

/****************************************************************************************************//**
 * @defgroup USBD_VENDOR USB Device Vendor API
 * @ingroup USBD
 * @brief   USB Device Vendor API
 *
 * @addtogroup USBD_VENDOR
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SL_USBD_CLASS_VENDOR_H
#define  SL_USBD_CLASS_VENDOR_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "sl_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/
/// Vendor callbacks
typedef const struct {
  void (*enable)(uint8_t class_nbr);                            ///< Callback for enable event

  void (*disable)(uint8_t class_nbr);                           ///< Callback for disable event

  void (*setup_req)(uint8_t                   class_nbr,
                    const sl_usbd_setup_req_t *p_setup_req);    ///< Callback for setup req event
} sl_usbd_vendor_callbacks_t;

/// App callback used for async comm.
typedef void (*sl_usbd_vendor_async_function_t)(uint8_t      class_nbr,
                                                void         *p_buf,
                                                uint32_t     buf_len,
                                                uint32_t     xfer_len,
                                                void         *p_callback_arg,
                                                sl_status_t  status);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************
 *                                           VENDOR FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 * @brief    Initialize the internal structures and variables used by the Vendor class.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_init(void);

/****************************************************************************************************//**
 * @brief    Add a new instance of the Vendor class.
 *
 * @param    intr_en            Interrupt endpoints IN and OUT flag:
 *                               - true    Pair of interrupt endpoints added to interface.
 *                               - false   Pair of interrupt endpoints not added to interface.
 *
 * @param    interval           Endpoint interval in milliseconds (must be a power of 2).
 *
 * @param   p_vendor_callbacks  Pointer to vendor callback structure.
 *                              [Content MUST be persistent]
 *
 * @param    p_class_nbr        Pointer to a variable that will receive class instance number, if no
 *                              errors are returned, or SL_USBD_CLASS_NBR_NONE otherwise.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_create_instance(bool                        intr_en,
                                           uint16_t                    interval,
                                           sl_usbd_vendor_callbacks_t  *p_vendor_callbacks,
                                           uint8_t                     *p_class_nbr);

/****************************************************************************************************//**
 * @brief    Add the Vendor class instance into the specified configuration (see Note #1).
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    config_nbr  Configuration index to which to add the Vendor class instance.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Called several times, it creates multiple instances and configurations.
 *               For instance, the following architecture could be created :
 *               @verbatim
 *               FS
 *               |-- Configuration 0
 *                   |-- Interface 0 (Vendor 0)
 *               |-- Configuration 1
 *                   |-- Interface 0 (Vendor 0)
 *                   |-- Interface 1 (Vendor 1)
 *               @endverbatim
 *               In that example, there are two instances of Vendor class: 'Vendor 0' and '1', and two
 *               possible configurations: 'Configuration 0' and '1'. 'Configuration 1' is composed
 *               of two interfaces. Each class instance has an association with one of the interfaces.
 *               If 'Configuration 1' is activated by the host, it allows the host to access two
 *               different functionalities offered by the device.
 *
 * @note     (2) Configuration Descriptor corresponding to a Vendor-specific device has the following
 *               format :
 *               @verbatim
 *               Configuration Descriptor
 *               |-- Interface Descriptor (Vendor class)
 *                   |-- Endpoint Descriptor (Bulk OUT)
 *                   |-- Endpoint Descriptor (Bulk IN)
 *                   |-- Endpoint Descriptor (Interrupt OUT) - optional
 *                   |-- Endpoint Descriptor (Interrupt IN)  - optional
 *               @endverbatim
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_add_to_configuration(uint8_t class_nbr,
                                                uint8_t config_nbr);

/****************************************************************************************************//**
 * @brief    Get the vendor class enable state.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_enabled   Pointer to a variable that will receive the enable state.
 *                       The variable is set to true, if the Vendor class is enabled,
 *                       and is set to false if the Vendor class is NOT enabled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_is_enabled(uint8_t  class_nbr,
                                      bool     *p_enabled);

/****************************************************************************************************//**
 * @brief    Add a Microsoft OS extended property to this vendor class instance.
 *
 * @param    class_nbr           Class instance number.
 *
 * @param    property_type       Property type (see Note #2).
 *                                   - OS_PROPERTY_TYPE_REG_SZ
 *                                   - OS_PROPERTY_TYPE_REG_EXPAND_SZ
 *                                   - OS_PROPERTY_TYPE_REG_BINARY
 *                                   - OS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN
 *                                   - OS_PROPERTY_TYPE_REG_DWORD_BIG_ENDIAN
 *                                   - OS_PROPERTY_TYPE_REG_LINK
 *                                   - OS_PROPERTY_TYPE_REG_MULTI_SZ
 *
 * @param    p_property_name     Pointer to the buffer that contains the property name.
 *                               ---- Buffer assumed to be persistent ----
 *
 * @param    property_name_len   Length of the property name in octets.
 *
 * @param    p_property          Pointer to the buffer that contains the property name.
 *                               ---- Buffer assumed to be persistent ----
 *
 * @param    property_len        Length of the property in octets.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) For more information on Microsoft OS descriptors, see
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *
 * @note     (2) For more information on property types, refer to "Table 3. Property Data Types" of
 *               "Extended Properties OS Feature Descriptor Specification" document provided by
 *               Microsoft available at
 *               'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
sl_status_t sl_usbd_vendor_add_microsoft_ext_property(uint8_t       class_nbr,
                                                      uint8_t       property_type,
                                                      const uint8_t *p_property_name,
                                                      uint16_t      property_name_len,
                                                      const uint8_t *p_property,
                                                      uint32_t      property_len);
#endif

/****************************************************************************************************//**
 * @brief    Receive the data from the host through the Bulk OUT endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive the buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_xfer_len  Pointer to a variable that will receive transfer length.
 *                       The variable is set to number of octets received, if no errors are returned,
 *                       or is set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_bulk_sync(uint8_t  class_nbr,
                                          void     *p_buf,
                                          uint32_t buf_len,
                                          uint16_t timeout,
                                          uint32_t *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Send data to the host through Bulk IN endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit the buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #1).
 *
 * @param    p_xfer_len  Pointer to a variable that will receive transfer length.
 *                       The variable is set to number of octets received, if no errors are returned,
 *                       or is set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_bulk_sync(uint8_t   class_nbr,
                                           void      *p_buf,
                                           uint32_t  buf_len,
                                           uint16_t  timeout,
                                           bool      end,
                                           uint32_t  *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Receive data from the host through the Bulk OUT endpoint. This function is non-blocking
 *           are returns immediately after transfer preparation. Upon transfer completion, a callback
 *           provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the receive buffer.
 *
 * @param    buf_len         Receive buffer length in octets.
 *
 * @param    async_fnct      Receive the the callback.
 *
 * @param    p_async_arg     Additional argument provided by the application for the receive callback.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_bulk_async(uint8_t                         class_nbr,
                                           void                            *p_buf,
                                           uint32_t                        buf_len,
                                           sl_usbd_vendor_async_function_t async_fnct,
                                           void                            *p_async_arg);

/****************************************************************************************************//**
 * @brief    Send data to the host through the Bulk IN endpoint. This function is non-blocking
 *           and returns immediately after transfer preparation. Upon transfer completion, a
 *           callback provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the transmit buffer.
 *
 * @param    buf_len         Transmit buffer length in octets.
 *
 * @param    async_fnct      Transmit the callback.
 *
 * @param    p_async_arg     Additional argument provided by the application for the transmit callback.
 *
 * @param    end             End-of-transfer flag (see Note #1).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_bulk_async(uint8_t                         class_nbr,
                                            void                            *p_buf,
                                            uint32_t                        buf_len,
                                            sl_usbd_vendor_async_function_t async_fnct,
                                            void                            *p_async_arg,
                                            bool                            end);

/****************************************************************************************************//**
 * @brief    Receive data from the the host through the Interrupt OUT endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_xfer_len  Pointer to a variable that will receive transfer length.
 *                       The variable is set to number of octets received, if no errors are returned,
 *                       or is set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_interrupt_sync(uint8_t  class_nbr,
                                               void     *p_buf,
                                               uint32_t buf_len,
                                               uint16_t timeout,
                                               uint32_t *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Send data to the host through the Interrupt IN endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Transmit buffer length in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #1).
 *
 * @param    p_xfer_len  Pointer to a variable that will receive transfer length.
 *                       The variable is set to number of octets received, if no errors are returned,
 *                       or is set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_interrupt_sync(uint8_t   class_nbr,
                                                void      *p_buf,
                                                uint32_t  buf_len,
                                                uint16_t  timeout,
                                                bool      end,
                                                uint32_t  *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Receive data from the host through Interrupt OUT endpoint. This function is non-blocking
 *           and returns immediately after transfer preparation. Upon transfer completion, a callback
 *           provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the receive buffer.
 *
 * @param    buf_len         Receive buffer length in octets.
 *
 * @param    async_fnct      Receive callback.
 *
 * @param    p_async_arg     Additional argument provided by application for the receive callback.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_read_interrupt_async(uint8_t                         class_nbr,
                                                void                            *p_buf,
                                                uint32_t                        buf_len,
                                                sl_usbd_vendor_async_function_t async_fnct,
                                                void                            *p_async_arg);

/****************************************************************************************************//**
 * @brief    Send data to the host through the Interrupt IN endpoint. This function is non-blocking
 *           and returns immediately after transfer preparation. Upon transfer completion, a callback
 *           provided by the application will be called to finalize the transfer.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to the transmit buffer.
 *
 * @param    buf_len         Transmit buffer length in octets.
 *
 * @param    async_fnct      Transmit callback.
 *
 * @param    p_async_arg     Additional argument provided by the application for the transmit callback.
 *
 * @param    end             End-of-transfer flag (see Note #1).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the end-of-transfer is set and the transfer length is a multiple of the maximum
 *               packet size, a zero-length packet is transferred to signal the end of transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_vendor_write_interrupt_async(uint8_t                         class_nbr,
                                                 void                            *p_buf,
                                                 uint32_t                        buf_len,
                                                 sl_usbd_vendor_async_function_t async_fnct,
                                                 void                            *p_async_arg,
                                                 bool                            end);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
