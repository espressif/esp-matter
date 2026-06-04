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
import os

from utils.helper import convert_to_snake_case, write_to_file
from utils.helper import esp_name
from chip_source_deps.server_files_config import (
    local_mappings,
    PLUGIN_CB_PATTERN,
    DELEGATE_MEMBER_RE,
    DELEGATE_METHOD_RE,
    SKIP_DELEGATE_RE,
    delegate_skip_list,
)

logger = logging.getLogger(__name__)


def _get_cluster_name_prefix(name: str) -> str:
    """Strip path, extension, and common cluster directory/file suffixes."""
    basename = os.path.basename(name)
    common_suffixes = ("-server", "-cluster", "Cluster")
    basename = basename.rsplit(".", 1)[0]
    for suffix in common_suffixes:
        basename = basename.replace(suffix, "")
    return basename


def normalize_cluster_name(name: str) -> str:
    """Normalize a cluster name from a directory path, source filename, or identifier.

    e.g. on-off-server -> on_off, LevelControlCluster.cpp -> level_control
    """
    prefix = _get_cluster_name_prefix(name)
    normalized = convert_to_snake_case(prefix)
    normalized = local_mappings.get(normalized, normalized)
    return esp_name(normalized)


def process_cluster_files(root_dir):
    """Analyze cluster files in a single directory traversal."""

    migrated_clusters = set()
    migrated_clusters_with_codegen_impl = set()
    delegate_clusters = set()

    for dirpath, dirnames, filenames in os.walk(root_dir):
        has_codegen = "codegen" in dirnames
        scan_dir = os.path.join(dirpath, "codegen") if has_codegen else dirpath
        cluster_name = normalize_cluster_name(dirpath)

        if has_codegen:
            filenames = os.listdir(scan_dir)
            dirnames[:] = []
            migrated_clusters_with_codegen_impl.add(cluster_name)

        is_migrated = cluster_name in migrated_clusters
        for filename in [f for f in filenames if f.endswith(".h")]:
            full_path = os.path.join(scan_dir, filename)
            try:
                with open(full_path, "r", encoding="utf-8") as f:
                    content = f.read()
            except Exception as e:
                logger.warning(f"Failed reading {full_path}: {e}")
                continue
            if (
                not has_codegen
                and not is_migrated
                and "public DefaultServerCluster" in content
            ):
                migrated_clusters.add(cluster_name)
                is_migrated = True
            if cluster_name not in delegate_clusters:
                for line in content.splitlines():
                    stripped = line.strip()
                    if SKIP_DELEGATE_RE.search(stripped):
                        continue
                    if DELEGATE_MEMBER_RE.search(stripped) or DELEGATE_METHOD_RE.search(
                        stripped
                    ):
                        if cluster_name not in delegate_skip_list:
                            delegate_clusters.add(cluster_name)
                        break

    return {
        "migrated_clusters": {
            "migrated_cluster": sorted(migrated_clusters),
            "migrated_cluster_with_codegen_impl": sorted(
                migrated_clusters_with_codegen_impl
            ),
        },
        "delegate_clusters": sorted(delegate_clusters),
    }


def generate_delegate_cluster_mapping(
    root_dir, delegate_cluster_json_file_path, delegate_server_files
) -> tuple[bool, str]:
    """Generate cluster mapping list for clusters with delegate

    :param root_dir: The root directory to search for server files.
    :param delegate_cluster_json_file_path: The path to the JSON file for delegate clusters.
    :param delegate_server_files: The list of delegate server files.
    :returns: True if successful, False otherwise.
    """
    try:
        if write_to_file(
            delegate_cluster_json_file_path, delegate_server_files, "json"
        ):
            logger.info(
                f"Successfully written Delegate Clusters to {delegate_cluster_json_file_path}"
            )
            return True, None
        return False, f"Error writing to {delegate_cluster_json_file_path}"
    except Exception as e:
        return False, f"Error generating delegate cluster mapping: {str(e)}"


def generate_migrated_clusters(
    root_dir, migrated_clusters_json_file_path, migrated_clusters
) -> tuple[bool, str]:
    """Generate migrated clusters

    :param root_dir: The root directory to search for server files.
    :param migrated_clusters_json_file_path: The path to the JSON file for migrated clusters.
    :param migrated_clusters: The list of migrated clusters.
    :returns: True if successful, False otherwise.
    """
    try:
        if write_to_file(migrated_clusters_json_file_path, migrated_clusters, "json"):
            logger.info(
                f"Successfully written Migrated Clusters to {migrated_clusters_json_file_path}"
            )
            return True, None
        return False, f"Error writing to {migrated_clusters_json_file_path}"
    except Exception as e:
        return False, f"Error generating migrated clusters: {str(e)}"


def extract_cluster_names(header_file_path):
    """Extract cluster names from MatterXXXPluginServerInitCallback declarations.

    :param header_file_path: Path to the header file
    :returns: List of cluster names
    """
    try:
        with open(header_file_path, "r", encoding="utf-8") as f:
            content = f.read()
    except Exception as e:
        logger.error(f"Error reading file {header_file_path}: {e}")
        return []

    clusters = []
    for match in PLUGIN_CB_PATTERN.finditer(content):
        clusters.append(normalize_cluster_name(match.group(1)))
    return clusters


def generated_plugin_init_cb_cluster_mapping(
    header_file_path, plugin_init_cb_cluster_json_file_path
) -> tuple[bool, str]:
    """
    Generate plugin init callback cluster mapping

    :param header_file_path: The path to the header file.
    :param plugin_init_cb_cluster_json_file_path: The path to the JSON file for plugin init callback clusters.
    :returns: True if successful, False otherwise.

    """
    try:
        if not os.path.isfile(header_file_path):
            logger.warning(f"File {header_file_path} does not exist")
            return False, f"File {header_file_path} does not exist"

        cluster_names = extract_cluster_names(header_file_path)
        if write_to_file(plugin_init_cb_cluster_json_file_path, cluster_names, "json"):
            logger.info(
                f"Successfully written Plugin Init Callback Clusters to {plugin_init_cb_cluster_json_file_path}"
            )
            return True, None
        return False, f"Error writing to {plugin_init_cb_cluster_json_file_path}"
    except Exception as e:
        return False, f"Error generating plugin init callback cluster mapping: {str(e)}"
