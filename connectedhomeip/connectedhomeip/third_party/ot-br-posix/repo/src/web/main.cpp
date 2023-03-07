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
 *   This file is the entry of the program, it starts a Web service.
 */
#define OT_HTTP_PORT 80
#define OTBR_LOG_TAG "WEB"

#include "openthread-br/config.h"

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "web/web-service/web_server.hpp"

static const char kSyslogIdent[]          = "otbr-web";
static const char kDefaultInterfaceName[] = "wpan0";
static const char kDefaultListenAddr[]    = "0.0.0.0";

std::unique_ptr<otbr::Web::WebServer> sServer(nullptr);

static void HandleSignal(int aSignal)
{
    signal(aSignal, SIG_DFL);

    otbrLogCrit("Stopping web server");

    if (sServer != nullptr)
    {
        sServer->StopWebServer();
    }
}

static void PrintVersion(void)
{
    printf("%s\n", OTBR_PACKAGE_VERSION);
}

int main(int argc, char **argv)
{
    const char * interfaceName  = nullptr;
    const char * httpListenAddr = nullptr;
    const char * httpPort       = nullptr;
    otbrLogLevel logLevel       = OTBR_LOG_INFO;
    int          ret            = 0;
    int          opt;
    uint16_t     port = OT_HTTP_PORT;

    while ((opt = getopt(argc, argv, "d:I:p:va:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            httpListenAddr = optarg;
            break;
        case 'd':
            logLevel = static_cast<otbrLogLevel>(atoi(optarg));
            break;
        case 'I':
            interfaceName = optarg;
            break;

        case 'p':
            httpPort = optarg;
            VerifyOrExit(httpPort != nullptr);
            port = atoi(httpPort);
            break;

        case 'v':
            PrintVersion();
            ExitNow();
            break;

        default:
            fprintf(stderr, "Usage: %s [-d DEBUG_LEVEL] [-I interfaceName] [-p port] [-a listenAddress] [-v]\n",
                    argv[0]);
            ExitNow(ret = -1);
            break;
        }
    }

    otbrLogInit(kSyslogIdent, logLevel, true);
    otbrLogInfo("Running %s", OTBR_PACKAGE_VERSION);

    if (interfaceName == nullptr)
    {
        interfaceName = kDefaultInterfaceName;
        printf("interfaceName not specified, using default %s\n", interfaceName);
    }

    if (httpListenAddr == nullptr)
    {
        httpListenAddr = kDefaultListenAddr;
        printf("listenAddr not specified, using default %s\n", httpListenAddr);
    }

    if (httpPort == nullptr)
    {
        printf("http port not specified, using default %d\n", port);
    }

    otbrLogInfo("Border router web started on %s", interfaceName);

    // allow quitting elegantly
    signal(SIGTERM, HandleSignal);
    signal(SIGINT, HandleSignal);

    sServer.reset(new otbr::Web::WebServer());
    sServer->StartWebServer(interfaceName, httpListenAddr, port);

    otbrLogDeinit();

exit:
    return ret;
}
