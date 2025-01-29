// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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
#include <esp_matter_console.h>
#include <esp_matter_rainmaker.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_user_mapping.h>
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

#define RAINMAKER_CLUSTER_REVISION 2

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

namespace command {
namespace configuration {
[[deprecated("Configuration command is deprecated and will be removed in future.")]]
static constexpr chip::CommandId Id = 0x00000000;
} /* configuration */
namespace sign_data {
static constexpr chip::CommandId Id = 0x00000001;
} /* sign_data */
} /* command */

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
#endif

static esp_err_t status_attribute_update(bool status)
{
    uint16_t endpoint_id = cluster::rainmaker::endpoint_id;
    uint32_t cluster_id = cluster::rainmaker::Id;
    uint32_t attribute_id = cluster::rainmaker::attribute::status::Id;
    esp_matter_attr_val_t val = esp_matter_bool(status);
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
    esp_matter_attr_val_t val = esp_matter_char_str(rmaker_node_id, strlen(rmaker_node_id));
    return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

static esp_err_t challenge_response_attribute_update(char *challenge_response)
{
    uint16_t endpoint_id = cluster::rainmaker::endpoint_id;
    uint32_t cluster_id = cluster::rainmaker::Id;
    uint32_t attribute_id = cluster::rainmaker::attribute::challenge_response::Id;
    esp_matter_attr_val_t val = esp_matter_char_str(challenge_response, strlen(challenge_response));
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

// RainMaker cluster, has two commands: "configuration" and "sign message."
// The "configuration" command has been deprecated, and the iOS app uses "sign message" command for user node
// association.
//
// As per Matter specification, the payload for an invoke command should be encapsulated in a TLV structure, and each
// argument should be encoded using a context-specific tag.
//
// However, the iOS app continues to send the payload without the TLV structure and context-specific tag. Future
// versions of iOS will support both methods of RainMaker user-node association and so, no specific action is required from
// firmware developers.
//
// On the other hand, the "config_command_callback()" correctly parses the payload. It initially extracts the TLV
// structure and then examines the context-specific tag before decoding the actual argument, which is of type octet
// string.
static esp_err_t config_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    /* Get ids */
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    /* Return if this is not the rainmaker configuration command */
    if (endpoint_id != cluster::rainmaker::endpoint_id || cluster_id != cluster::rainmaker::Id ||
        command_id != cluster::rainmaker::command::configuration::Id) {
        ESP_LOGE(TAG, "Got rainmaker command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "RainMaker configuration command callback");
    static int command_count = ESP_MATTER_RAINMAKER_COMMAND_LIMIT;
    if (command_count <= 0) {
        ESP_LOGE(TAG, "This command has reached a limit. Please reboot to try again.");
        return ESP_FAIL;
    }
    command_count--;

    /* Parse the tlv data */

    if (chip::TLV::kTLVType_Structure != tlv_data.GetType()) {
        return ESP_FAIL;
    }
    chip::TLV::TLVType mOuter;
    if (CHIP_NO_ERROR != tlv_data.EnterContainer(mOuter)) {
        return ESP_FAIL;
    }
    if (CHIP_NO_ERROR != tlv_data.Next()) {
        return ESP_FAIL;
    }
    chip::TLV::Tag tag = tlv_data.GetTag();
    if (!IsContextTag(tag)) {
        return ESP_FAIL;
    }
    // check tag number, since this has only one argument, checking against 0
    if (0 != TagNumFromTag(tag)) {
        return ESP_FAIL;
    }
    // decode the octet string argument
    chip::CharSpan config_value;
    if (CHIP_NO_ERROR != DataModel::Decode(tlv_data, config_value)) {
        return ESP_FAIL;
    }

    const char *data = config_value.data();
    int size = config_value.size();
    if (!data || size <= 0) {
        ESP_LOGE(TAG, "Command data not found or was not decoded correctly. The expected data is a string or the"
                 "format is \"<user_id>::<secret_key>\"");
        return ESP_FAIL;
    }

    if (CHIP_NO_ERROR != tlv_data.ExitContainer(mOuter)) {
        return ESP_FAIL;
    }

    /* The expected format of the data is "<user_id>::<secret_key>" */
    char ch = ':';
    char *check_first = (char *)memchr(data, (int)ch, size);
    char *check_second = NULL;
    if (check_first && (size >= (int)((check_first + 1) - data))) {
        check_second =  (char *)memchr(check_first + 1, (int)ch, size - (check_first - data + 1));
    }
    if (!check_first || !check_second) {
        ESP_LOGE(TAG, "\"::\" not found in the received data: %.*s. The expected format is \"<user_id>::<secret_key>\"",
                 size, data);
        return ESP_FAIL;
    }

    /* Get sizes */
    int user_id_index = 0;
    int user_id_len = (int)((char *)memchr(data, (int)ch, size) - data); /* (first ':') - (start of string) */
    int secret_key_index = (int)(&data[user_id_len] - data) + 2; /* (user id end) - (start of string) + 2 */
    int secret_key_len = size - secret_key_index;
    if (user_id_len <= 0 || user_id_len >= ESP_MATTER_RAINMAKER_MAX_DATA_LEN || secret_key_len <= 0 ||
        secret_key_len >= ESP_MATTER_RAINMAKER_MAX_DATA_LEN) {
        ESP_LOGE(TAG, "User id or secret key length invalid: user_id_len: %d, secret_key_len: %d, received_data: %.*s",
                 user_id_len, secret_key_len, size, data);
        return ESP_FAIL;
    }

    /* Copy the data. This done to make the strings NULL terminated. */
    char user_id[ESP_MATTER_RAINMAKER_MAX_DATA_LEN] = {0};
    char secret_key[ESP_MATTER_RAINMAKER_MAX_DATA_LEN] = {0};
    strncpy(user_id, &data[user_id_index], user_id_len);
    strncpy(secret_key, &data[secret_key_index], secret_key_len);
    ESP_LOGI(TAG, "user_id: %s, secret_key: %s", user_id, secret_key);

    /* Call the rainmaker API */
    if (strlen(user_id) > 0 && strlen(secret_key) > 0) {
        esp_rmaker_start_user_node_mapping(user_id, secret_key);
    }
    return ESP_OK;
}

static esp_err_t sign_and_update_challenge_response(chip::CharSpan challenge_span)
{
    if (challenge_span.size() > ESP_MATTER_RAINMAKER_MAX_CHALLENGE_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Copy the data. This is done to make the strings NULL terminated. */
    char challenge[ESP_MATTER_RAINMAKER_MAX_CHALLENGE_LEN] = {0};

    uint8_t bytes_to_copy = std::min(sizeof(challenge), challenge_span.size());
    strncpy(challenge, challenge_span.data(), bytes_to_copy);
    challenge[bytes_to_copy] = 0;
    ESP_LOGI(TAG, "challenge: %s", challenge);

    // sign the data here
    char *challenge_response = NULL;
    size_t outlen = 0;

    esp_err_t err = esp_rmaker_node_auth_sign_msg((void *)challenge, challenge_span.size(), (void **)&challenge_response, &outlen);
    if (err != ESP_OK) {
        return err;
    }

    err = challenge_response_attribute_update(challenge_response);
    free(challenge_response);
    return err;
}

static esp_err_t sign_data_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    /* Get ids */
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    /* Return if this is not the rainmaker sign_data command */
    if (endpoint_id != cluster::rainmaker::endpoint_id || cluster_id != cluster::rainmaker::Id ||
        command_id != cluster::rainmaker::command::sign_data::Id) {
        ESP_LOGE(TAG, "Got rainmaker command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "RainMaker sign_data command callback");
    static int command_count = ESP_MATTER_RAINMAKER_COMMAND_LIMIT;
    if (command_count <= 0) {
        ESP_LOGE(TAG, "This command has reached a limit. Please reboot to try again.");
        return ESP_FAIL;
    }
    command_count--;

    // In an invoke interaction, the payload should be enclosed within an anonymous struct,
    // and each argument must be specified using a context-specific tag.
    // The following code parses the data without context-specific tags, which is incorrect and needs correction.
    // TODO: CON-820

    /* Parse the tlv data */
    chip::CharSpan config_value;
    DataModel::Decode(tlv_data, config_value);
    if (!config_value.data() || config_value.size() <= 0) {
        ESP_LOGE(TAG, "Command data not found or was not decoded correctly. The expected data is a string or the"
                 "format is \"<data>\"");
        return ESP_FAIL;
    }

    return sign_and_update_challenge_response(config_value);
}

static esp_err_t custom_cluster_create()
{
    /* Get the endpoint */
    endpoint_t *endpoint = endpoint::get(cluster::rainmaker::endpoint_id);

    /* Create custom rainmaker cluster */
    cluster_t *cluster = esp_matter::cluster::create(endpoint, cluster::rainmaker::Id, CLUSTER_FLAG_SERVER);
    attribute::create(cluster, Globals::Attributes::ClusterRevision::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(RAINMAKER_CLUSTER_REVISION));

    /* Create custom status attribute */
    /* Update the value of the attribute after esp_rmaker_node_init() is done */
    attribute::create(cluster, cluster::rainmaker::attribute::status::Id, ATTRIBUTE_FLAG_NONE, esp_matter_bool(false));

    /* Create custom rmaker_node_id attribute */
    /* Update the value of the attribute after esp_rmaker_node_init() is done */
    char rmaker_node_id[ESP_MATTER_RAINMAKER_MAX_NODE_ID_LEN] = {0};
    attribute::create(cluster, cluster::rainmaker::attribute::rmaker_node_id::Id, ATTRIBUTE_FLAG_NONE,
                      esp_matter_char_str(rmaker_node_id, strlen(rmaker_node_id)), sizeof(rmaker_node_id));

    /* Create custom challenge_response attribute */
    /* Update the value of the attribute after sign_data command is called */
    char challenge_response[ESP_MATTER_RAINMAKER_MAX_CHALLENGE_RESPONSE_LEN] = {0};
    attribute::create(cluster, cluster::rainmaker::attribute::challenge_response::Id, ATTRIBUTE_FLAG_NONE,
                      esp_matter_char_str(challenge_response, strlen(challenge_response)), sizeof(challenge_response));

    /* Create custom challenge attribute */
    char challenge[ESP_MATTER_RAINMAKER_MAX_CHALLENGE_LEN] = {0};
    attribute::create(cluster, cluster::rainmaker::attribute::challenge::Id, ATTRIBUTE_FLAG_WRITABLE,
                      esp_matter_char_str(challenge, strlen(challenge)), sizeof(challenge));

    /* Create custom configuration command */
    command::create(cluster, cluster::rainmaker::command::configuration::Id,
                    COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM, config_command_callback);

    /* Create custom sign_data command */
    command::create(cluster, cluster::rainmaker::command::sign_data::Id,
                    COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM, sign_data_command_callback);

    return ESP_OK;
}

class RainmakerAttrAccess : public AttributeAccessInterface
{
public:
    // Register for the RainMaker cluster on endpoint 0.
    RainmakerAttrAccess() : AttributeAccessInterface(chip::Optional<chip::EndpointId>(cluster::rainmaker::endpoint_id),
            cluster::rainmaker::Id) {}

    CHIP_ERROR Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder) override
    {
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Write(const ConcreteDataAttributePath & aPath, AttributeValueDecoder & aDecoder) override
    {
        ConcreteDataAttributePath challengeAttrPath(cluster::rainmaker::endpoint_id, cluster::rainmaker::Id,
                                                        cluster::rainmaker::attribute::challenge::Id);

        if (challengeAttrPath.MatchesConcreteAttributePath(aPath))
        {
            chip::CharSpan challenge;
            CHIP_ERROR c_err = aDecoder.Decode(challenge);
            if (c_err != CHIP_NO_ERROR)
            {
                ESP_LOGE(TAG, "Failed to decode challenge, err:%" CHIP_ERROR_FORMAT, c_err.Format());
                return c_err;
            }

            return (ESP_OK == sign_and_update_challenge_response(challenge)) ? CHIP_NO_ERROR : CHIP_ERROR_INCORRECT_STATE;
        }

        return CHIP_NO_ERROR;
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
