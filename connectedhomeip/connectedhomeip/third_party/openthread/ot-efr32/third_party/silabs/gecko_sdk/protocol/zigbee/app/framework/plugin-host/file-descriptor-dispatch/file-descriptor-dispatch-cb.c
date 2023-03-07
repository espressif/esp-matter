/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "af.h"

/** @brief Bad File Descriptor
 *
 * The indicated file descriptor has been detected to be bad; for example,
 * the file descriptor caused an EBADF error on a select() call. The file
 * descriptor will be omitted from subsequent file descriptor polling. The
 * application responsible for the file descriptor should take steps to
 * recover and clean up.
 *
 * @param fd The bad file descriptor Ver.: always
 */
WEAK(void emberAfPluginFileDescriptorDispatchBadFileDescriptorCallback(int fd))
{
}
