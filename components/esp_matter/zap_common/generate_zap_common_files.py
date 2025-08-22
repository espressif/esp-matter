#!/usr/bin/env python3

# Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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
Script to generate files under zap_common directory
"""

import os
import xml.etree.ElementTree as ET
import argparse
import logging
import sys
import glob
import re

# These clusters are not implemented in current connectedhomeip repo
EXCLUDE_CLUSTERS = ['Demand Response Load Control', 'Timer']
# These words are special when formatting the cluster name
WORD_FORMAT_LIST = {'Wifi': 'WiFi', 'Pm2.5': 'Pm25', 'Webrtc': 'WebRTC'}

if not os.getenv('ESP_MATTER_PATH'):
    logging.error("ESP_MATTER_PATH environment variable is not set")
    sys.exit(1)

BASE_PATH = os.getenv('ESP_MATTER_PATH')


def get_args():
    parser = argparse.ArgumentParser(description='Script to generate files under zap_common directory',
                                     formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=50))
    parser.add_argument('-x', '--xml-dir',
                        default=os.path.join(
                            BASE_PATH, 'connectedhomeip/connectedhomeip/src/app/zap-templates/zcl/data-model/chip'),
                        help='The directory of the xml template files of all the clusters')
    parser.add_argument('-o', '--output-dir',
                        default=os.path.join(
                            BASE_PATH, 'components/esp_matter/zap_common'),
                        help='The output directory')
    return parser.parse_args()


def format_id(id):
    if id[0:2] != '0x':
        id = hex(int(id))
    hex_id = id[2:]
    if len(hex_id) < 8:
        hex_id = hex_id.zfill(8)
    return '0x' + hex_id.upper()


def get_cluster_name(cluster):
    ret = ''
    for element in cluster:
        if element.tag == 'name':
            ret = element.text
    return ret


def get_cluster_id(cluster):
    for element in cluster:
        if element.tag == 'code':
            return format_id(element.text)


def get_clusters_from_xml_files(xml_files):
    ret = []
    for xml_file in xml_files:
        tree = ET.parse(xml_file)
        root = tree.getroot()
        for child in root:
            if child.tag == 'cluster' and get_cluster_name(child) not in EXCLUDE_CLUSTERS:
                ret.append(child)
    return ret


def word_format(word):
    capitalized_word = word.capitalize()
    if capitalized_word in WORD_FORMAT_LIST.keys():
        return WORD_FORMAT_LIST[capitalized_word]
    else:
        return capitalized_word


def format_cluster_name(cluster_name):
    words = re.split(r'[ /-]', cluster_name)
    capitalized_words = [word_format(word) for word in words]
    return ''.join(capitalized_words)


def get_formatted_cluster_name(cluster):
    return format_cluster_name(get_cluster_name(cluster))


def generate_plugin_application_callbacks_h(xml_files, output_dir):
    with open(os.path.join(output_dir, 'app/PluginApplicationCallbacks.h'), 'w') as header_file:
        header_file.write('#pragma once\n')
        clusters = get_clusters_from_xml_files(xml_files)
        clusters.sort(key=get_formatted_cluster_name)
        for cluster in clusters:
            header_file.write('void Matter{}PluginServerInitCallback();\n'.format(
                format_cluster_name(get_cluster_name(cluster))))

        header_file.writelines(
            ['\n',
             '#include <esp_matter_cluster.h>\n',
             '\n',
             '#define MATTER_PLUGINS_INIT esp_matter::cluster::plugin_init_callback_common();\n'])


def generate_callback_stub_cpp(xml_files, output_dir):
    with open(os.path.join(output_dir, 'app/callback-stub.cpp'), 'w') as src_file:
        src_file.writelines(
            ['#include <app-common/zap-generated/callback.h>\n',
             '\n',
             'using namespace chip;\n'])
        clusters = get_clusters_from_xml_files(xml_files)
        clusters.sort(key=get_formatted_cluster_name)
        for cluster in clusters:
            src_file.write('void __attribute__((weak)) emberAf{}ClusterInitCallback(EndpointId endpoint)\n'.format(
                format_cluster_name(get_cluster_name(cluster))))
            src_file.writelines(
                ['{\n',
                 '    // To prevent warning\n',
                 '    (void) endpoint;\n'
                 '}\n'])


def generate_cluster_init_callback_cpp(xml_files, output_dir):
    with open(os.path.join(output_dir, 'app/cluster-init-callback.cpp'), 'w') as src_file:
        src_file.writelines(
            ['#include <app-common/zap-generated/callback.h>\n',
             '#include <app-common/zap-generated/ids/Clusters.h>\n',
             '#include <lib/support/Span.h>\n',
             '#include <protocols/interaction_model/Constants.h>\n',
             '\n',
             'using namespace chip;\n',
             '\n',
             '// Cluster Init Functions\n',
             'void emberAfClusterInitCallback(EndpointId endpoint, ClusterId clusterId)\n',
             '{\n',
             '    switch (clusterId)\n',
             '    {\n'])
        clusters = get_clusters_from_xml_files(xml_files)
        clusters.sort(key=get_formatted_cluster_name)
        for cluster in clusters:
            formatted_cluster_name = format_cluster_name(
                get_cluster_name(cluster))
            src_file.writelines(
                ['    case app::Clusters::{}::Id:\n'.format(formatted_cluster_name),
                 '        emberAf{}ClusterInitCallback(endpoint);\n'.format(
                     formatted_cluster_name),
                 '        break;\n'])

        src_file.writelines(
            ['    default:\n',
             '        // Unrecognized cluster ID\n',
             '        break;\n',
             '    }\n',
             '}\n'])


def get_attribute_read_privilege(attribute):
    for element in attribute:
        if element.tag == 'access' and element.attrib['op'] == 'read':
            if 'privilege' in element.attrib.keys():
                return element.attrib['privilege']
            elif 'role' in element.attrib.keys():
                return element.attrib['role']
    return 'view'


def get_attribute_write_privilege(attribute):
    for element in attribute:
        if element.tag == 'access' and element.attrib['op'] == 'write':
            if 'privilege' in element.attrib.keys():
                return element.attrib['privilege']
            elif 'role' in element.attrib.keys():
                return element.attrib['role']
    return 'operate'


def get_attribute_name(attribute):
    try:
        return attribute.attrib['name']
    except KeyError:
        for element in attribute:
            if element.tag == 'description':
                return element.text


def get_command_invoke_privilege(command):
    for element in command:
        if element.tag == 'access' and element.attrib['op'] == 'invoke':
            if 'privilege' in element.attrib.keys():
                return element.attrib['privilege']
            elif 'role' in element.attrib.keys():
                return element.attrib['role']
    return 'operate'


def get_event_read_privilege(event):
    for element in event:
        if element.tag == 'access' and element.attrib['op'] == 'read':
            if 'privilege' in element.attrib.keys():
                return element.attrib['privilege']
            elif 'role' in element.attrib.keys():
                return element.attrib['role']
    return 'view'


def get_privilege_sort_key(privilege):
    return privilege[0] + privilege[2]


def get_privileges(clusters):
    attribute_read_privileges = []
    attribute_write_privileges = []
    command_invoke_privileges = []
    event_read_privileges = []

    for cluster in clusters:
        cluster_id = get_cluster_id(cluster)
        cluster_name = get_cluster_name(cluster)
        for element in cluster:
            if element.tag == 'attribute':
                attribute_id = format_id(element.attrib['code'])
                attribute_name = get_attribute_name(element)
                attribute_read_privilege = get_attribute_read_privilege(element)
                attribute_write_privilege = get_attribute_write_privilege(element)

                if attribute_read_privilege != 'view':
                    attribute_read_privileges.append(
                        [cluster_id, cluster_name, attribute_id, attribute_name, attribute_read_privilege])

                if attribute_write_privilege != 'operate':
                    attribute_write_privileges.append(
                        [cluster_id, cluster_name, attribute_id, attribute_name, attribute_write_privilege])

            if element.tag == 'command':
                command_id = format_id(element.attrib['code'])
                command_name = element.attrib['name']
                command_invoke_privilege = get_command_invoke_privilege(element)

                if command_invoke_privilege != 'operate':
                    command_invoke_privileges.append(
                        [cluster_id, cluster_name, command_id, command_name, command_invoke_privilege])

            if element.tag == 'event':
                event_id = format_id(element.attrib['code'])
                event_name = element.attrib['name']
                event_read_privilege = get_event_read_privilege(element)

                if event_read_privilege != 'view':
                    event_read_privileges.append(
                        [cluster_id, cluster_name, event_id, event_name, event_read_privilege])
    # Sort the privileges by cluster_id and attribute_id/command_id/event_id
    attribute_read_privileges.sort(key=get_privilege_sort_key)
    attribute_write_privileges.sort(key=get_privilege_sort_key)
    command_invoke_privileges.sort(key=get_privilege_sort_key)
    event_read_privileges.sort(key=get_privilege_sort_key)
    return attribute_read_privileges, attribute_write_privileges, command_invoke_privileges, event_read_privileges




def get_privileges_array(privileges, array_type, interaction_type, object_type):
    if array_type == 'cluster':
        array = '// Parallel array data (*cluster*, {}, privilege) for {} {}\n'.format(
            object_type, interaction_type, object_type)
    elif array_type == 'attribute' or array_type == 'command' or array_type == 'event':
        array = '// Parallel array data (cluster, *{}*, privilege) for {} {}\n'.format(
            object_type, interaction_type, object_type)
    elif array_type == 'privilege':
        array = '// Parallel array data (cluster, {}, *privilege*) for {} {}\n'.format(
            object_type, interaction_type, object_type)
    else:
        logging.error("Invalid array_type")
        sys.exit(1)

    array += '#define GENERATED_ACCESS_{}_{}__{} {{ \\\n'.format(
        interaction_type.upper(), object_type.upper(), array_type.upper())
    for privilege in privileges:
        if array_type == 'cluster':
            array += '    {}, /* Cluster: {}, {}: {}, Privilege: {} */ \\\n'.format(
                privilege[0], privilege[1], object_type.capitalize(), privilege[3], privilege[4])
        elif array_type == 'attribute' or array_type == 'command' or array_type == 'event':
            array += '    {}, /* Cluster: {}, {}: {}, Privilege: {} */ \\\n'.format(
                privilege[2], privilege[1], object_type.capitalize(), privilege[3], privilege[4])
        elif array_type == 'privilege':
            array += '    chip::Access::Privilege::k{}, /* Cluster: {}, {}: {}, Privilege: {} */ \\\n'.format(
                privilege[4].capitalize(), privilege[1], object_type.capitalize(), privilege[3], privilege[4])

    array += '}\n'
    array += '\n'
    return array


def write_privileges(file, privileges, interaction_type, object_type):
    file.write(get_privileges_array(privileges, 'cluster', interaction_type, object_type))
    file.write(get_privileges_array(privileges, object_type, interaction_type, object_type))
    file.write(get_privileges_array(privileges, 'privilege', interaction_type, object_type))
    file.write('////////////////////////////////////////////////////////////////////////////////\n')
    file.write('\n')


def generate_access_h(xml_files, output_dir):
    with open(os.path.join(output_dir, 'zap-generated/access.h'), 'w') as header_file:
        header_file.writelines(
            ['#pragma once\n',
             '\n',
             '#include <access/Privilege.h>\n',
             '\n',
             '// Prevent changing generated format\n'
             '// clang-format off\n',
             '\n',
             '////////////////////////////////////////////////////////////////////////////////\n',
             '\n'])
        # Get privileges from the xml files
        clusters = get_clusters_from_xml_files(xml_files)
        [attr_read, attr_write, cmd_invoke, event_read] = get_privileges(clusters)
        write_privileges(header_file, attr_read, 'read', 'attribute')
        write_privileges(header_file, attr_write, 'write', 'attribute')
        write_privileges(header_file, cmd_invoke, 'invoke', 'command')
        write_privileges(header_file, event_read, 'read', 'event')

        header_file.write('// clang-format on\n')


def main():
    logging.basicConfig(
        format='[%(asctime)s] [%(levelname)7s] - %(message)s', level=logging.INFO)
    args = get_args()
    xml_files = glob.glob(os.path.join(args.xml_dir, '*.xml'))
    generate_plugin_application_callbacks_h(xml_files, args.output_dir)
    generate_callback_stub_cpp(xml_files, args.output_dir)
    generate_cluster_init_callback_cpp(xml_files, args.output_dir)
    generate_access_h(xml_files, args.output_dir)


if __name__ == "__main__":
    main()
