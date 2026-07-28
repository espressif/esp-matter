/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "camera-device.h"
#include "webrtc-provider-manager.h"
#include <iomanip>
#include <jsmn.h>
#include <sstream>
#include <string.h>
#include <webrtc-kvs_esp_port_utils.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WebRTCTransportProvider;

using namespace Camera;

static const char * TAG = "webrtc-kvs_esp_port_utils";

extern CameraDevice gCameraDevice;

static std::atomic<unsigned int> peerConnectionCounter{ 0 }; // Starts from 0

std::string json_escape(const std::string  &input)
{
    std::string output;
    for (char c : input) {
        switch (c) {
        case '\"':
            output += "\\\"";
            break;
        case '\\':
            output += "\\\\";
            break;
        case '\b':
            output += "\\b";
            break;
        case '\f':
            output += "\\f";
            break;
        case '\n':
            output += "\\n";
            break;
        case '\r':
            output += "\\r";
            break;
        case '\t':
            output += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) {
                char buf[7];
                snprintf(buf, sizeof(buf), "\\u%04x", c);
                output += buf;
            } else {
                output += c;
            }
        }
    }
    return output;
}

std::string generateMonotonicPeerConnectionId()
{
    // Atomically get the current ID and increment it
    unsigned int id = peerConnectionCounter.fetch_add(1, std::memory_order_relaxed);

    // Convert the ID to a zero-padded 8-digit hex string
    std::ostringstream oss;
    oss << "0x" << std::setw(8) << std::setfill('0') << std::hex << id;

    return oss.str();
}