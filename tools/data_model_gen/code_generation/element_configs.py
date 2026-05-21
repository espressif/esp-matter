# Copyright 2026 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from typing import Any, Dict, Optional

from utils.conversion_utils import convert_to_int, format_hex_value

"""
Mapping of ESP-Matter configuration macros to Matter data model elements.
This list associates ESP-Matter/Connectedhomeip specific configuration guard macros with
device types, clusters, and their corresponding features, attributes,
commands, and events. These macros can be used to conditionally enable
or exclude specific data model elements during build time.
List structure:
{
    "device_type_id": {
        "cluster_id": {
            "config_guard": "MACRO",
            "features": {
                "feature_id": {
                    "config_guard": "MACRO",
                },
            },
            "attributes": {
                "attribute_id": {
                    "config_guard": "MACRO",
                },
            },
            "commands": {
                "command_id": {
                    "config_guard": "MACRO",
                },
            },
            "events": {
                "event_id": {
                    "config_guard": "MACRO",
                },
            },
        },
    },
}
"""
cluster_config_overrides = {
    "0x0046": {  # icd_management
        "config_guard": "CHIP_CONFIG_ENABLE_ICD_SERVER",
        "features": {
            "0x0004": {  # long_idle_time_support
                "config_guard": "CHIP_CONFIG_ENABLE_ICD_LIT",
            },
            "0x0001": {  # check_in_protocol_support
                "config_guard": "CHIP_CONFIG_ENABLE_ICD_CIP",
            },
            "0x0002": {  # user_active_mode_trigger
                "config_guard": "CHIP_CONFIG_ENABLE_ICD_UAT",
            },
        },
    },
    "0x0031": {  # network_commissioning
        "config_guard": "!defined(CONFIG_CUSTOM_NETWORK_CONFIG)",
        "features": {
            "0x0001": {  # wifi_network_interface
                "config_guard": "CHIP_DEVICE_CONFIG_ENABLE_WIFI",
            },
            "0x0002": {  # thread_network_interface
                "config_guard": "CHIP_DEVICE_CONFIG_ENABLE_THREAD",
            },
            "0x0004": {  # ethernet_network_interface
                "config_guard": "CHIP_DEVICE_CONFIG_ENABLE_ETHERNET",
            },
        },
    },
    "0x0036": {  # wi_fi_network_diagnostics
        "config_guard": [
            "CHIP_DEVICE_CONFIG_ENABLE_WIFI",
            "defined(CONFIG_SUPPORT_WIFI_NETWORK_DIAGNOSTICS_CLUSTER)",
        ],
    },
    "0x0035": {  # thread_network_diagnostics
        "config_guard": [
            "CHIP_DEVICE_CONFIG_ENABLE_THREAD",
            "defined(CONFIG_SUPPORT_THREAD_NETWORK_DIAGNOSTICS_CLUSTER)",
        ],
    },
    "0x0065": {  # groupcast
        "config_guard": ["defined(CONFIG_SUPPORT_GROUPCAST_CLUSTER)"],
    },
    "0x0039": {  # bridged_device_basic_information
        "attributes": {
            "0x0012": {  # endpoint_unique_id
                "config_guard": "CHIP_CONFIG_USE_ENDPOINT_UNIQUE_ID",
            },
        },
    },
    "0x001D": {  # descriptor
        "attributes": {
            "0x0005": {  # endpoint_unique_id
                "config_guard": "CHIP_CONFIG_USE_ENDPOINT_UNIQUE_ID",
            },
        },
    },
    "0x001F": {  # access_control
        "attributes": {
            "0x0005": {  # commissioning_arl
                "config_guard": "CHIP_CONFIG_USE_ACCESS_RESTRICTIONS",
            },
            "0x0006": {  # arl
                "config_guard": "CHIP_CONFIG_USE_ACCESS_RESTRICTIONS",
            },
        },
        "commands": {
            "0x0000": {  # review_fabric_restrictions
                "config_guard": "CHIP_CONFIG_USE_ACCESS_RESTRICTIONS",
            },
        },
    },
}

_ELEMENT_SECTION = {
    "Feature": "features",
    "Attribute": "attributes",
    "Command": "commands",
    "Event": "events",
}


def _build_config_guard(entry: Optional[Dict[str, Any]]) -> Optional[str]:
    if not entry:
        return None
    guard = entry.get("config_guard")
    if isinstance(guard, list):
        return " && ".join(guard)
    if isinstance(guard, str):
        return guard
    if "config_macros" in entry:
        return " && ".join(entry["config_macros"])
    return None


def _lookup_section_entry(section: dict, element_id: str) -> Optional[dict]:
    target_id = convert_to_int(element_id)
    for key, value in section.items():
        if convert_to_int(key) == target_id:
            return value
    return None


def get_config_guard(
    cluster_id: str,
    element_type: str = "Cluster",
    element_id: Optional[str] = None,
) -> Optional[str]:
    """Get the preprocessor guard expression for a cluster or cluster element."""
    cluster_id = format_hex_value(cluster_id)
    cluster_entry = cluster_config_overrides.get(cluster_id)
    if not cluster_entry:
        return None

    if element_type == "Cluster":
        return _build_config_guard(cluster_entry)

    section_name = _ELEMENT_SECTION.get(element_type)
    if not section_name or element_id is None:
        return None

    entry = _lookup_section_entry(cluster_entry.get(section_name, {}), element_id)
    return _build_config_guard(entry)


def apply_cluster_special_configs(cluster) -> None:
    """Set special_config on a cluster and all of its child elements."""
    cluster.special_config = get_config_guard(cluster.id, "Cluster")
    for attr in cluster.attributes:
        attr.special_config = get_config_guard(cluster.id, "Attribute", attr.id)
    for cmd in cluster.commands:
        cmd.special_config = get_config_guard(cluster.id, "Command", cmd.id)
    for event in cluster.events:
        event.special_config = get_config_guard(cluster.id, "Event", event.id)
    for feature in cluster.features:
        feature.special_config = get_config_guard(cluster.id, "Feature", feature.id)


def apply_device_cluster_configs(device) -> None:
    """Apply cluster-level guards to device cluster stubs."""
    for cluster in list(device.clusters) + device.extra_clusters:
        if cluster.special_config is None:
            cluster.special_config = get_config_guard(cluster.id, "Cluster")
