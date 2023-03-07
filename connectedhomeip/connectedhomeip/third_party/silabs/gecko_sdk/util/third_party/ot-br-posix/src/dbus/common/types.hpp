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
 * This file includes definitions for types used by d-bus API.
 */

#ifndef OTBR_DBUS_COMMON_TYPES_HPP_
#define OTBR_DBUS_COMMON_TYPES_HPP_

#include "dbus/common/error.hpp"

#include <stdint.h>

#include <string>
#include <vector>

namespace otbr {
namespace DBus {

enum DeviceRole
{
    OTBR_DEVICE_ROLE_DISABLED = 0,
    OTBR_DEVICE_ROLE_DETACHED = 1,
    OTBR_DEVICE_ROLE_CHILD    = 2,
    OTBR_DEVICE_ROLE_ROUTER   = 3,
    OTBR_DEVICE_ROLE_LEADER   = 4,
};

struct ActiveScanResult
{
    uint64_t             mExtAddress;    ///< IEEE 802.15.4 Extended Address
    std::string          mNetworkName;   ///< Thread Network Name
    uint64_t             mExtendedPanId; ///< Thread Extended PAN ID
    std::vector<uint8_t> mSteeringData;  ///< Steering Data
    uint16_t             mPanId;         ///< IEEE 802.15.4 PAN ID
    uint16_t             mJoinerUdpPort; ///< Joiner UDP Port
    uint8_t              mChannel;       ///< IEEE 802.15.4 Channel
    int8_t               mRssi;          ///< RSSI (dBm)
    uint8_t              mLqi;           ///< LQI
    uint8_t              mVersion;       ///< Version
    bool                 mIsNative;      ///< Native Commissioner flag
    bool                 mDiscover;      ///< Result from MLE Discovery
};

struct EnergyScanResult
{
    uint8_t mChannel; ///< IEEE 802.15.4 Channel
    int8_t  mMaxRssi; ///< The max RSSI (dBm)
};

struct LinkModeConfig
{
    bool mRxOnWhenIdle; ///< 1, if the sender has its receiver on when not transmitting. 0, otherwise.
    bool mDeviceType;   ///< 1, if the sender is an FTD. 0, otherwise.
    bool mNetworkData;  ///< 1, if the sender requires the full Network Data. 0, otherwise.
};

struct Ip6Prefix
{
    std::vector<uint8_t> mPrefix; ///< The IPv6 prefix.

    uint8_t mLength; ///< The IPv6 prefix length.
};

struct OnMeshPrefix
{
    /**
     * The IPv6 prefix.
     */
    Ip6Prefix mPrefix;

    /**
     * The Rloc associated with the Border Router prefix.
     */
    uint16_t mRloc16;

    /**
     * A 2-bit signed integer indicating router preference as defined in RFC 4191.
     */
    int8_t mPreference;

    /**
     * TRUE, if @p mPrefix is preferred.  FALSE, otherwise.
     */
    bool mPreferred;

    /**
     * TRUE, if @p mPrefix should be used for address autoconfiguration.  FALSE, otherwise.
     */
    bool mSlaac;

    /**
     * TRUE, if this border router is a DHCPv6 Agent that supplies IPv6 address configuration.  FALSE, otherwise.
     */
    bool mDhcp;

    /**
     * TRUE, if this border router is a DHCPv6 Agent that supplies other configuration data.  FALSE, otherwise.
     */
    bool mConfigure;

    /**
     * TRUE, if this border router is a default route for @p mPrefix.  FALSE, otherwise.
     */
    bool mDefaultRoute;

    /**
     * TRUE if this prefix is considered on-mesh. FALSE otherwise.
     */
    bool mOnMesh;

    /**
     * TRUE if this configuration is considered Stable Network Data. FALSE otherwise.
     */
    bool mStable;

    /**
     * TRUE if this border router can supply DNS information via ND. FALSE otherwise.
     */
    bool mNdDns;

    /**
     * TRUE if this prefix is a Thread Domain Prefix. FALSE otherwise.
     */
    bool mDp;
};

struct ExternalRoute
{
    /**
     * The IPv6 prefix.
     */
    Ip6Prefix mPrefix;

    /**
     * The Rloc associated with the external route entry.
     *
     * This value is ignored when adding an external route. For any added route, the device's Rloc is used.
     */
    uint16_t mRloc16;

    /**
     * A 2-bit signed integer indicating router preference as defined in RFC 4191.
     */
    int8_t mPreference;

    /**
     * TRUE, if this configuration is considered Stable Network Data.  FALSE, otherwise.
     */
    bool mStable;

    /**
     * TRUE if the external route entry's next hop is this device itself (i.e., the route was added earlier by this
     * device). FALSE otherwise.
     *
     * This value is ignored when adding an external route. For any added route the next hop is this device.
     */
    bool mNextHopIsThisDevice;
};

/**
 * This structure represents the MAC layer counters.
 *
 */
struct MacCounters
{
    /**
     * The total number of unique MAC frame transmission requests.
     *
     * Note that this counter is incremented for each MAC transmission request only by one,
     * regardless of the amount of CCA failures, CSMA-CA attempts, or retransmissions.
     *
     * This incrementation rule applies to the following counters:
     *   @p mTxUnicast
     *   @p mTxBroadcast
     *   @p mTxAckRequested
     *   @p mTxNoAckRequested
     *   @p mTxData
     *   @p mTxDataPoll
     *   @p mTxBeacon
     *   @p mTxBeaconRequest
     *   @p mTxOther
     *   @p mTxErrAbort
     *   @p mTxErrBusyChannel
     *
     * The following equations are valid:
     *     @p mTxTotal = @p mTxUnicast + @p mTxBroadcast
     *     @p mTxTotal = @p mTxAckRequested + @p mTxNoAckRequested
     *     @p mTxTotal = @p mTxData + @p mTxDataPoll + @p mTxBeacon + @p mTxBeaconRequest + @p mTxOther
     *
     */
    uint32_t mTxTotal;

    /**
     * The total number of unique unicast MAC frame transmission requests.
     *
     */
    uint32_t mTxUnicast;

    /**
     * The total number of unique broadcast MAC frame transmission requests.
     *
     */
    uint32_t mTxBroadcast;

    /**
     * The total number of unique MAC frame transmission requests with requested acknowledgment.
     *
     */
    uint32_t mTxAckRequested;

    /**
     * The total number of unique MAC frame transmission requests that were acked.
     *
     */
    uint32_t mTxAcked;

    /**
     * The total number of unique MAC frame transmission requests without requested acknowledgment.
     *
     */
    uint32_t mTxNoAckRequested;

    /**
     * The total number of unique MAC Data frame transmission requests.
     *
     */
    uint32_t mTxData;

    /**
     * The total number of unique MAC Data Poll frame transmission requests.
     *
     */
    uint32_t mTxDataPoll;

    /**
     * The total number of unique MAC Beacon frame transmission requests.
     *
     */
    uint32_t mTxBeacon;

    /**
     * The total number of unique MAC Beacon Request frame transmission requests.
     *
     */
    uint32_t mTxBeaconRequest;

    /**
     * The total number of unique other MAC frame transmission requests.
     *
     * This counter is currently unused.
     *
     */
    uint32_t mTxOther;

    /**
     * The total number of MAC retransmission attempts.
     *
     * Note that this counter is incremented by one for each retransmission attempt that may be
     * triggered by lack of acknowledgement, CSMA/CA failure, or other type of transmission error.
     * The @p mTxRetry counter is incremented both for unicast and broadcast MAC frames.
     *
     * Check the following configuration parameters to control the amount of retransmissions in the system:
     *   @sa OPENTHREAD_CONFIG_MAC_DEFAULT_MAX_FRAME_RETRIES_DIRECT
     *   @sa OPENTHREAD_CONFIG_MAC_DEFAULT_MAX_FRAME_RETRIES_INDIRECT
     *   @sa OPENTHREAD_CONFIG_MAC_TX_NUM_BCAST
     *   @sa OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT
     *   @sa OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_INDIRECT
     *
     * Currently, this counter is invalid if the platform's radio driver capability includes
     * @sa OT_RADIO_CAPS_TRANSMIT_RETRIES.
     *
     */
    uint32_t mTxRetry;

    /**
     * The total number of unique MAC transmission packets that meet maximal retry limit for direct packets.
     *
     */
    uint32_t mTxDirectMaxRetryExpiry;

    /**
     * The total number of unique MAC transmission packets that meet maximal retry limit for indirect packets.
     *
     */
    uint32_t mTxIndirectMaxRetryExpiry;

    /**
     * The total number of CCA failures.
     *
     * The meaning of this counter can be different and it depends on the platform's radio driver capabilities.
     *
     * If @sa OT_RADIO_CAPS_CSMA_BACKOFF is enabled, this counter represents the total number of full CSMA/CA
     * failed attempts and it is incremented by one also for each retransmission (in case of a CSMA/CA fail).
     *
     * If @sa OT_RADIO_CAPS_TRANSMIT_RETRIES is enabled, this counter represents the total number of full CSMA/CA
     * failed attempts and it is incremented by one for each individual data frame request (regardless of the amount of
     * retransmissions).
     *
     */
    uint32_t mTxErrCca;

    /**
     * The total number of unique MAC transmission request failures cause by an abort error.
     *
     */
    uint32_t mTxErrAbort;

    /**
     * The total number of unique MAC transmission requests failures caused by a busy channel (a CSMA/CA fail).
     *
     */
    uint32_t mTxErrBusyChannel;

    /**
     * The total number of received frames.
     *
     * This counter counts all frames reported by the platform's radio driver, including frames
     * that were dropped, for example because of an FCS error.
     *
     */
    uint32_t mRxTotal;

    /**
     * The total number of unicast frames received.
     *
     */
    uint32_t mRxUnicast;

    /**
     * The total number of broadcast frames received.
     *
     */
    uint32_t mRxBroadcast;

    /**
     * The total number of MAC Data frames received.
     *
     */
    uint32_t mRxData;

    /**
     * The total number of MAC Data Poll frames received.
     *
     */
    uint32_t mRxDataPoll;

    /**
     * The total number of MAC Beacon frames received.
     *
     */
    uint32_t mRxBeacon;

    /**
     * The total number of MAC Beacon Request frames received.
     *
     */
    uint32_t mRxBeaconRequest;

    /**
     * The total number of other types of frames received.
     *
     */
    uint32_t mRxOther;

    /**
     * The total number of frames dropped by MAC Filter module, for example received from denylisted node.
     *
     */
    uint32_t mRxAddressFiltered;

    /**
     * The total number of frames dropped by destination address check, for example received frame for other node.
     *
     */
    uint32_t mRxDestAddrFiltered;

    /**
     * The total number of frames dropped due to duplication, that is when the frame has been already received.
     *
     * This counter may be incremented, for example when ACK frame generated by the receiver hasn't reached
     * transmitter node which performed retransmission.
     *
     */
    uint32_t mRxDuplicated;

    /**
     * The total number of frames dropped because of missing or malformed content.
     *
     */
    uint32_t mRxErrNoFrame;

    /**
     * The total number of frames dropped due to unknown neighbor.
     *
     */
    uint32_t mRxErrUnknownNeighbor;

    /**
     * The total number of frames dropped due to invalid source address.
     *
     */
    uint32_t mRxErrInvalidSrcAddr;

    /**
     * The total number of frames dropped due to security error.
     *
     * This counter may be incremented, for example when lower than expected Frame Counter is used
     * to encrypt the frame.
     *
     */
    uint32_t mRxErrSec;

    /**
     * The total number of frames dropped due to invalid FCS.
     *
     */
    uint32_t mRxErrFcs;

    /**
     * The total number of frames dropped due to other error.
     *
     */
    uint32_t mRxErrOther;
};

struct IpCounters
{
    uint32_t mTxSuccess; ///< The number of IPv6 packets successfully transmitted.
    uint32_t mRxSuccess; ///< The number of IPv6 packets successfully received.
    uint32_t mTxFailure; ///< The number of IPv6 packets failed to transmit.
    uint32_t mRxFailure; ///< The number of IPv6 packets failed to receive.
};

struct ChannelQuality
{
    uint8_t  mChannel;
    uint16_t mOccupancy;
};

struct ChildInfo
{
    uint64_t mExtAddress;         ///< IEEE 802.15.4 Extended Address
    uint32_t mTimeout;            ///< Timeout
    uint32_t mAge;                ///< Time last heard
    uint16_t mRloc16;             ///< RLOC16
    uint16_t mChildId;            ///< Child ID
    uint8_t  mNetworkDataVersion; ///< Network Data Version
    uint8_t  mLinkQualityIn;      ///< Link Quality In
    int8_t   mAverageRssi;        ///< Average RSSI
    int8_t   mLastRssi;           ///< Last observed RSSI
    uint16_t mFrameErrorRate;     ///< Frame error rate (0xffff->100%). Requires error tracking feature.
    uint16_t mMessageErrorRate;   ///< (IPv6) msg error rate (0xffff->100%). Requires error tracking feature.
    bool     mRxOnWhenIdle;       ///< rx-on-when-idle
    bool     mFullThreadDevice;   ///< Full Thread Device
    bool     mFullNetworkData;    ///< Full Network Data
    bool     mIsStateRestoring;   ///< Is in restoring state
};

struct NeighborInfo
{
    uint64_t mExtAddress;       ///< IEEE 802.15.4 Extended Address
    uint32_t mAge;              ///< Time last heard
    uint16_t mRloc16;           ///< RLOC16
    uint32_t mLinkFrameCounter; ///< Link Frame Counter
    uint32_t mMleFrameCounter;  ///< MLE Frame Counter
    uint8_t  mLinkQualityIn;    ///< Link Quality In
    int8_t   mAverageRssi;      ///< Average RSSI
    int8_t   mLastRssi;         ///< Last observed RSSI
    uint16_t mFrameErrorRate;   ///< Frame error rate (0xffff->100%). Requires error tracking feature.
    uint16_t mMessageErrorRate; ///< (IPv6) msg error rate (0xffff->100%). Requires error tracking feature.
    bool     mRxOnWhenIdle;     ///< rx-on-when-idle
    bool     mFullThreadDevice; ///< Full Thread Device
    bool     mFullNetworkData;  ///< Full Network Data
    bool     mIsChild;          ///< Is the neighbor a child
};

struct LeaderData
{
    uint32_t mPartitionId;       ///< Partition ID
    uint8_t  mWeighting;         ///< Leader Weight
    uint8_t  mDataVersion;       ///< Full Network Data Version
    uint8_t  mStableDataVersion; ///< Stable Network Data Version
    uint8_t  mLeaderRouterId;    ///< Leader Router ID
};

struct TxtEntry
{
    std::string          mKey;
    std::vector<uint8_t> mValue;
};

enum SrpServerState : uint8_t
{
    OTBR_SRP_SERVER_STATE_DISABLED = 0, ///< The SRP server is disabled.
    OTBR_SRP_SERVER_STATE_RUNNING  = 1, ///< The SRP server is running.
    OTBR_SRP_SERVER_STATE_STOPPED  = 2, ///< The SRP server is stopped.
};

enum SrpServerAddressMode : uint8_t
{
    OTBR_SRP_SERVER_ADDRESS_MODE_UNICAST = 0, ///< Unicast address mode.
    OTBR_SRP_SERVER_ADDRESS_MODE_ANYCAST = 1, ///< Anycast address mode.
};

struct SrpServerInfo
{
    struct Registration
    {
        uint32_t mFreshCount;        ///< The number of active hosts/services registered on the SRP server
        uint32_t mDeletedCount;      ///< The number of hosts/services in 'Deleted' state on the SRP server
        uint64_t mLeaseTimeTotal;    ///< The sum of lease time in milliseconds of all active hosts/services
                                     ///< on the SRP server
        uint64_t mKeyLeaseTimeTotal; ///< The sum of key lease time in milliseconds of all active hosts/services on the
                                     ///< SRP server
        uint64_t mRemainingLeaseTimeTotal;    ///< The sum of remaining lease time in milliseconds of all active
                                              ///< hosts/services on the SRP server
        uint64_t mRemainingKeyLeaseTimeTotal; ///< The sum of remaining key lease time in milliseconds of all active
                                              ///< hosts/services on the SRP server
    };

    struct ResponseCounters
    {
        uint32_t mSuccess;       ///< The number of successful responses
        uint32_t mServerFailure; ///< The number of server failure responses
        uint32_t mFormatError;   ///< The number of format error responses
        uint32_t mNameExists;    ///< The number of 'name exists' responses
        uint32_t mRefused;       ///< The number of refused responses
        uint32_t mOther;         ///< The number of other responses
    };

    SrpServerState       mState;            ///< The state of the SRP server
    uint16_t             mPort;             ///< The listening port number
    SrpServerAddressMode mAddressMode;      ///< The address mode {unicast, anycast} of the SRP server
    Registration         mHosts;            ///< The registration information of hosts on the SRP server
    Registration         mServices;         ///< The registration information of services on the SRP server
    ResponseCounters     mResponseCounters; ///< The counters of response codes sent by the SRP server
};

struct DnssdCounters
{
    uint32_t mSuccessResponse;        ///< The number of successful responses
    uint32_t mServerFailureResponse;  ///< The number of server failure responses
    uint32_t mFormatErrorResponse;    ///< The number of format error responses
    uint32_t mNameErrorResponse;      ///< The number of name error responses
    uint32_t mNotImplementedResponse; ///< The number of 'not implemented' responses
    uint32_t mOtherResponse;          ///< The number of other responses

    uint32_t mResolvedBySrp; ///< The number of queries completely resolved by the local SRP server
};

} // namespace DBus
} // namespace otbr

#endif // OTBR_DBUS_COMMON_TYPES_HPP_
