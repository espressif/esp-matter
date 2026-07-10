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

"""Tests for xml_processing/attribute_type.py — Matter type to C type mappings."""

import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from xml_processing.attribute_type import (  # noqa: E402
    AttributeType,
    attribute_types,
    attribute_type_map,
)


class TestAttributeType(unittest.TestCase):
    """Test AttributeType class — type string to C type mapping."""

    def test_uint8(self):
        self.assertEqual(AttributeType("uint8").get_attribute_type(), "uint8_t")

    def test_uint16(self):
        self.assertEqual(AttributeType("uint16").get_attribute_type(), "uint16_t")

    def test_uint32(self):
        self.assertEqual(AttributeType("uint32").get_attribute_type(), "uint32_t")

    def test_uint64(self):
        self.assertEqual(AttributeType("uint64").get_attribute_type(), "uint64_t")

    def test_int8(self):
        self.assertEqual(AttributeType("int8").get_attribute_type(), "int8_t")

    def test_int16(self):
        self.assertEqual(AttributeType("int16").get_attribute_type(), "int16_t")

    def test_int32(self):
        self.assertEqual(AttributeType("int32").get_attribute_type(), "int32_t")

    def test_int64(self):
        self.assertEqual(AttributeType("int64").get_attribute_type(), "int64_t")

    def test_bool(self):
        self.assertEqual(AttributeType("bool").get_attribute_type(), "bool")

    def test_enum8(self):
        self.assertEqual(AttributeType("enum8").get_attribute_type(), "uint8_t")

    def test_enum16(self):
        self.assertEqual(AttributeType("enum16").get_attribute_type(), "uint16_t")

    def test_bitmap8(self):
        self.assertEqual(AttributeType("bitmap8").get_attribute_type(), "uint8_t")

    def test_bitmap16(self):
        self.assertEqual(AttributeType("bitmap16").get_attribute_type(), "uint16_t")

    def test_bitmap32(self):
        self.assertEqual(AttributeType("bitmap32").get_attribute_type(), "uint32_t")

    def test_string(self):
        self.assertEqual(AttributeType("string").get_attribute_type(), "char *")

    def test_octstr(self):
        self.assertEqual(AttributeType("octstr").get_attribute_type(), "uint8_t *")

    def test_list(self):
        self.assertEqual(AttributeType("list").get_attribute_type(), "uint8_t *")

    def test_unknown_type_raises(self):
        from utils.exceptions import XmlParseError

        at = AttributeType("custom_unknown")
        with self.assertRaises(XmlParseError):
            at.get_attribute_type()


class TestAttributeTypesDict(unittest.TestCase):
    """Test the global attribute_types dictionary — key type mappings."""

    def test_subject_id_is_uint64(self):
        self.assertEqual(attribute_types.get("subject-id"), "uint64")

    def test_fabric_idx_is_uint8(self):
        self.assertEqual(attribute_types.get("fabric-idx"), "uint8")

    def test_node_id(self):
        self.assertEqual(attribute_types.get("node-id"), "uint64")

    def test_epoch_us(self):
        self.assertEqual(attribute_types.get("epoch-us"), "uint64")

    def test_epoch_s(self):
        self.assertEqual(attribute_types.get("epoch-s"), "uint32")

    def test_vendor_id(self):
        self.assertEqual(attribute_types.get("vendor-id"), "uint16")

    def test_percent(self):
        self.assertEqual(attribute_types.get("percent"), "uint8")

    def test_percent100ths(self):
        self.assertEqual(attribute_types.get("percent100ths"), "uint16")

    def test_temperature(self):
        self.assertEqual(attribute_types.get("temperature"), "int16")

    def test_basic_primitives_present(self):
        for t in [
            "uint8",
            "uint16",
            "uint32",
            "uint64",
            "int8",
            "int16",
            "int32",
            "int64",
        ]:
            self.assertIn(t, attribute_types, f"Missing type: {t}")

    def test_string_types(self):
        self.assertIn("string", attribute_types)
        self.assertIn("octstr", attribute_types)


class TestAttributeTypeMap(unittest.TestCase):
    """Test per-cluster attribute type overrides."""

    def test_color_control_overrides_exist(self):
        self.assertIn("color_control", attribute_type_map)

    def test_thermostat_overrides_exist(self):
        self.assertIn("thermostat", attribute_type_map)

    def test_door_lock_overrides_exist(self):
        self.assertIn("door_lock", attribute_type_map)

    def test_override_values_are_valid(self):
        for cluster, overrides in attribute_type_map.items():
            for attr_name, type_val in overrides.items():
                self.assertIsInstance(
                    type_val,
                    (str, dict),
                    f"{cluster}.{attr_name} type should be str or dict",
                )

    def test_color_temp_physical_max_mireds_has_default(self):
        """color_temp_physical_max_mireds should have a sensible default value."""
        self.assertIn("color_control", attribute_type_map)
        self.assertIn(
            "color_temp_physical_max_mireds", attribute_type_map["color_control"]
        )
        override = attribute_type_map["color_control"]["color_temp_physical_max_mireds"]
        self.assertIn("default", override)
        # Default should be > 0 to avoid divide-by-zero in rate calculations
        self.assertGreater(override["default"], 0)
        # Default should be 65279 (max valid value per Matter spec)
        self.assertEqual(override["default"], 65279)

    def test_color_temp_physical_min_mireds_has_default(self):
        """color_temp_physical_min_mireds should have min=1 default value."""
        self.assertIn("color_control", attribute_type_map)
        self.assertIn(
            "color_temp_physical_min_mireds", attribute_type_map["color_control"]
        )
        override = attribute_type_map["color_control"]["color_temp_physical_min_mireds"]
        self.assertIn("default", override)
        # Default should be 1 (min valid value per Matter spec)
        self.assertEqual(override["default"], 1)


if __name__ == "__main__":
    unittest.main()
