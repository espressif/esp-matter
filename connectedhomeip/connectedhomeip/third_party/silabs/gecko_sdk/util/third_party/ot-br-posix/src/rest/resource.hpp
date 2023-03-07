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

/**
 * @file
 *   This file includes Handler definition for RESTful HTTP server.
 */

#ifndef OTBR_REST_RESOURCE_HPP_
#define OTBR_REST_RESOURCE_HPP_

#include <unordered_map>

#include <openthread/border_router.h>

#include "ncp/ncp_openthread.hpp"
#include "rest/json.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"
#include "utils/thread_helper.hpp"

using otbr::Ncp::ControllerOpenThread;
using std::chrono::steady_clock;

namespace otbr {
namespace rest {

/**
 * This class implements the Resource handler for OTBR-REST.
 *
 */
class Resource
{
public:
    /**
     * The constructor initializes the resource handler instance.
     *
     * @param[in] aNcp  A pointer to the NCP controller.
     *
     */
    Resource(ControllerOpenThread *aNcp);

    /**
     * This method initialize the Resource handler.
     *
     *
     */
    void Init(void);

    /**
     * This method is the main entry of resource handler, which find corresponding handler according to request url
     * find the resource and set the content of response.
     *
     * @param[in]     aRequest  A request instance referred by the Resource handler.
     * @param[in,out] aResponse  A response instance will be set by the Resource handler.
     *
     */
    void Handle(Request &aRequest, Response &aResponse) const;

    /**
     * This method distributes a callback handler for each connection needs a callback.
     *
     * @param[in]     aRequest   A request instance referred by the Resource handler.
     * @param[in,out] aResponse  A response instance will be set by the Resource handler.
     *
     */
    void HandleCallback(Request &aRequest, Response &aResponse);

    /**
     * This method provides a quick handler, which could directly set response code of a response and set error code and
     * error message to the request body.
     *
     * @param[in]     aRequest    A request instance referred by the Resource handler.
     * @param[in,out] aErrorCode  An enum class represents the status code.
     *
     */
    void ErrorHandler(Response &aResponse, HttpStatusCode aErrorCode) const;

private:
    typedef void (Resource::*ResourceHandler)(const Request &aRequest, Response &aResponse) const;
    typedef void (Resource::*ResourceCallbackHandler)(const Request &aRequest, Response &aResponse);
    void NodeInfo(const Request &aRequest, Response &aResponse) const;
    void ExtendedAddr(const Request &aRequest, Response &aResponse) const;
    void State(const Request &aRequest, Response &aResponse) const;
    void NetworkName(const Request &aRequest, Response &aResponse) const;
    void LeaderData(const Request &aRequest, Response &aResponse) const;
    void NumOfRoute(const Request &aRequest, Response &aResponse) const;
    void Rloc16(const Request &aRequest, Response &aResponse) const;
    void ExtendedPanId(const Request &aRequest, Response &aResponse) const;
    void Rloc(const Request &aRequest, Response &aResponse) const;
    void Diagnostic(const Request &aRequest, Response &aResponse) const;
    void HandleDiagnosticCallback(const Request &aRequest, Response &aResponse);

    void GetNodeInfo(Response &aResponse) const;
    void GetDataExtendedAddr(Response &aResponse) const;
    void GetDataState(Response &aResponse) const;
    void GetDataNetworkName(Response &aResponse) const;
    void GetDataLeaderData(Response &aResponse) const;
    void GetDataNumOfRoute(Response &aResponse) const;
    void GetDataRloc16(Response &aResponse) const;
    void GetDataExtendedPanId(Response &aResponse) const;
    void GetDataRloc(Response &aResponse) const;

    void DeleteOutDatedDiagnostic(void);
    void UpdateDiag(std::string aKey, std::vector<otNetworkDiagTlv> &aDiag);

    static void DiagnosticResponseHandler(otError              aError,
                                          otMessage *          aMessage,
                                          const otMessageInfo *aMessageInfo,
                                          void *               aContext);
    void        DiagnosticResponseHandler(otError aError, const otMessage *aMessage, const otMessageInfo *aMessageInfo);

    otInstance *          mInstance;
    ControllerOpenThread *mNcp;

    std::unordered_map<std::string, ResourceHandler>         mResourceMap;
    std::unordered_map<std::string, ResourceCallbackHandler> mResourceCallbackMap;

    std::unordered_map<std::string, DiagInfo> mDiagSet;
};

} // namespace rest
} // namespace otbr

#endif // OTBR_REST_RESOURCE_HPP_
