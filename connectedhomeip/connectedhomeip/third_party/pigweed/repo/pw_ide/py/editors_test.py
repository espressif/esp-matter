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

from collections import OrderedDict
from enum import Enum
import unittest

from pw_ide.editors import (
    dict_deep_merge,
    EditorSettingsFile,
    EditorSettingsManager,
    JsonFileFormat,
)

from test_cases import PwIdeTestCase


class TestDictDeepMerge(unittest.TestCase):
    """Tests dict_deep_merge"""

    def test_invariants_with_dict_success(self):
        # pylint: disable=unnecessary-lambda
        dict_a = {'hello': 'world'}
        dict_b = {'foo': 'bar'}

        expected = {
            'hello': 'world',
            'foo': 'bar',
        }

        result = dict_deep_merge(dict_b, dict_a, lambda: dict())
        self.assertEqual(result, expected)

    def test_invariants_with_dict_implicit_ctor_success(self):
        # pylint: disable=unnecessary-lambda
        dict_a = {'hello': 'world'}
        dict_b = {'foo': 'bar'}

        expected = {
            'hello': 'world',
            'foo': 'bar',
        }

        result = dict_deep_merge(dict_b, dict_a)
        self.assertEqual(result, expected)

    def test_invariants_with_dict_fails_wrong_ctor_type(self):
        # pylint: disable=unnecessary-lambda
        dict_a = {'hello': 'world'}
        dict_b = {'foo': 'bar'}

        with self.assertRaises(TypeError):
            dict_deep_merge(dict_b, dict_a, lambda: OrderedDict())

    def test_invariants_with_ordered_dict_success(self):
        # pylint: disable=unnecessary-lambda
        dict_a = OrderedDict({'hello': 'world'})
        dict_b = OrderedDict({'foo': 'bar'})

        expected = OrderedDict(
            {
                'hello': 'world',
                'foo': 'bar',
            }
        )

        result = dict_deep_merge(dict_b, dict_a, lambda: OrderedDict())
        self.assertEqual(result, expected)

    def test_invariants_with_ordered_dict_implicit_ctor_success(self):
        # pylint: disable=unnecessary-lambda
        dict_a = OrderedDict({'hello': 'world'})
        dict_b = OrderedDict({'foo': 'bar'})

        expected = OrderedDict(
            {
                'hello': 'world',
                'foo': 'bar',
            }
        )

        result = dict_deep_merge(dict_b, dict_a)
        self.assertEqual(result, expected)

    def test_invariants_with_ordered_dict_fails_wrong_ctor_type(self):
        # pylint: disable=unnecessary-lambda
        dict_a = OrderedDict({'hello': 'world'})
        dict_b = OrderedDict({'foo': 'bar'})

        with self.assertRaises(TypeError):
            dict_deep_merge(dict_b, dict_a, lambda: dict())


class TestEditorSettingsFile(PwIdeTestCase):
    """Tests EditorSettingsFile"""

    def test_open_new_file_and_write(self):
        name = 'settings'
        json_fmt = JsonFileFormat()
        settings_file = EditorSettingsFile(self.temp_dir_path, name, json_fmt)

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        with open(self.temp_dir_path / f'{name}.{json_fmt.ext}') as file:
            settings_dict = json_fmt.load(file)

        self.assertEqual(settings_dict['hello'], 'world')

    def test_open_new_file_and_get(self):
        name = 'settings'
        json_fmt = JsonFileFormat()
        settings_file = EditorSettingsFile(self.temp_dir_path, name, json_fmt)

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        settings_dict = settings_file.get()
        self.assertEqual(settings_dict['hello'], 'world')

    def test_open_new_file_no_backup(self):
        name = 'settings'
        json_fmt = JsonFileFormat()
        settings_file = EditorSettingsFile(self.temp_dir_path, name, json_fmt)

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        backup_files = [
            path
            for path in self.temp_dir_path.iterdir()
            if path.name != f'{name}.{json_fmt.ext}'
        ]

        self.assertEqual(len(backup_files), 0)

    def test_open_existing_file_and_backup(self):
        name = 'settings'
        json_fmt = JsonFileFormat()
        settings_file = EditorSettingsFile(self.temp_dir_path, name, json_fmt)

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        with settings_file.modify() as settings:
            settings['hello'] = 'mundo'

        settings_dict = settings_file.get()
        self.assertEqual(settings_dict['hello'], 'mundo')

        backup_files = [
            path
            for path in self.temp_dir_path.iterdir()
            if path.name != f'{name}.{json_fmt.ext}'
        ]

        self.assertEqual(len(backup_files), 1)

        with open(backup_files[0]) as file:
            settings_dict = json_fmt.load(file)

        self.assertEqual(settings_dict['hello'], 'world')

    def test_open_existing_file_with_reinit_and_backup(self):
        name = 'settings'
        json_fmt = JsonFileFormat()
        settings_file = EditorSettingsFile(self.temp_dir_path, name, json_fmt)

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        with settings_file.modify(reinit=True) as settings:
            settings['hello'] = 'mundo'

        settings_dict = settings_file.get()
        self.assertEqual(settings_dict['hello'], 'mundo')

        backup_files = [
            path
            for path in self.temp_dir_path.iterdir()
            if path.name != f'{name}.{json_fmt.ext}'
        ]

        self.assertEqual(len(backup_files), 1)

        with open(backup_files[0]) as file:
            settings_dict = json_fmt.load(file)

        self.assertEqual(settings_dict['hello'], 'world')

    def open_existing_file_no_change_no_backup(self):
        name = 'settings'
        json_fmt = JsonFileFormat()
        settings_file = EditorSettingsFile(self.temp_dir_path, name, json_fmt)

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        settings_dict = settings_file.get()
        self.assertEqual(settings_dict['hello'], 'world')

        backup_files = [
            path
            for path in self.temp_dir_path.iterdir()
            if path.name != f'{name}.{json_fmt.ext}'
        ]

        self.assertEqual(len(backup_files), 0)

        with open(backup_files[0]) as file:
            settings_dict = json_fmt.load(file)

        self.assertEqual(settings_dict['hello'], 'world')

    def test_write_bad_file_restore_backup(self):
        name = 'settings'
        json_fmt = JsonFileFormat()
        settings_file = EditorSettingsFile(self.temp_dir_path, name, json_fmt)

        with settings_file.modify() as settings:
            settings['hello'] = 'world'

        with self.assertRaises(TypeError):
            with settings_file.modify() as settings:
                settings['hello'] = object()

        settings_dict = settings_file.get()
        self.assertEqual(settings_dict['hello'], 'world')

        backup_files = [
            path
            for path in self.temp_dir_path.iterdir()
            if path.name != f'{name}.{json_fmt.ext}'
        ]

        self.assertEqual(len(backup_files), 0)


class EditorSettingsTestType(Enum):
    SETTINGS = 'settings'


class TestEditorSettingsManager(PwIdeTestCase):
    """Tests EditorSettingsManager"""

    def test_settings_merge(self):
        """Test that settings merge as expected in isolation."""
        default_settings = OrderedDict(
            {
                'foo': 'bar',
                'baz': 'qux',
                'lorem': OrderedDict(
                    {
                        'ipsum': 'dolor',
                    }
                ),
            }
        )

        types_with_defaults = {
            EditorSettingsTestType.SETTINGS: lambda _: default_settings
        }

        ide_settings = self.make_ide_settings()
        json_fmt = JsonFileFormat()
        manager = EditorSettingsManager(
            ide_settings, self.temp_dir_path, json_fmt, types_with_defaults
        )

        project_settings = OrderedDict(
            {
                'alpha': 'beta',
                'baz': 'xuq',
                'foo': 'rab',
            }
        )

        with manager.project(
            EditorSettingsTestType.SETTINGS
        ).modify() as settings:
            dict_deep_merge(project_settings, settings)

        user_settings = OrderedDict(
            {
                'baz': 'xqu',
                'lorem': OrderedDict(
                    {
                        'ipsum': 'sit amet',
                        'consectetur': 'adipiscing',
                    }
                ),
            }
        )

        with manager.user(EditorSettingsTestType.SETTINGS).modify() as settings:
            dict_deep_merge(user_settings, settings)

        expected = {
            'alpha': 'beta',
            'foo': 'rab',
            'baz': 'xqu',
            'lorem': {
                'ipsum': 'sit amet',
                'consectetur': 'adipiscing',
            },
        }

        with manager.active(
            EditorSettingsTestType.SETTINGS
        ).modify() as active_settings:
            manager.default(EditorSettingsTestType.SETTINGS).sync_to(
                active_settings
            )
            manager.project(EditorSettingsTestType.SETTINGS).sync_to(
                active_settings
            )
            manager.user(EditorSettingsTestType.SETTINGS).sync_to(
                active_settings
            )

        self.assertCountEqual(
            manager.active(EditorSettingsTestType.SETTINGS).get(), expected
        )


if __name__ == '__main__':
    unittest.main()
