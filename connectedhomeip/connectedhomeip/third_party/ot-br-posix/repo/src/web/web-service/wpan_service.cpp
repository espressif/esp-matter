/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the wpan controller service
 */

#define OTBR_LOG_TAG "WEB"

#include "web/web-service/wpan_service.hpp"

#include <sstream>

#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

#include "common/byteswap.hpp"
#include "common/code_utils.hpp"

namespace otbr {
namespace Web {

#define WPAN_RESPONSE_SUCCESS "successful"
#define WPAN_RESPONSE_FAILURE "failed"
#define CREDENTIAL_TYPE_NETWORK_KEY "networkKeyType"
#define CREDENTIAL_TYPE_PSKD "pskdType"

std::string WpanService::HandleGetQRCodeRequest()
{
    Json::Value                 root, networkInfo;
    Json::FastWriter            jsonWriter;
    std::string                 response;
    int                         ret = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);
    char *                      rval;

    VerifyOrExit(client.Connect(), ret = kWpanStatus_SetFailed);

    // eui64 is the only required information to generate the QR code.
    VerifyOrExit((rval = client.Execute("eui64")) != nullptr, ret = kWpanStatus_GetPropertyFailed);

exit:

    root.clear();
    root["result"] = WPAN_RESPONSE_SUCCESS;

    if (ret == kWpanStatus_Ok)
    {
        root["eui64"] = rval;
    }
    else
    {
        root["result"] = WPAN_RESPONSE_FAILURE;
        otbrLogErr("Wpan service error: %d", ret);
    }

    response = jsonWriter.write(root);
    return response;
}

std::string WpanService::HandleJoinNetworkRequest(const std::string &aJoinRequest)
{
    Json::Value                 root;
    Json::Reader                reader;
    Json::FastWriter            jsonWriter;
    std::string                 response;
    int                         index;
    std::string                 credentialType;
    std::string                 networkKey;
    std::string                 pskd;
    std::string                 prefix;
    bool                        defaultRoute;
    int                         ret = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);
    char *                      rval;

    VerifyOrExit(client.Connect(), ret = kWpanStatus_SetFailed);

    VerifyOrExit(reader.parse(aJoinRequest.c_str(), root) == true, ret = kWpanStatus_ParseRequestFailed);
    index          = root["index"].asUInt();
    credentialType = root["credentialType"].asString();
    networkKey     = root["networkKey"].asString();
    pskd           = root["pskd"].asString();
    prefix         = root["prefix"].asString();
    defaultRoute   = root["defaultRoute"].asBool();

    if (prefix.find('/') == std::string::npos)
    {
        prefix += "/64";
    }

    VerifyOrExit(client.FactoryReset(), ret = kWpanStatus_LeaveFailed);

    if (credentialType == CREDENTIAL_TYPE_NETWORK_KEY)
    {
        VerifyOrExit((ret = joinActiveDataset(client, networkKey, mNetworks[index].mChannel,
                                              mNetworks[index].mPanId)) == kWpanStatus_Ok);
        VerifyOrExit(client.Execute("ifconfig up") != nullptr, ret = kWpanStatus_JoinFailed);
    }
    else if (credentialType == CREDENTIAL_TYPE_PSKD)
    {
        VerifyOrExit(client.Execute("ifconfig up") != nullptr, ret = kWpanStatus_JoinFailed);
        VerifyOrExit(client.Execute("joiner start %s", pskd.c_str()) != nullptr, ret = kWpanStatus_JoinFailed);
        VerifyOrExit((rval = client.Read("Join ", 5000)) != nullptr, ret = kWpanStatus_JoinFailed);
        if (strstr(rval, "Join success"))
        {
            ExitNow();
        }
        else if (strstr(rval, "Join failed [NotFound]"))
        {
            ExitNow(ret = kWpanStatus_JoinFailed_NotFound);
        }
        else if (strstr(rval, "Join failed [Security]"))
        {
            ExitNow(ret = kWpanStatus_JoinFailed_Security);
        }
        else
        {
            ExitNow(ret = kWpanStatus_JoinFailed);
        }
    }
    else
    {
        ExitNow(ret = kWpanStatus_JoinFailed);
    }

    VerifyOrExit(client.Execute("thread start") != nullptr, ret = kWpanStatus_JoinFailed);
    VerifyOrExit(client.Execute("prefix add %s paso%s", prefix.c_str(), (defaultRoute ? "r" : "")) != nullptr,
                 ret = kWpanStatus_SetFailed);

exit:

    root.clear();
    root["result"] = WPAN_RESPONSE_SUCCESS;

    root["error"] = ret;
    if (ret != kWpanStatus_Ok)
    {
        otbrLogErr("Wpan service error: %d", ret);
        root["result"] = WPAN_RESPONSE_FAILURE;
    }

    root["message"] = "";
    if (ret == kWpanStatus_JoinFailed_NotFound)
    {
        root["message"] = "Please make sure this joiner has been added by an active commissioner.";
    }
    else if (ret == kWpanStatus_JoinFailed_Security)
    {
        root["message"] = "Please make sure the provided PSKd matches the one given to the commissioner.";
    }

    response = jsonWriter.write(root);
    return response;
}

std::string WpanService::HandleFormNetworkRequest(const std::string &aFormRequest)
{
    Json::Value                 root;
    Json::FastWriter            jsonWriter;
    Json::Reader                reader;
    std::string                 response;
    otbr::Psk::Pskc             psk;
    char                        pskcStr[OT_PSKC_MAX_LENGTH * 2 + 1];
    uint8_t                     extPanIdBytes[OT_EXTENDED_PANID_LENGTH];
    std::string                 networkKey;
    std::string                 prefix;
    uint16_t                    channel;
    std::string                 networkName;
    std::string                 passphrase;
    uint16_t                    panId;
    uint64_t                    extPanId;
    bool                        defaultRoute;
    int                         ret = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);

    VerifyOrExit(client.Connect(), ret = kWpanStatus_SetFailed);

    pskcStr[OT_PSKC_MAX_LENGTH * 2] = '\0'; // for manipulating with strlen
    VerifyOrExit(reader.parse(aFormRequest.c_str(), root) == true, ret = kWpanStatus_ParseRequestFailed);
    networkKey  = root["networkKey"].asString();
    prefix      = root["prefix"].asString();
    channel     = root["channel"].asUInt();
    networkName = root["networkName"].asString();
    passphrase  = root["passphrase"].asString();
    VerifyOrExit(sscanf(root["panId"].asString().c_str(), "%hx", &panId) == 1, ret = kWpanStatus_ParseRequestFailed);
    VerifyOrExit(sscanf(root["extPanId"].asString().c_str(), "%" PRIx64, &extPanId) == 1,
                 ret = kWpanStatus_ParseRequestFailed);
    defaultRoute = root["defaultRoute"].asBool();

    otbr::Utils::Hex2Bytes(root["extPanId"].asString().c_str(), extPanIdBytes, OT_EXTENDED_PANID_LENGTH);
    otbr::Utils::Bytes2Hex(psk.ComputePskc(extPanIdBytes, networkName.c_str(), passphrase.c_str()), OT_PSKC_MAX_LENGTH,
                           pskcStr);

    if (prefix.find('/') == std::string::npos)
    {
        prefix += "/64";
    }

    VerifyOrExit(client.FactoryReset(), ret = kWpanStatus_LeaveFailed);
    VerifyOrExit((ret = formActiveDataset(client, networkKey, networkName, pskcStr, channel, extPanId, panId)) ==
                 kWpanStatus_Ok);
    VerifyOrExit(client.Execute("ifconfig up") != nullptr, ret = kWpanStatus_FormFailed);
    VerifyOrExit(client.Execute("thread start") != nullptr, ret = kWpanStatus_FormFailed);
    VerifyOrExit(client.Execute("prefix add %s paso%s", prefix.c_str(), (defaultRoute ? "r" : "")) != nullptr,
                 ret = kWpanStatus_SetFailed);
exit:

    root.clear();

    root["result"] = WPAN_RESPONSE_SUCCESS;
    root["error"]  = ret;
    if (ret != kWpanStatus_Ok)
    {
        otbrLogErr("Wpan service error: %d", ret);
        root["result"] = WPAN_RESPONSE_FAILURE;
    }
    response = jsonWriter.write(root);
    return response;
}

std::string WpanService::HandleAddPrefixRequest(const std::string &aAddPrefixRequest)
{
    Json::Value                 root;
    Json::FastWriter            jsonWriter;
    Json::Reader                reader;
    std::string                 response;
    std::string                 prefix;
    bool                        defaultRoute;
    int                         ret = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);

    VerifyOrExit(client.Connect(), ret = kWpanStatus_SetFailed);

    VerifyOrExit(reader.parse(aAddPrefixRequest.c_str(), root) == true, ret = kWpanStatus_ParseRequestFailed);
    prefix       = root["prefix"].asString();
    defaultRoute = root["defaultRoute"].asBool();

    if (prefix.find('/') == std::string::npos)
    {
        prefix += "/64";
    }

    VerifyOrExit(client.Execute("prefix add %s paso%s", prefix.c_str(), (defaultRoute ? "r" : "")) != nullptr,
                 ret = kWpanStatus_SetGatewayFailed);
    VerifyOrExit(client.Execute("netdata register") != nullptr, ret = kWpanStatus_SetGatewayFailed);
exit:

    root.clear();

    root["result"] = WPAN_RESPONSE_SUCCESS;
    root["error"]  = ret;
    if (ret != kWpanStatus_Ok)
    {
        otbrLogErr("Wpan service error: %d", ret);
        root["result"] = WPAN_RESPONSE_FAILURE;
    }
    response = jsonWriter.write(root);
    return response;
}

std::string WpanService::HandleDeletePrefixRequest(const std::string &aDeleteRequest)
{
    Json::Value                 root;
    Json::FastWriter            jsonWriter;
    Json::Reader                reader;
    std::string                 response;
    std::string                 prefix;
    int                         ret = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);

    VerifyOrExit(client.Connect(), ret = kWpanStatus_SetFailed);

    VerifyOrExit(reader.parse(aDeleteRequest.c_str(), root) == true, ret = kWpanStatus_ParseRequestFailed);
    prefix = root["prefix"].asString();

    if (prefix.find('/') == std::string::npos)
    {
        prefix += "/64";
    }

    VerifyOrExit(client.Execute("prefix remove %s", prefix.c_str()) != nullptr, ret = kWpanStatus_SetGatewayFailed);
    VerifyOrExit(client.Execute("netdata register") != nullptr, ret = kWpanStatus_SetGatewayFailed);
exit:

    root.clear();
    root["result"] = WPAN_RESPONSE_SUCCESS;

    root["error"] = ret;
    if (ret != kWpanStatus_Ok)
    {
        otbrLogErr("Wpan service error: %d", ret);
        root["result"] = WPAN_RESPONSE_FAILURE;
    }
    response = jsonWriter.write(root);
    return response;
}

std::string WpanService::HandleStatusRequest()
{
    Json::Value                 root, networkInfo;
    Json::FastWriter            jsonWriter;
    std::string                 response, networkName, extPanId, propertyValue;
    int                         ret = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);
    char *                      rval;

    networkInfo["WPAN service"] = "uninitialized";
    VerifyOrExit(client.Connect(), ret = kWpanStatus_SetFailed);

    VerifyOrExit((rval = client.Execute("state")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["RCP:State"] = rval;

    if (!strcmp(rval, "disabled"))
    {
        networkInfo["WPAN service"] = "offline";
        ExitNow();
    }
    else if (!strcmp(rval, "detached"))
    {
        networkInfo["WPAN service"] = "associating";
        ExitNow();
    }
    else
    {
        networkInfo["WPAN service"] = "associated";
    }

    VerifyOrExit((rval = client.Execute("version")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["OpenThread:Version"] = rval;

    VerifyOrExit((rval = client.Execute("version api")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["OpenThread:Version API"] = rval;

    VerifyOrExit((rval = client.Execute("rcp version")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["RCP:Version"] = rval;

    VerifyOrExit((rval = client.Execute("eui64")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["RCP:EUI64"] = rval;

    VerifyOrExit((rval = client.Execute("channel")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["RCP:Channel"] = rval;

    VerifyOrExit((rval = client.Execute("txpower")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["RCP:TxPower"] = rval;

    VerifyOrExit((rval = client.Execute("networkname")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["Network:Name"] = rval;

    VerifyOrExit((rval = client.Execute("extpanid")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["Network:XPANID"] = rval;

    VerifyOrExit((rval = client.Execute("panid")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["Network:PANID"] = rval;

    VerifyOrExit((rval = client.Execute("partitionid")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
    networkInfo["Network:PartitionID"] = rval;

    {
        static const char kMeshLocalPrefixLocator[]       = "Mesh Local Prefix: ";
        static const char kMeshLocalAddressTokenLocator[] = "0:ff:fe00:";
        static const char localAddressToken[]             = "fd";
        static const char linkLocalAddressToken[]         = "fe80";
        std::string       meshLocalPrefix                 = "";

        VerifyOrExit((rval = client.Execute("dataset active")) != nullptr, ret = kWpanStatus_GetPropertyFailed);
        rval = strstr(rval, kMeshLocalPrefixLocator);
        if (rval != nullptr)
        {
            rval += sizeof(kMeshLocalPrefixLocator) - 1;
            *strstr(rval, "\r\n")               = '\0';
            networkInfo["IPv6:MeshLocalPrefix"] = rval;

            meshLocalPrefix = rval;
            meshLocalPrefix.resize(meshLocalPrefix.find(":/"));
        }

        VerifyOrExit((rval = client.Execute("ipaddr")) != nullptr, ret = kWpanStatus_GetPropertyFailed);

        for (rval = strtok(rval, "\r\n"); rval != nullptr; rval = strtok(nullptr, "\r\n"))
        {
            char *meshLocalAddressToken = nullptr;

            if (strstr(rval, "> ") != nullptr)
            {
                rval += 2;
            }

            if (strstr(rval, linkLocalAddressToken) == rval)
            {
                networkInfo["IPv6:LinkLocalAddress"] = rval;
                continue;
            }

            meshLocalAddressToken = strstr(rval, kMeshLocalAddressTokenLocator);

            if (meshLocalAddressToken == nullptr)
            {
                if ((meshLocalPrefix.size() > 0) && (strstr(rval, meshLocalPrefix.c_str()) == rval))
                {
                    networkInfo["IPv6:MeshLocalAddress"] = rval;
                    continue;
                }

                if (strstr(rval, localAddressToken) != rval)
                {
                    networkInfo["IPv6:GlobalAddress"] = rval;
                }
                else
                {
                    networkInfo["IPv6:LocalAddress"] = rval;
                }
            }
            else
            {
                *meshLocalAddressToken              = '\0';
                meshLocalPrefix                     = rval;
                networkInfo["IPv6:MeshLocalPrefix"] = rval;
                std::string la                      = networkInfo.get("IPv6:LocalAddress", "unknown").asString();
                if (strstr(rval, la.c_str()) != nullptr)
                {
                    networkInfo["IPv6:MeshLocalAddress"] = networkInfo.get("IPv6:LocalAddress", "notfound").asString();
                    networkInfo.removeMember("IPv6:LocalAddress");
                }
            }
        }
    }

exit:
    root["result"] = networkInfo;

    if (ret != kWpanStatus_Ok)
    {
        root["result"] = WPAN_RESPONSE_FAILURE;
        otbrLogErr("Wpan service error: %d", ret);
    }
    root["error"] = ret;
    response      = jsonWriter.write(root);
    return response;
}

std::string WpanService::HandleAvailableNetworkRequest()
{
    Json::Value                 root, networks, networkInfo;
    Json::FastWriter            jsonWriter;
    std::string                 response;
    int                         ret = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);

    VerifyOrExit(client.Connect(), ret = kWpanStatus_ScanFailed);
    VerifyOrExit((mNetworksCount = client.Scan(mNetworks, sizeof(mNetworks) / sizeof(mNetworks[0]))) > 0,
                 ret = kWpanStatus_NetworkNotFound);

    for (int i = 0; i < mNetworksCount; i++)
    {
        char panId[OT_PANID_LENGTH * 2 + 3], hardwareAddress[OT_HARDWARE_ADDRESS_LENGTH * 2 + 1];
        otbr::Utils::Bytes2Hex(mNetworks[i].mHardwareAddress, OT_HARDWARE_ADDRESS_LENGTH, hardwareAddress);
        sprintf(panId, "0x%X", mNetworks[i].mPanId);
        networkInfo[i]["pi"] = panId;
        networkInfo[i]["ch"] = mNetworks[i].mChannel;
        networkInfo[i]["ha"] = hardwareAddress;
    }

    root["result"] = networkInfo;

exit:
    if (ret != kWpanStatus_Ok)
    {
        root["result"] = WPAN_RESPONSE_FAILURE;
        otbrLogErr("Error is %d", ret);
    }
    root["error"] = ret;
    response      = jsonWriter.write(root);
    return response;
}

int WpanService::GetWpanServiceStatus(std::string &aNetworkName, std::string &aExtPanId) const
{
    int                         status = kWpanStatus_Ok;
    otbr::Web::OpenThreadClient client(mIfName);
    const char *                rval;

    VerifyOrExit(client.Connect(), status = kWpanStatus_Uninitialized);
    rval = client.Execute("state");
    VerifyOrExit(rval != nullptr, status = kWpanStatus_Down);
    if (!strcmp(rval, "disabled"))
    {
        status = kWpanStatus_Offline;
    }
    else if (!strcmp(rval, "detached"))
    {
        status = kWpanStatus_Associating;
    }
    else
    {
        rval = client.Execute("networkname");
        VerifyOrExit(rval != nullptr, status = kWpanStatus_Down);
        aNetworkName = rval;

        rval = client.Execute("extpanid");
        VerifyOrExit(rval != nullptr, status = kWpanStatus_Down);
        aExtPanId = rval;
    }

exit:

    return status;
}

std::string WpanService::HandleCommission(const std::string &aCommissionRequest)
{
    Json::Value      root;
    Json::Reader     reader;
    Json::FastWriter jsonWriter;
    int              ret = kWpanStatus_Ok;
    std::string      pskd;
    std::string      response;
    const char *     rval;

    VerifyOrExit(reader.parse(aCommissionRequest.c_str(), root) == true, ret = kWpanStatus_ParseRequestFailed);
    pskd = root["pskd"].asString();

    {
        otbr::Web::OpenThreadClient client(mIfName);

        VerifyOrExit(client.Connect(), ret = kWpanStatus_Uninitialized);

        for (int i = 0; i < 5; i++)
        {
            VerifyOrExit((rval = client.Execute("commissioner state")) != nullptr, ret = kWpanStatus_Down);

            if (strcmp(rval, "disabled") == 0)
            {
                VerifyOrExit((rval = client.Execute("commissioner start")) != nullptr, ret = kWpanStatus_Down);
            }
            else if (strcmp(rval, "active") == 0)
            {
                VerifyOrExit(client.Execute("commissioner joiner add * %s", pskd.c_str()) != nullptr,
                             ret = kWpanStatus_Down);
                root["error"] = ret;
                ExitNow();
            }

            sleep(1);
        }

        client.Execute("commissioner stop");
    }

    ret = kWpanStatus_SetFailed;

exit:

    root.clear();
    root["result"] = WPAN_RESPONSE_SUCCESS;
    root["error"]  = ret;

    if (ret != kWpanStatus_Ok)
    {
        root["result"] = WPAN_RESPONSE_FAILURE;
        otbrLogErr("error: %d", ret);
    }
    response = jsonWriter.write(root);

    return response;
}

int WpanService::joinActiveDataset(otbr::Web::OpenThreadClient &aClient,
                                   const std::string &          aNetworkKey,
                                   uint16_t                     aChannel,
                                   uint16_t                     aPanId)
{
    int ret = kWpanStatus_Ok;

    VerifyOrExit(aClient.Execute("dataset clear") != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset networkkey %s", aNetworkKey.c_str()) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset channel %u", aChannel) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset panid %u", aPanId) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset commit active") != nullptr, ret = kWpanStatus_SetFailed);

exit:
    return ret;
}

int WpanService::formActiveDataset(otbr::Web::OpenThreadClient &aClient,
                                   const std::string &          aNetworkKey,
                                   const std::string &          aNetworkName,
                                   const std::string &          aPskc,
                                   uint16_t                     aChannel,
                                   uint64_t                     aExtPanId,
                                   uint16_t                     aPanId)
{
    int ret = kWpanStatus_Ok;

    VerifyOrExit(aClient.Execute("dataset init new") != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset networkkey %s", aNetworkKey.c_str()) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset networkname %s", escapeOtCliEscapable(aNetworkName).c_str()) != nullptr,
                 ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset pskc %s", aPskc.c_str()) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset channel %u", aChannel) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset extpanid %016" PRIx64, aExtPanId) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset panid %u", aPanId) != nullptr, ret = kWpanStatus_SetFailed);
    VerifyOrExit(aClient.Execute("dataset commit active") != nullptr, ret = kWpanStatus_SetFailed);

exit:
    return ret;
}

std::string WpanService::escapeOtCliEscapable(const std::string &aArg)
{
    std::stringbuf strbuf;

    for (char c : aArg)
    {
        switch (c)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '\\':
            strbuf.sputc('\\');
            // Fallthrough
        default:
            strbuf.sputc(c);
        }
    }

    return strbuf.str();
}

} // namespace Web
} // namespace otbr
