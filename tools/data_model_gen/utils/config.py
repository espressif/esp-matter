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
import re
from enum import Enum
from typing import List, Optional, Tuple

from .exceptions import ConfigurationError
from colorlog import ColoredFormatter
import sys

# Use %(filename)s instead of %(pathname)s to get just the filename
LOG_FORMAT_STRING = "[%(levelname)s] %(filename)s:%(lineno)d: %(message)s"

COLORED_FORMATTER = ColoredFormatter(
    "%(log_color)s" + LOG_FORMAT_STRING,
    log_colors={
        "DEBUG": "cyan",
        "INFO": "green",
        "WARNING": "yellow",
        "ERROR": "red",
        "CRITICAL": "bold_red",
    },
)

DEFAULT_OUTPUT_DIR = "out"

ARTIFACTS_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "artifacts"
)

# Folder names under `connectedhomeip/connectedhomeip/data_model` that look like
# a numeric revision (e.g. 1.4, 1.4.2, 1.5.1) — excludes README and other files.
_DATA_MODEL_VERSION_DIR = re.compile(r"^\d+(?:\.\d+)*$")


def get_chip_data_model_root(esp_matter_path: str) -> str:
    """Path to the CHIP `data_model` directory for the connectedhomeip submodule."""
    return os.path.join(
        esp_matter_path, "connectedhomeip", "connectedhomeip", "data_model"
    )


def specification_version_sort_key(version: str) -> Tuple[int, ...]:
    """Sort key for Matter `data_model` folder names (dot-separated integers)."""
    return tuple(int(p) for p in version.split("."))


def discover_data_model_specification_versions(esp_matter_path: str) -> List[str]:
    """
    List data model revision folder names that exist under CHIP's `data_model/`
    and have the expected `clusters` and `device_types` subdirectories.
    """
    root = get_chip_data_model_root(esp_matter_path)
    if not os.path.isdir(root):
        return []

    out: List[str] = []
    for name in os.listdir(root):
        if not _DATA_MODEL_VERSION_DIR.match(name):
            continue
        full = os.path.join(root, name)
        if not os.path.isdir(full):
            continue
        clusters = os.path.join(full, "clusters")
        device_types = os.path.join(full, "device_types")
        if os.path.isdir(clusters) and os.path.isdir(device_types):
            out.append(name)

    return sorted(out, key=specification_version_sort_key)


def get_highest_data_model_version(esp_matter_path: str) -> Optional[str]:
    """Highest revision folder name under `data_model/`, or None if none qualify."""
    versions = discover_data_model_specification_versions(esp_matter_path)
    if not versions:
        return None
    return max(versions, key=specification_version_sort_key)


ALLOW_PROVISIONAL = False

DEFAULT_DATA_MODEL_DIR = None

ESP_MATTER_PATH = None


def allow_provisional():
    """
    Check if provisional elements are allowed.
    """
    return ALLOW_PROVISIONAL


def setup_provisional_mode(allow: bool):
    """
    Setup provisional mode.

    Args:
        allow: Whether to allow provisional elements
    """
    global ALLOW_PROVISIONAL
    ALLOW_PROVISIONAL = allow


def get_default_data_model_dir():
    """
    Get the default data model directory.
    """
    esp_path = ESP_MATTER_PATH or os.getenv("ESP_MATTER_PATH", "")
    if not esp_path or not os.path.isdir(esp_path):
        raise ConfigurationError(
            "ESP Matter path is not set or is not a directory",
            context="get_default_data_model_dir",
            suggestion="Set ESP_MATTER_PATH environment variable to the esp-matter repository root.",
        )
    return os.path.join(esp_path, "components", "esp_matter", "data_model", "generated")


def set_esp_matter_path(path: str):
    """
    Set the ESP Matter path.

    Args:
        path: The path to the ESP Matter repository

    Raises:
        ConfigurationError: If path is empty or does not exist.
    """
    if not path or not path.strip():
        raise ConfigurationError(
            "ESP Matter path is empty",
            context="set_esp_matter_path",
            suggestion="Set ESP_MATTER_PATH environment variable to the esp-matter repository root.",
        )
    if not os.path.isdir(path):
        raise ConfigurationError(
            f"ESP Matter path is not a directory: {path}",
            context="set_esp_matter_path",
            suggestion="Ensure ESP_MATTER_PATH points to an existing directory.",
        )
    global ESP_MATTER_PATH
    ESP_MATTER_PATH = path
    global DEFAULT_DATA_MODEL_DIR
    DEFAULT_DATA_MODEL_DIR = os.path.join(
        ESP_MATTER_PATH, "components", "esp_matter", "data_model", "generated"
    )


def setup_logger(log_level="INFO", is_colored=False):
    """
    Sets up a logger with the specified level and color formatting.

    Args:
        log_level: Logging level to use
        is_colored: Whether to use colored output
    """
    root_logger = logging.getLogger()
    root_logger.setLevel(log_level)

    if root_logger.hasHandlers():
        root_logger.handlers.clear()

    stream_handler = logging.StreamHandler(sys.stdout)

    if is_colored:
        stream_handler.setFormatter(COLORED_FORMATTER)
    else:
        stream_handler.setFormatter(logging.Formatter(LOG_FORMAT_STRING))

    root_logger.addHandler(stream_handler)


class FileNames(Enum):
    """Enum for file names used by the data model generation tool"""

    INTERNALLY_MANAGED_ATTRIBUTES = "internally_managed_attributes.json"
    DELEGATE_CLUSTERS = "delegate_clusters.json"
    PLUGIN_INIT_CB_CLUSTERS = "plugin_init_cb_clusters.json"
    CLUSTER_MAPPING = "cluster_mapping.json"
    MIGRATED_CLUSTERS = "migrated_clusters.json"
    ZAP_FILTER_LIST = "zap_filter_list.json"
    CLUSTER_JSON = "clusters.json"
    DEVICE_JSON = "device_types.json"
