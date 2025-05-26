// Application configuration for Time Synchronization based on EMBER configuration
#pragma once

#include <app-common/zap-generated/cluster-enums.h>
#include <app/util/cluster-config.h>

#include <array>

namespace chip {
namespace app {
namespace Clusters {
namespace TimeSynchronization {
namespace StaticApplicationConfig {

using FeatureBitmapType = Clusters::StaticApplicationConfig::NoFeatureFlagsDefined;

inline constexpr std::array<Clusters::StaticApplicationConfig::ClusterConfiguration<FeatureBitmapType>, 0> kFixedClusterConfig = { };

} // namespace StaticApplicationConfig
} // namespace TimeSynchronization
} // namespace Clusters
} // namespace app
} // namespace chip
