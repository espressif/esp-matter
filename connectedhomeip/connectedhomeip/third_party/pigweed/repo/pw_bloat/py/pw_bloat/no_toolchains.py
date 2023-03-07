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
"""Outputs a ReST warning about the size toolchains variable being empty."""

import os
import sys

_NO_TOOLCHAIN_ERROR: str = '''
.. warning::

  The ``pw_size_report_toolchains`` build variable is empty for this target.
  Size reports will not be generated.

  See :ref:`bloat-howto` for details on how to set up size reports.
'''


def main() -> int:
    os.makedirs(os.path.dirname(sys.argv[1]), exist_ok=True)
    with open(sys.argv[1], 'w') as fd:
        fd.write(_NO_TOOLCHAIN_ERROR)
    return 0


if __name__ == '__main__':
    sys.exit(main())
