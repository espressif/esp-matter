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
#
"""Unit tests for owners_checks.py."""
from pathlib import Path
import tempfile
from typing import Iterable, Sequence, Tuple
import unittest
from unittest import mock
from pw_presubmit import owners_checks

# ===== Test data =====

bad_duplicate = """\
# Should raise OwnersDuplicateError.
set noparent

file:/foo/OWNERZ
file:../OWNERS
file:../OWNERS
test1@example.com
#Test 2 comment
test2@example.com
"""

bad_duplicate_user = """\
# Should raise OwnersDuplicateError.
set noparent

file:/foo/OWNERZ
file:../OWNERS

*
test1@example.com
#Test 2 comment
test2@example.com
                        test1@example.com
"""

bad_duplicate_wildcard = """\
# Should raise OwnersDuplicateError.
set noparent
*
file:/foo/OWNERZ
file:../OWNERS
test1@example.com
#Test 2 comment
test2@example.com
*
"""

bad_email = """\
# Should raise OwnersInvalidLineError.
set noparent
*
file:/foo/OWNERZ
file:../OWNERS
test1example.com
#Test 2 comment
test2@example.com
*
"""
bad_grant_combo = """\
# Should raise OwnersUserGrantError.

file:/foo/OWNERZ
file:../OWNERS

test1@example.com
#Test noparent comment
set noparent
test2@example.com

*
"""

bad_ordering1 = """\
# Tests formatter reorders groupings of lines into the right order.
file:/foo/OWNERZ
file:bar/OWNERZ

test1@example.com
#Test noparent comment
set noparent
test2@example.com
"""

bad_ordering1_fixed = """\
#Test noparent comment
set noparent

# Tests formatter reorders groupings of lines into the right order.
file:/foo/OWNERZ
file:bar/OWNERZ

test1@example.com
test2@example.com
"""

bad_prohibited1 = """\
# Should raise OwnersProhibitedError.
set noparent

file:/foo/OWNERZ
file:../OWNERS

test1@example.com
#Test 2 comment
test2@example.com

include file1.txt

per-file foo.txt=test3@example.com
"""

bad_moving_comments = """\
# Test comments move with the rule that follows them.
test2@example.com
test1@example.com

# foo comment
file:/foo/OWNERZ
# .. comment
file:../OWNERS

set noparent
"""
bad_moving_comments_fixed = """\
set noparent

# .. comment
file:../OWNERS
# foo comment
file:/foo/OWNERZ

test1@example.com
# Test comments move with the rule that follows them.
test2@example.com
"""

bad_whitespace = """\
 set   noparent



file:/foo/OWNERZ

  file:../OWNERS

test1@example.com
#Test 2    comment
    test2@example.com

"""

bad_whitespace_fixed = """\
set noparent

file:../OWNERS
file:/foo/OWNERZ

test1@example.com
#Test 2    comment
test2@example.com
"""

no_dependencies = """\
# Test no imports are found when there are none.
set noparent

test1@example.com
#Test 2 comment
test2@example.com
"""

has_dependencies_file = """\
# Test if owners checks examine file: imports.
set noparent

file:foo_owners
file:bar_owners

test1@example.com
#Test 2 comment
test2@example.com
"""

has_dependencies_perfile = """\
# Test if owners checks examine per-file imports.
set noparent

test1@example.com
#Test 2 comment
test2@example.com

per-file *.txt=file:foo_owners
per-file *.md=example.google.com
"""

has_dependencies_include = """\
# Test if owners checks examine per-file imports.
set noparent

test1@example.com
#Test 2 comment
test2@example.com

per-file *.txt=file:foo_owners
per-file *.md=example.google.com
"""

dependencies_paths_relative = """\
set noparent

include foo/bar/../include_owners

file:foo/bar/../file_owners

*

per-file *.txt=file:foo/bar/../perfile_owners
"""

dependencies_paths_absolute = """\
set noparent

include /test/include_owners

file:/test/file_owners

*

per-file *.txt=file:/test/perfile_owners
"""

good1 = """\
# Checks should fine this formatted correctly
set noparent

include good1_include

file:good1_file

test1@example.com
#Test 2 comment
test2@example.com #{LAST_RESORT_SUGGESTION}
# LAST LINE
"""

good1_include = """\
test1@example.comom
"""

good1_file = """\
# Checks should fine this formatted correctly.
test1@example.com
"""

foo_owners = """\
test1@example.com
"""

bar_owners = """\
test1@example.com
"""

BAD_TEST_FILES = (
    ("bad_duplicate", owners_checks.OwnersDuplicateError),
    ("bad_duplicate_user", owners_checks.OwnersDuplicateError),
    ("bad_duplicate_wildcard", owners_checks.OwnersDuplicateError),
    ("bad_email", owners_checks.OwnersInvalidLineError),
    ("bad_grant_combo", owners_checks.OwnersUserGrantError),
    ("bad_ordering1", owners_checks.OwnersStyleError),
    ("bad_prohibited1", owners_checks.OwnersProhibitedError),
)

STYLING_CHECKS = (
    ("bad_moving_comments", "bad_moving_comments_fixed"),
    ("bad_ordering1", "bad_ordering1_fixed"),
    ("bad_whitespace", "bad_whitespace_fixed"),
)

DEPENDENCY_TEST_CASES: Iterable[Tuple[str, Iterable[str]]] = (
    ("no_dependencies", tuple()),
    ("has_dependencies_file", ("foo_owners", "bar_owners")),
    ("has_dependencies_perfile", ("foo_owners",)),
    ("has_dependencies_include", ("foo_owners",)),
)

DEPENDENCY_PATH_TEST_CASES: Iterable[str] = (
    "dependencies_paths_relative",
    "dependencies_paths_absolute",
)

GOOD_TEST_CASES = (("good1", "good1_include", "good1_file"),)


# ===== Unit Tests =====
class TestOwnersChecks(unittest.TestCase):
    """Unittest class for owners_checks.py."""

    maxDiff = 2000

    @staticmethod
    def _create_temp_files(
        temp_dir: str, file_list: Sequence[Tuple[str, str]]
    ) -> Sequence[Path]:
        real_files = []
        temp_dir_path = Path(temp_dir)
        for name, contents in file_list:
            file_path = temp_dir_path / name
            file_path.write_text(contents)
            real_files.append(file_path)
        return real_files

    def test_bad_files(self):
        # First test_file is the "primary" owners file followed by any needed
        # "secondary" owners.
        for test_file, expected_exception in BAD_TEST_FILES:
            with self.subTest(
                i=test_file
            ), tempfile.TemporaryDirectory() as temp_dir, self.assertRaises(
                expected_exception
            ):
                file_contents = globals()[test_file]
                primary_file = self._create_temp_files(
                    temp_dir=temp_dir, file_list=((test_file, file_contents),)
                )[0]
                owners_file = owners_checks.OwnersFile(primary_file)
                owners_file.look_for_owners_errors()
                owners_file.check_style()

    def test_good(self):
        # First test_file is the "primary" owners file followed by any needed
        # "secondary" owners.
        for test_files in GOOD_TEST_CASES:
            with self.subTest(
                i=test_files[0]
            ), tempfile.TemporaryDirectory() as temp_dir:
                files = [
                    (file_name, globals()[file_name])
                    for file_name in test_files
                ]
                primary_file = self._create_temp_files(
                    temp_dir=temp_dir, file_list=files
                )[0]
                self.assertDictEqual(
                    {}, owners_checks.run_owners_checks(primary_file)
                )

    def test_style_proposals(self):
        for unstyled_file, styled_file in STYLING_CHECKS:
            with self.subTest(
                i=unstyled_file
            ), tempfile.TemporaryDirectory() as temp_dir:
                unstyled_contents = globals()[unstyled_file]
                styled_contents = globals()[styled_file]
                unstyled_real_file = self._create_temp_files(
                    temp_dir=temp_dir,
                    file_list=((unstyled_file, unstyled_contents),),
                )[0]
                owners_file = owners_checks.OwnersFile(unstyled_real_file)
                formatted_content = "\n".join(owners_file.formatted_lines)
                self.assertEqual(styled_contents, formatted_content)

    def test_dependency_discovery(self):
        for file_under_test, expected_deps in DEPENDENCY_TEST_CASES:
            # During test make the test file directory the "git root"
            with tempfile.TemporaryDirectory() as temp_dir:
                temp_dir_path = Path(temp_dir).resolve()
                with self.subTest(i=file_under_test), mock.patch(
                    "pw_presubmit.owners_checks.git_repo.root",
                    return_value=temp_dir_path,
                ):
                    primary_file = (file_under_test, globals()[file_under_test])
                    deps_files = tuple(
                        (dep, (globals()[dep])) for dep in expected_deps
                    )

                    primary_file = self._create_temp_files(
                        temp_dir=temp_dir, file_list=(primary_file,)
                    )[0]
                    dep_files = self._create_temp_files(
                        temp_dir=temp_dir, file_list=deps_files
                    )

                    owners_file = owners_checks.OwnersFile(primary_file)

                    # get_dependencies is expected to resolve() files
                    found_deps = owners_file.get_dependencies()
                    expected_deps_path = [path.resolve() for path in dep_files]
                    expected_deps_path.sort()
                    found_deps.sort()
                    self.assertListEqual(expected_deps_path, found_deps)

    def test_dependency_path_creation(self):
        """Confirm paths care constructed for absolute and relative paths."""
        for file_under_test in DEPENDENCY_PATH_TEST_CASES:
            # During test make the test file directory the "git root"
            with tempfile.TemporaryDirectory() as temp_dir:
                temp_dir_path = Path(temp_dir).resolve()
                with self.subTest(i=file_under_test), mock.patch(
                    "pw_presubmit.owners_checks.git_repo.root",
                    return_value=temp_dir_path,
                ):
                    owners_file_path = (
                        temp_dir_path / "owners" / file_under_test
                    )
                    owners_file_path.parent.mkdir(parents=True)
                    owners_file_path.write_text(globals()[file_under_test])
                    owners_file = owners_checks.OwnersFile(owners_file_path)

                    # get_dependencies is expected to resolve() files
                    found_deps = owners_file.get_dependencies()

                    if "absolute" in file_under_test:
                        # Absolute paths start with the git/project root
                        expected_prefix = temp_dir_path / "test"
                    else:
                        # Relative paths start with owners file dir
                        expected_prefix = owners_file_path.parent / "foo"

                    expected_deps_path = [
                        (expected_prefix / filename).resolve()
                        for filename in (
                            "include_owners",
                            "file_owners",
                            "perfile_owners",
                        )
                    ]
                    expected_deps_path.sort()
                    found_deps.sort()
                    self.assertListEqual(expected_deps_path, found_deps)


if __name__ == "__main__":
    unittest.main()
