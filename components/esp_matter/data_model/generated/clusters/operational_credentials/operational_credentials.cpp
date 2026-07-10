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
#include <operational_credentials.h>
#include <operational_credentials_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace operational_credentials {

namespace attribute {
attribute_t *create_no_cs(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NOCs::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_fabrics(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Fabrics::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_supported_fabrics(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SupportedFabrics::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(SupportedFabrics::Min)), esp_matter_attr_val(static_cast<uint8_t>(SupportedFabrics::Max)));
    return attribute;
}

attribute_t *create_commissioned_fabrics(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CommissionedFabrics::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_trusted_root_certificates(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, TrustedRootCertificates::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_fabric_index(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CurrentFabricIndex::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_attestation_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AttestationRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_attestation_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AttestationResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_certificate_chain_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CertificateChainRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_certificate_chain_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CertificateChainResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_csr_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CSRRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_csr_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CSRResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_add_noc(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddNOC::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_update_noc(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateNOC::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_noc_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NOCResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_update_fabric_label(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateFabricLabel::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_fabric(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveFabric::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_trusted_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddTrustedRootCertificate::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_vid_verification_statement(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetVIDVerificationStatement::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_sign_vid_verification_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SignVIDVerificationRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_sign_vid_verification_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SignVIDVerificationResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, operational_credentials::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, operational_credentials::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOperationalCredentialsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_supported_fabrics(cluster, config->supported_fabrics);
        attribute::create_commissioned_fabrics(cluster, config->commissioned_fabrics);
        attribute::create_current_fabric_index(cluster, config->current_fabric_index);
        attribute::create_no_cs(cluster, NULL, 0, 0);
        attribute::create_fabrics(cluster, NULL, 0, 0);
        attribute::create_trusted_root_certificates(cluster, NULL, 0, 0);
        command::create_attestation_request(cluster);
        command::create_attestation_response(cluster);
        command::create_certificate_chain_request(cluster);
        command::create_certificate_chain_response(cluster);
        command::create_csr_request(cluster);
        command::create_csr_response(cluster);
        command::create_add_noc(cluster);
        command::create_update_noc(cluster);
        command::create_noc_response(cluster);
        command::create_update_fabric_label(cluster);
        command::create_remove_fabric(cluster);
        command::create_add_trusted_root_certificate(cluster);
        command::create_set_vid_verification_statement(cluster);
        command::create_sign_vid_verification_request(cluster);
        command::create_sign_vid_verification_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterOperationalCredentialsClusterServerInitCallback,
                                                 ESPMatterOperationalCredentialsClusterServerShutdownCallback);
    }

    return cluster;
}

} /* operational_credentials */
} /* cluster */
} /* esp_matter */
