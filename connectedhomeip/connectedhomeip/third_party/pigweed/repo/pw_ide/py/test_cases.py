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
"""pw_ide test classes."""

from contextlib import contextmanager
from io import TextIOWrapper
from pathlib import Path
import tempfile
from typing import Generator, List, Optional, Tuple, Union
import unittest

from pw_ide.settings import PigweedIdeSettings


class TempDirTestCase(unittest.TestCase):
    """Run tests that need access to a temporary directory."""

    def setUp(self) -> None:
        self.temp_dir = tempfile.TemporaryDirectory()
        self.temp_dir_path = Path(self.temp_dir.name)

    def tearDown(self) -> None:
        self.temp_dir.cleanup()
        return super().tearDown()

    @contextmanager
    def make_temp_file(
        self, filename: Union[Path, str], content: str = ''
    ) -> Generator[Tuple[TextIOWrapper, Path], None, None]:
        """Create a temp file in the test case's temp dir.

        Returns a tuple containing the file reference and the file's path.
        The file can be read immediately.
        """
        path = self.temp_dir_path / filename

        with open(path, 'a+', encoding='utf-8') as file:
            file.write(content)
            file.flush()
            file.seek(0)
            yield (file, path)

    def touch_temp_file(
        self, filename: Union[Path, str], content: str = ''
    ) -> None:
        """Create a temp file in the test case's temp dir, without context."""
        with self.make_temp_file(filename, content):
            pass

    @contextmanager
    def open_temp_file(
        self,
        filename: Union[Path, str],
    ) -> Generator[Tuple[TextIOWrapper, Path], None, None]:
        """Open an existing temp file in the test case's temp dir.

        Returns a tuple containing the file reference and the file's path.
        """
        path = self.temp_dir_path / filename

        with open(path, 'r', encoding='utf-8') as file:
            yield (file, path)

    @contextmanager
    def make_temp_files(
        self, files_data: List[Tuple[Union[Path, str], str]]
    ) -> Generator[List[TextIOWrapper], None, None]:
        """Create several temp files in the test case's temp dir.

        Provide a list of file name and content tuples. Saves you the trouble
        of excessive `with self.make_temp_file, self.make_temp_file...`
        nesting, and allows programmatic definition of multiple temp file
        contexts. Files can be read immediately.
        """
        files: List[TextIOWrapper] = []

        for filename, content in files_data:
            file = open(self.path_in_temp_dir(filename), 'a+', encoding='utf-8')
            file.write(content)
            file.flush()
            file.seek(0)
            files.append(file)

        yield files

        for file in files:
            file.close()

    def touch_temp_files(
        self, files_data: List[Tuple[Union[Path, str], str]]
    ) -> None:
        """Create several temp files in the temp dir, without context."""
        with self.make_temp_files(files_data):
            pass

    @contextmanager
    def open_temp_files(
        self, files_data: List[Union[Path, str]]
    ) -> Generator[List[TextIOWrapper], None, None]:
        """Open several existing temp files in the test case's temp dir.

        Provide a list of file names. Saves you the trouble of excessive
        `with self.open_temp_file, self.open_temp_file...` nesting, and allows
        programmatic definition of multiple temp file contexts.
        """
        files: List[TextIOWrapper] = []

        for filename in files_data:
            file = open(self.path_in_temp_dir(filename), 'r', encoding='utf-8')
            files.append(file)

        yield files

        for file in files:
            file.close()

    def path_in_temp_dir(self, path: Union[Path, str]) -> Path:
        """Place a path into the test case's temp dir.

        This only works with a relative path; with an absolute path, this is a
        no-op.
        """
        return self.temp_dir_path / path

    def paths_in_temp_dir(self, *paths: Union[Path, str]) -> List[Path]:
        """Place several paths into the test case's temp dir.

        This only works with relative paths; with absolute paths, this is a
        no-op.
        """
        return [self.path_in_temp_dir(path) for path in paths]


class PwIdeTestCase(TempDirTestCase):
    """A test case for testing `pw_ide`.

    Provides a temp dir for testing file system actions and access to IDE
    settings that wrap the temp dir.
    """

    def make_ide_settings(
        self,
        working_dir: Optional[Union[str, Path]] = None,
        targets: Optional[List[str]] = None,
    ) -> PigweedIdeSettings:
        """Make settings that wrap provided paths in the temp path."""

        if working_dir is not None:
            working_dir_path = self.path_in_temp_dir(working_dir)
        else:
            working_dir_path = self.temp_dir_path

        if targets is None:
            targets = []

        return PigweedIdeSettings(
            False,
            False,
            False,
            default_config={
                'working_dir': str(working_dir_path),
                'targets': targets,
            },
        )
