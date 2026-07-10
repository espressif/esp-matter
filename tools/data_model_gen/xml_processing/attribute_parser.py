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

from .conformance_parser import parse_conformance, is_mandatory
from .data_type_parser import resolve_attribute_type, resolve_attribute_bounds
from .element_parser_base import ClusterElementBaseParser
from .elements import Attribute, Cluster
from .attribute_type import attribute_type_map
from utils.overrides import should_skip_internally_managed_flag
from typing import List
from utils.helper import safe_get_attr

logger = logging.getLogger(__name__)


class AttributeParser(ClusterElementBaseParser):
    """Parses cluster attributes from XML."""

    def __init__(
        self,
        cluster: Cluster,
        feature_map: dict,
        managed_attributes: list,
        allowed_attributes_ids: list = None,
        base_attributes: List[Attribute] = None,
    ):
        super().__init__(
            cluster, feature_map, allowed_attributes_ids or [], base_attributes or []
        )
        self.managed_attributes = managed_attributes if managed_attributes else []

    def parse(self, root) -> None:
        """
        Parse attributes from cluster XML root and add to cluster.attributes. Merges base_attributes not already present.
        Raises:
            XmlParseError: From resolve_attribute_type if an attribute has no type.
        """
        logger.debug(
            f"Parsing attributes for cluster {safe_get_attr(self.cluster, 'name')}"
        )
        for elem in root.findall("attributes/attribute"):
            skip, reason = self.can_skip(elem)
            if skip:
                logger.debug("Skipping attribute %s: %s", elem.get("name"), reason)
                continue
            attr = self.create(elem)
            self._apply_type_overrides(attr, elem)
            self.cluster.attributes.add(attr)

        for base_attr in self.base_elements:
            # check if the base attribute is already processed
            # priority is given to derived cluster attributes over base attributes
            if base_attr.name not in self.processed:
                if base_attr.conformance is not None:
                    base_attr.conformance.feature_map = self.feature_map
                    if base_attr.conformance.is_disallowed():
                        continue
                self.cluster.attributes.add(base_attr)

    def _fill_from_base(self, elem: Element, base):
        super()._fill_from_base(elem, base)
        if not elem.get("type") and getattr(base, "type", None):
            elem.set("type", base.type)

    def create(self, elem: Element) -> Attribute:
        """
        Build one Attribute from XML element.
        Raises:
            XmlParseError: If type cannot be resolved (from resolve_attribute_type).
        """
        name = elem.get("name")
        code = elem.get("id")
        type_str = resolve_attribute_type(elem, self.cluster.attribute_types)
        attr = Attribute(
            name=name,
            id=code,
            type_=type_str,
            is_mandatory=is_mandatory(elem),
            access=self._access(elem.find("access")),
            quality=self._quality(elem.find("quality")),
            constraint=self._constraint(elem.find("constraint")),
            default_value=elem.get("default"),
        )
        self._set_internally_managed(attr)
        resolve_attribute_bounds(attr, elem, self.cluster.data_types)
        attr.conformance = parse_conformance(elem, self.feature_map)
        return attr

    def _apply_type_overrides(self, attr: Attribute, elem: Element) -> None:
        """some cluster/attribute types are inferred wrong from XML"""
        cluster_name = self.cluster.func_name
        if (
            cluster_name not in attribute_type_map
            or attr.func_name not in attribute_type_map[cluster_name]
        ):
            return
        override_attr = attribute_type_map[cluster_name][attr.func_name]
        attr.type = override_attr["type"]
        attr.min_value = override_attr.get("min", None)
        attr.max_value = override_attr.get("max", None)
        attr.default_value = str(override_attr.get("default", None))

    def _set_internally_managed(self, attr: Attribute) -> None:
        if attr.type == "list":
            attr.internally_managed = True
            return
        if self.cluster.is_migrated_cluster:
            attr.internally_managed = False
            return
        if should_skip_internally_managed_flag(self.cluster.id, attr.id):
            attr.internally_managed = False
        elif attr.name and attr.name.lower() in self.managed_attributes:
            attr.internally_managed = True
        else:
            attr.internally_managed = False

    def _access(self, access_elem: Element):
        if access_elem is None:
            return None
        return Attribute.Access(
            read=access_elem.get("read", "false"),
            readPrivilege=access_elem.get("readPrivilege"),
            write=access_elem.get("write", "false"),
            writePrivilege=access_elem.get("writePrivilege"),
        )

    def _quality(self, quality_elem: Element):
        if quality_elem is None:
            return None
        return Attribute.Quality(
            changeOmitted=quality_elem.get("changeOmitted", "false"),
            nullable=quality_elem.get("nullable", "false"),
            scene=quality_elem.get("scene", "false"),
            persistence=quality_elem.get("persistence", ""),
            reportable=quality_elem.get("reportable", "false"),
            sourceAttribution=quality_elem.get("sourceAttribution", "false"),
            quieterReporting=quality_elem.get("quieterReporting", "false"),
        )

    def _constraint(self, constraint_elem: Element):
        if constraint_elem is None:
            return None
        _CONSTRAINT_TAG_HANDLERS = {
            "maxLength": ("maxLength", lambda c: c.get("value", "")),
            "min": ("min", lambda c: c.get("value", "")),
            "max": ("max", lambda c: c.get("value", "")),
        }
        ctype, cfrom, cto, cval = None, None, None, None
        for child in constraint_elem:
            if child.tag == "between":
                ctype = "between"
                from_el = child.find("from")
                to_el = child.find("to")
                cfrom = from_el.get("value", "0") if from_el is not None else "0"
                cto = to_el.get("value", "0") if to_el is not None else "0"
            elif child.tag == "desc":
                ctype = "desc"
                cval = (child.text or "").strip() or None
            elif child.tag in _CONSTRAINT_TAG_HANDLERS:
                ctype, getter = _CONSTRAINT_TAG_HANDLERS[child.tag]
                cval = getter(child)
        if ctype == "between":
            return Attribute.Constraint(type=ctype, from_=cfrom, to_=cto, value=None)
        return Attribute.Constraint(type=ctype, from_=cval, to_=cval, value=cval)
