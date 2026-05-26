# Copyright 2026 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Tests for chip_source_deps/cluster_mapping.py — cluster name normalization and scanning."""

import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from chip_source_deps.cluster_mapping import (  # noqa: E402
    _get_cluster_name_prefix,
    normalize_cluster_name,
    process_cluster_files,
)


class TestGetClusterNamePrefix(unittest.TestCase):
    """Tests for _get_cluster_name_prefix() — suffix and extension stripping only."""

    def test_directory_path(self):
        self.assertEqual(
            _get_cluster_name_prefix("/chip/clusters/on-off-server"),
            "on-off",
        )

    def test_server_cpp_filename(self):
        self.assertEqual(
            _get_cluster_name_prefix("door-lock-server.cpp"),
            "door-lock",
        )

    def test_pascal_case_cluster_cpp(self):
        self.assertEqual(
            _get_cluster_name_prefix("LevelControlCluster.cpp"),
            "LevelControl",
        )

    def test_strips_cluster_substring_in_hyphenated_name(self):
        self.assertEqual(
            _get_cluster_name_prefix("mode-base-cluster-objects.cpp"),
            "mode-base-objects",
        )

    def test_plugin_callback_name_unchanged(self):
        self.assertEqual(_get_cluster_name_prefix("OnOff"), "OnOff")


class TestNormalizeClusterName(unittest.TestCase):
    """Tests for normalize_cluster_name() — esp-matter cluster keys."""

    def test_cluster_directory(self):
        self.assertEqual(
            normalize_cluster_name("/connectedhomeip/src/app/clusters/on-off-server"),
            "on_off",
        )

    def test_server_cpp(self):
        self.assertEqual(
            normalize_cluster_name("thermostat-server.cpp"),
            "thermostat",
        )

    def test_pascal_case_cpp(self):
        self.assertEqual(
            normalize_cluster_name("LevelControlCluster.cpp"),
            "level_control",
        )
        self.assertEqual(
            normalize_cluster_name("OnOffCluster.cpp"),
            "on_off",
        )

    def test_plugin_init_callback_identifier(self):
        self.assertEqual(normalize_cluster_name("OnOff"), "on_off")
        self.assertEqual(normalize_cluster_name("LevelControl"), "level_control")
        self.assertEqual(
            normalize_cluster_name("OTARequestor"),
            "ota_software_update_requestor",
        )

    def test_local_mappings(self):
        self.assertEqual(
            normalize_cluster_name("boolean-state-server"),
            "boolean_state_configuration",
        )
        self.assertEqual(
            normalize_cluster_name("dishwasher-alarm-server.cpp"),
            "dish_washer_alarm",
        )
        self.assertEqual(
            normalize_cluster_name("group-key-mgmt-server"),
            "group_key_management",
        )
        self.assertEqual(
            normalize_cluster_name("bindings-server"),
            "binding",
        )

    def test_ota_requestor_from_directory_and_cpp(self):
        self.assertEqual(
            normalize_cluster_name("ota-requestor"),
            "ota_software_update_requestor",
        )
        self.assertEqual(
            normalize_cluster_name("OTARequestorCluster.cpp"),
            "ota_software_update_requestor",
        )

    def test_idempotent_snake_case_input(self):
        self.assertEqual(normalize_cluster_name("level_control"), "level_control")

    def test_normalize_cases(self):
        cases = [
            ("/clusters/door-lock-server", "door_lock"),
            ("occupancy-sensor-server.cpp", "occupancy_sensor"),
            ("GeneralDiagnosticsCluster.cpp", "general_diagnostics"),
            ("WiFiNetworkManagement", "wi_fi_network_management"),
        ]
        for name, expected in cases:
            with self.subTest(name=name, expected=expected):
                self.assertEqual(normalize_cluster_name(name), expected)


class ClusterFixtureTestCase(unittest.TestCase):
    """Builds a temporary CHIP-style cluster tree for process_cluster_files() tests."""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.root = self._tmp.name

    def tearDown(self):
        self._tmp.cleanup()

    def _cluster_dir(self, cluster_dir: str) -> str:
        path = os.path.join(self.root, cluster_dir)
        os.makedirs(path, exist_ok=True)
        return path

    def _write_header(self, cluster_dir: str, filename: str, content: str) -> None:
        with open(
            os.path.join(self._cluster_dir(cluster_dir), filename),
            "w",
            encoding="utf-8",
        ) as f:
            f.write(content)

    def _write_codegen_header(
        self, cluster_dir: str, filename: str, content: str
    ) -> None:
        codegen_dir = os.path.join(self._cluster_dir(cluster_dir), "codegen")
        os.makedirs(codegen_dir, exist_ok=True)
        with open(os.path.join(codegen_dir, filename), "w", encoding="utf-8") as f:
            f.write(content)

    def _ensure_codegen_dir(self, cluster_dir: str) -> str:
        codegen_dir = os.path.join(self._cluster_dir(cluster_dir), "codegen")
        os.makedirs(codegen_dir, exist_ok=True)
        return codegen_dir

    def _write_file(self, directory: str, filename: str, content: str) -> None:
        with open(os.path.join(directory, filename), "w", encoding="utf-8") as f:
            f.write(content)


class TestProcessClusterFilesMigrated(ClusterFixtureTestCase):
    """Migrated cluster detection via ``public DefaultServerCluster`` in headers."""

    def test_detects_migrated_cluster_without_codegen(self):
        self._write_header(
            "door-lock-server",
            "door-lock-server.h",
            "class DoorLockCluster : public DefaultServerCluster\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster"],
            ["door_lock"],
        )
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            [],
        )

    def test_codegen_cluster_not_listed_as_migrated(self):
        self._write_codegen_header(
            "on-off-server",
            "on-off-server.h",
            "class OnOffCluster : public DefaultServerCluster\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            ["on_off"],
        )

    def test_legacy_header_without_default_server_cluster(self):
        self._write_header(
            "thermostat-server",
            "thermostat-server.h",
            "class ThermostatAttrAccess\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            [],
        )


class TestProcessClusterFilesDelegate(ClusterFixtureTestCase):
    """Delegate cluster detection from header patterns."""

    def test_detects_delegate_member(self):
        self._write_header(
            "chime-server",
            "chime-server.h",
            "    ChimeDelegate & mDelegate;\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], ["chime"])

    def test_detects_set_delegate_method(self):
        self._write_header(
            "door-lock-server",
            "door-lock-server.h",
            "    void SetDelegate(EndpointId endpoint, Delegate * delegate);\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], ["door_lock"])

    def test_ignores_timer_delegate_infrastructure(self):
        self._write_header(
            "groupcast",
            "GroupcastContext.h",
            "    chip::TimerDelegate & timerDelegate;\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], [])

    def test_ignores_delegate_skip_list_cluster(self):
        self._write_header(
            "identify-server",
            "IdentifyCluster.h",
            "    IdentifyDelegate * mDelegate;\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], [])

    def test_ignores_delegate_skip_list_even_with_set_delegate(self):
        self._write_header(
            "ota-requestor",
            "OTARequestorCluster.h",
            "    void SetDelegate(OTARequestorDelegate * delegate);\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], [])

    def test_combined_scan_results(self):
        self._write_header(
            "door-lock-server",
            "door-lock-server.h",
            "class DoorLockCluster : public DefaultServerCluster\n"
            "    Delegate * mDelegate;\n",
        )
        self._write_codegen_header(
            "level-control",
            "level-control.h",
            "class LevelControlCluster : public DefaultServerCluster\n",
        )
        self._write_header(
            "thermostat-server",
            "thermostat-server.h",
            "// legacy cluster\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster"],
            ["door_lock"],
        )
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            ["level_control"],
        )
        self.assertEqual(result["delegate_clusters"], ["door_lock"])


class TestProcessClusterFilesCodegen(ClusterFixtureTestCase):
    """``codegen/`` layout: headers scanned there only; parent dir headers ignored."""

    def test_codegen_impl_listed_when_codegen_dir_exists_even_if_empty(self):
        self._ensure_codegen_dir("on-off-server")
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            ["on_off"],
        )

    def test_parent_headers_ignored_when_codegen_present(self):
        self._write_header(
            "on-off-server",
            "OnOffCluster.h",
            "class OnOffCluster : public DefaultServerCluster\n"
            "    OnOffDelegate * mDelegate;\n",
        )
        self._write_codegen_header(
            "on-off-server",
            "CodegenIntegration.h",
            "// codegen implementation\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(result["delegate_clusters"], [])
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            ["on_off"],
        )

    def test_delegate_detected_from_codegen_header_only(self):
        self._write_header(
            "level-control",
            "LevelControlCluster.h",
            "    LevelControlDelegate * mDelegate;\n",
        )
        self._write_codegen_header(
            "level-control",
            "level-control.h",
            "    void SetDelegate(LevelControlDelegate * delegate);\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(result["delegate_clusters"], ["level_control"])
        self.assertIn(
            "level_control",
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
        )

    def test_default_server_cluster_in_codegen_does_not_mark_migrated(self):
        self._write_codegen_header(
            "fan-control-server",
            "fan-control-server.h",
            "class FanControlCluster : public DefaultServerCluster\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            ["fan_control"],
        )

    def test_non_header_files_under_codegen_are_ignored(self):
        codegen_dir = self._ensure_codegen_dir("on-off-server")
        self._write_file(
            codegen_dir,
            "on-off-server.cpp",
            "class OnOffCluster : public DefaultServerCluster\n"
            "    void SetDelegate(Delegate * delegate);\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(result["delegate_clusters"], [])

    def test_multiple_codegen_clusters_sorted(self):
        self._write_codegen_header(
            "on-off-server",
            "on-off-server.h",
            "// on off\n",
        )
        self._write_codegen_header(
            "level-control",
            "level-control.h",
            "// level\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            ["level_control", "on_off"],
        )

    def test_codegen_does_not_walk_nested_subdirectories(self):
        cluster_path = self._cluster_dir("on-off-server")
        self._ensure_codegen_dir("on-off-server")
        tests_dir = os.path.join(cluster_path, "tests")
        os.makedirs(tests_dir, exist_ok=True)
        self._write_file(
            tests_dir,
            "TestOnOffCluster.h",
            "    void SetDelegate(Delegate * delegate);\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], [])


class TestProcessClusterFilesEdgeCases(ClusterFixtureTestCase):
    """Miscellaneous scan behavior and delegate pattern variants."""

    def test_empty_root_directory(self):
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            [],
        )
        self.assertEqual(result["delegate_clusters"], [])

    def test_detects_get_delegate_and_set_default_delegate(self):
        self._write_header(
            "window-covering-server",
            "window-covering-server.h",
            "    Delegate * GetDelegate(EndpointId endpoint);\n"
            "    void SetDefaultDelegate(EndpointId endpoint, Delegate * delegate);\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], ["window_covering"])

    def test_ignores_fabric_delegate_infrastructure(self):
        self._write_header(
            "groupcast",
            "GroupcastContext.h",
            "    FabricDelegate & fabricDelegate;\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], [])

    def test_ignores_timer_delegate_infrastructure(self):
        self._write_header(
            "on-off-server",
            "OnOffCluster.h",
            "    chip::TimerDelegate & timerDelegate;\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], [])

    def test_ignores_basic_information_platform_manager_delegate(self):
        """PolicyBased cluster uses PlatformManager Get/SetDelegate, not app cluster delegate."""
        self._write_header(
            "basic-information",
            "DeviceLayerBasicInformationPolicy.h",
            "    void RegisterPlatformDelegate(DeviceLayer::PlatformManagerDelegate * delegate)\n"
            "    {\n"
            "        if (mPlatformManager.GetDelegate() == nullptr)\n"
            "            mPlatformManager.SetDelegate(delegate);\n"
            "    }\n",
        )
        self._write_header(
            "basic-information",
            "BasicInformationCluster.h",
            "using BasicInformationCluster = BasicInformation::PolicyBased<DeviceLayerBasicInformationPolicy>;\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], [])

    def test_non_header_files_in_cluster_root_are_ignored(self):
        self._write_header(
            "thermostat-server",
            "thermostat-server.h",
            "// legacy\n",
        )
        self._write_file(
            self._cluster_dir("thermostat-server"),
            "thermostat-server.cpp",
            "class ThermostatAttrAccess : public DefaultServerCluster\n"
            "    void SetDelegate(Delegate * delegate);\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["migrated_clusters"]["migrated_cluster"], [])
        self.assertEqual(result["delegate_clusters"], [])

    def test_multiple_clusters_mixed_legacy_codegen_and_delegate(self):
        self._write_header(
            "door-lock-server",
            "door-lock-server.h",
            "class DoorLockCluster : public DefaultServerCluster\n"
            "    void SetDelegate(EndpointId endpoint, Delegate * delegate);\n",
        )
        self._write_header(
            "chime-server",
            "chime-server.h",
            "    ChimeDelegate & mDelegate;\n",
        )
        self._write_codegen_header("on-off-server", "stub.h", "// codegen\n")
        self._write_header(
            "thermostat-server",
            "thermostat-server.h",
            "// legacy only\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster"],
            ["door_lock"],
        )
        self.assertEqual(
            result["migrated_clusters"]["migrated_cluster_with_codegen_impl"],
            ["on_off"],
        )
        self.assertEqual(
            result["delegate_clusters"],
            ["chime", "door_lock"],
        )

    def test_result_lists_are_sorted(self):
        self._write_header(
            "chime-server",
            "chime-server.h",
            "    ChimeDelegate & mDelegate;\n",
        )
        self._write_header(
            "door-lock-server",
            "door-lock-server.h",
            "    Delegate * mDelegate;\n",
        )
        result = process_cluster_files(self.root)
        self.assertEqual(result["delegate_clusters"], ["chime", "door_lock"])


if __name__ == "__main__":
    unittest.main()
