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
import os
import logging
from utils.config import ARTIFACTS_DIR, FileNames
from utils.exceptions import ConfigurationError, CodeGenerationError
from chip_source_deps.cluster_mapping import (
    process_cluster_files,
    generate_delegate_cluster_mapping,
    generate_migrated_clusters,
    generated_plugin_init_cb_cluster_mapping,
)
from chip_source_deps.zap_filter import generate_zap_filter_list
from chip_source_deps.internally_managed_attributes import (
    generate_internally_managed_attributes,
)

logger = logging.getLogger(__name__)


def generate_requirements(esp_matter_path, output_dir):
    """
    Generate all the required intermediate server files

    Args:
        esp_matter_path: Path to the ESP Matter repository
        output_dir: Directory where the generated files will be stored
    Returns:
        True if successful, False otherwise
    """
    os.makedirs(output_dir, exist_ok=True)

    chip_dir = os.path.join(esp_matter_path, "connectedhomeip/connectedhomeip")
    root_cluster_server_dir = os.path.join(chip_dir, "src/app/clusters/")
    header_files_dir = os.path.join(chip_dir, "zzz_generated/app-common/clusters")
    plugin_cb_header_file = os.path.join(
        esp_matter_path,
        "components/esp_matter/zap_common/app/PluginApplicationCallbacks.h",
    )

    if not os.path.exists(root_cluster_server_dir):
        raise ConfigurationError(
            "Clusters directory does not exist",
            file_path=root_cluster_server_dir,
            context="generate_requirements",
            suggestion="Ensure connectedhomeip is checked out under esp_matter_path.",
        )

    if not os.path.exists(header_files_dir):
        raise ConfigurationError(
            "Header files directory does not exist",
            file_path=header_files_dir,
            context="generate_requirements",
            suggestion="Generate CHIP app-common headers (zzz_generated) first.",
        )

    if not os.path.exists(plugin_cb_header_file):
        raise ConfigurationError(
            "Plugin callback header file does not exist",
            file_path=plugin_cb_header_file,
            context="generate_requirements",
            suggestion="Ensure esp_matter components include zap_common/app/PluginApplicationCallbacks.h.",
        )

    file_paths = {
        FileNames.INTERNALLY_MANAGED_ATTRIBUTES: os.path.join(
            output_dir, FileNames.INTERNALLY_MANAGED_ATTRIBUTES.value
        ),
        FileNames.DELEGATE_CLUSTERS: os.path.join(
            output_dir, FileNames.DELEGATE_CLUSTERS.value
        ),
        FileNames.PLUGIN_INIT_CB_CLUSTERS: os.path.join(
            output_dir, FileNames.PLUGIN_INIT_CB_CLUSTERS.value
        ),
        FileNames.ZAP_FILTER_LIST: os.path.join(
            output_dir, FileNames.ZAP_FILTER_LIST.value
        ),
        FileNames.MIGRATED_CLUSTERS: os.path.join(
            ARTIFACTS_DIR, FileNames.MIGRATED_CLUSTERS.value
        ),
    }

    logger.debug(
        "Generating internally managed attributes from both server files and zcl.json..."
    )
    zcl_json_path = os.path.join(chip_dir, "src/app/zap-templates/zcl/zcl.json")
    if not os.path.exists(zcl_json_path):
        raise ConfigurationError(
            "zcl.json file does not exist",
            file_path=zcl_json_path,
            context="generate_requirements",
            suggestion="Ensure connectedhomeip zap-templates are present.",
        )
    is_generated, error_message = generate_internally_managed_attributes(
        root_cluster_server_dir,
        zcl_json_path,
        file_paths[FileNames.INTERNALLY_MANAGED_ATTRIBUTES],
    )
    if not is_generated:
        raise CodeGenerationError(
            error_message,
            file_path=file_paths[FileNames.INTERNALLY_MANAGED_ATTRIBUTES],
            context="generate_requirements",
            suggestion="Check CHIP cluster server sources and zcl.json format.",
        )

    result = process_cluster_files(root_cluster_server_dir)

    logger.debug("Generating delegate clusters...")
    is_generated, error_message = generate_delegate_cluster_mapping(
        root_cluster_server_dir,
        file_paths[FileNames.DELEGATE_CLUSTERS],
        result["delegate_clusters"],
    )
    if not is_generated:
        raise CodeGenerationError(
            error_message,
            file_path=file_paths[FileNames.DELEGATE_CLUSTERS],
            context="generate_requirements",
            suggestion=f"Check {root_cluster_server_dir} and write permissions.",
        )

    logger.debug("Generating migrated clusters...")
    os.makedirs(ARTIFACTS_DIR, exist_ok=True)
    is_generated, error_message = generate_migrated_clusters(
        root_cluster_server_dir,
        file_paths[FileNames.MIGRATED_CLUSTERS],
        result["migrated_clusters"],
    )
    if not is_generated:
        raise CodeGenerationError(
            error_message,
            file_path=file_paths[FileNames.MIGRATED_CLUSTERS],
            context="generate_requirements",
            suggestion=f"Check {root_cluster_server_dir} and write permissions.",
        )

    logger.debug("Generating plugin init callback clusters...")
    is_generated, error_message = generated_plugin_init_cb_cluster_mapping(
        plugin_cb_header_file, file_paths[FileNames.PLUGIN_INIT_CB_CLUSTERS]
    )
    if not is_generated:
        raise CodeGenerationError(
            error_message,
            file_path=file_paths[FileNames.PLUGIN_INIT_CB_CLUSTERS],
            context="generate_requirements",
            suggestion=f"Check {plugin_cb_header_file} and output path.",
        )

    logger.debug("Generating include list...")
    is_generated, error_message = generate_zap_filter_list(
        header_files_dir, file_paths[FileNames.ZAP_FILTER_LIST]
    )
    if not is_generated:
        raise CodeGenerationError(
            error_message,
            file_path=file_paths[FileNames.ZAP_FILTER_LIST],
            context="generate_requirements",
            suggestion=f"Check {header_files_dir} and write permissions.",
        )

    return True
