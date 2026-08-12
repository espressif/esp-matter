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
Parse context: pre-loaded metadata used during cluster parsing.
Loads JSON artifacts once per run to avoid repeated file I/O.
"""

import json
import os

from utils.config import ARTIFACTS_DIR, FileNames
from utils.conversion_utils import hex_to_int


def load_cluster_parse_context(output_dir: str):
    """Load all cluster-related metadata from output_dir into a single context."""

    def _load_json(filename: str, base_dir: str = output_dir):
        path = os.path.join(base_dir, filename)
        with open(path, "r") as f:
            return json.load(f)

    return ClusterParseContext(
        delegate_clusters=_load_json(FileNames.DELEGATE_CLUSTERS.value),
        plugin_init_cb_clusters=_load_json(FileNames.PLUGIN_INIT_CB_CLUSTERS.value),
        migrated_clusters=_load_json(
            FileNames.MIGRATED_CLUSTERS.value, base_dir=ARTIFACTS_DIR
        )["migrated_cluster"],
        zap_filter_list=_load_json(FileNames.ZAP_FILTER_LIST.value),
        internally_managed_attributes=_load_json(
            FileNames.INTERNALLY_MANAGED_ATTRIBUTES.value
        ),
    )


class ClusterParseContext:
    """Holds pre-loaded metadata for cluster parsing. Built once per run."""

    def __init__(
        self,
        delegate_clusters: list,
        plugin_init_cb_clusters: list,
        migrated_clusters: list,
        zap_filter_list: dict,
        internally_managed_attributes: dict,
    ):
        self.delegate_clusters = delegate_clusters
        self.plugin_init_cb_clusters = (
            plugin_init_cb_clusters if isinstance(plugin_init_cb_clusters, dict) else {}
        )
        self.migrated_clusters = migrated_clusters
        self.zap_filter_list = zap_filter_list
        self.internally_managed_attributes = internally_managed_attributes

    def get_allowed_attributes(self, cluster_id: str):
        """Return allowed attributes for cluster_id."""
        cluster_info = self.zap_filter_list.get("clusters", {}).get(cluster_id, {})
        if not cluster_info:
            return []
        return hex_to_int(list(cluster_info.get("Attributes", {}).values()))

    def get_allowed_commands(self, cluster_id: str):
        """Return allowed commands for cluster_id."""
        cluster_info = self.zap_filter_list.get("clusters", {}).get(cluster_id, {})
        if not cluster_info:
            return []
        return hex_to_int(list(cluster_info.get("Commands", {}).values()))

    def get_allowed_events(self, cluster_id: str):
        """Return allowed events for cluster_id."""
        cluster_info = self.zap_filter_list.get("clusters", {}).get(cluster_id, {})
        if not cluster_info:
            return []
        return hex_to_int(list(cluster_info.get("Events", {}).values()))

    def get_internally_managed_attributes(self, esp_name: str) -> list:
        """Return list of internally managed attribute names for the cluster."""
        return self.internally_managed_attributes.get(esp_name, [])
