/*
 *  Copyright (c) 2020, The OpenThread Authors.
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

#include "rest/json.hpp"

#include "common/code_utils.hpp"
#include "common/types.hpp"

extern "C" {
#include <cJSON.h>
}

namespace otbr {
namespace rest {
namespace Json {

static cJSON *Bytes2HexJson(const uint8_t *aBytes, uint8_t aLength)
{
    char hex[2 * aLength + 1];

    otbr::Utils::Bytes2Hex(aBytes, aLength, hex);
    hex[2 * aLength] = '\0';

    return cJSON_CreateString(hex);
}

std::string String2JsonString(const std::string &aString)
{
    std::string ret;
    cJSON *     json    = nullptr;
    char *      jsonOut = nullptr;

    VerifyOrExit(aString.size() > 0);

    json    = cJSON_CreateString(aString.c_str());
    jsonOut = cJSON_Print(json);
    ret     = jsonOut;
    if (jsonOut != nullptr)
    {
        cJSON_free(jsonOut);
        jsonOut = nullptr;
    }

    cJSON_Delete(json);

exit:
    return ret;
}

std::string Json2String(const cJSON *aJson)
{
    std::string ret;
    char *      jsonOut = nullptr;

    VerifyOrExit(aJson != nullptr);

    jsonOut = cJSON_Print(aJson);
    ret     = jsonOut;
    if (jsonOut != nullptr)
    {
        cJSON_free(jsonOut);
        jsonOut = nullptr;
    }

exit:
    return ret;
}

static cJSON *CString2Json(const char *aString)
{
    return cJSON_CreateString(aString);
}

static cJSON *Mode2Json(const otLinkModeConfig &aMode)
{
    cJSON *mode = cJSON_CreateObject();

    cJSON_AddItemToObject(mode, "RxOnWhenIdle", cJSON_CreateNumber(aMode.mRxOnWhenIdle));
    cJSON_AddItemToObject(mode, "DeviceType", cJSON_CreateNumber(aMode.mDeviceType));
    cJSON_AddItemToObject(mode, "NetworkData", cJSON_CreateNumber(aMode.mNetworkData));

    return mode;
}

static cJSON *IpAddr2Json(const otIp6Address &aAddress)
{
    Ip6Address addr(aAddress.mFields.m8);

    return cJSON_CreateString(addr.ToString().c_str());
}

static cJSON *ChildTableEntry2Json(const otNetworkDiagChildEntry &aChildEntry)
{
    cJSON *childEntry = cJSON_CreateObject();

    cJSON_AddItemToObject(childEntry, "ChildId", cJSON_CreateNumber(aChildEntry.mChildId));
    cJSON_AddItemToObject(childEntry, "Timeout", cJSON_CreateNumber(aChildEntry.mTimeout));

    cJSON *mode = Mode2Json(aChildEntry.mMode);
    cJSON_AddItemToObject(childEntry, "Mode", mode);

    return childEntry;
}

static cJSON *MacCounters2Json(const otNetworkDiagMacCounters &aMacCounters)
{
    cJSON *macCounters = cJSON_CreateObject();

    cJSON_AddItemToObject(macCounters, "IfInUnknownProtos", cJSON_CreateNumber(aMacCounters.mIfInUnknownProtos));
    cJSON_AddItemToObject(macCounters, "IfInErrors", cJSON_CreateNumber(aMacCounters.mIfInErrors));
    cJSON_AddItemToObject(macCounters, "IfOutErrors", cJSON_CreateNumber(aMacCounters.mIfOutErrors));
    cJSON_AddItemToObject(macCounters, "IfInUcastPkts", cJSON_CreateNumber(aMacCounters.mIfInUcastPkts));
    cJSON_AddItemToObject(macCounters, "IfInBroadcastPkts", cJSON_CreateNumber(aMacCounters.mIfInBroadcastPkts));
    cJSON_AddItemToObject(macCounters, "IfInDiscards", cJSON_CreateNumber(aMacCounters.mIfInDiscards));
    cJSON_AddItemToObject(macCounters, "IfOutUcastPkts", cJSON_CreateNumber(aMacCounters.mIfOutUcastPkts));
    cJSON_AddItemToObject(macCounters, "IfOutBroadcastPkts", cJSON_CreateNumber(aMacCounters.mIfOutBroadcastPkts));
    cJSON_AddItemToObject(macCounters, "IfOutDiscards", cJSON_CreateNumber(aMacCounters.mIfOutDiscards));

    return macCounters;
}

static cJSON *Connectivity2Json(const otNetworkDiagConnectivity &aConnectivity)
{
    cJSON *connectivity = cJSON_CreateObject();

    cJSON_AddItemToObject(connectivity, "ParentPriority", cJSON_CreateNumber(aConnectivity.mParentPriority));
    cJSON_AddItemToObject(connectivity, "LinkQuality3", cJSON_CreateNumber(aConnectivity.mLinkQuality3));
    cJSON_AddItemToObject(connectivity, "LinkQuality2", cJSON_CreateNumber(aConnectivity.mLinkQuality2));
    cJSON_AddItemToObject(connectivity, "LinkQuality1", cJSON_CreateNumber(aConnectivity.mLinkQuality1));
    cJSON_AddItemToObject(connectivity, "LeaderCost", cJSON_CreateNumber(aConnectivity.mLeaderCost));
    cJSON_AddItemToObject(connectivity, "IdSequence", cJSON_CreateNumber(aConnectivity.mIdSequence));
    cJSON_AddItemToObject(connectivity, "ActiveRouters", cJSON_CreateNumber(aConnectivity.mActiveRouters));
    cJSON_AddItemToObject(connectivity, "SedBufferSize", cJSON_CreateNumber(aConnectivity.mSedBufferSize));
    cJSON_AddItemToObject(connectivity, "SedDatagramCount", cJSON_CreateNumber(aConnectivity.mSedDatagramCount));

    return connectivity;
}

static cJSON *RouteData2Json(const otNetworkDiagRouteData &aRouteData)
{
    cJSON *routeData = cJSON_CreateObject();

    cJSON_AddItemToObject(routeData, "RouteId", cJSON_CreateNumber(aRouteData.mRouterId));
    cJSON_AddItemToObject(routeData, "LinkQualityOut", cJSON_CreateNumber(aRouteData.mLinkQualityOut));
    cJSON_AddItemToObject(routeData, "LinkQualityIn", cJSON_CreateNumber(aRouteData.mLinkQualityIn));
    cJSON_AddItemToObject(routeData, "RouteCost", cJSON_CreateNumber(aRouteData.mRouteCost));

    return routeData;
}

static cJSON *Route2Json(const otNetworkDiagRoute &aRoute)
{
    cJSON *route = cJSON_CreateObject();

    cJSON_AddItemToObject(route, "IdSequence", cJSON_CreateNumber(aRoute.mIdSequence));

    cJSON *RouteData = cJSON_CreateArray();
    for (uint16_t i = 0; i < aRoute.mRouteCount; ++i)
    {
        cJSON *RouteDatavalue = RouteData2Json(aRoute.mRouteData[i]);
        cJSON_AddItemToArray(RouteData, RouteDatavalue);
    }

    cJSON_AddItemToObject(route, "RouteData", RouteData);

    return route;
}

static cJSON *LeaderData2Json(const otLeaderData &aLeaderData)
{
    cJSON *leaderData = cJSON_CreateObject();

    cJSON_AddItemToObject(leaderData, "PartitionId", cJSON_CreateNumber(aLeaderData.mPartitionId));
    cJSON_AddItemToObject(leaderData, "Weighting", cJSON_CreateNumber(aLeaderData.mWeighting));
    cJSON_AddItemToObject(leaderData, "DataVersion", cJSON_CreateNumber(aLeaderData.mDataVersion));
    cJSON_AddItemToObject(leaderData, "StableDataVersion", cJSON_CreateNumber(aLeaderData.mStableDataVersion));
    cJSON_AddItemToObject(leaderData, "LeaderRouterId", cJSON_CreateNumber(aLeaderData.mLeaderRouterId));

    return leaderData;
}

std::string IpAddr2JsonString(const otIp6Address &aAddress)
{
    std::string ret;
    cJSON *     ipAddr = IpAddr2Json(aAddress);

    ret = Json2String(ipAddr);
    cJSON_Delete(ipAddr);

    return ret;
}

std::string Node2JsonString(const NodeInfo &aNode)
{
    cJSON *     node = cJSON_CreateObject();
    std::string ret;

    cJSON_AddItemToObject(node, "State", cJSON_CreateNumber(aNode.mRole));
    cJSON_AddItemToObject(node, "NumOfRouter", cJSON_CreateNumber(aNode.mNumOfRouter));
    cJSON_AddItemToObject(node, "RlocAddress", IpAddr2Json(aNode.mRlocAddress));
    cJSON_AddItemToObject(node, "ExtAddress", Bytes2HexJson(aNode.mExtAddress, OT_EXT_ADDRESS_SIZE));
    cJSON_AddItemToObject(node, "NetworkName", cJSON_CreateString(aNode.mNetworkName.c_str()));
    cJSON_AddItemToObject(node, "Rloc16", cJSON_CreateNumber(aNode.mRloc16));
    cJSON_AddItemToObject(node, "LeaderData", LeaderData2Json(aNode.mLeaderData));
    cJSON_AddItemToObject(node, "ExtPanId", Bytes2HexJson(aNode.mExtPanId, OT_EXT_PAN_ID_SIZE));

    ret = Json2String(node);
    cJSON_Delete(node);

    return ret;
}

std::string Diag2JsonString(const std::vector<std::vector<otNetworkDiagTlv>> &aDiagSet)
{
    cJSON *     diagInfo          = cJSON_CreateArray();
    cJSON *     diagInfoOfOneNode = nullptr;
    cJSON *     addrList          = nullptr;
    cJSON *     tableList         = nullptr;
    std::string ret;
    uint64_t    timeout;

    for (auto diagItem : aDiagSet)
    {
        diagInfoOfOneNode = cJSON_CreateObject();
        for (auto diagTlv : diagItem)
        {
            switch (diagTlv.mType)
            {
            case OT_NETWORK_DIAGNOSTIC_TLV_EXT_ADDRESS:

                cJSON_AddItemToObject(diagInfoOfOneNode, "ExtAddress",
                                      Bytes2HexJson(diagTlv.mData.mExtAddress.m8, OT_EXT_ADDRESS_SIZE));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_SHORT_ADDRESS:

                cJSON_AddItemToObject(diagInfoOfOneNode, "Rloc16", cJSON_CreateNumber(diagTlv.mData.mAddr16));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_MODE:

                cJSON_AddItemToObject(diagInfoOfOneNode, "Mode", Mode2Json(diagTlv.mData.mMode));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_TIMEOUT:

                timeout = static_cast<uint64_t>(diagTlv.mData.mTimeout);
                cJSON_AddItemToObject(diagInfoOfOneNode, "Timeout", cJSON_CreateNumber(timeout));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_CONNECTIVITY:

                cJSON_AddItemToObject(diagInfoOfOneNode, "Connectivity",
                                      Connectivity2Json(diagTlv.mData.mConnectivity));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_ROUTE:

                cJSON_AddItemToObject(diagInfoOfOneNode, "Route", Route2Json(diagTlv.mData.mRoute));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_LEADER_DATA:

                cJSON_AddItemToObject(diagInfoOfOneNode, "LeaderData", LeaderData2Json(diagTlv.mData.mLeaderData));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_NETWORK_DATA:

                cJSON_AddItemToObject(diagInfoOfOneNode, "NetworkData",
                                      Bytes2HexJson(diagTlv.mData.mNetworkData.m8, diagTlv.mData.mNetworkData.mCount));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_IP6_ADDR_LIST:

                addrList = cJSON_CreateArray();

                for (uint16_t i = 0; i < diagTlv.mData.mIp6AddrList.mCount; ++i)
                {
                    cJSON_AddItemToArray(addrList, IpAddr2Json(diagTlv.mData.mIp6AddrList.mList[i]));
                }
                cJSON_AddItemToObject(diagInfoOfOneNode, "IP6AddressList", addrList);

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_MAC_COUNTERS:

                cJSON_AddItemToObject(diagInfoOfOneNode, "MACCounters", MacCounters2Json(diagTlv.mData.mMacCounters));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_BATTERY_LEVEL:

                cJSON_AddItemToObject(diagInfoOfOneNode, "BatteryLevel",
                                      cJSON_CreateNumber(diagTlv.mData.mBatteryLevel));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_SUPPLY_VOLTAGE:

                cJSON_AddItemToObject(diagInfoOfOneNode, "SupplyVoltage",
                                      cJSON_CreateNumber(diagTlv.mData.mSupplyVoltage));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_CHILD_TABLE:

                tableList = cJSON_CreateArray();

                for (uint16_t i = 0; i < diagTlv.mData.mChildTable.mCount; ++i)
                {
                    cJSON_AddItemToArray(tableList, ChildTableEntry2Json(diagTlv.mData.mChildTable.mTable[i]));
                }

                cJSON_AddItemToObject(diagInfoOfOneNode, "ChildTable", tableList);

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_CHANNEL_PAGES:

                cJSON_AddItemToObject(
                    diagInfoOfOneNode, "ChannelPages",
                    Bytes2HexJson(diagTlv.mData.mChannelPages.m8, diagTlv.mData.mChannelPages.mCount));

                break;
            case OT_NETWORK_DIAGNOSTIC_TLV_MAX_CHILD_TIMEOUT:

                cJSON_AddItemToObject(diagInfoOfOneNode, "MaxChildTimeout",
                                      cJSON_CreateNumber(diagTlv.mData.mMaxChildTimeout));

                break;
            default:
                break;
            }
        }
        cJSON_AddItemToArray(diagInfo, diagInfoOfOneNode);
    }

    ret = Json2String(diagInfo);

    cJSON_Delete(diagInfo);

    return ret;
}

std::string Bytes2HexJsonString(const uint8_t *aBytes, uint8_t aLength)
{
    cJSON *     hex = Bytes2HexJson(aBytes, aLength);
    std::string ret = Json2String(hex);

    cJSON_Delete(hex);

    return ret;
}

std::string Number2JsonString(const uint32_t &aNumber)
{
    cJSON *     number = cJSON_CreateNumber(aNumber);
    std::string ret    = Json2String(number);

    cJSON_Delete(number);

    return ret;
}

std::string Mode2JsonString(const otLinkModeConfig &aMode)
{
    cJSON *     mode = Mode2Json(aMode);
    std::string ret  = Json2String(mode);

    cJSON_Delete(mode);

    return ret;
}

std::string Connectivity2JsonString(const otNetworkDiagConnectivity &aConnectivity)
{
    cJSON *     connectivity = Connectivity2Json(aConnectivity);
    std::string ret          = Json2String(connectivity);

    cJSON_Delete(connectivity);

    return ret;
}

std::string RouteData2JsonString(const otNetworkDiagRouteData &aRouteData)
{
    cJSON *     routeData = RouteData2Json(aRouteData);
    std::string ret       = Json2String(routeData);

    cJSON_Delete(routeData);

    return ret;
}

std::string Route2JsonString(const otNetworkDiagRoute &aRoute)
{
    cJSON *     route = Route2Json(aRoute);
    std::string ret   = Json2String(route);

    cJSON_Delete(route);

    return ret;
}

std::string LeaderData2JsonString(const otLeaderData &aLeaderData)
{
    cJSON *     leaderData = LeaderData2Json(aLeaderData);
    std::string ret        = Json2String(leaderData);

    cJSON_Delete(leaderData);

    return ret;
}

std::string MacCounters2JsonString(const otNetworkDiagMacCounters &aMacCounters)
{
    cJSON *     macCounters = MacCounters2Json(aMacCounters);
    std::string ret         = Json2String(macCounters);

    cJSON_Delete(macCounters);

    return ret;
}

std::string ChildTableEntry2JsonString(const otNetworkDiagChildEntry &aChildEntry)
{
    cJSON *     childEntry = ChildTableEntry2Json(aChildEntry);
    std::string ret        = Json2String(childEntry);

    cJSON_Delete(childEntry);

    return ret;
}

std::string CString2JsonString(const char *aCString)
{
    cJSON *     cString = CString2Json(aCString);
    std::string ret     = Json2String(cString);

    cJSON_Delete(cString);

    return ret;
}

std::string Error2JsonString(HttpStatusCode aErrorCode, std::string aErrorMessage)
{
    std::string ret;
    cJSON *     error = cJSON_CreateObject();

    cJSON_AddItemToObject(error, "ErrorCode", cJSON_CreateNumber(static_cast<int16_t>(aErrorCode)));
    cJSON_AddItemToObject(error, "ErrorMessage", cJSON_CreateString(aErrorMessage.c_str()));

    ret = Json2String(error);

    cJSON_Delete(error);

    return ret;
}

} // namespace Json
} // namespace rest
} // namespace otbr
