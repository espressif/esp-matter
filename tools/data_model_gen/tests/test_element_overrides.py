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

"""Tests for code_generation/element_overrides.py."""

import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from code_generation.element_configs import apply_cluster_special_configs
from code_generation.element_overrides import apply_device_type_element_overrides
from code_generation.elements import Cluster, Device, Feature


def _build_lookup(*clusters):
    return {cluster.esp_name: cluster for cluster in clusters}


class TestApplyDeviceTypeElementOverrides(unittest.TestCase):
    def test_merges_icd_features_for_root_node(self):
        icd_cluster = Cluster(
            name="ICD Management",
            id="0x0046",
            revision=3,
            is_mandatory=False,
        )
        icd_cluster.features = [
            Feature(
                name="LongIdleTimeSupport",
                id="0x0004",
                code="LITS",
                is_mandatory=False,
            ),
            Feature(
                name="CheckInProtocolSupport",
                id="0x0001",
                code="CIP",
                is_mandatory=False,
            ),
            Feature(
                name="UserActiveModeTrigger",
                id="0x0002",
                code="UAT",
                is_mandatory=False,
            ),
        ]
        apply_cluster_special_configs(icd_cluster)
        lookup = _build_lookup(icd_cluster)

        device = Device(id="0x0016", name="Root Node", revision=5)
        device.clusters = [
            Cluster(
                name="ICD Management",
                id="0x0046",
                revision=3,
                is_mandatory=False,
            )
        ]

        apply_device_type_element_overrides(device, lookup)

        icd = device.clusters[0]
        self.assertTrue(icd.is_mandatory)
        self.assertEqual(len(icd.device_mandatory_features), 3)
        self.assertEqual(
            [feature.func_name for feature in icd.device_mandatory_features],
            [
                "long_idle_time_support",
                "check_in_protocol_support",
                "user_active_mode_trigger",
            ],
        )
        self.assertEqual(
            icd.device_mandatory_features[0].special_config,
            "CHIP_CONFIG_ENABLE_ICD_LIT",
        )

    def test_merges_network_commissioning_features(self):
        nc_cluster = Cluster(
            name="Network Commissioning",
            id="0x0031",
            revision=4,
            is_mandatory=False,
        )
        nc_cluster.features = [
            Feature(
                name="WiFiNetworkInterface",
                id="0x0001",
                code="WI",
                is_mandatory=False,
            ),
            Feature(
                name="ThreadNetworkInterface",
                id="0x0002",
                code="TH",
                is_mandatory=False,
            ),
            Feature(
                name="EthernetNetworkInterface",
                id="0x0004",
                code="EN",
                is_mandatory=False,
            ),
        ]
        apply_cluster_special_configs(nc_cluster)
        nc_cluster.has_choice_features = lambda: True
        lookup = _build_lookup(nc_cluster)

        device = Device(id="0x0016", name="Root Node", revision=5)
        device.clusters = [
            Cluster(
                name="Network Commissioning",
                id="0x0031",
                revision=4,
                is_mandatory=True,
            )
        ]

        apply_device_type_element_overrides(device, lookup)

        nc = device.clusters[0]
        self.assertEqual(len(nc.device_mandatory_features), 3)
        self.assertTrue(nc.device_has_choice_features)
        self.assertEqual(
            nc.device_mandatory_features[0].special_config,
            "CHIP_DEVICE_CONFIG_ENABLE_WIFI",
        )

    def test_creates_cluster_stub_when_missing_from_device_xml(self):
        icd_cluster = Cluster(
            name="ICD Management",
            id="0x0046",
            revision=3,
            is_mandatory=False,
        )
        icd_cluster.features = [
            Feature(
                name="LongIdleTimeSupport",
                id="0x0004",
                code="LITS",
                is_mandatory=False,
            ),
        ]
        lookup = _build_lookup(icd_cluster)

        device = Device(id="0x0016", name="Root Node", revision=5)

        apply_device_type_element_overrides(device, lookup)

        self.assertEqual(len(device.clusters), 0)
        self.assertEqual(len(device.extra_clusters), 1)
        icd = device.extra_clusters[0]
        self.assertTrue(icd.is_device_extra)
        self.assertFalse(icd.is_mandatory)
        self.assertEqual(len(icd.device_mandatory_features), 1)

    def test_no_op_for_unknown_device_type(self):
        device = Device(id="0x0100", name="On/Off Light", revision=3)
        device.clusters = [
            Cluster(name="OnOff", id="0x0006", revision=6, is_mandatory=True)
        ]
        lookup = _build_lookup(device.clusters[0])

        apply_device_type_element_overrides(device, lookup)

        self.assertEqual(len(device.clusters[0].device_mandatory_features), 0)

    def test_adds_extra_clusters_for_root_node(self):
        wifi_diag = Cluster(
            name="Wi-Fi Network Diagnostics",
            id="0x0036",
            revision=1,
            is_mandatory=False,
        )
        thread_diag = Cluster(
            name="Thread Network Diagnostics",
            id="0x0035",
            revision=1,
            is_mandatory=False,
        )
        groupcast = Cluster(
            name="Groupcast",
            id="0x0065",
            revision=1,
            is_mandatory=False,
        )
        lookup = _build_lookup(wifi_diag, thread_diag, groupcast)

        device = Device(id="0x0016", name="Root Node", revision=5)
        apply_device_type_element_overrides(device, lookup)

        self.assertEqual(len(device.extra_clusters), 3)
        self.assertEqual(
            [cluster.esp_name for cluster in device.extra_clusters],
            [
                "wi_fi_network_diagnostics",
                "thread_network_diagnostics",
                "groupcast",
            ],
        )
        for cluster in device.extra_clusters[:2]:
            self.assertTrue(cluster.is_device_extra)
            self.assertTrue(cluster.server_cluster)

        groupcast = device.extra_clusters[2]
        self.assertTrue(groupcast.is_device_extra)
        self.assertTrue(groupcast.server_cluster)
        self.assertEqual(len(groupcast.device_mandatory_features), 0)
        self.assertFalse(groupcast.is_mandatory)

    def test_optional_xml_clusters_included_in_template_clusters(self):
        wifi_diag = Cluster(
            name="Wi-Fi Network Diagnostics",
            id="0x0036",
            revision=1,
            is_mandatory=False,
        )
        thread_diag = Cluster(
            name="Thread Network Diagnostics",
            id="0x0035",
            revision=1,
            is_mandatory=False,
        )
        lookup = _build_lookup(wifi_diag, thread_diag)

        device = Device(id="0x0016", name="Root Node", revision=5)
        device.clusters = [
            Cluster(
                name="Basic Information",
                id="0x0028",
                revision=4,
                is_mandatory=True,
            ),
            wifi_diag,
            thread_diag,
        ]

        apply_device_type_element_overrides(device, lookup)

        self.assertFalse(wifi_diag.is_device_extra)
        self.assertFalse(thread_diag.is_device_extra)
        self.assertEqual(
            [cluster.esp_name for cluster in device.extra_clusters],
            ["wi_fi_network_diagnostics", "thread_network_diagnostics"],
        )

        template_clusters = device.get_mandatory_clusters()
        self.assertEqual(
            [cluster.esp_name for cluster in template_clusters],
            [
                "basic_information",
                "wi_fi_network_diagnostics",
                "thread_network_diagnostics",
            ],
        )

    def test_no_extra_clusters_for_unknown_device_type(self):
        device = Device(id="0x0100", name="On/Off Light", revision=3)
        lookup = _build_lookup(
            Cluster(name="OnOff", id="0x0006", revision=6, is_mandatory=True)
        )

        apply_device_type_element_overrides(device, lookup)

        self.assertEqual(device.extra_clusters, [])


class TestGetTemplateClusters(unittest.TestCase):
    def test_includes_mandatory_and_extra_clusters(self):
        device = Device(id="0x0016", name="Root Node", revision=5)
        device.clusters = [
            Cluster(
                name="Basic Information", id="0x0028", revision=4, is_mandatory=True
            ),
            Cluster(name="OnOff", id="0x0006", revision=6, is_mandatory=False),
        ]
        device.extra_clusters = [
            Cluster(name="Groupcast", id="0x0065", revision=1, is_mandatory=False),
        ]
        device.extra_clusters[0].is_device_extra = True

        template_clusters = device.get_mandatory_clusters()
        self.assertEqual(len(template_clusters), 2)
        self.assertEqual(template_clusters[0].esp_name, "basic_information")
        self.assertEqual(template_clusters[1].esp_name, "groupcast")


if __name__ == "__main__":
    unittest.main()
