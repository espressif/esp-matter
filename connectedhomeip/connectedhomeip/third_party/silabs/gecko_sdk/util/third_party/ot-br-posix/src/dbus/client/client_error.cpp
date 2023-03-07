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

#include "client_error.hpp"

#include "common/code_utils.hpp"
#include "dbus/common/dbus_message_helper.hpp"

#define OTBR_OPENTHREAD_ERROR_PREFIX "io.openthread.Error"

namespace otbr {
namespace DBus {

static const std::pair<ClientError, const char *> sErrorNames[] = {
    {ClientError::ERROR_NONE, OTBR_OPENTHREAD_ERROR_PREFIX ".OK"},
    {ClientError::OT_ERROR_GENERIC, OTBR_OPENTHREAD_ERROR_PREFIX ".Generic"},
    {ClientError::OT_ERROR_FAILED, OTBR_OPENTHREAD_ERROR_PREFIX ".Failed"},
    {ClientError::OT_ERROR_DROP, OTBR_OPENTHREAD_ERROR_PREFIX ".Drop"},
    {ClientError::OT_ERROR_NO_BUFS, OTBR_OPENTHREAD_ERROR_PREFIX ".NoBufs"},
    {ClientError::OT_ERROR_NO_ROUTE, OTBR_OPENTHREAD_ERROR_PREFIX ".NoRoute"},
    {ClientError::OT_ERROR_BUSY, OTBR_OPENTHREAD_ERROR_PREFIX ".Busy"},
    {ClientError::OT_ERROR_PARSE, OTBR_OPENTHREAD_ERROR_PREFIX ".Parse"},
    {ClientError::OT_ERROR_INVALID_ARGS, OTBR_OPENTHREAD_ERROR_PREFIX ".InvalidArgs"},
    {ClientError::OT_ERROR_SECURITY, OTBR_OPENTHREAD_ERROR_PREFIX ".Security"},
    {ClientError::OT_ERROR_ADDRESS_QUERY, OTBR_OPENTHREAD_ERROR_PREFIX ".AddressQuery"},
    {ClientError::OT_ERROR_NO_ADDRESS, OTBR_OPENTHREAD_ERROR_PREFIX ".NoAddress"},
    {ClientError::OT_ERROR_ABORT, OTBR_OPENTHREAD_ERROR_PREFIX ".Abort"},
    {ClientError::OT_ERROR_NOT_IMPLEMENTED, OTBR_OPENTHREAD_ERROR_PREFIX ".NotImplemented"},
    {ClientError::OT_ERROR_INVALID_STATE, OTBR_OPENTHREAD_ERROR_PREFIX ".InvalidState"},
    {ClientError::OT_ERROR_NO_ACK, OTBR_OPENTHREAD_ERROR_PREFIX ".NoAck"},
    {ClientError::OT_ERROR_CHANNEL_ACCESS_FAILURE, OTBR_OPENTHREAD_ERROR_PREFIX ".ChannelAccessFailure"},
    {ClientError::OT_ERROR_DETACHED, OTBR_OPENTHREAD_ERROR_PREFIX ".Detached"},
    {ClientError::OT_ERROR_FCS, OTBR_OPENTHREAD_ERROR_PREFIX ".FcsErr"},
    {ClientError::OT_ERROR_NO_FRAME_RECEIVED, OTBR_OPENTHREAD_ERROR_PREFIX ".NoFrameReceived"},
    {ClientError::OT_ERROR_UNKNOWN_NEIGHBOR, OTBR_OPENTHREAD_ERROR_PREFIX ".UnknownNeighbor"},
    {ClientError::OT_ERROR_INVALID_SOURCE_ADDRESS, OTBR_OPENTHREAD_ERROR_PREFIX ".InvalidSourceAddress"},
    {ClientError::OT_ERROR_ADDRESS_FILTERED, OTBR_OPENTHREAD_ERROR_PREFIX ".AddressFiltered"},
    {ClientError::OT_ERROR_DESTINATION_ADDRESS_FILTERED, OTBR_OPENTHREAD_ERROR_PREFIX ".DestinationAddressFiltered"},
    {ClientError::OT_ERROR_NOT_FOUND, OTBR_OPENTHREAD_ERROR_PREFIX ".NotFound"},
    {ClientError::OT_ERROR_ALREADY, OTBR_OPENTHREAD_ERROR_PREFIX ".Already"},
    {ClientError::OT_ERROR_IP6_ADDRESS_CREATION_FAILURE, OTBR_OPENTHREAD_ERROR_PREFIX ".Ipv6AddressCreationFailure"},
    {ClientError::OT_ERROR_NOT_CAPABLE, OTBR_OPENTHREAD_ERROR_PREFIX ".NotCapable"},
    {ClientError::OT_ERROR_RESPONSE_TIMEOUT, OTBR_OPENTHREAD_ERROR_PREFIX ".ResponseTimeout"},
    {ClientError::OT_ERROR_DUPLICATED, OTBR_OPENTHREAD_ERROR_PREFIX ".Duplicated"},
    {ClientError::OT_ERROR_REASSEMBLY_TIMEOUT, OTBR_OPENTHREAD_ERROR_PREFIX ".ReassemblyTimeout"},
    {ClientError::OT_ERROR_NOT_TMF, OTBR_OPENTHREAD_ERROR_PREFIX ".NotTmf"},
    {ClientError::OT_ERROR_NOT_LOWPAN_DATA_FRAME, OTBR_OPENTHREAD_ERROR_PREFIX ".NonLowpanDatatFrame"},
    {ClientError::OT_ERROR_LINK_MARGIN_LOW, OTBR_OPENTHREAD_ERROR_PREFIX ".LinkMarginLow"},
    {ClientError::OT_ERROR_REJECTED, OTBR_OPENTHREAD_ERROR_PREFIX ".Rejected"},
};

ClientError ConvertFromDBusErrorName(const std::string &aErrorName)
{
    ClientError error = ClientError::ERROR_NONE;

    for (const auto &p : sErrorNames)
    {
        if (p.second == aErrorName)
        {
            error = static_cast<ClientError>(p.first);
            break;
        }
    }
    return error;
}

ClientError CheckErrorMessage(DBusMessage *aMessage)
{
    ClientError error = ClientError::ERROR_NONE;

    if (dbus_message_get_type(aMessage) == DBUS_MESSAGE_TYPE_ERROR)
    {
        std::string errorMsg;
        auto        args = std::tie(errorMsg);

        if (dbus_message_get_type(aMessage) == DBUS_MESSAGE_TYPE_ERROR)
        {
            error = ConvertFromDBusErrorName(dbus_message_get_error_name(aMessage));
        }
        else
        {
            VerifyOrExit(DBusMessageToTuple(*aMessage, args) == OTBR_ERROR_NONE, error = ClientError::ERROR_DBUS);
            error = ConvertFromDBusErrorName(errorMsg);
        }
    }

exit:
    return error;
}

} // namespace DBus
} // namespace otbr
