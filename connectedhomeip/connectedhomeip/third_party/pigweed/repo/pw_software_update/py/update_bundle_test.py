# Copyright 2021 The Pigweed Authors
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
"""Unit tests for pw_software_update/update_bundle.py."""

from pathlib import Path
import tempfile
import unittest

from pw_software_update import update_bundle
from pw_software_update.tuf_pb2 import SignedRootMetadata, TargetsMetadata


class TargetsFromDirectoryTest(unittest.TestCase):
    """Test turning a directory into TUF targets."""

    def test_excludes(self):
        """Checks that excludes are excluded."""
        with tempfile.TemporaryDirectory() as tempdir_name:
            temp_root = Path(tempdir_name)
            foo_path = temp_root / 'foo.bin'
            bar_path = temp_root / 'bar.bin'
            baz_path = temp_root / 'baz.bin'
            qux_path = temp_root / 'qux.exe'
            for path in (foo_path, bar_path, baz_path, qux_path):
                path.touch()

            targets = update_bundle.targets_from_directory(
                temp_root, exclude=(Path('foo.bin'), Path('baz.bin'))
            )

            self.assertNotIn('foo.bin', targets)
            self.assertEqual(bar_path, targets['bar.bin'])
            self.assertNotIn('baz.bin', targets)
            self.assertEqual(qux_path, targets['qux.exe'])

    def test_excludes_and_remapping(self):
        """Checks that remapping works, even in combination with excludes."""
        with tempfile.TemporaryDirectory() as tempdir_name:
            temp_root = Path(tempdir_name)
            foo_path = temp_root / 'foo.bin'
            bar_path = temp_root / 'bar.bin'
            baz_path = temp_root / 'baz.bin'
            qux_path = temp_root / 'qux.exe'
            remap_paths = {
                Path('foo.bin'): 'main',
                Path('bar.bin'): 'backup',
                Path('baz.bin'): 'tertiary',
            }
            for path in (foo_path, bar_path, baz_path, qux_path):
                path.touch()

            targets = update_bundle.targets_from_directory(
                temp_root, exclude=(Path('qux.exe'),), remap_paths=remap_paths
            )

            self.assertEqual(foo_path, targets['main'])
            self.assertEqual(bar_path, targets['backup'])
            self.assertEqual(baz_path, targets['tertiary'])
            self.assertNotIn('qux.exe', targets)

    def test_incomplete_remapping_logs(self):
        """Checks that incomplete remappings log warnings."""
        with tempfile.TemporaryDirectory() as tempdir_name:
            temp_root = Path(tempdir_name)
            foo_path = temp_root / 'foo.bin'
            bar_path = temp_root / 'bar.bin'
            foo_path.touch()
            bar_path.touch()
            remap_paths = {Path('foo.bin'): 'main'}

            with self.assertLogs(level='WARNING') as log:
                update_bundle.targets_from_directory(
                    temp_root,
                    exclude=(Path('qux.exe'),),
                    remap_paths=remap_paths,
                )

                self.assertIn(
                    'Some remaps defined, but not "bar.bin"', log.output[0]
                )

    def test_remap_of_missing_file(self):
        """Checks that remapping a missing file raises an error."""
        with tempfile.TemporaryDirectory() as tempdir_name:
            temp_root = Path(tempdir_name)
            foo_path = temp_root / 'foo.bin'
            foo_path.touch()
            remap_paths = {
                Path('foo.bin'): 'main',
                Path('bar.bin'): 'backup',
            }

            with self.assertRaises(FileNotFoundError):
                update_bundle.targets_from_directory(
                    temp_root, remap_paths=remap_paths
                )


class GenUnsignedUpdateBundleTest(unittest.TestCase):
    """Test the generation of unsigned update bundles."""

    def test_bundle_generation(self):
        """Tests basic creation of an UpdateBundle."""
        with tempfile.TemporaryDirectory() as tempdir_name:
            temp_root = Path(tempdir_name)
            foo_path = temp_root / 'foo.bin'
            bar_path = temp_root / 'bar.bin'
            baz_path = temp_root / 'baz.bin'
            qux_path = temp_root / 'subdir' / 'qux.exe'
            foo_bytes = b'\xf0\x0b\xa4'
            bar_bytes = b'\x0b\xa4\x99'
            baz_bytes = b'\xba\x59\x06'
            qux_bytes = b'\x8a\xf3\x12'
            foo_path.write_bytes(foo_bytes)
            bar_path.write_bytes(bar_bytes)
            baz_path.write_bytes(baz_bytes)
            (temp_root / 'subdir').mkdir()
            qux_path.write_bytes(qux_bytes)
            targets = {
                foo_path: 'foo',
                bar_path: 'bar',
                baz_path: 'baz',
                qux_path: 'qux',
            }
            serialized_root_metadata_bytes = b'\x12\x34\x56\x78'

            bundle = update_bundle.gen_unsigned_update_bundle(
                targets,
                targets_metadata_version=42,
                root_metadata=SignedRootMetadata(
                    serialized_root_metadata=serialized_root_metadata_bytes
                ),
            )

            self.assertEqual(foo_bytes, bundle.target_payloads['foo'])
            self.assertEqual(bar_bytes, bundle.target_payloads['bar'])
            self.assertEqual(baz_bytes, bundle.target_payloads['baz'])
            self.assertEqual(qux_bytes, bundle.target_payloads['qux'])
            targets_metadata = TargetsMetadata.FromString(
                bundle.targets_metadata['targets'].serialized_targets_metadata
            )
            self.assertEqual(targets_metadata.common_metadata.version, 42)
            self.assertEqual(
                serialized_root_metadata_bytes,
                bundle.root_metadata.serialized_root_metadata,
            )

    def test_persist_to_disk(self):
        """Tests persisting the TUF repo to disk for debugging"""
        with tempfile.TemporaryDirectory() as tempdir_name:
            temp_root = Path(tempdir_name)
            foo_path = temp_root / 'foo.bin'
            bar_path = temp_root / 'bar.bin'
            baz_path = temp_root / 'baz.bin'
            qux_path = temp_root / 'subdir' / 'qux.exe'
            foo_bytes = b'\xf0\x0b\xa4'
            bar_bytes = b'\x0b\xa4\x99'
            baz_bytes = b'\xba\x59\x06'
            qux_bytes = b'\x8a\xf3\x12'
            foo_path.write_bytes(foo_bytes)
            bar_path.write_bytes(bar_bytes)
            baz_path.write_bytes(baz_bytes)
            (temp_root / 'subdir').mkdir()
            qux_path.write_bytes(qux_bytes)
            targets = {
                foo_path: 'foo',
                bar_path: 'bar',
                baz_path: 'baz',
                qux_path: 'subdir/qux',
            }
            persist_path = temp_root / 'persisted'

            update_bundle.gen_unsigned_update_bundle(
                targets, persist=persist_path
            )

            self.assertEqual(foo_bytes, (persist_path / 'foo').read_bytes())
            self.assertEqual(bar_bytes, (persist_path / 'bar').read_bytes())
            self.assertEqual(baz_bytes, (persist_path / 'baz').read_bytes())
            self.assertEqual(
                qux_bytes, (persist_path / 'subdir' / 'qux').read_bytes()
            )


class ParseTargetArgTest(unittest.TestCase):
    """Test the parsing of target argument strings."""

    def test_valid_arg(self):
        """Checks that valid remap strings are parsed correctly."""
        file_path, target_name = update_bundle.parse_target_arg(
            'foo.bin > main'
        )

        self.assertEqual(Path('foo.bin'), file_path)
        self.assertEqual('main', target_name)

    def test_invalid_arg_raises(self):
        """Checks that invalid remap string raise an error."""
        with self.assertRaises(ValueError):
            update_bundle.parse_target_arg('foo.bin main')


if __name__ == '__main__':
    unittest.main()
