// Application configuration for User Label based on EMBER configuration
#pragma once

#include <app-common/zap-generated/cluster-enums.h>
#include <app/util/cluster-config.h>

#include <array>

namespace chip {
namespace app {
namespace Clusters {
namespace UserLabel {
namespace StaticApplicationConfig {

using FeatureBitmapType = Clusters::StaticApplicationConfig::NoFeatureFlagsDefined;

inline constexpr std::array<Clusters::StaticApplicationConfig::ClusterConfiguration<FeatureBitmapType>, 0> kFixedClusterConfig = { };

} // namespace StaticApplicationConfig
} // namespace UserLabel
} // namespace Clusters
} // namespace app
} // namespace chip
