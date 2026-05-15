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
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element
from .elements import Cluster, Device
from .conformance_parser import parse_choice
from utils.helper import check_valid_id, convert_to_snake_case, safe_get_attr

logger = logging.getLogger(__name__)


def is_mandatory(element: Element) -> bool:
    mandatory_conform = element.find("mandatoryConform")
    return mandatory_conform is not None and len(mandatory_conform) == 0


class DeviceParser:
    def can_skip(self, element: Element):
        if not element.get("name"):
            return True, "name is missing"
        if not element.get("id"):
            return True, "id is missing"
        if not check_valid_id(element.get("id")):
            return True, "id is not valid"
        return False, "Unknown reason"

    def parse(self, file_path):
        """Parse a device XML file and return the parsed device object.

        :param file_path:
        :returns: The parsed device object.

        """
        tree = ET.parse(file_path)
        root = tree.getroot()

        skip, reason = self.can_skip(root)
        if skip:
            logger.warning(f"Skipping {file_path} reason : {reason}")
            return None

        device = self.create_device(root)
        for cluster_elem in root.findall("clusters/cluster"):
            skip, reason = self.can_skip(cluster_elem)
            if skip:
                logger.warning(
                    f"Skipping cluster {cluster_elem.get('name')} reason : {reason}"
                )
                continue
            cluster = self.create_cluster(cluster_elem)
            device.clusters.add(cluster)
        logger.debug(
            f"Processed device {safe_get_attr(device, 'name')} successfully with {len(device.clusters)} clusters"
        )
        return device

    def create_device(self, root: Element):
        """Create a device object from the device XML element.
        Assumes device has valid id and name.
        """
        device = Device(
            id=root.get("id"),
            name=root.get("name"),
            revision=root.get("revision", "Unknown"),
        )
        device.revision_history = self._get_revision_history(root)
        device.classification = self._get_classification(root)
        device.conditions = self._get_conditions(root)
        return device

    def create_cluster(self, cluster_elem: Element):
        """Create a cluster object from the cluster XML element.
        Assumes cluster has valid id and name.
        """
        cluster = Cluster(
            name=cluster_elem.get("name"), id=cluster_elem.get("id"), revision=None
        )
        cluster.server_cluster = cluster_elem.get("side") == "server"
        cluster.client_cluster = cluster_elem.get("side") == "client"

        mandatory_conform = cluster_elem.find("mandatoryConform")
        if (
            mandatory_conform is not None
            and mandatory_conform.find("condition") is None
        ):
            cluster.is_mandatory = True

        optional_conform = cluster_elem.find("optionalConform")
        if optional_conform is not None:
            choice = parse_choice(optional_conform)
            if choice is not None:
                cluster.optional_choice = choice.to_dict()

        attribute_list = []
        for attribute_elem in cluster_elem.findall("attributes/attribute"):
            if is_mandatory(attribute_elem):
                attribute_list.append(convert_to_snake_case(attribute_elem.get("name")))
        cluster.attribute_name_list = attribute_list

        feature_list = []
        for feature_elem in cluster_elem.findall("features/feature"):
            if is_mandatory(feature_elem):
                feature_name = (
                    feature_elem.get("name")
                    if feature_elem.get("name")
                    else feature_elem.get("code")
                )
                feature_list.append(convert_to_snake_case(feature_name))
        cluster.feature_name_list = feature_list

        command_list = []
        for command_elem in cluster_elem.findall("commands/command"):
            if is_mandatory(command_elem):
                command_list.append(convert_to_snake_case(command_elem.get("name")))
        cluster.command_name_list = command_list

        event_list = []
        for event_elem in cluster_elem.findall("events/event"):
            if is_mandatory(event_elem):
                event_list.append(convert_to_snake_case(event_elem.get("name")))
        cluster.event_name_list = event_list

        return cluster

    def _get_revision_history(self, root):
        revision_history = []
        for revision in root.findall("revisionHistory/revision"):
            revision_history.append(
                {
                    "revision": revision.get("revision"),
                    "summary": revision.get("summary"),
                }
            )
        return revision_history

    def _get_classification(self, root):
        classification = {}
        for classification_elem in root.findall("classification"):
            for attr_name, attr_value in classification_elem.attrib.items():
                classification[attr_name] = attr_value
        return classification

    def _get_conditions(self, root):
        conditions = []
        for condition_elem in root.findall("conditions/condition"):
            conditions.append(
                {
                    "name": condition_elem.get("name"),
                    "summary": condition_elem.get("summary"),
                }
            )
        return conditions
