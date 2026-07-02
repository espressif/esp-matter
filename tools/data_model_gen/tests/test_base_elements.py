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

"""Tests for utils/base_elements.py — abstract base classes for model elements."""

import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from utils.base_elements import (  # noqa: E402
    BaseElement,
    BaseAttribute,
    BaseCommand,
    BaseEvent,
    BaseFeature,
    BaseCluster,
    BaseDevice,
)
from code_generation.elements import get_id_name_lambda  # noqa: E402


# Concrete subclasses for testing abstract classes
class ConcreteElement(BaseElement):
    pass


class ConcreteFeature(BaseFeature):
    def get_attributes(self):
        return []

    def get_commands(self):
        return []

    def get_events(self):
        return []


class ConcreteCluster(BaseCluster):
    def get_attributes(self):
        return []

    def get_commands(self):
        return []

    def get_events(self):
        return []

    def get_features(self):
        return []


class ConcreteDevice(BaseDevice):
    def get_clusters(self):
        return []


class TestBaseElement(unittest.TestCase):
    """Test BaseElement name processing and ID handling."""

    def test_name_and_id(self):
        elem = ConcreteElement(name="Test Element", id="0x0001", element_type="Test")
        self.assertEqual(elem.id, "0x0001")
        self.assertEqual(elem.get_id(), "0x0001")

    def test_esp_name_generation(self):
        elem = ConcreteElement(name="Color Control", id="0x0001", element_type="Test")
        self.assertEqual(elem.esp_name, "color_control")

    def test_chip_name_generation(self):
        elem = ConcreteElement(name="Color Control", id="0x0001", element_type="Test")
        self.assertEqual(elem.chip_name, "ColorControl")

    def test_func_name_generation(self):
        elem = ConcreteElement(name="Color Control", id="0x0001", element_type="Test")
        self.assertEqual(elem.func_name, "color_control")

    def test_reserved_word_renaming_via_cluster_element(self):
        """Reserved word handling happens in BaseClusterElement, not BaseElement directly."""
        attr = BaseAttribute(
            name="auto",
            id="0x0001",
            type_="uint8",
            is_mandatory=True,
            default_value="0",
        )
        self.assertIn("Attribute", attr.name)

    def test_name_required(self):
        with self.assertRaises(AssertionError):
            ConcreteElement(name="", id="0x0001", element_type="Test")


class TestBaseClusterElement(unittest.TestCase):
    """Test BaseClusterElement with reserved word handling."""

    def test_reserved_word_attribute(self):
        attr = BaseAttribute(
            name="auto",
            id="0x0001",
            type_="uint8",
            is_mandatory=True,
            default_value="0",
        )
        self.assertIn("Attribute", attr.name)

    def test_reserved_word_command(self):
        cmd = BaseCommand(
            name="auto",
            id="0x0001",
            is_mandatory=True,
            direction="commandToServer",
            response="Y",
        )
        self.assertIn("Command", cmd.name)

    def test_reserved_word_event(self):
        evt = BaseEvent(name="switch", id="0x0001", is_mandatory=True)
        self.assertIn("Event", evt.name)

    def test_non_reserved_word(self):
        attr = BaseAttribute(
            name="temperature",
            id="0x0001",
            type_="int16",
            is_mandatory=True,
            default_value="0",
        )
        self.assertEqual(attr.func_name, "temperature")

    def test_mandatory_flag(self):
        attr = BaseAttribute(
            name="test",
            id="0x0001",
            type_="uint8",
            is_mandatory=True,
            default_value="0",
        )
        self.assertTrue(attr.is_mandatory)

    def test_optional_flag(self):
        attr = BaseAttribute(
            name="test",
            id="0x0001",
            type_="uint8",
            is_mandatory=False,
            default_value="0",
        )
        self.assertFalse(attr.is_mandatory)


class TestBaseAttribute(unittest.TestCase):
    """Test BaseAttribute initialization."""

    def test_type_stored(self):
        attr = BaseAttribute(
            name="temp",
            id="0x0001",
            type_="int16",
            is_mandatory=True,
            default_value="20",
        )
        self.assertEqual(attr.type, "int16")

    def test_default_value_stored(self):
        attr = BaseAttribute(
            name="temp",
            id="0x0001",
            type_="int16",
            is_mandatory=True,
            default_value="20",
        )
        self.assertEqual(attr.default_value, "20")

    def test_nullable_default_false(self):
        attr = BaseAttribute(
            name="temp",
            id="0x0001",
            type_="int16",
            is_mandatory=True,
            default_value="0",
        )
        self.assertFalse(attr.is_nullable)


class TestBaseCommand(unittest.TestCase):
    """Test BaseCommand initialization."""

    def test_direction_stored(self):
        cmd = BaseCommand(
            name="SetTemp",
            id="0x0001",
            is_mandatory=True,
            direction="commandToServer",
            response="Y",
        )
        self.assertEqual(cmd.direction, "commandToServer")

    def test_response_stored(self):
        cmd = BaseCommand(
            name="SetTemp",
            id="0x0001",
            is_mandatory=True,
            direction="commandToServer",
            response="Y",
        )
        self.assertEqual(cmd.response, "Y")


class TestBaseFeature(unittest.TestCase):
    """Test BaseFeature with abstract methods."""

    def test_creation(self):
        feat = ConcreteFeature(name="Lighting", id="0x0001", is_mandatory=False)
        self.assertFalse(feat.is_mandatory)
        self.assertEqual(feat.get_attributes(), [])
        self.assertEqual(feat.get_commands(), [])
        self.assertEqual(feat.get_events(), [])


class TestBaseCluster(unittest.TestCase):
    """Test BaseCluster initialization and callback flags."""

    def test_revision(self):
        cluster = ConcreteCluster(
            name="OnOff", id="0x0006", revision=6, is_mandatory=True
        )
        self.assertEqual(cluster.get_revision(), 6)

    def test_default_flags(self):
        cluster = ConcreteCluster(
            name="OnOff", id="0x0006", revision=6, is_mandatory=True
        )
        self.assertFalse(cluster.server_cluster)
        self.assertFalse(cluster.client_cluster)
        self.assertFalse(cluster.init_function_available)
        self.assertFalse(cluster.delegate_init_callback_available)
        self.assertIsNone(cluster.role)

    def test_abstract_methods(self):
        cluster = ConcreteCluster(
            name="OnOff", id="0x0006", revision=6, is_mandatory=True
        )
        self.assertEqual(cluster.get_attributes(), [])
        self.assertEqual(cluster.get_features(), [])


class TestBaseDevice(unittest.TestCase):
    """Test BaseDevice filename and revision."""

    def test_filename_generation(self):
        device = ConcreteDevice(name="On/Off Light", id="0x0100", revision=3)
        self.assertEqual(device.filename, "on_off_light_device")

    def test_device_type_id(self):
        device = ConcreteDevice(name="Temperature Sensor", id="0x0302", revision=2)
        self.assertEqual(device.get_device_type_id(), "0x0302")

    def test_device_type_version(self):
        device = ConcreteDevice(name="Temperature Sensor", id="0x0302", revision=2)
        self.assertEqual(device.get_device_type_version(), 2)

    def test_device_name_override(self):
        device = ConcreteDevice(name="Dishwasher", id="0x0075", revision=1)
        self.assertIn("dish_washer", device.esp_name)


class TestGetIdNameLambda(unittest.TestCase):
    """Test sorting lambda."""

    def test_sorts_by_id(self):
        sorter = get_id_name_lambda()

        class FakeElem:
            def __init__(self, id_, name):
                self._id = id_
                self.name = name

            def get_id(self):
                return self._id

        items = [
            FakeElem("0x0003", "C"),
            FakeElem("0x0001", "A"),
            FakeElem("0x0002", "B"),
        ]
        sorted_items = sorted(items, key=sorter)
        self.assertEqual([i.name for i in sorted_items], ["A", "B", "C"])


if __name__ == "__main__":
    unittest.main()
