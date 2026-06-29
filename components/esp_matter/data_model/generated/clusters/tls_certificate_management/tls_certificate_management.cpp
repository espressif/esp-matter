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
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <tls_certificate_management.h>
#include <tls_certificate_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace tls_certificate_management {

namespace attribute {
attribute_t *create_max_root_certificates(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxRootCertificates::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(5)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_provisioned_root_certificates(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ProvisionedRootCertificates::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_max_client_certificates(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxClientCertificates::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(2)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_provisioned_client_certificates(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ProvisionedClientCertificates::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_provision_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvisionRootCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_provision_root_certificate_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvisionRootCertificateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_find_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindRootCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_find_root_certificate_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindRootCertificateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_lookup_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LookupRootCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_lookup_root_certificate_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LookupRootCertificateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveRootCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_client_csr(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ClientCSR::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_client_csr_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ClientCSRResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_provision_client_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvisionClientCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_find_client_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindClientCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_find_client_certificate_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindClientCertificateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_lookup_client_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LookupClientCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_lookup_client_certificate_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LookupClientCertificateResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_client_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveClientCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, tls_certificate_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, tls_certificate_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterTlsCertificateManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_root_certificates(cluster, config->max_root_certificates);
        attribute::create_max_client_certificates(cluster, config->max_client_certificates);
        attribute::create_provisioned_root_certificates(cluster, NULL, 0, 0);
        attribute::create_provisioned_client_certificates(cluster, NULL, 0, 0);
        command::create_provision_root_certificate(cluster);
        command::create_provision_root_certificate_response(cluster);
        command::create_find_root_certificate(cluster);
        command::create_find_root_certificate_response(cluster);
        command::create_lookup_root_certificate(cluster);
        command::create_lookup_root_certificate_response(cluster);
        command::create_remove_root_certificate(cluster);
        command::create_client_csr(cluster);
        command::create_client_csr_response(cluster);
        command::create_provision_client_certificate(cluster);
        command::create_find_client_certificate(cluster);
        command::create_find_client_certificate_response(cluster);
        command::create_lookup_client_certificate(cluster);
        command::create_lookup_client_certificate_response(cluster);
        command::create_remove_client_certificate(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterTlsCertificateManagementClusterServerInitCallback,
                                                 ESPMatterTlsCertificateManagementClusterServerShutdownCallback);
    }

    return cluster;
}

} /* tls_certificate_management */
} /* cluster */
} /* esp_matter */
