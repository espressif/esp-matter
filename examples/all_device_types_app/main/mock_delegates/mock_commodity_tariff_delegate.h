/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/commodity-tariff-server/commodity-tariff-server.h>

/*
 * Mock CommodityTariff Delegate Implementation
 * This file provides a mock implementation of the CommodityTariff::Delegate interface.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/commodity-tariff-server/commodity-tariff-server.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace CommodityTariff {

class MockCommodityTariffDelegate : public Delegate {
public:
    MockCommodityTariffDelegate() = default;
    virtual ~MockCommodityTariffDelegate() = default;

    // Note: This delegate is mostly a placeholder.
    // The Delegate class has no pure virtual methods - all functionality is
    // implemented in the base class with attribute accessors and setters.

private:
    static constexpr const char * LOG_TAG = "MockCommodityTariffDelegate";
};

} // namespace CommodityTariff
} // namespace Clusters
} // namespace app
} // namespace chip
