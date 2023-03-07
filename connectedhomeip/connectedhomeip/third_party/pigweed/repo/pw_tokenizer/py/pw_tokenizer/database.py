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
"""Creates and manages token databases.

This module manages reading tokenized strings from ELF files and building and
maintaining token databases.
"""

import argparse
from datetime import datetime
import glob
import itertools
import json
import logging
import os
from pathlib import Path
import re
import struct
import sys
from typing import (
    Any,
    Callable,
    Dict,
    Iterable,
    Iterator,
    List,
    Optional,
    Pattern,
    Set,
    TextIO,
    Tuple,
    Union,
)

try:
    from pw_tokenizer import elf_reader, tokens
except ImportError:
    # Append this path to the module search path to allow running this module
    # without installing the pw_tokenizer package.
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from pw_tokenizer import elf_reader, tokens

_LOG = logging.getLogger('pw_tokenizer')


def _elf_reader(elf) -> elf_reader.Elf:
    return elf if isinstance(elf, elf_reader.Elf) else elf_reader.Elf(elf)


# Magic number used to indicate the beginning of a tokenized string entry. This
# value MUST match the value of _PW_TOKENIZER_ENTRY_MAGIC in
# pw_tokenizer/public/pw_tokenizer/internal/tokenize_string.h.
_TOKENIZED_ENTRY_MAGIC = 0xBAA98DEE
_ENTRY = struct.Struct('<4I')
_TOKENIZED_ENTRY_SECTIONS = re.compile(r'^\.pw_tokenizer.entries(?:\.[_\d]+)?$')

_ERROR_HANDLER = 'surrogateescape'  # How to deal with UTF-8 decoding errors


class Error(Exception):
    """Failed to extract token entries from an ELF file."""


def _read_tokenized_entries(
    data: bytes, domain: Pattern[str]
) -> Iterator[tokens.TokenizedStringEntry]:
    index = 0

    while index + _ENTRY.size <= len(data):
        magic, token, domain_len, string_len = _ENTRY.unpack_from(data, index)

        if magic != _TOKENIZED_ENTRY_MAGIC:
            raise Error(
                f'Expected magic number 0x{_TOKENIZED_ENTRY_MAGIC:08x}, '
                f'found 0x{magic:08x}'
            )

        start = index + _ENTRY.size
        index = start + domain_len + string_len

        # Create the entries, trimming null terminators.
        entry = tokens.TokenizedStringEntry(
            token,
            data[start + domain_len : index - 1].decode(errors=_ERROR_HANDLER),
            data[start : start + domain_len - 1].decode(errors=_ERROR_HANDLER),
        )

        if data[start + domain_len - 1] != 0:
            raise Error(
                f'Domain {entry.domain} for {entry.string} not null terminated'
            )

        if data[index - 1] != 0:
            raise Error(f'String {entry.string} is not null terminated')

        if domain.fullmatch(entry.domain):
            yield entry


def _database_from_elf(elf, domain: Pattern[str]) -> tokens.Database:
    """Reads the tokenized strings from an elf_reader.Elf or ELF file object."""
    _LOG.debug('Reading tokenized strings in domain "%s" from %s', domain, elf)

    reader = _elf_reader(elf)

    # Read tokenized string entries.
    section_data = reader.dump_section_contents(_TOKENIZED_ENTRY_SECTIONS)
    if section_data is not None:
        return tokens.Database(_read_tokenized_entries(section_data, domain))

    return tokens.Database([])


def tokenization_domains(elf) -> Iterator[str]:
    """Lists all tokenization domains in an ELF file."""
    reader = _elf_reader(elf)
    section_data = reader.dump_section_contents(_TOKENIZED_ENTRY_SECTIONS)
    if section_data is not None:
        yield from frozenset(
            e.domain
            for e in _read_tokenized_entries(section_data, re.compile('.*'))
        )


def read_tokenizer_metadata(elf) -> Dict[str, int]:
    """Reads the metadata entries from an ELF."""
    sections = _elf_reader(elf).dump_section_contents(r'\.pw_tokenizer\.info')

    metadata: Dict[str, int] = {}
    if sections is not None:
        for key, value in struct.iter_unpack('12sI', sections):
            try:
                metadata[key.rstrip(b'\0').decode()] = value
            except UnicodeDecodeError as err:
                _LOG.error(
                    'Failed to decode metadata key %r: %s',
                    key.rstrip(b'\0'),
                    err,
                )

    return metadata


def _database_from_strings(strings: List[str]) -> tokens.Database:
    """Generates a C and C++ compatible database from untokenized strings."""
    # Generate a C-compatible database from the fixed length hash.
    c_db = tokens.Database.from_strings(strings, tokenize=tokens.c_hash)

    # Generate a C++ compatible database by allowing the hash to follow the
    # string length.
    cpp_db = tokens.Database.from_strings(
        strings, tokenize=tokens.pw_tokenizer_65599_hash
    )

    # Use a union of the C and C++ compatible databases.
    return tokens.Database.merged(c_db, cpp_db)


def _database_from_json(fd) -> tokens.Database:
    return _database_from_strings(json.load(fd))


def _load_token_database(  # pylint: disable=too-many-return-statements
    db, domain: Pattern[str]
) -> tokens.Database:
    """Loads a Database from supported database types.

    Supports Database objects, JSONs, ELFs, CSVs, and binary databases.
    """
    if db is None:
        return tokens.Database()

    if isinstance(db, tokens.Database):
        return db

    if isinstance(db, elf_reader.Elf):
        return _database_from_elf(db, domain)

    # If it's a str, it might be a path. Check if it's an ELF, CSV, or JSON.
    if isinstance(db, (str, Path)):
        if not os.path.exists(db):
            raise FileNotFoundError(f'"{db}" is not a path to a token database')

        if Path(db).is_dir():
            return tokens.DatabaseFile.load(Path(db))

        # Read the path as an ELF file.
        with open(db, 'rb') as fd:
            if elf_reader.compatible_file(fd):
                return _database_from_elf(fd, domain)

        # Generate a database from JSON.
        if str(db).endswith('.json'):
            with open(db, 'r') as json_fd:
                return _database_from_json(json_fd)

        # Read the path as a packed binary or CSV file.
        return tokens.DatabaseFile.load(Path(db))

    # Assume that it's a file object and check if it's an ELF.
    if elf_reader.compatible_file(db):
        return _database_from_elf(db, domain)

    # Read the database as JSON, CSV, or packed binary from a file object's
    # path.
    if hasattr(db, 'name') and os.path.exists(db.name):
        if db.name.endswith('.json'):
            return _database_from_json(db)

        return tokens.DatabaseFile.load(Path(db.name))

    # Read CSV directly from the file object.
    return tokens.Database(tokens.parse_csv(db))


def load_token_database(
    *databases, domain: Union[str, Pattern[str]] = tokens.DEFAULT_DOMAIN
) -> tokens.Database:
    """Loads a Database from supported database types.

    Supports Database objects, JSONs, ELFs, CSVs, and binary databases.
    """
    domain = re.compile(domain)
    return tokens.Database.merged(
        *(_load_token_database(db, domain) for db in databases)
    )


def database_summary(db: tokens.Database) -> Dict[str, Any]:
    """Returns a simple report of properties of the database."""
    present = [entry for entry in db.entries() if not entry.date_removed]
    collisions = {
        token: list(e.string for e in entries)
        for token, entries in db.collisions()
    }

    # Add 1 to each string's size to account for the null terminator.
    return dict(
        present_entries=len(present),
        present_size_bytes=sum(len(entry.string) + 1 for entry in present),
        total_entries=len(db.entries()),
        total_size_bytes=sum(len(entry.string) + 1 for entry in db.entries()),
        collisions=collisions,
    )


_DatabaseReport = Dict[str, Dict[str, Dict[str, Any]]]


def generate_reports(paths: Iterable[Path]) -> _DatabaseReport:
    """Returns a dictionary with information about the provided databases."""
    reports: _DatabaseReport = {}

    for path in paths:
        domains = ['']
        if path.is_file():
            with path.open('rb') as file:
                if elf_reader.compatible_file(file):
                    domains = list(tokenization_domains(file))

        domain_reports = {}

        for domain in domains:
            domain_reports[domain] = database_summary(
                load_token_database(path, domain=domain)
            )

        reports[str(path)] = domain_reports

    return reports


def _handle_create(
    databases,
    database: Path,
    force: bool,
    output_type: str,
    include: list,
    exclude: list,
    replace: list,
) -> None:
    """Creates a token database file from one or more ELF files."""
    if not force and database.exists():
        raise FileExistsError(
            f'The file {database} already exists! Use --force to overwrite.'
        )

    if output_type == 'directory':
        if str(database) == '-':
            raise ValueError(
                'Cannot specify "-" (stdout) for directory databases'
            )

        database.mkdir(exist_ok=True)
        database = database / f'database{tokens.DIR_DB_SUFFIX}'
        output_type = 'csv'

    if str(database) == '-':
        # Must write bytes to stdout; use sys.stdout.buffer.
        fd = sys.stdout.buffer
    else:
        fd = database.open('wb')

    db = tokens.Database.merged(*databases)
    db.filter(include, exclude, replace)

    with fd:
        if output_type == 'csv':
            tokens.write_csv(db, fd)
        elif output_type == 'binary':
            tokens.write_binary(db, fd)
        else:
            raise ValueError(f'Unknown database type "{output_type}"')

    _LOG.info(
        'Wrote database with %d entries to %s as %s',
        len(db),
        fd.name,
        output_type,
    )


def _handle_add(
    token_database: tokens.DatabaseFile,
    databases: List[tokens.Database],
    commit: Optional[str],
) -> None:
    initial = len(token_database)
    if commit:
        entries = itertools.chain.from_iterable(
            db.entries() for db in databases
        )
        token_database.add_and_discard_temporary(entries, commit)
    else:
        for source in databases:
            token_database.add(source.entries())

        token_database.write_to_file()

    number_of_changes = len(token_database) - initial

    if number_of_changes:
        _LOG.info(
            'Added %d entries to %s', number_of_changes, token_database.path
        )


def _handle_mark_removed(
    token_database: tokens.DatabaseFile,
    databases: List[tokens.Database],
    date: Optional[datetime],
):
    marked_removed = token_database.mark_removed(
        (
            entry
            for entry in tokens.Database.merged(*databases).entries()
            if not entry.date_removed
        ),
        date,
    )

    token_database.write_to_file(rewrite=True)

    _LOG.info(
        'Marked %d of %d entries as removed in %s',
        len(marked_removed),
        len(token_database),
        token_database.path,
    )


def _handle_purge(
    token_database: tokens.DatabaseFile, before: Optional[datetime]
):
    purged = token_database.purge(before)
    token_database.write_to_file(rewrite=True)

    _LOG.info('Removed %d entries from %s', len(purged), token_database.path)


def _handle_report(token_database_or_elf: List[Path], output: TextIO) -> None:
    json.dump(generate_reports(token_database_or_elf), output, indent=2)
    output.write('\n')


def expand_paths_or_globs(*paths_or_globs: str) -> Iterable[Path]:
    """Expands any globs in a list of paths; raises FileNotFoundError."""
    for path_or_glob in paths_or_globs:
        if os.path.exists(path_or_glob):
            # This is a valid path; yield it without evaluating it as a glob.
            yield Path(path_or_glob)
        else:
            paths = glob.glob(path_or_glob, recursive=True)

            # If no paths were found and the path is not a glob, raise an Error.
            if not paths and not any(c in path_or_glob for c in '*?[]!'):
                raise FileNotFoundError(f'{path_or_glob} is not a valid path')

            for path in paths:
                # Resolve globs to JSON, CSV, or compatible binary files.
                if elf_reader.compatible_file(path) or path.endswith(
                    ('.csv', '.json')
                ):
                    yield Path(path)


class ExpandGlobs(argparse.Action):
    """Argparse action that expands and appends paths."""

    def __call__(self, parser, namespace, values, unused_option_string=None):
        setattr(namespace, self.dest, list(expand_paths_or_globs(*values)))


def _read_elf_with_domain(
    elf: str, domain: Pattern[str]
) -> Iterable[tokens.Database]:
    for path in expand_paths_or_globs(elf):
        with path.open('rb') as file:
            if not elf_reader.compatible_file(file):
                raise ValueError(
                    f'{elf} is not an ELF file, '
                    f'but the "{domain}" domain was specified'
                )

            yield _database_from_elf(file, domain)


class LoadTokenDatabases(argparse.Action):
    """Argparse action that reads tokenize databases from paths or globs.

    ELF files may have #domain appended to them to specify a tokenization domain
    other than the default.
    """

    def __call__(self, parser, namespace, values, option_string=None):
        databases: List[tokens.Database] = []
        paths: Set[Path] = set()

        try:
            for value in values:
                if value.count('#') == 1:
                    path, domain = value.split('#')
                    domain = re.compile(domain)
                    databases.extend(_read_elf_with_domain(path, domain))
                else:
                    paths.update(expand_paths_or_globs(value))

            for path in paths:
                databases.append(load_token_database(path))
        except tokens.DatabaseFormatError as err:
            parser.error(
                f'argument elf_or_token_database: {path} is not a supported '
                'token database file. Only ELF files or token databases (CSV '
                f'or binary format) are supported. {err}. '
            )
        except FileNotFoundError as err:
            parser.error(f'argument elf_or_token_database: {err}')
        except:  # pylint: disable=bare-except
            _LOG.exception('Failed to load token database %s', path)
            parser.error(
                'argument elf_or_token_database: '
                f'Error occurred while loading token database {path}'
            )

        setattr(namespace, self.dest, databases)


def token_databases_parser(nargs: str = '+') -> argparse.ArgumentParser:
    """Returns an argument parser for reading token databases.

    These arguments can be added to another parser using the parents arg.
    """
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument(
        'databases',
        metavar='elf_or_token_database',
        nargs=nargs,
        action=LoadTokenDatabases,
        help=(
            'ELF or token database files from which to read strings and '
            'tokens. For ELF files, the tokenization domain to read from '
            'may specified after the path as #domain_name (e.g. '
            'foo.elf#TEST_DOMAIN). Unless specified, only the default '
            'domain ("") is read from ELF files; .* reads all domains. '
            'Globs are expanded to compatible database files.'
        ),
    )
    return parser


def _parse_args():
    """Parse and return command line arguments."""

    def year_month_day(value) -> datetime:
        if value == 'today':
            return datetime.now()

        return datetime.strptime(value, tokens.DATE_FORMAT)

    year_month_day.__name__ = 'year-month-day (YYYY-MM-DD)'

    # Shared command line options.
    option_db = argparse.ArgumentParser(add_help=False)
    option_db.add_argument(
        '-d',
        '--database',
        dest='token_database',
        type=lambda arg: tokens.DatabaseFile.load(Path(arg)),
        required=True,
        help='The database file to update.',
    )

    option_tokens = token_databases_parser('*')

    # Top-level argument parser.
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.set_defaults(handler=lambda **_: parser.print_help())

    subparsers = parser.add_subparsers(
        help='Tokenized string database management actions:'
    )

    # The 'create' command creates a database file.
    subparser = subparsers.add_parser(
        'create',
        parents=[option_tokens],
        help=(
            'Creates a database with tokenized strings from one or more '
            'sources.'
        ),
    )
    subparser.set_defaults(handler=_handle_create)
    subparser.add_argument(
        '-d',
        '--database',
        required=True,
        type=Path,
        help='Path to the database file to create; use - for stdout.',
    )
    subparser.add_argument(
        '-t',
        '--type',
        dest='output_type',
        choices=('csv', 'binary', 'directory'),
        default='csv',
        help='Which type of database to create. (default: csv)',
    )
    subparser.add_argument(
        '-f',
        '--force',
        action='store_true',
        help='Overwrite the database if it exists.',
    )
    subparser.add_argument(
        '-i',
        '--include',
        type=re.compile,
        default=[],
        action='append',
        help=(
            'If provided, at least one of these regular expressions must '
            'match for a string to be included in the database.'
        ),
    )
    subparser.add_argument(
        '-e',
        '--exclude',
        type=re.compile,
        default=[],
        action='append',
        help=(
            'If provided, none of these regular expressions may match for a '
            'string to be included in the database.'
        ),
    )

    unescaped_slash = re.compile(r'(?<!\\)/')

    def replacement(value: str) -> Tuple[Pattern, 'str']:
        try:
            find, sub = unescaped_slash.split(value, 1)
        except ValueError as err:
            raise argparse.ArgumentTypeError(
                'replacements must be specified as "search_regex/replacement"'
            )

        try:
            return re.compile(find.replace(r'\/', '/')), sub
        except re.error as err:
            raise argparse.ArgumentTypeError(
                f'"{value}" is not a valid regular expression: {err}'
            )

    subparser.add_argument(
        '--replace',
        type=replacement,
        default=[],
        action='append',
        help=(
            'If provided, replaces text that matches a regular expression. '
            'This can be used to replace sensitive terms in a token '
            'database that will be distributed publicly. The expression and '
            'replacement are specified as "search_regex/replacement". '
            'Plain slash characters in the regex must be escaped with a '
            r'backslash (\/). The replacement text may include '
            'backreferences for captured groups in the regex.'
        ),
    )

    # The 'add' command adds strings to a database from a set of ELFs.
    subparser = subparsers.add_parser(
        'add',
        parents=[option_db, option_tokens],
        help=(
            'Adds new strings to a database with tokenized strings from a set '
            'of ELF files or other token databases. Missing entries are NOT '
            'marked as removed.'
        ),
    )
    subparser.set_defaults(handler=_handle_add)
    subparser.add_argument(
        '--discard-temporary',
        dest='commit',
        help=(
            'Deletes temporary tokens in memory and on disk when a CSV exists '
            'within a commit. Afterwards, new strings are added to the '
            'database from a set of ELF files or other token databases. '
            'Missing entries are NOT marked as removed.'
        ),
    )

    # The 'mark_removed' command marks removed entries to match a set of ELFs.
    subparser = subparsers.add_parser(
        'mark_removed',
        parents=[option_db, option_tokens],
        help=(
            'Updates a database with tokenized strings from a set of strings. '
            'Strings not present in the set remain in the database but are '
            'marked as removed. New strings are NOT added.'
        ),
    )
    subparser.set_defaults(handler=_handle_mark_removed)
    subparser.add_argument(
        '--date',
        type=year_month_day,
        help=(
            'The removal date to use for all strings. '
            'May be YYYY-MM-DD or "today". (default: today)'
        ),
    )

    # The 'purge' command removes old entries.
    subparser = subparsers.add_parser(
        'purge',
        parents=[option_db],
        help='Purges removed strings from a database.',
    )
    subparser.set_defaults(handler=_handle_purge)
    subparser.add_argument(
        '-b',
        '--before',
        type=year_month_day,
        help=(
            'Delete all entries removed on or before this date. '
            'May be YYYY-MM-DD or "today".'
        ),
    )

    # The 'report' command prints a report about a database.
    subparser = subparsers.add_parser(
        'report', help='Prints a report about a database.'
    )
    subparser.set_defaults(handler=_handle_report)
    subparser.add_argument(
        'token_database_or_elf',
        nargs='+',
        action=ExpandGlobs,
        help=(
            'The ELF files or token databases about which to generate '
            'reports.'
        ),
    )
    subparser.add_argument(
        '-o',
        '--output',
        type=argparse.FileType('w'),
        default=sys.stdout,
        help='The file to which to write the output; use - for stdout.',
    )

    args = parser.parse_args()

    handler = args.handler
    del args.handler

    return handler, args


def _init_logging(level: int) -> None:
    _LOG.setLevel(logging.DEBUG)
    log_to_stderr = logging.StreamHandler()
    log_to_stderr.setLevel(level)
    log_to_stderr.setFormatter(
        logging.Formatter(
            fmt='%(asctime)s.%(msecs)03d-%(levelname)s: %(message)s',
            datefmt='%H:%M:%S',
        )
    )

    _LOG.addHandler(log_to_stderr)


def _main(handler: Callable, args: argparse.Namespace) -> int:
    _init_logging(logging.INFO)
    handler(**vars(args))
    return 0


if __name__ == '__main__':
    sys.exit(_main(*_parse_args()))
