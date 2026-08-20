// Copyright 2023-2026 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <string.h>

#include <esp_matter.h>
#include <esp_matter_attribute.h>
#include <esp_matter_console.h>
#include <esp_matter_rainmaker.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_user_mapping.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <app/AttributeAccessInterface.h>
#include <app/AttributeAccessInterfaceRegistry.h>

#define ESP_MATTER_RAINMAKER_COMMAND_LIMIT              5 /* This command can be called 5 times per reboot */
#define ESP_MATTER_RAINMAKER_MAX_DATA_LEN               40
#define ESP_MATTER_RAINMAKER_MAX_CHALLENGE_LEN          40
#define ESP_MATTER_RAINMAKER_MAX_NODE_ID_LEN            40
#define ESP_MATTER_RAINMAKER_MAX_CHALLENGE_RESPONSE_LEN 150

using namespace chip::app;
using namespace chip::app::Clusters;

#define RAINMAKER_CLUSTER_REVISION 3

static const char *TAG = "esp_matter_rainmaker";

namespace esp_matter {

// RainMaker cluster data model definition
namespace cluster {
namespace rainmaker {
static constexpr chip::EndpointId endpoint_id = 0x00000000; /* Same as root node endpoint. This will always be
                                                            endpoint_id 0. */
static constexpr chip::ClusterId Id = 0x131BFC00; /* 0x131B == manufacturer code.
                                                     0xFCOO == start of manufacturer specific cluster_id */
namespace attribute {

namespace status {
static constexpr chip::AttributeId Id = 0x00000000;
} /* status */

namespace rmaker_node_id {
static constexpr chip::AttributeId Id = 0x00000001;
} /* rmaker_node_id */

// This attribute is of access type read-only.
namespace challenge_response {
static constexpr chip::AttributeId Id = 0x00000002;
} /* challenge_response */

// This attribute is of access type write.
// When a client writes to this attribute, it signs the incoming payload and
// subsequently stores that signature in the challenge_response attribute.
namespace challenge {
static constexpr chip::AttributeId Id = 0x00000003;
} /* challenge */

} /* attribute */

} /* rainmaker */
} /* cluster */

namespace rainmaker {

#if CONFIG_ENABLE_CHIP_SHELL
static esp_matter::console::engine rainmaker_console;

static esp_err_t console_add_user_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("%s: Invalid Usage.\n", TAG);
        return ESP_ERR_INVALID_ARG;
    }
    printf("%s: Starting user-node mapping\n", TAG);
    if (esp_rmaker_start_user_node_mapping(argv[0], argv[1]) != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t console_dispatch(int argc, char **argv)
{
    if (argc <= 0) {
        rainmaker_console.for_each_command(esp_matter::console::print_description, NULL);
        return ESP_OK;
    }
    return rainmaker_console.exec_command(argc, argv);
}

static void register_commands()
{
    static bool init_done = false;
    if (init_done) {
        return;
    }
    static const esp_matter::console::command_t command = {
        .name = "rainmaker",
        .description = "This can be used to run ESP RainMaker commands. ",
        .handler = console_dispatch,
    };

    static const esp_matter::console::command_t rainmaker_commands[] = {
        {
            .name = "add-user",
            .description = "Initiate ESP RainMaker User-Node mapping from the node. "
            "Usage: matter esp rainmaker add-user <user_id> <secret_key>",
            .handler = console_add_user_handler,
        },
    };
    rainmaker_console.register_commands(rainmaker_commands,
                                        sizeof(rainmaker_commands) / sizeof(esp_matter::console::command_t));
    esp_matter::console::add_commands(&command, 1);
    init_done = true;
}
#endif // CONFIG_ENABLE_CHIP_SHELL

static esp_err_t status_attribute_update(bool status)
{
    uint16_t endpoint_id = cluster::rainmaker::endpoint_id;
    uint32_t cluster_id = cluster::rainmaker::Id;
    uint32_t attribute_id = cluster::rainmaker::attribute::status::Id;
    esp_matter_attr_val_t val = esp_matter_attr_val(status);
    return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

static esp_err_t rmaker_node_id_attribute_update(char *rmaker_node_id)
{
    if (!rmaker_node_id) {
        return ESP_ERR_INVALID_ARG;
    }
    uint16_t endpoint_id = cluster::rainmaker::endpoint_id;
    uint32_t cluster_id = cluster::rainmaker::Id;
    uint32_t attribute_id = cluster::rainmaker::attribute::rmaker_node_id::Id;
    esp_matter_attr_val_t val = esp_matter_attr_val(rmaker_node_id, strlen(rmaker_node_id));
    return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

static esp_err_t challenge_response_attribute_update(char *challenge_response)
{
    uint16_t endpoint_id = cluster::rainmaker::endpoint_id;
    uint32_t cluster_id = cluster::rainmaker::Id;
    uint32_t attribute_id = cluster::rainmaker::attribute::challenge_response::Id;
    esp_matter_attr_val_t val = esp_matter_attr_val(challenge_response, strlen(challenge_response));
    return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

static void user_node_association_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                                                void *event_data)
{
    /* This event handler is only for user node association status */
    if (event_base == RMAKER_EVENT) {
        if (event_id == RMAKER_EVENT_USER_NODE_MAPPING_DONE) {
            ESP_LOGI(TAG, "User node association complete. Updating the status attribute.");
            status_attribute_update(true);
        } else if (event_id == RMAKER_EVENT_USER_NODE_MAPPING_RESET) {
            ESP_LOGI(TAG, "User node association reset. Updating the status attribute.");
            status_attribute_update(false);
        }
    }
}

static esp_err_t sign_node_id_and_update_challenge_response(chip::FabricIndex fabric_index)
{
    static int sign_count = ESP_MATTER_RAINMAKER_COMMAND_LIMIT;
    if (sign_count <= 0) {
        ESP_LOGE(TAG, "Signing limit reached. Please reboot to try again.");
        return ESP_FAIL;
    }

    // Never sign external/arbitrary data. Always sign the Matter operational node ID for the accessing fabric.
    const auto *fabric_info = chip::Server::GetInstance().GetFabricTable().FindFabricWithIndex(fabric_index);
    if (!fabric_info) {
        ESP_LOGE(TAG, "Fabric index %u not found", fabric_index);
        return ESP_ERR_INVALID_STATE;
    }

    chip::NodeId node_id = fabric_info->GetNodeId();
    ESP_LOGD(TAG, "fabric_index: %u, node_id: 0x" ChipLogFormatX64, fabric_index, ChipLogValueX64(node_id));

    char node_id_str[24];
    snprintf(node_id_str, sizeof(node_id_str), ChipLogFormatX64, ChipLogValueX64(node_id));
    ESP_LOGD(TAG, "Signing node_id_str: '%s' (len: %u)", node_id_str, (unsigned)strlen(node_id_str));

    sign_count--;

    char *challenge_response = NULL;
    size_t outlen = 0;

    esp_err_t err = esp_rmaker_node_auth_sign_msg((void *)node_id_str, strlen(node_id_str),
                                                  (void **)&challenge_response, &outlen);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to sign node_id: %s", node_id_str);
        return err;
    }

    if (!challenge_response) {
        return ESP_ERR_INVALID_STATE;
    }

    err = challenge_response_attribute_update(challenge_response);
    free(challenge_response);
    return err;
}

// This creates a server cluster
static esp_err_t custom_cluster_create()
{
    /* Get the endpoint */
    endpoint_t *endpoint = endpoint::get(cluster::rainmaker::endpoint_id);

    /* Create custom rainmaker cluster */
    cluster_t *cluster = esp_matter::cluster::create(endpoint, cluster::rainmaker::Id, CLUSTER_FLAG_SERVER);
    VerifyOrReturnError(cluster != NULL, ESP_FAIL,
                        ESP_LOGE(TAG, "Failed to create rainmaker cluster, id:0x%" PRIX32, cluster::rainmaker::Id));

    // global attributes
    cluster::global::attribute::create_cluster_revision(cluster, RAINMAKER_CLUSTER_REVISION);
    cluster::global::attribute::create_feature_map(cluster, 0); // There aren't any features for this cluster

    /* Create custom status attribute */
    /* Update the value of the attribute after esp_rmaker_node_init() is done */
    attribute::create(cluster, cluster::rainmaker::attribute::status::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(false));

    /* Create custom rmaker_node_id attribute */
    /* Update the value of the attribute after esp_rmaker_node_init() is done */
    char rmaker_node_id[ESP_MATTER_RAINMAKER_MAX_NODE_ID_LEN] = {0};
    attribute::create(cluster, cluster::rainmaker::attribute::rmaker_node_id::Id, ATTRIBUTE_FLAG_NONE,
                      esp_matter_attr_val(rmaker_node_id, strlen(rmaker_node_id)), sizeof(rmaker_node_id));

    /* Create custom challenge_response attribute */
    /* Update the value of the attribute after sign_data command is called */
    char challenge_response[ESP_MATTER_RAINMAKER_MAX_CHALLENGE_RESPONSE_LEN] = {0};
    attribute::create(cluster, cluster::rainmaker::attribute::challenge_response::Id, ATTRIBUTE_FLAG_NONE,
                      esp_matter_attr_val(challenge_response, strlen(challenge_response)), sizeof(challenge_response));

    /* Create custom challenge attribute */
    char challenge[ESP_MATTER_RAINMAKER_MAX_CHALLENGE_LEN] = {0};
    attribute::create(cluster, cluster::rainmaker::attribute::challenge::Id, ATTRIBUTE_FLAG_WRITABLE,
                      esp_matter_attr_val(challenge, strlen(challenge)), sizeof(challenge));

    return ESP_OK;
}

// Enable this to log the incoming challenge payload for debugging
#define LOG_INCOMING_PAYLOAD 0

class RainmakerAttrAccess : public AttributeAccessInterface {
public:
    // Register for the RainMaker cluster on endpoint 0.
    RainmakerAttrAccess() : AttributeAccessInterface(chip::Optional<chip::EndpointId>(cluster::rainmaker::endpoint_id),
                                                         cluster::rainmaker::Id) {}

    CHIP_ERROR Read(const ConcreteReadAttributePath  &aPath, AttributeValueEncoder  &aEncoder) override
    {
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Write(const ConcreteDataAttributePath  &aPath, AttributeValueDecoder  &aDecoder) override
    {
        ConcreteDataAttributePath challengeAttrPath(cluster::rainmaker::endpoint_id, cluster::rainmaker::Id,
                                                    cluster::rainmaker::attribute::challenge::Id);

        if (challengeAttrPath.MatchesConcreteAttributePath(aPath)) {
#if LOG_INCOMING_PAYLOAD
            chip::CharSpan incoming;
            CHIP_ERROR decode_err = aDecoder.Decode(incoming);
            if (decode_err == CHIP_NO_ERROR && incoming.data()) {
                ESP_LOGI(TAG, "Challenge write: incoming payload (%u bytes): %.*s",
                         (unsigned)incoming.size(), (int)incoming.size(), incoming.data());
            } else {
                ESP_LOGI(TAG, "Challenge write: no payload or decode failed");
            }
#endif // LOG_INCOMING_PAYLOAD
            chip::FabricIndex fabric_index = aDecoder.GetSubjectDescriptor().fabricIndex;
            return (ESP_OK == sign_node_id_and_update_challenge_response(fabric_index)) ? CHIP_NO_ERROR : CHIP_ERROR_INCORRECT_STATE;
        }

        return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
    }
};

RainmakerAttrAccess gAttrAccess;

esp_err_t init()
{
    /* Add custom rainmaker cluster */
#if CONFIG_ENABLE_CHIP_SHELL
    register_commands();
#endif
    esp_err_t err = custom_cluster_create();
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "Failed to create rainmaker cluster");
        return err;
    }

    AttributeAccessInterfaceRegistry::Instance().Register(&gAttrAccess);
    return ESP_OK;

}

esp_err_t start()
{
    /* Check user node association */
    if (esp_rmaker_user_node_mapping_get_state() == ESP_RMAKER_USER_MAPPING_DONE) {
        status_attribute_update(true);
    }

    /* Register an event handler and update the state later */
    esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_DONE, &user_node_association_event_handler,
                               NULL);
    esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_RESET,
                               &user_node_association_event_handler, NULL);

    /* Update rmaker_node_id */
    return rmaker_node_id_attribute_update(esp_rmaker_get_node_id());
}

} /* rainmaker */
} /* esp_matter */
