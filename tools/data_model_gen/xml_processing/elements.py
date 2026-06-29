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
import logging
from dataclasses import dataclass
from typing import List, Any
from .attribute_type import AttributeType
from utils.base_elements import (
    BaseDevice,
    BaseEvent,
    BaseFeature,
    BaseCommand,
    BaseAttribute,
    BaseCluster,
)
from utils.overrides import (
    should_skip_delegate_callback,
    should_skip_plugin_callback,
    should_include_delegate_callback,
)
from utils.conversion_utils import convert_to_int
from .serializers import (
    DeviceSerializer,
    EventSerializer,
    FeatureSerializer,
)

logger = logging.getLogger(__name__)


class Device(BaseDevice):
    """Device class that inherits from BaseDevice."""

    def __init__(self, id, name, revision):
        super().__init__(id=id, name=name, revision=revision)
        self.clusters = set()
        self.classification = {}
        self.conformance = None
        self.filename = self.esp_name + "_device"
        self.revision_history = []
        self.conditions = []

    def get_clusters(self) -> List[BaseCluster]:
        return sorted(
            self.clusters, key=lambda x: (int(x.get_id(), 16), not x.server_cluster)
        )

    def get_unique_clusters(self):
        unique_clusters_dict = {}
        for cluster in self.clusters:
            cluster_id = cluster.get_id()
            if cluster_id not in unique_clusters_dict:
                unique_clusters_dict[cluster_id] = cluster
        unique_clusters = list(unique_clusters_dict.values())
        return sorted(
            unique_clusters, key=lambda x: (int(x.get_id(), 16), not x.server_cluster)
        )

    def get_optional_choice_clusters(self) -> dict:
        """Get clusters with O.a+ conformance grouped by choice marker.
        Returns a dict: {choice_marker: [cluster, ...]}
        """
        choice_groups = {}
        for cluster in self.clusters:
            if (
                hasattr(cluster, "optional_choice")
                and cluster.optional_choice is not None
            ):
                marker = cluster.optional_choice.get("choice")
                if marker not in choice_groups:
                    choice_groups[marker] = []
                choice_groups[marker].append(cluster)
        # Sort clusters within each group
        for marker in choice_groups:
            choice_groups[marker] = sorted(
                choice_groups[marker], key=lambda x: (int(x.get_id(), 16), x.name)
            )
        return choice_groups

    def has_optional_choice_clusters(self) -> bool:
        """Check if device has any O.a+ clusters."""
        return len(self.get_optional_choice_clusters()) > 0

    def to_dict(self):
        return DeviceSerializer.to_dict(self)


class Event(BaseEvent):
    def __init__(self, id, name, is_mandatory):
        super().__init__(name, id, is_mandatory)
        self.conformance = None

    def to_dict(self, attribute_map=None):
        """Convert event object to dictionary representation"""
        return EventSerializer.to_dict(self, attribute_map)


class Feature(BaseFeature):
    def __init__(self, name, code, id):
        super().__init__(name, id, is_mandatory=False)
        self.code = code
        self.command_set = set()
        self.attribute_set = set()
        self.event_set = set()
        self.summary = None
        self.conformance = None

    def add_attribute_list(self, attributes: set):
        if attributes:
            self.attribute_set.update(attributes)

    def add_event_list(self, events: set):
        if events:
            self.event_set.update(events)

    def get_attributes(self):
        """Returns the list of mandatory attributes for this feature"""
        attr_list = list(self.attribute_set)
        if len(attr_list) > 0:
            attr_list.sort(key=lambda x: int(x.get_id(), 16))
        return attr_list

    def get_events(self) -> List[BaseEvent]:
        """Returns the list of mandatory events for this feature"""
        event_list = list(self.event_set)
        if len(event_list) > 0:
            event_list.sort(key=lambda x: int(x.get_id(), 16))
        return event_list

    def add_command_list(self, commands):
        if commands is not None:
            self.command_set.update(commands)

    def get_commands(self):
        command_list = list(self.command_set)
        if len(command_list) > 0:
            command_list.sort(key=lambda x: int(x.get_id(), 16))
        return command_list

    def to_dict(self, attribute_map=None):
        """Convert feature object to dictionary representation"""
        return FeatureSerializer.to_dict(self, attribute_map)


class Command(BaseCommand):
    @dataclass(frozen=True)
    class CommandFlags:
        COMMAND_FLAG_NONE: str = "COMMAND_FLAG_NONE"
        COMMAND_FLAG_CUSTOM: str = "COMMAND_FLAG_CUSTOM"
        COMMAND_FLAG_ACCEPTED: str = "COMMAND_FLAG_ACCEPTED"
        COMMAND_FLAG_GENERATED: str = "COMMAND_FLAG_GENERATED"

    @dataclass
    class CommandAccess:
        invokePrivilege: str
        timed: bool
        fabric_scoped: bool

    @dataclass
    class CommandField:
        id: Any
        name: str
        type_: str
        default_value: Any = None
        is_mandatory: bool = False
        constraint: Any = None

    def __init__(self, id, name, direction, response, is_mandatory):
        super().__init__(
            (
                name.split(" ")[0]
                if len(name.split(" ")) > 1 and name.split(" ")[1] == "Command"
                else name
            ),
            id,
            is_mandatory,
            direction,
            response,
        )
        self.feature_list = set()
        self.access = None
        self.conformance = None
        self.fields = []  # List of CommandField objects
        self.feature_map = {}

        self.skip_command_cb = False
        self.command_handler_available = False

    def set_access(self, access):
        self.access = access

    def add_field(self, field):
        self.fields.append(field)

    def get_flag(self):
        if self.direction and self.direction.lower() == "commandtoserver":
            return self.CommandFlags.COMMAND_FLAG_ACCEPTED
        elif self.direction and self.direction.lower() == "responsefromserver":
            return self.CommandFlags.COMMAND_FLAG_GENERATED
        return self.CommandFlags.COMMAND_FLAG_NONE

    def callback_required(self):
        """Determine if a command requires a callback"""
        if self.command_handler_available:
            return False

        # If command is part of a cluster file with multiple cluster ids e.g. ResourceMonitoring
        if self.skip_command_cb:
            return False

        # Skip callbacks for client-bound commands
        if self.direction is not None and self.direction.lower() != "commandtoserver":
            return False

        # Commands with response='Y' or specific response commands need callbacks
        if self.response is None or self.response == "N":
            return False

        # Check if this is a response command (ends with 'Response')
        if self.name and self.name.endswith("Response"):
            return False

        return True

    def to_dict(self, attribute_map=None):
        """Convert command object to dictionary representation"""
        from .serializers import CommandSerializer

        return CommandSerializer.to_dict(self, attribute_map)


class Attribute(BaseAttribute):
    @dataclass(frozen=True)
    class AttributeFlags:
        ATTRIBUTE_FLAG_NONE: str = "ATTRIBUTE_FLAG_NONE"
        ATTRIBUTE_FLAG_WRITABLE: str = "ATTRIBUTE_FLAG_WRITABLE"
        ATTRIBUTE_FLAG_NONVOLATILE: str = "ATTRIBUTE_FLAG_NONVOLATILE"
        ATTRIBUTE_FLAG_MIN_MAX: str = "ATTRIBUTE_FLAG_MIN_MAX"
        ATTRIBUTE_FLAG_MUST_USE_TIMED_WRITE: str = "ATTRIBUTE_FLAG_MUST_USE_TIMED_WRITE"
        ATTRIBUTE_FLAG_EXTERNAL_STORAGE: str = "ATTRIBUTE_FLAG_EXTERNAL_STORAGE"
        ATTRIBUTE_FLAG_SINGLETON: str = "ATTRIBUTE_FLAG_SINGLETON"
        ATTRIBUTE_FLAG_NULLABLE: str = "ATTRIBUTE_FLAG_NULLABLE"
        ATTRIBUTE_FLAG_OVERRIDE: str = "ATTRIBUTE_FLAG_OVERRIDE"
        ATTRIBUTE_FLAG_DEFERRED: str = "ATTRIBUTE_FLAG_DEFERRED"
        ATTRIBUTE_FLAG_MANAGED_INTERNALLY: str = "ATTRIBUTE_FLAG_MANAGED_INTERNALLY"

    @dataclass
    class Access:
        read: str
        readPrivilege: str
        write: str
        writePrivilege: str

    @dataclass
    class Quality:
        changeOmitted: str
        nullable: str
        scene: str
        persistence: str
        reportable: str
        sourceAttribution: str = None
        quieterReporting: str = None

    @dataclass
    class Constraint:
        type: str
        from_: str = None
        to_: str = None
        value: str = None

        def to_dict(self):
            result = {"type": self.type}

            if not self.type:
                return result

            if self.type == "min":
                if self.value:
                    result["min"] = self.value
            elif self.type == "max":
                if self.value:
                    result["max"] = self.value
            elif self.type == "maxLength":
                if self.value:
                    result["maxLength"] = self.value
            elif self.type == "between":
                if self.from_:
                    result["min"] = self.from_
                if self.to_:
                    result["max"] = self.to_
            elif self.type == "desc":
                if self.value:
                    result["description"] = self.value
            else:
                # For other constraint types
                if self.value:
                    result["value"] = self.value

            return result

    def __init__(
        self,
        name,
        id,
        type_,
        default_value,
        is_mandatory,
        access=None,
        quality=None,
        constraint=None,
    ):
        super().__init__(name, id, type_, is_mandatory, default_value)
        self.conformance = None
        self.max_value = None
        self.min_value = None

        # Store access, quality, and constraint information
        self.access = access
        self.quality = quality
        self.constraint = constraint
        self.internally_managed = False
        self.is_nullable = (
            self.quality is not None
            and getattr(self.quality, "nullable", None) is not None
            and self.quality.nullable.lower() == "true"
        )

    def get_flag(self):
        flags = []
        if self.access is not None:
            write_val = getattr(self.access, "write", None)
            if write_val and write_val.lower() in ("true", "optional"):
                flags.append(self.AttributeFlags.ATTRIBUTE_FLAG_WRITABLE)
            if self.internally_managed:
                flags.append(self.AttributeFlags.ATTRIBUTE_FLAG_MANAGED_INTERNALLY)

        if self.quality is not None:
            nullable_val = getattr(self.quality, "nullable", None)
            if nullable_val and nullable_val.lower() == "true":
                flags.append(self.AttributeFlags.ATTRIBUTE_FLAG_NULLABLE)
            persistence_val = getattr(self.quality, "persistence", None)
            if persistence_val and persistence_val.lower() == "nonvolatile":
                flags.append(self.AttributeFlags.ATTRIBUTE_FLAG_NONVOLATILE)

        return " | ".join(flags) if flags else self.AttributeFlags.ATTRIBUTE_FLAG_NONE

    def get_default_value_type(self):
        """Get the ESP type for the default value"""
        value = self.get_default_value()
        if isinstance(value, bool):
            return "bool"
        if isinstance(value, int):
            if value <= 255:
                return "uint8_t"
            elif value <= 65535:
                return "uint16_t"
            return "uint32_t"
        return "uint32_t"

    def get_default_value(self):
        """Get the default value of the attribute"""
        return self._convert_default_values()

    def get_type(self):
        """Get the ESP type for the attribute"""
        return AttributeType(self.type).get_attribute_type()

    def _convert_default_values(self):
        """Convert the default value to known values"""
        if self.type == "bool":
            if self.default_value is None:
                return "false"
            return "true" if self.default_value.lower() in ("true", "1") else "false"

        if self.type in ("string", "octstr"):
            if (
                self.constraint is not None
                and getattr(self.constraint, "type", None) == "maxLength"
                and getattr(self.constraint, "value", None) is not None
            ):
                return int(self.constraint.value)
            return 0

        if self.type == "list":
            if (
                self.constraint is not None
                and getattr(self.constraint, "value", None)
                and self.constraint.value.isdigit()
            ):
                return int(self.constraint.value)
            return 0

        if "enum" in self.type.lower() or "bitmap" in self.type.lower():
            if self.default_value is not None:
                return convert_to_int(self.default_value, default="0")
            return "0"

        if self.default_value is not None and "°" in self.default_value:
            default_value = self.default_value.split("°")[0]
            if default_value.isdigit():
                return int(default_value) * 100
            return 0

        if self.default_value is not None and not self.default_value.isdigit():
            first_part = self.default_value.split(" ")[0]
            if first_part.isdigit():
                return int(first_part)

        return convert_to_int(self.default_value, default="0")

    def get_max_value(self):
        """Get the max value of the attribute"""
        return self.max_value

    def get_min_value(self):
        """Get the min value of the attribute"""
        return self.min_value

    def to_dict(self, attribute_map=None):
        """Convert attribute object to dictionary representation"""
        from .serializers import AttributeSerializer

        return AttributeSerializer.to_dict(self, attribute_map)


class Cluster(BaseCluster):
    @dataclass(frozen=True)
    class ClusterFlags:
        CLUSTER_FLAG_NONE: str = "CLUSTER_FLAG_NONE"
        CLUSTER_FLAG_INIT_FUNCTION: str = "CLUSTER_FLAG_INIT_FUNCTION"
        CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION: str = (
            "CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION"
        )
        CLUSTER_FLAG_SHUTDOWN_FUNCTION: str = "CLUSTER_FLAG_SHUTDOWN_FUNCTION"
        CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION: str = (
            "CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION"
        )
        CLUSTER_FLAG_SERVER: str = "CLUSTER_FLAG_SERVER"
        CLUSTER_FLAG_CLIENT: str = "CLUSTER_FLAG_CLIENT"

    def __init__(self, name, id, revision, is_mandatory=False):
        super().__init__(name, id, revision, is_mandatory)
        self.attributes = set()
        self.commands = set()
        self.events = set()
        self.features = set()
        self.conformance = None
        self.revision_history = []
        self.data_types = {}
        # Classification details
        self.role = "application"  # Default value
        self.hierarchy = None
        self.pics_code = None
        self.scope = None
        self.base_cluster_name = None
        self.is_migrated_cluster = False
        self.skip_command_cb = False
        self.optional_choice = None

    def get_callback_functions(self):
        """Get the callback functions for the cluster"""
        callback_functions = []
        if self.init_function_available:
            callback_functions.append(
                f"emberAf{self.chip_name}ClusterServerInitCallback"
            )
        if self.attribute_changed_function_available:
            callback_functions.append(
                f"Matter{self.chip_name}ClusterServerAttributeChangedCallback"
            )
        if self.shutdown_function_available:
            callback_functions.append(
                f"Matter{self.chip_name}ClusterServerShutdownCallback"
            )
        if self.pre_attribute_change_function_available:
            callback_functions.append(
                f"Matter{self.chip_name}ClusterServerPreAttributeChangedCallback"
            )
        return callback_functions

    def get_plugin_server_init_callback(self):
        """Get the plugin server init callback for the cluster"""
        if not self.plugin_init_cb_available or should_skip_plugin_callback(self.id):
            return None
        if "_cluster" in self.name.lower():
            cluster_name = self.name.split("_Cluster")[0]
            return f"Matter{cluster_name}PluginServerInitCallback"
        else:
            return f"Matter{self.chip_name}PluginServerInitCallback"

    def get_delegate_init_callback(self):
        """Get the delegate init callback for the cluster"""
        if self.delegate_init_callback_available and not should_skip_delegate_callback(
            self.id
        ):
            return f"{self.chip_name}DelegateInitCB"
        if should_include_delegate_callback(self.id):
            return f"{self.chip_name}DelegateInitCB"
        return None

    def get_attributes(self):
        """Get all attributes sorted by attribute id, then by name if ids match"""
        attributes = list(self.attributes)
        attributes.sort(key=lambda x: (int(x.get_id(), 16), x.name))
        return attributes

    def get_commands(self):
        """Get all commands sorted by command id, then by name if ids match"""
        commands = list(self.commands)
        commands.sort(key=lambda x: (int(x.get_id(), 16), x.name))
        return commands

    def get_events(self) -> List[BaseEvent]:
        """Get all events sorted by event id, then by name if ids match"""
        events = list(self.events)
        events.sort(key=lambda x: (int(x.get_id(), 16), x.name))
        return events

    def get_features(self):
        """Get all features sorted by feature id"""
        features = list(self.features)
        features.sort(key=lambda x: int(x.get_id(), 16))
        return features

    def _get_mandatory_elements(self, elements):
        """Helper to filter mandatory elements with no conformance condition."""
        cluster_revision = convert_to_int(self.revision, default=None)
        result = []
        for elem in elements:
            if not elem.is_mandatory:
                continue
            conformance = getattr(elem, "conformance", None)
            if conformance is None:
                continue
            if cluster_revision is not None and hasattr(
                conformance, "is_mandatory_at_revision"
            ):
                if not conformance.is_mandatory_at_revision(cluster_revision):
                    continue
            result.append(elem)
        return (
            sorted(result, key=lambda x: (int(x.get_id(), 16), x.name))
            if result
            else result
        )

    def get_mandatory_attributes(self):
        """Get only mandatory attributes from the attribute list"""
        return self._get_mandatory_elements(self.attributes)

    def get_mandatory_commands(self):
        """Get only mandatory commands from the command list"""
        return self._get_mandatory_elements(self.commands)

    def get_mandatory_events(self):
        """Get only mandatory events from the event list"""
        return self._get_mandatory_elements(self.events)

    def get_function_flags(self):
        """Get the function flags for the cluster"""
        flags = []
        if self.server_cluster:
            flags.append(self.ClusterFlags.CLUSTER_FLAG_SERVER)
        if self.client_cluster:
            flags.append(self.ClusterFlags.CLUSTER_FLAG_CLIENT)
        if self.init_function_available:
            flags.append(self.ClusterFlags.CLUSTER_FLAG_INIT_FUNCTION)
        if self.attribute_changed_function_available:
            flags.append(self.ClusterFlags.CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION)
        if self.shutdown_function_available:
            flags.append(self.ClusterFlags.CLUSTER_FLAG_SHUTDOWN_FUNCTION)
        if self.pre_attribute_change_function_available:
            flags.append(self.ClusterFlags.CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION)

        if len(flags) > 0:
            return " | ".join(flags)
        return self.ClusterFlags.CLUSTER_FLAG_NONE

    def to_dict(self):
        """Convert cluster object to dictionary representation"""
        from .serializers import ClusterSerializer

        return ClusterSerializer.to_dict(self)
