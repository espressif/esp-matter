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
"""pw_ide exceptions."""


class UnsupportedPlatformException(Exception):
    """Raised when an action is attempted on an unsupported platform."""


class InvalidTargetException(Exception):
    """Exception for invalid compilation targets."""


class BadCompDbException(Exception):
    """Exception for compliation databases that don't conform to the format."""


class MissingCompDbException(Exception):
    """Exception for missing compilation database files."""


class UnresolvablePathException(Exception):
    """Raised when an ambiguous path cannot be resolved."""
