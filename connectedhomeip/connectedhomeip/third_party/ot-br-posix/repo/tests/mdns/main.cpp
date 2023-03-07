/*
 *    Copyright (c) 2017, The OpenThread Authors.
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

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <netinet/in.h>
#include <signal.h>

#include <vector>

#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "common/mainloop.hpp"
#include "common/mainloop_manager.hpp"
#include "mdns/mdns.hpp"

using namespace otbr;

static struct Context
{
    Mdns::Publisher *mPublisher;
    bool             mUpdate;
} sContext;

int RunMainloop(void)
{
    int rval = 0;

    while (true)
    {
        MainloopContext mainloop;

        mainloop.mMaxFd   = -1;
        mainloop.mTimeout = {INT_MAX, INT_MAX};
        FD_ZERO(&mainloop.mReadFdSet);
        FD_ZERO(&mainloop.mWriteFdSet);
        FD_ZERO(&mainloop.mErrorFdSet);

        MainloopManager::GetInstance().Update(mainloop);
        rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                      (mainloop.mTimeout.tv_sec == INT_MAX ? nullptr : &mainloop.mTimeout));

        if (rval < 0)
        {
            perror("select");
            break;
        }

        MainloopManager::GetInstance().Process(mainloop);
    }

    return rval;
}

void PublishSingleServiceWithCustomHost(void *aContext, Mdns::Publisher::State aState)
{
    uint8_t              xpanid[kSizeExtPanId] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    uint8_t              extAddr[kSizeExtAddr] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    std::vector<uint8_t> hostAddr(16, 0);
    const char           hostName[] = "custom-host";

    hostAddr[0]  = 0x20;
    hostAddr[1]  = 0x02;
    hostAddr[15] = 0x01;

    VerifyOrDie(aContext == &sContext, "unexpected context");
    if (aState == Mdns::Publisher::State::kReady)
    {
        Mdns::Publisher::TxtList txtList{
            {"nn", "cool"}, {"xp", xpanid, sizeof(xpanid)}, {"tv", "1.1.1"}, {"xa", extAddr, sizeof(extAddr)}};

        sContext.mPublisher->PublishHost(hostName, hostAddr,
                                         [](otbrError aError) { SuccessOrDie(aError, "cannot publish the host"); });

        sContext.mPublisher->PublishService(
            hostName, "SingleService", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "cannot publish the service"); });
    }
}

void PublishMultipleServicesWithCustomHost(void *aContext, Mdns::Publisher::State aState)
{
    uint8_t              xpanid[kSizeExtPanId] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    uint8_t              extAddr[kSizeExtAddr] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    std::vector<uint8_t> hostAddr(16, 0);
    const char           hostName1[] = "custom-host-1";
    const char           hostName2[] = "custom-host-2";

    hostAddr[0]  = 0x20;
    hostAddr[1]  = 0x02;
    hostAddr[15] = 0x01;

    VerifyOrDie(aContext == &sContext, "unexpected context");
    if (aState == Mdns::Publisher::State::kReady)
    {
        Mdns::Publisher::TxtList txtList{
            {"nn", "cool"}, {"xp", xpanid, sizeof(xpanid)}, {"tv", "1.1.1"}, {"xa", extAddr, sizeof(extAddr)}};

        sContext.mPublisher->PublishHost(hostName1, hostAddr,
                                         [](otbrError aError) { SuccessOrDie(aError, "cannot publish the host"); });

        sContext.mPublisher->PublishService(
            hostName1, "MultipleService11", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "cannot publish the first service"); });

        sContext.mPublisher->PublishService(
            hostName1, "MultipleService12", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "cannot publish the second service"); });

        sContext.mPublisher->PublishHost(
            hostName2, hostAddr, [](otbrError aError) { SuccessOrDie(aError, "cannot publish the second host"); });

        sContext.mPublisher->PublishService(
            hostName2, "MultipleService21", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "cannot publish the first service"); });

        sContext.mPublisher->PublishService(
            hostName2, "MultipleService22", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "cannot publish the second service"); });
    }
}

void PublishSingleService(void *aContext, Mdns::Publisher::State aState)
{
    OT_UNUSED_VARIABLE(aContext);

    uint8_t                  xpanid[kSizeExtPanId] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    uint8_t                  extAddr[kSizeExtAddr] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    Mdns::Publisher::TxtList txtList{
        {"nn", "cool"}, {"xp", xpanid, sizeof(xpanid)}, {"tv", "1.1.1"}, {"xa", extAddr, sizeof(extAddr)}};

    assert(aContext == &sContext);
    if (aState == Mdns::Publisher::State::kReady)
    {
        sContext.mPublisher->PublishService(
            "", "SingleService", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "SingleService._meshcop._udp."); });
    }
}

void PublishMultipleServices(void *aContext, Mdns::Publisher::State aState)
{
    OT_UNUSED_VARIABLE(aContext);

    uint8_t xpanid[kSizeExtPanId] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    uint8_t extAddr[kSizeExtAddr] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};

    assert(aContext == &sContext);
    if (aState == Mdns::Publisher::State::kReady)
    {
        Mdns::Publisher::TxtList txtList{
            {"nn", "cool1"}, {"xp", xpanid, sizeof(xpanid)}, {"tv", "1.1.1"}, {"xa", extAddr, sizeof(extAddr)}};

        sContext.mPublisher->PublishService(
            "", "MultipleService1", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "MultipleService1._meshcop._udp."); });
    }

    if (aState == Mdns::Publisher::State::kReady)
    {
        Mdns::Publisher::TxtList txtList{
            {"nn", "cool2"}, {"xp", xpanid, sizeof(xpanid)}, {"tv", "1.1.1"}, {"xa", extAddr, sizeof(extAddr)}};

        sContext.mPublisher->PublishService(
            "", "MultipleService2", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "MultipleService2._meshcop._udp."); });
    }
}

void PublishUpdateServices(void *aContext)
{
    OT_UNUSED_VARIABLE(aContext);

    uint8_t xpanidOld[kSizeExtPanId] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    uint8_t xpanidNew[kSizeExtPanId] = {0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41};
    uint8_t extAddr[kSizeExtAddr]    = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};

    assert(aContext == &sContext);
    if (!sContext.mUpdate)
    {
        Mdns::Publisher::TxtList txtList{
            {"nn", "cool"}, {"xp", xpanidOld, sizeof(xpanidOld)}, {"tv", "1.1.1"}, {"xa", extAddr, sizeof(extAddr)}};

        sContext.mPublisher->PublishService(
            "", "UpdateService", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { otbrLogResult(aError, "UpdateService._meshcop._udp"); });
    }
    else
    {
        Mdns::Publisher::TxtList txtList{{"nn", "coolcool"},
                                         {"xp", xpanidNew, sizeof(xpanidNew)},
                                         {"tv", "1.1.1"},
                                         {"xa", extAddr, sizeof(extAddr)}};

        sContext.mPublisher->PublishService(
            "", "UpdateService", "_meshcop._udp.", Mdns::Publisher::SubTypeList{}, 12345, txtList,
            [](otbrError aError) { SuccessOrDie(aError, "UpdateService._meshcop._udp"); });
    }
}

void PublishServiceSubTypes(void *aContext)
{
    OT_UNUSED_VARIABLE(aContext);

    assert(aContext == &sContext);

    Mdns::Publisher::SubTypeList subTypeList{"_subtype1", "_SUBTYPE2"};

    subTypeList.back() = "_SUBTYPE3";

    sContext.mPublisher->PublishService(
        "", "ServiceWithSubTypes", "_meshcop._udp.", subTypeList, 12345, Mdns::Publisher::TxtList{},
        [](otbrError aError) { SuccessOrDie(aError, "ServiceWithSubTypes._meshcop._udp."); });
}

otbrError TestSingleServiceWithCustomHost(void)
{
    otbrError error = OTBR_ERROR_NONE;

    Mdns::Publisher *pub = Mdns::Publisher::Create(
        [](Mdns::Publisher::State aState) { PublishSingleServiceWithCustomHost(&sContext, aState); });
    sContext.mPublisher = pub;
    SuccessOrExit(error = pub->Start());
    RunMainloop();

exit:
    Mdns::Publisher::Destroy(pub);
    return error;
}

otbrError TestMultipleServicesWithCustomHost(void)
{
    otbrError error = OTBR_ERROR_NONE;

    Mdns::Publisher *pub = Mdns::Publisher::Create(
        [](Mdns::Publisher::State aState) { PublishMultipleServicesWithCustomHost(&sContext, aState); });
    sContext.mPublisher = pub;
    SuccessOrExit(error = pub->Start());
    RunMainloop();

exit:
    Mdns::Publisher::Destroy(pub);
    return error;
}

otbrError TestSingleService(void)
{
    otbrError ret = OTBR_ERROR_NONE;

    Mdns::Publisher *pub =
        Mdns::Publisher::Create([](Mdns::Publisher::State aState) { PublishSingleService(&sContext, aState); });
    sContext.mPublisher = pub;
    SuccessOrExit(ret = pub->Start());
    RunMainloop();

exit:
    Mdns::Publisher::Destroy(pub);
    return ret;
}

otbrError TestMultipleServices(void)
{
    otbrError ret = OTBR_ERROR_NONE;

    Mdns::Publisher *pub =
        Mdns::Publisher::Create([](Mdns::Publisher::State aState) { PublishMultipleServices(&sContext, aState); });
    sContext.mPublisher = pub;
    SuccessOrExit(ret = pub->Start());
    RunMainloop();

exit:
    Mdns::Publisher::Destroy(pub);
    return ret;
}

otbrError TestUpdateService(void)
{
    otbrError ret = OTBR_ERROR_NONE;

    Mdns::Publisher *pub = Mdns::Publisher::Create([](Mdns::Publisher::State aState) {
        if (aState == Mdns::Publisher::State::kReady)
        {
            sContext.mUpdate = false;
            PublishUpdateServices(&sContext);
            sContext.mUpdate = true;
            PublishUpdateServices(&sContext);
        }
    });
    sContext.mPublisher  = pub;
    SuccessOrExit(ret = pub->Start());
    RunMainloop();

exit:
    Mdns::Publisher::Destroy(pub);
    return ret;
}

otbrError TestServiceSubTypes(void)
{
    otbrError ret = OTBR_ERROR_NONE;

    Mdns::Publisher *pub = Mdns::Publisher::Create([](Mdns::Publisher::State aState) {
        if (aState == Mdns::Publisher::State::kReady)
        {
            PublishServiceSubTypes(&sContext);
        }
    });
    sContext.mPublisher  = pub;
    SuccessOrExit(ret = pub->Start());
    RunMainloop();

exit:
    Mdns::Publisher::Destroy(pub);
    return ret;
}

void RecoverSignal(int aSignal)
{
    if (aSignal == SIGUSR1)
    {
        signal(SIGUSR1, SIG_DFL);
    }
    else if (aSignal == SIGUSR2)
    {
        signal(SIGUSR2, SIG_DFL);
    }
}

otbrError TestStopService(void)
{
    otbrError ret = OTBR_ERROR_NONE;

    Mdns::Publisher *pub =
        Mdns::Publisher::Create([](Mdns::Publisher::State aState) { PublishSingleService(&sContext, aState); });
    sContext.mPublisher = pub;
    SuccessOrExit(ret = pub->Start());
    signal(SIGUSR1, RecoverSignal);
    signal(SIGUSR2, RecoverSignal);
    RunMainloop();
    sContext.mPublisher->Stop();
    RunMainloop();
    SuccessOrExit(ret = sContext.mPublisher->Start());
    RunMainloop();

exit:
    Mdns::Publisher::Destroy(pub);
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    if (argc < 2)
    {
        return 1;
    }

    otbrLogInit("otbr-mdns", OTBR_LOG_DEBUG, true);
    // allow quitting elegantly
    signal(SIGTERM, RecoverSignal);
    switch (argv[1][0])
    {
    case 's':
        ret = argv[1][1] == 'c' ? TestSingleServiceWithCustomHost() : TestSingleService();
        break;

    case 'm':
        ret = argv[1][1] == 'c' ? TestMultipleServicesWithCustomHost() : TestMultipleServices();
        break;

    case 'u':
        ret = TestUpdateService();
        break;

    case 't':
        ret = TestServiceSubTypes();
        break;

    case 'k':
        ret = TestStopService();
        break;

    default:
        ret = 1;
        break;
    }

    return ret;
}
