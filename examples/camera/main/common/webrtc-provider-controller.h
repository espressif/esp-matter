#pragma once

#include <app/clusters/webrtc-transport-provider-server/webrtc-transport-provider-server.h>

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

  virtual void
  SetWebRTCTransportProvider(std::unique_ptr<WebRTCTransportProviderServer>
                                 webRTCTransportProvider) = 0;
};

} // namespace WebRTCTransportProvider
} // namespace Clusters
} // namespace app
} // namespace chip
