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

#define OTBR_LOG_TAG "DBUS"

#include "dbus_message_dump.hpp"

#include <sstream>

#include "common/code_utils.hpp"
#include "common/logging.hpp"

namespace otbr {
namespace DBus {

static void DumpDBusMessage(std::ostringstream &sout, DBusMessageIter *aIter)
{
    int type = dbus_message_iter_get_arg_type(aIter);

    while (type != DBUS_TYPE_INVALID)
    {
        switch (type)
        {
        case DBUS_TYPE_BOOLEAN:
        {
            dbus_bool_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << (value ? "true, " : "false, ");
            break;
        }
        case DBUS_TYPE_BYTE:
        {
            uint8_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << static_cast<int>(value) << ", ";
            break;
        }
        case DBUS_TYPE_INT16:
        {
            int16_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << value << ", ";
            break;
        }
        case DBUS_TYPE_UINT16:
        {
            uint16_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << value << ", ";
            break;
        }
        case DBUS_TYPE_INT32:
        {
            int32_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << value << ", ";
            break;
        }
        case DBUS_TYPE_UINT32:
        {
            uint32_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << value << ", ";
            break;
        }
        case DBUS_TYPE_INT64:
        {
            int64_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << value << ", ";
            break;
        }
        case DBUS_TYPE_UINT64:
        {
            uint64_t value;

            dbus_message_iter_get_basic(aIter, &value);
            sout << value << ", ";
            break;
        }
        case DBUS_TYPE_STRING:
        {
            const char *buf;

            dbus_message_iter_get_basic(aIter, &buf);
            sout << "\"" << buf << "\", ";
            break;
        }
        case DBUS_TYPE_ARRAY:
        {
            DBusMessageIter subIter;

            dbus_message_iter_recurse(aIter, &subIter);
            sout << "[ ";
            DumpDBusMessage(sout, &subIter);
            sout << "], ";
            break;
        }
        case DBUS_TYPE_STRUCT:
        case DBUS_TYPE_VARIANT:
        {
            DBusMessageIter subIter;

            dbus_message_iter_recurse(aIter, &subIter);
            sout << "{ ";
            DumpDBusMessage(sout, &subIter);
            sout << " }, ";
            break;
        }
        case DBUS_TYPE_DICT_ENTRY:
        {
            DBusMessageIter subIter;
            char *          key;

            dbus_message_iter_recurse(aIter, &subIter);
            dbus_message_iter_get_basic(&subIter, &key);
            sout << key << ":{ ";
            dbus_message_iter_next(&subIter);
            if (dbus_message_iter_get_arg_type(&subIter) == DBUS_TYPE_VARIANT)
            {
                DBusMessageIter valueIter;
                dbus_message_iter_recurse(&subIter, &valueIter);
                DumpDBusMessage(sout, &valueIter);
            }
            sout << "}, ";
            break;
        }
        case DBUS_TYPE_INVALID:
        default:
            break;
        }
        dbus_message_iter_next(aIter);
        type = dbus_message_iter_get_arg_type(aIter);
    }
    return;
}

void DumpDBusMessage(DBusMessage &aMessage)
{
    DBusMessageIter    iter;
    std::ostringstream sout;

    VerifyOrExit(dbus_message_iter_init(&aMessage, &iter), otbrLogDebug("Failed to iterate dbus message during dump"));
    sout << "{ ";
    DumpDBusMessage(sout, &iter);
    sout << "}";
    otbrLogDebug(sout.str().c_str());
exit:
    return;
}

} // namespace DBus
} // namespace otbr
