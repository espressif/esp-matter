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

import re

# Plugin init callback pattern
PLUGIN_CB_PATTERN = re.compile(r"\bvoid\s+Matter(\w+)PluginServerInitCallback\s*\(")

# Matches a delegate member field: <type containing Delegate> [&*] <var>delegate (e.g. mDelegate, appDelegate)
DELEGATE_MEMBER_RE = re.compile(r"[\w:]*Delegate\w*\s*[&*]\s*[a-z]*[Dd]elegate\b")

# Matches delegate accessor methods: Set/SetDefault/Get/WithDelegate
DELEGATE_METHOD_RE = re.compile(r"\b(SetDefault|Set|Get|With)Delegate\b")

# Skip delegates other than application cluster delegates
SKIP_DELEGATE_RE = re.compile(
    r"(?:"
    r"\b(?:(?:Timer|Fabric|PlatformManager|ConfigurationVersion)Delegate)\b|"
    r"PlatformManager\.|"
    r"RegisterPlatformDelegate|UnregisterPlatformDelegate"
    r")"
)

# Regex to capture case statements like case <attribute_name>:Id:
ATTRIBUTE_REGEX = re.compile(r"case\s+([\w:]+)(?:::Id)?:")

# Regex to capture Read, ReadAttribute, and ReadImpl functions
READ_FUNC_PATTERN = re.compile(
    r"(?:Read|ReadAttribute|ReadImpl)\s*\([^)]*\)\s*(?:const)?\s*(?:override)?\s*\{"
)

# Regex to capture attribute names like namespace <cluster_name> { inline constexpr AttributeId Id = <attribute_id>; }
ATTRIBUTE_PATTERN = re.compile(
    r"namespace\s+(\w+)\s*{[\s\n]*inline\s+constexpr\s+AttributeId\s+Id\s*=\s*([\w:]+);"
)
# Regex to capture command names like namespace <cluster_name> { inline constexpr CommandId Id = <command_id>; }
COMMAND_PATTERN = re.compile(
    r"namespace\s+(\w+)\s*{[\s\n]*inline\s+constexpr\s+CommandId\s+Id\s*=\s*([\w:]+);"
)
# Regex to capture event names like namespace <cluster_name> { inline constexpr EventId Id = <event_id>; }
EVENT_PATTERN = re.compile(
    r"namespace\s+(\w+)\s*{[\s\n]*inline\s+constexpr\s+EventId\s+Id\s*=\s*([\w:]+);"
)
# Regex to capture cluster id like inline constexpr ClusterId Id = <cluster_id>;
CLUSTER_ID_PATTERN = re.compile(r"inline\s+constexpr\s+ClusterId\s+Id\s*=\s*([\w:]+);")

# Search for these function names in server files for internally managed attributes
KEYWORDS = ["Read(", "ReadAttribute(", "ReadImpl("]

# Local mappings to override cluster names as per the Matter specification
local_mappings = {
    "dishwasher": "dish_washer",
    "dishwasher_alarm": "dish_washer_alarm",
    "dishwasher_mode": "dish_washer_mode",
    "ota_requestor": "ota_software_update_requestor",
    "ota_provider": "ota_software_update_provider",
    "group_key_mgmt": "group_key_management",
    "bindings": "binding",
    "boolean_state": "boolean_state_configuration",
}

# List of clusters to skip for delegate scanning
delegate_skip_list = [
    "ota_software_update_requestor",
    "bridged_device_basic_information",
    "identify",
]
