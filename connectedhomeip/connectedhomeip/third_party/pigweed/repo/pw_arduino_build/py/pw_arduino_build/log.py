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
"""Configure the system logger for the default pw command log format."""

import logging

_LOG = logging.getLogger(__name__)
_STDERR_HANDLER = logging.StreamHandler()


def install(level: int = logging.INFO) -> None:
    """Configure the system logger for the arduino_builder log format."""

    try:
        import pw_cli.log  # pylint: disable=import-outside-toplevel

        pw_cli.log.install(level=level)
    except ImportError:
        # Set log level on root logger to debug, otherwise any higher levels
        # elsewhere are ignored.
        root = logging.getLogger()
        root.setLevel(logging.DEBUG)

        _STDERR_HANDLER.setLevel(level)
        _STDERR_HANDLER.setFormatter(
            logging.Formatter(
                "[%(asctime)s] " "%(levelname)s %(message)s", "%Y%m%d %H:%M:%S"
            )
        )
        root.addHandler(_STDERR_HANDLER)


def set_level(log_level: int):
    """Sets the log level for logs to stderr."""
    _STDERR_HANDLER.setLevel(log_level)
