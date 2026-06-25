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

"""
Tests for O.a+ (Optional Choice) cluster handling across the data model generation pipeline.

This covers:
  - device_parser.py: DeviceParser.create_cluster() with optionalConform choice
  - xml_processing/elements.py: Device.get_optional_choice_clusters(), has_optional_choice_clusters()
  - code_generation/elements.py: Device.get_optional_choice_clusters(), get_unique_optional_choice_clusters()
  - serializers.py: DeviceSerializer serializing optional_choice
  - deserializer.py: DeviceDeserializer deserializing optional_choice
"""

import unittest
import sys
import os
from xml.etree.ElementTree import Element, SubElement

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from xml_processing.device_parser import (  # noqa: E402
    DeviceParser,
    is_mandatory,
)
from xml_processing.elements import (  # noqa: E402
    Device as XmlDevice,
    Cluster as XmlCluster,
)
from xml_processing.serializers import DeviceSerializer  # noqa: E402
from code_generation.elements import (  # noqa: E402
    Device as CodegenDevice,
    Cluster as CodegenCluster,
)


# ---------------------------------------------------------------------------
# Helpers to build minimal XML elements
# ---------------------------------------------------------------------------


def _cluster_xml_optional_choice(name, id_, choice="a", more="true"):
    """Create a cluster XML element with optionalConform choice attribute."""
    elem = Element("cluster", name=name, id=id_, side="server")
    optional = SubElement(elem, "optionalConform", choice=choice)
    if more:
        optional.set("more", more)
    return elem


def _cluster_xml_mandatory(name, id_):
    """Create a cluster XML element with mandatoryConform."""
    elem = Element("cluster", name=name, id=id_, side="server")
    SubElement(elem, "mandatoryConform")
    return elem


def _cluster_xml_optional_no_choice(name, id_):
    """Create a cluster XML element with optionalConform but no choice."""
    elem = Element("cluster", name=name, id=id_, side="server")
    SubElement(elem, "optionalConform")
    return elem


def _make_xml_cluster(name, id_, is_mandatory=False, optional_choice=None):
    """Create an XmlCluster object for testing."""
    cluster = XmlCluster(name=name, id=id_, revision=1, is_mandatory=is_mandatory)
    cluster.server_cluster = True
    cluster.client_cluster = False
    if optional_choice:
        cluster.optional_choice = optional_choice
    return cluster


def _make_codegen_cluster(name, id_, is_mandatory=False, optional_choice=None):
    """Create a CodegenCluster object for testing."""
    cluster = CodegenCluster(name=name, id=id_, revision=1, is_mandatory=is_mandatory)
    cluster.server_cluster = True
    cluster.client_cluster = False
    if optional_choice:
        cluster.optional_choice = optional_choice
    return cluster


# ---------------------------------------------------------------------------
# Tests for DeviceParser.create_cluster() with optionalConform choice
# ---------------------------------------------------------------------------


class TestDeviceParserOptionalChoiceParsing(unittest.TestCase):
    """Test DeviceParser.create_cluster() parses optionalConform choice correctly."""

    def setUp(self):
        self.parser = DeviceParser()

    def test_optional_with_choice_and_more(self):
        """O.a+ cluster should have optional_choice dict with marker and more=True."""
        elem = _cluster_xml_optional_choice("EPM", "0x0090", choice="a", more="true")
        cluster = self.parser.create_cluster(elem)
        self.assertIsNotNone(cluster.optional_choice)
        self.assertEqual(cluster.optional_choice["choice"], "a")
        self.assertTrue(cluster.optional_choice["more"])
        self.assertEqual(cluster.optional_choice["min"], 1)

    def test_optional_with_choice_no_more(self):
        """Optional with choice but more=false should not include more key."""
        elem = _cluster_xml_optional_choice("EPM", "0x0090", choice="b", more="false")
        cluster = self.parser.create_cluster(elem)
        self.assertIsNotNone(cluster.optional_choice)
        self.assertEqual(cluster.optional_choice["choice"], "b")
        # When more=False, the to_dict() method does not include 'more'
        self.assertNotIn("more", cluster.optional_choice)

    def test_optional_without_choice(self):
        """Optional conformance without choice attribute results in optional_choice=None."""
        elem = _cluster_xml_optional_no_choice("SomeCluster", "0x0091")
        cluster = self.parser.create_cluster(elem)
        self.assertIsNone(cluster.optional_choice)

    def test_mandatory_conformance(self):
        """Mandatory conformance results in optional_choice=None."""
        elem = _cluster_xml_mandatory("PowerTopology", "0x009C")
        cluster = self.parser.create_cluster(elem)
        self.assertIsNone(cluster.optional_choice)

    def test_no_conformance_element(self):
        """Cluster with no conformance element results in optional_choice=None."""
        elem = Element("cluster", name="Test", id="0x0001", side="server")
        cluster = self.parser.create_cluster(elem)
        self.assertIsNone(cluster.optional_choice)


# ---------------------------------------------------------------------------
# Tests for is_mandatory()
# ---------------------------------------------------------------------------


class TestIsMandatory(unittest.TestCase):
    """Test device_parser.is_mandatory()."""

    def test_mandatory_conform_present(self):
        elem = _cluster_xml_mandatory("Test", "0x0001")
        self.assertTrue(is_mandatory(elem))

    def test_mandatory_conform_absent(self):
        elem = _cluster_xml_optional_no_choice("Test", "0x0001")
        self.assertFalse(is_mandatory(elem))

    def test_mandatory_conform_with_child(self):
        """mandatoryConform with child elements (like condition) should return False."""
        elem = Element("cluster", name="Test", id="0x0001")
        mandatory = SubElement(elem, "mandatoryConform")
        SubElement(mandatory, "condition", name="SomeCondition")
        self.assertFalse(is_mandatory(elem))


# ---------------------------------------------------------------------------
# Tests for XmlDevice optional choice clusters
# ---------------------------------------------------------------------------


class TestXmlDeviceOptionalChoiceClusters(unittest.TestCase):
    """Test xml_processing.elements.Device O.a+ cluster methods."""

    def test_has_optional_choice_clusters_true(self):
        """Device with O.a+ clusters returns True."""
        device = XmlDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        c1 = _make_xml_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c2 = _make_xml_cluster("PowerTopology", "0x009C", is_mandatory=True)
        device.clusters = {c1, c2}
        self.assertTrue(device.has_optional_choice_clusters())

    def test_has_optional_choice_clusters_false(self):
        """Device without O.a+ clusters returns False."""
        device = XmlDevice(id="0x0100", name="OnOff_Light", revision=1)
        c1 = _make_xml_cluster("OnOff", "0x0006", is_mandatory=True)
        device.clusters = {c1}
        self.assertFalse(device.has_optional_choice_clusters())

    def test_get_optional_choice_clusters_grouped(self):
        """O.a+ clusters should be grouped by choice marker."""
        device = XmlDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        c1 = _make_xml_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c2 = _make_xml_cluster(
            "EEM", "0x0091", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c3 = _make_xml_cluster("PowerTopology", "0x009C", is_mandatory=True)
        device.clusters = {c1, c2, c3}

        groups = device.get_optional_choice_clusters()
        self.assertIn("a", groups)
        self.assertEqual(len(groups["a"]), 2)

    def test_get_optional_choice_clusters_sorted(self):
        """Clusters within each choice group should be sorted by ID."""
        device = XmlDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        c1 = _make_xml_cluster(
            "EEM", "0x0091", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c2 = _make_xml_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        device.clusters = {c1, c2}

        groups = device.get_optional_choice_clusters()
        self.assertEqual(groups["a"][0].get_id(), "0x0090")
        self.assertEqual(groups["a"][1].get_id(), "0x0091")


# ---------------------------------------------------------------------------
# Tests for CodegenDevice optional choice clusters
# ---------------------------------------------------------------------------


class TestCodegenDeviceOptionalChoiceClusters(unittest.TestCase):
    """Test code_generation.elements.Device O.a+ cluster methods."""

    def test_has_optional_choice_clusters_true(self):
        """Device with O.a+ clusters returns True."""
        device = CodegenDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        c1 = _make_codegen_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c2 = _make_codegen_cluster("PowerTopology", "0x009C", is_mandatory=True)
        device.clusters = [c1, c2]
        self.assertTrue(device.has_optional_choice_clusters())

    def test_has_optional_choice_clusters_false(self):
        """Device without O.a+ clusters returns False."""
        device = CodegenDevice(id="0x0100", name="OnOff_Light", revision=1)
        c1 = _make_codegen_cluster("OnOff", "0x0006", is_mandatory=True)
        device.clusters = [c1]
        self.assertFalse(device.has_optional_choice_clusters())

    def test_get_optional_choice_clusters_grouped(self):
        """O.a+ clusters should be grouped by choice marker."""
        device = CodegenDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        c1 = _make_codegen_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c2 = _make_codegen_cluster(
            "EEM", "0x0091", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c3 = _make_codegen_cluster("PowerTopology", "0x009C", is_mandatory=True)
        device.clusters = [c1, c2, c3]

        groups = device.get_optional_choice_clusters()
        self.assertIn("a", groups)
        self.assertEqual(len(groups["a"]), 2)

    def test_get_unique_optional_choice_clusters(self):
        """get_unique_optional_choice_clusters returns flattened unique list."""
        device = CodegenDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        c1 = _make_codegen_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c2 = _make_codegen_cluster(
            "EEM", "0x0091", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        device.clusters = [c1, c2]

        unique = device.get_unique_optional_choice_clusters()
        self.assertEqual(len(unique), 2)
        # Should be sorted by ID
        self.assertEqual(unique[0].get_id(), "0x0090")
        self.assertEqual(unique[1].get_id(), "0x0091")

    def test_get_unique_optional_choice_clusters_deduplicates(self):
        """Duplicate clusters should be deduplicated."""
        device = CodegenDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        c1 = _make_codegen_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        c2 = _make_codegen_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        device.clusters = [c1, c2]

        unique = device.get_unique_optional_choice_clusters()
        self.assertEqual(len(unique), 1)


# ---------------------------------------------------------------------------
# Tests for DeviceSerializer with optional_choice
# ---------------------------------------------------------------------------


class TestDeviceSerializerOptionalChoice(unittest.TestCase):
    """Test xml_processing.serializers.DeviceSerializer with O.a+ clusters."""

    def test_serialize_cluster_with_optional_choice(self):
        """Cluster with optional_choice should include it in serialized output."""
        device = XmlDevice(id="0x0510", name="Electrical_Sensor", revision=1)
        cluster = _make_xml_cluster(
            "EPM", "0x0090", optional_choice={"choice": "a", "more": True, "min": 1}
        )
        cluster.attribute_name_list = []
        cluster.feature_name_list = []
        cluster.command_name_list = []
        cluster.event_name_list = []
        device.clusters = {cluster}

        result = DeviceSerializer.to_dict(device)
        clusters = result["clusters"]
        self.assertEqual(len(clusters), 1)
        self.assertIn("optional_choice", clusters[0])
        self.assertEqual(clusters[0]["optional_choice"]["choice"], "a")
        self.assertTrue(clusters[0]["optional_choice"]["more"])

    def test_serialize_cluster_without_optional_choice(self):
        """Cluster without optional_choice should not include it in serialized output."""
        device = XmlDevice(id="0x0100", name="OnOff_Light", revision=1)
        cluster = _make_xml_cluster("OnOff", "0x0006", is_mandatory=True)
        cluster.attribute_name_list = []
        cluster.feature_name_list = []
        cluster.command_name_list = []
        cluster.event_name_list = []
        device.clusters = {cluster}

        result = DeviceSerializer.to_dict(device)
        clusters = result["clusters"]
        self.assertEqual(len(clusters), 1)
        self.assertNotIn("optional_choice", clusters[0])


# ---------------------------------------------------------------------------
# Integration test: Full device with O.a+ clusters
# ---------------------------------------------------------------------------


class TestElectricalSensorLikeDevice(unittest.TestCase):
    """Integration test simulating ElectricalSensor device type with O.a+ clusters."""

    def test_electrical_sensor_pattern(self):
        """Simulate ElectricalSensor with power_topology (M) and EPM/EEM (O.a+)."""
        device = CodegenDevice(id="0x0510", name="Electrical_Sensor", revision=1)

        # Mandatory cluster (use name that produces "power_topology" in esp_name)
        power_topology = _make_codegen_cluster(
            "Power Topology", "0x009C", is_mandatory=True
        )

        # O.a+ clusters
        epm = _make_codegen_cluster(
            "Electrical Power Measurement",
            "0x0090",
            optional_choice={"choice": "a", "more": True, "min": 1},
        )
        eem = _make_codegen_cluster(
            "Electrical Energy Measurement",
            "0x0091",
            optional_choice={"choice": "a", "more": True, "min": 1},
        )

        device.clusters = [power_topology, epm, eem]

        # Verify mandatory clusters
        mandatory = device.get_mandatory_clusters()
        self.assertEqual(len(mandatory), 1)
        self.assertEqual(mandatory[0].esp_name, "power_topology")

        # Verify O.a+ clusters
        self.assertTrue(device.has_optional_choice_clusters())
        unique_oa = device.get_unique_optional_choice_clusters()
        self.assertEqual(len(unique_oa), 2)

        # Verify grouped by choice marker
        groups = device.get_optional_choice_clusters()
        self.assertIn("a", groups)
        self.assertEqual(len(groups["a"]), 2)


class TestCookSurfaceLikeDevice(unittest.TestCase):
    """Integration test simulating CookSurface device type with O.a+ clusters."""

    def test_cook_surface_pattern(self):
        """Simulate CookSurface with temperature_control and temperature_measurement (O.a+)."""
        device = CodegenDevice(id="0x0077", name="Cook_Surface", revision=2)

        # O.a+ clusters
        temp_control = _make_codegen_cluster(
            "TemperatureControl",
            "0x0056",
            optional_choice={"choice": "a", "more": True, "min": 1},
        )
        temp_measurement = _make_codegen_cluster(
            "TemperatureMeasurement",
            "0x0402",
            optional_choice={"choice": "a", "more": True, "min": 1},
        )

        device.clusters = [temp_control, temp_measurement]

        # Verify O.a+ clusters
        self.assertTrue(device.has_optional_choice_clusters())
        unique_oa = device.get_unique_optional_choice_clusters()
        self.assertEqual(len(unique_oa), 2)

        # Should be sorted by ID (0x0056 < 0x0402)
        self.assertEqual(unique_oa[0].get_id(), "0x0056")
        self.assertEqual(unique_oa[1].get_id(), "0x0402")


if __name__ == "__main__":
    unittest.main()
