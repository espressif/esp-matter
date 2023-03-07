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
"""Tests for pw_ide.commands"""

import logging
import os
import unittest

from pw_ide.commands import _make_working_dir, LoggingStatusReporter
from pw_ide.settings import PW_IDE_DIR_NAME

from test_cases import PwIdeTestCase

_LOG = logging.getLogger(__package__)


class TestMakeWorkingDir(PwIdeTestCase):
    """Tests _make_working_dir"""

    def test_does_not_exist_creates_dir(self):
        settings = self.make_ide_settings(working_dir=PW_IDE_DIR_NAME)
        self.assertFalse(settings.working_dir.exists())
        _make_working_dir(
            reporter=LoggingStatusReporter(_LOG), settings=settings
        )
        self.assertTrue(settings.working_dir.exists())

    def test_does_exist_is_idempotent(self):
        settings = self.make_ide_settings(working_dir=PW_IDE_DIR_NAME)
        _make_working_dir(
            reporter=LoggingStatusReporter(_LOG), settings=settings
        )
        modified_when_1 = os.path.getmtime(settings.working_dir)
        _make_working_dir(
            reporter=LoggingStatusReporter(_LOG), settings=settings
        )
        modified_when_2 = os.path.getmtime(settings.working_dir)
        self.assertEqual(modified_when_1, modified_when_2)


if __name__ == '__main__':
    unittest.main()
