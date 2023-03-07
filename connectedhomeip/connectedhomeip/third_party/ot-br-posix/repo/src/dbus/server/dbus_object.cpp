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

#define OTBR_LOG_TAG "DBUS"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <dbus/dbus.h>

#include "common/logging.hpp"
#include "dbus/common/dbus_message_dump.hpp"
#include "dbus/server/dbus_object.hpp"

using std::placeholders::_1;

namespace otbr {
namespace DBus {

DBusObject::DBusObject(DBusConnection *aConnection, const std::string &aObjectPath)
    : mConnection(aConnection)
    , mObjectPath(aObjectPath)
{
}

otbrError DBusObject::Init(void)
{
    otbrError            error = OTBR_ERROR_NONE;
    DBusObjectPathVTable vTable;

    memset(&vTable, 0, sizeof(vTable));

    vTable.message_function = DBusObject::sMessageHandler;

    VerifyOrExit(dbus_connection_register_object_path(mConnection, mObjectPath.c_str(), &vTable, this),
                 error = OTBR_ERROR_DBUS);
    RegisterMethod(DBUS_INTERFACE_PROPERTIES, DBUS_PROPERTY_GET_METHOD,
                   std::bind(&DBusObject::GetPropertyMethodHandler, this, _1));
    RegisterMethod(DBUS_INTERFACE_PROPERTIES, DBUS_PROPERTY_SET_METHOD,
                   std::bind(&DBusObject::SetPropertyMethodHandler, this, _1));
    RegisterMethod(DBUS_INTERFACE_PROPERTIES, DBUS_PROPERTY_GET_ALL_METHOD,
                   std::bind(&DBusObject::GetAllPropertiesMethodHandler, this, _1));

exit:
    return error;
}

void DBusObject::RegisterMethod(const std::string &      aInterfaceName,
                                const std::string &      aMethodName,
                                const MethodHandlerType &aHandler)
{
    std::string fullPath = aInterfaceName + "." + aMethodName;

    assert(mMethodHandlers.find(fullPath) == mMethodHandlers.end());
    mMethodHandlers.emplace(fullPath, aHandler);
}

void DBusObject::RegisterGetPropertyHandler(const std::string &        aInterfaceName,
                                            const std::string &        aPropertyName,
                                            const PropertyHandlerType &aHandler)
{
    mGetPropertyHandlers[aInterfaceName].emplace(aPropertyName, aHandler);
}

void DBusObject::RegisterSetPropertyHandler(const std::string &        aInterfaceName,
                                            const std::string &        aPropertyName,
                                            const PropertyHandlerType &aHandler)
{
    std::string fullPath = aInterfaceName + "." + aPropertyName;

    assert(mSetPropertyHandlers.find(fullPath) == mSetPropertyHandlers.end());
    mSetPropertyHandlers.emplace(fullPath, aHandler);
}

DBusHandlerResult DBusObject::sMessageHandler(DBusConnection *aConnection, DBusMessage *aMessage, void *aData)
{
    DBusObject *server = reinterpret_cast<DBusObject *>(aData);

    return server->MessageHandler(aConnection, aMessage);
}

DBusHandlerResult DBusObject::MessageHandler(DBusConnection *aConnection, DBusMessage *aMessage)
{
    DBusHandlerResult handled = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    DBusRequest       request(aConnection, aMessage);
    std::string       interface  = dbus_message_get_interface(aMessage);
    std::string       memberName = interface + "." + dbus_message_get_member(aMessage);
    auto              iter       = mMethodHandlers.find(memberName);

    if (dbus_message_get_type(aMessage) == DBUS_MESSAGE_TYPE_METHOD_CALL && iter != mMethodHandlers.end())
    {
        otbrLogInfo("Handling method %s", memberName.c_str());
        if (otbrLogGetLevel() >= OTBR_LOG_DEBUG)
        {
            DumpDBusMessage(*aMessage);
        }
        (iter->second)(request);
        handled = DBUS_HANDLER_RESULT_HANDLED;
    }

    return handled;
}

void DBusObject::GetPropertyMethodHandler(DBusRequest &aRequest)
{
    UniqueDBusMessage reply{dbus_message_new_method_return(aRequest.GetMessage())};

    DBusMessageIter iter;
    std::string     interfaceName;
    std::string     propertyName;
    otError         error      = OT_ERROR_NONE;
    otError         replyError = OT_ERROR_NONE;

    VerifyOrExit(reply != nullptr, error = OT_ERROR_NO_BUFS);
    VerifyOrExit(dbus_message_iter_init(aRequest.GetMessage(), &iter), error = OT_ERROR_FAILED);
    VerifyOrExit(DBusMessageExtract(&iter, interfaceName) == OTBR_ERROR_NONE, error = OT_ERROR_PARSE);
    VerifyOrExit(DBusMessageExtract(&iter, propertyName) == OTBR_ERROR_NONE, error = OT_ERROR_PARSE);
    {
        auto propertyIter = mGetPropertyHandlers.find(interfaceName);

        otbrLogInfo("GetProperty %s.%s", interfaceName.c_str(), propertyName.c_str());
        VerifyOrExit(propertyIter != mGetPropertyHandlers.end(), error = OT_ERROR_NOT_FOUND);
        {
            DBusMessageIter replyIter;
            auto &          interfaceHandlers = propertyIter->second;
            auto            interfaceIter     = interfaceHandlers.find(propertyName);

            VerifyOrExit(interfaceIter != interfaceHandlers.end(), error = OT_ERROR_NOT_FOUND);
            dbus_message_iter_init_append(reply.get(), &replyIter);
            SuccessOrExit(replyError = interfaceIter->second(replyIter));
        }
    }
exit:
    if (error == OT_ERROR_NONE && replyError == OT_ERROR_NONE)
    {
        if (otbrLogGetLevel() >= OTBR_LOG_DEBUG)
        {
            otbrLogDebug("GetProperty %s.%s reply:", interfaceName.c_str(), propertyName.c_str());
            DumpDBusMessage(*reply);
        }

        dbus_connection_send(aRequest.GetConnection(), reply.get(), nullptr);
    }
    else if (error == OT_ERROR_NONE)
    {
        otbrLogInfo("GetProperty %s.%s reply:%s", interfaceName.c_str(), propertyName.c_str(),
                    ConvertToDBusErrorName(replyError));
        aRequest.ReplyOtResult(replyError);
    }
    else
    {
        otbrLogWarning("GetProperty %s.%s error:%s", interfaceName.c_str(), propertyName.c_str(),
                       ConvertToDBusErrorName(error));
        aRequest.ReplyOtResult(error);
    }
}

void DBusObject::GetAllPropertiesMethodHandler(DBusRequest &aRequest)
{
    UniqueDBusMessage reply{dbus_message_new_method_return(aRequest.GetMessage())};
    DBusMessageIter   iter, subIter, dictEntryIter;
    std::string       interfaceName;
    auto              args  = std::tie(interfaceName);
    otError           error = OT_ERROR_NONE;

    VerifyOrExit(reply != nullptr, error = OT_ERROR_NO_BUFS);
    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_PARSE);
    VerifyOrExit(mGetPropertyHandlers.find(interfaceName) != mGetPropertyHandlers.end(), error = OT_ERROR_NOT_FOUND);
    dbus_message_iter_init_append(reply.get(), &iter);

    for (auto &p : mGetPropertyHandlers.at(interfaceName))
    {
        VerifyOrExit(dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                                      "{" DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_VARIANT_AS_STRING "}",
                                                      &subIter),
                     error = OT_ERROR_FAILED);
        VerifyOrExit(dbus_message_iter_open_container(&subIter, DBUS_TYPE_DICT_ENTRY, nullptr, &dictEntryIter),
                     error = OT_ERROR_FAILED);
        VerifyOrExit(DBusMessageEncode(&dictEntryIter, p.first) == OTBR_ERROR_NONE, error = OT_ERROR_FAILED);

        SuccessOrExit(error = p.second(dictEntryIter));

        VerifyOrExit(dbus_message_iter_close_container(&subIter, &dictEntryIter), error = OT_ERROR_FAILED);
        VerifyOrExit(dbus_message_iter_close_container(&iter, &subIter));
    }

exit:
    if (error == OT_ERROR_NONE)
    {
        dbus_connection_send(aRequest.GetConnection(), reply.get(), nullptr);
    }
    else
    {
        aRequest.ReplyOtResult(error);
    }
}

void DBusObject::SetPropertyMethodHandler(DBusRequest &aRequest)
{
    DBusMessageIter iter;
    std::string     interfaceName;
    std::string     propertyName;
    std::string     propertyFullPath;
    otError         error = OT_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_init(aRequest.GetMessage(), &iter), error = OT_ERROR_FAILED);
    VerifyOrExit(DBusMessageExtract(&iter, interfaceName) == OTBR_ERROR_NONE, error = OT_ERROR_PARSE);
    VerifyOrExit(DBusMessageExtract(&iter, propertyName) == OTBR_ERROR_NONE, error = OT_ERROR_PARSE);

    propertyFullPath = interfaceName + "." + propertyName;
    otbrLogInfo("SetProperty %s", propertyFullPath.c_str());
    {
        auto handlerIter = mSetPropertyHandlers.find(propertyFullPath);

        VerifyOrExit(handlerIter != mSetPropertyHandlers.end(), error = OT_ERROR_NOT_FOUND);
        error = handlerIter->second(iter);
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        otbrLogWarning("SetProperty %s.%s error:%s", interfaceName.c_str(), propertyName.c_str(),
                       ConvertToDBusErrorName(error));
    }
    aRequest.ReplyOtResult(error);
    return;
}

DBusObject::~DBusObject(void)
{
}

UniqueDBusMessage DBusObject::NewSignalMessage(const std::string &aInterfaceName, const std::string &aSignalName)
{
    return UniqueDBusMessage(dbus_message_new_signal(mObjectPath.c_str(), aInterfaceName.c_str(), aSignalName.c_str()));
}

} // namespace DBus
} // namespace otbr
