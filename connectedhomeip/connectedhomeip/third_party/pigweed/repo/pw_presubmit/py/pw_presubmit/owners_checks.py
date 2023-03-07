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
#
"""OWNERS file checks."""

import argparse
import collections
import dataclasses
import difflib
import enum
import functools
import logging
import pathlib
import re
import sys
from typing import (
    Callable,
    Collection,
    DefaultDict,
    Dict,
    Iterable,
    List,
    OrderedDict,
    Set,
    Union,
)
from pw_presubmit import git_repo
from pw_presubmit.presubmit import PresubmitFailure

_LOG = logging.getLogger(__name__)


class LineType(enum.Enum):
    COMMENT = "comment"
    WILDCARD = "wildcard"
    FILE_LEVEL = "file_level"
    FILE_RULE = "file_rule"
    INCLUDE = "include"
    PER_FILE = "per-file"
    USER = "user"
    # Special type to hold lines that don't get attached to another type
    TRAILING_COMMENTS = "trailing-comments"


_LINE_TYPERS: OrderedDict[
    LineType, Callable[[str], bool]
] = collections.OrderedDict(
    (
        (LineType.COMMENT, lambda x: x.startswith("#")),
        (LineType.WILDCARD, lambda x: x == "*"),
        (LineType.FILE_LEVEL, lambda x: x.startswith("set ")),
        (LineType.FILE_RULE, lambda x: x.startswith("file:")),
        (LineType.INCLUDE, lambda x: x.startswith("include ")),
        (LineType.PER_FILE, lambda x: x.startswith("per-file ")),
        (
            LineType.USER,
            lambda x: bool(re.match("^[a-zA-Z1-9.+-]+@[a-zA-Z0-9.-]+", x)),
        ),
    )
)


class OwnersError(Exception):
    """Generic level OWNERS file error."""

    def __init__(self, message: str, *args: object) -> None:
        super().__init__(*args)
        self.message = message


class FormatterError(OwnersError):
    """Errors where formatter doesn't know how to act."""


class OwnersDuplicateError(OwnersError):
    """Errors where duplicate lines are found in OWNERS files."""


class OwnersUserGrantError(OwnersError):
    """Invalid user grant, * is used with any other grant."""


class OwnersProhibitedError(OwnersError):
    """Any line that is prohibited by the owners syntax.

    https://android-review.googlesource.com/plugins/code-owners/Documentation/backend-find-owners.html
    """


class OwnersDependencyError(OwnersError):
    """OWNERS file tried to import file that does not exists."""


class OwnersInvalidLineError(OwnersError):
    """Line in OWNERS file does not match any 'line_typer'."""


class OwnersStyleError(OwnersError):
    """OWNERS file does not match style guide."""


@dataclasses.dataclass
class Line:
    content: str
    comments: List[str] = dataclasses.field(default_factory=list)


class OwnersFile:
    """Holds OWNERS file in easy to use parsed structure."""

    path: pathlib.Path
    original_lines: List[str]
    sections: Dict[LineType, List[Line]]
    formatted_lines: List[str]

    def __init__(self, path: pathlib.Path) -> None:
        if not path.exists():
            error_msg = f"Tried to import {path} but it does not exists"
            raise OwnersDependencyError(error_msg)
        self.path = path

        self.original_lines = self.load_owners_file(self.path)
        cleaned_lines = self.clean_lines(self.original_lines)
        self.sections = self.parse_owners(cleaned_lines)
        self.formatted_lines = self.format_sections(self.sections)

    @staticmethod
    def load_owners_file(owners_file: pathlib.Path) -> List[str]:
        return owners_file.read_text().split("\n")

    @staticmethod
    def clean_lines(dirty_lines: List[str]) -> List[str]:
        """Removes extra whitespace from list of strings."""

        cleaned_lines = []
        for line in dirty_lines:
            line = line.strip()  # Remove initial and trailing whitespace

            # Compress duplicated whitespace and remove tabs.
            # Allow comment lines to break this rule as they may have initial
            # whitespace for lining up text with neighboring lines.
            if not line.startswith("#"):
                line = re.sub(r"\s+", " ", line)
            if line:
                cleaned_lines.append(line)
        return cleaned_lines

    @staticmethod
    def __find_line_type(line: str) -> LineType:
        for line_type, type_matcher in _LINE_TYPERS.items():
            if type_matcher(line):
                return line_type

        raise OwnersInvalidLineError(
            f"Unrecognized OWNERS file line, '{line}'."
        )

    @staticmethod
    def parse_owners(
        cleaned_lines: List[str],
    ) -> DefaultDict[LineType, List[Line]]:
        """Converts text lines of OWNERS into structured object."""
        sections: DefaultDict[LineType, List[Line]] = collections.defaultdict(
            list
        )
        comment_buffer: List[str] = []

        def add_line_to_sections(
            sections, section: LineType, line: str, comment_buffer: List[str]
        ):
            if any(
                seen_line.content == line for seen_line in sections[section]
            ):
                raise OwnersDuplicateError(f"Duplicate line '{line}'.")
            line_obj = Line(content=line, comments=comment_buffer)
            sections[section].append(line_obj)

        for line in cleaned_lines:
            line_type: LineType = OwnersFile.__find_line_type(line)
            if line_type == LineType.COMMENT:
                comment_buffer.append(line)
            else:
                add_line_to_sections(sections, line_type, line, comment_buffer)
                comment_buffer = []

        add_line_to_sections(
            sections, LineType.TRAILING_COMMENTS, "", comment_buffer
        )

        return sections

    @staticmethod
    def format_sections(
        sections: DefaultDict[LineType, List[Line]]
    ) -> List[str]:
        """Returns ideally styled OWNERS file.

        The styling rules are
        * Content will be sorted in the following orders with a blank line
        separating
            * "set noparent"
            * "include" lines
            * "file:" lines
            * user grants (example, "*", foo@example.com)
            * "per-file:" lines
        * Do not combine user grants and "*"
        * User grants should be sorted alphabetically (this assumes English
        ordering)

        Returns:
          List of strings that make up a styled version of a OWNERS file.

        Raises:
          FormatterError: When formatter does not handle all lines of input.
                          This is a coding error in owners_checks.
        """
        all_sections = [
            LineType.FILE_LEVEL,
            LineType.INCLUDE,
            LineType.FILE_RULE,
            LineType.WILDCARD,
            LineType.USER,
            LineType.PER_FILE,
            LineType.TRAILING_COMMENTS,
        ]
        formatted_lines: List[str] = []

        def append_section(line_type):
            # Add a line of separation if there was a previous section and our
            # current section has any content. I.e. do not lead with padding and
            # do not have multiple successive lines of padding.
            if (
                formatted_lines
                and line_type != LineType.TRAILING_COMMENTS
                and sections[line_type]
            ):
                formatted_lines.append("")

            sections[line_type].sort(key=lambda line: line.content)
            for line in sections[line_type]:
                # Strip keep-sorted comments out since sorting is done by this
                # script
                formatted_lines.extend(
                    [
                        comment
                        for comment in line.comments
                        if not comment.startswith("# keep-sorted: ")
                    ]
                )
                formatted_lines.append(line.content)

        for section in all_sections:
            append_section(section)

        if any(section_name not in all_sections for section_name in sections):
            raise FormatterError("Formatter did not process all sections.")
        return formatted_lines

    def check_style(self) -> None:
        """Checks styling of OWNERS file.

        Enforce consistent style on OWNERS file. This also incidentally detects
        a few classes of errors.

        Raises:
          OwnersStyleError: Indicates styled lines do not match original input.
        """

        if self.original_lines != self.formatted_lines:
            print(
                "\n".join(
                    difflib.unified_diff(
                        self.original_lines,
                        self.formatted_lines,
                        fromfile=str(self.path),
                        tofile="styled",
                        lineterm="",
                    )
                )
            )

            raise OwnersStyleError(
                "OWNERS file format does not follow styling."
            )

    def look_for_owners_errors(self) -> None:
        """Scans owners files for invalid or useless content."""

        # Confirm when using the wildcard("*") we don't also try to use
        # individual user grants.
        if self.sections[LineType.WILDCARD] and self.sections[LineType.USER]:
            raise OwnersUserGrantError(
                "Do not use '*' with individual user "
                "grants, * already applies to all users."
            )

        # NOTE: Using the include keyword in combination with a per-file rule is
        # not possible.
        # https://android-review.googlesource.com/plugins/code-owners/Documentation/backend-find-owners.html#syntax:~:text=NOTE%3A%20Using%20the%20include%20keyword%20in%20combination%20with%20a%20per%2Dfile%20rule%20is%20not%20possible.
        if self.sections[LineType.INCLUDE] and self.sections[LineType.PER_FILE]:
            raise OwnersProhibitedError(
                "'include' cannot be used with 'per-file'."
            )

    def __complete_path(self, sub_owners_file_path) -> pathlib.Path:
        """Always return absolute path."""
        # Absolute paths start with the git/project root
        if sub_owners_file_path.startswith("/"):
            root = git_repo.root(self.path)
            full_path = root / sub_owners_file_path[1:]
        else:
            # Relative paths start with owners file dir
            full_path = self.path.parent / sub_owners_file_path
        return full_path.resolve()

    def get_dependencies(self) -> List[pathlib.Path]:
        """Finds owners files this file includes."""
        dependencies = []
        # All the includes
        for include in self.sections.get(LineType.INCLUDE, []):
            file_str = include.content[len("include ") :]
            dependencies.append(self.__complete_path(file_str))

        # all file: rules:
        for file_rule in self.sections.get(LineType.FILE_RULE, []):
            file_str = file_rule.content[len("file:") :]
            if ":" in file_str:
                _LOG.warning(
                    "TODO(b/254322931): This check does not yet support "
                    "<project> or <branch> in a file: rule"
                )
                _LOG.warning(
                    "It will not check line '%s' found in %s",
                    file_rule.content,
                    self.path,
                )

            dependencies.append(self.__complete_path(file_str))

        # all the per-file rule includes
        for per_file in self.sections.get(LineType.PER_FILE, []):
            file_str = per_file.content[len("per-file ") :]
            access_grant = file_str[file_str.index("=") + 1 :]
            if access_grant.startswith("file:"):
                dependencies.append(
                    self.__complete_path(access_grant[len("file:") :])
                )

        return dependencies

    def write_formatted(self) -> None:
        self.path.write_text("\n".join(self.formatted_lines))


def resolve_owners_tree(root_owners: pathlib.Path) -> List[OwnersFile]:
    """Given a starting OWNERS file return it and all of it's dependencies."""
    found = []
    todo = collections.deque((root_owners,))
    checked: Set[pathlib.Path] = set()
    while todo:
        cur_file = todo.popleft()
        checked.add(cur_file)
        owners_obj = OwnersFile(cur_file)
        found.append(owners_obj)
        new_dependents = owners_obj.get_dependencies()
        for new_dep in new_dependents:
            if new_dep not in checked and new_dep not in todo:
                todo.append(new_dep)
    return found


def _run_owners_checks(owners_obj: OwnersFile) -> None:
    owners_obj.look_for_owners_errors()
    owners_obj.check_style()


def _format_owners_file(owners_obj: OwnersFile) -> None:
    owners_obj.look_for_owners_errors()

    if owners_obj.original_lines != owners_obj.formatted_lines:
        owners_obj.write_formatted()


def _list_unwrapper(
    func, list_or_path: Union[Iterable[pathlib.Path], pathlib.Path]
) -> Dict[pathlib.Path, str]:
    """Decorator that accepts Paths or list of Paths and iterates as needed."""
    errors: Dict[pathlib.Path, str] = {}
    if isinstance(list_or_path, Iterable):
        files = list_or_path
    else:
        files = (list_or_path,)

    all_owners_obj: List[OwnersFile] = []
    for file in files:
        all_owners_obj.extend(resolve_owners_tree(file))

    checked: Set[pathlib.Path] = set()
    for current_owners in all_owners_obj:
        # Ensure we don't check the same file twice
        if current_owners.path in checked:
            continue
        checked.add(current_owners.path)
        try:
            func(current_owners)
        except OwnersError as err:
            errors[current_owners.path] = err.message
            _LOG.error(
                "%s: %s", str(current_owners.path.absolute()), err.message
            )
    return errors


# This generates decorated versions of the functions that can used with both
# formatter (which supplies files one at a time) and presubmits (which supplies
# list of files).
run_owners_checks = functools.partial(_list_unwrapper, _run_owners_checks)
format_owners_file = functools.partial(_list_unwrapper, _format_owners_file)


def presubmit_check(
    files: Union[pathlib.Path, Collection[pathlib.Path]]
) -> None:
    errors = run_owners_checks(files)
    if errors:
        for file in errors:
            _LOG.warning("  pw format --fix %s", file)
        _LOG.warning("will automatically fix this.")
        raise PresubmitFailure


def main() -> int:
    """Standalone test of styling."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--style", action="store_true")
    parser.add_argument("--owners_file", required=True, type=str)
    args = parser.parse_args()

    try:
        owners_obj = OwnersFile(pathlib.Path(args.owners_file))
        owners_obj.look_for_owners_errors()
        owners_obj.check_style()
    except OwnersError as err:
        _LOG.error("%s %s", err, err.message)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
