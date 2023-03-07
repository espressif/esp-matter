/***************************************************************************//**
 * @file
 * @brief   Header file for the USBXpress firmware library.  Includes function
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

/**************************************************************************//**
 * @addtogroup usbxpress USBXpress
 * @{
 *
 * @brief
 *   USBXpress interface library
 *
 * ## Introduction
 *
 *   This module provides a firmware interface which communicates with the
 *   Silicon Labs USBXpress driver and allows firmware to interact with host
 *   programs via the USBXpress DLL.
 *
 * ## Theory of operation:
 *
 *   The USBxpress library interfaces with the Silicon Labs USBXpress drivers.
 *   The interface consists of two data transmission pipes (RX and TX) which
 *   are used to send and receive data.
 *
 *  ### Data
 *
 *   Data reception and transmission is handled with USBX_blockRead() and
 *   USBX_blockWrite(). Upon completion of a sent or received transfer the user
 *   is called back with USBX_RX_COMPLETE or USBX_TX_COMPLETE. If the user
 *   calls USBX_blockRead() with a numBytes value smaller than the number of
 *   bytes received in the transfer, the user is called back with both
 *   USBX_RX_COMPLETE and USBX_RX_OVERRUN set.
 *
 *  # Additional Resources
 *
 *   * [USBXpress AppNote - AN169](http://www.silabs.com/Support%20Documents/TechnicalDocs/AN169.pdf)
 *   * [Driver Customization - AN220](http://www.silabs.com/Support%20Documents/TechnicalDocs/AN200.pdf)
 *
 *   This driver sits on top of the EFM32 USB Library.
 *
 * ## Interrupts
 *
 *   This library handles USB interrupts and will enable USB interrupts at
 *   its  discretion. It will NOT enable global interrupts and the user is
 *   responsible for enabling global interrupts.
 *****************************************************************************/

#ifndef  EM_USBXPRESS_H
#define  EM_USBXPRESS_H

#include <stdint.h>
#include <stdbool.h>

#include "em_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * Define user API call-back function with:
 *
 * void my_callback(void) {}
 *
 * then pass it to USBX_apiCallbackEnable() with:
 *
 * USBX_apiCallbackEnable(my_callback);
 *
 * @warning
 *   The call-back routine is called by the USBXpress library from
 *   within the USB interrupt. It should run as quickly as possible.
 *
 * If timing becomes an issue, consider using the call-back routine to set
 * user-defined flags that may be serviced in the project's main loop.
 * Additionally note that the call-back routine will be called for every
 * call-back source, whether or not the user checks for that call-back source.
 * All code except variable declarations and the USBX_getCallbackSource() call
 * should check for a pertinent call-back source (see the @ref usbx_status
 * section for all call-back sources).
 *
 *****************************************************************************/
typedef void (*USBX_apiCallback_t)(void);

/***************************************************************************//**
 *  @addtogroup usbx_callback_status Callback Status Flags
 *  @brief Callback Status Flags
 *  @{
 *
 *  These constant values are returned by USBX_getCallbackSource(). The return
 *  value is a mask and may contain the logical OR of any combination of status
 *  flags. Each flag maps to a call-back event.
 *
 ******************************************************************************/
// Basic USBX_getCallbackSource() return value bit masks (32 bits)
// Note: More than one bit can be set at the same time.
#define USBX_RESET               0x00000001     //!< USB Reset Interrupt has occurred
#define USBX_TX_COMPLETE         0x00000002     //!< Transmit Complete Interrupt has occurred
#define USBX_RX_COMPLETE         0x00000004     //!< Receive Complete Interrupt has occurred
#define USBX_FIFO_PURGE          0x00000008     //!< Receive and Transmit FIFO's were purged
#define USBX_DEV_OPEN            0x00000010     //!< Device Instance Opened on host side
#define USBX_DEV_CLOSE           0x00000020     //!< Device Instance Closed on host side
#define USBX_DEV_CONFIGURED      0x00000040     //!< Device has entered configured state
#define USBX_DEV_SUSPEND         0x00000080     //!< USB suspend signaling present on bus
#define USBX_RX_OVERRUN          0x00000100     //!< Data received with no place to put it
/**  @} (end usbx_callback_status) */

/***************************************************************************//**
 *  @addtogroup usbx_status Status Flags
 *  *brief Status Flags
 *  @{
 *
 *  These constant values are returned by USBX_blockRead() and
 *  USBX_blockWrite().
 *
 ******************************************************************************/
#define USBX_STATUS_OK                  USB_STATUS_OK                   //!< Success
#define USBX_STATUS_EP_BUSY             USB_STATUS_EP_BUSY              //!< Failed because the endpoint is busy
#define USBX_STATUS_ILLEGAL             USB_STATUS_ILLEGAL              //!< Failed due to an illegal parameter
#define USBX_STATUS_EP_STALLED          USB_STATUS_EP_STALLED           //!< Failed because the endpoint is stalled
#define USBX_STATUS_DEVICE_UNCONFIGURED USB_STATUS_DEVICE_UNCONFIGURED  //!< Failed because the device is not configured
/**  @} (end usbx_status) */

//! @cond DOXYGEN_SKIP
#ifdef char16_t
#undef char16_t
#endif
#if defined(__GNUC__)                     /* GCC compilers */
#if defined(__CHAR16_TYPE__)
typedef __CHAR16_TYPE__ char16_t;
#else
typedef unsigned short char16_t;          /**< Data type used for UTF-16LE formatted USB string descriptors. */
#endif

#elif defined(__ICCARM__)                 /* IAR compiler */
#include <uchar.h>

#elif defined(__CC_ARM)                   /* MDK-ARM compiler */
typedef unsigned short char16_t;
#endif
//! @endcond

/***************************************************************************//**
 *  @addtogroup usbx_macros Macros
 *  @brief Macros
 *  @{
 ******************************************************************************/

/***************************************************************************//**
 *  @brief Macro for creating USB-Compliant UTF-16LE UNICODE string descriptor.
 *
 *  @n Example:
 *     USBX_STRING_DESC(iManufacturer, 'S','i','l','i','c','o','n',' ' \
 *                                             'L','a','b','s');
 *  @note The size of the resulting struct will be two bytes larger than a USB
 *        string descriptor. This is to accommodate a terminating null char for
 *        the string. The value assigned to the 'len' member does not take this
 *        into account and is therefore correct USB-wise.
 ******************************************************************************/
#define USBX_STRING_DESC(_name, ...)                            \
  SL_PACK_START(1)                                              \
  typedef struct                                                \
  {                                                             \
    uint8_t  len;                                               \
    uint8_t  type;                                              \
    char16_t name[1 + sizeof((char16_t[]){ __VA_ARGS__ }) / 2]; \
  } SL_ATTRIBUTE_PACKED _##_name;                               \
  SL_PACK_END()                                                 \
  SL_ALIGN(4)                                                   \
  SL_PACK_START(1)                                              \
  static const _##_name _name SL_ATTRIBUTE_ALIGN(4) =           \
  {                                                             \
    .len  = sizeof(_##_name) - 2,                               \
    .type = 3,                                                  \
    .name = { __VA_ARGS__ },                                    \
    .name[((sizeof(_##_name) - 2) / 2) - 1] = '\0'              \
  }                                                             \
  SL_PACK_END()

/***************************************************************************//**
 *  @brief Macro for creating WORD (4 byte) aligned uint8_t array with size
 *         which is a multiple of WORD size.
 *
 *  @n Example: @n USBX_BUF(rxBuffer, 37);  =>  uint8_t rxBuffer[40];
 *
 *  @note Any data buffer which is written to or read by the USB hardware must
 *        be aligned on a WORD (4 byte) boundary. This macro provides an easy
 *        way to create USB data buffers which are guaranteed to be correctly
 *        aligned.
 ******************************************************************************/
#define USBX_BUF(x, y) \
  SL_ALIGN(4) uint8_t x[((y) + 3) & ~3] SL_ATTRIBUTE_ALIGN(4)

/***************************************************************************//**
 *  @brief Macro for creating WORD (4 byte) aligned static uint8_t array with
 *         size which is a multiple of WORD size.
 *
 *  @n Example: @n STATIC_USBX_BUF(rxBuffer, 37);  =>  uint8_t rxBuffer[40];
 *
 *  @note Any data buffer which is written to or read by the USB hardware must
 *        be aligned on a WORD (4 byte) boundary. This macro provides an easy
 *        way to create static USB data buffers which are guaranteed to be
 *        correctly aligned.
 ******************************************************************************/
#define STATIC_USBX_BUF(x, y) SL_ALIGN(4) \
  static uint8_t x[((y) + 3) & ~3] SL_ATTRIBUTE_ALIGN(4)
/**  @} (end usbx_macros) */

/***************************************************************************//**
 *  @brief
 *    USBXpress initialization function parameter typedef
 *
 *  User should instantiate and pass by reference to USBX_init().
 *
 *****************************************************************************/
SL_PACK_START(1)
typedef struct {
  uint16_t vendorId;             /**< 16-bit Vendor ID to be returned to the
                                  *   host's Operating System during USB
                                  *   enumeration. Set to 0x10C4 to use the
                                  *   default Silicon Laboratories Vendor ID.
                                  */
  uint16_t productId;            /**< 16-bit Product ID to be returned to the
                                  *   host's Operating System during USB
                                  *   enumeration. Set to 0xEA61 to associate
                                  *   with the default VCP driver.
                                  */
  void const* manufacturerString;/**< Pointer to a character string. See AN571
                                  *    Appendix B for formatting. NULL pointer
                                  *    will be treated as a valid address.
                                  */
  void const* productString;     /**< Pointer to a character string. See AN571
                                  *   Appendix B for formatting. NULL pointer
                                  *   will be treated as a valid address.
                                  */
  void const* serialString;      /**< Pointer to a character string. See
                                  *   Appendix B for formatting. NULL pointer
                                  *   will be treated as a valid address.
                                  */
  uint8_t maxPower;              /**< Specifies how much bus current a device
                                  *   requires.  Set to one half the number of
                                  *   milliamperes required. The maximum
                                  *   allowed current is 500 milliamperes, and
                                  *   hence any value above 0xFA will be
                                  *   automatically set to 0xFA. Example: Set
                                  *   to 0x32 to request 100 mA.
                                  */
  uint8_t powerAttribute;        /**< Set bit 6 to 1 if the device is self-
                                  *   powered and to 0 if it is bus-powered.
                                  *   Set bit 5 to 1 if the device supports the
                                  *   remote wakeup feature. Bits 0 through 4
                                  *   must be 0 and bit 7 must be 1. Example:
                                  *   Set to 0x80 to specify a bus-powered
                                  *   device that does not support remote
                                  *   wakeup.
                                  */
  uint16_t releaseBcd;           /**< The device's release number in BCD
                                  *   (binary-coded decimal) format. In BCD,
                                  *   the upper byte represents the integer,
                                  *   the next four bits are tenths, and the
                                  *   final four bits are hundredths.  Example:
                                  *   2.13 is denoted by 0x0213.
                                  */
  uint8_t useFifo;               /**< Whether to use the USB FIFO space to
                                  *   store VCP variables or else store them in
                                  *   user XRAM.  The addresses of the
                                  *   variables are the same either way.  If
                                  *   the current device does not support the
                                  *   use of the USB FIFO space form variable
                                  *   storage, this option does nothing.
                                  */
} SL_ATTRIBUTE_PACKED USBX_Init_t;
SL_PACK_END()

/***************************************************************************//**
 *  @addtogroup usbx_func Functions
 *  @brief Functions
 *  @{
 ******************************************************************************/

/**
 * @brief
 *   User API USB initialization function.
 *
 * @param p:
 *   Pointer to USBX_Init_t structure.  Not modified by this routine.
 *
 * To minimize user XRAM use, the user should pass initialization parameters
 * from code memory to the USBX_init() function.
 *
 *****************************************************************************/
void USBX_init(USBX_Init_t* p);

/**************************************************************************//**
 * @brief
 *   User API function to send data to host.
 *
 * @param   block:
 *   Pointer to user's array where data to be transmitted is stored.
 * @param   numBytes:
 *   Number of bytes to send to host
 * @param[out]   countPtr:
 *   Pointer to user's storage for number of bytes actually transmitted to the
 *   host.  This will be valid upon the subsequent USBX_TX_COMPLETE call-back.
 *
 * @return
 *   This function returns a status word. 0 indicates that the write was
 *   successfully started. Any other value indicates that the request was
 *   ignored. The most common reason is USBX_STATUS_EP_BUSY meaning that
 *   a write is already in progress.
 *
 *   Valid return codes are fond in @ref usbx_status
 *
 * A USBX_TX_COMPLETE call-back will occur when the entire transfer has
 * completed. For example if the user writes a block of 100 bytes, two packets
 * will be sent (one 64 byte packet and one 36 byte packet), but the
 * USBX_TX_COMPLETE call-back will only occur after the final packet has
 * been sent.
 *
 * This function utilizes the EFM32 USB Library's USBD_Write() function, which
 * only prepares for a write.  The actual write will occur over time as the
 * host transmits data.
 *
 *****************************************************************************/
int USBX_blockWrite(uint8_t* block,
                    uint32_t numBytes,
                    uint32_t* countPtr);

/**************************************************************************//**
 * @brief
 *   User API function to get data from host.
 *
 * @param   block:
 *   Pointer to user's array where received data will be stored.
 * @param   numBytes:
 *   Number of bytes to receive from host
 * @param[out]   countPtr:
 *   Pointer to user's storage for number of bytes actually received from the
 *   host.  This will be valid upon the subsequent USBX_RX_COMPLETE call-back.
 *
 * @return
 *   This function returns a status word. 0 indicates that the read was
 *   successfully started. Any other value indicates that the request was
 *   ignored. The most common reason is USBX_STATUS_EP_BUSY meaning that
 *   a read is already in progress.
 *
 *   Valid return codes are fond in @ref usbx_status
 *
 * A USBX_RX_COMPLETE call-back will occur when the read transfer has completed.
 * For example if the user reads a block of 100 bytes, two packets will be
 * received (one 64 byte packet and one 36 byte packet), but the
 * USBX_RX_COMPLETE call-back will only occur after the final packet has been
 * received.
 *
 * If the device receives more data than the value specified by numBytes, the
 * USBX_RX_COMPLETE call-back will occur when numBytes number of bytes has been
 * received. At this point, the USBX_RX_OVERRUN status flag will also be set,
 * indicating that more data was received than is available to write to the
 * buffer at block. At this point, the application may call USBX_blockRead()
 * again to receive the remaining data from the transfer.
 *
 * This function utilizes the EFM32 USB Library's USBD_Read() function,
 * which only prepares for a read.  The actual read will occur over time as the
 * host transmits data.
 *
 *****************************************************************************/
int USBX_blockRead(uint8_t* block,
                   uint32_t numBytes,
                   uint32_t* countPtr);

/**************************************************************************//**
 * @brief
 *   User API function to get the call-back source.
 *
 * Returns an 32-bit value indicating the reason(s) for the API call-back, and
 * clears the USBXpress API call-back pending flag(s). This function should be
 * called at the beginning of the user's call-back service routine to determine
 * which event(s) has/have occurred.
 *
 * @return
 *   An unsigned 32-bit code indicating the reason(s) for the API
 *   interrupt. The code can indicate more than one type of interrupt at
 *   the same time. Valid flags are found in @ref usbx_status
 *
 *****************************************************************************/
uint32_t USBX_getCallbackSource(void);

/**************************************************************************//**
 * @brief
 *   Enables user API call-backs
 *
 * @param   f:
 *   Pointer to user-defined call-back function.
 *
 * Enables USB interrupts and sets the API call-back function pointer to the
 * passed function.
 *
 *****************************************************************************/
void USBX_apiCallbackEnable(USBX_apiCallback_t f);

/**************************************************************************//**
 * @brief
 *   Inhibits user API call-backs and does NOT disable the USB interrupt.
 *
 *****************************************************************************/
void USBX_apiCallbackDisable(void);

/**************************************************************************//**
 * @brief
 *   Disables the USB interface.
 *
 *****************************************************************************/
void USBX_disable(void);

/**************************************************************************//**
 * @brief
 *   Returns the USBXpress library version.
 *
 * @return Library version
 *
 * Library version is returned in binary-coded decimal format.
 *
 *****************************************************************************/
uint_least16_t USBX_getLibraryVersion(void);

/**  @} (end usbx_func) */

/**  @} (end addtogroup usbxpress) */

#ifdef __cplusplus
}
#endif

#endif  // EM_USBXPRESS_H
