/***************************************************************************//**
 * @file
 * @brief USB Host Class-Core Interface
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

#ifndef  _USBH_CLASS_CORE_PRIV_H_
#define  _USBH_CLASS_CORE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_class.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               CLASS CODES
 *
 * Note(s) : (1) See 'Universal Class Codes', www.usb.org/developers/defined_class.
 *
 *           (2) Except as noted, these should be used ONLY in interface descriptors.
 *
 *               (a) Can only be used in device descriptor.
 *
 *               (b) Can be used in either device or interface descriptor.
 *
 *           (3) Subclass & protocol codes are defined in the relevant class drivers.
 *******************************************************************************************************/

#define  USBH_CLASS_CODE_USE_IF_DESC                    0x00u   // See Notes #2a.
#define  USBH_CLASS_CODE_AUDIO                          0x01u
#define  USBH_CLASS_CODE_CDC_CTRL                       0x02u   // See Notes #2b.
#define  USBH_CLASS_CODE_HID                            0x03u
#define  USBH_CLASS_CODE_PHYSICAL                       0x05u
#define  USBH_CLASS_CODE_IMAGE                          0x06u
#define  USBH_CLASS_CODE_PRINTER                        0x07u
#define  USBH_CLASS_CODE_MASS_STORAGE                   0x08u
#define  USBH_CLASS_CODE_HUB                            0x09u   // See Notes #2a.
#define  USBH_CLASS_CODE_CDC_DATA                       0x0Au
#define  USBH_CLASS_CODE_SMART_CARD                     0x0Bu
#define  USBH_CLASS_CODE_CONTENT_SECURITY               0x0Du
#define  USBH_CLASS_CODE_VIDEO                          0x0Eu
#define  USBH_CLASS_CODE_PERSONAL_HEALTHCARE            0x0Fu
#define  USBH_CLASS_CODE_DIAGNOSTIC_DEV                 0xDCu   // See Notes #2b
#define  USBH_CLASS_CODE_WIRELESS_CTRLR                 0xE0u
#define  USBH_CLASS_CODE_MISCELLANEOUS                  0xEFu   // See Notes #2b.
#define  USBH_CLASS_CODE_APP_SPECIFIC                   0xFEu
#define  USBH_CLASS_CODE_VENDOR_SPECIFIC                0xFFu   // See Notes #2b.

/********************************************************************************************************
 *                                           SUBCLASS CODES
 *
 * Note(s) : (1) See 'Universal Class Codes', www.usb.org/developers/defined_class.
 *
 *           (2) Except as noted, these should be used ONLY in interface descriptors.
 *
 *               (a) Can only be used in device descriptor.
 *
 *               (b) Can be used in either device or interface descriptor.
 *
 *           (3) Subclass & protocol codes are defined in the relevant class drivers.
 *******************************************************************************************************/

#define  USBH_SUBCLASS_CODE_USE_IF_DESC                 0x00u   // See Notes #2a.
#define  USBH_SUBCLASS_CODE_USE_COMMON_CLASS            0x02u   // See Notes #2a.
#define  USBH_SUBCLASS_CODE_VENDOR_SPECIFIC             0xFFu   // See Notes #2b.

/********************************************************************************************************
 *                                           PROTOCOL CODES
 *
 * Note(s) : (1) See 'Universal Class Codes', www.usb.org/developers/defined_class.
 *
 *           (2) Except as noted, these should be used ONLY in interface descriptors.
 *
 *               (a) Can only be used in device descriptor.
 *
 *               (b) See "USB Interface Association Descriptor Device Class Code and
 *                   Use Model" Document at www.usb.org/developers/whitepapers/iadclasscode_r10.pdf.
 *
 *               (c) Can be used in either device or interface descriptor.
 *
 *           (3) Subclass & protocol codes are defined in the relevant class drivers.
 *******************************************************************************************************/

#define  USBH_PROTOCOL_CODE_USE_IF_DESC                 0x00u   // See Notes #2a.
#define  USBH_PROTOCOL_CODE_USE_IAD                     0x01u   // See Notes #2b.
#define  USBH_PROTOCOL_CODE_VENDOR_SPECIFIC             0xFFu   // See Notes #2c.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef const struct usbh_class_drv USBH_CLASS_DRV;

/********************************************************************************************************
 *                                               CLASS DRIVER
 *******************************************************************************************************/

struct usbh_class_drv {
  //                                                               Probe class driver at dev level.
  CPU_BOOLEAN (*ProbeDev)   (USBH_DEV_HANDLE dev_handle,
                             CPU_INT08U      class_code,
                             void            **pp_class_fnct,
                             RTOS_ERR        *p_err);

  //                                                               Probe class driver at fnct level.
  CPU_BOOLEAN (*ProbeFnct)  (USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             CPU_INT08U       class_code,
                             void             **pp_class_fnct,
                             RTOS_ERR         *p_err);

  //                                                               Probe class driver to see if EP should be open.
  CPU_BOOLEAN (*EP_Probe)   (void        *p_class_fnct,
                             CPU_INT08U  if_ix,
                             CPU_INT08U  ep_type,
                             CPU_BOOLEAN ep_dir_in);

  //                                                               Notify class driver endpoint open.
  void (*EP_Open)    (void           *p_class_fnct,
                      USBH_EP_HANDLE ep_handle,
                      CPU_INT08U     if_ix,
                      CPU_INT08U     ep_type,
                      CPU_BOOLEAN    ep_dir_in);

  //                                                               Notify class driver endpoint close.
  void (*EP_Close)   (void           *p_class_fnct,
                      USBH_EP_HANDLE ep_handle,
                      CPU_INT08U     if_ix);

  //                                                               Notify class driver function is ready for comm.
  void (*Conn)       (void *p_class_fnct);

  //                                                               Notify class driver alt setting was set.
  void (*IF_AltSet)  (void       *p_class_fnct,
                      CPU_INT08U if_ix);

  //                                                               Notify class driver function was disconnected.
  void (*Disconn)    (void *p_class_fnct);

  //                                                               Notify class driver function was suspended.
  void (*Suspend)    (void *p_class_fnct);

  //                                                               Notify class driver function was resumed.
  void (*Resume)     (void *p_class_fnct);
  //                                                               Uninit class driver.
  void (*UnInit)     (void);

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  //                                                               Dump class specific debug information on function.
  void (*TraceDump)(void *p_class_fnct,
                    CPU_INT32U opt,
                    USBH_CMD_TRACE_FNCT trace_fnct);
#endif
  //                                                               Human readable name for class driver.
  CPU_CHAR *NamePtr;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_ClassDrvReg(USBH_CLASS_DRV *p_class_drv,
                      RTOS_ERR       *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
