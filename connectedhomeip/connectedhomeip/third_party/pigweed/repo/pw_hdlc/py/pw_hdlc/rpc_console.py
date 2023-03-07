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
"""Console for interacting with pw_rpc over HDLC.

This command is no longer supported. Please run pw_system.console instead.

  python -m pw_system.console --device /dev/ttyUSB0 --proto-globs sample.proto
"""

import sys

# TODO(tonymd): Delete this when no longer needed.
PW_RPC_MAX_PACKET_SIZE = 256


def main() -> int:
    print(__doc__)
    return 1


if __name__ == '__main__':
    sys.exit(main())
