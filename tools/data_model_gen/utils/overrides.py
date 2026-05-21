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
from typing import Dict, FrozenSet, Tuple

from utils.conversion_utils import convert_to_int


# ── Cluster-level skip / include sets (keyed by cluster ID) ─────────────────

COMMAND_CALLBACK_SKIP: FrozenSet[int] = frozenset(
    [
        0x0039,  # bridged_device_basic_information
        0x0450,  # network_identity_management
        0x0064,  # temperature_alarm
    ]
)

DELEGATE_CALLBACK_SKIP: FrozenSet[int] = frozenset(
    [
        0x0552,  # camera_av_settings_user_level_management
        0x0551,  # camera_av_stream_management
        0x0553,  # webrtc_transport_provider
        0x0554,  # webrtc_transport_requestor
        0x0801,  # tls_certificate_management
        0x0802,  # tls_client_management
        0x0550,  # zone_management
    ]
)

# TODO: To be removed once the delegate callback is present in the codebase
# connectedhomeip/src/app/clusters/mode-select/
DELEGATE_CALLBACK_INCLUDE: FrozenSet[int] = frozenset(
    [
        0x0050,  # mode_select
    ]
)

PLUGIN_CALLBACK_SKIP: FrozenSet[int] = frozenset(
    [
        0x0046,  # icd_management
    ]
)


# ── Cluster display name overrides ──────────────────────────────────────────
# (cluster_id, original_name_key, corrected_name)
_CLUSTER_NAME_OVERRIDE_DEFS = (
    (0x003E, "Node Operational Credentials", "Operational Credentials"),
    (0x0096, "Demand Response and Load Control", "Demand Response Load Control"),
    (0x0503, "WakeOnLAN", "Wake on LAN"),
    (0x0006, "OnOff", "On/Off"),
    (0x005D, "Dishwasher Alarm", "Dish Washer Alarm"),
    (0x0059, "Dishwasher Mode", "Dish Washer Mode"),
)

CLUSTER_NAME_OVERRIDES: Dict[int, str] = {
    cid: corrected for cid, _, corrected in _CLUSTER_NAME_OVERRIDE_DEFS
}

_CLUSTER_NAME_OVERRIDES_BY_NAME: Dict[str, str] = {
    orig: corrected for _, orig, corrected in _CLUSTER_NAME_OVERRIDE_DEFS
}


# ── Feature name overrides ──────────────────────────────────────────────────
# (cluster_id, feature_id, original_name, corrected_name)
_FEATURE_NAME_OVERRIDE_DEFS = (
    (0x0101, 0x0010, "WeekDayAccessSchedules", "weekday_access_schedules"),  # Door Lock
    (0x0202, 0x0002, "Auto", "fan_auto"),  # Fan Control
)

FEATURE_NAME_OVERRIDES: Dict[Tuple[int, int], str] = {
    (cid, fid): corrected for cid, fid, _, corrected in _FEATURE_NAME_OVERRIDE_DEFS
}

_FEATURE_NAME_OVERRIDES_BY_NAME: Dict[str, str] = {
    orig: corrected for _, _, orig, corrected in _FEATURE_NAME_OVERRIDE_DEFS
}


# ── Device type name overrides ──────────────────────────────────────────────
# (device_id, original_name, corrected_name)
_DEVICE_NAME_OVERRIDE_DEFS = ((0x0075, "Dishwasher", "Dish Washer"),)

DEVICE_NAME_OVERRIDES: Dict[int, str] = {
    did: corrected for did, _, corrected in _DEVICE_NAME_OVERRIDE_DEFS
}

_DEVICE_NAME_OVERRIDES_BY_NAME: Dict[str, str] = {
    orig: corrected for _, orig, corrected in _DEVICE_NAME_OVERRIDE_DEFS
}


# ── Element name overrides (attribute/command/event name corrections) ───────
# (cluster_id, element_id, original_name, corrected_name)
_ELEMENT_NAME_OVERRIDE_DEFS = (
    (
        0x0101,
        0x0033,
        "RequirePINforRemoteOperation",
        "require_pin_for_remote_operation",
    ),  # Door Lock
    (0x0099, 0x0026, "NextChargeTargetSoC", "next_charge_target_soc"),  # Energy EVSE
)

ELEMENT_NAME_OVERRIDES: Dict[Tuple[int, int], str] = {
    (cid, eid): corrected for cid, eid, _, corrected in _ELEMENT_NAME_OVERRIDE_DEFS
}

_ELEMENT_NAME_OVERRIDES_BY_NAME: Dict[str, str] = {
    orig: corrected for _, _, orig, corrected in _ELEMENT_NAME_OVERRIDE_DEFS
}


# ── Skip internally managed attribute flag ──────────────────────────────────
# cluster_id -> frozenset of attribute_ids to skip
SKIP_INTERNALLY_MANAGED_ATTRIBUTE_FLAG: Dict[int, FrozenSet[int]] = {
    0x0046: frozenset(  # icd_management
        [
            0x0006,  # user_active_mode_trigger_hint
            0x0007,  # user_active_mode_trigger_instruction
        ]
    ),
    0x0062: frozenset(  # scenes_management
        [
            0x0001,  # scene_table_size
        ]
    ),
    0x0201: frozenset(  # thermostat
        [
            0x0000,  # local_temperature
            0x001A,  # remote_sensing
        ]
    ),
    0x005B: frozenset(  # air_quality
        [
            0x0000,  # air_quality
        ]
    ),
}


# ── Cluster callback name overrides ─────────────────────────────────────────
# cluster_id -> (init_callback_name, shutdown_callback_name)
CLUSTER_CALLBACK_NAME_OVERRIDES: Dict[int, Tuple[str, str]] = {
    0x0553: (  # webrtc_transport_provider
        "ESPMatterWebRTCTransportProviderClusterServerInitCallback",
        "ESPMatterWebRTCTransportProviderClusterServerShutdownCallback",
    ),
    0x0554: (  # webrtc_transport_requestor
        "ESPMatterWebRTCTransportRequestorClusterServerInitCallback",
        "ESPMatterWebRTCTransportRequestorClusterServerShutdownCallback",
    ),
}


# ── C++ reserved words (name-based — not cluster/element related) ───────────
# can grow over time as we observe more reserved words in the codebase
_RESERVED_WORDS_OBSERVED_IN_PRACTICE: FrozenSet[str] = frozenset(
    word.lower()
    for word in [
        "auto",
        "switch",
    ]
)


# ── Public API ──────────────────────────────────────────────────────────────


def normalize_cluster_display_name(cluster_name: str, cluster_id: str = None) -> str:
    """Normalize a cluster display name. Uses ID-based lookup when cluster_id is provided,
    falls back to name-based lookup otherwise."""
    if cluster_id is not None:
        cid = convert_to_int(cluster_id)
        if cid in CLUSTER_NAME_OVERRIDES:
            return CLUSTER_NAME_OVERRIDES[cid]
    return _CLUSTER_NAME_OVERRIDES_BY_NAME.get(cluster_name, cluster_name)


def normalize_feature_name(
    feature_name: str, cluster_id: str = None, feature_id: str = None
) -> str:
    """Normalize a feature name. Uses ID-based lookup when both cluster_id and feature_id
    are provided, falls back to name-based lookup otherwise."""
    if cluster_id is not None and feature_id is not None:
        key = (convert_to_int(cluster_id), convert_to_int(feature_id))
        if key in FEATURE_NAME_OVERRIDES:
            return FEATURE_NAME_OVERRIDES[key]
    return _FEATURE_NAME_OVERRIDES_BY_NAME.get(feature_name, feature_name)


def normalize_device_type_name(device_type_name: str, device_id: str = None) -> str:
    """Normalize a device type name. Uses ID-based lookup when device_id is provided,
    falls back to name-based lookup otherwise."""
    if device_id is not None:
        did = convert_to_int(device_id)
        if did in DEVICE_NAME_OVERRIDES:
            return DEVICE_NAME_OVERRIDES[did]
    return _DEVICE_NAME_OVERRIDES_BY_NAME.get(device_type_name, device_type_name)


def normalize_element_name(
    element_name: str, cluster_id: str = None, element_id: str = None
) -> str:
    """Normalize an element name. Uses ID-based lookup when both cluster_id and element_id
    are provided, falls back to name-based lookup otherwise."""
    if cluster_id is not None and element_id is not None:
        key = (convert_to_int(cluster_id), convert_to_int(element_id))
        if key in ELEMENT_NAME_OVERRIDES:
            return ELEMENT_NAME_OVERRIDES[key]
    return _ELEMENT_NAME_OVERRIDES_BY_NAME.get(element_name, element_name)


def is_cpp_reserved_word(cpp_name: str) -> bool:
    return cpp_name.lower() in _RESERVED_WORDS_OBSERVED_IN_PRACTICE


def should_skip_cluster_command_callbacks(cluster_id: str) -> bool:
    return convert_to_int(cluster_id) in COMMAND_CALLBACK_SKIP


def should_skip_delegate_callback(cluster_id: str) -> bool:
    return convert_to_int(cluster_id) in DELEGATE_CALLBACK_SKIP


def should_include_delegate_callback(cluster_id: str) -> bool:
    return convert_to_int(cluster_id) in DELEGATE_CALLBACK_INCLUDE


def should_skip_plugin_callback(cluster_id: str) -> bool:
    return convert_to_int(cluster_id) in PLUGIN_CALLBACK_SKIP


def should_skip_internally_managed_flag(cluster_id: str, attribute_id: str) -> bool:
    cid = convert_to_int(cluster_id)
    if cid not in SKIP_INTERNALLY_MANAGED_ATTRIBUTE_FLAG:
        return False
    return convert_to_int(attribute_id) in SKIP_INTERNALLY_MANAGED_ATTRIBUTE_FLAG[cid]


def get_overridden_cluster_init_callback_name(cluster_id: str, chip_name: str) -> str:
    cid = convert_to_int(cluster_id)
    entry = CLUSTER_CALLBACK_NAME_OVERRIDES.get(cid)
    if entry:
        return entry[0]
    return f"ESPMatter{chip_name}ClusterServerInitCallback"


def get_overridden_cluster_shutdown_callback_name(
    cluster_id: str, chip_name: str
) -> str:
    cid = convert_to_int(cluster_id)
    entry = CLUSTER_CALLBACK_NAME_OVERRIDES.get(cid)
    if entry:
        return entry[1]
    return f"ESPMatter{chip_name}ClusterServerShutdownCallback"
