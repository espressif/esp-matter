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
from typing import Dict, List, Optional

from utils.conversion_utils import format_hex_value

from .elements import Cluster, Device

"""
Platform-specific cluster elements for each device type.
This list is used to define additional cluster elements that are specific to
ESP-Matter and are not included in the standard Matter Data Model XML
definitions.
List structure:
{
    "device_type_id": {
        "cluster_id": {
        "features": [feature_id, ...],
        "attributes": [attribute_id, ...],
        "commands": [command_id, ...],
        "events": [event_id, ...],
        },
    },
}
"""
device_type_element_overrides: Dict[str, Dict[str, Dict[str, List[str]]]] = {
    "0x0016": {  # root_node
        "0x0046": {  # icd_management
            "features": [
                "0x0004",  # long_idle_time_support
                "0x0001",  # check_in_protocol_support
                "0x0002",  # user_active_mode_trigger
            ],
        },
        "0x0031": {  # network_commissioning
            "features": [
                "0x0001",  # wifi_network_interface
                "0x0002",  # thread_network_interface
                "0x0004",  # ethernet_network_interface
            ],
        },
        "0x0036": {},  # wi_fi_network_diagnostics
        "0x0035": {},  # thread_network_diagnostics
        "0x0065": {},  # groupcast
    },
    "0x0019": {  # secondary_network_interface
        "0x0031": {  # network_commissioning
            "features": [
                "0x0001",  # wifi_network_interface
                "0x0002",  # thread_network_interface
                "0x0004",  # ethernet_network_interface
            ],
        }
    },
}

_ELEMENT_SECTIONS = {
    "features": ("features", "device_mandatory_features"),
    "attributes": ("attributes", "device_mandatory_attributes"),
    "commands": ("commands", "device_mandatory_commands"),
    "events": ("events", "device_mandatory_events"),
}


def _find_cluster_by_id(
    cluster_lookup_table: Dict[str, Cluster], cluster_id: str
) -> Optional[Cluster]:
    target_id = format_hex_value(cluster_id)
    for cluster in cluster_lookup_table.values():
        if format_hex_value(cluster.id) == target_id:
            return cluster
    return None


def _merge_elements(target: list, source: list, element_ids: List[str]) -> bool:
    seen = {element.func_name for element in target}
    merged = False
    for element_id in element_ids:
        target_id = format_hex_value(element_id)
        element = next(
            (item for item in source if format_hex_value(item.id) == target_id),
            None,
        )
        if element is not None and element.func_name not in seen:
            target.append(element)
            seen.add(element.func_name)
            merged = True
    return merged


def _create_device_cluster(cluster_obj: Cluster, *, is_device_extra: bool) -> Cluster:
    cluster = Cluster(
        name=cluster_obj.name,
        id=cluster_obj.id,
        revision=cluster_obj.revision,
        is_mandatory=False,
    )
    cluster.is_device_extra = is_device_extra
    cluster.server_cluster = True
    cluster.function_flags = "CLUSTER_FLAG_SERVER"
    return cluster


def _ensure_device_cluster(
    device: Device,
    clusters_by_id: Dict[str, Cluster],
    cluster_id: str,
    cluster_obj: Cluster,
) -> Cluster:
    formatted_id = format_hex_value(cluster_id)
    cluster = clusters_by_id.get(formatted_id)
    if cluster is not None:
        return cluster

    cluster = _create_device_cluster(cluster_obj, is_device_extra=True)
    device.extra_clusters.append(cluster)
    clusters_by_id[formatted_id] = cluster
    return cluster


def apply_device_type_element_overrides(
    device: Device, cluster_lookup_table: Dict[str, Cluster]
) -> None:
    """Merge platform-specific cluster overrides for a device type.

    Overrides merge features/attributes/commands/events into a cluster from the
    device-type XML when present, otherwise into a new optional extra cluster.
    """
    overrides = device_type_element_overrides.get(format_hex_value(device.id), {})
    if not overrides:
        return

    device.extra_clusters = []
    clusters_by_id = {
        format_hex_value(cluster.id): cluster for cluster in device.clusters
    }

    for cluster_id, element_override in overrides.items():
        cluster_obj = _find_cluster_by_id(cluster_lookup_table, cluster_id)
        if cluster_obj is None:
            continue

        cluster = _ensure_device_cluster(
            device, clusters_by_id, cluster_id, cluster_obj
        )
        merged = False
        for section, (source_attr, target_attr) in _ELEMENT_SECTIONS.items():
            element_ids = element_override.get(section, [])
            if not element_ids:
                continue
            if _merge_elements(
                getattr(cluster, target_attr),
                getattr(cluster_obj, source_attr),
                element_ids,
            ):
                merged = True

        if merged:
            if not cluster.is_device_extra:
                cluster.is_mandatory = True
            if cluster.device_mandatory_features and cluster_obj.has_choice_features():
                cluster.device_has_choice_features = True
        elif not cluster.is_mandatory and not cluster.is_device_extra:
            # Optional clusters from device-type XML
            # must be tracked in extra_clusters for template codegen.
            if cluster not in device.extra_clusters:
                device.extra_clusters.append(cluster)


def apply_device_type_overrides(
    device: Device, cluster_lookup_table: Dict[str, Cluster]
) -> None:
    """Apply all platform-specific device type overrides."""
    apply_device_type_element_overrides(device, cluster_lookup_table)
