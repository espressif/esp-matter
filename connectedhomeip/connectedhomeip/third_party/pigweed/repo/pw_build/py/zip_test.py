# Copyright 2020 The Pigweed Authors
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
"""Tests for the pw_build.zip module."""

import unittest
import os
import tempfile
import pathlib
import zipfile

from pw_build.zip import zip_up, ZipError

DELIMITER = '>'
IN_FILENAMES = [
    'file1.txt',
    'file2.txt',
    'dir1/file3.txt',
    'dir1/file4.txt',
    'dir1/dir2/file5.txt',
    'dir1/dir2/file6.txt',
]


def make_directory(parent_path: pathlib.Path, dir_name: str, filenames: list):
    """Creates a directory and returns a pathlib.Path() of it's root dir.

    Args:
        parent_path: Path to directory where the new directory will be made.
        dir_name: Name of the new directory.
        filenames: list of file contents of the new directory. Also allows
            the creation of subdirectories. Example:
            [
                'file1.txt',
                'subdir/file2.txt'
            ]

    Returns: pathlib.Path() to the newly created directory.
    """
    root_path = pathlib.Path(parent_path / dir_name)
    os.mkdir(root_path)
    for filename in filenames:
        # Make the sub directories if they don't already exist.
        directories = filename.split('/')[:-1]
        for i in range(len(directories)):
            directory = pathlib.PurePath('/'.join(directories[: i + 1]))
            if not (root_path / directory).is_dir():
                os.mkdir(root_path / directory)

        # Create a file at the destination.
        touch(root_path, filename)
    return root_path


def touch(parent_dir: pathlib.Path, filename: str):
    """Creates an empty file at parent_dir/filename."""
    with open(parent_dir / filename, 'a') as touch_file:
        touch_file.write(filename)


def get_directory_contents(path: pathlib.Path):
    """Iterates through a directory and returns a set of its contents."""
    contents = set()
    for filename in path.glob('**/*'):
        # Remove the original parent directories to get just the relative path.
        contents.add(filename.relative_to(path))
    return contents


class TestZipping(unittest.TestCase):
    """Tests for the pw_build.zip module."""

    def test_zip_up_file(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            tmp_path = pathlib.Path(tmp_dir)
            in_path = make_directory(tmp_path, 'in', IN_FILENAMES)
            input_list = [f'{in_path}/file1.txt {DELIMITER} /']
            out_filename = f'{tmp_path}/out.zip'

            # Act.
            zip_up(input_list, out_filename)
            out_path = pathlib.Path(f'{tmp_path}/out/')
            with zipfile.ZipFile(out_filename, 'r') as zip_file:
                zip_file.extractall(out_path)
            expected_path = make_directory(tmp_path, 'expected', ['file1.txt'])

            # Assert.
            self.assertSetEqual(
                get_directory_contents(out_path),
                get_directory_contents(expected_path),
            )

    def test_zip_up_dir(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            tmp_path = pathlib.Path(tmp_dir)
            in_path = make_directory(tmp_path, 'in', IN_FILENAMES)
            input_list = [f'{in_path}/dir1/ {DELIMITER} /']
            out_filename = f'{tmp_path}/out.zip'

            # Act.
            zip_up(input_list, out_filename)
            out_path = pathlib.Path(f'{tmp_path}/out/')
            with zipfile.ZipFile(out_filename, 'r') as zip_file:
                zip_file.extractall(out_path)
            expected_path = make_directory(
                tmp_path,
                'expected',
                [
                    'file3.txt',
                    'file4.txt',
                    'dir2/file5.txt',
                    'dir2/file6.txt',
                ],
            )

            # Assert.
            self.assertSetEqual(
                get_directory_contents(out_path),
                get_directory_contents(expected_path),
            )

    def test_file_rename(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            tmp_path = pathlib.Path(tmp_dir)
            in_path = make_directory(tmp_path, 'in', IN_FILENAMES)
            input_list = [f'{in_path}/file1.txt {DELIMITER} /renamed.txt']
            out_filename = f'{tmp_path}/out.zip'

            # Act.
            zip_up(input_list, out_filename)
            out_path = pathlib.Path(f'{tmp_path}/out/')
            with zipfile.ZipFile(out_filename, 'r') as zip_file:
                zip_file.extractall(out_path)
            expected_path = make_directory(
                tmp_path, 'expected', ['renamed.txt']
            )

            # Assert.
            self.assertSetEqual(
                get_directory_contents(out_path),
                get_directory_contents(expected_path),
            )

    def test_file_move(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            tmp_path = pathlib.Path(tmp_dir)
            in_path = make_directory(tmp_path, 'in', IN_FILENAMES)
            input_list = [f'{in_path}/file1.txt {DELIMITER} /foo/']
            out_filename = f'{tmp_path}/out.zip'

            # Act.
            zip_up(input_list, out_filename)
            out_path = pathlib.Path(f'{tmp_path}/out/')
            with zipfile.ZipFile(out_filename, 'r') as zip_file:
                zip_file.extractall(out_path)
            expected_path = make_directory(
                tmp_path, 'expected', ['foo/file1.txt']
            )

            # Assert.
            self.assertSetEqual(
                get_directory_contents(out_path),
                get_directory_contents(expected_path),
            )

    def test_dir_move(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            tmp_path = pathlib.Path(tmp_dir)
            in_path = make_directory(tmp_path, 'in', IN_FILENAMES)
            input_list = [f'{in_path}/dir1/ {DELIMITER} /foo/']
            out_filename = f'{tmp_path}/out.zip'

            # Act.
            zip_up(input_list, out_filename)
            out_path = pathlib.Path(f'{tmp_path}/out/')
            with zipfile.ZipFile(out_filename, 'r') as zip_file:
                zip_file.extractall(out_path)
            expected_path = make_directory(
                tmp_path,
                'expected',
                [
                    'foo/file3.txt',
                    'foo/file4.txt',
                    'foo/dir2/file5.txt',
                    'foo/dir2/file6.txt',
                ],
            )

            # Assert.
            self.assertSetEqual(
                get_directory_contents(out_path),
                get_directory_contents(expected_path),
            )

    def test_change_delimiter(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            tmp_path = pathlib.Path(tmp_dir)
            in_path = make_directory(tmp_path, 'in', IN_FILENAMES)
            delimiter = '==>'
            input_list = [f'{in_path}/file1.txt {delimiter} /']
            out_filename = f'{tmp_path}/out.zip'

            # Act.
            zip_up(input_list, out_filename, delimiter=delimiter)
            out_path = pathlib.Path(f'{tmp_path}/out/')
            with zipfile.ZipFile(out_filename, 'r') as zip_file:
                zip_file.extractall(out_path)
            expected_path = make_directory(tmp_path, 'expected', ['file1.txt'])

            # Assert.
            self.assertSetEqual(
                get_directory_contents(out_path),
                get_directory_contents(expected_path),
            )

    def test_wrong_input_syntax_raises_error(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            bad_inputs = [
                '',  # Empty input
                f'{tmp_dir}/ /',  # No delimiter
                f'{tmp_dir}/ {DELIMITER} ',  # No zip destination
                f'{tmp_dir} /',  # No source
                f'{tmp_dir}/',  # No delimiter or zip destination
                f'{DELIMITER}',  # No source or zip destination
                f'{tmp_dir} {DELIMITER} /',  # No trailing source '/'
                f'{tmp_dir}/ {DELIMITER} foo/',  # No leading zip root '/'
                f'{tmp_dir}/ {DELIMITER} /foo',  # No trailing zip dest '/'
                f'{tmp_dir}/ {DELIMITER} /{tmp_dir}/ '
                f'{DELIMITER} /{tmp_dir}/',  # Too many paths on split
            ]
            out_filename = f'{tmp_dir}/out.zip'

            # Act & Assert.
            for bad_input in bad_inputs:
                with self.assertRaises(ZipError):
                    zip_up([bad_input], out_filename)

    def test_nonexistant_file_raises_error(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            input_list = [f'{tmp_dir}/nonexistant-file.txt > /']
            out_filename = f'{tmp_dir}/out.zip'

            # Act & Assert.
            with self.assertRaises(ZipError):
                zip_up(input_list, out_filename)

    def test_nonexistant_dir_raises_error(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            # Arrange.
            input_list = [f'{tmp_dir}/nonexistant-dir/ > /']
            out_filename = f'{tmp_dir}/out.zip'

            # Act & Assert.
            with self.assertRaises(ZipError):
                zip_up(input_list, out_filename)


if __name__ == '__main__':
    unittest.main()
