/*
 *    Copyright (c) 2018, The OpenThread Authors.
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

#include <CppUTest/TestHarness.h>

#include "mdns/mdns_mdnssd.cpp"

TEST_GROUP(MdnsSd){};

TEST(MdnsSd, TestDNSErrorToString)
{
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NoError));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Unknown));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NoSuchName));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NoMemory));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadParam));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadReference));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadState));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadFlags));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Unsupported));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NotInitialized));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_AlreadyRegistered));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NameConflict));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Invalid));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Firewall));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Incompatible));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadInterfaceIndex));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Refused));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NoSuchRecord));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NoAuth));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NoSuchKey));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NATTraversal));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_DoubleNAT));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadTime));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadSig));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_BadKey));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Transient));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_ServiceNotRunning));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NATPortMappingUnsupported));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NATPortMappingDisabled));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_NoRouter));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_PollingMode));
    CHECK(nullptr != otbr::Mdns::DNSErrorToString(kDNSServiceErr_Timeout));
}
