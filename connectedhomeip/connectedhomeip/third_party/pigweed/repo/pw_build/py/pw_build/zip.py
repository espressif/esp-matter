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
"""Takes a set of input files and zips them up."""

import argparse
import pathlib
import sys
import zipfile

from collections.abc import Iterable

DEFAULT_DELIMITER = '>'


class ZipError(Exception):
    """Raised when a pw_zip archive can't be built as specified."""


def _parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--delimiter',
        nargs='?',
        default=DEFAULT_DELIMITER,
        help='Symbol that separates the path and the zip path destination.',
    )
    parser.add_argument(
        '--input_list',
        nargs='+',
        help='Paths to files and dirs to zip and their desired zip location.',
    )
    parser.add_argument('--out_filename', help='Zip file destination.')

    return parser.parse_args()


def zip_up(
    input_list: Iterable, out_filename: str, delimiter=DEFAULT_DELIMITER
):
    """Zips up all input files/dirs.

    Args:
        input_list: List of strings consisting of file or directory,
            the delimiter, and a path to the desired .zip destination.
        out_filename: Path and name of the .zip file.
        delimiter: string that separates the input source and the zip
            destination. Defaults to '>'. Examples:
            '/foo.txt > /'         # /foo.txt zipped as /foo.txt
            '/foo.txt > /bar.txt'  # /foo.txt zipped as /bar.txt
            'foo.txt > /'  # foo.txt from invokers dir zipped as /foo.txt
            '/bar/ > /'            # Whole bar dir zipped into /
    """
    with zipfile.ZipFile(out_filename, 'w', zipfile.ZIP_DEFLATED) as zip_file:
        for _input in input_list:
            try:
                source, destination = _input.split(delimiter)
                source = source.strip()
                destination = destination.strip()
            except ValueError as value_error:
                msg = (
                    f'Input in the form of "[filename or dir] {delimiter} '
                    f'/zip_destination/" expected. Instead got:\n  {_input}'
                )
                raise ZipError(msg) from value_error
            if not source:
                raise ZipError(
                    f'Bad input:\n  {_input}\nInput source '
                    f'cannot be empty. Please specify the input in the form '
                    f'of "[filename or dir] {delimiter} /zip_destination/".'
                )
            if not destination.startswith('/'):
                raise ZipError(
                    f'Bad input:\n  {_input}\nZip desination '
                    f'"{destination}" must start with "/" to indicate the '
                    f'zip file\'s root directory.'
                )
            source_path = pathlib.Path(source)
            destination_path = pathlib.PurePath(destination)

            # Case: the input source path points to a file.
            if source_path.is_file():
                # Case: "foo.txt > /mydir/"; destination is dir. Put foo.txt
                # into mydir as /mydir/foo.txt
                if destination.endswith('/'):
                    zip_file.write(
                        source_path, destination_path / source_path.name
                    )
                # Case: "foo.txt > /bar.txt"; destination is a file--rename the
                # source file: put foo.txt into the zip as /bar.txt
                else:
                    zip_file.write(source_path, destination_path)
                continue
            # Case: the input source path points to a directory.
            if source_path.is_dir():
                zip_up_dir(
                    source, source_path, destination, destination_path, zip_file
                )
                continue
            raise ZipError(f'Unknown source path\n  {source_path}')


def zip_up_dir(
    source: str,
    source_path: pathlib.Path,
    destination: str,
    destination_path: pathlib.PurePath,
    zip_file: zipfile.ZipFile,
):
    if not source.endswith('/'):
        raise ZipError(
            f'Source path:\n  {source}\nis a directory, but is '
            f'missing a trailing "/". The / requirement helps prevent bugs. '
            f'To fix, add a trailing /:\n  {source}/'
        )
    if not destination.endswith('/'):
        raise ZipError(
            f'Destination path:\n  {destination}\nis a directory, '
            f'but is missing a trailing "/". The / requirement helps prevent '
            f'bugs. To fix, add a trailing /:\n  {destination}/'
        )

    # Walk the directory and add zip all of the files with the
    # same structure as the source.
    for file_path in source_path.glob('**/*'):
        if file_path.is_file():
            rel_path = file_path.relative_to(source_path)
            zip_file.write(file_path, destination_path / rel_path)


def main():
    zip_up(**vars(_parse_args()))


if __name__ == '__main__':
    try:
        main()
    except ZipError as err:
        print('ERROR:', str(err), file=sys.stderr)
        sys.exit(1)
