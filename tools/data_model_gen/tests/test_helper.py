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

"""Tests for utils/helper.py — name conversions, ID validation, type conversions, file I/O."""

import unittest
import sys
import os
import tempfile
import json

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from utils.helper import (  # noqa: E402
    chip_name,
    esp_name,
    convert_to_snake_case,
    check_valid_id,
    safe_get_attr,
    write_to_file,
)


class TestChipName(unittest.TestCase):
    """Test chip_name() — converts names to CamelCase chip convention."""

    def test_already_camel_case(self):
        self.assertEqual(chip_name("OnOff"), "OnOff")

    def test_slash_separated(self):
        self.assertEqual(chip_name("On/Off"), "OnOff")

    def test_space_separated(self):
        self.assertEqual(chip_name("Color Control"), "ColorControl")

    def test_hyphen_separated(self):
        self.assertEqual(chip_name("Wi-Fi"), "WiFi")

    def test_preserves_mixed_case_acronyms(self):
        # Mixed-case acronyms are preserved as-is (they match CHIP), while all-caps
        # acronyms are title-cased (AV -> Av) the same way CHIP names them.
        self.assertEqual(
            chip_name("WiFi Network Diagnostics"), "WiFiNetworkDiagnostics"
        )
        self.assertEqual(
            chip_name("WebRTC Transport Provider"), "WebRTCTransportProvider"
        )
        self.assertEqual(
            chip_name("Camera AV Stream Management"), "CameraAvStreamManagement"
        )

    def test_dishwasher_special_case(self):
        self.assertEqual(chip_name("Dish Washer Mode"), "DishwasherMode")

    def test_single_uppercase_word(self):
        self.assertEqual(chip_name("Thermostat"), "Thermostat")

    def test_mixed_special_chars(self):
        result = chip_name("PM2.5 Concentration")
        self.assertTrue(result[0].isupper())

    def test_all_lowercase(self):
        result = chip_name("on off")
        self.assertEqual(result, "OnOff")


class TestEspName(unittest.TestCase):
    """Test esp_name() — converts names to lowercase underscore convention."""

    def test_simple_name(self):
        self.assertEqual(esp_name("OnOff"), "onoff")

    def test_spaces_to_underscores(self):
        self.assertEqual(esp_name("Color Control"), "color_control")

    def test_slash_to_underscore(self):
        self.assertEqual(esp_name("On/Off"), "on_off")

    def test_hyphen_to_underscore(self):
        self.assertEqual(esp_name("Wi-Fi"), "wi_fi")

    def test_already_lowercase(self):
        self.assertEqual(esp_name("thermostat"), "thermostat")


class TestConvertToSnakeCase(unittest.TestCase):
    """Test convert_to_snake_case() — CamelCase/mixed to snake_case."""

    def test_camel_case(self):
        result = convert_to_snake_case("ColorControl")
        self.assertEqual(result, "color_control")

    def test_with_command_suffix(self):
        result = convert_to_snake_case("SetpointRaiseLowerCommand")
        self.assertNotIn("command", result.lower())

    def test_empty_string(self):
        self.assertEqual(convert_to_snake_case(""), "")

    def test_none(self):
        self.assertIsNone(convert_to_snake_case(None))

    def test_numbers_separated(self):
        result = convert_to_snake_case("PM25Concentration")
        self.assertIn("_", result)

    def test_all_uppercase_acronym(self):
        result = convert_to_snake_case("ICDManagement")
        self.assertIn("icd", result)
        self.assertIn("management", result)

    def test_already_snake_case(self):
        result = convert_to_snake_case("on_off")
        self.assertEqual(result, "on_off")


class TestCheckValidId(unittest.TestCase):
    """Test check_valid_id() — hex ID validation."""

    def test_valid_hex_id(self):
        self.assertTrue(check_valid_id("0x0001"))

    def test_valid_long_hex_id(self):
        self.assertTrue(check_valid_id("0x00000006"))

    def test_none_id(self):
        self.assertFalse(check_valid_id(None))

    def test_empty_id(self):
        self.assertFalse(check_valid_id(""))

    def test_no_hex_prefix(self):
        self.assertFalse(check_valid_id("0001"))

    def test_tbd_id(self):
        self.assertFalse(check_valid_id("ID-TBD"))

    def test_invalid_hex_chars(self):
        self.assertFalse(check_valid_id("0xGGGG"))


class TestSafeGetAttr(unittest.TestCase):
    """Test safe_get_attr() — safely get attributes with defaults."""

    def test_existing_attribute(self):
        class Obj:
            x = 42

        self.assertEqual(safe_get_attr(Obj(), "x"), 42)

    def test_missing_attribute(self):
        class Obj:
            pass

        self.assertIsNone(safe_get_attr(Obj(), "missing"))

    def test_missing_with_default(self):
        class Obj:
            pass

        self.assertEqual(safe_get_attr(Obj(), "missing", "default"), "default")

    def test_none_object(self):
        self.assertIsNone(safe_get_attr(None, "x"))

    def test_none_object_with_default(self):
        self.assertEqual(safe_get_attr(None, "x", 99), 99)


class TestWriteToFile(unittest.TestCase):
    """Test write_to_file() — file I/O with error handling."""

    def test_write_text(self):
        with tempfile.NamedTemporaryFile(mode="w", suffix=".txt", delete=False) as f:
            path = f.name
        try:
            self.assertTrue(write_to_file(path, "hello world"))
            with open(path) as f:
                self.assertEqual(f.read(), "hello world")
        finally:
            os.unlink(path)

    def test_write_json(self):
        with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False) as f:
            path = f.name
        try:
            data = {"key": "value", "num": 42}
            self.assertTrue(write_to_file(path, data, file_type="json"))
            with open(path) as f:
                loaded = json.load(f)
            self.assertEqual(loaded, data)
        finally:
            os.unlink(path)

    def test_write_to_invalid_path(self):
        with self.assertRaises(Exception):
            write_to_file("/nonexistent/dir/file.txt", "data")


if __name__ == "__main__":
    unittest.main()
