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
"""Options file parsing for proto generation."""

from fnmatch import fnmatchcase
from pathlib import Path
import re
from typing import List, Tuple

from google.protobuf import text_format

from pw_protobuf_codegen_protos.options_pb2 import Options

_MULTI_LINE_COMMENT_RE = re.compile(r'/\*.*?\*/', flags=re.MULTILINE)
_SINGLE_LINE_COMMENT_RE = re.compile(r'//.*?$', flags=re.MULTILINE)
_SHELL_STYLE_COMMENT_RE = re.compile(r'#.*?$', flags=re.MULTILINE)

# A list of (proto field path, Options) tuples.
FieldOptions = List[Tuple[str, Options]]


def load_options_from(options: FieldOptions, options_file_name: Path):
    """Loads a single .options file for the given .proto"""
    with open(options_file_name) as options_file:
        # Read the options file and strip all styles of comments before parsing.
        options_data = options_file.read()
        options_data = _MULTI_LINE_COMMENT_RE.sub('', options_data)
        options_data = _SINGLE_LINE_COMMENT_RE.sub('', options_data)
        options_data = _SHELL_STYLE_COMMENT_RE.sub('', options_data)

        for line in options_data.split('\n'):
            parts = line.strip().split(None, 1)
            if len(parts) < 2:
                continue

            # Parse as a name glob followed by a protobuf text format.
            try:
                opts = Options()
                text_format.Merge(parts[1], opts)
                options.append((parts[0], opts))
            except:  # pylint: disable=bare-except
                continue


def load_options(
    include_paths: List[Path], proto_file_name: Path
) -> FieldOptions:
    """Loads the .options for the given .proto."""
    options: FieldOptions = []

    for include_path in include_paths:
        options_file_name = include_path / proto_file_name.with_suffix(
            '.options'
        )
        if options_file_name.exists():
            load_options_from(options, options_file_name)

    return options


def match_options(name: str, options: FieldOptions) -> Options:
    """Return the matching options for a name."""
    matched = Options()
    for name_glob, mask_options in options:
        if fnmatchcase(name, name_glob):
            matched.MergeFrom(mask_options)

    return matched
