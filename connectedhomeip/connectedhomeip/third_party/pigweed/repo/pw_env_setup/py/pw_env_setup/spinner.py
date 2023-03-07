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
"""Spinner!"""

import contextlib
import os
import sys
import threading
import time


class Spinner(object):  # pylint: disable=useless-object-inheritance
    """Spinner!"""

    def __init__(self, quiet=False):
        self._done = None
        self._thread = None
        self._quiet = quiet

    def _disabled(self):
        if os.environ.get('PW_ENVSETUP_DISABLE_SPINNER'):
            return True
        if os.environ.get('PW_ENVSETUP_QUIET'):
            return True
        if self._quiet:
            return True
        if not sys.stdout.isatty():
            return True
        return False

    def __del__(self):
        self._done = True

    def _spin(self):
        i = 0
        chars = '|/-\\'
        while not self._done:
            sys.stdout.write('[{}]'.format(chars[i]))
            sys.stdout.flush()
            time.sleep(0.1)
            sys.stdout.write('\b\b\b')
            i = (i + 1) % len(chars)

    def start(self):
        if self._disabled():
            return

        self._done = False
        self._thread = threading.Thread(target=self._spin)
        self._thread.start()

    def stop(self):
        if self._disabled():
            return

        assert self._thread
        self._done = True
        self._thread.join()
        self._thread = None

    @contextlib.contextmanager
    def __call__(self):
        try:
            self.start()
            yield self
        finally:
            self.stop()

    @contextlib.contextmanager
    def pause(self):
        try:
            self.stop()
            yield self
        finally:
            self.start()
