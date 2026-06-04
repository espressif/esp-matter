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
"""
Serializer classes to convert source parser elements to dictionary representations.
"""

import logging
from utils.helper import safe_get_attr

logger = logging.getLogger(__name__)


class DataTypeSerializer:
    @staticmethod
    def to_dict(data_types):
        parsed = {}
        for data_type, data_type_list in data_types.items():
            type_list = []
            for data_type_name, data_type_object in data_type_list.items():
                type_list.append(data_type_object.to_dict())
            parsed[data_type] = type_list
        return parsed


class AttributeSerializer:
    @staticmethod
    def to_dict(attr, attribute_map=None):
        return {
            "name": attr.name,
            "id": attr.get_id(),
            "type": safe_get_attr(attr, "type"),
            "converted_type": attr.get_type(),
            "default_value": attr.get_default_value(),
            "mandatory": safe_get_attr(attr, "is_mandatory"),
            "nullable": safe_get_attr(attr, "is_nullable"),
            "flags": attr.get_flag(),
            "max_length": safe_get_attr(attr, "max_length"),
            "min_value": attr.min_value,
            "max_value": attr.max_value,
            "conformance": (
                safe_get_attr(attr, "conformance").to_dict(attribute_map=attribute_map)
                if safe_get_attr(attr, "conformance")
                and safe_get_attr(safe_get_attr(attr, "conformance"), "to_dict")
                else safe_get_attr(attr, "conformance")
            ),
            "constraint": (
                safe_get_attr(attr, "constraint").to_dict()
                if safe_get_attr(attr, "constraint")
                and safe_get_attr(safe_get_attr(attr, "constraint"), "to_dict")
                else safe_get_attr(attr, "constraint")
            ),
            "access": AccessSerializer.to_dict(safe_get_attr(attr, "access")),
            "quality": QualitySerializer.to_dict(safe_get_attr(attr, "quality")),
        }


class CommandSerializer:
    @staticmethod
    def to_dict(cmd, attribute_map=None):
        return {
            "name": cmd.name,
            "id": cmd.get_id(),
            "direction": safe_get_attr(cmd, "direction"),
            "response": safe_get_attr(cmd, "response"),
            "mandatory": safe_get_attr(cmd, "is_mandatory"),
            "flags": cmd.get_flag(),
            "callback_required": cmd.callback_required(),
            "conformance": (
                safe_get_attr(cmd, "conformance").to_dict(attribute_map=attribute_map)
                if safe_get_attr(cmd, "conformance")
                and safe_get_attr(safe_get_attr(cmd, "conformance"), "to_dict")
                else safe_get_attr(cmd, "conformance")
            ),
            "fields": [
                CommandFieldSerializer.to_dict(field)
                for field in safe_get_attr(cmd, "fields", [])
            ],
            "access": CommandAccessSerializer.to_dict(safe_get_attr(cmd, "access")),
        }


class EventSerializer:
    @staticmethod
    def to_dict(event, attribute_map=None):
        return {
            "name": event.name,
            "id": event.get_id(),
            "mandatory": safe_get_attr(event, "is_mandatory"),
            "conformance": (
                safe_get_attr(event, "conformance").to_dict(attribute_map=attribute_map)
                if safe_get_attr(event, "conformance")
                and safe_get_attr(safe_get_attr(event, "conformance"), "to_dict")
                else safe_get_attr(event, "conformance")
            ),
        }


class FeatureSerializer:
    @staticmethod
    def to_dict(feature, attribute_map=None):
        return {
            "name": feature.name,
            "id": feature.get_id(),
            "code": safe_get_attr(feature, "code"),
            "summary": safe_get_attr(feature, "summary"),
            "conformance": (
                safe_get_attr(feature, "conformance").to_dict(
                    attribute_map=attribute_map
                )
                if safe_get_attr(feature, "conformance")
                and safe_get_attr(safe_get_attr(feature, "conformance"), "to_dict")
                else safe_get_attr(feature, "conformance")
            ),
            "attributes": [
                attr.name
                for attr in sorted(
                    (
                        feature.get_attributes()
                        if safe_get_attr(feature, "get_attributes")
                        else safe_get_attr(feature, "attributes", [])
                    ),
                    key=lambda attr: (
                        safe_get_attr(attr, "id", 0),
                        safe_get_attr(attr, "name", ""),
                    ),
                )
            ],
            "commands": [
                cmd.name
                for cmd in sorted(
                    (
                        feature.get_commands()
                        if safe_get_attr(feature, "get_commands")
                        else safe_get_attr(feature, "commands", [])
                    ),
                    key=lambda cmd: (
                        safe_get_attr(cmd, "id", 0),
                        safe_get_attr(cmd, "name", ""),
                    ),
                )
            ],
            "events": [
                event.name
                for event in sorted(
                    (
                        feature.get_events()
                        if safe_get_attr(feature, "get_events")
                        else safe_get_attr(feature, "events", [])
                    ),
                    key=lambda event: (
                        safe_get_attr(event, "id", 0),
                        safe_get_attr(event, "name", ""),
                    ),
                )
            ],
        }


class AccessSerializer:
    @staticmethod
    def to_dict(access):
        if not access:
            return None

        return {
            "read": safe_get_attr(access, "read"),
            "write": safe_get_attr(access, "write"),
            "readPrivilege": safe_get_attr(access, "readPrivilege"),
            "writePrivilege": safe_get_attr(access, "writePrivilege"),
        }


class CommandAccessSerializer:
    @staticmethod
    def to_dict(access):
        if not access:
            return None

        return {
            "invokePrivilege": safe_get_attr(access, "invokePrivilege"),
            "timed": safe_get_attr(access, "timed"),
            "fabric_scoped": safe_get_attr(access, "fabric_scoped"),
        }


class QualitySerializer:
    @staticmethod
    def to_dict(quality):
        if not quality:
            return None

        return {
            "scene": safe_get_attr(quality, "scene"),
            "persistence": safe_get_attr(quality, "persistence"),
            "nullable": safe_get_attr(quality, "nullable"),
            "reportable": safe_get_attr(quality, "reportable"),
            "changeOmitted": safe_get_attr(quality, "changeOmitted"),
            "sourceAttribution": safe_get_attr(quality, "sourceAttribution"),
            "quieterReporting": safe_get_attr(quality, "quieterReporting"),
        }


class ClusterSerializer:
    @staticmethod
    def to_dict(cluster):
        attribute_map = {}
        for attr in cluster.get_attributes():
            attribute_map[attr.name] = attr.get_id()

        command_map = {}
        for cmd in cluster.get_commands():
            command_map[cmd.name] = (cmd.get_id(), cmd.get_flag())

        reference_map = {**attribute_map, **command_map}

        return {
            "name": cluster.name,
            "id": cluster.get_id(),
            "revision": cluster.get_revision(),
            "revision_history": safe_get_attr(cluster, "revision_history"),
            "classification": {
                "hierarchy": safe_get_attr(cluster, "hierarchy"),
                "role": safe_get_attr(cluster, "role"),
                "picsCode": safe_get_attr(cluster, "pics_code"),
                "scope": safe_get_attr(cluster, "scope"),
                "baseCluster": safe_get_attr(cluster, "base_cluster_name"),
            },
            "callback_functions": cluster.get_callback_functions(),
            "function_flags": cluster.get_function_flags(),
            "bound_callback_available": safe_get_attr(
                cluster, "bound_callback_available"
            ),
            "delegate_init_callback": cluster.get_delegate_init_callback(),
            "plugin_server_init_callback": cluster.get_plugin_server_init_callback(),
            "is_migrated_cluster": safe_get_attr(cluster, "is_migrated_cluster"),
            "data_types": DataTypeSerializer.to_dict(cluster.data_types),
            "attributes": [
                AttributeSerializer.to_dict(attr, reference_map)
                for attr in sorted(
                    (
                        cluster.get_attributes()
                        if safe_get_attr(cluster, "get_attributes")
                        else safe_get_attr(cluster, "attributes", [])
                    ),
                    key=lambda attr: (
                        safe_get_attr(attr, "id", 0),
                        safe_get_attr(attr, "name", ""),
                    ),
                )
            ],
            "commands": [
                CommandSerializer.to_dict(cmd, reference_map)
                for cmd in sorted(
                    (
                        cluster.get_commands()
                        if safe_get_attr(cluster, "get_commands")
                        else safe_get_attr(cluster, "commands", [])
                    ),
                    key=lambda cmd: (
                        safe_get_attr(cmd, "id", 0),
                        safe_get_attr(cmd, "name", ""),
                    ),
                )
            ],
            "events": [
                EventSerializer.to_dict(event, reference_map)
                for event in sorted(
                    (
                        cluster.get_events()
                        if safe_get_attr(cluster, "get_events")
                        else safe_get_attr(cluster, "events", [])
                    ),
                    key=lambda event: (
                        safe_get_attr(event, "id", 0),
                        safe_get_attr(event, "name", ""),
                    ),
                )
            ],
            "features": [
                FeatureSerializer.to_dict(feature, reference_map)
                for feature in sorted(
                    (
                        cluster.get_features()
                        if safe_get_attr(cluster, "get_features")
                        else safe_get_attr(cluster, "features", [])
                    ),
                    key=lambda feature: (
                        safe_get_attr(feature, "id", 0),
                        safe_get_attr(feature, "name", ""),
                    ),
                )
            ],
        }


class DeviceSerializer:
    @staticmethod
    def to_dict(device):
        def serialize_cluster(cluster):
            cluster_dict = {
                "name": cluster.name,
                "id": cluster.get_id(),
                "is_mandatory": safe_get_attr(cluster, "is_mandatory", False),
                "type": (
                    "server"
                    if safe_get_attr(cluster, "server_cluster")
                    else (
                        "client" if safe_get_attr(cluster, "client_cluster") else None
                    )
                ),
                "flags": cluster.get_function_flags(),
                "features": safe_get_attr(cluster, "feature_name_list", []),
                "commands": safe_get_attr(cluster, "command_name_list", []),
                "attributes": safe_get_attr(cluster, "attribute_name_list", []),
                "events": safe_get_attr(cluster, "event_name_list", []),
            }
            # Include optional_choice if present (O.a+ clusters)
            optional_choice = safe_get_attr(cluster, "optional_choice")
            if optional_choice is not None:
                cluster_dict["optional_choice"] = optional_choice
            return cluster_dict

        result = {
            "name": safe_get_attr(device, "name"),
            "id": device.get_id(),
            "revision": safe_get_attr(device, "revision"),
            "classification": safe_get_attr(device, "classification", {}),
            "revision_history": safe_get_attr(device, "revision_history", []),
            "conditions": safe_get_attr(device, "conditions", []),
            "clusters": [
                serialize_cluster(cluster)
                for cluster in (
                    device.get_clusters()
                    if safe_get_attr(device, "get_clusters")
                    else safe_get_attr(device, "clusters", [])
                )
            ],
        }
        return result


class CommandFieldSerializer:
    @staticmethod
    def to_dict(field):
        if not field:
            logger.debug("No field element found")
            return None

        result = {
            "id": safe_get_attr(field, "id"),
            "name": safe_get_attr(field, "name"),
            "type": safe_get_attr(field, "type"),
            "mandatory": safe_get_attr(field, "is_mandatory"),
        }

        default_value = safe_get_attr(field, "default_value")
        if default_value:
            result["default_value"] = default_value

        constraint = safe_get_attr(field, "constraint")
        if constraint:
            if isinstance(constraint, dict):
                result["constraint"] = constraint
            elif safe_get_attr(constraint, "to_dict"):
                result["constraint"] = constraint.to_dict()

        return result
