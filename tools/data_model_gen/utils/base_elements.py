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
from abc import ABC, abstractmethod
from xml.etree import ElementTree
from typing import List
from utils.overrides import (
    is_cpp_reserved_word,
    normalize_cluster_display_name,
    normalize_device_type_name,
    normalize_feature_name,
)
from utils.helper import chip_name, convert_to_snake_case, esp_name


class BaseElement(ABC):
    """Base class for all elements in the Matter data model"""

    def __init__(self, name, id, element_type):
        assert name, "Name is required"
        self.element_type = element_type
        self.id = id
        self.name = name.replace(" ", "_")
        self.esp_name = esp_name(name)
        self.chip_name = chip_name(name)
        self.func_name = convert_to_snake_case(name)

    def get_id(self):
        """Get the ID of the element"""
        return self.id


class BaseClusterElement(BaseElement):
    """Base class for elements within a cluster"""

    def __init__(self, name, id, is_mandatory, element_type):
        if name and is_cpp_reserved_word(name):
            name = f"{name}_{element_type}"
        if name and element_type == "Cluster":
            name = normalize_cluster_display_name(
                name, cluster_id=id if element_type == "Cluster" else None
            )
        super().__init__(name=name, id=id, element_type=element_type)
        self.is_mandatory = is_mandatory


class BaseAttribute(BaseClusterElement):
    """Base class for attributes"""

    def __init__(self, name, id, type_, is_mandatory, default_value):
        super().__init__(
            name=name, id=id, is_mandatory=is_mandatory, element_type="Attribute"
        )
        self.type = type_
        self.default_value = default_value
        self.is_nullable = False


class BaseCommand(BaseClusterElement):
    """Base class for commands"""

    def __init__(self, name, id, is_mandatory, direction, response):
        super().__init__(
            name=name, id=id, is_mandatory=is_mandatory, element_type="Command"
        )
        self.direction = direction
        self.response = response


class BaseEvent(BaseClusterElement):
    """Base class for events"""

    def __init__(self, name, id, is_mandatory):
        super().__init__(
            name=name, id=id, is_mandatory=is_mandatory, element_type="Event"
        )


class BaseFeature(BaseClusterElement):
    """Base class for features"""

    def __init__(self, name, id, is_mandatory):
        name = normalize_feature_name(name, feature_id=id)
        super().__init__(
            name=name, id=id, is_mandatory=is_mandatory, element_type="Feature"
        )

    @abstractmethod
    def get_attributes(self) -> List[BaseAttribute]:
        """Get the list of attributes"""
        pass

    @abstractmethod
    def get_commands(self) -> List[BaseCommand]:
        """Get the list of commands"""
        pass

    @abstractmethod
    def get_events(self) -> List[BaseEvent]:
        """Get the list of events"""
        pass


class BaseCluster(BaseClusterElement):
    """Base class for clusters"""

    def __init__(self, name, id, revision, is_mandatory):
        super().__init__(
            name=name, id=id, is_mandatory=is_mandatory, element_type="Cluster"
        )
        self.revision = revision
        self.server_cluster = False
        self.client_cluster = False
        self.command_handler_available = False
        self.init_function_available = False
        self.attribute_changed_function_available = False
        self.shutdown_function_available = False
        self.pre_attribute_change_function_available = False
        self.delegate_init_callback_available = False
        self.plugin_init_cb_available = False

        self.delegate_init_callback = None
        self.plugin_server_init_callback = None
        self.role = None

    def get_revision(self):
        """Get the revision of the cluster"""
        return self.revision

    @abstractmethod
    def get_attributes(self) -> List[BaseAttribute]:
        """Get the list of attributes"""
        pass

    @abstractmethod
    def get_commands(self) -> List[BaseCommand]:
        """Get the list of commands"""
        pass

    @abstractmethod
    def get_events(self) -> List[BaseEvent]:
        """Get the list of events"""
        pass

    @abstractmethod
    def get_features(self) -> List[BaseFeature]:
        """Get the list of features"""
        pass


class BaseDevice(BaseElement):
    """Base class for devices"""

    def __init__(self, name, id, revision):
        name = normalize_device_type_name(name, device_id=id)
        super().__init__(name=name, id=id, element_type="Device")
        self.filename = self.esp_name + "_device"
        self.revision = revision

    def get_device_type_id(self):
        """Get the device type ID"""
        return self.id

    def get_device_type_version(self):
        """Get the device type version"""
        return self.revision

    @abstractmethod
    def get_clusters(self) -> List[BaseCluster]:
        """Get the list of clusters"""
        pass


class BaseElementParser(ABC):
    """Base class for element parsers"""

    @abstractmethod
    def parse(self, element: ElementTree.Element):
        raise NotImplementedError
