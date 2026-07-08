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
from jinja2 import Environment, FileSystemLoader
from .deserializer import ClusterDeserializer, DeviceDeserializer
from .elements import Cluster, Device
from typing import List

from utils.helper import write_to_file
from utils.exceptions import CodeGenerationError

template_dir = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "templates"
)


def format_filter(value, fmt):
    """Format filter for Jinja templates"""
    return fmt.format(value)


# Common file header for all generated files
FILE_HEADER = """// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* THIS IS A GENERATED FILE, DO NOT EDIT */"""


env = Environment(
    loader=FileSystemLoader(template_dir),
    trim_blocks=True,
    lstrip_blocks=True,
    extensions=["jinja2.ext.do"],
)
env.filters["format_filter"] = format_filter
env.globals["file_header"] = FILE_HEADER

# Expose the shared template macros (guard_open / guard_close) as globals so
# every template can use them without an explicit {% import %} line.
_macros = env.get_template("macros.jinja").module
env.globals["guard_open"] = _macros.guard_open
env.globals["guard_close"] = _macros.guard_close
env.globals["attr_member"] = _macros.attr_member


CLUSTER_CPP_TEMPLATE = "cluster.cpp.jinja"
CLUSTER_H_TEMPLATE = "cluster.h.jinja"
CLUSTER_IDS_TEMPLATE = "cluster_ids.h.jinja"
DEVICE_CPP_TEMPLATE = "device.cpp.jinja"
DEVICE_H_TEMPLATE = "device.h.jinja"

logger = logging.getLogger(__name__)


def get_all_cluster_objects(json_path: str) -> List[Cluster]:
    """Parse JSON file and create Cluster objects

    :param json_path: The path to the JSON file
    :returns: A list of clusters
    """
    return ClusterDeserializer().deserialize(json_path)


def get_all_device_objects(json_path: str, clusters: List[Cluster]) -> List[Device]:
    """Parse device types JSON and create Device objects

    :param json_path: The path to the JSON file
    :param clusters: A list of clusters
    :returns: A list of devices
    """
    cluster_lookup_table = {cluster.esp_name: cluster for cluster in clusters}
    return DeviceDeserializer().deserialize(json_path, cluster_lookup_table)


def render_templates(obj, cpp_template_obj, h_template_obj):
    if isinstance(obj, Cluster):
        cpp_code = cpp_template_obj.render(cluster=obj)
        h_code = h_template_obj.render(cluster=obj)
    elif isinstance(obj, Device):
        cpp_code = cpp_template_obj.render(device=obj)
        h_code = h_template_obj.render(device=obj)
    else:
        return None, None
    return cpp_code, h_code


def save_generated_files(file_name, cpp_code, h_code, output_dir):
    """Save the generated C++ and header files."""
    cpp_file_path = os.path.join(output_dir, f"{file_name}.cpp")
    h_file_path = os.path.join(output_dir, f"{file_name}.h")
    if not write_to_file(cpp_file_path, cpp_code):
        raise CodeGenerationError(
            "Failed to write generated C++ file",
            file_path=cpp_file_path,
            context=file_name,
            suggestion="Check write permissions and disk space.",
        )
    if not write_to_file(h_file_path, h_code):
        raise CodeGenerationError(
            "Failed to write generated header file",
            file_path=h_file_path,
            context=file_name,
            suggestion="Check write permissions and disk space.",
        )


def generate_cluster_files(json_path, output_dir):
    """Parse cluster JSON and generate C++ and header files.

    :param json_path: The path to the cluster JSON file.
    :param output_dir: The directory to save the generated files.
    :returns: A list of clusters.
    """
    try:
        cluster_output_dir = os.path.join(output_dir, "clusters")
        cluster_name_list = []

        os.makedirs(cluster_output_dir, exist_ok=True)

        cpp_template = env.get_template(CLUSTER_CPP_TEMPLATE)
        h_template = env.get_template(CLUSTER_H_TEMPLATE)
        ids_template = env.get_template(CLUSTER_IDS_TEMPLATE)

        clusters = get_all_cluster_objects(json_path)

        for cluster in clusters:
            cpp_code, h_code = render_templates(cluster, cpp_template, h_template)
            ids_code = ids_template.render(cluster=cluster)
            cluster_specific_dir = os.path.join(cluster_output_dir, cluster.esp_name)
            os.makedirs(cluster_specific_dir, exist_ok=True)
            if cpp_code and h_code:
                save_generated_files(
                    cluster.esp_name,
                    cpp_code,
                    h_code,
                    cluster_specific_dir,
                )
                cluster_name_list.append(cluster.esp_name)
                ids_file_path = os.path.join(
                    cluster_specific_dir, f"{cluster.esp_name}_ids.h"
                )
                write_to_file(ids_file_path, ids_code)

        header_file_path = os.path.join(output_dir, "clusters", "all_clusters.h")
        generate_header_file(header_file_path, cluster_name_list)
        logger.info(f"Cluster files generated at: {cluster_output_dir}")
        return clusters
    except Exception as e:
        logger.error(f"Error generating cluster files: {e}")
        raise CodeGenerationError(
            "Error generating cluster files",
            context="generate_cluster_files",
            suggestion="Check the cluster JSON file and the template files.",
        ) from e


def generate_device_files(json_path, output_dir, clusters: List[Cluster]):
    """Parse device JSON and generate C++ and header files.

    :param json_path: The path to the device JSON file.
    :param output_dir: The directory to save the generated files.
    :param clusters: A list of clusters.
    :returns: A list of devices.
    """
    try:
        device_output_dir = os.path.join(output_dir, "device_types")
        device_name_list = []

        os.makedirs(device_output_dir, exist_ok=True)

        cpp_template = env.get_template(DEVICE_CPP_TEMPLATE)
        h_template = env.get_template(DEVICE_H_TEMPLATE)
        devices = get_all_device_objects(json_path, clusters)

        for device in devices:
            cpp_code, h_code = render_templates(device, cpp_template, h_template)
            device_specific_dir = os.path.join(device_output_dir, device.filename)
            os.makedirs(device_specific_dir, exist_ok=True)
            if cpp_code and h_code:
                save_generated_files(
                    device.filename,
                    cpp_code,
                    h_code,
                    device_specific_dir,
                )
                device_name_list.append(device.filename)

        header_file_path = os.path.join(
            output_dir, "device_types", "all_device_types.h"
        )
        generate_header_file(header_file_path, device_name_list)
        logger.info(f"Device files generated at: {device_output_dir}")
        return devices
    except Exception as e:
        logger.error(f"Error generating device files: {e}")
        raise CodeGenerationError(
            "Error generating device files",
            context="generate_device_files",
            suggestion="Check the device JSON file and the template files.",
        ) from e


def generate_header_file(output_file_path: str, objects: List[str]):
    """Generate a all cluster and device header files.
    :param output_file_path: The filepath to save the header file.
    :param objects: A list of cluster or device names.
    """
    os.makedirs(os.path.dirname(output_file_path), exist_ok=True)
    header_content = [f"{FILE_HEADER}\n\n#pragma once\n"]
    sorted_objects = sorted(objects)

    for object_name in sorted_objects:
        header_content.append(f'#include "{object_name}.h"\n')

    with open(output_file_path, "w") as f:
        f.writelines(header_content)
    logger.info(f"Generated header file: {output_file_path}")
