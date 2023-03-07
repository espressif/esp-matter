#!/usr/bin/env python3
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
"""Tests for todo_check."""

from pathlib import Path
import re
import unittest
from unittest.mock import MagicMock, mock_open, patch

from pw_presubmit import todo_check

# pylint: disable=attribute-defined-outside-init
# todo-check: disable


class TestTodoCheck(unittest.TestCase):
    """Test TODO checker."""

    def _run(self, regex: re.Pattern, contents: str) -> None:
        self.ctx = MagicMock()
        self.ctx.fail = MagicMock()
        path = MagicMock(spec=Path('foo/bar'))

        def mocked_open_read(*args, **kwargs):
            return mock_open(read_data=contents)(*args, **kwargs)

        with patch.object(path, 'open', mocked_open_read):
            # pylint: disable=protected-access
            todo_check._process_file(self.ctx, regex, path)

            # pylint: enable=protected-access

    def _run_bugs_users(self, contents: str) -> None:
        self._run(todo_check.BUGS_OR_USERNAMES, contents)

    def _run_bugs(self, contents: str) -> None:
        self._run(todo_check.BUGS_ONLY, contents)

    def test_one_bug(self) -> None:
        contents = 'TODO(b/123): foo\n'
        self._run_bugs_users(contents)
        self.ctx.fail.assert_not_called()
        self._run_bugs(contents)
        self.ctx.fail.assert_not_called()

    def test_two_bugs(self) -> None:
        contents = 'TODO(b/123, b/456): foo\n'
        self._run_bugs_users(contents)
        self.ctx.fail.assert_not_called()
        self._run_bugs(contents)
        self.ctx.fail.assert_not_called()

    def test_three_bugs(self) -> None:
        contents = 'TODO(b/123,b/456,b/789): foo\n'
        self._run_bugs_users(contents)
        self.ctx.fail.assert_not_called()
        self._run_bugs(contents)
        self.ctx.fail.assert_not_called()

    def test_one_username(self) -> None:
        self._run_bugs_users('TODO(usera): foo\n')
        self.ctx.fail.assert_not_called()

    def test_two_usernames(self) -> None:
        self._run_bugs_users('TODO(usera, userb): foo\n')
        self.ctx.fail.assert_not_called()

    def test_three_usernames(self) -> None:
        self._run_bugs_users('TODO(usera,userb,userc): foo\n')
        self.ctx.fail.assert_not_called()

    def test_username_not_allowed(self) -> None:
        self._run_bugs('TODO(usera): foo\n')
        self.ctx.fail.assert_called()

    def test_space_after_todo_bugsonly(self) -> None:
        self._run_bugs('TODO (b/123): foo\n')
        self.ctx.fail.assert_called()

    def test_space_after_todo_bugsusers(self) -> None:
        self._run_bugs_users('TODO (b/123): foo\n')
        self.ctx.fail.assert_called()

    def test_space_before_bug_bugsonly(self) -> None:
        self._run_bugs('TODO( b/123): foo\n')
        self.ctx.fail.assert_called()

    def test_space_before_bug_bugsusers(self) -> None:
        self._run_bugs_users('TODO( b/123): foo\n')
        self.ctx.fail.assert_called()

    def test_space_after_bug_bugsonly(self) -> None:
        self._run_bugs('TODO(b/123 ): foo\n')
        self.ctx.fail.assert_called()

    def test_space_after_bug_bugsusers(self) -> None:
        self._run_bugs_users('TODO(b/123 ): foo\n')
        self.ctx.fail.assert_called()

    def test_missing_explanation_bugsonly(self) -> None:
        self._run_bugs('TODO(b/123)\n')
        self.ctx.fail.assert_called()

    def test_missing_explanation_bugsusers(self) -> None:
        self._run_bugs_users('TODO(b/123)\n')
        self.ctx.fail.assert_called()

    def test_not_a_bug_bugsonly(self) -> None:
        self._run_bugs('TODO(cl/123): foo\n')
        self.ctx.fail.assert_called()

    def test_not_a_bug_bugsusers(self) -> None:
        self._run_bugs_users('TODO(cl/123): foo\n')
        self.ctx.fail.assert_called()

    def test_but_not_bug_bugsonly(self) -> None:
        self._run_bugs('TODO(b/123, cl/123): foo\n')
        self.ctx.fail.assert_called()

    def test_bug_not_bug_bugsusers(self) -> None:
        self._run_bugs_users('TODO(b/123, cl/123): foo\n')
        self.ctx.fail.assert_called()

    def test_empty_bugsonly(self) -> None:
        self._run_bugs('TODO(): foo\n')
        self.ctx.fail.assert_called()

    def test_empty_bugsusers(self) -> None:
        self._run_bugs_users('TODO(): foo\n')
        self.ctx.fail.assert_called()

    def test_bare_bugsonly(self) -> None:
        self._run_bugs('TODO: foo\n')
        self.ctx.fail.assert_called()

    def test_bare_bugsusers(self) -> None:
        self._run_bugs_users('TODO: foo\n')
        self.ctx.fail.assert_called()


if __name__ == '__main__':
    unittest.main()

# todo-check: enable
