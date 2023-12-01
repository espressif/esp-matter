#!/usr/bin/env python3

# Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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
This file contains the CHIP specific key along with the data type and encoding format
"""

# This map contains mandatory CHIP specific key along with the data type and encoding format
# Additionally to add more keys to chip-factory, use chip_factory_append() API

import csv

CHIP_NVS_MAP = {
    'chip-factory': {
        # Commissionable Data
        'discriminator': {
            'type': 'data',
            'encoding': 'u32',
            'value': None,
        },
        'iteration-count': {
            'type': 'data',
            'encoding': 'u32',
            'value': None,
        },
        'salt': {
            'type': 'data',
            'encoding': 'string',
            'value': None,
        },
    }
}


def get_dict(key, type, encoding, value):
    return {
        key: {
            'type': type,
            'encoding': encoding,
            'value': value,
        }
    }


def get_namespace_dict(namespace):
    return {
        namespace: {
        }
    }


def chip_nvs_get_config_csv():
    csv_data = ''
    for k, v in CHIP_NVS_MAP.items():
        csv_data += k + ',' + 'namespace,' + '\n'
        for k1, v1 in v.items():
            csv_data += k1 + ',' + v1['type'] + ',' + v1['encoding'] + '\n'
    return csv_data


def chip_factory_append(key, type, encoding, value):
    CHIP_NVS_MAP['chip-factory'].update(get_dict(key, type, encoding, value))


def chip_factory_update(key, value):
    CHIP_NVS_MAP['chip-factory'][key]['value'] = value


def chip_nvs_map_update(namespace, key, type, encoding, value):
    CHIP_NVS_MAP[namespace].update(get_dict(key, type, encoding, value))

def chip_nvs_map_append_config_csv(csv_path):
    with open(csv_path, 'r') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        # Set current namespace to 'chip-factory' when the first line of the csv file is not 'XX,namespace,'
        current_namespace = 'chip-factory'
        for csv_data in csv_reader:
            if 'namespace' in csv_data:
                current_namespace = csv_data[0]
                if (current_namespace not in list(CHIP_NVS_MAP.keys())):
                    CHIP_NVS_MAP.update(get_namespace_dict(current_namespace))
            else:
                chip_nvs_map_update(current_namespace, csv_data[0], csv_data[1], csv_data[2], None)


def chip_factory_get_val(key):
    return CHIP_NVS_MAP['chip-factory'][key]['value']


def chip_get_keys_as_csv():
    keys = list()
    for ns in CHIP_NVS_MAP:
        keys.extend(list(CHIP_NVS_MAP[ns]))
    return ','.join(keys)


def chip_get_values_as_csv():
    values = list()
    for ns in CHIP_NVS_MAP:
        for k in CHIP_NVS_MAP[ns]:
            values.append(str(CHIP_NVS_MAP[ns][k]['value']))
    return ','.join(values)
