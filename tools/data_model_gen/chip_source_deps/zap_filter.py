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
import os
import logging

from utils.conversion_utils import format_hex_value, is_hex_value
from chip_source_deps.server_files_config import (
    ATTRIBUTE_PATTERN,
    COMMAND_PATTERN,
    EVENT_PATTERN,
    CLUSTER_ID_PATTERN,
)

logger = logging.getLogger(__name__)


def parse_attributes_header(file_path):
    """Parse a C++ header file containing attribute definitions and return a dictionary
    mapping attribute names to their ID values.

    :param file_path:
    :returns: Dictionary with attribute_name: attribute_id mapping
    :rtype: dict

    """
    attributes = {}

    try:
        with open(file_path, "r") as f:
            content = f.read()
            matches = ATTRIBUTE_PATTERN.finditer(content)
            for match in matches:
                attr_name = match.group(1)
                attr_id = match.group(2)
                if is_hex_value(attr_id):
                    attributes[attr_name] = format_hex_value(attr_id)
        return attributes
    except Exception as e:
        logger.error(f"Error parsing attribute header file {file_path}: {e}")
        return {}


def parse_commands_header(file_path):
    """Parse a C++ header file containing command definitions and return a dictionary
    mapping command names to their ID values.

    :param file_path:
    :returns: Dictionary with command_name: command_id mapping
    :rtype: dict

    """
    commands = {}
    try:
        with open(file_path, "r") as f:
            content = f.read()
            matches = COMMAND_PATTERN.finditer(content)
            for match in matches:
                cmd_name = match.group(1)
                cmd_id = match.group(2)
                commands[cmd_name] = format_hex_value(cmd_id)
        return commands
    except Exception as e:
        logger.error(f"Error parsing command header file {file_path}: {e}")
        return {}


def parse_events_header(file_path):
    """Parse a C++ header file containing event definitions and return a dictionary
    mapping event names to their ID values.

    :param file_path:
    :returns: Dictionary with event_name: event_id mapping
    :rtype: dict

    """
    events = {}
    try:
        with open(file_path, "r") as f:
            content = f.read()
            matches = EVENT_PATTERN.finditer(content)
            for match in matches:
                event_name = match.group(1)
                event_id = match.group(2)
                events[event_name] = format_hex_value(event_id)
        return events
    except Exception as e:
        logger.error(f"Error parsing event header file {file_path}: {e}")
        return {}


def extract_cluster_id(file_path):
    """Extract cluster ID from ClusterId.h file.

    :param file_path:
    :returns: Cluster ID value as a string
    :rtype: str

    """
    try:
        with open(file_path, "r") as f:
            content = f.read()
            match = CLUSTER_ID_PATTERN.search(content)
            if match:
                return format_hex_value(match.group(1))
        return None
    except Exception as e:
        logger.error(f"Error extracting cluster ID from {file_path}: {e}")
        return None


def parse_cluster_directory(cluster_dir):
    """Parse a cluster directory containing header files and return a structured dictionary
    with all the cluster information.

    :param cluster_dir:
    :returns: Dictionary with cluster information
    :rtype: dict

    """
    cluster_name = os.path.basename(cluster_dir)
    cluster_id_file = os.path.join(cluster_dir, "ClusterId.h")
    attributes_file = os.path.join(cluster_dir, "AttributeIds.h")
    commands_file = os.path.join(cluster_dir, "CommandIds.h")
    events_file = os.path.join(cluster_dir, "EventIds.h")
    result = {
        "name": cluster_name,
        "id": None,
        "Attributes": {},
        "Commands": {},
        "Events": {},
    }
    if os.path.exists(cluster_id_file):
        result["id"] = extract_cluster_id(cluster_id_file)
    if os.path.exists(attributes_file):
        result["Attributes"] = parse_attributes_header(attributes_file)
    if os.path.exists(commands_file):
        result["Commands"] = parse_commands_header(commands_file)
    if os.path.exists(events_file):
        result["Events"] = parse_events_header(events_file)
    return result


def generate_zap_filter_list(clusters_base_dir, output_file) -> tuple[bool, str]:
    """Generate zap_filter_list.json from zap-generated header files.

    :param clusters_base_dir: The base directory containing the cluster header files.
    :param output_file:
    :returns: True if the file was written successfully, False otherwise.

    """
    all_clusters = {}
    for cluster_dir in os.listdir(clusters_base_dir):
        result = parse_cluster_directory(os.path.join(clusters_base_dir, cluster_dir))
        all_clusters[result["id"]] = {
            "name": result["name"],
            "Attributes": result["Attributes"],
            "Commands": result["Commands"],
            "Events": result["Events"],
        }
    formatted_output = {}
    formatted_output["clusters"] = all_clusters
    try:
        output_dir = os.path.dirname(output_file)
        if output_dir and not os.path.exists(output_dir):
            os.makedirs(output_dir)

        with open(output_file, "w") as f:
            json.dump(formatted_output, f, indent=2)

        logger.info(
            f"Successfully written zap data model elements filter list to {output_file}"
        )
        return True, None
    except Exception as e:
        return False, f"Error writing to {output_file}: {str(e)}"
