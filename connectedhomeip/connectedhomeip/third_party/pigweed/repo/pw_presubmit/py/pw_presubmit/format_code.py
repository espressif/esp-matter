#!/usr/bin/env python3

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
"""Checks and fixes formatting for source files.

This uses clang-format, gn format, gofmt, and python -m yapf to format source
code. These tools must be available on the path when this script is invoked!
"""

import argparse
import collections
import dataclasses
import difflib
import logging
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
from typing import (
    Callable,
    Collection,
    Dict,
    Iterable,
    List,
    NamedTuple,
    Optional,
    Pattern,
    Tuple,
    Union,
)

try:
    import pw_presubmit
except ImportError:
    # Append the pw_presubmit package path to the module search path to allow
    # running this module without installing the pw_presubmit package.
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    import pw_presubmit

import pw_cli.color
import pw_cli.env
from pw_presubmit.presubmit import FileFilter
from pw_presubmit import cli, git_repo, owners_checks, PresubmitContext
from pw_presubmit.tools import exclude_paths, file_summary, log_run, plural

_LOG: logging.Logger = logging.getLogger(__name__)
_COLOR = pw_cli.color.colors()
_DEFAULT_PATH = Path('out', 'format')


@dataclasses.dataclass
class FormatContext:
    """Context passed into formatting helpers.

    This class is a subset of PresubmitContext (from presubmit.py) containing
    only what's needed by this function.

    For full documentation on the members see the PresubmitContext section of
    pw_presubmit/docs.rst.

    Args:
        root: Source checkout root directory
        output_dir: Output directory for this specific language
        paths: Modified files for the presubmit step to check (often used in
            formatting steps but ignored in compile steps)
    """

    root: Path
    output_dir: Path
    paths: Tuple[Path, ...]


_Context = Union[PresubmitContext, FormatContext]


def _colorize_diff_line(line: str) -> str:
    if line.startswith('--- ') or line.startswith('+++ '):
        return _COLOR.bold_white(line)
    if line.startswith('-'):
        return _COLOR.red(line)
    if line.startswith('+'):
        return _COLOR.green(line)
    if line.startswith('@@ '):
        return _COLOR.cyan(line)
    return line


def colorize_diff(lines: Iterable[str]) -> str:
    """Takes a diff str or list of str lines and returns a colorized version."""
    if isinstance(lines, str):
        lines = lines.splitlines(True)

    return ''.join(_colorize_diff_line(line) for line in lines)


def _diff(path, original: bytes, formatted: bytes) -> str:
    return colorize_diff(
        difflib.unified_diff(
            original.decode(errors='replace').splitlines(True),
            formatted.decode(errors='replace').splitlines(True),
            f'{path}  (original)',
            f'{path}  (reformatted)',
        )
    )


Formatter = Callable[[str, bytes], bytes]


def _diff_formatted(path, formatter: Formatter) -> Optional[str]:
    """Returns a diff comparing a file to its formatted version."""
    with open(path, 'rb') as fd:
        original = fd.read()

    formatted = formatter(path, original)

    return None if formatted == original else _diff(path, original, formatted)


def _check_files(files, formatter: Formatter) -> Dict[Path, str]:
    errors = {}

    for path in files:
        difference = _diff_formatted(path, formatter)
        if difference:
            errors[path] = difference

    return errors


def _clang_format(*args: Union[Path, str], **kwargs) -> bytes:
    return log_run(
        ['clang-format', '--style=file', *args],
        stdout=subprocess.PIPE,
        check=True,
        **kwargs,
    ).stdout


def clang_format_check(ctx: _Context) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    return _check_files(ctx.paths, lambda path, _: _clang_format(path))


def clang_format_fix(ctx: _Context) -> Dict[Path, str]:
    """Fixes formatting for the provided files in place."""
    _clang_format('-i', *ctx.paths)
    return {}


def check_gn_format(ctx: _Context) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    return _check_files(
        ctx.paths,
        lambda _, data: log_run(
            ['gn', 'format', '--stdin'],
            input=data,
            stdout=subprocess.PIPE,
            check=True,
        ).stdout,
    )


def fix_gn_format(ctx: _Context) -> Dict[Path, str]:
    """Fixes formatting for the provided files in place."""
    log_run(['gn', 'format', *ctx.paths], check=True)
    return {}


def check_bazel_format(ctx: _Context) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    errors: Dict[Path, str] = {}

    def _format_temp(path: Union[Path, str], data: bytes) -> bytes:
        # buildifier doesn't have an option to output the changed file, so
        # copy the file to a temp location, run buildifier on it, read that
        # modified copy, and return its contents.
        with tempfile.TemporaryDirectory(dir=ctx.output_dir) as temp:
            build = Path(temp) / os.path.basename(path)
            build.write_bytes(data)

            proc = log_run(['buildifier', build], capture_output=True)
            if proc.returncode:
                stderr = proc.stderr.decode(errors='replace')
                stderr = stderr.replace(str(build), str(path))
                errors[Path(path)] = stderr
            return build.read_bytes()

    result = _check_files(ctx.paths, _format_temp)
    result.update(errors)
    return result


def fix_bazel_format(ctx: _Context) -> Dict[Path, str]:
    """Fixes formatting for the provided files in place."""
    errors = {}
    for path in ctx.paths:
        proc = log_run(['buildifier', path], capture_output=True)
        if proc.returncode:
            errors[path] = proc.stderr.decode()
    return errors


def check_owners_format(ctx: _Context) -> Dict[Path, str]:
    return owners_checks.run_owners_checks(ctx.paths)


def fix_owners_format(ctx: _Context) -> Dict[Path, str]:
    return owners_checks.format_owners_file(ctx.paths)


def check_go_format(ctx: _Context) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    return _check_files(
        ctx.paths,
        lambda path, _: log_run(
            ['gofmt', path], stdout=subprocess.PIPE, check=True
        ).stdout,
    )


def fix_go_format(ctx: _Context) -> Dict[Path, str]:
    """Fixes formatting for the provided files in place."""
    log_run(['gofmt', '-w', *ctx.paths], check=True)
    return {}


# TODO(b/259595799) Remove yapf support.
def _yapf(*args, **kwargs) -> subprocess.CompletedProcess:
    return log_run(
        ['python', '-m', 'yapf', '--parallel', *args],
        capture_output=True,
        **kwargs,
    )


_DIFF_START = re.compile(r'^--- (.*)\s+\(original\)$', flags=re.MULTILINE)


def check_py_format_yapf(ctx: _Context) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    process = _yapf('--diff', *ctx.paths)

    errors: Dict[Path, str] = {}

    if process.stdout:
        raw_diff = process.stdout.decode(errors='replace')

        matches = tuple(_DIFF_START.finditer(raw_diff))
        for start, end in zip(matches, (*matches[1:], None)):
            errors[Path(start.group(1))] = colorize_diff(
                raw_diff[start.start() : end.start() if end else None]
            )

    if process.stderr:
        _LOG.error(
            'yapf encountered an error:\n%s',
            process.stderr.decode(errors='replace').rstrip(),
        )
        errors.update({file: '' for file in ctx.paths if file not in errors})

    return errors


def fix_py_format_yapf(ctx: _Context) -> Dict[Path, str]:
    """Fixes formatting for the provided files in place."""
    _yapf('--in-place', *ctx.paths, check=True)
    return {}


_BLACK_OPTS = (
    '--skip-string-normalization',
    '--line-length',
    '80',
    '--target-version',
    'py310',
    '--include',
    r'\.pyi?$',
)


def check_py_format_black(ctx: _Context) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    errors: Dict[Path, str] = {}

    # Run black --check on the full list of paths and then only run black
    # individually on the files that black found issue with.
    changed_paths: List[str] = []
    for line in (
        log_run(
            ['black', '--check', *_BLACK_OPTS, *ctx.paths],
            capture_output=True,
        )
        .stderr.decode()
        .splitlines()
    ):
        if match := re.search(r'^would reformat (.*)\s*$', line):
            changed_paths.append(match.group(1))
    paths: Tuple[str, ...] = tuple(changed_paths)

    def _format_temp(path: Union[Path, str], data: bytes) -> bytes:
        # black doesn't have an option to output the changed file, so copy the
        # file to a temp location, run buildifier on it, read that modified
        # copy, and return its contents.
        with tempfile.TemporaryDirectory(dir=ctx.output_dir) as temp:
            build = Path(temp) / os.path.basename(path)
            build.write_bytes(data)

            proc = log_run(['black', *_BLACK_OPTS, build], capture_output=True)
            if proc.returncode:
                stderr = proc.stderr.decode(errors='replace')
                stderr = stderr.replace(str(build), str(path))
                errors[Path(path)] = stderr
            return build.read_bytes()

    result = _check_files(
        [x for x in ctx.paths if str(x).endswith(paths)],
        _format_temp,
    )
    result.update(errors)
    return result


def fix_py_format_black(ctx: _Context) -> Dict[Path, str]:
    """Fixes formatting for the provided files in place."""
    errors: Dict[Path, str] = {}

    for path in ctx.paths:
        proc = log_run(['black', *_BLACK_OPTS, path], capture_output=True)
        if proc.returncode:
            errors[path] = proc.stderr.decode()
    return errors


_TRAILING_SPACE = re.compile(rb'[ \t]+$', flags=re.MULTILINE)


def _check_trailing_space(paths: Iterable[Path], fix: bool) -> Dict[Path, str]:
    """Checks for and optionally removes trailing whitespace."""
    errors = {}

    for path in paths:
        with path.open('rb') as fd:
            contents = fd.read()

        corrected = _TRAILING_SPACE.sub(b'', contents)
        if corrected != contents:
            errors[path] = _diff(path, contents, corrected)

            if fix:
                with path.open('wb') as fd:
                    fd.write(corrected)

    return errors


def check_trailing_space(ctx: _Context) -> Dict[Path, str]:
    return _check_trailing_space(ctx.paths, fix=False)


def fix_trailing_space(ctx: _Context) -> Dict[Path, str]:
    _check_trailing_space(ctx.paths, fix=True)
    return {}


def print_format_check(
    errors: Dict[Path, str], show_fix_commands: bool
) -> None:
    """Prints and returns the result of a check_*_format function."""
    if not errors:
        # Don't print anything in the all-good case.
        return
    # Show the format fixing diff suggested by the tooling (with colors).
    _LOG.warning(
        'Found %d files with formatting errors. Format changes:', len(errors)
    )
    for diff in errors.values():
        print(diff, end='')

    # Show a copy-and-pastable command to fix the issues.
    if show_fix_commands:

        def path_relative_to_cwd(path: Path):
            try:
                return Path(path).resolve().relative_to(Path.cwd().resolve())
            except ValueError:
                return Path(path).resolve()

        message = (
            f'  pw format --fix {path_relative_to_cwd(path)}' for path in errors
        )
        _LOG.warning('To fix formatting, run:\n\n%s\n', '\n'.join(message))


class CodeFormat(NamedTuple):
    language: str
    filter: FileFilter
    check: Callable[[_Context], Dict[Path, str]]
    fix: Callable[[_Context], Dict[Path, str]]

    @property
    def extensions(self):
        # TODO(b/23842636): Switch calls of this to using 'filter' and remove.
        return self.filter.endswith


CPP_HEADER_EXTS = frozenset(('.h', '.hpp', '.hxx', '.h++', '.hh', '.H'))
CPP_SOURCE_EXTS = frozenset(
    ('.c', '.cpp', '.cxx', '.c++', '.cc', '.C', '.inc', '.inl')
)
CPP_EXTS = CPP_HEADER_EXTS.union(CPP_SOURCE_EXTS)
CPP_FILE_FILTER = FileFilter(
    endswith=CPP_EXTS, exclude=(r'\.pb\.h$', r'\.pb\.c$')
)

C_FORMAT = CodeFormat(
    'C and C++', CPP_FILE_FILTER, clang_format_check, clang_format_fix
)

PROTO_FORMAT: CodeFormat = CodeFormat(
    'Protocol buffer',
    FileFilter(endswith=('.proto',)),
    clang_format_check,
    clang_format_fix,
)

JAVA_FORMAT: CodeFormat = CodeFormat(
    'Java',
    FileFilter(endswith=('.java',)),
    clang_format_check,
    clang_format_fix,
)

JAVASCRIPT_FORMAT: CodeFormat = CodeFormat(
    'JavaScript',
    FileFilter(endswith=('.js',)),
    clang_format_check,
    clang_format_fix,
)

GO_FORMAT: CodeFormat = CodeFormat(
    'Go', FileFilter(endswith=('.go',)), check_go_format, fix_go_format
)

# TODO(b/259595799) Remove yapf support.
PYTHON_FORMAT_YAPF: CodeFormat = CodeFormat(
    'Python',
    FileFilter(endswith=('.py',)),
    check_py_format_yapf,
    fix_py_format_yapf,
)

PYTHON_FORMAT_BLACK: CodeFormat = CodeFormat(
    'Python',
    FileFilter(endswith=('.py',)),
    check_py_format_black,
    fix_py_format_black,
)

GN_FORMAT: CodeFormat = CodeFormat(
    'GN', FileFilter(endswith=('.gn', '.gni')), check_gn_format, fix_gn_format
)

BAZEL_FORMAT: CodeFormat = CodeFormat(
    'Bazel',
    FileFilter(endswith=('BUILD', '.bazel', '.bzl'), name=('WORKSPACE')),
    check_bazel_format,
    fix_bazel_format,
)

COPYBARA_FORMAT: CodeFormat = CodeFormat(
    'Copybara',
    FileFilter(endswith=('.bara.sky',)),
    check_bazel_format,
    fix_bazel_format,
)

# TODO(b/234881054): Add real code formatting support for CMake
CMAKE_FORMAT: CodeFormat = CodeFormat(
    'CMake',
    FileFilter(endswith=('CMakeLists.txt', '.cmake')),
    check_trailing_space,
    fix_trailing_space,
)

RST_FORMAT: CodeFormat = CodeFormat(
    'reStructuredText',
    FileFilter(endswith=('.rst',)),
    check_trailing_space,
    fix_trailing_space,
)

MARKDOWN_FORMAT: CodeFormat = CodeFormat(
    'Markdown',
    FileFilter(endswith=('.md',)),
    check_trailing_space,
    fix_trailing_space,
)

OWNERS_CODE_FORMAT = CodeFormat(
    'OWNERS',
    filter=FileFilter(name=('OWNERS',)),
    check=check_owners_format,
    fix=fix_owners_format,
)

_CODE_FORMATS_WITHOUT_PYTHON: Tuple[CodeFormat, ...] = (
    # keep-sorted: start
    BAZEL_FORMAT,
    CMAKE_FORMAT,
    COPYBARA_FORMAT,
    C_FORMAT,
    GN_FORMAT,
    GO_FORMAT,
    JAVASCRIPT_FORMAT,
    JAVA_FORMAT,
    MARKDOWN_FORMAT,
    OWNERS_CODE_FORMAT,
    PROTO_FORMAT,
    RST_FORMAT,
    # keep-sorted: end
)

# TODO(b/259595799) Remove yapf support.
CODE_FORMATS_WITH_YAPF: Tuple[CodeFormat, ...] = (
    *_CODE_FORMATS_WITHOUT_PYTHON,
    PYTHON_FORMAT_YAPF,
)

CODE_FORMATS_WITH_BLACK: Tuple[CodeFormat, ...] = (
    *_CODE_FORMATS_WITHOUT_PYTHON,
    PYTHON_FORMAT_BLACK,
)

# TODO(b/259595799) For downstream compatibility only.
CODE_FORMATS = CODE_FORMATS_WITH_YAPF


def presubmit_check(
    code_format: CodeFormat,
    *,
    exclude: Collection[Union[str, Pattern[str]]] = (),
) -> Callable:
    """Creates a presubmit check function from a CodeFormat object.

    Args:
      exclude: Additional exclusion regexes to apply.
    """

    # Make a copy of the FileFilter and add in any additional excludes.
    file_filter = FileFilter(**vars(code_format.filter))
    file_filter.exclude += tuple(re.compile(e) for e in exclude)

    @pw_presubmit.filter_paths(file_filter=file_filter)
    def check_code_format(ctx: pw_presubmit.PresubmitContext):
        errors = code_format.check(ctx)
        print_format_check(
            errors,
            # When running as part of presubmit, show the fix command help.
            show_fix_commands=True,
        )
        if errors:
            raise pw_presubmit.PresubmitFailure

    language = code_format.language.lower().replace('+', 'p').replace(' ', '_')
    check_code_format.name = f'{language}_format'
    check_code_format.doc = f'Check the format of {code_format.language} files.'

    return check_code_format


def presubmit_checks(
    *,
    exclude: Collection[Union[str, Pattern[str]]] = (),
    code_formats: Collection[CodeFormat] = CODE_FORMATS_WITH_YAPF,
) -> Tuple[Callable, ...]:
    """Returns a tuple with all supported code format presubmit checks.

    Args:
      exclude: Additional exclusion regexes to apply.
      code_formats: A list of CodeFormat objects to run checks with.
    """

    return tuple(presubmit_check(fmt, exclude=exclude) for fmt in code_formats)


class CodeFormatter:
    """Checks or fixes the formatting of a set of files."""

    def __init__(
        self,
        root: Path,
        files: Iterable[Path],
        output_dir: Path,
        code_formats: Collection[CodeFormat] = CODE_FORMATS_WITH_YAPF,
    ):
        self.root = root
        self.paths = list(files)
        self._formats: Dict[CodeFormat, List] = collections.defaultdict(list)
        self.root_output_dir = output_dir

        for path in self.paths:
            for code_format in code_formats:
                if code_format.filter.matches(path):
                    _LOG.debug(
                        'Formatting %s as %s', path, code_format.language
                    )
                    self._formats[code_format].append(path)
                    break
            else:
                _LOG.debug('No formatter found for %s', path)

    def _context(self, code_format: CodeFormat):
        outdir = self.root_output_dir / code_format.language.replace(' ', '_')
        os.makedirs(outdir, exist_ok=True)

        return FormatContext(
            root=self.root,
            output_dir=outdir,
            paths=tuple(self._formats[code_format]),
        )

    def check(self) -> Dict[Path, str]:
        """Returns {path: diff} for files with incorrect formatting."""
        errors: Dict[Path, str] = {}

        for code_format, files in self._formats.items():
            _LOG.debug('Checking %s', ', '.join(str(f) for f in files))
            errors.update(code_format.check(self._context(code_format)))

        return collections.OrderedDict(sorted(errors.items()))

    def fix(self) -> Dict[Path, str]:
        """Fixes format errors for supported files in place."""
        all_errors: Dict[Path, str] = {}
        for code_format, files in self._formats.items():
            errors = code_format.fix(self._context(code_format))
            if errors:
                for path, error in errors.items():
                    _LOG.error('Failed to format %s', path)
                    for line in error.splitlines():
                        _LOG.error('%s', line)
                all_errors.update(errors)
                continue

            _LOG.info(
                'Formatted %s', plural(files, code_format.language + ' file')
            )
        return all_errors


def _file_summary(files: Iterable[Union[Path, str]], base: Path) -> List[str]:
    try:
        return file_summary(
            Path(f).resolve().relative_to(base.resolve()) for f in files
        )
    except ValueError:
        return []


def format_paths_in_repo(
    paths: Collection[Union[Path, str]],
    exclude: Collection[Pattern[str]],
    fix: bool,
    base: str,
    code_formats: Collection[CodeFormat] = CODE_FORMATS_WITH_YAPF,
    output_directory: Optional[Path] = None,
) -> int:
    """Checks or fixes formatting for files in a Git repo."""

    files = [Path(path).resolve() for path in paths if os.path.isfile(path)]
    repo = git_repo.root() if git_repo.is_repo() else None

    # Implement a graceful fallback in case the tracking branch isn't available.
    if base == git_repo.TRACKING_BRANCH_ALIAS and not git_repo.tracking_branch(
        repo
    ):
        _LOG.warning(
            'Failed to determine the tracking branch, using --base HEAD~1 '
            'instead of listing all files'
        )
        base = 'HEAD~1'

    # If this is a Git repo, list the original paths with git ls-files or diff.
    if repo:
        project_root = Path(pw_cli.env.pigweed_environment().PW_PROJECT_ROOT)
        _LOG.info(
            'Formatting %s',
            git_repo.describe_files(
                repo, Path.cwd(), base, paths, exclude, project_root
            ),
        )

        # Add files from Git and remove duplicates.
        files = sorted(
            set(exclude_paths(exclude, git_repo.list_files(base, paths)))
            | set(files)
        )
    elif base:
        _LOG.critical(
            'A base commit may only be provided if running from a Git repo'
        )
        return 1

    return format_files(
        files,
        fix,
        repo=repo,
        code_formats=code_formats,
        output_directory=output_directory,
    )


def format_files(
    paths: Collection[Union[Path, str]],
    fix: bool,
    repo: Optional[Path] = None,
    code_formats: Collection[CodeFormat] = CODE_FORMATS,
    output_directory: Optional[Path] = None,
) -> int:
    """Checks or fixes formatting for the specified files."""

    if git_repo.is_repo():
        root = git_repo.root()
    elif paths:
        parent = Path(next(iter(paths))).parent
        if git_repo.is_repo(parent):
            root = git_repo.root(parent)

    output_dir: Path
    if output_directory:
        output_dir = output_directory
    elif root:
        output_dir = root / _DEFAULT_PATH
    else:
        tempdir = tempfile.TemporaryDirectory()
        output_dir = Path(tempdir.name)

    formatter = CodeFormatter(
        files=(Path(p) for p in paths),
        code_formats=code_formats,
        root=root,
        output_dir=output_dir,
    )

    _LOG.info('Checking formatting for %s', plural(formatter.paths, 'file'))

    for line in _file_summary(paths, repo if repo else Path.cwd()):
        print(line, file=sys.stderr)

    check_errors = formatter.check()
    print_format_check(check_errors, show_fix_commands=(not fix))

    if check_errors:
        if fix:
            _LOG.info(
                'Applying formatting fixes to %d files', len(check_errors)
            )
            fix_errors = formatter.fix()
            if fix_errors:
                _LOG.info('Failed to apply formatting fixes')
                print_format_check(fix_errors, show_fix_commands=False)
                return 1

            _LOG.info('Formatting fixes applied successfully')
            return 0

        _LOG.error('Formatting errors found')
        return 1

    _LOG.info('Congratulations! No formatting changes needed')
    return 0


def arguments(
    git_paths: bool,
    use_black: bool = False,
) -> argparse.ArgumentParser:
    """Creates an argument parser for format_files or format_paths_in_repo."""

    parser = argparse.ArgumentParser(description=__doc__)

    if git_paths:
        cli.add_path_arguments(parser)
    else:

        def existing_path(arg: str) -> Path:
            path = Path(arg)
            if not path.is_file():
                raise argparse.ArgumentTypeError(
                    f'{arg} is not a path to a file'
                )

            return path

        parser.add_argument(
            'paths',
            metavar='path',
            nargs='+',
            type=existing_path,
            help='File paths to check',
        )

    parser.add_argument(
        '--fix', action='store_true', help='Apply formatting fixes in place.'
    )

    # TODO(b/259595799, b/261025545) Remove --code-formats option when
    # downstream projects have switched away from yapf.
    default_code_formats = CODE_FORMATS_WITH_YAPF
    if use_black:
        default_code_formats = CODE_FORMATS_WITH_BLACK
    parser.add_argument(
        '--code-formats',
        choices=(CODE_FORMATS_WITH_YAPF, CODE_FORMATS_WITH_BLACK),
        default=default_code_formats,
        help=argparse.SUPPRESS,
    )

    parser.add_argument(
        '--output-directory',
        type=Path,
        help=f"Output directory (default: {'<repo root>' / _DEFAULT_PATH})",
    )
    return parser


def main() -> int:
    """Check and fix formatting for source files."""
    return format_paths_in_repo(**vars(arguments(git_paths=True).parse_args()))


def _pigweed_upstream_main() -> int:
    """Check and fix formatting for source files in upstream Pigweed.

    Excludes third party sources.
    """
    args = arguments(git_paths=True, use_black=True).parse_args()

    # Exclude paths with third party code from formatting.
    args.exclude.append(re.compile('^third_party/fuchsia/repo/'))

    return format_paths_in_repo(**vars(args))


if __name__ == '__main__':
    try:
        # If pw_cli is available, use it to initialize logs.
        from pw_cli import log

        log.install(logging.INFO)
    except ImportError:
        # If pw_cli isn't available, display log messages like a simple print.
        logging.basicConfig(format='%(message)s', level=logging.INFO)

    sys.exit(main())
