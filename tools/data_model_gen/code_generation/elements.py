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
from __future__ import annotations
import logging
from utils.base_elements import (
    BaseCluster,
    BaseAttribute,
    BaseCommand,
    BaseEvent,
    BaseFeature,
    BaseDevice,
)
from .conformance_codegen import Conformance, ConformanceDecision, FeatureConformance
from typing import Dict, List, Tuple
from utils.conversion_utils import convert_to_int
from utils.overrides import (
    get_overridden_cluster_init_callback_name,
    get_overridden_cluster_shutdown_callback_name,
)

logger = logging.getLogger(__name__)


def get_id_name_lambda():
    """Returns a lambda function for sorting by ID and name"""
    return lambda x: (convert_to_int(x.get_id()), x.name)


def get_choice_group(
    parent_feature_type: str,
    conformance_type: ConformanceDecision,
    features: List[Feature],
):
    """
    Get groups of parent feature with their dependent features.
    Returns a list of dicts: {
        "parent_feature": <Feature>,
        "dependent_features": [<Feature>, ...],
        "constraint": "at_least_one" | "exact_one" | None
    }
    """
    groups_by_key: Dict[Tuple[str, str], List] = {}
    for feature in features:
        if not feature.conformance:
            continue
        if feature.conformance.type != conformance_type:
            continue
        parent_name = getattr(feature.conformance, parent_feature_type, None)
        if not parent_name or not feature.conformance.choice:
            continue
        key = (parent_name, feature.conformance.choice.marker)
        if key not in groups_by_key:
            groups_by_key[key] = []
        groups_by_key[key].append(feature)

    result = []
    for (parent_name, _marker), dependents in groups_by_key.items():
        parent = next(
            (f for f in features if f.func_name == parent_name),
            None,
        )
        if not parent:
            continue
        first = dependents[0]
        if first.conformance.is_at_least_one():
            constraint = "at_least_one"
        elif first.conformance.is_exact_one():
            constraint = "exact_one"
        else:
            constraint = None
        result.append(
            {
                "parent_feature": parent,
                "dependent_features": sorted(dependents, key=get_id_name_lambda()),
                "constraint": constraint,
            }
        )
    return result


class SpecialConfigElement:
    """Base Class for code-generation elements with preprocessor guard config."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.special_config = None

    def has_special_config(self) -> bool:
        return self.special_config is not None

    def get_special_config(self):
        return self.special_config if self.has_special_config() else None


class Cluster(SpecialConfigElement, BaseCluster):
    """Cluster class that inherits from BaseCluster"""

    def __init__(self, name, id, revision, is_mandatory):
        super().__init__(name=name, id=id, revision=revision, is_mandatory=is_mandatory)
        self.attributes = []
        self.commands = []
        self.events = []
        self.features = []
        self.function_flags = ""
        self.is_migrated_cluster = False
        self.is_base_cluster = False
        # O.a+ conformance (optional with choice)
        self.optional_choice = None
        self.device_has_choice_features = False
        self.device_mandatory_attributes = []
        self.device_mandatory_features = []
        self.device_mandatory_commands = []
        self.device_mandatory_events = []
        self.is_device_extra = False

    def get_attributes(self):
        """Get the list of attributes sorted by ID and name"""
        return sorted(self.attributes, key=get_id_name_lambda())

    def get_commands(self):
        """Get the list of commands sorted by ID and name"""
        return sorted(self.commands, key=get_id_name_lambda())

    def get_events(self) -> List[BaseEvent]:
        """Get the list of events sorted by ID and name"""
        return sorted(self.events, key=get_id_name_lambda())

    def get_features(self):
        """Get the list of features sorted by ID and name"""
        return sorted(self.features, key=get_id_name_lambda())

    def get_mandatory_attributes(self):
        """Get the list of mandatory attributes
        Attribute is mandatory:
        - if it is marked as mandatory in the cluster JSON file
        - has no conformance condition or there is no NOT TERM present in the conformance condition
        NOTE: NOT TERM indicate we have to create attribute by default while creating the cluster
        Attributes with runtime comparison conditions are excluded and handled separately.
        """
        mandatory_attributes = []
        for attr in self.get_attributes():
            if (
                attr.conformance
                and attr.conformance.get_comparison_condition_info() is not None
            ):
                continue
            if attr.is_mandatory and (
                attr.conformance.get_mandatory_condition() is None
                or attr.conformance.is_not_term_present
            ):
                mandatory_attributes.append(attr)
        return mandatory_attributes

    def get_comparison_conditional_attributes(self):
        """Get attributes whose creation depends on a runtime comparison condition.
        Returns a list of (condition_expr_string, [attributes]) tuples, ordered by literal value.
        Only includes groups where the referenced attribute is unconditionally mandatory —
        if the dependency is not in config the comparison would reference a missing field.
        """
        mandatory_config_names: set = set()
        comparison_pending: List[Tuple] = []

        for attr in self.get_attributes():
            if not attr.conformance:
                continue
            info = attr.conformance._get_comparison_info()
            if info is not None:
                comparison_pending.append((attr, *info))
            elif attr.is_mandatory and (
                attr.conformance.get_mandatory_condition() is None
                or attr.conformance.is_not_term_present
            ):
                mandatory_config_names.add(attr.func_name)

        groups: Dict[str, Tuple] = {}  # condition_expr → (literal, [attributes])
        for attr, expr_str, literal, ref_config_name in comparison_pending:
            if ref_config_name and ref_config_name not in mandatory_config_names:
                continue
            if expr_str not in groups:
                groups[expr_str] = (literal, [])
            groups[expr_str][1].append(attr)
        sorted_groups = sorted(groups.items(), key=lambda kv: kv[1][0])
        return [
            (expr, sorted(attrs, key=get_id_name_lambda()))
            for expr, (_, attrs) in sorted_groups
        ]

    def get_mandatory_commands(self):
        """Get the list of mandatory commands.
        A command is mandatory if it is marked as mandatory AND either:
        - has no conformance condition, or
        - has a NOT TERM present in the conformance condition (create by default).
        Additionally, response commands whose conformance requires another command are
        considered mandatory when that referenced command is already mandatory.
        """
        mandatory_commands = []
        response_candidates: List[Tuple] = []  # (cmd, [ref_cmd_names])

        for cmd in self.get_commands():
            if not cmd.is_mandatory:
                continue
            if cmd.conformance.get_comparison_condition_info() is not None:
                continue
            if (
                cmd.conformance.get_mandatory_condition() is None
                or cmd.conformance.is_not_term_present
            ):
                mandatory_commands.append(cmd)
            else:
                ref_names = cmd.conformance.get_mandatory_ref_command_names()
                if ref_names:
                    response_candidates.append((cmd, ref_names))

        mandatory_names = {c.name for c in mandatory_commands}
        for cmd, ref_names in response_candidates:
            if any(name in mandatory_names for name in ref_names):
                mandatory_commands.append(cmd)

        return mandatory_commands

    def get_mandatory_events(self):
        """Get the list of mandatory events
        Event is mandatory:
        - if it is marked as mandatory in the cluster JSON file
        - has no conformance condition or there is no NOT TERM present in the conformance condition
        NOTE: NOT TERM indicate we have to create event by default while creating the cluster
        """
        mandatory_events = []
        for event in self.get_events():
            if not event.is_mandatory:
                continue
            if event.conformance.get_comparison_condition_info() is not None:
                continue
            if (
                event.conformance.get_mandatory_condition() is None
                or event.conformance.is_not_term_present
            ):
                mandatory_events.append(event)
        return mandatory_events

    def get_all_exact_one_features(self):
        """Get all features that require exactly one feature to be supported"""
        feature_list = {}
        for feature in self.features:
            if not feature.conformance:
                continue
            if feature.conformance.is_exact_one():
                key = feature.conformance.choice.marker
                if key not in feature_list:
                    feature_list[key] = []
                feature_list[key].append(feature)
        return feature_list

    def get_all_at_least_one_features(self) -> Dict:
        """Get all features that require at least one feature to be supported"""
        feature_list = {}
        for feature in self.features:
            if not feature.conformance:
                continue
            if feature.conformance.is_at_least_one():
                key = feature.conformance.choice.marker
                if key not in feature_list:
                    feature_list[key] = []
                feature_list[key].append(feature)
        return feature_list

    def has_choice_features(self):
        """Check if the cluster has any choice features"""
        return (
            len(self.get_all_exact_one_features()) > 0
            or len(self.get_all_at_least_one_features()) > 0
        )

    def get_device_feature_flags_groups(self) -> List[Dict]:
        """Groups the choice features of the cluster by choice marker from device_mandatory_features list.

        grouped features are emitted under a single #if/#elif chain.
        All other features use independent #if blocks.
        e.g.
         #if CHIP_DEVICE_CONFIG_ENABLE_WIFI
            feature_flags |= feature::wi_fi_network_interface::get_id();
        #elif CHIP_DEVICE_CONFIG_ENABLE_THREAD
            feature_flags |= feature::thread_network_interface::get_id();
        +#elif CHIP_DEVICE_CONFIG_ENABLE_ETHERNET
            feature_flags |= feature::ethernet_network_interface::get_id();
        #endif
        """
        if not self.device_mandatory_features:
            return []

        exact_one_by_marker: Dict[str, list] = {}
        assigned = set()
        for feature in self.device_mandatory_features:
            if feature.conformance and feature.conformance.is_exact_one():
                marker = feature.conformance.choice.marker
                exact_one_by_marker.setdefault(marker, []).append(feature)

        groups = []
        for features in exact_one_by_marker.values():
            sorted_features = sorted(features, key=get_id_name_lambda())
            assigned.update(feature.func_name for feature in sorted_features)
            constraint = "exact_one" if len(sorted_features) > 1 else "independent"
            groups.append({"constraint": constraint, "features": sorted_features})

        for feature in sorted(
            (
                item
                for item in self.device_mandatory_features
                if item.func_name not in assigned
            ),
            key=get_id_name_lambda(),
        ):
            groups.append({"constraint": "independent", "features": [feature]})

        return groups

    def get_otherwise_choice_groups(self) -> List[Dict]:
        return get_choice_group(
            "mandatory_parent", ConformanceDecision.OTHERWISE, self.features
        )

    def get_optional_choice_groups(self) -> List[Dict]:
        return get_choice_group(
            "optional_parent", ConformanceDecision.OPTIONAL, self.features
        )

    def get_mandatory_choice_groups(self) -> List[Dict]:
        return get_choice_group(
            "mandatory_parent", ConformanceDecision.MANDATORY, self.features
        )

    def get_choice_group_feature_set(self) -> set:
        """Set of features that are parent or dependent in any otherwise choice group."""
        feature_set = set()
        for group in self.get_otherwise_choice_groups():
            feature_set.add(group["parent_feature"])
            feature_set.update(group["dependent_features"])
        for group in self.get_optional_choice_groups():
            feature_set.update(group["dependent_features"])
        for group in self.get_mandatory_choice_groups():
            feature_set.add(group["parent_feature"])
            feature_set.update(group["dependent_features"])
        return feature_set

    def get_standalone_choice_groups(self) -> List[Tuple[str, List]]:
        """
        Choice groups that do not have a mandatory parent (not otherwise groups).
        Returns list of (constraint_type, features) where constraint_type is
        "exact_one", or "at_least_one".
        """
        all_choice_set = self.get_choice_group_feature_set()
        choice_group_list = []
        for marker, features in self.get_all_exact_one_features().items():
            if not all(f in all_choice_set for f in features):
                choice_group_list.append(("exact_one", features))
        for marker, features in self.get_all_at_least_one_features().items():
            if not all(f in all_choice_set for f in features):
                choice_group_list.append(("at_least_one", features))
        return choice_group_list

    def get_independent_features(self) -> List:
        """Features that are not part of any otherwise choice group (parent or dependent)."""
        choice_set = self.get_choice_group_feature_set()
        choice_groups_list = self.get_standalone_choice_groups()
        choice_groups_set = set()
        for type, features in choice_groups_list:
            choice_groups_set.update(features)
        final_choice_set = choice_set | choice_groups_set
        return [f for f in self.get_features() if f not in final_choice_set]

    def get_cluster_init_callback(self):
        """Get the cluster init callback name"""
        return get_overridden_cluster_init_callback_name(self.id, self.chip_name)

    def get_cluster_shutdown_callback(self):
        """Get the cluster shutdown callback name"""
        return get_overridden_cluster_shutdown_callback_name(self.id, self.chip_name)

    def get_response_command(self, command_name: str):
        """Get the response command for a given command name"""
        for command in self.commands:
            if command.name == command_name:
                return command
        return None

    def get_destroyable_elements(self, feature_name: str):
        """Get the list of destroyable elements for a given feature name"""
        elements = {
            "attributes": [],
            "commands": [],
            "events": [],
        }
        for attribute in self.attributes:
            conformance_condition = (
                attribute.conformance.get_mandatory_condition()
                if attribute.conformance
                and attribute.conformance.get_mandatory_condition() is not None
                else ""
            )
            if (
                attribute.conformance.is_not_term_present
                and f"!(has_feature({feature_name})" in conformance_condition
            ):
                elements["attributes"].append(attribute)
        for command in self.commands:
            conformance_condition = (
                command.conformance.get_mandatory_condition()
                if command.conformance
                and command.conformance.get_mandatory_condition() is not None
                else ""
            )
            if (
                command.conformance.is_not_term_present
                and f"!(has_feature({feature_name})" in conformance_condition
            ):
                elements["commands"].append(command)
        for event in self.events:
            conformance_condition = (
                event.conformance.get_mandatory_condition()
                if event.conformance
                and event.conformance.get_mandatory_condition() is not None
                else ""
            )
            if (
                event.conformance.is_not_term_present
                and f"!(has_feature({feature_name})" in conformance_condition
            ):
                elements["events"].append(event)
        return elements


class Attribute(SpecialConfigElement, BaseAttribute):
    """Attribute class that inherits from BaseAttribute"""

    def __init__(self, name, id, type_, is_mandatory, default_value):
        super().__init__(
            name=name,
            id=id,
            type_=type_,
            is_mandatory=is_mandatory,
            default_value=default_value,
        )
        self.converted_type = None
        self.is_nullable = False
        self._flag = None
        self.max_length = 0  # For string attributes
        self.min_value = None  # For attribute bounds
        self.max_value = None  # For attribute bounds

        self.conformance = Conformance()
        self.is_internally_managed = False
        self.is_complex = False

    def get_flag(self):
        """Get the attribute flags"""
        return self._flag

    def get_type(self):
        """Get the attribute type"""
        return self.converted_type

    def get_min_value(self):
        """Get the min value"""
        return self.min_value

    def get_max_value(self):
        """Get the max value"""
        if self.max_value is None:
            return self.get_default_value()
        return self.max_value

    def get_default_value(self):
        """Get the default value"""
        return self.default_value

    def get_default_value_type(self):
        """Get the ESP type for the default value"""
        value = self.get_default_value()
        int_value = convert_to_int(value)
        if int_value is None:
            return "uint32_t"
        elif int_value <= 255:
            return "uint8_t"
        elif int_value <= 65535:
            return "uint16_t"
        return "uint32_t"

    def get_conformance_condition(self):
        """Get the conformance condition"""
        return self.conformance.get_mandatory_condition()

    def get_attr_val_expr(self, value_expr: str) -> str:
        """Get the C++ expression that constructs an esp_matter_attr_val."""
        if self.type == "string":
            return f"esp_matter_attr_val({value_expr}, length)"
        if self.type == "octstr":
            return f"esp_matter_attr_val({value_expr}, length)"
        if self.type == "list":
            return f"esp_matter_attr_val({value_expr}, length, count)"

        expr = value_expr
        if value_expr != "value":
            if self.is_nullable:
                expr = f"nullable<{self.get_type()}>({value_expr})"
            else:
                expr = f"static_cast<{self.get_type()}>({value_expr})"

        if self.type.startswith("enum"):
            return (
                f"esp_matter_attr_val({expr}, "
                "esp_matter_attr_val::uint_sub_type::k_enum)"
            )
        if self.type.startswith("bitmap"):
            return (
                f"esp_matter_attr_val({expr}, "
                "esp_matter_attr_val::uint_sub_type::k_bitmap)"
            )

        return f"esp_matter_attr_val({expr})"


class Command(SpecialConfigElement, BaseCommand):
    """Command class that inherits from BaseCommand"""

    def __init__(self, name, id, is_mandatory, direction, response):
        super().__init__(
            name=name,
            id=id,
            is_mandatory=is_mandatory,
            direction=direction,
            response=response,
        )
        self._flag = ""
        self.has_callback = False
        self.conformance = Conformance()
        self.is_fabric_scoped = False

    def get_flag(self):
        """Get the command flags"""
        return self._flag

    def get_conformance_condition(self):
        """Get the conformance condition"""
        return self.conformance.get_mandatory_condition()


class Feature(SpecialConfigElement, BaseFeature):
    """Feature class that inherits from BaseFeature"""

    def __init__(self, name, id, code, is_mandatory):
        super().__init__(name=name, id=id, is_mandatory=is_mandatory)
        self.attributes = []
        self.commands = []
        self.events = []
        self.conformance = FeatureConformance()
        self.code = code

    def get_attributes(self):
        """Get the list of attributes sorted by ID and name"""
        return sorted(self.attributes, key=get_id_name_lambda())

    def get_externally_managed_attributes(self):
        """Get the list of attributes that are externally managed"""
        return [attr for attr in self.attributes if not attr.is_internally_managed]

    def get_commands(self):
        """Get the list of commands sorted by ID and name"""
        return sorted(self.commands, key=get_id_name_lambda())

    def get_events(self) -> List[BaseEvent]:
        """Get the list of events sorted by ID and name"""
        return sorted(self.events, key=get_id_name_lambda())

    def get_conformance_condition(self):
        """Get the conformance condition"""
        return self.conformance.get_optional_condition()


class Event(SpecialConfigElement, BaseEvent):
    """Event class that inherits from BaseEvent"""

    def __init__(self, name, id, is_mandatory):
        super().__init__(name=name, id=id, is_mandatory=is_mandatory)
        self.priority = "Info"  # Default priority
        self.conformance = Conformance()

    def get_conformance_condition(self):
        """Get the conformance condition"""
        return self.conformance.get_mandatory_condition()


class Device(BaseDevice):
    """Device class that inherits from BaseDevice"""

    def __init__(self, id, name, revision):
        super().__init__(id=id, name=name, revision=revision)
        self.clusters = []
        self.extra_clusters = []

    def get_device_type_id(self):
        """Return the device type ID"""
        return self.id

    def get_device_type_version(self):
        """Get the device type version"""
        return self.revision

    def get_clusters(self):
        """Get all clusters sorted by ID and server/client type"""
        return sorted(
            self.clusters,
            key=lambda x: (convert_to_int(x.get_id()), not x.server_cluster),
        )

    def binding_cluster_available(self) -> bool:
        """Check if a binding cluster is available"""
        return any(cluster.client_cluster for cluster in self.get_mandatory_clusters())

    def get_mandatory_clusters(self):
        """Mandatory clusters from XML plus platform-specific extras."""
        return [
            cluster for cluster in self.clusters if cluster.is_mandatory
        ] + self.extra_clusters

    def get_unique_mandatory_clusters(self):
        """Get all unique mandatory clusters"""
        return [
            cluster for cluster in self.get_unique_clusters() if cluster.is_mandatory
        ]

    def get_unique_clusters(self):
        """Get unique clusters (no duplicates) sorted by ID"""
        unique_clusters = []
        seen_names = set()
        # Add the descriptor and binding clusters to the set of seen names as they are always present
        seen_names.add("descriptor")
        seen_names.add("binding")
        sorted_clusters = self.get_clusters()
        for cluster in sorted_clusters:
            if cluster.esp_name not in seen_names:
                unique_clusters.append(cluster)
                seen_names.add(cluster.esp_name)
        return unique_clusters

    def get_optional_choice_clusters(self) -> Dict[str, List]:
        """Get clusters with O.a+ conformance grouped by choice marker.
        Returns a dict: {choice_marker: [cluster, ...]}
        """
        choice_groups: Dict[str, List] = {}
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
                choice_groups[marker],
                key=lambda x: (convert_to_int(x.get_id()), x.name),
            )
        return choice_groups

    def get_unique_optional_choice_clusters(self) -> List:
        """Get unique O.a+ clusters (flattened, no duplicates)."""
        seen_names = set()
        unique_clusters = []
        for marker, clusters in self.get_optional_choice_clusters().items():
            for cluster in clusters:
                if cluster.esp_name not in seen_names:
                    unique_clusters.append(cluster)
                    seen_names.add(cluster.esp_name)
        return sorted(
            unique_clusters, key=lambda x: (convert_to_int(x.get_id()), x.name)
        )

    def has_optional_choice_clusters(self) -> bool:
        """Check if device has any O.a+ clusters."""
        return len(self.get_optional_choice_clusters()) > 0
