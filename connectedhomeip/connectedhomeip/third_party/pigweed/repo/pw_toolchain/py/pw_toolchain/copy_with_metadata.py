#!/usr/bin/env python3
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
"""Emulation of `cp -af src dest`."""

import logging
import os
import shutil
import sys

_LOG = logging.getLogger(__name__)


def copy_with_metadata(src, dest):
    """Emulation of `cp -af in out` command."""
    if not os.path.exists(src):
        _LOG.error('No such file or directory.')
        return -1

    try:
        if os.path.isdir(src):
            shutil.copytree(src, dest, symlinks=True)
        else:
            shutil.copy2(src, dest, follow_symlinks=False)
    except:  # pylint: disable=bare-except
        _LOG.exception('Error during copying procedure.')
        return -1

    return 0


def main():
    # Require exactly two arguments, source and destination.
    if (len(sys.argv) - 1) != 2:
        _LOG.error('Incorrect parameters provided.')
        return -1

    return copy_with_metadata(sys.argv[1], sys.argv[2])


if __name__ == '__main__':
    sys.exit(main())
