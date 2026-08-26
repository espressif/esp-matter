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
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element

from .attribute_parser import AttributeParser
from .command_parser import CommandParser
from .data_type_parser import DataTypeParser
from .event_parser import EventParser
from .feature_parser import FeatureParser
from .parse_context import ClusterParseContext, load_cluster_parse_context
from .yaml_parser import YamlParser
from .elements import Cluster
from utils.helper import check_valid_id, esp_name, safe_get_attr
from utils.overrides import should_skip_cluster_command_callbacks

logger = logging.getLogger(__name__)
DUMMY_CLUSTER_ID = hex(0xFFFF)


class ClusterParser:
    """Class for parsing cluster data"""

    def parse(
        self,
        file_path,
        output_dir: str,
        yaml_parser: YamlParser,
        base_clusters: list[Cluster] = None,
        context: ClusterParseContext = None,
        root=None,
    ):
        """Parses an XML cluster file
        (A single base cluster file can have multiple derived clusters in single file)

        :param file_path: The path to the cluster XML file (used for logging; for parsing if root is None).
        :param output_dir: The path to the output directory.
        :param yaml_parser: The YAML parser.
        :param base_clusters: list[Cluster]:  (Default value = None)
        :param context: Pre-loaded metadata; if None, loaded from output_dir.
        :param root: Optional pre-parsed XML root element; if None, file_path is parsed.
        :returns: A list of clusters.
        """
        if root is None:
            tree = ET.parse(file_path)
            root = tree.getroot()
        clusters = []
        context = context or load_cluster_parse_context(output_dir)
        # If single file has multiple cluster ids, skip command callback generation (e.g. ResourceMonitoring)
        # or base cluster with dummy Id
        skip_command_cb = False

        cluster_name_id_list = self._get_cluster_name_and_id(root)
        if not cluster_name_id_list or len(cluster_name_id_list) == 0:
            logger.warning(f"Skipping {file_path} as it is not a valid cluster")
            return clusters
        if len(cluster_name_id_list) > 1:
            skip_command_cb = True

        for cluster_name, cluster_id in cluster_name_id_list:
            if not cluster_name or not cluster_id:
                logger.warning(
                    f"Skipping {file_path} as name or id is missing, (Either base cluster or not supported yet)"
                )
                continue
            if not check_valid_id(cluster_id):
                logger.warning(f"Skipping {file_path} as id is not valid: {cluster_id}")
                continue
            cluster = self.create(cluster_name, cluster_id, root)
            if cluster_id == DUMMY_CLUSTER_ID:
                skip_command_cb = True
            cluster.skip_command_cb = skip_command_cb
            self._set_context_flags(cluster, context, skip_command_cb)
            self._process_cluster_yaml(cluster, yaml_parser)

            base_cluster = (
                self._get_base_cluster(root, base_clusters) if base_clusters else None
            )
            self._inherit_from_base_cluster(cluster, base_cluster)

            base_features = safe_get_attr(base_cluster, "features", [])
            feature_parser = FeatureParser(root, cluster, base_features)
            feature_map = feature_parser.feature_map

            self._parse_attributes(cluster, feature_map, root, context, base_cluster)
            self._parse_commands(cluster, feature_map, root, context, base_cluster)
            self._parse_events(cluster, feature_map, root, context, base_cluster)
            feature_parser.parse(root)

            clusters.append(cluster)
        return clusters

    def create(self, cluster_name: str, cluster_id: str, elem: Element) -> Cluster:
        """Create a cluster object from the XML element.
        Assuming cluster has valid id and name."""
        cluster = Cluster(
            id=cluster_id,
            name=cluster_name,
            revision=elem.get("revision", "Unknown"),
        )
        self._set_metadata(cluster, elem)
        self._parse_revision_history(cluster, elem)
        return cluster

    def _inherit_from_base_cluster(self, derived_cluster, base_cluster):
        """Inherit property flags from base cluster to derived cluster."""
        if not derived_cluster or not base_cluster:
            return
        if base_cluster.delegate_init_callback_available:
            derived_cluster.delegate_init_callback_available = True
        if base_cluster.attribute_changed_function_available:
            derived_cluster.attribute_changed_function_available = True
        if base_cluster.shutdown_function_available:
            derived_cluster.shutdown_function_available = True
        if base_cluster.pre_attribute_change_function_available:
            derived_cluster.pre_attribute_change_function_available = True
        if base_cluster.plugin_init_cb_available:
            derived_cluster.plugin_init_cb_available = True

    def _set_context_flags(self, cluster, context, skip_command_cb=False):
        cluster.skip_command_cb = (
            skip_command_cb or should_skip_cluster_command_callbacks(cluster.id)
        )
        if cluster.esp_name in context.delegate_clusters:
            cluster.delegate_init_callback_available = True
        # Keyed by the (possibly overridden) esp_name; the value is the exact CHIP callback symbol.
        plugin_init_cb = context.plugin_init_cb_clusters.get(cluster.esp_name)
        if plugin_init_cb is not None:
            cluster.plugin_init_cb_available = True
            cluster.plugin_server_init_callback = plugin_init_cb
        if cluster.esp_name in context.migrated_clusters:
            cluster.is_migrated_cluster = True

    def _set_metadata(self, cluster: Cluster, root: Element):
        classification = root.find("classification")

        if classification is not None:
            cluster.role = classification.get("role", "application")
            cluster.hierarchy = classification.get("hierarchy")
            base_cluster_name = classification.get("baseCluster")
            if base_cluster_name:
                cluster.base_cluster_name = esp_name(base_cluster_name)
            cluster.pics_code = classification.get("picsCode")
            cluster.scope = classification.get("scope")
        else:
            logger.debug(
                f"Setting default role 'application' for cluster {cluster.name}"
            )
            cluster.role = "application"

    def _parse_attributes(self, cluster, feature_map, root, context, base_cluster):
        allowed_attribute_ids = context.get_allowed_attributes(cluster.id)
        managed_attributes = context.get_internally_managed_attributes(cluster.esp_name)
        base_attributes = safe_get_attr(base_cluster, "attributes", [])
        data_type_parser = DataTypeParser()
        attribute_parser = AttributeParser(
            cluster,
            feature_map,
            managed_attributes,
            allowed_attribute_ids,
            base_attributes,
        )
        cluster.attribute_types = data_type_parser.parse(root)
        cluster.data_types = data_type_parser.get_data_types()
        attribute_parser.parse(root)

    def _parse_commands(self, cluster, feature_map, root, context, base_cluster):
        allowed_command_ids = context.get_allowed_commands(cluster.id)
        base_commands = safe_get_attr(base_cluster, "commands", [])
        command_parser = CommandParser(
            cluster,
            feature_map,
            allowed_command_ids,
            base_commands,
        )
        command_parser.parse(root)

    def _parse_events(self, cluster, feature_map, root, context, base_cluster):
        allowed_event_ids = context.get_allowed_events(cluster.id)
        base_events = safe_get_attr(base_cluster, "events", [])
        event_parser = EventParser(
            cluster,
            feature_map,
            allowed_event_ids,
            base_events,
        )
        event_parser.parse(root)

    def _parse_revision_history(self, cluster, root):
        revision_history_elem = root.find("revisionHistory")
        if revision_history_elem is not None:
            for revision in revision_history_elem.findall("revision"):
                revision_info = {
                    "revision": revision.get("revision", "1"),
                    "summary": revision.get("summary", ""),
                }
                cluster.revision_history.append(revision_info)

    def _get_cluster_name_and_id(self, root):
        name_id_list = []
        cluster_name = root.get("name", "").replace(" Cluster", "")
        cluster_id = root.get("id")

        if cluster_name and cluster_id:
            return [[cluster_name, cluster_id]]

        if not cluster_name or not cluster_id:
            all_cluster_ids_element = root.find("clusterIds")
            if all_cluster_ids_element is None:
                return name_id_list
            cluster_ids_element = all_cluster_ids_element.findall("clusterId")
            for cluster_id_element in cluster_ids_element:
                cluster_name = cluster_id_element.get("name")
                cluster_id = cluster_id_element.get("id")
                if not cluster_id:
                    # Default to 0xFFFF if id is not present
                    cluster_id = hex(0xFFFF)
                if cluster_name and cluster_id:
                    name_id_list.append([cluster_name, cluster_id])
        return name_id_list

    def _get_base_cluster(self, root, base_clusters: list[Cluster]):
        classification = root.find("classification")
        if classification is None:
            return None
        base_cluster_name = classification.get("baseCluster")
        if not base_cluster_name:
            return None
        return next(
            (
                bc
                for bc in base_clusters
                if esp_name(bc.name) == esp_name(base_cluster_name)
            ),
            None,
        )

    # YAML list name -> cluster attribute to set (if cluster name is in that list)
    _YAML_CLUSTER_FLAGS = (
        ("CommandHandlerInterfaceOnlyClusters", "command_handler_available"),
        ("ClustersWithInitFunctions", "init_function_available"),
        (
            "ClustersWithAttributeChangedFunctions",
            "attribute_changed_function_available",
        ),
        ("ClustersWithShutdownFunctions", "shutdown_function_available"),
        (
            "ClustersWithPreAttributeChangeFunctions",
            "pre_attribute_change_function_available",
        ),
        ("CodeDrivenClusters", "is_migrated_cluster"),
    )

    def _process_cluster_yaml(self, cluster, yaml_parser: YamlParser):
        """Set cluster flags from YAML config: each list name maps to a cluster attribute."""
        if not yaml_parser:
            return
        cluster_name = safe_get_attr(cluster, "name")
        for list_name, attr_name in self._YAML_CLUSTER_FLAGS:
            if yaml_parser.is_present_in_list(list_name, cluster_name):
                setattr(cluster, attr_name, True)
