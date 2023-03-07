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

#define OTBR_LOG_TAG "WEB"

#include "web/web-service/web_server.hpp"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <server_http.hpp>

#include "common/code_utils.hpp"
#include "common/logging.hpp"

#define OT_ADD_PREFIX_PATH "^/add_prefix"
#define OT_AVAILABLE_NETWORK_PATH "^/available_network$"
#define OT_DELETE_PREFIX_PATH "^/delete_prefix"
#define OT_FORM_NETWORK_PATH "^/form_network$"
#define OT_GET_NETWORK_PATH "^/get_properties$"
#define OT_JOIN_NETWORK_PATH "^/join_network$"
#define OT_GET_QRCODE_PATH "^/get_qrcode$"
#define OT_SET_NETWORK_PATH "^/settings$"
#define OT_COMMISSIONER_START_PATH "^/commission$"
#define OT_REQUEST_METHOD_GET "GET"
#define OT_REQUEST_METHOD_POST "POST"
#define OT_RESPONSE_SUCCESS_STATUS "HTTP/1.1 200 OK\r\n"
#define OT_RESPONSE_HEADER_LENGTH "Content-Length: "
#define OT_RESPONSE_HEADER_CSS_TYPE "\r\nContent-Type: text/css"
#define OT_RESPONSE_HEADER_TEXT_HTML_TYPE "\r\nContent-Type: text/html; charset=utf-8"
#define OT_RESPONSE_HEADER_TYPE "Content-Type: application/json\r\n charset=utf-8"
#define OT_RESPONSE_PLACEHOLD "\r\n\r\n"
#define OT_RESPONSE_FAILURE_STATUS "HTTP/1.1 400 Bad Request\r\n"
#define OT_BUFFER_SIZE 1024

namespace otbr {
namespace Web {

static void EscapeHtml(std::string &content)
{
    std::string output;

    output.reserve(content.size());
    for (char c : content)
    {
        switch (c)
        {
        case '&':
            output.append("&amp;");
            break;
        case '<':
            output.append("&lt;");
            break;
        case '>':
            output.append("&gt;");
            break;
        case '"':
            output.append("&quot;");
            break;
        case '\'':
            output.append("&apos;");
            break;
        default:
            output.push_back(c);
            break;
        }
    }

    output.swap(content);
}

WebServer::WebServer(void)
    : mServer(new HttpServer())
{
}

WebServer::~WebServer(void)
{
    delete mServer;
}

void WebServer::Init()
{
    std::string networkName, extPanId;

    if (mWpanService.GetWpanServiceStatus(networkName, extPanId) > 0)
    {
        return;
    }
}

void WebServer::StartWebServer(const char *aIfName, const char *aListenAddr, uint16_t aPort)
{
    if (aListenAddr != nullptr)
    {
        mServer->config.address = aListenAddr;
    }
    mServer->config.port = aPort;
    mWpanService.SetInterfaceName(aIfName);
    Init();
    ResponseGetQRCode();
    ResponseJoinNetwork();
    ResponseFormNetwork();
    ResponseAddOnMeshPrefix();
    ResponseDeleteOnMeshPrefix();
    ResponseGetStatus();
    ResponseGetAvailableNetwork();
    ResponseCommission();
    DefaultHttpResponse();

    try
    {
        mServer->start();
    } catch (const std::exception &e)
    {
        otbrLogCrit("failed to start web server: %s", e.what());
        abort();
    }
}

void WebServer::StopWebServer(void)
{
    try
    {
        mServer->stop();
    } catch (const std::exception &e)
    {
        otbrLogCrit("failed to stop web server: %s", e.what());
    }
}

void WebServer::HandleHttpRequest(const char *aUrl, const char *aMethod, HttpRequestCallback aCallback)
{
    mServer->resource[aUrl][aMethod] = [aCallback, this](std::shared_ptr<HttpServer::Response> response,
                                                         std::shared_ptr<HttpServer::Request>  request) {
        try
        {
            std::string httpResponse;
            if (aCallback != nullptr)
            {
                httpResponse = aCallback(request->content.string(), this);
            }

            *response << OT_RESPONSE_SUCCESS_STATUS << OT_RESPONSE_HEADER_LENGTH << httpResponse.length()
                      << OT_RESPONSE_PLACEHOLD << httpResponse;
        } catch (std::exception &e)
        {
            std::string content = e.what();
            EscapeHtml(content);
            *response << OT_RESPONSE_FAILURE_STATUS << OT_RESPONSE_HEADER_LENGTH << strlen(e.what())
                      << OT_RESPONSE_PLACEHOLD << content;
        }
    };
}

void DefaultResourceSend(const HttpServer &                           aServer,
                         const std::shared_ptr<HttpServer::Response> &aResponse,
                         const std::shared_ptr<std::ifstream> &       aIfStream)
{
    static std::vector<char> buffer(OT_BUFFER_SIZE); // Safe when server is running on one thread

    std::streamsize readLength;

    if ((readLength = aIfStream->read(&buffer[0], buffer.size()).gcount()) > 0)
    {
        aResponse->write(&buffer[0], readLength);
        if (readLength == static_cast<std::streamsize>(buffer.size()))
        {
            aServer.send(aResponse, [&aServer, aResponse, aIfStream](const boost::system::error_code &ec) {
                if (!ec)
                {
                    DefaultResourceSend(aServer, aResponse, aIfStream);
                }
                else
                {
                    std::cerr << "Connection interrupted" << std::endl;
                }
            });
        }
    }
}

void WebServer::DefaultHttpResponse(void)
{
    mServer->default_resource[OT_REQUEST_METHOD_GET] = [this](std::shared_ptr<HttpServer::Response> response,
                                                              std::shared_ptr<HttpServer::Request>  request) {
        try
        {
            auto webRootPath = boost::filesystem::canonical(WEB_FILE_PATH);
            auto path        = boost::filesystem::canonical(webRootPath / request->path);

            // Check if path is within webRootPath
            if (std::distance(webRootPath.begin(), webRootPath.end()) > std::distance(path.begin(), path.end()) ||
                !std::equal(webRootPath.begin(), webRootPath.end(), path.begin()))
            {
                throw std::invalid_argument("path must be within root path");
            }
            if (boost::filesystem::is_directory(path))
            {
                path /= "index.html";
            }
            if (!(boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path)))
            {
                throw std::invalid_argument("file does not exist");
            }

            std::string cacheControl, etag;

            auto ifs = std::make_shared<std::ifstream>();
            ifs->open(path.string(), std::ifstream::in | std::ios::binary | std::ios::ate);
            std::string extension = boost::filesystem::extension(path.string());
            std::string header    = "";
            if (extension == ".css")
            {
                header = OT_RESPONSE_HEADER_CSS_TYPE;
            }
            else if (extension == ".html")
            {
                header = OT_RESPONSE_HEADER_TEXT_HTML_TYPE;
            }

            if (*ifs)
            {
                auto length = ifs->tellg();
                ifs->seekg(0, std::ios::beg);

                *response << OT_RESPONSE_SUCCESS_STATUS << cacheControl << etag << OT_RESPONSE_HEADER_LENGTH << length
                          << header << OT_RESPONSE_PLACEHOLD;

                DefaultResourceSend(*mServer, response, ifs);
            }
            else
            {
                throw std::invalid_argument("could not read file");
            }

        } catch (const std::exception &e)
        {
            std::string content = "Could not open path `" + request->path + "`: " + e.what();
            EscapeHtml(content);
            *response << OT_RESPONSE_FAILURE_STATUS << OT_RESPONSE_HEADER_LENGTH << content.length()
                      << OT_RESPONSE_PLACEHOLD << content;
        }
    };
}

std::string WebServer::HandleJoinNetworkRequest(const std::string &aJoinRequest, void *aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleJoinNetworkRequest(aJoinRequest);
}

std::string WebServer::HandleGetQRCodeRequest(const std::string &aGetQRCodeRequest, void *aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleGetQRCodeRequest(aGetQRCodeRequest);
}

std::string WebServer::HandleFormNetworkRequest(const std::string &aFormRequest, void *aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleFormNetworkRequest(aFormRequest);
}

std::string WebServer::HandleAddPrefixRequest(const std::string &aAddPrefixRequest, void *aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleAddPrefixRequest(aAddPrefixRequest);
}

std::string WebServer::HandleDeletePrefixRequest(const std::string &aDeletePrefixRequest, void *aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleDeletePrefixRequest(aDeletePrefixRequest);
}

std::string WebServer::HandleGetStatusRequest(const std::string &aGetStatusRequest, void *aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleGetStatusRequest(aGetStatusRequest);
}

std::string WebServer::HandleGetAvailableNetworkResponse(const std::string &aGetAvailableNetworkRequest,
                                                         void *             aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleGetAvailableNetworkResponse(aGetAvailableNetworkRequest);
}

std::string WebServer::HandleCommission(const std::string &aCommissionRequest, void *aUserData)
{
    WebServer *webServer = static_cast<WebServer *>(aUserData);

    return webServer->HandleCommission(aCommissionRequest);
}

void WebServer::ResponseJoinNetwork(void)
{
    HandleHttpRequest(OT_JOIN_NETWORK_PATH, OT_REQUEST_METHOD_POST, HandleJoinNetworkRequest);
}

void WebServer::ResponseGetQRCode(void)
{
    HandleHttpRequest(OT_GET_QRCODE_PATH, OT_REQUEST_METHOD_GET, HandleGetQRCodeRequest);
}

void WebServer::ResponseFormNetwork(void)
{
    HandleHttpRequest(OT_FORM_NETWORK_PATH, OT_REQUEST_METHOD_POST, HandleFormNetworkRequest);
}

void WebServer::ResponseAddOnMeshPrefix(void)
{
    HandleHttpRequest(OT_ADD_PREFIX_PATH, OT_REQUEST_METHOD_POST, HandleAddPrefixRequest);
}

void WebServer::ResponseDeleteOnMeshPrefix(void)
{
    HandleHttpRequest(OT_DELETE_PREFIX_PATH, OT_REQUEST_METHOD_POST, HandleDeletePrefixRequest);
}

void WebServer::ResponseGetStatus(void)
{
    HandleHttpRequest(OT_GET_NETWORK_PATH, OT_REQUEST_METHOD_GET, HandleGetStatusRequest);
}

void WebServer::ResponseGetAvailableNetwork(void)
{
    HandleHttpRequest(OT_AVAILABLE_NETWORK_PATH, OT_REQUEST_METHOD_GET, HandleGetAvailableNetworkResponse);
}

void WebServer::ResponseCommission(void)
{
    HandleHttpRequest(OT_COMMISSIONER_START_PATH, OT_REQUEST_METHOD_POST, HandleCommission);
}

std::string WebServer::HandleJoinNetworkRequest(const std::string &aJoinRequest)
{
    return mWpanService.HandleJoinNetworkRequest(aJoinRequest);
}

std::string WebServer::HandleGetQRCodeRequest(const std::string &aGetQRCodeRequest)
{
    OTBR_UNUSED_VARIABLE(aGetQRCodeRequest);
    return mWpanService.HandleGetQRCodeRequest();
}

std::string WebServer::HandleFormNetworkRequest(const std::string &aFormRequest)
{
    return mWpanService.HandleFormNetworkRequest(aFormRequest);
}

std::string WebServer::HandleAddPrefixRequest(const std::string &aAddPrefixRequest)
{
    return mWpanService.HandleAddPrefixRequest(aAddPrefixRequest);
}

std::string WebServer::HandleDeletePrefixRequest(const std::string &aDeletePrefixRequest)
{
    return mWpanService.HandleDeletePrefixRequest(aDeletePrefixRequest);
}

std::string WebServer::HandleGetStatusRequest(const std::string &aGetStatusRequest)
{
    OTBR_UNUSED_VARIABLE(aGetStatusRequest);
    return mWpanService.HandleStatusRequest();
}

std::string WebServer::HandleGetAvailableNetworkResponse(const std::string &aGetAvailableNetworkRequest)
{
    OTBR_UNUSED_VARIABLE(aGetAvailableNetworkRequest);
    return mWpanService.HandleAvailableNetworkRequest();
}

std::string WebServer::HandleCommission(const std::string &aCommissionRequest)
{
    return mWpanService.HandleCommission(aCommissionRequest);
}

} // namespace Web
} // namespace otbr
