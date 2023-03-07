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
"""A simple tool to decode a CFSR register from the command line

Example usage:

  $ python -m pw_cpu_exception_cortex_m.cfsr_decoder 0x00010100

  20210412 15:09:01 INF Exception caused by a usage fault, bus fault.

  Active Crash Fault Status Register (CFSR) fields:
  IBUSERR     Bus fault on instruction fetch.
  UNDEFINSTR  Encountered invalid instruction.

  All registers:
  cfsr       0x00010100
"""

import argparse
import logging
import sys
import pw_cli.log

from pw_cpu_exception_cortex_m_protos import cpu_state_pb2
from pw_cpu_exception_cortex_m import exception_analyzer

_LOG = logging.getLogger('decode_cfsr')


def _parse_args() -> argparse.Namespace:
    """Parses arguments for this script, splitting out the command to run."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'cfsr', type=lambda val: int(val, 0), help='The Cortex-M CFSR to decode'
    )
    return parser.parse_args()


def dump_cfsr(cfsr: int) -> int:
    cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
    cpu_state_proto.cfsr = cfsr
    cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
        cpu_state_proto
    )
    _LOG.info(cpu_state_info)
    return 0


if __name__ == '__main__':
    pw_cli.log.install(level=logging.INFO)
    sys.exit(dump_cfsr(**vars(_parse_args())))
