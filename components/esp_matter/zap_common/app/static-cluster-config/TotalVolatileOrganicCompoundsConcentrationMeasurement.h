// Application configuration for Total Volatile Organic Compounds Concentration Measurement based on EMBER configuration
#pragma once

#include <app-common/zap-generated/cluster-enums.h>
#include <app/util/cluster-config.h>

#include <array>

namespace chip {
namespace app {
namespace Clusters {
namespace TotalVolatileOrganicCompoundsConcentrationMeasurement {
namespace StaticApplicationConfig {

using FeatureBitmapType = Clusters::StaticApplicationConfig::NoFeatureFlagsDefined;

inline constexpr std::array<Clusters::StaticApplicationConfig::ClusterConfiguration<FeatureBitmapType>, 0> kFixedClusterConfig = { };

} // namespace StaticApplicationConfig
} // namespace TotalVolatileOrganicCompoundsConcentrationMeasurement
} // namespace Clusters
} // namespace app
} // namespace chip
