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
"""Module for running subprocesses from pw and capturing their output."""

import asyncio
import logging
import os
import shlex
import tempfile
from typing import Dict, IO, Tuple, Union, Optional

import pw_cli.color
import pw_cli.log

_COLOR = pw_cli.color.colors()
_LOG = logging.getLogger(__name__)

# Environment variable passed down to subprocesses to indicate that they are
# running as a subprocess. Can be imported by other code.
PW_SUBPROCESS_ENV = 'PW_SUBPROCESS'


class CompletedProcess:
    """Information about a process executed in run_async."""

    def __init__(
        self,
        process: 'asyncio.subprocess.Process',
        output: Union[bytes, IO[bytes]],
    ):
        assert process.returncode is not None
        self.returncode: int = process.returncode
        self.pid = process.pid
        self._output = output

    @property
    def output(self) -> bytes:
        # If the output is a file, read it, then close it.
        if not isinstance(self._output, bytes):
            with self._output as file:
                file.flush()
                file.seek(0)
                self._output = file.read()

        return self._output


async def _run_and_log(program: str, args: Tuple[str, ...], env: dict):
    process = await asyncio.create_subprocess_exec(
        program,
        *args,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.STDOUT,
        env=env,
    )

    output = bytearray()

    if process.stdout:
        while True:
            line = await process.stdout.readline()
            if not line:
                break

            output += line
            _LOG.log(
                pw_cli.log.LOGLEVEL_STDOUT,
                '[%s] %s',
                _COLOR.bold_white(process.pid),
                line.decode(errors='replace').rstrip(),
            )

    return process, bytes(output)


async def run_async(
    program: str,
    *args: str,
    env: Optional[Dict[str, str]] = None,
    log_output: bool = False,
    timeout: Optional[float] = None,
) -> CompletedProcess:
    """Runs a command, capturing and optionally logging its output.

    Returns a CompletedProcess with details from the process.
    """

    _LOG.debug(
        'Running `%s`', ' '.join(shlex.quote(arg) for arg in [program, *args])
    )

    hydrated_env = os.environ.copy()
    if env is not None:
        for key, value in env.items():
            hydrated_env[key] = value
    hydrated_env[PW_SUBPROCESS_ENV] = '1'
    output: Union[bytes, IO[bytes]]

    if log_output:
        process, output = await _run_and_log(program, args, hydrated_env)
    else:
        output = tempfile.TemporaryFile()
        process = await asyncio.create_subprocess_exec(
            program,
            *args,
            stdout=output,
            stderr=asyncio.subprocess.STDOUT,
            env=hydrated_env,
        )

    try:
        await asyncio.wait_for(process.wait(), timeout)
    except asyncio.TimeoutError:
        _LOG.error('%s timed out after %d seconds', program, timeout)
        process.kill()
        await process.wait()

    if process.returncode:
        _LOG.error('%s exited with status %d', program, process.returncode)
    else:
        _LOG.info('%s exited successfully', program)

    return CompletedProcess(process, output)


def run(program: str, *args: str, **kwargs) -> CompletedProcess:
    """Synchronous wrapper for run_async."""
    return asyncio.run(run_async(program, *args, **kwargs))
