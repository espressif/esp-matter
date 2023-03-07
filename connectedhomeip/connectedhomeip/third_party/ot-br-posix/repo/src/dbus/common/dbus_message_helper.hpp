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

/**
 * @file
 * This file includes utilities for manipulate d-bus message.
 */

#ifndef DBUS_MESSAGE_HELPER_HPP_
#define DBUS_MESSAGE_HELPER_HPP_

#include <array>
#include <string>
#include <tuple>
#include <vector>

#include <dbus/dbus.h>

#include "common/code_utils.hpp"
#include "common/types.hpp"
#include "dbus/common/dbus_resources.hpp"
#include "dbus/common/types.hpp"

namespace otbr {
namespace DBus {

otbrError DBusMessageEncode(DBusMessageIter *aIter, const otbrError &aError);
otbrError DBusMessageExtract(DBusMessageIter *aIter, otbrError &aError);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const ActiveScanResult &aScanResult);
otbrError DBusMessageExtract(DBusMessageIter *aIter, ActiveScanResult &aScanResult);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const EnergyScanResult &aResult);
otbrError DBusMessageExtract(DBusMessageIter *aIter, EnergyScanResult &aResult);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const LinkModeConfig &aConfig);
otbrError DBusMessageExtract(DBusMessageIter *aIter, LinkModeConfig &aConfig);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const Ip6Prefix &aPrefix);
otbrError DBusMessageExtract(DBusMessageIter *aIter, Ip6Prefix &aPrefix);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const ExternalRoute &aRoute);
otbrError DBusMessageExtract(DBusMessageIter *aIter, ExternalRoute &aRoute);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const OnMeshPrefix &aPrefix);
otbrError DBusMessageExtract(DBusMessageIter *aIter, OnMeshPrefix &aPrefix);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const MacCounters &aCounters);
otbrError DBusMessageExtract(DBusMessageIter *aIter, MacCounters &aCounters);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const IpCounters &aCounters);
otbrError DBusMessageExtract(DBusMessageIter *aIter, IpCounters &aCounters);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const ChildInfo &aChildInfo);
otbrError DBusMessageExtract(DBusMessageIter *aIter, ChildInfo &aChildInfo);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const NeighborInfo &aNeighborInfo);
otbrError DBusMessageExtract(DBusMessageIter *aIter, NeighborInfo &aNeighborInfo);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const LeaderData &aLeaderData);
otbrError DBusMessageExtract(DBusMessageIter *aIter, LeaderData &aLeaderData);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const ChannelQuality &aQuality);
otbrError DBusMessageExtract(DBusMessageIter *aIter, ChannelQuality &aQuality);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const TxtEntry &aTxtEntry);
otbrError DBusMessageExtract(DBusMessageIter *aIter, TxtEntry &aTxtEntry);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo::Registration &aRegistration);
otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo::Registration &aRegistration);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo::ResponseCounters &aResponseCounters);
otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo::ResponseCounters &aResponseCounters);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const SrpServerInfo &aSrpServerInfo);
otbrError DBusMessageExtract(DBusMessageIter *aIter, SrpServerInfo &aSrpServerInfo);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const MdnsResponseCounters &aMdnsResponseCounters);
otbrError DBusMessageExtract(DBusMessageIter *aIter, MdnsResponseCounters &aMdnsResponseCounters);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const MdnsTelemetryInfo &aMdnsTelemetryInfo);
otbrError DBusMessageExtract(DBusMessageIter *aIter, MdnsTelemetryInfo &aMdnsTelemetryInfo);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const DnssdCounters &aDnssdCounters);
otbrError DBusMessageExtract(DBusMessageIter *aIter, DnssdCounters &aDnssdCounters);

template <typename T> struct DBusTypeTrait;

template <> struct DBusTypeTrait<IpCounters>
{
    // struct of 32 bytes
    static constexpr const char *TYPE_AS_STRING = "(uuuu)";
};

template <> struct DBusTypeTrait<MacCounters>
{
    // struct of 32 bytes
    static constexpr const char *TYPE_AS_STRING = "(uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu)";
};

template <> struct DBusTypeTrait<LinkModeConfig>
{
    // struct of four booleans
    static constexpr const char *TYPE_AS_STRING = "(bbb)";
};

template <> struct DBusTypeTrait<std::vector<uint8_t>>
{
    // array of bytes
    static constexpr const char *TYPE_AS_STRING = "ay";
};

template <size_t SIZE> struct DBusTypeTrait<std::array<uint8_t, SIZE>>
{
    // array of bytes
    static constexpr const char *TYPE_AS_STRING = "ay";
};

template <> struct DBusTypeTrait<Ip6Prefix>
{
    // struct of {array of bytes, byte}
    static constexpr const char *TYPE_AS_STRING = "(ayy)";
};

template <> struct DBusTypeTrait<ExternalRoute>
{
    // struct of {{array of bytes, byte}, uint16, byte, bool, bool}
    static constexpr const char *TYPE_AS_STRING = "((ayy)qybb)";
};

template <> struct DBusTypeTrait<std::vector<ExternalRoute>>
{
    // array of {{array of bytes, byte}, uint16, byte, bool, bool}
    static constexpr const char *TYPE_AS_STRING = "a((ayy)qybb)";
};

template <> struct DBusTypeTrait<OnMeshPrefix>
{
    // struct of {{array of bytes, byte}, uint16, byte, bool, bool, bool, bool, bool, bool, bool, bool, bool}
    static constexpr const char *TYPE_AS_STRING = "((ayy)qybbbbbbbbb)";
};

template <> struct DBusTypeTrait<std::vector<OnMeshPrefix>>
{
    // array of {{array of bytes, byte}, uint16, byte, bool, bool, bool, bool, bool, bool, bool, bool, bool}
    static constexpr const char *TYPE_AS_STRING = "a((ayy)qybbbbbbbbb)";
};

template <> struct DBusTypeTrait<LeaderData>
{
    // struct of { uint32, byte, byte, byte, byte }
    static constexpr const char *TYPE_AS_STRING = "(uyyyy)";
};

template <> struct DBusTypeTrait<std::vector<ChannelQuality>>
{
    // array of struct of { uint8, uint16 }
    static constexpr const char *TYPE_AS_STRING = "a(yq)";
};

template <> struct DBusTypeTrait<NeighborInfo>
{
    // struct of { uint64, uint32, uint16, uint32, uint32, uint8,
    //             uint8, uint8, uint16, uint16, bool, bool, bool, bool }
    static constexpr const char *TYPE_AS_STRING = "(tuquuyyyqqbbbb)";
};

template <> struct DBusTypeTrait<std::vector<NeighborInfo>>
{
    // array of struct of { uint64, uint32, uint16, uint32, uint32, uint8,
    //                      uint8, uint8, uint16, uint16, bool, bool, bool }
    static constexpr const char *TYPE_AS_STRING = "a(tuquuyyyqqbbbb)";
};

template <> struct DBusTypeTrait<ChildInfo>
{
    // struct of { uint64, uint32, uint32, uint16, uint16, uint8, uint8,
    //             uint8, uint8, uint16, uint16, bool, bool, bool, bool }
    static constexpr const char *TYPE_AS_STRING = "(tuuqqyyyyqqbbbb)";
};

template <> struct DBusTypeTrait<ActiveScanResult>
{
    // struct of { uint64, string, uint64, array<uint8>, uint16, uint16, uint8,
    //             uint8, uint8, uint8, bool, bool }
    static constexpr const char *TYPE_AS_STRING = "(tstayqqyyyybb)";
};

template <> struct DBusTypeTrait<EnergyScanResult>
{
    // struct of { uint8, int8_t }
    static constexpr const char *TYPE_AS_STRING = "(yy)";
};

template <> struct DBusTypeTrait<ChannelQuality>
{
    // struct of { uint8, uint16}
    static constexpr const char *TYPE_AS_STRING = "(yq)";
};

template <> struct DBusTypeTrait<std::vector<ChildInfo>>
{
    // array of struct of { uint64, uint32, uint32, uint16, uint16, uint8, uint8,
    //                      uint8, uint8, uint16, uint16, bool, bool, bool, bool }
    static constexpr const char *TYPE_AS_STRING = "a(tuuqqyyyyqqbbbb)";
};

template <> struct DBusTypeTrait<TxtEntry>
{
    // struct of { string, array<uint8> }
    static constexpr const char *TYPE_AS_STRING = "(say)";
};

template <> struct DBusTypeTrait<std::vector<TxtEntry>>
{
    // array of struct of { string, array<uint8> }
    static constexpr const char *TYPE_AS_STRING = "a(say)";
};

template <> struct DBusTypeTrait<SrpServerState>
{
    static constexpr int         TYPE           = DBUS_TYPE_BYTE;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_BYTE_AS_STRING;
};

template <> struct DBusTypeTrait<SrpServerAddressMode>
{
    static constexpr int         TYPE           = DBUS_TYPE_BYTE;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_BYTE_AS_STRING;
};

template <> struct DBusTypeTrait<SrpServerInfo>
{
    // struct of { uint8, uint16, uint8,
    //              struct of { uint32, uint32, uint64, uint64, uint64, uint64 },
    //              struct of { uint32, uint32, uint64, uint64, uint64, uint64 },
    //              struct of { uint32, uint32, uint32, uint32, uint32, uint32} }
    static constexpr const char *TYPE_AS_STRING = "(yqy(uutttt)(uutttt)(uuuuuu))";
};

template <> struct DBusTypeTrait<MdnsTelemetryInfo>
{
    // struct of { struct of { uint32, uint32, uint32, uint32, uint32, uint32 },
    //              struct of { uint32, uint32, uint32, uint32, uint32, uint32 },
    //              struct of { uint32, uint32, uint32, uint32, uint32, uint32 },
    //              struct of { uint32, uint32, uint32, uint32, uint32, uint32 },
    //              uint32, uint32, uint32, uint32 }
    static constexpr const char *TYPE_AS_STRING = "((uuuuuu)(uuuuuu)(uuuuuu)(uuuuuu)uuuu)";
};

template <> struct DBusTypeTrait<DnssdCounters>
{
    // struct of { uint32, uint32, uint32, uint32, uint32, uint32, uint32 }
    static constexpr const char *TYPE_AS_STRING = "(uuuuuuu)";
};

template <> struct DBusTypeTrait<int8_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_BYTE;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_BYTE_AS_STRING;
};

template <> struct DBusTypeTrait<uint8_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_BYTE;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_BYTE_AS_STRING;
};

template <> struct DBusTypeTrait<uint16_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_UINT16;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_UINT16_AS_STRING;
};

template <> struct DBusTypeTrait<uint32_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_UINT32;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_UINT32_AS_STRING;
};

template <> struct DBusTypeTrait<uint64_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_UINT64;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_UINT64_AS_STRING;
};

template <> struct DBusTypeTrait<int16_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_INT16;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_INT16_AS_STRING;
};

template <> struct DBusTypeTrait<int32_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_INT32;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_INT32_AS_STRING;
};

template <> struct DBusTypeTrait<int64_t>
{
    static constexpr int         TYPE           = DBUS_TYPE_INT64;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_INT64_AS_STRING;
};

template <> struct DBusTypeTrait<std::string>
{
    static constexpr int         TYPE           = DBUS_TYPE_STRING;
    static constexpr const char *TYPE_AS_STRING = DBUS_TYPE_STRING_AS_STRING;
};

otbrError DBusMessageEncode(DBusMessageIter *aIter, bool aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, int8_t aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::string &aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const char *aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<uint8_t> &aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<uint16_t> &aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<uint32_t> &aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<uint64_t> &aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<int16_t> &aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<int32_t> &aValue);
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<int64_t> &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, bool &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, int8_t &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::string &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<uint8_t> &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<uint16_t> &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<uint32_t> &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<uint64_t> &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<int16_t> &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<int32_t> &aValue);
otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<int64_t> &aValue);

template <typename T> otbrError DBusMessageExtract(DBusMessageIter *aIter, T &aValue)
{
    otbrError error = OTBR_ERROR_DBUS;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBusTypeTrait<T>::TYPE);
    dbus_message_iter_get_basic(aIter, &aValue);
    dbus_message_iter_next(aIter);
    error = OTBR_ERROR_NONE;

exit:
    return error;
}

template <typename T> otbrError DBusMessageExtract(DBusMessageIter *aIter, std::vector<T> &aValue)
{
    otbrError       error = OTBR_ERROR_NONE;
    DBusMessageIter subIter;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_ARRAY, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &subIter);

    aValue.clear();
    while (dbus_message_iter_get_arg_type(&subIter) != DBUS_TYPE_INVALID)
    {
        T val;
        SuccessOrExit(error = DBusMessageExtract(&subIter, val));
        aValue.push_back(val);
    }
    dbus_message_iter_next(aIter);

exit:
    return error;
}

template <typename T> otbrError DBusMessageExtractPrimitive(DBusMessageIter *aIter, std::vector<T> &aValue)
{
    DBusMessageIter subIter;
    otbrError       error = OTBR_ERROR_NONE;
    T *             val;
    int             n;
    int             subtype;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_ARRAY, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &subIter);

    subtype = dbus_message_iter_get_arg_type(&subIter);
    if (subtype != DBUS_TYPE_INVALID)
    {
        VerifyOrExit(dbus_message_iter_get_arg_type(&subIter) == DBusTypeTrait<T>::TYPE, error = OTBR_ERROR_DBUS);
        dbus_message_iter_get_fixed_array(&subIter, &val, &n);

        if (val != nullptr)
        {
            aValue = std::vector<T>(val, val + n);
        }
    }
    dbus_message_iter_next(aIter);

exit:
    return error;
}

template <typename T, size_t SIZE> otbrError DBusMessageExtract(DBusMessageIter *aIter, std::array<T, SIZE> &aValue)
{
    DBusMessageIter subIter;
    otbrError       error = OTBR_ERROR_NONE;
    T *             val;
    int             n;
    int             subtype;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_ARRAY, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &subIter);

    subtype = dbus_message_iter_get_arg_type(&subIter);
    if (subtype != DBUS_TYPE_INVALID)
    {
        VerifyOrExit(dbus_message_iter_get_arg_type(&subIter) == DBusTypeTrait<T>::TYPE, error = OTBR_ERROR_DBUS);
        dbus_message_iter_get_fixed_array(&subIter, &val, &n);
        VerifyOrExit(n == SIZE, error = OTBR_ERROR_DBUS);

        if (val != nullptr)
        {
            std::copy(val, val + n, aValue.begin());
        }
    }
    dbus_message_iter_next(aIter);

exit:
    return error;
}

template <typename T> otbrError DBusMessageEncode(DBusMessageIter *aIter, T aValue)
{
    otbrError error = OTBR_ERROR_NONE;
    VerifyOrExit(dbus_message_iter_append_basic(aIter, DBusTypeTrait<T>::TYPE, &aValue), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

template <typename T> otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::vector<T> &aValue)
{
    otbrError       error = OTBR_ERROR_NONE;
    DBusMessageIter subIter;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_ARRAY, DBusTypeTrait<T>::TYPE_AS_STRING, &subIter),
                 error = OTBR_ERROR_DBUS);

    for (const auto &v : aValue)
    {
        SuccessOrExit(error = DBusMessageEncode(&subIter, v));
    }

    VerifyOrExit(dbus_message_iter_close_container(aIter, &subIter), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

template <typename T> otbrError DBusMessageEncodePrimitive(DBusMessageIter *aIter, const std::vector<T> &aValue)
{
    DBusMessageIter subIter;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_ARRAY, DBusTypeTrait<T>::TYPE_AS_STRING, &subIter),
                 error = OTBR_ERROR_DBUS);

    if (!aValue.empty())
    {
        const T *buf = &aValue.front();

        VerifyOrExit(dbus_message_iter_append_fixed_array(&subIter, DBusTypeTrait<T>::TYPE, &buf,
                                                          static_cast<int>(aValue.size())),
                     error = OTBR_ERROR_DBUS);
    }
    VerifyOrExit(dbus_message_iter_close_container(aIter, &subIter), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

template <typename T, size_t SIZE>
otbrError DBusMessageEncode(DBusMessageIter *aIter, const std::array<T, SIZE> &aValue)
{
    DBusMessageIter subIter;
    otbrError       error = OTBR_ERROR_NONE;

    VerifyOrExit(dbus_message_iter_open_container(aIter, DBUS_TYPE_ARRAY, DBusTypeTrait<T>::TYPE_AS_STRING, &subIter),
                 error = OTBR_ERROR_DBUS);

    if (!aValue.empty())
    {
        const T *buf = &aValue.front();

        VerifyOrExit(dbus_message_iter_append_fixed_array(&subIter, DBusTypeTrait<T>::TYPE, &buf,
                                                          static_cast<int>(aValue.size())),
                     error = OTBR_ERROR_DBUS);
    }
    VerifyOrExit(dbus_message_iter_close_container(aIter, &subIter), error = OTBR_ERROR_DBUS);
exit:
    return error;
}

template <size_t I, typename... FieldTypes> struct ElementType
{
    using ValueType         = typename std::tuple_element<I, std::tuple<FieldTypes...>>::type;
    using NonconstValueType = typename std::remove_cv<ValueType>::type;
    using RawValueType      = typename std::remove_reference<NonconstValueType>::type;
};

template <size_t I, size_t N, typename... FieldTypes> class DBusMessageIterFor
{
public:
    static otbrError ConvertToTuple(DBusMessageIter *aIter, std::tuple<FieldTypes...> &aValues)
    {
        using RawValueType  = typename ElementType<N - I, FieldTypes...>::RawValueType;
        RawValueType &val   = std::get<N - I>(aValues);
        otbrError     error = DBusMessageExtract(aIter, val);

        SuccessOrExit(error);
        error = DBusMessageIterFor<I - 1, N, FieldTypes...>::ConvertToTuple(aIter, aValues);

    exit:
        return error;
    }

    static otbrError ConvertToDBusMessage(DBusMessageIter *aIter, const std::tuple<FieldTypes...> &aValues)
    {
        otbrError error = DBusMessageEncode(aIter, std::get<N - I>(aValues));

        SuccessOrExit(error);
        error = DBusMessageIterFor<I - 1, N, FieldTypes...>::ConvertToDBusMessage(aIter, aValues);

    exit:
        return error;
    }
};

template <> class DBusMessageIterFor<0, 0>
{
public:
    static otbrError ConvertToTuple(DBusMessageIter *aIter, std::tuple<> &aValues)
    {
        OTBR_UNUSED_VARIABLE(aIter);
        OTBR_UNUSED_VARIABLE(aValues);
        return OTBR_ERROR_NONE;
    }

    static otbrError ConvertToDBusMessage(DBusMessageIter *aIter, const std::tuple<> &aValues)
    {
        OTBR_UNUSED_VARIABLE(aIter);
        OTBR_UNUSED_VARIABLE(aValues);
        return OTBR_ERROR_NONE;
    }
};

template <size_t N, typename... FieldTypes> class DBusMessageIterFor<1, N, FieldTypes...>
{
public:
    static otbrError ConvertToTuple(DBusMessageIter *aIter, std::tuple<FieldTypes...> &aValues)
    {
        using RawValueType  = typename ElementType<N - 1, FieldTypes...>::RawValueType;
        RawValueType &val   = std::get<N - 1>(aValues);
        otbrError     error = DBusMessageExtract(aIter, val);

        return error;
    }

    static otbrError ConvertToDBusMessage(DBusMessageIter *aIter, const std::tuple<FieldTypes...> &aValues)
    {
        otbrError error = DBusMessageEncode(aIter, std::get<N - 1>(aValues));

        return error;
    }
};

template <typename... FieldTypes>
otbrError ConvertToDBusMessage(DBusMessageIter *aIter, const std::tuple<FieldTypes...> &aValues)
{
    return DBusMessageIterFor<sizeof...(FieldTypes), sizeof...(FieldTypes), FieldTypes...>::ConvertToDBusMessage(
        aIter, aValues);
}

template <typename... FieldTypes>
constexpr otbrError ConvertToTuple(DBusMessageIter *aIter, std::tuple<FieldTypes...> &aValues)
{
    return DBusMessageIterFor<sizeof...(FieldTypes), sizeof...(FieldTypes), FieldTypes...>::ConvertToTuple(aIter,
                                                                                                           aValues);
}

/**
 * This function converts a value to a d-bus variant.
 *
 * @param[out] aIter   The message iterator pointing to the variant.
 * @param[in]  aValue  The value input.
 *
 * @retval OTBR_ERROR_NONE  Successfully encoded to the variant.
 * @retval OTBR_ERROR_DBUS  Failed to encode to the variant.
 */
template <typename ValueType> otbrError DBusMessageEncodeToVariant(DBusMessageIter *aIter, const ValueType &aValue)
{
    otbrError       error = OTBR_ERROR_NONE;
    DBusMessageIter subIter;

    VerifyOrExit(
        dbus_message_iter_open_container(aIter, DBUS_TYPE_VARIANT, DBusTypeTrait<ValueType>::TYPE_AS_STRING, &subIter),
        error = OTBR_ERROR_DBUS);

    SuccessOrExit(error = DBusMessageEncode(&subIter, aValue));

    VerifyOrExit(dbus_message_iter_close_container(aIter, &subIter), error = OTBR_ERROR_DBUS);

exit:
    return error;
}

/**
 * This function converts a d-bus variant to a value.
 *
 * @param[in]  aIter   The message iterator pointing to the variant.
 * @param[out] aValue  The value output.
 *
 * @retval OTBR_ERROR_NONE  Successfully decoded the variant.
 * @retval OTBR_ERROR_DBUS  Failed to decode the variant.
 */
template <typename ValueType> otbrError DBusMessageExtractFromVariant(DBusMessageIter *aIter, ValueType &aValue)
{
    otbrError       error = OTBR_ERROR_NONE;
    DBusMessageIter subIter;

    VerifyOrExit(dbus_message_iter_get_arg_type(aIter) == DBUS_TYPE_VARIANT, error = OTBR_ERROR_DBUS);
    dbus_message_iter_recurse(aIter, &subIter);

    SuccessOrExit(error = DBusMessageExtract(&subIter, aValue));

exit:
    return error;
}

/**
 * This function converts a d-bus message to a tuple of C++ types.
 *
 * @param[in]  aMessage  The dbus message to decode.
 * @param[out] aValues   The tuple output.
 *
 * @retval OTBR_ERROR_NONE  Successfully decoded the message.
 * @retval OTBR_ERROR_DBUS  Failed to decode the message.
 */
template <typename... FieldTypes>
otbrError DBusMessageToTuple(DBusMessage &aMessage, std::tuple<FieldTypes...> &aValues)
{
    otbrError       error = OTBR_ERROR_NONE;
    DBusMessageIter iter;

    VerifyOrExit(dbus_message_iter_init(&aMessage, &iter), error = OTBR_ERROR_DBUS);

    error = ConvertToTuple(&iter, aValues);

exit:
    return error;
}

/**
 * This function converts a tuple of C++ types to a d-bus message.
 *
 * @param[out] aMessage  The dbus message output.
 * @param[in]  aValues   The tuple to encode.
 *
 * @retval OTBR_ERROR_NONE  Successfully encoded the message.
 * @retval OTBR_ERROR_DBUS  Failed to encode the message.
 */
template <typename... FieldTypes>
otbrError TupleToDBusMessage(DBusMessage &aMessage, const std::tuple<FieldTypes...> &aValues)
{
    DBusMessageIter iter;

    dbus_message_iter_init_append(&aMessage, &iter);
    return ConvertToDBusMessage(&iter, aValues);
}

/**
 * This function converts a d-bus message to a tuple of C++ types.
 *
 * @param[in]  aMessage  The dbus message to decode.
 * @param[out] aValues   The tuple output.
 *
 * @retval OTBR_ERROR_NONE  Successfully decoded the message.
 * @retval OTBR_ERROR_DBUS  Failed to decode the message.
 */
template <typename... FieldTypes>
otbrError DBusMessageToTuple(UniqueDBusMessage const &aMessage, std::tuple<FieldTypes...> &aValues)
{
    return DBusMessageToTuple(*aMessage.get(), aValues);
}

bool IsDBusMessageEmpty(DBusMessage &aMessage);

} // namespace DBus
} // namespace otbr

#endif // DBUS_MESSAGE_HELPER_HPP_
