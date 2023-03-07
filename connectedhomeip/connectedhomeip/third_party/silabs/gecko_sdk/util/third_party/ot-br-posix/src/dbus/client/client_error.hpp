/*
 *    Copyright (c) 2020, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * This file includes functions for handling d-bus client errors.
 */

#ifndef OTBR_DBUS_CLIENT_CLIENT_ERROR_HPP_
#define OTBR_DBUS_CLIENT_CLIENT_ERROR_HPP_

#include <string>

#include <dbus/dbus.h>

#include "dbus/common/error.hpp"

namespace otbr {
namespace DBus {

/**
 * This function converts an error string to client error.
 *
 * @param[in] aErrorName  The error string.
 *
 * @returns The corresponding otError. OT_ERROR_GENERIC will be returned
 *          if the error name is not defined in OpenThread.
 *
 */
ClientError ConvertFromDBusErrorName(const std::string &aErrorName);

/**
 * This function converts an DBus reply message to otError.
 *
 * @param[in] aMessage  The dbus reply message.
 *
 * @returns The error code encoded in the message.
 *
 */
ClientError CheckErrorMessage(DBusMessage *aMessage);

} // namespace DBus
} // namespace otbr

#endif // OTBR_DBUS_CLIENT_CLIENT_ERROR_HPP_
