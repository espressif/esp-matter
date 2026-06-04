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

"""Tests for utils/config.py — configuration, provisional mode, logger, file names."""

import unittest
import sys
import os
import logging
import tempfile
import shutil

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from utils.config import (  # noqa: E402
    setup_provisional_mode,
    allow_provisional,
    set_esp_matter_path,
    setup_logger,
    FileNames,
    discover_data_model_specification_versions,
    specification_version_sort_key,
    get_highest_data_model_version,
    get_chip_data_model_root,
)
from utils.exceptions import ConfigurationError  # noqa: E402


class TestProvisionalMode(unittest.TestCase):
    """Test provisional mode control."""

    def tearDown(self):
        setup_provisional_mode(False)

    def test_default_not_provisional(self):
        setup_provisional_mode(False)
        self.assertFalse(allow_provisional())

    def test_enable_provisional(self):
        setup_provisional_mode(True)
        self.assertTrue(allow_provisional())

    def test_disable_provisional(self):
        setup_provisional_mode(True)
        setup_provisional_mode(False)
        self.assertFalse(allow_provisional())


class TestSetEspMatterPath(unittest.TestCase):
    """Test set_esp_matter_path() — path validation."""

    def test_empty_path_raises(self):
        with self.assertRaises(ConfigurationError):
            set_esp_matter_path("")

    def test_whitespace_path_raises(self):
        with self.assertRaises(ConfigurationError):
            set_esp_matter_path("   ")

    def test_none_path_raises(self):
        with self.assertRaises(ConfigurationError):
            set_esp_matter_path(None)

    def test_nonexistent_path_raises(self):
        with self.assertRaises(ConfigurationError):
            set_esp_matter_path("/nonexistent/path/that/does/not/exist")

    def test_valid_path(self):
        import tempfile

        with tempfile.TemporaryDirectory() as tmpdir:
            set_esp_matter_path(tmpdir)


class TestSetupLogger(unittest.TestCase):
    """Test setup_logger() — logger configuration."""

    def test_setup_default(self):
        setup_logger("INFO", False)
        root = logging.getLogger()
        self.assertTrue(root.hasHandlers())

    def test_setup_colored(self):
        setup_logger("DEBUG", True)
        root = logging.getLogger()
        self.assertTrue(root.hasHandlers())


class TestFileNames(unittest.TestCase):
    """Test FileNames enum values."""

    def test_cluster_json(self):
        self.assertEqual(FileNames.CLUSTER_JSON.value, "clusters.json")

    def test_device_json(self):
        self.assertEqual(FileNames.DEVICE_JSON.value, "device_types.json")

    def test_all_values_are_json(self):
        for fn in FileNames:
            self.assertTrue(
                fn.value.endswith(".json"), f"{fn.name} should end with .json"
            )


def _make_fake_esp_matter_with_data_model(versions):
    """
    Build a minimal esp-matter tree with connectedhomeip/.../data_model/<ver>/{clusters,device_types}.
    """
    root = tempfile.mkdtemp()
    for ver in versions:
        base = os.path.join(
            root,
            "connectedhomeip",
            "connectedhomeip",
            "data_model",
            ver,
        )
        os.makedirs(os.path.join(base, "clusters"), exist_ok=True)
        os.makedirs(os.path.join(base, "device_types"), exist_ok=True)
    return root


class TestDataModelVersionDiscovery(unittest.TestCase):
    """Test discovery of data_model revision folders from the connectedhomeip submodule layout."""

    def test_discovers_sorted_versions(self):
        tmp = _make_fake_esp_matter_with_data_model(["1.4", "1.3", "1.5.1", "1.6"])
        self.addCleanup(shutil.rmtree, tmp, True)
        found = discover_data_model_specification_versions(tmp)
        self.assertEqual(found, ["1.3", "1.4", "1.5.1", "1.6"])

    def test_highest_version(self):
        tmp = _make_fake_esp_matter_with_data_model(["1.2", "1.3", "1.5.1", "1.6"])
        self.addCleanup(shutil.rmtree, tmp, True)
        self.assertEqual(get_highest_data_model_version(tmp), "1.6")

    def test_ignores_readme_and_non_version_names(self):
        tmp = _make_fake_esp_matter_with_data_model(["1.4", "1.5"])
        self.addCleanup(shutil.rmtree, tmp, True)
        readme = os.path.join(get_chip_data_model_root(tmp), "README.md")
        with open(readme, "w", encoding="utf-8") as f:
            f.write("x\n")
        bad = os.path.join(get_chip_data_model_root(tmp), "not-a-version")
        os.mkdir(bad)
        found = discover_data_model_specification_versions(tmp)
        self.assertEqual(found, ["1.4", "1.5"])

    def test_version_sort_key_orders_numeric_parts(self):
        self.assertLess(
            specification_version_sort_key("1.5.1"),
            specification_version_sort_key("1.6"),
        )
        self.assertLess(
            specification_version_sort_key("1.4.1"),
            specification_version_sort_key("1.4.2"),
        )


if __name__ == "__main__":
    unittest.main()
