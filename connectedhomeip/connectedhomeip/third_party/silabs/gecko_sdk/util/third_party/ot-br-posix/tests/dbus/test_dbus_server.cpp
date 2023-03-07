/*
 *    Copyright (c) 2019, The OpenThread Authors.
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

#include "dbus/common/dbus_message_helper.hpp"
#include "dbus/server/dbus_object.hpp"

using otbr::DBus::DBusMessageEncodeToVariant;
using otbr::DBus::DBusMessageExtractFromVariant;
using otbr::DBus::DBusMessageToTuple;
using otbr::DBus::DBusObject;
using otbr::DBus::DBusRequest;
using std::placeholders::_1;

class TestObject : public DBusObject
{
public:
    TestObject(DBusConnection *aConnection)
        : DBusObject(aConnection, "/io/openthread/testobj")
        , mEnded(false)
        , mCount(0)
    {
        RegisterMethod("io.openthread", "Ping", std::bind(&TestObject::PingHandler, this, _1));
        RegisterGetPropertyHandler("io.openthread", "Count", std::bind(&TestObject::CountGetHandler, this, _1));
        RegisterSetPropertyHandler("io.openthread", "Count", std::bind(&TestObject::CountSetHandler, this, _1));
    }

    bool IsEnded(void) const { return mEnded; }

private:
    otError CountGetHandler(DBusMessageIter &aIter)
    {
        DBusMessageEncodeToVariant(&aIter, mCount);
        return OT_ERROR_NONE;
    }

    otError CountSetHandler(DBusMessageIter &aIter)
    {
        int32_t cnt = 0;

        DBusMessageExtractFromVariant(&aIter, cnt);
        mCount = cnt;

        return OT_ERROR_NONE;
    }

    void PingHandler(DBusRequest &aRequest)
    {
        uint32_t    id;
        std::string pingMessage;
        auto        args = std::tie(id, pingMessage);

        if (DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE)
        {
            aRequest.Reply(std::make_tuple(id, pingMessage + "Pong"));
        }
        else
        {
            aRequest.Reply(std::make_tuple("hello"));
            mEnded = true;
        }
    }

    bool    mEnded;
    int32_t mCount;
};

int main()
{
    int       ret = EXIT_SUCCESS;
    int       requestReply;
    DBusError dbusErr;

    dbus_error_init(&dbusErr);

    DBusConnection *connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbusErr);
    VerifyOrExit(connection != nullptr);
    dbus_bus_register(connection, &dbusErr);

    requestReply =
        dbus_bus_request_name(connection, "io.openthread.TestServer", DBUS_NAME_FLAG_REPLACE_EXISTING, &dbusErr);
    VerifyOrExit(requestReply == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER ||
                     requestReply == DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER,
                 ret = EXIT_FAILURE);

    {
        TestObject s(connection);
        s.Init();

        while (!s.IsEnded())
        {
            dbus_connection_read_write_dispatch(connection, -1);
        }
    }

exit:
    dbus_error_free(&dbusErr);
    dbus_connection_unref(connection);
    return ret;
}
