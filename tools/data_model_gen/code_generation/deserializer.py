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

import json
from typing import List, Dict, Any
import logging

from .elements import Attribute, Command, Event, Feature, Cluster, Device
from .conformance_codegen import FeatureConformance, Conformance
from .element_configs import apply_cluster_special_configs, apply_device_cluster_configs
from .element_overrides import apply_device_type_overrides

logger = logging.getLogger(__name__)


class ClusterDeserializer:
    """Deserializer for building cluster objects from JSON data"""

    def deserialize(self, json_path: str) -> List[Cluster]:
        """Deserialize clusters from cluster JSON file

        :param json_path: Path to the cluster JSON file
        :returns: List of Cluster objects
        """
        with open(json_path, "r") as f:
            cluster_data_list = json.load(f)

        clusters = [
            self._deserialize_single_cluster(cluster_data)
            for cluster_data in cluster_data_list
        ]

        return sorted(
            clusters, key=lambda x: (int(x.get_id(), 16), not x.server_cluster)
        )

    def _deserialize_single_cluster(self, cluster_data: Dict[str, Any]) -> Cluster:
        """Deserialize a single cluster from cluster JSON data"""
        logger.debug(f"Deserializing cluster: {cluster_data.get('name', 'Unknown')}")

        cluster = Cluster(
            name=cluster_data["name"],
            id=cluster_data["id"],
            revision=cluster_data["revision"],
            is_mandatory=cluster_data.get("is_mandatory", False),
        )

        self._set_cluster_properties(cluster, cluster_data)

        cluster.attributes = self._deserialize_attributes(
            cluster_data.get("attributes", [])
        )
        cluster.commands = self._deserialize_commands(cluster_data.get("commands", []))
        cluster.events = self._deserialize_events(cluster_data.get("events", []))
        cluster.features = self._deserialize_features(
            cluster_data.get("features", []), cluster
        )

        apply_cluster_special_configs(cluster)
        return cluster

    def _set_cluster_properties(
        self, cluster: Cluster, cluster_data: Dict[str, Any]
    ) -> None:
        """Set cluster properties"""
        cluster.role = cluster_data.get("classification", {}).get("role", "Unknown")
        cluster.server_cluster = cluster_data.get("type", "Unknown") == "Server"
        cluster.client_cluster = cluster_data.get("type", "Unknown") == "Client"
        cluster.callback_functions = cluster_data.get("callback_functions", [])
        cluster.function_flags = cluster_data.get("function_flags", "")
        cluster.delegate_init_callback = cluster_data.get(
            "delegate_init_callback", "None"
        )
        cluster.plugin_server_init_callback = cluster_data.get(
            "plugin_server_init_callback", ""
        )
        cluster.delegate_init_callback_available = bool(cluster.delegate_init_callback)
        cluster.is_migrated_cluster = cluster_data.get("is_migrated_cluster", False)
        cluster.is_derived_cluster = (
            cluster_data.get("classification", {}).get("hierarchy", "Unknown")
            == "derived"
        )

    def _deserialize_attributes(
        self, attributes_data: List[Dict[str, Any]]
    ) -> List[Attribute]:
        """Deserialize attributes from cluster JSON data"""
        attributes = []
        for attr_data in attributes_data:
            attr = Attribute(
                name=attr_data["name"],
                id=attr_data["id"],
                type_=attr_data["type"],
                is_mandatory=attr_data.get("mandatory", False),
                default_value=attr_data.get("default_value", "0"),
            )

            attr.converted_type = attr_data.get("converted_type", "")
            attr.is_nullable = attr_data.get("nullable", False)
            attr._flag = attr_data.get("flags", "")
            attr.max_length = attr_data.get("max_length", 0)
            attr.min_value = attr_data.get("min_value", "")
            attr.max_value = attr_data.get("max_value", "")
            attr.conformance = Conformance(attr_data.get("conformance", ""))
            attr.is_internally_managed = (
                "ATTRIBUTE_FLAG_MANAGED_INTERNALLY" in attr.get_flag()
            )
            if attr.type in ["string", "octstr", "struct", "list", "array"]:
                attr.is_complex = True

            attributes.append(attr)

        return attributes

    def _deserialize_commands(
        self, commands_data: List[Dict[str, Any]]
    ) -> List[Command]:
        """Deserialize commands from cluster JSON data"""
        commands = []
        for cmd_data in commands_data:
            cmd = Command(
                name=cmd_data["name"],
                id=cmd_data["id"],
                is_mandatory=cmd_data.get("mandatory", False),
                direction=cmd_data.get("direction", ""),
                response=self.get_command_response(cmd_data),
            )

            cmd._flag = cmd_data.get("flags", "")
            cmd.has_callback = cmd_data.get("callback_required", False)
            cmd.conformance = Conformance(cmd_data.get("conformance", ""))

            access_obj = cmd_data.get("access", "")
            if access_obj:
                cmd.is_fabric_scoped = access_obj.get("fabric_scoped", False)

            commands.append(cmd)

        return commands

    def get_command_response(self, cmd_data: Dict[str, Any]) -> str:
        """Get the response command for a given command"""
        if cmd_data.get("response") is None:
            return None
        if cmd_data.get("response") == "Y" or cmd_data.get("response") == "N":
            return None
        return cmd_data.get("response")

    def _deserialize_events(self, events_data: List[Dict[str, Any]]) -> List[Event]:
        """Deserialize events from cluster JSON data"""
        events = []
        for event_data in events_data:
            event = Event(
                name=event_data["name"],
                id=event_data["id"],
                is_mandatory=event_data.get("mandatory", False),
            )

            event.priority = event_data.get("priority", "Info")
            event.conformance = Conformance(event_data.get("conformance", ""))
            events.append(event)

        return events

    def _deserialize_features(
        self, features_data: List[Dict[str, Any]], cluster: Cluster
    ) -> List[Feature]:
        """Deserialize features from cluster JSON data"""
        features = []
        for feature_data in features_data:
            feature = Feature(
                name=feature_data["name"],
                id=feature_data["id"],
                code=feature_data.get("code", None),
                is_mandatory=feature_data.get("mandatory", False),
            )

            conformance_data = feature_data.get("conformance", "")
            if conformance_data is not None:
                feature.conformance = FeatureConformance(conformance=conformance_data)

            self._link_feature_attributes(
                feature, feature_data.get("attributes", []), cluster.attributes
            )
            self._link_feature_commands(
                feature, feature_data.get("commands", []), cluster.commands
            )
            self._link_feature_events(
                feature, feature_data.get("events", []), cluster.events
            )

            features.append(feature)

        return features

    def _link_feature_attributes(
        self,
        feature: Feature,
        attr_names: List[str],
        cluster_attributes: List[Attribute],
    ) -> None:
        """Replace attribute name list with actual attribute objects"""
        for attr_name in attr_names:
            attr = next((a for a in cluster_attributes if a.name == attr_name), None)
            if attr:
                feature.attributes.append(attr)
            else:
                logger.debug(
                    f"Attribute '{attr_name}' not found in cluster attributes may be deprecated"
                )

    def _link_feature_commands(
        self, feature: Feature, cmd_names: List[str], cluster_commands: List[Command]
    ) -> None:
        """Replace command name list with actual command objects"""
        for cmd_name in cmd_names:
            cmd = next((c for c in cluster_commands if c.name == cmd_name), None)
            if cmd:
                feature.commands.append(cmd)
            else:
                logger.debug(
                    f"Command '{cmd_name}' not found in cluster commands may be deprecated"
                )

    def _link_feature_events(
        self, feature: Feature, event_names: List[str], cluster_events: List[Event]
    ) -> None:
        """Replace event name list with actual event objects"""
        for event_name in event_names:
            event = next((e for e in cluster_events if e.name == event_name), None)
            if event:
                feature.events.append(event)
            else:
                logger.debug(
                    f"Event '{event_name}' not found in cluster events may be deprecated"
                )


class DeviceDeserializer:
    """Deserializer for building device objects from JSON data"""

    def deserialize(
        self, json_path: str, cluster_lookup_table: Dict[str, Cluster]
    ) -> List[Device]:
        """Deserialize devices from device JSON file

        :param json_path: Path to the device JSON file
        :param cluster_lookup_table: Dictionary containing cluster lookup table
        :returns: List of Device objects
        """
        with open(json_path, "r") as f:
            device_data_list = json.load(f)

        devices = []
        for device_data in device_data_list:
            device = self._deserialize_single_device(device_data, cluster_lookup_table)
            if device is not None:
                devices.append(device)
        return devices

    def _deserialize_single_device(
        self,
        device_data: Dict[str, Any],
        cluster_lookup_table: Dict[str, Cluster],
    ) -> Device:
        """Deserialize a single device from device JSON data.
        Returns None if any required cluster is missing from the lookup table.
        """
        device_name = device_data.get("name", "Unknown")
        logger.debug(f"Deserializing device: {device_name}")

        clusters, missing_clusters = self._deserialize_device_clusters(
            device_data.get("clusters", []), cluster_lookup_table
        )

        # Skip device if any cluster is missing (cluster XML not available)
        if missing_clusters:
            logger.warning(
                f"Skipping device '{device_name}': missing cluster(s) {missing_clusters}"
            )
            return None

        device = Device(
            id=device_data["id"],
            name=device_data["name"],
            revision=device_data["revision"],
        )

        device.clusters = clusters
        apply_device_type_overrides(device, cluster_lookup_table)
        apply_device_cluster_configs(device)
        return device

    def _deserialize_device_clusters(
        self,
        clusters_data: List[Dict[str, Any]],
        cluster_lookup_table: Dict[str, Cluster],
    ) -> tuple:
        """Deserialize clusters for a device from device JSON data.
        Returns a tuple of (clusters_list, missing_cluster_names).
        """
        clusters = []
        missing_clusters = []
        for cluster_data in clusters_data:
            cluster = Cluster(
                name=cluster_data["name"],
                id=cluster_data["id"],
                revision=cluster_data.get("revision", "0"),
                is_mandatory=cluster_data.get("is_mandatory", False),
            )

            cluster_obj = cluster_lookup_table.get(cluster.esp_name, None)

            # Set device-specific cluster properties
            cluster.function_flags = cluster_data.get("flags", "")
            cluster.server_cluster = cluster_data.get("type", "Unknown") == "server"
            cluster.client_cluster = cluster_data.get("type", "Unknown") == "client"

            cluster.optional_choice = cluster_data.get("optional_choice", None)
            if cluster_obj:
                cluster_attribute_names = cluster_data.get("attributes", [])
                cluster_feature_names = cluster_data.get("features", [])
                cluster_command_names = cluster_data.get("commands", [])
                cluster_event_names = cluster_data.get("events", [])
                cluster.device_has_choice_features = (
                    True
                    if len(cluster_feature_names) > 0
                    and cluster_obj.has_choice_features()
                    else False
                )
                cluster.device_mandatory_attributes = [
                    attribute
                    for attribute in cluster_obj.attributes
                    if attribute.func_name in cluster_attribute_names
                ]
                cluster.device_mandatory_features = [
                    feature
                    for feature in cluster_obj.features
                    if feature.func_name in cluster_feature_names
                    or feature.code.lower() in cluster_feature_names
                ]
                cluster.device_mandatory_commands = [
                    command
                    for command in cluster_obj.commands
                    if command.func_name in cluster_command_names
                ]
                cluster.device_mandatory_events = [
                    event
                    for event in cluster_obj.events
                    if event.func_name in cluster_event_names
                ]
                clusters.append(cluster)
            else:
                missing_clusters.append(cluster.esp_name)
        return clusters, missing_clusters
