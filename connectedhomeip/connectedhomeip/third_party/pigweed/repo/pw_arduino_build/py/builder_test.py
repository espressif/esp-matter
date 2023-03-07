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
"""Tests for arduinobuilder."""

import shlex
import unittest
from parameterized import parameterized  # type: ignore


class TestShellArgumentSplitting(unittest.TestCase):
    """Tests to ensure shlex.split handles expected use cases."""

    @parameterized.expand(
        [
            (
                "remove-both-quotes",
                """ -DUSB_CONFIG_POWER=100 """
                """ '-DUSB_MANUFACTURER="Adafruit LLC"' """
                """ '-DUSB_PRODUCT="Adafruit PyGamer Advance M4"' """
                """ "-I$HOME/samd/1.6.2/cores/arduino/TinyUSB" """,
                [
                    """ -DUSB_CONFIG_POWER=100 """.strip(),
                    """ -DUSB_MANUFACTURER="Adafruit LLC" """.strip(),
                    """ -DUSB_PRODUCT="Adafruit PyGamer Advance M4" """.strip(),
                    """ -I$HOME/samd/1.6.2/cores/arduino/TinyUSB """.strip(),
                ],
            )
        ]
    )
    def test_split_arguments_and_remove_quotes(
        self, unused_test_name, input_string, expected
    ):
        """Test splitting a str into a list of arguments with quotes removed."""
        result = shlex.split(input_string)
        self.assertEqual(result, expected)


if __name__ == '__main__':
    unittest.main()
