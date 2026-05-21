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

"""Tests for utils/overrides.py — name overrides, reserved words, skip lists, special config."""

import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from utils.overrides import (  # noqa: E402
    normalize_cluster_display_name,
    normalize_feature_name,
    normalize_device_type_name,
    normalize_element_name,
    is_cpp_reserved_word,
    should_skip_cluster_command_callbacks,
    should_skip_delegate_callback,
    should_include_delegate_callback,
    should_skip_plugin_callback,
    should_skip_internally_managed_flag,
    get_overridden_cluster_init_callback_name,
    get_overridden_cluster_shutdown_callback_name,
)


class TestClusterNameOverrides(unittest.TestCase):
    """Test cluster display name normalization."""

    def test_onoff_override(self):
        self.assertEqual(
            normalize_cluster_display_name("OnOff", cluster_id="0x0006"), "On/Off"
        )

    def test_operational_credentials_override(self):
        self.assertEqual(
            normalize_cluster_display_name(
                "Node Operational Credentials", cluster_id="0x003E"
            ),
            "Operational Credentials",
        )

    def test_dishwasher_override(self):
        self.assertEqual(
            normalize_cluster_display_name("Dishwasher Mode", cluster_id="0x0059"),
            "Dish Washer Mode",
        )

    def test_no_override(self):
        self.assertEqual(
            normalize_cluster_display_name("Thermostat", cluster_id="0x0201"),
            "Thermostat",
        )

    def test_name_fallback(self):
        """Without cluster_id, falls back to name-based lookup."""
        self.assertEqual(normalize_cluster_display_name("OnOff"), "On/Off")


class TestFeatureNameOverrides(unittest.TestCase):
    """Test feature name normalization."""

    def test_auto_override(self):
        self.assertEqual(
            normalize_feature_name("Auto", cluster_id="0x0202", feature_id="0x2"),
            "fan_auto",
        )

    def test_weekday_override(self):
        self.assertEqual(
            normalize_feature_name(
                "WeekDayAccessSchedules", cluster_id="0x0101", feature_id="0x10"
            ),
            "weekday_access_schedules",
        )

    def test_no_override(self):
        self.assertEqual(normalize_feature_name("Lighting"), "Lighting")

    def test_name_fallback(self):
        """Without IDs, falls back to name-based lookup."""
        self.assertEqual(normalize_feature_name("Auto"), "fan_auto")


class TestDeviceNameOverrides(unittest.TestCase):
    """Test device type name normalization."""

    def test_dishwasher_override(self):
        self.assertEqual(
            normalize_device_type_name("Dishwasher", device_id="0x0075"), "Dish Washer"
        )

    def test_no_override(self):
        self.assertEqual(normalize_device_type_name("Fan", device_id="0x002B"), "Fan")

    def test_name_fallback(self):
        """Without device_id, falls back to name-based lookup."""
        self.assertEqual(normalize_device_type_name("Dishwasher"), "Dish Washer")


class TestElementNameOverrides(unittest.TestCase):
    """Test element name normalization."""

    def test_pin_override(self):
        self.assertEqual(
            normalize_element_name(
                "RequirePINforRemoteOperation", cluster_id="0x0101", element_id="0x0033"
            ),
            "require_pin_for_remote_operation",
        )

    def test_soc_override(self):
        self.assertEqual(
            normalize_element_name(
                "NextChargeTargetSoC", cluster_id="0x0099", element_id="0x0026"
            ),
            "next_charge_target_soc",
        )

    def test_no_override(self):
        self.assertEqual(normalize_element_name("temperature"), "temperature")

    def test_name_fallback(self):
        """Without IDs, falls back to name-based lookup."""
        self.assertEqual(
            normalize_element_name("RequirePINforRemoteOperation"),
            "require_pin_for_remote_operation",
        )


class TestCppReservedWords(unittest.TestCase):
    """Test C++ reserved word detection."""

    def test_switch_is_reserved(self):
        self.assertTrue(is_cpp_reserved_word("switch"))

    def test_auto_is_reserved(self):
        self.assertTrue(is_cpp_reserved_word("auto"))

    def test_case_insensitive(self):
        self.assertTrue(is_cpp_reserved_word("SWITCH"))
        self.assertTrue(is_cpp_reserved_word("Switch"))

    def test_non_reserved(self):
        self.assertFalse(is_cpp_reserved_word("temperature"))
        self.assertFalse(is_cpp_reserved_word("on_off"))

    def test_observed_keywords(self):
        # Only words actually observed in practice in Matter XML are tracked;
        # the list intentionally stays minimal and grows on demand.
        for word in ["auto", "switch"]:
            self.assertTrue(is_cpp_reserved_word(word), f"{word} should be reserved")


class TestSkipLists(unittest.TestCase):
    """Test skip/include callback logic."""

    def test_skip_command_callback(self):
        self.assertTrue(
            should_skip_cluster_command_callbacks("0x0039")
        )  # bridged_device_basic_information

    def test_no_skip_command_callback(self):
        self.assertFalse(should_skip_cluster_command_callbacks("0x0006"))  # on_off

    def test_skip_delegate_callback(self):
        self.assertTrue(
            should_skip_delegate_callback("0x0553")
        )  # webrtc_transport_provider

    def test_no_skip_delegate_callback(self):
        self.assertFalse(should_skip_delegate_callback("0x0201"))  # thermostat

    def test_include_delegate_callback(self):
        self.assertTrue(should_include_delegate_callback("0x0050"))  # mode_select

    def test_no_include_delegate_callback(self):
        self.assertFalse(should_include_delegate_callback("0x0201"))  # thermostat

    def test_skip_plugin_callback(self):
        self.assertTrue(should_skip_plugin_callback("0x0046"))  # icd_management

    def test_no_skip_plugin_callback(self):
        self.assertFalse(should_skip_plugin_callback("0x0006"))  # on_off


class TestInternallyManagedSkip(unittest.TestCase):
    """Test internally managed attribute skip logic."""

    def test_skip_icd_attribute(self):
        self.assertTrue(
            should_skip_internally_managed_flag(
                "0x0046", "0x0006"
            )  # icd_management, user_active_mode_trigger_hint
        )

    def test_skip_thermostat_attribute(self):
        self.assertTrue(
            should_skip_internally_managed_flag(
                "0x0201", "0x0000"
            )  # thermostat, local_temperature
        )

    def test_no_skip_unknown_cluster(self):
        self.assertFalse(
            should_skip_internally_managed_flag("0x0006", "0x0000")  # on_off, some_attr
        )

    def test_no_skip_unknown_attribute(self):
        self.assertFalse(
            should_skip_internally_managed_flag(
                "0x0201", "0x0001"
            )  # thermostat, unknown_attr
        )


class TestCallbackNameOverrides(unittest.TestCase):
    """Test WebRTC callback name overrides."""

    def test_webrtc_provider_init(self):
        result = get_overridden_cluster_init_callback_name(
            "0x0553", "WebrtcTransportProvider"
        )
        self.assertIn("WebRTC", result)

    def test_webrtc_requestor_shutdown(self):
        result = get_overridden_cluster_shutdown_callback_name(
            "0x0554", "WebrtcTransportRequestor"
        )
        self.assertIn("WebRTC", result)

    def test_no_override(self):
        result = get_overridden_cluster_init_callback_name("0x0201", "Thermostat")
        self.assertEqual(result, "ESPMatterThermostatClusterServerInitCallback")


if __name__ == "__main__":
    unittest.main()
