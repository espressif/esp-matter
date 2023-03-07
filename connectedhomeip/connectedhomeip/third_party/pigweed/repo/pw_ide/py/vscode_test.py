# Copyright 2022 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Tests for pw_ide.editors"""

import unittest

from pw_ide.vscode import VscSettingsManager, VscSettingsType

from test_cases import PwIdeTestCase


class TestVscSettingsManager(PwIdeTestCase):
    """Tests VscSettingsManager"""

    def test_setup(self):
        """Test realistic setup procedure. Success == doesn't raise."""
        ide_settings = self.make_ide_settings()
        manager = VscSettingsManager(ide_settings, self.temp_dir_path)

        with manager.active(
            VscSettingsType.SETTINGS
        ).modify() as active_settings:
            manager.default(VscSettingsType.SETTINGS).sync_to(active_settings)
            manager.project(VscSettingsType.SETTINGS).sync_to(active_settings)
            manager.user(VscSettingsType.SETTINGS).sync_to(active_settings)

        with manager.active(VscSettingsType.TASKS).modify() as active_settings:
            manager.default(VscSettingsType.TASKS).sync_to(active_settings)
            manager.project(VscSettingsType.TASKS).sync_to(active_settings)
            manager.user(VscSettingsType.TASKS).sync_to(active_settings)

        with manager.active(
            VscSettingsType.EXTENSIONS
        ).modify() as active_settings:
            manager.default(VscSettingsType.EXTENSIONS).sync_to(active_settings)
            manager.project(VscSettingsType.EXTENSIONS).sync_to(active_settings)
            manager.user(VscSettingsType.EXTENSIONS).sync_to(active_settings)

    def test_json5(self):
        """Test that we can parse JSON5 files."""
        content = """{
    // This is a comment, and this list has a trailing comma.
    "_pw": [
        "foo",
        "bar",
        "baz",
    ]
}
        """

        self.touch_temp_file('pw_project_settings.json', content)
        ide_settings = self.make_ide_settings()
        manager = VscSettingsManager(ide_settings, self.temp_dir_path)

        with manager.active(
            VscSettingsType.SETTINGS
        ).modify() as active_settings:
            manager.default(VscSettingsType.SETTINGS).sync_to(active_settings)
            manager.project(VscSettingsType.SETTINGS).sync_to(active_settings)
            manager.user(VscSettingsType.SETTINGS).sync_to(active_settings)

        active_settings = manager.active(VscSettingsType.SETTINGS).get()
        self.assertIn('_pw', active_settings.keys())
        self.assertEqual(len(active_settings['_pw']), 3)


if __name__ == '__main__':
    unittest.main()
