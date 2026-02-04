#pragma once

#include <app/clusters/webrtc-transport-provider-server/WebRTCTransportProviderCluster.h>

namespace chip {
namespace app {
namespace Clusters {
namespace WebRTCTransportProvider {

/**
 * The application interface to define the options & implement commands.
 */
class WebRTCTransportProviderController {
public:
    virtual ~WebRTCTransportProviderController() = default;

    virtual void SetWebRTCTransportProvider(WebRTCTransportProviderCluster *webRTCTransportProvider) = 0;
};

} // namespace WebRTCTransportProvider
} // namespace Clusters
} // namespace app
} // namespace chip
