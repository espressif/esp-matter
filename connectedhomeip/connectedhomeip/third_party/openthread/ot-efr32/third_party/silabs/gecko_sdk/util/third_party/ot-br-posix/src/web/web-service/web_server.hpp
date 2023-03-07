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
 *   This file implements the web server of border router
 */

#ifndef OTBR_WEB_WEB_SERVICE_WEB_SERVER_
#define OTBR_WEB_WEB_SERVICE_WEB_SERVER_

#include "openthread-br/config.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <net/if.h>
#include <syslog.h>

#include <boost/asio/ip/tcp.hpp>

#include "web/web-service/wpan_service.hpp"

namespace SimpleWeb {
template <class T> class Server;
typedef boost::asio::ip::tcp::socket HTTP;
} // namespace SimpleWeb

namespace otbr {
namespace Web {

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

/**
 * This class implements the http server.
 *
 */
class WebServer
{
public:
    /**
     * This method is constructor to initialize the WebServer.
     *
     */
    WebServer(void);

    /**
     * This method is destructor to free the WebServer.
     *
     */
    ~WebServer(void);

    /**
     * This method starts the Web Server.
     *
     * @param[in] aIfName      The pointer to the Thread interface name.
     * @param[in] aListenAddr  The http server listen address, can be nullptr for any address.
     * @param[in] aPort        The port of http server.
     *
     */
    void StartWebServer(const char *aIfName, const char *aListenAddr, uint16_t aPort);

    /**
     * This method stops the Web Server.
     *
     */
    void StopWebServer(void);

private:
    typedef std::string (*HttpRequestCallback)(const std::string &aRequest, void *aUserData);
    static std::string HandleJoinNetworkRequest(const std::string &aJoinRequest, void *aUserData);
    static std::string HandleGetQRCodeRequest(const std::string &aGetQRCodeRequest, void *aUserData);
    static std::string HandleFormNetworkRequest(const std::string &aFormRequest, void *aUserData);
    static std::string HandleAddPrefixRequest(const std::string &aAddPrefixRequest, void *aUserData);
    static std::string HandleDeletePrefixRequest(const std::string &aDeletePrefixRequest, void *aUserData);
    static std::string HandleGetStatusRequest(const std::string &aGetStatusRequest, void *aUserData);
    static std::string HandleGetAvailableNetworkResponse(const std::string &aGetAvailableNetworkRequest,
                                                         void *             aUserData);
    static std::string HandleCommission(const std::string &aCommissionRequest, void *aUserData);

    std::string HandleJoinNetworkRequest(const std::string &aJoinRequest);
    std::string HandleGetQRCodeRequest(const std::string &aGetQRCodeRequest);
    std::string HandleFormNetworkRequest(const std::string &aFormRequest);
    std::string HandleAddPrefixRequest(const std::string &aAddPrefixRequest);
    std::string HandleDeletePrefixRequest(const std::string &aDeletePrefixRequest);
    std::string HandleGetStatusRequest(const std::string &aGetStatusRequest);
    std::string HandleGetAvailableNetworkResponse(const std::string &aGetAvailableNetworkRequest);
    std::string HandleCommission(const std::string &aCommissionRequest);

    void HandleHttpRequest(const char *aUrl, const char *aMethod, HttpRequestCallback aCallback);
    void ResponseGetQRCode(void);
    void ResponseJoinNetwork(void);
    void ResponseFormNetwork(void);
    void ResponseAddOnMeshPrefix(void);
    void ResponseDeleteOnMeshPrefix(void);
    void ResponseGetStatus(void);
    void ResponseGetAvailableNetwork(void);
    void DefaultHttpResponse(void);
    void ResponseCommission(void);

    void Init(void);

    HttpServer *           mServer;
    otbr::Web::WpanService mWpanService;
};

} // namespace Web
} // namespace otbr

#endif // OTBR_WEB_WEB_SERVICE_WEB_SERVER_
