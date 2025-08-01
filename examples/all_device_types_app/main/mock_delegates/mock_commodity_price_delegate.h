/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/commodity-price-server/commodity-price-server.h>

/*
 * Mock CommodityPrice Delegate Implementation
 * This file provides a mock implementation of the CommodityPrice::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/commodity-price-server/commodity-price-server.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace CommodityPrice {

class MockCommodityPriceDelegate : public Delegate {
public:
    MockCommodityPriceDelegate() = default;
    virtual ~MockCommodityPriceDelegate() = default;

    // Note: This delegate is mostly a placeholder as mentioned in the original header
    // There are no delegated methods since these are largely implemented in the
    // commodity-price-server.cpp Instance class
};

} // namespace CommodityPrice
} // namespace Clusters
} // namespace app
} // namespace chip