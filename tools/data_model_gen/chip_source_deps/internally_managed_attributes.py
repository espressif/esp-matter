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
from utils.helper import convert_to_snake_case, write_to_file
from chip_source_deps.cluster_mapping import normalize_cluster_name
from chip_source_deps.server_files_config import (
    KEYWORDS,
    ATTRIBUTE_REGEX,
    READ_FUNC_PATTERN,
)

logger = logging.getLogger(__name__)


def merge_attributes_dicts(dict1, dict2):
    """
    Merge two dictionaries of attributes, combining their values and removing duplicates

    :param dict1: First dictionary of attributes
    :param dict2: Second dictionary of attributes
    :return: Merged dictionary with sorted attributes
    """
    merged = dict1.copy()

    for cluster_name, attributes in dict2.items():
        if cluster_name in merged:
            merged[cluster_name] = sorted(list(set(merged[cluster_name] + attributes)))
        else:
            merged[cluster_name] = sorted(attributes)

    return merged


def _extract_braced_region(code: str, open_brace_index: int) -> str:
    """Return the substring from open_brace_index through its matching closing brace."""
    depth = 0
    for index in range(open_brace_index, len(code)):
        if code[index] == "{":
            depth += 1
        elif code[index] == "}":
            depth -= 1
            if depth == 0:
                return code[open_brace_index : index + 1]
    return ""


def _attribute_names_from_case_labels(body: str) -> list[str]:
    """Extract attribute names from ``case Attribute::Id:`` labels in a function body."""
    labels = []
    for match in ATTRIBUTE_REGEX.finditer(body):
        parts = match.group(1).split("::")
        if len(parts) >= 2 and "Id" in parts[-1]:
            labels.append(parts[-2].lower())
    return labels


def _function_bodies_for_read_keywords(code: str) -> list[str]:
    """Return braced bodies of Read / ReadAttribute / ReadImpl member functions."""
    if not any(keyword in code for keyword in KEYWORDS):
        return []

    bodies = []
    for match in READ_FUNC_PATTERN.finditer(code):
        body = _extract_braced_region(code, match.end() - 1)
        if body:
            bodies.append(body)
    return bodies


def extract_attributes_from_server_file(file_path):
    """Extract attributes from Read Attribute function in the server source file.

    :param file_path: Path to the C++ file
    :returns: A list of unique attributes found in Read Attribute function

    NOTE: There are different Read methods followed for different clusters.
    This API try to find different Read method definitions and extract the attributes from them.
    """
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            code = f.read()
    except Exception as e:
        logger.error(f"Error reading file {file_path}: {e}")
        return []

    all_attributes = []
    for body in _function_bodies_for_read_keywords(code):
        all_attributes.extend(_attribute_names_from_case_labels(body))

    return sorted(list(set(all_attributes)))


def find_server_source_files(root_dir):
    """Find all server source files in the given directory

    :param root_dir: cluster implementation directory from the connectedhomeip repository.
    :returns: A dictionary of cluster names to their corresponding server source file path.

    NOTE: Now all source files from the corresponding repo. are matched with the cluster name
    as there is no properly naming convention followed for the source files and it's difficult to identify the server source file from all the files.

    TODO: return only server source file once upstream follows same naming convention for all clusters.
    """
    server_source_files = {}
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in (f for f in filenames if f and f.endswith(".cpp")):
            cluster_name = normalize_cluster_name(filename)
            server_source_files[cluster_name] = os.path.join(dirpath, filename)
    return server_source_files


def get_attributes_from_server_files(root_dir, json_file_path) -> bool:
    """Parse all server source files in the given directory and extract attributes

    :param root_dir: The root directory to search for server source files.
    :param json_file_path: The path to the JSON file to write the results to.

    """
    if not os.path.isdir(root_dir):
        logger.warning("Directory %s does not exist", root_dir)
        return False
    server_source_files = find_server_source_files(root_dir)

    if not server_source_files:
        logger.debug(f"No server source files found in {root_dir}")
        return False

    logger.debug(f"Found {len(server_source_files)} server source files")

    internally_managed_attributes = {}

    for cluster_name, file_path in server_source_files.items():
        attributes = extract_attributes_from_server_file(file_path)
        if attributes:
            internally_managed_attributes[cluster_name] = sorted(attributes)

    internally_managed_attributes = dict(sorted(internally_managed_attributes.items()))
    write_to_file(json_file_path, internally_managed_attributes, "json")

    logger.debug(f"\nResults written to {json_file_path}")
    return True


def get_attributes_from_zcl(zcl_json_path, output_json_path):
    """
    Get attributes from the zcl.json file in the connectedhomeip repository.

    :param zcl_json_path: Path to the zcl.json file
    :param output_json_path: Path to write the output JSON file

    NOTE: The zcl.json file is maintained in the connectedhomeip repository.
    It contains the attributes that are managed internally by the cluster.

    :return: True if attributes are extracted and written to the output JSON file, False otherwise
    """
    try:
        with open(zcl_json_path, "r", encoding="utf-8") as f:
            zcl_data = json.load(f)
    except Exception as e:
        logger.error(f"Error reading zcl.json file: {e}")
        return False

    if "attributeAccessInterfaceAttributes" not in zcl_data:
        logger.error("attributeAccessInterfaceAttributes section not found in zcl.json")
        return False

    attr_access_interface = zcl_data["attributeAccessInterfaceAttributes"]

    internally_managed_attributes = {}

    for cluster_name, attributes in attr_access_interface.items():
        cluster_key = convert_to_snake_case(cluster_name)

        attr_list = sorted([attr.lower() for attr in attributes])

        internally_managed_attributes[cluster_key] = attr_list

    internally_managed_attributes = dict(sorted(internally_managed_attributes.items()))

    with open(output_json_path, "w", encoding="utf-8") as f:
        json.dump(internally_managed_attributes, f, indent=4, sort_keys=True)

    return True


def generate_internally_managed_attributes(
    root_cluster_server_dir, zcl_json_path, output_json_path
) -> tuple[bool, str]:
    """
    Get internally managed attributes.
    Get attributes from server source files and zcl.json.
    Merge the attributes and write to single JSON file ensuring no missing attributes.

    :param root_cluster_server_dir: Path to the directory containing server files
    :param zcl_json_path: Path to the zcl.json file
    :param output_json_path: Path to write the output JSON file
    :return: True if successful, False otherwise
    """
    temp_server_file = None
    temp_zcl_file = None
    try:
        temp_server_file = output_json_path + ".server_temp"
        temp_zcl_file = output_json_path + ".zcl_temp"

        logger.debug("Extracting attributes from server files...")
        server_success = get_attributes_from_server_files(
            root_cluster_server_dir, temp_server_file
        )

        logger.debug("Extracting attributes from zcl.json...")
        zcl_success = get_attributes_from_zcl(zcl_json_path, temp_zcl_file)

        if not server_success and not zcl_success:
            return False, "Failed to generate attributes from both sources"

        server_attributes = {}
        zcl_attributes = {}

        if server_success:
            try:
                with open(temp_server_file, "r", encoding="utf-8") as f:
                    server_attributes = json.load(f)
                logger.debug(
                    f"Loaded {len(server_attributes)} clusters from server files"
                )
            except Exception as e:
                logger.error(f"Error reading server attributes: {e}")

        if zcl_success:
            try:
                with open(temp_zcl_file, "r", encoding="utf-8") as f:
                    zcl_attributes = json.load(f)
                logger.debug(f"Loaded {len(zcl_attributes)} clusters from zcl.json")
            except Exception as e:
                logger.error(f"Error reading zcl attributes: {e}")

        merged_attributes = merge_attributes_dicts(server_attributes, zcl_attributes)
        logger.debug(f"Combined into {len(merged_attributes)} clusters")

        sorted_merged_attributes = dict(sorted(merged_attributes.items()))
        write_to_file(output_json_path, sorted_merged_attributes, "json")
        logger.info(
            f"Successfully generated Internally Managed Attributes at {output_json_path}"
        )
        return True, None
    except Exception as e:
        return False, f"Error generating internally managed attributes: {str(e)}"
    finally:
        if os.path.exists(temp_server_file):
            os.remove(temp_server_file)
        if os.path.exists(temp_zcl_file):
            os.remove(temp_zcl_file)
