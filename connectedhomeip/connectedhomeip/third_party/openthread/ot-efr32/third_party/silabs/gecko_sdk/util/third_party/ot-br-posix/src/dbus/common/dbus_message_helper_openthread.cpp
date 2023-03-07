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

#include <string.h>

#include "dbus/common/dbus_message_helper.hpp"

namespace otbr {
namespace DBus {

otbrError DBusMessageExtract(DBusMessageIter *aIter, otbrError &aError)
{
    uint8_t   val;
    otbrError error = DBusMessageExtract(aIter, val);

    VerifyOrExit(error == OTBR_ERROR_NONE);
    aError = static_cast<otbrError>(val);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const otbrError &aError)
{
    return DBusMessageEncode(aIter, static_cast<uint8_t>(aError));
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ActiveScanResult &aScanResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);

    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mExtAddress));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mNetworkName));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mExtendedPanId));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mSteeringData));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mPanId));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mJoinerUdpPort));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mChannel));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mRssi));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mLqi));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mVersion));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mIsNative));
    SuccessOrExit(error = DBusMessageExtract(&sub, aScanResult.mDiscover));

    dbus_message_iter_next(aIter);
    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ActiveScanResult &aScanResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mExtAddress));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mNetworkName));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mExtendedPanId));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mSteeringData));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mPanId));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mJoinerUdpPort));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mChannel));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mRssi));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mLqi));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mVersion));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mIsNative));
    SuccessOrExit(error = DBusMessageEncode(&sub, aScanResult.mDiscover));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, EnergyScanResult &aResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);

    SuccessOrExit(error = DBusMessageExtract(&sub, aResult.mChannel));
    SuccessOrExit(error = DBusMessageExtract(&sub, aResult.mMaxRssi));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const EnergyScanResult &aResult)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aResult.mChannel));
    SuccessOrExit(error = DBusMessageEncode(&sub, aResult.mMaxRssi));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const LinkModeConfig &aConfig)
{
    otbrError       error = OTBR_ERROR_NONE;
    DBusMessageIter sub;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aConfig.mRxOnWhenIdle));
    SuccessOrExit(error = DBusMessageEncode(&sub, aConfig.mDeviceType));
    SuccessOrExit(error = DBusMessageEncode(&sub, aConfig.mNetworkData));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, LinkModeConfig &aConfig)
{
    otbrError       error = OTBR_ERROR_DBUS;
    DBusMessageIter sub;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT);
    dbus_message_iter_recurse(aIter, &sub);

    SuccessOrExit(DBusMessageExtract(&sub, aConfig.mRxOnWhenIdle));
    SuccessOrExit(DBusMessageExtract(&sub, aConfig.mDeviceType));
    SuccessOrExit(DBusMessageExtract(&sub, aConfig.mNetworkData));

    dbus_message_iter_next(aIter);
    error = OTBR_ERROR_NONE;
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const Ip6Prefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    VerifyOrExit(aPrefix.mPrefix.size() <= OTBR_IP6_PREFIX_SIZE, error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPrefix));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mLength));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, Ip6Prefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPrefix));
    VerifyOrExit(aPrefix.mPrefix.size() <= OTBR_IP6_PREFIX_SIZE, error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mLength));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ExternalRoute &aRoute)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mPrefix));
    SuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mRloc16));
    SuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mPreference));
    SuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mStable));
    SuccessOrExit(error = DBusMessageEncode(&sub, aRoute.mNextHopIsThisDevice));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ExternalRoute &aRoute)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mPrefix));
    SuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mRloc16));
    SuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mPreference));
    SuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mStable));
    SuccessOrExit(error = DBusMessageExtract(&sub, aRoute.mNextHopIsThisDevice));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const OnMeshPrefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPrefix));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mRloc16));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPreference));

    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mPreferred));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mSlaac));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mDhcp));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mConfigure));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mDefaultRoute));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mOnMesh));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mStable));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mNdDns));
    SuccessOrExit(error = DBusMessageEncode(&sub, aPrefix.mDp));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, OnMeshPrefix &aPrefix)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPrefix));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mRloc16));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPreference));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mPreferred));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mSlaac));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mDhcp));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mConfigure));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mDefaultRoute));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mOnMesh));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mStable));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mNdDns));
    SuccessOrExit(error = DBusMessageExtract(&sub, aPrefix.mDp));

    dbus_message_iter_next(aIter);

exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const MacCounters &aCounters)
{
    auto args = std::tie(aCounters.mTxTotal, aCounters.mTxUnicast, aCounters.mTxBroadcast, aCounters.mTxAckRequested,
                         aCounters.mTxAcked, aCounters.mTxNoAckRequested, aCounters.mTxData, aCounters.mTxDataPoll,
                         aCounters.mTxBeacon, aCounters.mTxBeaconRequest, aCounters.mTxOther, aCounters.mTxRetry,
                         aCounters.mTxErrCca, aCounters.mTxErrAbort, aCounters.mTxErrBusyChannel, aCounters.mRxTotal,
                         aCounters.mRxUnicast, aCounters.mRxBroadcast, aCounters.mRxData, aCounters.mRxDataPoll,
                         aCounters.mRxBeacon, aCounters.mRxBeaconRequest, aCounters.mRxOther,
                         aCounters.mRxAddressFiltered, aCounters.mRxDestAddrFiltered, aCounters.mRxDuplicated,
                         aCounters.mRxErrNoFrame, aCounters.mRxErrUnknownNeighbor, aCounters.mRxErrInvalidSrcAddr,
                         aCounters.mRxErrSec, aCounters.mRxErrFcs, aCounters.mRxErrOther);
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, MacCounters &aCounters)
{
    auto args = std::tie(aCounters.mTxTotal, aCounters.mTxUnicast, aCounters.mTxBroadcast, aCounters.mTxAckRequested,
                         aCounters.mTxAcked, aCounters.mTxNoAckRequested, aCounters.mTxData, aCounters.mTxDataPoll,
                         aCounters.mTxBeacon, aCounters.mTxBeaconRequest, aCounters.mTxOther, aCounters.mTxRetry,
                         aCounters.mTxErrCca, aCounters.mTxErrAbort, aCounters.mTxErrBusyChannel, aCounters.mRxTotal,
                         aCounters.mRxUnicast, aCounters.mRxBroadcast, aCounters.mRxData, aCounters.mRxDataPoll,
                         aCounters.mRxBeacon, aCounters.mRxBeaconRequest, aCounters.mRxOther,
                         aCounters.mRxAddressFiltered, aCounters.mRxDestAddrFiltered, aCounters.mRxDuplicated,
                         aCounters.mRxErrNoFrame, aCounters.mRxErrUnknownNeighbor, aCounters.mRxErrInvalidSrcAddr,
                         aCounters.mRxErrSec, aCounters.mRxErrFcs, aCounters.mRxErrOther);
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const IpCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aCounters.mTxSuccess, aCounters.mRxSuccess, aCounters.mTxFailure, aCounters.mRxFailure);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, IpCounters &aCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aCounters.mTxSuccess, aCounters.mRxSuccess, aCounters.mTxFailure, aCounters.mRxFailure);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ChildInfo &aChildInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aChildInfo.mExtAddress, aChildInfo.mTimeout, aChildInfo.mAge, aChildInfo.mRloc16,
                         aChildInfo.mChildId, aChildInfo.mNetworkDataVersion, aChildInfo.mLinkQualityIn,
                         aChildInfo.mAverageRssi, aChildInfo.mLastRssi, aChildInfo.mFrameErrorRate,
                         aChildInfo.mMessageErrorRate, aChildInfo.mRxOnWhenIdle, aChildInfo.mFullThreadDevice,
                         aChildInfo.mFullNetworkData, aChildInfo.mIsStateRestoring);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ChildInfo &aChildInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aChildInfo.mExtAddress, aChildInfo.mTimeout, aChildInfo.mAge, aChildInfo.mRloc16,
                         aChildInfo.mChildId, aChildInfo.mNetworkDataVersion, aChildInfo.mLinkQualityIn,
                         aChildInfo.mAverageRssi, aChildInfo.mLastRssi, aChildInfo.mFrameErrorRate,
                         aChildInfo.mMessageErrorRate, aChildInfo.mRxOnWhenIdle, aChildInfo.mFullThreadDevice,
                         aChildInfo.mFullNetworkData, aChildInfo.mIsStateRestoring);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const NeighborInfo &aNeighborInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aNeighborInfo.mExtAddress, aNeighborInfo.mAge, aNeighborInfo.mRloc16,
                         aNeighborInfo.mLinkFrameCounter, aNeighborInfo.mMleFrameCounter, aNeighborInfo.mLinkQualityIn,
                         aNeighborInfo.mAverageRssi, aNeighborInfo.mLastRssi, aNeighborInfo.mFrameErrorRate,
                         aNeighborInfo.mMessageErrorRate, aNeighborInfo.mRxOnWhenIdle, aNeighborInfo.mFullThreadDevice,
                         aNeighborInfo.mFullNetworkData, aNeighborInfo.mIsChild);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, NeighborInfo &aNeighborInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aNeighborInfo.mExtAddress, aNeighborInfo.mAge, aNeighborInfo.mRloc16,
                         aNeighborInfo.mLinkFrameCounter, aNeighborInfo.mMleFrameCounter, aNeighborInfo.mLinkQualityIn,
                         aNeighborInfo.mAverageRssi, aNeighborInfo.mLastRssi, aNeighborInfo.mFrameErrorRate,
                         aNeighborInfo.mMessageErrorRate, aNeighborInfo.mRxOnWhenIdle, aNeighborInfo.mFullThreadDevice,
                         aNeighborInfo.mFullNetworkData, aNeighborInfo.mIsChild);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const LeaderData &aLeaderData)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aLeaderData.mPartitionId, aLeaderData.mWeighting, aLeaderData.mDataVersion,
                         aLeaderData.mStableDataVersion, aLeaderData.mLeaderRouterId);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, LeaderData &aLeaderData)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aLeaderData.mPartitionId, aLeaderData.mWeighting, aLeaderData.mDataVersion,
                         aLeaderData.mStableDataVersion, aLeaderData.mLeaderRouterId);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const ChannelQuality &aQuality)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aQuality.mChannel, aQuality.mOccupancy);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub));
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, ChannelQuality &aQuality)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aQuality.mChannel, aQuality.mOccupancy);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const TxtEntry &aTxtEntry)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aTxtEntry.mKey, aTxtEntry.mValue);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub));
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, TxtEntry &aTxtEntry)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto            args  = std::tie(aTxtEntry.mKey, aTxtEntry.mValue);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo::Registration &aRegistration)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aRegistration.mFreshCount, aRegistration.mDeletedCount, aRegistration.mLeaseTimeTotal,
                         aRegistration.mKeyLeaseTimeTotal, aRegistration.mRemainingLeaseTimeTotal,
                         aRegistration.mRemainingKeyLeaseTimeTotal);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo::Registration &aRegistration)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aRegistration.mFreshCount, aRegistration.mDeletedCount, aRegistration.mLeaseTimeTotal,
                         aRegistration.mKeyLeaseTimeTotal, aRegistration.mRemainingLeaseTimeTotal,
                         aRegistration.mRemainingKeyLeaseTimeTotal);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo::ResponseCounters &aResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aResponseCounters.mSuccess, aResponseCounters.mServerFailure, aResponseCounters.mFormatError,
                         aResponseCounters.mNameExists, aResponseCounters.mRefused, aResponseCounters.mOther);

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);
    SuccessOrExit(error = ConvertToDBusMessage(&sub, args));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub) == true, error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo::ResponseCounters &aResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;
    auto args = std::tie(aResponseCounters.mSuccess, aResponseCounters.mServerFailure, aResponseCounters.mFormatError,
                         aResponseCounters.mNameExists, aResponseCounters.mRefused, aResponseCounters.mOther);

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = ConvertToTuple(&sub, args));
    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo &aSrpServerInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mState));
    SuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mPort));
    SuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mAddressMode));
    SuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mHosts));
    SuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mServices));
    SuccessOrExit(error = DBusMessageEncode(&sub, aSrpServerInfo.mResponseCounters));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo &aSrpServerInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mState));
    SuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mPort));
    SuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mAddressMode));
    SuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mHosts));
    SuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mServices));
    SuccessOrExit(error = DBusMessageExtract(&sub, aSrpServerInfo.mResponseCounters));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const DnssdCounters &aDnssdCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mSuccessResponse));
    SuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mServerFailureResponse));
    SuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mFormatErrorResponse));
    SuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mNameErrorResponse));
    SuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mNotImplementedResponse));
    SuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mOtherResponse));

    SuccessOrExit(error = DBusMessageEncode(&sub, aDnssdCounters.mResolvedBySrp));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, DnssdCounters &aDnssdCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    SuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mSuccessResponse));
    SuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mServerFailureResponse));
    SuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mFormatErrorResponse));
    SuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mNameErrorResponse));
    SuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mNotImplementedResponse));
    SuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mOtherResponse));

    SuccessOrExit(error = DBusMessageExtract(&sub, aDnssdCounters.mResolvedBySrp));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const MdnsResponseCounters &aMdnsResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mSuccess));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mNotFound));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mInvalidArgs));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mDuplicated));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mNotImplemented));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsResponseCounters.mUnknownError));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, MdnsResponseCounters &aMdnsResponseCounters)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mSuccess));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mNotFound));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mInvalidArgs));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mDuplicated));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mNotImplemented));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsResponseCounters.mUnknownError));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

otbrError DBusMessageEncode(DBusMessageIter *aIter, const MdnsTelemetryInfo &aMdnsTelemetryInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostRegistrations));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceRegistrations));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostResolutions));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceResolutions));

    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostRegistrationEmaLatency));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceRegistrationEmaLatency));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mHostResolutionEmaLatency));
    SuccessOrExit(error = DBusMessageEncode(&sub, aMdnsTelemetryInfo.mServiceResolutionEmaLatency));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

otbrError DBusMessageExtract(DBusMessageIter *aIter, MdnsTelemetryInfo &aMdnsTelemetryInfo)
{
    DBusMessageIter sub;
    otbrError       error = OTBR_ERROR_NONE;

    dbus_message_iter_recurse(aIter, &sub);

    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostRegistrations));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceRegistrations));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostResolutions));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceResolutions));

    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostRegistrationEmaLatency));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceRegistrationEmaLatency));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mHostResolutionEmaLatency));
    SuccessOrExit(error = DBusMessageExtract(&sub, aMdnsTelemetryInfo.mServiceResolutionEmaLatency));

    dbus_message_iter_next(aIter);
exit:
    return error;
}

} // namespace DBus
} // namespace otbr
