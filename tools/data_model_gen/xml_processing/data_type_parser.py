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
from typing import Dict
from xml.etree.ElementTree import Element

from .attribute_type import attribute_types
from utils.conversion_utils import convert_to_int
from utils.helper import safe_get_attr
from utils.exceptions import XmlParseError

logger = logging.getLogger(__name__)

# Named per-type integer limits (avoid magic numbers below).
INT8_MIN, INT8_MAX = -(2**7), 2**7 - 1
INT16_MIN, INT16_MAX = -(2**15), 2**15 - 1
INT32_MIN, INT32_MAX = -(2**31), 2**31 - 1
UINT8_MAX = 2**8 - 1
UINT16_MAX = 2**16 - 1
UINT32_MAX = 2**32 - 1

# NOTE: 64-bit types are capped to the 32-bit range to avoid esp-idf compiler warnings
# on 64-bit literal bounds.
INT_BOUNDS = {
    "int8": (INT8_MIN, INT8_MAX),
    "int16": (INT16_MIN, INT16_MAX),
    "int32": (INT32_MIN, INT32_MAX),
    "int64": (INT32_MIN, INT32_MAX),
    "uint8": (0, UINT8_MAX),
    "uint16": (0, UINT16_MAX),
    "uint32": (0, UINT32_MAX),
    "uint64": (0, UINT32_MAX),
}


def resolve_attribute_type(attribute_elem: Element, attribute_types_dict: dict) -> str:
    """
    Resolve attribute type from XML using cluster type map. Widens enum/bitmap by default value.
    Raises:
        XmlParseError: If attribute has no type and it cannot be inferred.
    """
    raw = attribute_elem.get("type")
    if raw is None:
        raise XmlParseError(
            f"Attribute type is None for {attribute_elem}",
            context="resolve_attribute_type",
            suggestion="Ensure the attribute element has a 'type' attribute in the XML.",
        )
    type_str = raw.split(" ")[0].lower()
    resolved = attribute_types_dict.get(type_str)
    if resolved is None:
        resolved = _fallback_unknown_type(type_str)
    return _override_type_by_default_value(attribute_elem, resolved)


def _fallback_unknown_type(type_str: str) -> str:
    """some XML types missing; map to concrete type."""
    if "bitmap" in type_str.lower():
        return "bitmap8"
    if "enum" in type_str.lower():
        return "enum8"
    if "struct" in type_str.lower():
        return "list"
    return type_str


def _override_type_by_default_value(attribute_elem: Element, type_str: str) -> str:
    default = attribute_elem.get("default")
    if default is None:
        return type_str
    try:
        if isinstance(default, str) and default.startswith("0x"):
            val = int(default, 16)
        elif isinstance(default, str) and default.isdigit():
            val = int(default)
        else:
            return type_str
    except ValueError:
        return type_str
    if val > 255 and type_str == "enum8":
        return "enum16"
    if val > 65535 and type_str == "enum16":
        return "enum32"
    if val > 4294967295 and type_str == "enum32":
        return "enum64"
    if val > 255 and type_str == "bitmap8":
        return "bitmap16"
    if val > 65535 and type_str == "bitmap16":
        return "bitmap32"
    if val > 4294967295 and type_str == "bitmap32":
        return "bitmap64"
    return type_str


def resolve_attribute_bounds(
    attr, attribute_elem: Element, data_types_dict: dict
) -> None:
    """Set attr.min_value and attr.max_value from enum, bitmap, constraint, or type defaults."""
    attr.min_value = None
    attr.max_value = None
    data_types_dict = data_types_dict or {}
    enums = data_types_dict.get("enums") or {}
    bitmaps = data_types_dict.get("bitmaps") or {}

    if "enum" in (attr.type or "").lower():
        xml_type = (attribute_elem.get("type") or "").lower()
        if xml_type in enums and safe_get_attr(enums[xml_type], "items"):
            attr.min_value = 0
            attr.max_value = len(enums[xml_type].items) - 1
            _bounds_to_int(attr)
            return
    if "bitmap" in (attr.type or "").lower():
        xml_type = (attribute_elem.get("type") or "").lower()
        if xml_type in bitmaps and safe_get_attr(bitmaps[xml_type], "bitfields"):
            attr.min_value = 0
            attr.max_value = 2 ** len(bitmaps[xml_type].bitfields) - 1
            _bounds_to_int(attr)
            return

    _bounds_from_constraint(attr, attribute_elem)
    _normalize_bounds(attr)
    _default_bounds_by_type(attr)
    _bounds_to_int(attr)


def _bounds_from_constraint(attr, attribute_elem: Element) -> None:
    constraint_elem = attribute_elem.find("constraint")
    if not constraint_elem:
        return
    min_el = constraint_elem.find("min")
    if min_el is not None and min_el.get("value") is not None:
        attr.min_value = min_el.get("value", None)
    max_el = constraint_elem.find("max")
    if max_el is not None and max_el.get("value") is not None:
        attr.max_value = max_el.get("value", None)
    between = constraint_elem.find("between")
    if between is not None:
        f, t = between.find("from"), between.find("to")
        if f is not None and f.get("value") is not None:
            attr.min_value = f.get("value", None)
        if t is not None and t.get("value") is not None:
            attr.max_value = t.get("value", None)
    length_between = constraint_elem.find("lengthBetween")
    if length_between is not None:
        f, t = length_between.find("from"), length_between.find("to")
        if f is not None and f.get("value") is not None:
            attr.min_value = f.get("value", None)
        if t is not None and t.get("value") is not None:
            attr.max_value = t.get("value", None)
    max_len = constraint_elem.find("maxLength")
    if max_len is not None and max_len.get("value") is not None:
        attr.max_value = max_len.get("value", None)
        if attr.min_value is None:
            attr.min_value = 0
    allowed = constraint_elem.find("allowed")
    if allowed is not None and (attr.type == "string" or attr.type == "octstr"):
        allowed_value = convert_to_int(allowed.get("value", None))
        if allowed_value is not None:
            attr.max_value = allowed_value
            attr.min_value = 0


def _normalize_bounds(attr) -> None:
    for key in ("min_value", "max_value"):
        v = getattr(attr, key)
        if v is None:
            continue
        if not isinstance(v, str):
            continue
        if "0x" in v:
            try:
                setattr(attr, key, int(v, 16))
            except ValueError:
                setattr(attr, key, None)
        elif not v.lstrip("-").isdigit():
            setattr(attr, key, None)


def _default_bounds_by_type(attr) -> None:
    """Set the default bounds for the attribute based on the type. Only fill a missing
    side from the type range when the other side is a real (spec-derived) constraint."""
    if attr.min_value is None and attr.max_value is None:
        return

    bounds = INT_BOUNDS.get(attr.type)
    if not bounds:
        return

    min_val, max_val = bounds

    if attr.min_value is None:
        attr.min_value = min_val

    if attr.max_value is None:
        attr.max_value = max_val


def _bounds_to_int(attr) -> None:
    if attr.min_value is not None:
        attr.min_value = int(attr.min_value)
    if attr.max_value is not None:
        attr.max_value = int(attr.max_value)


class Item:
    """Class representing an item in a data type (enum, bitmap, struct)"""

    def __init__(self, name, value, summary, is_mandatory):
        self.name = name
        self.value = value
        self.summary = summary
        self.is_mandatory = is_mandatory

    def to_dict(self):
        """Convert item to dictionary representation"""
        return {
            "name": self.name,
            "value": self.value,
            "summary": self.summary,
            "is_mandatory": self.is_mandatory,
        }


class Enum:
    """Class representing an enumeration data type"""

    def __init__(self, name, base_type, items):
        self.name: str = name
        self.base_type: str = base_type
        self.items: list[Item] = items

    def to_dict(self):
        """Convert enum to dictionary representation"""
        return {
            "name": self.name,
            "base_type": self.base_type,
            "items": [item.to_dict() for item in self.items],
        }


class Bitmap:
    """Class representing a bitmap data type"""

    def __init__(self, name, base_type, bitfields):
        self.name: str = name
        self.base_type: str = base_type
        self.bitfields: list[Item] = bitfields

    def to_dict(self):
        """Convert bitmap to dictionary representation"""
        return {
            "name": self.name,
            "base_type": self.base_type,
            "bitfields": [bitfield.to_dict() for bitfield in self.bitfields],
        }


class Struct:
    """Class representing a struct data type"""

    def __init__(self, name, base_type, fields):
        self.name: str = name
        self.base_type: str = base_type
        self.fields: list[Item] = fields

    def to_dict(self):
        """Convert struct to dictionary representation"""
        return {
            "name": self.name,
            "base_type": self.base_type,
            "fields": [field.to_dict() for field in self.fields],
        }


def _infer_type_by_count(
    count: int, type_prefix: str, thresholds: tuple = (8, 16)
) -> str:
    """Infer type size based on item count. Used for both enum and bitmap."""
    if count < thresholds[0]:
        return f"{type_prefix}8"
    if count < thresholds[1]:
        return f"{type_prefix}16"
    return f"{type_prefix}32"


class DataTypeParser:
    """Parser for Matter data types"""

    def __init__(self):
        self.attribute_types: Dict[str, str] = dict(attribute_types)
        self.enums: Dict[str, Enum] = {}
        self.bitmaps: Dict[str, Bitmap] = {}
        self.structs: Dict[str, Struct] = {}

    def parse(self, root: Element) -> Dict[str, str]:
        """Parse dataTypes section and create type mapping."""
        logger.debug("Parsing dataTypes section")
        data_types = root.find("dataTypes")
        if data_types is None:
            return self.attribute_types

        self._parse_enums(data_types)
        self._parse_bitmaps(data_types)
        self._parse_structs(data_types)

        return self.attribute_types

    def _parse_enums(self, data_types: Element) -> None:
        for enum in data_types.findall("enum"):
            enum_name = enum.get("name", "").lower()
            if not enum_name:
                continue
            items = enum.findall("item")
            base_type = _infer_type_by_count(len(items), "enum", (256, 65536))
            self.attribute_types[enum_name] = base_type
            self.enums[enum_name] = Enum(
                enum.get("name"),
                base_type,
                [
                    Item(
                        item.get("name"),
                        item.get("value"),
                        item.get("summary"),
                        item.find("mandatoryConform") is not None,
                    )
                    for item in items
                ],
            )

    def _parse_bitmaps(self, data_types: Element) -> None:
        for bitmap in data_types.findall("bitmap"):
            bitmap_name = bitmap.get("name", "").lower()
            if not bitmap_name:
                continue
            bitfields = bitmap.findall("bitfield")
            base_type = _infer_type_by_count(len(bitfields), "bitmap")
            self.attribute_types[bitmap_name] = base_type
            self.bitmaps[bitmap_name] = Bitmap(
                bitmap.get("name"),
                base_type,
                [
                    Item(
                        bf.get("name"),
                        bf.get("bit"),
                        bf.get("summary"),
                        bf.find("mandatoryConform") is not None,
                    )
                    for bf in bitfields
                ],
            )

    def _parse_structs(self, data_types: Element) -> None:
        for struct in data_types.findall("struct"):
            struct_name = struct.get("name", "").lower()
            if not struct_name:
                continue
            self.attribute_types[struct_name] = "list"
            self.structs[struct_name] = Struct(
                struct.get("name", ""),
                "list",
                [
                    Item(
                        field.get("name"),
                        field.get("type"),
                        field.get("summary"),
                        field.find("mandatoryConform") is not None,
                    )
                    for field in struct.findall("field")
                ],
            )

    def get_data_types(self) -> Dict[str, Dict]:
        """Return all parsed data types."""
        return {"enums": self.enums, "bitmaps": self.bitmaps, "structs": self.structs}
