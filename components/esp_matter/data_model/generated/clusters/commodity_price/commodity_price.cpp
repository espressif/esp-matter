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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <commodity_price.h>
#include <commodity_price_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "commodity_price_cluster";
constexpr uint16_t cluster_revision = 4;

namespace esp_matter {
namespace cluster {
namespace commodity_price {

namespace feature {
namespace forecasting {
uint32_t get_id()
{
    return Forecasting::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_price_forecast(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* forecasting */

} /* feature */

namespace attribute {
attribute_t *create_tariff_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, TariffUnit::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

attribute_t *create_currency(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Currency::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_price(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentPrice::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_price_forecast(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(forecasting), NULL);
    return esp_matter::attribute::create(cluster, PriceForecast::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_get_detailed_price_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetDetailedPriceRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_detailed_price_response(cluster_t *cluster)
{
    VerifyOrReturnValue(has_command(GetDetailedPriceRequest, COMMAND_FLAG_ACCEPTED), NULL);
    return esp_matter::command::create(cluster, GetDetailedPriceResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_get_detailed_forecast_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetDetailedForecastRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_detailed_forecast_response(cluster_t *cluster)
{
    VerifyOrReturnValue(has_command(GetDetailedForecastRequest, COMMAND_FLAG_ACCEPTED), NULL);
    return esp_matter::command::create(cluster, GetDetailedForecastResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

namespace event {
event_t *create_price_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PriceChange::Id);
}

} /* event */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, commodity_price::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, commodity_price::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = CommodityPriceDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterCommodityPricePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_tariff_unit(cluster, 0);
        attribute::create_currency(cluster, NULL, 0, 0);
        attribute::create_current_price(cluster, NULL, 0, 0);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* commodity_price */
} /* cluster */
} /* esp_matter */
