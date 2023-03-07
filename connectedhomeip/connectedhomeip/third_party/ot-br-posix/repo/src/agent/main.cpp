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

#define OTBR_LOG_TAG "AGENT"

#include <openthread-br/config.h>

#include <algorithm>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <assert.h>
#include <openthread/logging.h>
#include <openthread/platform/radio.h>

#if __ANDROID__ && OTBR_CONFIG_ANDROID_PROPERTY_ENABLE
#include <cutils/properties.h>
#endif

#include "agent/application.hpp"
#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "common/mainloop.hpp"
#include "common/types.hpp"
#include "ncp/ncp_openthread.hpp"

static const char kSyslogIdent[]          = "otbr-agent";
static const char kDefaultInterfaceName[] = "wpan0";

enum
{
    OTBR_OPT_BACKBONE_INTERFACE_NAME = 'B',
    OTBR_OPT_DEBUG_LEVEL             = 'd',
    OTBR_OPT_HELP                    = 'h',
    OTBR_OPT_INTERFACE_NAME          = 'I',
    OTBR_OPT_VERBOSE                 = 'v',
    OTBR_OPT_VERSION                 = 'V',
    OTBR_OPT_SHORTMAX                = 128,
    OTBR_OPT_RADIO_VERSION,
    OTBR_OPT_AUTO_ATTACH,
};

static jmp_buf            sResetJump;
static otbr::Application *gApp = nullptr;

void                       __gcov_flush();
static const struct option kOptions[] = {
    {"backbone-ifname", required_argument, nullptr, OTBR_OPT_BACKBONE_INTERFACE_NAME},
    {"debug-level", required_argument, nullptr, OTBR_OPT_DEBUG_LEVEL},
    {"help", no_argument, nullptr, OTBR_OPT_HELP},
    {"thread-ifname", required_argument, nullptr, OTBR_OPT_INTERFACE_NAME},
    {"verbose", no_argument, nullptr, OTBR_OPT_VERBOSE},
    {"version", no_argument, nullptr, OTBR_OPT_VERSION},
    {"radio-version", no_argument, nullptr, OTBR_OPT_RADIO_VERSION},
    {"auto-attach", optional_argument, nullptr, OTBR_OPT_AUTO_ATTACH},
    {0, 0, 0, 0}};

static bool ParseInteger(const char *aStr, long &aOutResult)
{
    bool  successful = true;
    char *strEnd;
    long  result;

    VerifyOrExit(aStr != nullptr, successful = false);
    errno  = 0;
    result = strtol(aStr, &strEnd, 0);
    VerifyOrExit(errno != ERANGE, successful = false);
    VerifyOrExit(aStr != strEnd, successful = false);

    aOutResult = result;

exit:
    return successful;
}

static constexpr char kAutoAttachDisableArg[] = "--auto-attach=0";
static char           sAutoAttachDisableArgStorage[sizeof(kAutoAttachDisableArg)];

static std::vector<char *> AppendAutoAttachDisableArg(int argc, char *argv[])
{
    std::vector<char *> args(argv, argv + argc);

    args.erase(std::remove_if(
                   args.begin(), args.end(),
                   [](const char *arg) { return arg != nullptr && std::string(arg).rfind("--auto-attach", 0) == 0; }),
               args.end());
    strcpy(sAutoAttachDisableArgStorage, kAutoAttachDisableArg);
    args.push_back(sAutoAttachDisableArgStorage);
    args.push_back(nullptr);

    return args;
}

static void PrintHelp(const char *aProgramName)
{
    fprintf(stderr,
            "Usage: %s [-I interfaceName] [-B backboneIfName] [-d DEBUG_LEVEL] [-v] [--auto-attach[=0/1]] RADIO_URL "
            "[RADIO_URL]\n"
            "    --auto-attach defaults to 1\n",
            aProgramName);
    fprintf(stderr, "%s", otSysGetRadioUrlHelpString());
}

static void PrintVersion(void)
{
    printf("%s\n", OTBR_PACKAGE_VERSION);
}

static void OnAllocateFailed(void)
{
    otbrLogCrit("Allocate failure, exiting...");
    exit(1);
}

static otbrLogLevel GetDefaultLogLevel(void)
{
    otbrLogLevel level = OTBR_LOG_INFO;

#if __ANDROID__ && OTBR_CONFIG_ANDROID_PROPERTY_ENABLE
    char value[PROPERTY_VALUE_MAX];

    property_get("ro.build.type", value, "user");
    if (!strcmp(value, "user"))
    {
        level = OTBR_LOG_WARNING;
    }
#endif

    return level;
}

static void PrintRadioVersionAndExit(const std::vector<const char *> &aRadioUrls)
{
    otbr::Ncp::ControllerOpenThread ncpOpenThread{/* aInterfaceName */ "", aRadioUrls, /* aBackboneInterfaceName */ "",
                                                  /* aDryRun */ true, /* aEnableAutoAttach */ false};
    const char *                    radioVersion;

    ncpOpenThread.Init();

    radioVersion = otPlatRadioGetVersionString(ncpOpenThread.GetInstance());
    otbrLogNotice("Radio version: %s", radioVersion);
    printf("%s\n", radioVersion);

    ncpOpenThread.Deinit();

    exit(EXIT_SUCCESS);
}

static int realmain(int argc, char *argv[])
{
    otbrLogLevel              logLevel = GetDefaultLogLevel();
    int                       opt;
    int                       ret                   = EXIT_SUCCESS;
    const char *              interfaceName         = kDefaultInterfaceName;
    const char *              backboneInterfaceName = "";
    bool                      verbose               = false;
    bool                      printRadioVersion     = false;
    bool                      enableAutoAttach      = true;
    std::vector<const char *> radioUrls;
    long                      parseResult;

    std::set_new_handler(OnAllocateFailed);

    while ((opt = getopt_long(argc, argv, "B:d:hI:Vv", kOptions, nullptr)) != -1)
    {
        switch (opt)
        {
        case OTBR_OPT_BACKBONE_INTERFACE_NAME:
            backboneInterfaceName = optarg;
            break;

        case OTBR_OPT_DEBUG_LEVEL:
            VerifyOrExit(ParseInteger(optarg, parseResult), ret = EXIT_FAILURE);
            VerifyOrExit(OTBR_LOG_EMERG <= parseResult && parseResult <= OTBR_LOG_DEBUG, ret = EXIT_FAILURE);
            logLevel = static_cast<otbrLogLevel>(parseResult);
            break;

        case OTBR_OPT_INTERFACE_NAME:
            interfaceName = optarg;
            break;

        case OTBR_OPT_VERBOSE:
            verbose = true;
            break;

        case OTBR_OPT_VERSION:
            PrintVersion();
            ExitNow();
            break;

        case OTBR_OPT_HELP:
            PrintHelp(argv[0]);
            ExitNow(ret = EXIT_SUCCESS);
            break;

        case OTBR_OPT_RADIO_VERSION:
            printRadioVersion = true;
            break;

        case OTBR_OPT_AUTO_ATTACH:
            if (optarg == nullptr)
            {
                enableAutoAttach = true;
            }
            else
            {
                VerifyOrExit(ParseInteger(optarg, parseResult), ret = EXIT_FAILURE);
                enableAutoAttach = parseResult;
            }
            break;

        default:
            PrintHelp(argv[0]);
            ExitNow(ret = EXIT_FAILURE);
            break;
        }
    }

    otbrLogInit(kSyslogIdent, logLevel, verbose);
    otbrLogNotice("Running %s", OTBR_PACKAGE_VERSION);
    otbrLogNotice("Thread version: %s", otbr::Ncp::ControllerOpenThread::GetThreadVersion());
    otbrLogNotice("Thread interface: %s", interfaceName);
    otbrLogNotice("Backbone interface: %s", backboneInterfaceName);

    for (int i = optind; i < argc; i++)
    {
        otbrLogNotice("Radio URL: %s", argv[i]);
        radioUrls.push_back(argv[i]);
    }

    if (printRadioVersion)
    {
        PrintRadioVersionAndExit(radioUrls);
        assert(false);
    }

    {
        otbr::Application app(interfaceName, backboneInterfaceName, radioUrls, enableAutoAttach);

        gApp = &app;
        app.Init();

        ret = app.Run();

        app.Deinit();
    }

    otbrLogDeinit();

exit:
    return ret;
}

void otPlatReset(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    gPlatResetReason = OT_PLAT_RESET_REASON_SOFTWARE;

    VerifyOrDie(gApp != nullptr, "gApp is null");
    gApp->Deinit();
    gApp = nullptr;

    longjmp(sResetJump, 1);
    assert(false);
}

int main(int argc, char *argv[])
{
    if (setjmp(sResetJump))
    {
        std::vector<char *> args = AppendAutoAttachDisableArg(argc, argv);

        alarm(0);
#if OPENTHREAD_ENABLE_COVERAGE
        __gcov_flush();
#endif

        execvp(args[0], args.data());
    }

    return realmain(argc, argv);
}
