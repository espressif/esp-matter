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
"""C++-related checks."""

import logging

from pw_presubmit import (
    build,
    Check,
    format_code,
    PresubmitContext,
    filter_paths,
)

_LOG: logging.Logger = logging.getLogger(__name__)


@filter_paths(endswith=format_code.CPP_HEADER_EXTS, exclude=(r'\.pb\.h$',))
def pragma_once(ctx: PresubmitContext) -> None:
    """Presubmit check that ensures all header files contain '#pragma once'."""

    for path in ctx.paths:
        _LOG.debug('Checking %s', path)
        with open(path) as file:
            for line in file:
                if line.startswith('#pragma once'):
                    break
            else:
                ctx.fail('#pragma once is missing!', path=path)


@Check
def asan(ctx: PresubmitContext) -> None:
    """Test with the address sanitizer."""
    build.gn_gen(ctx)
    build.ninja(ctx, 'asan')


@Check
def msan(ctx: PresubmitContext) -> None:
    """Test with the memory sanitizer."""
    build.gn_gen(ctx)
    build.ninja(ctx, 'msan')


@Check
def tsan(ctx: PresubmitContext) -> None:
    """Test with the thread sanitizer."""
    build.gn_gen(ctx)
    build.ninja(ctx, 'tsan')


@Check
def ubsan(ctx: PresubmitContext) -> None:
    """Test with the undefined behavior sanitizer."""
    build.gn_gen(ctx)
    build.ninja(ctx, 'ubsan')


@Check
def runtime_sanitizers(ctx: PresubmitContext) -> None:
    """Test with the address, thread, and undefined behavior sanitizers."""
    build.gn_gen(ctx)
    build.ninja(ctx, 'runtime_sanitizers')


def all_sanitizers():
    # TODO(b/234876100): msan will not work until the C++ standard library
    # included in the sysroot has a variant built with msan.
    return [asan, tsan, ubsan, runtime_sanitizers]
