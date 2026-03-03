/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/application-basic-server/application-basic-delegate.h>

/*
 * Mock ApplicationBasic Delegate Implementation
 * This file provides a mock implementation of the ApplicationBasic::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/application-basic-server/application-basic-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/application-basic/ApplicationBasicManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/application-basic/ApplicationBasicManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ApplicationBasic {

class MockApplicationBasicDelegate : public Delegate {
public:
    MockApplicationBasicDelegate() : Delegate() {}
    MockApplicationBasicDelegate(uint16_t szCatalogVendorId, const char * szApplicationId)
        : Delegate(szCatalogVendorId, szApplicationId) {}
    virtual ~MockApplicationBasicDelegate() = default;

    // Application basic attribute handlers
    CHIP_ERROR HandleGetVendorName(app::AttributeValueEncoder  &aEncoder) override;
    uint16_t HandleGetVendorId() override;
    CHIP_ERROR HandleGetApplicationName(app::AttributeValueEncoder  &aEncoder) override;
    uint16_t HandleGetProductId() override;
    CHIP_ERROR HandleGetApplicationVersion(app::AttributeValueEncoder  &aEncoder) override;
    CHIP_ERROR HandleGetAllowedVendorList(app::AttributeValueEncoder  &aEncoder) override;
    std::list<uint16_t> GetAllowedVendorList() override;

private:
    static constexpr const char * LOG_TAG = "MockApplicationBasicDelegate";
};

} // namespace ApplicationBasic
} // namespace Clusters
} // namespace app
} // namespace chip