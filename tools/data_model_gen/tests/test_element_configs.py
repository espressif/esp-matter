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

"""Tests for code_generation/element_configs.py — preprocessor guard lookups."""

import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from code_generation.element_configs import (  # noqa: E402
    apply_cluster_special_configs,
    apply_device_cluster_configs,
    get_config_guard,
)
from code_generation.elements import Attribute, Cluster, Command, Device, Feature  # noqa: E402


class TestGetConfigMacro(unittest.TestCase):
    """Test config macro lookups."""

    def test_icd_management_cluster(self):
        self.assertEqual(
            get_config_guard("0x0046", "Cluster"),
            "CHIP_CONFIG_ENABLE_ICD_SERVER",
        )

    def test_wifi_feature_by_id(self):
        self.assertEqual(
            get_config_guard("0x0031", "Feature", "0x1"),
            "CHIP_DEVICE_CONFIG_ENABLE_WIFI",
        )

    def test_thread_feature(self):
        self.assertEqual(
            get_config_guard("0x0031", "Feature", "0x0002"),
            "CHIP_DEVICE_CONFIG_ENABLE_THREAD",
        )

    def test_icd_lit_feature(self):
        self.assertEqual(
            get_config_guard("0x0046", "Feature", "0x0004"),
            "CHIP_CONFIG_ENABLE_ICD_LIT",
        )

    def test_access_control_attribute(self):
        self.assertEqual(
            get_config_guard("0x001F", "Attribute", "0x0005"),
            "CHIP_CONFIG_USE_ACCESS_RESTRICTIONS",
        )

    def test_no_config(self):
        self.assertIsNone(get_config_guard("0x0006", "Cluster"))
        self.assertIsNone(get_config_guard("0x0046", "Feature", "0x0008"))

    def test_compound_config_guard_list(self):
        self.assertEqual(
            get_config_guard("0x0036", "Cluster"),
            "CHIP_DEVICE_CONFIG_ENABLE_WIFI && defined(CONFIG_SUPPORT_WIFI_NETWORK_DIAGNOSTICS_CLUSTER)",
        )
        self.assertEqual(
            get_config_guard("0x0035", "Cluster"),
            "CHIP_DEVICE_CONFIG_ENABLE_THREAD && defined(CONFIG_SUPPORT_THREAD_NETWORK_DIAGNOSTICS_CLUSTER)",
        )
        self.assertEqual(
            get_config_guard("0x0065", "Cluster"),
            "defined(CONFIG_SUPPORT_GROUPCAST_CLUSTER)",
        )


class TestApplyDeviceClusterConfigs(unittest.TestCase):
    def test_applies_config_guard_to_extra_cluster_stub(self):
        device = Device(id="0x0016", name="Root Node", revision=5)
        device.extra_clusters = [
            Cluster(name="Groupcast", id="0x0065", revision=1, is_mandatory=False),
        ]
        device.extra_clusters[0].is_device_extra = True

        apply_device_cluster_configs(device)

        self.assertEqual(
            device.extra_clusters[0].special_config,
            "defined(CONFIG_SUPPORT_GROUPCAST_CLUSTER)",
        )


class TestApplyClusterSpecialConfigs(unittest.TestCase):
    """Test applying special_config during code generation deserialization."""

    def test_applies_to_cluster_and_children(self):
        cluster = Cluster(
            name="ICD Management",
            id="0x0046",
            revision=3,
            is_mandatory=False,
        )
        cluster.features = [
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
        ]
        cluster.attributes = [
            Attribute(
                name="IdleModeDuration",
                id="0x0003",
                type_="uint32",
                is_mandatory=True,
                default_value="0",
            ),
        ]
        cluster.commands = [
            Command(
                name="RegisterClient",
                id="0x0000",
                is_mandatory=False,
                direction="commandToServer",
                response="Y",
            ),
        ]

        apply_cluster_special_configs(cluster)

        self.assertEqual(cluster.special_config, "CHIP_CONFIG_ENABLE_ICD_SERVER")
        self.assertEqual(
            cluster.features[0].special_config, "CHIP_CONFIG_ENABLE_ICD_LIT"
        )
        self.assertEqual(
            cluster.features[1].special_config, "CHIP_CONFIG_ENABLE_ICD_CIP"
        )
        self.assertIsNone(cluster.attributes[0].special_config)
        self.assertIsNone(cluster.commands[0].special_config)

    def test_defaults_to_none_before_apply(self):
        cluster = Cluster(
            name="OnOff",
            id="0x0006",
            revision=6,
            is_mandatory=False,
        )
        self.assertFalse(cluster.has_special_config())
        self.assertIsNone(cluster.get_special_config())


if __name__ == "__main__":
    unittest.main()
