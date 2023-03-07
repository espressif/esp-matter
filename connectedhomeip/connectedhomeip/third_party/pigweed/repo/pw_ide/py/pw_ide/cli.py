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
"""CLI tools for pw_ide."""

import argparse
import enum
from inspect import cleandoc
from pathlib import Path
import re
from typing import Any, Callable, Dict, List, Optional, Protocol

from pw_ide.commands import (
    cmd_clear,
    cmd_cpp,
    cmd_python,
    cmd_reset,
    cmd_setup,
    cmd_vscode,
)

from pw_ide.vscode import VscSettingsType


def _get_docstring(obj: Any) -> Optional[str]:
    doc: Optional[str] = getattr(obj, '__doc__', None)
    return doc


class _ParsedDocstring:
    """Parses help content out of a standard docstring."""

    def __init__(self, obj: Any) -> None:
        self.description = ''
        self.epilog = ''

        if obj is not None and (doc := _get_docstring(obj)) is not None:
            lines = doc.split('\n')
            self.description = lines.pop(0)

            # Eliminate the blank line between the summary and the main content
            if len(lines) > 0:
                lines.pop(0)

            self.epilog = cleandoc('\n'.join(lines))


class SphinxStripperState(enum.Enum):
    SEARCHING = 0
    COLLECTING = 1
    HANDLING = 2


class SphinxStripper:
    """Strip Sphinx directives from text.

    The caller can provide an object with methods named _handle_directive_{}
    to handle specific directives. Otherwise, the default will apply.

    Feed text line by line to .process(line), then get the processed text back
    with .result().
    """

    def __init__(self, handler: Any) -> None:
        self.handler = handler
        self.directive: str = ''
        self.tag: str = ''
        self.lines_to_handle: List[str] = []
        self.handled_lines: List[str] = []
        self._prev_state: SphinxStripperState = SphinxStripperState.SEARCHING
        self._curr_state: SphinxStripperState = SphinxStripperState.SEARCHING

    @property
    def state(self) -> SphinxStripperState:
        return self._curr_state

    @state.setter
    def state(self, value: SphinxStripperState) -> None:
        self._prev_state = self._curr_state
        self._curr_state = value

    def search_for_directives(self, line: str) -> None:
        match = re.search(
            r'^\.\.\s*(?P<directive>[\-\w]+)::\s*(?P<tag>[\-\w]+)$', line
        )

        if match is not None:
            self.directive = match.group('directive')
            self.tag = match.group('tag')
            self.state = SphinxStripperState.COLLECTING
        else:
            self.handled_lines.append(line)

    def collect_lines(self, line) -> None:
        # Collect lines associated with a directive, including blank lines in
        # the middle of the directive text, but not the blank line between the
        # directive and the start of its text.
        if not (line.strip() == '' and len(self.lines_to_handle) == 0):
            self.lines_to_handle.append(line)

    def handle_lines(self, line: str = '') -> None:
        handler_fn = f'_handle_directive_{self.directive.replace("-", "_")}'

        self.handled_lines.extend(
            getattr(self.handler, handler_fn, lambda _, s: s)(
                self.tag, self.lines_to_handle
            )
        )

        self.handled_lines.append(line)
        self.lines_to_handle = []
        self.state = SphinxStripperState.SEARCHING

    def process_line(self, line: str) -> None:
        if self.state == SphinxStripperState.SEARCHING:
            self.search_for_directives(line)

        else:
            if self.state == SphinxStripperState.COLLECTING:
                # Assume that indented text below the directive is associated
                # with the directive.
                if line.strip() == '' or line[0] in (' ', '\t'):
                    self.collect_lines(line)
                # When we encounter non-indented text, we're done with this
                # directive.
                else:
                    self.state = SphinxStripperState.HANDLING

            if self.state == SphinxStripperState.HANDLING:
                self.handle_lines(line)

    def result(self) -> str:
        if self.state == SphinxStripperState.COLLECTING:
            self.state = SphinxStripperState.HANDLING
            self.handle_lines()

        return '\n'.join(self.handled_lines)


class RawDescriptionSphinxStrippedHelpFormatter(
    argparse.RawDescriptionHelpFormatter
):
    """An argparse formatter that strips Sphinx directives.

    CLI command docstrings can contain Sphinx directives for rendering in docs.
    But we don't want to include those directives when printing to the terminal.
    So we strip them and, if appropriate, replace them with something better
    suited to terminal output.
    """

    def _reformat(self, text: str) -> str:
        """Given a block of text, replace Sphinx directives.

        Directive handlers will be provided with the directive name, its tag,
        and all of the associated lines of text. "Association" is determined by
        those lines being indented to any degree under the directive.

        Unhandled directives will only have the directive line removed.
        """
        sphinx_stripper = SphinxStripper(self)

        for line in text.splitlines():
            sphinx_stripper.process_line(line)

        # The space at the end prevents the final blank line from being stripped
        # by argparse, which provides breathing room between the text and the
        # prompt.
        return sphinx_stripper.result() + ' '

    def _format_text(self, text: str) -> str:
        # This overrides an arparse method that is not technically a public API.
        return super()._format_text(self._reformat(text))

    def _handle_directive_code_block(  # pylint: disable=no-self-use
        self, tag: str, lines: List[str]
    ) -> List[str]:
        if tag == 'bash':
            processed_lines = []

            for line in lines:
                if line.strip() == '':
                    processed_lines.append(line)
                else:
                    stripped_line = line.lstrip()
                    indent = len(line) - len(stripped_line)
                    spaces = ' ' * indent
                    processed_line = f'{spaces}$ {stripped_line}'
                    processed_lines.append(processed_line)

            return processed_lines

        return lines


class _ParserAdder(Protocol):
    """Return type for _parser_adder.

    Essentially expresses the type of __call__, which cannot be expressed in
    type annotations.
    """

    def __call__(
        self, subcommand_handler: Callable[..., None], *args: Any, **kwargs: Any
    ) -> argparse.ArgumentParser:
        ...


def _parser_adder(subcommand_parser) -> _ParserAdder:
    """Create subcommand parsers with a consistent format.

    When given a subcommand handler, this will produce a parser that pulls the
    description, help, and epilog values from its docstring, and passes parsed
    args on to to the function.

    Create a subcommand parser, then feed it to this to get an `add_parser`
    function:

    .. code-block:: python

        subcommand_parser = parser_root.add_subparsers(help='Subcommands')
        add_parser = _parser_adder(subcommand_parser)

    Then use `add_parser` instead of `subcommand_parser.add_parser`.
    """

    def _add_parser(
        subcommand_handler: Callable[..., None], *args, **kwargs
    ) -> argparse.ArgumentParser:
        doc = _ParsedDocstring(subcommand_handler)
        default_kwargs = dict(
            # Displayed in list of subcommands
            description=doc.description,
            # Displayed as top-line summary for this subcommand's help
            help=doc.description,
            # Displayed as detailed help text for this subcommand's help
            epilog=doc.epilog,
            # Ensures that formatting is preserved and Sphinx directives are
            # stripped out when printing to the terminal
            formatter_class=RawDescriptionSphinxStrippedHelpFormatter,
        )

        new_kwargs = {**default_kwargs, **kwargs}
        parser = subcommand_parser.add_parser(*args, **new_kwargs)
        parser.set_defaults(func=subcommand_handler)
        return parser

    return _add_parser


def _build_argument_parser() -> argparse.ArgumentParser:
    parser_root = argparse.ArgumentParser(prog='pw ide', description=__doc__)

    parser_root.set_defaults(
        func=lambda *_args, **_kwargs: parser_root.print_help()
    )

    subcommand_parser = parser_root.add_subparsers(help='Subcommands')
    add_parser = _parser_adder(subcommand_parser)

    add_parser(cmd_setup, 'setup')

    parser_reset = add_parser(cmd_reset, 'reset')
    parser_reset.add_argument(
        '--hard',
        action='store_true',
        help='completely remove the .pw_ide working '
        'dir and supported editor files',
    )

    parser_cpp = add_parser(cmd_cpp, 'cpp')
    parser_cpp.add_argument(
        '-l',
        '--list',
        dest='should_list_targets',
        action='store_true',
        help='list the targets available for C/C++ ' 'language analysis',
    )
    parser_cpp.add_argument(
        '-g',
        '--get',
        dest='should_get_target',
        action='store_true',
        help='print the current target used for C/C++ ' 'language analysis',
    )
    parser_cpp.add_argument(
        '-s',
        '--set',
        dest='target_to_set',
        metavar='TARGET',
        help='set the target to use for C/C++ language ' 'server analysis',
    )
    parser_cpp.add_argument(
        '--set-default',
        dest='use_default_target',
        action='store_true',
        help='set the C/C++ analysis target to the default '
        'defined in pw_ide settings',
    )
    parser_cpp.add_argument(
        '--no-override',
        dest='override_current_target',
        action='store_const',
        const=False,
        default=True,
        help='if called with --set, don\'t override the '
        'current target if one is already set',
    )
    parser_cpp.add_argument(
        '--ninja',
        dest='should_run_ninja',
        action='store_true',
        help='use Ninja to generate a compilation database',
    )
    parser_cpp.add_argument(
        '--gn',
        dest='should_run_gn',
        action='store_true',
        help='run gn gen {out} --export-compile-commands, '
        'along with any other arguments defined in args.gn',
    )
    parser_cpp.add_argument(
        '-p',
        '--process',
        dest='compdb_file_paths',
        metavar='COMPILATION_DATABASE_FILES',
        type=Path,
        nargs='*',
        help='process a file or several files matching '
        'the clang compilation database format',
    )
    parser_cpp.add_argument(
        '--build-dir',
        type=Path,
        help='override the build directory defined in ' 'pw_ide settings',
    )
    parser_cpp.add_argument(
        '--clangd-command',
        action='store_true',
        help='print the command for your system that runs '
        'clangd in the activated Pigweed environment',
    )
    parser_cpp.add_argument(
        '--clangd-command-for',
        dest='clangd_command_system',
        metavar='SYSTEM',
        help='print the command for the specified system '
        'that runs clangd in the activated Pigweed '
        'environment',
    )

    parser_python = add_parser(cmd_python, 'python')
    parser_python.add_argument(
        '--venv',
        dest='should_print_venv',
        action='store_true',
        help='print the path to the Pigweed Python ' 'virtual environment',
    )

    parser_vscode = add_parser(cmd_vscode, 'vscode')
    parser_vscode.add_argument(
        '--include',
        nargs='+',
        type=VscSettingsType,
        metavar='SETTINGS_TYPE',
        help='update only these settings types',
    )
    parser_vscode.add_argument(
        '--exclude',
        nargs='+',
        type=VscSettingsType,
        metavar='SETTINGS_TYPE',
        help='do not update these settings types',
    )
    parser_vscode.add_argument(
        '--no-override',
        action='store_true',
        help='don\'t overwrite existing active ' 'settings files',
    )

    parser_clear = add_parser(cmd_clear, 'clear')
    parser_clear.add_argument(
        '--compdb',
        action='store_true',
        help='delete all compilation database from ' 'the working directory',
    )
    parser_clear.add_argument(
        '--cache',
        action='store_true',
        help='delete all compilation database caches '
        'from the working directory',
    )
    parser_clear.add_argument(
        '--editor',
        metavar='EDITOR',
        help='delete the active settings file for '
        'the provided supported editor',
    )
    parser_clear.add_argument(
        '--editor-backups',
        metavar='EDITOR',
        help='delete backup settings files for '
        'the provided supported editor',
    )

    return parser_root


def _parse_args() -> argparse.Namespace:
    args = _build_argument_parser().parse_args()
    return args


def _dispatch_command(func: Callable, **kwargs: Dict[str, Any]) -> int:
    """Dispatch arguments to a subcommand handler.

    Each CLI subcommand is handled by handler function, which is registered
    with the subcommand parser with `parser.set_defaults(func=handler)`.
    By calling this function with the parsed args, the appropriate subcommand
    handler is called, and the arguments are passed to it as kwargs.
    """
    return func(**kwargs)


def parse_args_and_dispatch_command() -> int:
    return _dispatch_command(**vars(_parse_args()))
