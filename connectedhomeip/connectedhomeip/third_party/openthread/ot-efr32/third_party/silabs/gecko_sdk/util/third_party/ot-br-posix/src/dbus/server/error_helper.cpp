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

#include "dbus/server/error_helper.hpp"
#include "common/code_utils.hpp"
#include "dbus/common/dbus_message_helper.hpp"

#define OPENTHREAD_ERROR_PREFIX "io.openthread.Error"

static const std::pair<otError, const char *> sErrorNames[] = {
    {OT_ERROR_NONE, OPENTHREAD_ERROR_PREFIX ".OK"},
    {OT_ERROR_GENERIC, OPENTHREAD_ERROR_PREFIX ".Generic"},
    {OT_ERROR_FAILED, OPENTHREAD_ERROR_PREFIX ".Failed"},
    {OT_ERROR_DROP, OPENTHREAD_ERROR_PREFIX ".Drop"},
    {OT_ERROR_NO_BUFS, OPENTHREAD_ERROR_PREFIX ".NoBufs"},
    {OT_ERROR_NO_ROUTE, OPENTHREAD_ERROR_PREFIX ".NoRoute"},
    {OT_ERROR_BUSY, OPENTHREAD_ERROR_PREFIX ".Busy"},
    {OT_ERROR_PARSE, OPENTHREAD_ERROR_PREFIX ".Parse"},
    {OT_ERROR_INVALID_ARGS, OPENTHREAD_ERROR_PREFIX ".InvalidArgs"},
    {OT_ERROR_SECURITY, OPENTHREAD_ERROR_PREFIX ".Security"},
    {OT_ERROR_ADDRESS_QUERY, OPENTHREAD_ERROR_PREFIX ".AddressQuery"},
    {OT_ERROR_NO_ADDRESS, OPENTHREAD_ERROR_PREFIX ".NoAddress"},
    {OT_ERROR_ABORT, OPENTHREAD_ERROR_PREFIX ".Abort"},
    {OT_ERROR_NOT_IMPLEMENTED, OPENTHREAD_ERROR_PREFIX ".NotImplemented"},
    {OT_ERROR_INVALID_STATE, OPENTHREAD_ERROR_PREFIX ".InvalidState"},
    {OT_ERROR_NO_ACK, OPENTHREAD_ERROR_PREFIX ".NoAck"},
    {OT_ERROR_CHANNEL_ACCESS_FAILURE, OPENTHREAD_ERROR_PREFIX ".ChannelAccessFailure"},
    {OT_ERROR_DETACHED, OPENTHREAD_ERROR_PREFIX ".Detached"},
    {OT_ERROR_FCS, OPENTHREAD_ERROR_PREFIX ".FcsErr"},
    {OT_ERROR_NO_FRAME_RECEIVED, OPENTHREAD_ERROR_PREFIX ".NoFrameReceived"},
    {OT_ERROR_UNKNOWN_NEIGHBOR, OPENTHREAD_ERROR_PREFIX ".UnknownNeighbor"},
    {OT_ERROR_INVALID_SOURCE_ADDRESS, OPENTHREAD_ERROR_PREFIX ".InvalidSourceAddress"},
    {OT_ERROR_ADDRESS_FILTERED, OPENTHREAD_ERROR_PREFIX ".AddressFiltered"},
    {OT_ERROR_DESTINATION_ADDRESS_FILTERED, OPENTHREAD_ERROR_PREFIX ".DestinationAddressFiltered"},
    {OT_ERROR_NOT_FOUND, OPENTHREAD_ERROR_PREFIX ".NotFound"},
    {OT_ERROR_ALREADY, OPENTHREAD_ERROR_PREFIX ".Already"},
    {OT_ERROR_IP6_ADDRESS_CREATION_FAILURE, OPENTHREAD_ERROR_PREFIX ".Ipv6AddressCreationFailure"},
    {OT_ERROR_NOT_CAPABLE, OPENTHREAD_ERROR_PREFIX ".NotCapable"},
    {OT_ERROR_RESPONSE_TIMEOUT, OPENTHREAD_ERROR_PREFIX ".ResponseTimeout"},
    {OT_ERROR_DUPLICATED, OPENTHREAD_ERROR_PREFIX ".Duplicated"},
    {OT_ERROR_REASSEMBLY_TIMEOUT, OPENTHREAD_ERROR_PREFIX ".ReassemblyTimeout"},
    {OT_ERROR_NOT_TMF, OPENTHREAD_ERROR_PREFIX ".NotTmf"},
    {OT_ERROR_NOT_LOWPAN_DATA_FRAME, OPENTHREAD_ERROR_PREFIX ".NonLowpanDatatFrame"},
    {OT_ERROR_LINK_MARGIN_LOW, OPENTHREAD_ERROR_PREFIX ".LinkMarginLow"},
    {OT_ERROR_REJECTED, OPENTHREAD_ERROR_PREFIX ".Rejected"},
};

namespace otbr {
namespace DBus {

const char *ConvertToDBusErrorName(otError aError)
{
    const char *name = sErrorNames[0].second;

    for (const auto &p : sErrorNames)
    {
        if (p.first == aError)
        {
            name = p.second;
            break;
        }
    }
    return name;
}

} // namespace DBus
} // namespace otbr
