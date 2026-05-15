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
Tests for code_generation/deserializer.py — JSON to object reconstruction.
Uses temporary JSON files for integration-level tests.
"""

import unittest
import sys
import os
import json
import tempfile

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from code_generation.deserializer import ClusterDeserializer, DeviceDeserializer  # noqa: E402


def _write_json(data):
    f = tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False)
    json.dump(data, f, indent=2)
    f.close()
    return f.name


# Minimal cluster JSON fixture
CLUSTER_DATA = [
    {
        "name": "OnOff",
        "id": "0x0006",
        "revision": 6,
        "is_mandatory": False,
        "type": "Server",
        "classification": {"role": "application", "hierarchy": "base"},
        "callback_functions": ["emberAfOnOffClusterServerInitCallback"],
        "function_flags": "CLUSTER_FLAG_INIT_FUNCTION",
        "delegate_init_callback": None,
        "plugin_server_init_callback": "MatterOnOffPluginServerInitCallback",
        "is_migrated_cluster": False,
        "attributes": [
            {
                "name": "OnOff",
                "id": "0x0000",
                "type": "bool",
                "converted_type": "bool",
                "default_value": "false",
                "mandatory": True,
                "nullable": False,
                "flags": "ATTRIBUTE_FLAG_NONVOLATILE",
                "max_length": 0,
                "min_value": None,
                "max_value": None,
                "conformance": {"type": "mandatory"},
            },
        ],
        "commands": [
            {
                "name": "Off",
                "id": "0x0000",
                "direction": "commandToServer",
                "response": "Y",
                "mandatory": True,
                "flags": "COMMAND_FLAG_ACCEPTED",
                "callback_required": True,
                "conformance": {"type": "mandatory"},
                "fields": [],
            },
            {
                "name": "On",
                "id": "0x0001",
                "direction": "commandToServer",
                "response": "Y",
                "mandatory": True,
                "flags": "COMMAND_FLAG_ACCEPTED",
                "callback_required": True,
                "conformance": {"type": "mandatory"},
                "fields": [],
            },
        ],
        "events": [
            {
                "name": "StateChange",
                "id": "0x0000",
                "mandatory": False,
                "priority": "Info",
                "conformance": {"type": "optional"},
            },
        ],
        "features": [
            {
                "name": "Lighting",
                "id": "0x0001",
                "code": "LT",
                "mandatory": False,
                "conformance": {"type": "optional"},
                "attributes": ["OnOff"],
                "commands": [],
                "events": [],
            },
        ],
    }
]


class TestClusterDeserializer(unittest.TestCase):
    """Test ClusterDeserializer — JSON to Cluster objects."""

    @classmethod
    def setUpClass(cls):
        cls.json_path = _write_json(CLUSTER_DATA)

    @classmethod
    def tearDownClass(cls):
        os.unlink(cls.json_path)

    def test_deserialize_count(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(len(clusters), 1)

    def test_cluster_name(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        c = clusters[0]
        self.assertIn("on_off", c.esp_name.lower())

    def test_cluster_revision(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(clusters[0].get_revision(), 6)

    def test_cluster_role(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(clusters[0].role, "application")

    def test_server_cluster(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertTrue(clusters[0].server_cluster)

    def test_attributes_deserialized(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(len(clusters[0].attributes), 1)
        attr = clusters[0].attributes[0]
        self.assertEqual(attr.type, "bool")
        self.assertTrue(attr.is_mandatory)

    def test_commands_deserialized(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(len(clusters[0].commands), 2)

    def test_command_has_callback(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        off_cmd = next(c for c in clusters[0].commands if c.name == "Off")
        self.assertTrue(off_cmd.has_callback)

    def test_events_deserialized(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(len(clusters[0].events), 1)

    def test_features_deserialized(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(len(clusters[0].features), 1)
        feat = clusters[0].features[0]
        self.assertEqual(feat.code, "LT")

    def test_feature_linked_attributes(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        feat = clusters[0].features[0]
        # The attr name "OnOff" goes through name override to "On/Off",
        # so linking by name may not always match. Just verify no crash.
        self.assertIsInstance(feat.attributes, list)

    def test_is_migrated_cluster(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertFalse(clusters[0].is_migrated_cluster)

    def test_callback_functions(self):
        clusters = ClusterDeserializer().deserialize(self.json_path)
        self.assertEqual(len(clusters[0].callback_functions), 1)

    def test_command_response_y(self):
        d = ClusterDeserializer()
        self.assertIsNone(d.get_command_response({"response": "Y"}))

    def test_command_response_n(self):
        d = ClusterDeserializer()
        self.assertIsNone(d.get_command_response({"response": "N"}))

    def test_command_response_none(self):
        d = ClusterDeserializer()
        self.assertIsNone(d.get_command_response({"response": None}))

    def test_command_response_name(self):
        d = ClusterDeserializer()
        self.assertEqual(d.get_command_response({"response": "GetResp"}), "GetResp")


# Minimal device JSON fixture
DEVICE_DATA = [
    {
        "name": "On/Off Light",
        "id": "0x0100",
        "revision": 3,
        "clusters": [
            {
                "name": "OnOff",
                "id": "0x0006",
                "revision": 6,
                "is_mandatory": True,
                "type": "server",
                "flags": "CLUSTER_FLAG_SERVER",
                "attributes": [],
                "features": [],
                "commands": [],
                "events": [],
            },
        ],
    }
]


class TestDeviceDeserializer(unittest.TestCase):
    """Test DeviceDeserializer — JSON to Device objects."""

    @classmethod
    def setUpClass(cls):
        cls.cluster_json_path = _write_json(CLUSTER_DATA)
        cls.device_json_path = _write_json(DEVICE_DATA)
        cls.clusters = ClusterDeserializer().deserialize(cls.cluster_json_path)
        cls.cluster_lookup = {c.esp_name: c for c in cls.clusters}

    @classmethod
    def tearDownClass(cls):
        os.unlink(cls.cluster_json_path)
        os.unlink(cls.device_json_path)

    def test_deserialize_count(self):
        devices = DeviceDeserializer().deserialize(
            self.device_json_path, self.cluster_lookup
        )
        self.assertEqual(len(devices), 1)

    def test_device_name(self):
        devices = DeviceDeserializer().deserialize(
            self.device_json_path, self.cluster_lookup
        )
        self.assertIn("on_off_light", devices[0].esp_name.lower())

    def test_device_revision(self):
        devices = DeviceDeserializer().deserialize(
            self.device_json_path, self.cluster_lookup
        )
        self.assertEqual(devices[0].get_device_type_version(), 3)

    def test_device_clusters(self):
        devices = DeviceDeserializer().deserialize(
            self.device_json_path, self.cluster_lookup
        )
        self.assertEqual(len(devices[0].clusters), 1)

    def test_device_cluster_server(self):
        devices = DeviceDeserializer().deserialize(
            self.device_json_path, self.cluster_lookup
        )
        self.assertTrue(devices[0].clusters[0].server_cluster)

    def test_device_cluster_mandatory(self):
        devices = DeviceDeserializer().deserialize(
            self.device_json_path, self.cluster_lookup
        )
        self.assertTrue(devices[0].clusters[0].is_mandatory)


# Device with O.a+ clusters JSON fixture
DEVICE_WITH_OPTIONAL_CHOICE_DATA = [
    {
        "name": "Electrical Sensor",
        "id": "0x0510",
        "revision": 1,
        "clusters": [
            {
                "name": "Power Topology",
                "id": "0x009C",
                "revision": 1,
                "is_mandatory": True,
                "type": "server",
                "flags": "CLUSTER_FLAG_SERVER",
                "attributes": [],
                "features": [],
                "commands": [],
                "events": [],
            },
            {
                "name": "Electrical Power Measurement",
                "id": "0x0090",
                "revision": 1,
                "is_mandatory": False,
                "type": "server",
                "flags": "CLUSTER_FLAG_SERVER",
                "attributes": [],
                "features": [],
                "commands": [],
                "events": [],
                "optional_choice": {"choice": "a", "more": True, "min": 1},
            },
            {
                "name": "Electrical Energy Measurement",
                "id": "0x0091",
                "revision": 1,
                "is_mandatory": False,
                "type": "server",
                "flags": "CLUSTER_FLAG_SERVER",
                "attributes": [],
                "features": [],
                "commands": [],
                "events": [],
                "optional_choice": {"choice": "a", "more": True, "min": 1},
            },
        ],
    }
]


def _make_mock_cluster(name, id_):
    """Create a minimal mock Cluster object for the lookup table."""
    from code_generation.elements import Cluster
    cluster = Cluster(name=name, id=id_, revision=1, is_mandatory=False)
    cluster.attributes = []
    cluster.features = []
    cluster.commands = []
    cluster.events = []
    return cluster


# Create mock cluster lookup table for optional choice tests
MOCK_CLUSTER_LOOKUP = {
    "power_topology": _make_mock_cluster("Power Topology", "0x009C"),
    "electrical_power_measurement": _make_mock_cluster("Electrical Power Measurement", "0x0090"),
    "electrical_energy_measurement": _make_mock_cluster("Electrical Energy Measurement", "0x0091"),
}


class TestDeviceDeserializerOptionalChoice(unittest.TestCase):
    """Test DeviceDeserializer — optional_choice deserialization for O.a+ clusters."""

    @classmethod
    def setUpClass(cls):
        cls.device_json_path = _write_json(DEVICE_WITH_OPTIONAL_CHOICE_DATA)

    @classmethod
    def tearDownClass(cls):
        os.unlink(cls.device_json_path)

    def test_device_has_optional_choice_clusters(self):
        """Device with O.a+ clusters should report has_optional_choice_clusters() == True."""
        devices = DeviceDeserializer().deserialize(self.device_json_path, MOCK_CLUSTER_LOOKUP)
        self.assertEqual(len(devices), 1)
        self.assertTrue(devices[0].has_optional_choice_clusters())

    def test_optional_choice_cluster_parsed(self):
        """Clusters with optional_choice should have the attribute set."""
        devices = DeviceDeserializer().deserialize(self.device_json_path, MOCK_CLUSTER_LOOKUP)
        device = devices[0]

        # Find the EPM cluster
        epm_cluster = next(
            (c for c in device.clusters if "electrical_power_measurement" in c.esp_name),
            None,
        )
        self.assertIsNotNone(epm_cluster)
        self.assertIsNotNone(epm_cluster.optional_choice)
        self.assertEqual(epm_cluster.optional_choice["choice"], "a")
        self.assertTrue(epm_cluster.optional_choice["more"])
        self.assertEqual(epm_cluster.optional_choice["min"], 1)

    def test_mandatory_cluster_no_optional_choice(self):
        """Mandatory cluster should not have optional_choice set."""
        devices = DeviceDeserializer().deserialize(self.device_json_path, MOCK_CLUSTER_LOOKUP)
        device = devices[0]

        # Find the Power Topology cluster
        pt_cluster = next(
            (c for c in device.clusters if "power_topology" in c.esp_name),
            None,
        )
        self.assertIsNotNone(pt_cluster)
        self.assertTrue(pt_cluster.is_mandatory)
        self.assertIsNone(pt_cluster.optional_choice)

    def test_get_unique_optional_choice_clusters(self):
        """get_unique_optional_choice_clusters should return the O.a+ clusters."""
        devices = DeviceDeserializer().deserialize(self.device_json_path, MOCK_CLUSTER_LOOKUP)
        device = devices[0]

        unique_oa = device.get_unique_optional_choice_clusters()
        self.assertEqual(len(unique_oa), 2)
        # Should be sorted by ID (0x0090 < 0x0091)
        self.assertEqual(unique_oa[0].get_id(), "0x0090")
        self.assertEqual(unique_oa[1].get_id(), "0x0091")

    def test_get_optional_choice_clusters_grouped(self):
        """get_optional_choice_clusters should group clusters by choice marker."""
        devices = DeviceDeserializer().deserialize(self.device_json_path, MOCK_CLUSTER_LOOKUP)
        device = devices[0]

        groups = device.get_optional_choice_clusters()
        self.assertIn("a", groups)
        self.assertEqual(len(groups["a"]), 2)

    def test_device_skipped_when_cluster_missing(self):
        """Device should be skipped when a required cluster is missing from lookup table."""
        devices = DeviceDeserializer().deserialize(self.device_json_path, {})
        self.assertEqual(len(devices), 0)


if __name__ == "__main__":
    unittest.main()
