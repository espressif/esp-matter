// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Upstream generates this header per application from its ZAP file, describing the
// statically configured cluster instances. esp-matter only uses dynamic endpoints,
// so the fixed configuration is empty and every attribute/command may be enabled
// at runtime.

#pragma once

#include <app/util/cluster-config.h>
#include <clusters/Thermostat/AttributeIds.h>
#include <clusters/Thermostat/CommandIds.h>
#include <clusters/Thermostat/Enums.h>

#include <array>

namespace chip {
namespace app {
namespace Clusters {
namespace Thermostat {
namespace StaticApplicationConfig {

using FeatureBitmapType = Feature;

inline constexpr std::array<Clusters::StaticApplicationConfig::ClusterConfiguration<FeatureBitmapType>, 0>
kFixedClusterConfig = {};

inline constexpr bool IsAttributeEnabledOnSomeEndpoint(AttributeId attributeId)
{
    return true;
}

inline constexpr bool IsCommandEnabledOnSomeEndpoint(CommandId commandId)
{
    return true;
}

} // namespace StaticApplicationConfig
} // namespace Thermostat
} // namespace Clusters
} // namespace app
} // namespace chip
