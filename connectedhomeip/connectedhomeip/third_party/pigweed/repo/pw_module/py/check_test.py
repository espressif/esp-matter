# Copyright 2019 The Pigweed Authors
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
"""Tests for pw_module.check."""

import logging
import pathlib
import shutil
import tempfile
import unittest

import pw_module.check

_LOG = logging.getLogger(__name__)


class TestWithTempDirectory(unittest.TestCase):
    """Tests for pw_module.check."""

    def setUp(self):
        # Create a temporary directory for the test.
        self.test_dir = tempfile.mkdtemp()

    def tearDown(self):
        # Remove it after the test.
        shutil.rmtree(self.test_dir)

    def create_file(self, path, contents=''):
        """Create a file and any directories assuming '/' path separator"""
        full_file_path = pathlib.Path(self.test_dir, path)
        if full_file_path.exists():
            raise Exception(f'File exists already: {path}')

        # Make parent directories if they don't exsit.
        full_file_path.parent.mkdir(parents=True, exist_ok=True)

        with open(full_file_path, 'w') as fd:
            fd.write(contents)

        return full_file_path

    def assert_no_issues(self, checker, directory=None):
        if directory is not None:
            directory = str(pathlib.Path(self.test_dir, directory))
        else:
            directory = self.test_dir
        return self.assertFalse(list(checker(directory)))

    def assert_issue(self, checker, match, directory=None):
        if directory is not None:
            directory = str(pathlib.Path(self.test_dir, directory))
        else:
            directory = self.test_dir
        issues = list(checker(directory))
        self.assertTrue(any((match in issue.message) for issue in issues))

    # Have Python code --> have setup.py.
    def test_pwck001_have_setup_py(self):
        # Python files; no setup --> error.
        self.create_file('pw_foo/py/pw_foo/__init__.py')
        self.create_file('pw_foo/py/pw_foo/bar.py')
        self.assert_issue(
            pw_module.check.check_python_proper_module, 'setup.py'
        )

        # Python files; have setup.py --> ok.
        self.create_file('pw_foo/py/setup.py')
        self.assert_no_issues(pw_module.check.check_python_proper_module)

    # Have C++ code --> have C++ tests.
    def test_pwck002_have_python_tests(self):
        self.create_file('pw_foo/public/foo.h')
        self.create_file('pw_foo/foo.cc')
        self.assert_issue(pw_module.check.check_have_cc_tests, 'tests')

        self.create_file('pw_foo/foo_test.cc')
        self.assert_no_issues(pw_module.check.check_have_cc_tests)

    # Have Python code --> have Python tests.
    def test_pwck003_have_python_tests(self):
        self.create_file('pw_foo/py/pw_foo/__init__.py')
        self.create_file('pw_foo/py/setup.py')
        self.assert_issue(pw_module.check.check_have_python_tests, 'tests')

        self.create_file('pw_foo/py/foo_test.py')
        self.assert_no_issues(pw_module.check.check_have_python_tests)

    # Have README.md
    def test_pwck004_have_readme(self):
        self.assert_issue(pw_module.check.check_has_readme, 'README')
        self.create_file('README.md')
        self.assert_no_issues(pw_module.check.check_has_readme)

    # Have ReST docs of some kind
    def test_pwck005_have_rst_docs(self):
        checker = pw_module.check.check_has_rst_docs
        self.assert_issue(checker, 'ReST')
        self.create_file('pw_foo/docs.rst')
        self.assert_no_issues(checker)

    # Have ReST docs of some kind
    def test_pwck006_have_public_or_override_headers(self):
        checker = pw_module.check.check_has_public_or_override_headers
        module_name = 'pw_foo'

        # Only have a doc? Great.
        self.create_file('pw_foo/docs.rst')
        self.assert_no_issues(checker, directory=module_name)

        # CC files with no public header --> error.
        self.create_file('pw_foo/implementation.cc')
        self.create_file('pw_foo/implementation_test.cc')
        self.assert_issue(checker, 'public/pw_foo', directory=module_name)

        # CC files with public header in unmatched module folder --> error.
        bad_header = self.create_file('pw_foo/public/wrong/foo.h')
        self.assert_issue(checker, 'public/pw_foo', directory=module_name)

        # Remove the "bad" header.
        bad_header_parent = bad_header.parent
        bad_header.unlink()
        bad_header_parent.rmdir()

        # Finally create the correct header.
        self.create_file('pw_foo/public/pw_foo/baz.h')
        self.assert_no_issues(checker, directory=module_name)

        # Reject if there are multiple directories in public/...
        self.create_file('pw_foo/public/fake/fake.h')
        self.assert_issue(checker, 'multiple', directory=module_name)


if __name__ == '__main__':
    import sys

    logging.basicConfig(stream=sys.stderr, level=logging.DEBUG)
    unittest.main()
