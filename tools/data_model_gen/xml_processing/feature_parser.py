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
from xml.etree.ElementTree import Element
from typing import List
from .conformance_parser import parse_conformance, match_conformance_items
from .elements import Feature
from .conformance_parser import is_restricted_by_conformance
from .element_parser_base import ClusterElementBaseParser, inherits_from_base

logger = logging.getLogger(__name__)


class FeatureParser(ClusterElementBaseParser):
    """Parses cluster features from XML and links them to attributes/commands/events via conformance."""

    def __init__(self, root: Element, cluster, base_features: List[Feature]):
        super().__init__(cluster, {}, [], base_features)
        self.feature_map = self._generate_feature_map(root, base_features)

    def parse(self, root: Element):
        """Parse features from cluster XML and add it to the cluster
        NOTE: This function expect the feature map to be created and populated before calling this function
        """
        for elem in root.findall("features/feature"):
            if inherits_from_base(self.cluster, elem):
                continue
            elem.set("id", str(self._generate_feature_id(elem)))
            skip, reason = self.can_skip(elem)
            if skip:
                logger.debug("Skipping feature %s: %s", elem.get("name"), reason)
                self.feature_map.pop(elem.get("code"), None)
                continue
            feature = self.create(elem)
            feature.conformance = parse_conformance(elem, self.feature_map)
            self._link_feature_items(feature)
            self.cluster.features.add(feature)
        for base_feature in self.base_elements:
            if (
                base_feature.name not in self.processed
                and base_feature.code in self.feature_map.keys()
            ):
                self.cluster.features.add(base_feature)

    def create(self, elem: Element) -> Feature:
        name = elem.get("name")
        code = elem.get("code")
        feature_id = self._generate_feature_id(elem)
        feature = Feature(name=name, code=code, id=feature_id)
        return feature

    def _generate_feature_map(
        self, root: Element, base_features: List[Feature]
    ) -> dict:
        """Build valid {code: Feature} map"""
        logger.debug(
            f"Creating feature map for the cluster {root.get('name', 'Unknown')}"
        )
        feature_map = {}
        for feature_elem in root.findall("features/feature"):
            if inherits_from_base(self.cluster, feature_elem):
                continue
            feature = self.create(feature_elem)
            feature_map[feature.code] = feature
        for base_feature in base_features:
            if base_feature.code not in feature_map:
                feature_map[base_feature.code] = base_feature
        for feature_elem in root.findall("features/feature"):
            if is_restricted_by_conformance(feature_map, feature_elem):
                feature_map.pop(feature_elem.get("code"), None)
        return feature_map

    def _generate_feature_id(self, elem: Element) -> hex:
        bit = elem.get("bit")
        return hex(0x1 << int(bit)) if bit is not None else 0

    def _link_feature_items(self, feature: Feature) -> None:
        attrs = match_conformance_items(feature, self.cluster.get_attributes())
        if attrs:
            feature.add_attribute_list(attrs)
        cmds = match_conformance_items(feature, self.cluster.get_commands())
        if cmds:
            feature.add_command_list(cmds)
        evts = match_conformance_items(feature, self.cluster.get_events())
        if evts:
            feature.add_event_list(evts)
