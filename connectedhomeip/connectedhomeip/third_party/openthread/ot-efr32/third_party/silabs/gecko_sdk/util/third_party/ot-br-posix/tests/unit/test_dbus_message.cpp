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

#include <string.h>

#include "dbus/common/dbus_message_helper.hpp"

#include <CppUTest/TestHarness.h>

using std::array;
using std::string;
using std::tuple;
using std::vector;

using otbr::DBus::DBusMessageEncode;
using otbr::DBus::DBusMessageExtract;
using otbr::DBus::DBusMessageToTuple;
using otbr::DBus::TupleToDBusMessage;

struct TestStruct
{
    uint8_t     tag;
    uint32_t    val;
    std::string name;
};

namespace otbr {
namespace DBus {
template <> struct DBusTypeTrait<TestStruct>
{
    static constexpr const char *TYPE_AS_STRING =
        //{uint8, uint32, string}
        "(yus)";
};
} // namespace DBus
} // namespace otbr

bool operator==(const TestStruct &aLhs, const TestStruct &aRhs)
{
    return aLhs.tag == aRhs.tag && aLhs.val == aRhs.val && aLhs.name == aRhs.name;
}

bool operator==(const otbr::DBus::ChannelQuality &aLhs, const otbr::DBus::ChannelQuality &aRhs)
{
    return aLhs.mChannel == aRhs.mChannel && aLhs.mOccupancy == aRhs.mOccupancy;
}

bool operator==(const otbr::DBus::ChildInfo &aLhs, const otbr::DBus::ChildInfo &aRhs)
{
    return aLhs.mExtAddress == aRhs.mExtAddress && aLhs.mTimeout == aRhs.mTimeout && aLhs.mAge == aRhs.mAge &&
           aLhs.mRloc16 == aRhs.mRloc16 && aLhs.mChildId == aRhs.mChildId &&
           aLhs.mNetworkDataVersion == aRhs.mNetworkDataVersion && aLhs.mLinkQualityIn == aRhs.mLinkQualityIn &&
           aLhs.mAverageRssi == aRhs.mAverageRssi && aLhs.mLastRssi == aRhs.mLastRssi &&
           aLhs.mFrameErrorRate == aRhs.mFrameErrorRate && aLhs.mMessageErrorRate == aRhs.mMessageErrorRate &&
           aLhs.mRxOnWhenIdle == aRhs.mRxOnWhenIdle && aLhs.mFullThreadDevice == aRhs.mFullThreadDevice &&
           aLhs.mFullNetworkData == aRhs.mFullNetworkData && aLhs.mIsStateRestoring == aRhs.mIsStateRestoring;
}

bool operator==(const otbr::DBus::NeighborInfo &aLhs, const otbr::DBus::NeighborInfo &aRhs)
{
    return aLhs.mExtAddress == aRhs.mExtAddress && aLhs.mAge == aRhs.mAge && aLhs.mRloc16 == aRhs.mRloc16 &&
           aLhs.mLinkFrameCounter == aRhs.mLinkFrameCounter && aLhs.mMleFrameCounter == aRhs.mMleFrameCounter &&
           aLhs.mLinkQualityIn == aRhs.mLinkQualityIn && aLhs.mAverageRssi == aRhs.mAverageRssi &&
           aLhs.mLastRssi == aRhs.mLastRssi && aLhs.mFrameErrorRate == aRhs.mFrameErrorRate &&
           aLhs.mMessageErrorRate == aRhs.mMessageErrorRate && aLhs.mRxOnWhenIdle == aRhs.mRxOnWhenIdle &&
           aLhs.mFullThreadDevice == aRhs.mFullThreadDevice && aLhs.mFullNetworkData == aRhs.mFullNetworkData &&
           aLhs.mIsChild == aRhs.mIsChild;
}

bool operator==(const otbr::DBus::LeaderData &aLhs, const otbr::DBus::LeaderData &aRhs)
{
    return aLhs.mPartitionId == aRhs.mPartitionId && aLhs.mWeighting == aRhs.mWeighting &&
           aLhs.mDataVersion == aRhs.mDataVersion && aLhs.mStableDataVersion == aRhs.mStableDataVersion &&
           aLhs.mLeaderRouterId == aRhs.mLeaderRouterId;
}

bool operator==(const otbr::DBus::ActiveScanResult &aLhs, const otbr::DBus::ActiveScanResult &aRhs)
{
    return aLhs.mExtAddress == aRhs.mExtAddress && aLhs.mNetworkName == aRhs.mNetworkName &&
           aLhs.mExtendedPanId == aRhs.mExtendedPanId && aLhs.mSteeringData == aRhs.mSteeringData &&
           aLhs.mPanId == aRhs.mPanId && aLhs.mJoinerUdpPort == aRhs.mJoinerUdpPort && aLhs.mChannel == aRhs.mChannel &&
           aLhs.mRssi == aRhs.mRssi && aLhs.mLqi == aRhs.mLqi && aLhs.mVersion == aRhs.mVersion &&
           aLhs.mIsNative == aRhs.mIsNative;
}

bool operator==(const otbr::DBus::Ip6Prefix &aLhs, const otbr::DBus::Ip6Prefix &aRhs)
{
    bool prefixDataEquality = (aLhs.mPrefix.size() == aRhs.mPrefix.size()) &&
                              (memcmp(&aLhs.mPrefix[0], &aRhs.mPrefix[0], aLhs.mPrefix.size()) == 0);

    return prefixDataEquality && aLhs.mLength == aRhs.mLength;
}

bool operator==(const otbr::DBus::ExternalRoute &aLhs, const otbr::DBus::ExternalRoute &aRhs)
{
    return aLhs.mPrefix == aRhs.mPrefix && aLhs.mRloc16 == aRhs.mRloc16 && aLhs.mPreference == aRhs.mPreference &&
           aLhs.mStable == aRhs.mStable && aLhs.mNextHopIsThisDevice == aRhs.mNextHopIsThisDevice;
}

inline otbrError DBusMessageEncode(DBusMessageIter *aIter, const TestStruct &aValue)
{
    otbrError       error = OTBR_ERROR_DBUS;
    DBusMessageIter sub;
    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_STRUCT, nullptr, &sub), error = OTBR_ERROR_DBUS);

    SuccessOrExit(DBusMessageEncode(&sub, aValue.tag));
    SuccessOrExit(DBusMessageEncode(&sub, aValue.val));
    SuccessOrExit(DBusMessageEncode(&sub, aValue.name));
    VerifyOrExit(dbus_message_iter_close_container(aIter, &sub), error = OTBR_ERROR_DBUS);
    error = OTBR_ERROR_NONE;

exit:
    return error;
}

inline otbrError DBusMessageExtract(DBusMessageIter *aIter, TestStruct &aValue)
{
    otbrError       error = OTBR_ERROR_DBUS;
    DBusMessageIter sub;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_STRUCT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &sub);
    SuccessOrExit(DBusMessageExtract(&sub, aValue.tag));
    SuccessOrExit(DBusMessageExtract(&sub, aValue.val));
    SuccessOrExit(DBusMessageExtract(&sub, aValue.name));

    dbus_message_iter_next(aIter);
    error = OTBR_ERROR_NONE;

exit:
    return error;
}

TEST_GROUP(DBusMessage){};

TEST(DBusMessage, TestVectorMessage)
{
    DBusMessage *msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<vector<uint8_t>, vector<uint16_t>, vector<uint32_t>, vector<uint64_t>, vector<int16_t>, vector<int32_t>,
          vector<int64_t>>
        setVals({0, 1}, {2, 3}, {4, 5}, {6, 7, 8}, {}, {9, 10}, {11, 12});

    tuple<vector<uint8_t>, vector<uint16_t>, vector<uint32_t>, vector<uint64_t>, vector<int16_t>, vector<int32_t>,
          vector<int64_t>>
        getVals({}, {}, {}, {}, {}, {}, {});
    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(setVals == getVals);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestArrayMessage)
{
    DBusMessage *            msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<array<uint8_t, 4>> setVals({1, 2, 3, 4});
    tuple<array<uint8_t, 4>> getVals({0, 0, 0, 0});

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(setVals == getVals);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestNumberMessage)
{
    DBusMessage *msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<uint8_t, uint16_t, uint32_t, uint64_t, bool, int16_t, int32_t, int64_t> setVals =
        std::make_tuple<uint8_t, uint16_t, uint32_t, uint64_t, bool, int16_t, int32_t, int64_t>(1, 2, 3, 4, true, 5, 6,
                                                                                                7);
    tuple<uint8_t, uint16_t, uint32_t, uint64_t, bool, int16_t, int32_t, int64_t> getVals =
        std::make_tuple<uint8_t, uint16_t, uint32_t, uint64_t, bool, int16_t, int32_t, int64_t>(0, 0, 0, 0, false, 0, 0,
                                                                                                0);

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(setVals == getVals);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestStructMessage)
{
    DBusMessage *msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<uint8_t, vector<int32_t>, vector<string>, vector<TestStruct>> setVals(
        0x03, {0x04, 0x05}, {"hello", "world"}, {{1, 0xf0a, "test1"}, {2, 0xf0b, "test2"}});
    tuple<uint8_t, vector<int32_t>, vector<string>, vector<TestStruct>> getVals(0, {}, {}, {});

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(setVals == getVals);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestOtbrChannelQuality)
{
    DBusMessage *                                  msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<std::vector<otbr::DBus::ChannelQuality>> setVals({{1, 2}});
    tuple<std::vector<otbr::DBus::ChannelQuality>> getVals;

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(std::get<0>(setVals)[0] == std::get<0>(getVals)[0]);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestOtbrChildInfo)
{
    DBusMessage *                             msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<std::vector<otbr::DBus::ChildInfo>> setVals({{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, true, false, true, false}});
    tuple<std::vector<otbr::DBus::ChildInfo>> getVals;

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(std::get<0>(setVals)[0] == std::get<0>(getVals)[0]);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestOtbrNeighborInfo)
{
    DBusMessage *                                msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<std::vector<otbr::DBus::NeighborInfo>> setVals({{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, true, false, true, false}});
    tuple<std::vector<otbr::DBus::NeighborInfo>> getVals;

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(std::get<0>(setVals)[0] == std::get<0>(getVals)[0]);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestOtbrLeaderData)
{
    DBusMessage *                              msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<std::vector<otbr::DBus::LeaderData>> setVals({{1, 2, 3, 4, 5}});
    tuple<std::vector<otbr::DBus::LeaderData>> getVals;

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(std::get<0>(setVals)[0] == std::get<0>(getVals)[0]);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestOtbrActiveScanResults)
{
    DBusMessage *                                    msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<std::vector<otbr::DBus::ActiveScanResult>> setVals({{1, "a", 2, {3}, 4, 5, 6, 7, 8, 9, true, false}});
    tuple<std::vector<otbr::DBus::ActiveScanResult>> getVals;

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(std::get<0>(setVals)[0] == std::get<0>(getVals)[0]);

    dbus_message_unref(msg);
}

TEST(DBusMessage, TestOtbrExternalRoute)
{
    DBusMessage *                                 msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    tuple<std::vector<otbr::DBus::ExternalRoute>> setVals(
        {{otbr::DBus::Ip6Prefix({{0xfa, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06}, 64}), uint16_t(0xfc00), 1, true,
          true}});
    tuple<std::vector<otbr::DBus::ExternalRoute>> getVals;

    CHECK(msg != nullptr);

    CHECK(TupleToDBusMessage(*msg, setVals) == OTBR_ERROR_NONE);
    CHECK(DBusMessageToTuple(*msg, getVals) == OTBR_ERROR_NONE);

    CHECK(std::get<0>(setVals)[0] == std::get<0>(getVals)[0]);

    dbus_message_unref(msg);
}
