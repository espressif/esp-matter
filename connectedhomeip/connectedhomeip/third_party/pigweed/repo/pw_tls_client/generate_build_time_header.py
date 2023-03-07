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
"""Generate a header file for UTC time"""

import argparse
from datetime import datetime
import sys

HEADER = """// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <stdint.h>

"""


def parse_args() -> None:
    """Setup argparse."""
    parser = argparse.ArgumentParser()
    parser.add_argument("out", help="path for output header file")
    return parser.parse_args()


def main() -> int:
    """Main function"""
    args = parse_args()
    time_stamp = int(datetime.now().timestamp())
    print(time_stamp)
    with open(args.out, "w") as header:
        print(args.out)
        header.write(HEADER)

        # Add a comment in the generated header to show readable build time
        string_date = datetime.fromtimestamp(time_stamp).strftime(
            "%m/%d/%Y %H:%M:%S"
        )
        header.write(f'// {string_date}\n')

        # Write to the header.
        header.write(
            ''.join(
                [
                    'constexpr uint64_t kBuildTimeMicrosecondsUTC = ',
                    f'{int(time_stamp * 1e6)};\n',
                ]
            )
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
