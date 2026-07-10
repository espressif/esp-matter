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

"""Tests for xml_processing/data_type_parser.py — type resolution, bounds, data types."""

import unittest
import sys
import os
from xml.etree.ElementTree import Element, SubElement

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from xml_processing.data_type_parser import (  # noqa: E402
    resolve_attribute_type,
    resolve_attribute_bounds,
    _fallback_unknown_type,
    _override_type_by_default_value,
    _default_bounds_by_type,
    _normalize_bounds,
    _bounds_from_constraint,
    _infer_type_by_count,
    INT_BOUNDS,
    Item,
    Enum,
    Bitmap,
    Struct,
    DataTypeParser,
)
from utils.exceptions import XmlParseError  # noqa: E402


class TestIntBounds(unittest.TestCase):
    """INT_BOUNDS holds the full usable range per integer type."""

    def test_uint8_bounds(self):
        self.assertEqual(INT_BOUNDS["uint8"], (0, 255))

    def test_uint16_bounds(self):
        self.assertEqual(INT_BOUNDS["uint16"], (0, 65535))

    def test_uint32_bounds(self):
        self.assertEqual(INT_BOUNDS["uint32"], (0, 2**32 - 1))

    def test_uint64_bounds(self):
        self.assertEqual(INT_BOUNDS["uint64"], (0, 2**32 - 1))

    def test_int8_bounds(self):
        self.assertEqual(INT_BOUNDS["int8"], (-128, 127))

    def test_int16_bounds(self):
        self.assertEqual(INT_BOUNDS["int16"], (-32768, 32767))

    def test_int32_bounds(self):
        self.assertEqual(INT_BOUNDS["int32"], (-(2**31), 2**31 - 1))

    def test_int64_bounds(self):
        self.assertEqual(INT_BOUNDS["int64"], (-(2**31), 2**31 - 1))


class TestResolveAttributeType(unittest.TestCase):
    """Test resolve_attribute_type() — XML type to C type resolution."""

    def test_basic_type(self):
        elem = Element("attribute", type="uint8")
        result = resolve_attribute_type(elem, {"uint8": "uint8"})
        self.assertEqual(result, "uint8")

    def test_type_with_space(self):
        elem = Element("attribute", type="uint8 some extra")
        result = resolve_attribute_type(elem, {"uint8": "uint8"})
        self.assertEqual(result, "uint8")

    def test_none_type_raises(self):
        elem = Element("attribute")
        with self.assertRaises(XmlParseError):
            resolve_attribute_type(elem, {})

    def test_unknown_type_fallback(self):
        elem = Element("attribute", type="CustomBitmapType")
        result = resolve_attribute_type(elem, {})
        self.assertEqual(result, "bitmap8")

    def test_enum_widening_by_default_value(self):
        elem = Element("attribute", type="enum8", default="0x100")
        result = resolve_attribute_type(elem, {"enum8": "enum8"})
        self.assertEqual(result, "enum16")


class TestFallbackUnknownType(unittest.TestCase):
    """Test _fallback_unknown_type() — fallback for unrecognized types."""

    def test_bitmap_fallback(self):
        self.assertEqual(_fallback_unknown_type("someBitmapType"), "bitmap8")

    def test_enum_fallback(self):
        self.assertEqual(_fallback_unknown_type("myEnumType"), "enum8")

    def test_struct_fallback(self):
        self.assertEqual(_fallback_unknown_type("MyStructType"), "list")

    def test_unknown_passthrough(self):
        self.assertEqual(_fallback_unknown_type("custom"), "custom")


class TestOverrideTypeByDefaultValue(unittest.TestCase):
    """Test _override_type_by_default_value() — enum/bitmap widening."""

    def test_no_default(self):
        elem = Element("attribute", type="enum8")
        self.assertEqual(_override_type_by_default_value(elem, "enum8"), "enum8")

    def test_enum8_to_enum16(self):
        elem = Element("attribute", type="enum8", default="0x100")
        self.assertEqual(_override_type_by_default_value(elem, "enum8"), "enum16")

    def test_bitmap8_to_bitmap16(self):
        elem = Element("attribute", type="bitmap8", default="0x100")
        self.assertEqual(_override_type_by_default_value(elem, "bitmap8"), "bitmap16")

    def test_no_widening_needed(self):
        elem = Element("attribute", type="enum8", default="0x0A")
        self.assertEqual(_override_type_by_default_value(elem, "enum8"), "enum8")

    def test_decimal_default(self):
        elem = Element("attribute", type="enum8", default="256")
        self.assertEqual(_override_type_by_default_value(elem, "enum8"), "enum16")

    def test_non_numeric_default(self):
        elem = Element("attribute", type="enum8", default="auto")
        self.assertEqual(_override_type_by_default_value(elem, "enum8"), "enum8")


class TestDefaultBoundsByType(unittest.TestCase):
    """Test _default_bounds_by_type() — applies default bounds from INT_BOUNDS."""

    def _make_attr(self, type_, min_val=None, max_val=None):
        class FakeAttr:
            pass

        a = FakeAttr()
        a.type = type_
        a.min_value = min_val
        a.max_value = max_val
        return a

    def test_no_bounds_when_fully_unconstrained(self):
        # No explicit min/max in XML -> no synthesized bounds (avoid redundant
        # full-type-range add_bounds; the C type already limits the value).
        attr = self._make_attr("uint8")
        _default_bounds_by_type(attr)
        self.assertIsNone(attr.min_value)
        self.assertIsNone(attr.max_value)

    def test_fills_missing_max_from_type_when_min_given(self):
        # A real min constraint present -> fill max from the full type range.
        attr = self._make_attr("uint8", min_val=5)
        _default_bounds_by_type(attr)
        self.assertEqual(attr.min_value, 5)
        self.assertEqual(attr.max_value, 255)

    def test_fills_missing_min_from_type_when_max_given(self):
        # A real max constraint present -> fill min from the type range.
        attr = self._make_attr("int16", max_val=100)
        _default_bounds_by_type(attr)
        self.assertEqual(attr.min_value, -32768)
        self.assertEqual(attr.max_value, 100)

    def test_preserves_existing_bounds(self):
        attr = self._make_attr("uint16", min_val=10, max_val=100)
        _default_bounds_by_type(attr)
        self.assertEqual(attr.min_value, 10)
        self.assertEqual(attr.max_value, 100)

    def test_unknown_type_no_bounds(self):
        attr = self._make_attr("string")
        _default_bounds_by_type(attr)
        self.assertIsNone(attr.min_value)
        self.assertIsNone(attr.max_value)


class TestNormalizeBounds(unittest.TestCase):
    """Test _normalize_bounds() — hex/string to int conversion."""

    def _make_attr(self, min_val, max_val):
        class FakeAttr:
            pass

        a = FakeAttr()
        a.min_value = min_val
        a.max_value = max_val
        return a

    def test_hex_string(self):
        attr = self._make_attr("0x0A", "0xFF")
        _normalize_bounds(attr)
        self.assertEqual(attr.min_value, 10)
        self.assertEqual(attr.max_value, 255)

    def test_non_digit_string(self):
        attr = self._make_attr("abc", None)
        _normalize_bounds(attr)
        self.assertIsNone(attr.min_value)

    def test_negative_digit_string(self):
        attr = self._make_attr("-100", "100")
        _normalize_bounds(attr)
        # Negative digit strings should be kept (they pass lstrip("-").isdigit())
        self.assertEqual(attr.min_value, "-100")

    def test_none_preserved(self):
        attr = self._make_attr(None, None)
        _normalize_bounds(attr)
        self.assertIsNone(attr.min_value)

    def test_int_preserved(self):
        attr = self._make_attr(10, 100)
        _normalize_bounds(attr)
        self.assertEqual(attr.min_value, 10)
        self.assertEqual(attr.max_value, 100)


class TestBoundsFromConstraint(unittest.TestCase):
    """Test _bounds_from_constraint() — XML constraint parsing."""

    def _make_attr(self):
        class FakeAttr:
            pass

        a = FakeAttr()
        a.min_value = None
        a.max_value = None
        a.type = "uint16"
        return a

    def test_min_max_constraint(self):
        attr = self._make_attr()
        elem = Element("attribute")
        constraint = SubElement(elem, "constraint")
        SubElement(constraint, "min", value="10")
        SubElement(constraint, "max", value="100")
        _bounds_from_constraint(attr, elem)
        self.assertEqual(attr.min_value, "10")
        self.assertEqual(attr.max_value, "100")

    def test_between_constraint(self):
        attr = self._make_attr()
        elem = Element("attribute")
        constraint = SubElement(elem, "constraint")
        between = SubElement(constraint, "between")
        SubElement(between, "from", value="5")
        SubElement(between, "to", value="50")
        _bounds_from_constraint(attr, elem)
        self.assertEqual(attr.min_value, "5")
        self.assertEqual(attr.max_value, "50")

    def test_max_length_constraint(self):
        attr = self._make_attr()
        elem = Element("attribute")
        constraint = SubElement(elem, "constraint")
        SubElement(constraint, "maxLength", value="254")
        _bounds_from_constraint(attr, elem)
        self.assertEqual(attr.min_value, 0)
        self.assertEqual(attr.max_value, "254")

    def test_no_constraint(self):
        attr = self._make_attr()
        elem = Element("attribute")
        _bounds_from_constraint(attr, elem)
        self.assertIsNone(attr.min_value)
        self.assertIsNone(attr.max_value)


class TestResolveAttributeBounds(unittest.TestCase):
    """Test resolve_attribute_bounds() — full bounds resolution pipeline."""

    def _make_attr(self, type_):
        class FakeAttr:
            pass

        a = FakeAttr()
        a.type = type_
        a.min_value = None
        a.max_value = None
        return a

    def test_enum_bounds_from_items(self):
        attr = self._make_attr("enum8")
        elem = Element("attribute", type="testenum")
        items = [
            Item("A", "0", "", True),
            Item("B", "1", "", True),
            Item("C", "2", "", True),
        ]
        data_types = {"enums": {"testenum": Enum("TestEnum", "enum8", items)}}
        resolve_attribute_bounds(attr, elem, data_types)
        self.assertEqual(attr.min_value, 0)
        self.assertEqual(attr.max_value, 2)

    def test_bitmap_bounds_from_bitfields(self):
        attr = self._make_attr("bitmap8")
        elem = Element("attribute", type="testbitmap")
        fields = [
            Item("A", "0", "", True),
            Item("B", "1", "", True),
            Item("C", "2", "", True),
        ]
        data_types = {
            "bitmaps": {"testbitmap": Bitmap("TestBitmap", "bitmap8", fields)}
        }
        resolve_attribute_bounds(attr, elem, data_types)
        self.assertEqual(attr.min_value, 0)
        self.assertEqual(attr.max_value, 7)  # 2^3 - 1

    def test_uint8_unconstrained_no_bounds(self):
        # No <constraint> in XML -> no bounds synthesized (redundant type-range removed)
        attr = self._make_attr("uint8")
        elem = Element("attribute", type="uint8")
        resolve_attribute_bounds(attr, elem, {})
        self.assertIsNone(attr.min_value)
        self.assertIsNone(attr.max_value)

    def test_uint8_min_constraint_fills_max(self):
        # Explicit min constraint -> keep min, fill max from type range
        attr = self._make_attr("uint8")
        elem = Element("attribute", type="uint8")
        constraint = SubElement(elem, "constraint")
        SubElement(constraint, "min", value="1")
        resolve_attribute_bounds(attr, elem, {})
        self.assertEqual(attr.min_value, 1)
        self.assertEqual(attr.max_value, 255)


class TestInferTypeByCount(unittest.TestCase):
    """Test _infer_type_by_count() — enum/bitmap size inference."""

    def test_small_enum(self):
        self.assertEqual(_infer_type_by_count(5, "enum", (256, 65536)), "enum8")

    def test_medium_enum(self):
        self.assertEqual(_infer_type_by_count(300, "enum", (256, 65536)), "enum16")

    def test_large_enum(self):
        self.assertEqual(_infer_type_by_count(70000, "enum", (256, 65536)), "enum32")

    def test_small_bitmap(self):
        self.assertEqual(_infer_type_by_count(5, "bitmap"), "bitmap8")

    def test_medium_bitmap(self):
        self.assertEqual(_infer_type_by_count(10, "bitmap"), "bitmap16")


class TestDataClasses(unittest.TestCase):
    """Test Item, Enum, Bitmap, Struct data classes."""

    def test_item_to_dict(self):
        item = Item("TestItem", "0x01", "A test item", True)
        d = item.to_dict()
        self.assertEqual(d["name"], "TestItem")
        self.assertEqual(d["value"], "0x01")
        self.assertTrue(d["is_mandatory"])

    def test_enum_to_dict(self):
        items = [Item("A", "0", "", True)]
        e = Enum("TestEnum", "enum8", items)
        d = e.to_dict()
        self.assertEqual(d["name"], "TestEnum")
        self.assertEqual(d["base_type"], "enum8")
        self.assertEqual(len(d["items"]), 1)

    def test_bitmap_to_dict(self):
        fields = [Item("Bit0", "0", "", True)]
        b = Bitmap("TestBitmap", "bitmap8", fields)
        d = b.to_dict()
        self.assertEqual(d["name"], "TestBitmap")
        self.assertEqual(len(d["bitfields"]), 1)

    def test_struct_to_dict(self):
        fields = [Item("Field1", "uint8", "", True)]
        s = Struct("TestStruct", "list", fields)
        d = s.to_dict()
        self.assertEqual(d["name"], "TestStruct")
        self.assertEqual(d["base_type"], "list")


class TestDataTypeParser(unittest.TestCase):
    """Test DataTypeParser — XML dataTypes section parsing."""

    def test_parse_enums(self):
        root = Element("cluster")
        data_types = SubElement(root, "dataTypes")
        enum_el = SubElement(data_types, "enum", name="StatusEnum")
        SubElement(enum_el, "item", name="Success", value="0", summary="OK")
        SubElement(enum_el, "item", name="Failure", value="1", summary="Fail")

        parser = DataTypeParser()
        types = parser.parse(root)
        self.assertIn("statusenum", types)
        self.assertEqual(types["statusenum"], "enum8")
        self.assertIn("statusenum", parser.enums)
        self.assertEqual(len(parser.enums["statusenum"].items), 2)

    def test_parse_bitmaps(self):
        root = Element("cluster")
        data_types = SubElement(root, "dataTypes")
        bitmap_el = SubElement(data_types, "bitmap", name="FeatureBitmap")
        SubElement(bitmap_el, "bitfield", name="Bit0", bit="0", summary="First")
        SubElement(bitmap_el, "bitfield", name="Bit1", bit="1", summary="Second")

        parser = DataTypeParser()
        types = parser.parse(root)
        self.assertIn("featurebitmap", types)
        self.assertIn("featurebitmap", parser.bitmaps)

    def test_parse_structs(self):
        root = Element("cluster")
        data_types = SubElement(root, "dataTypes")
        struct_el = SubElement(data_types, "struct", name="TestStruct")
        SubElement(struct_el, "field", name="field1", type="uint8", summary="F1")

        parser = DataTypeParser()
        types = parser.parse(root)
        self.assertIn("teststruct", types)
        self.assertEqual(types["teststruct"], "list")

    def test_parse_no_data_types(self):
        root = Element("cluster")
        parser = DataTypeParser()
        types = parser.parse(root)
        self.assertIsInstance(types, dict)

    def test_get_data_types(self):
        parser = DataTypeParser()
        dt = parser.get_data_types()
        self.assertIn("enums", dt)
        self.assertIn("bitmaps", dt)
        self.assertIn("structs", dt)


if __name__ == "__main__":
    unittest.main()
