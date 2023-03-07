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
"""File Helper Functions."""

import glob
import hashlib
import json
import logging
import os
import shutil
import sys
import subprocess
import tarfile
import urllib.request
import zipfile
from pathlib import Path
from typing import List

_LOG = logging.getLogger(__name__)


class InvalidChecksumError(Exception):
    pass


def find_files(
    starting_dir: str, patterns: List[str], directories_only=False
) -> List[str]:
    original_working_dir = os.getcwd()
    if not (os.path.exists(starting_dir) and os.path.isdir(starting_dir)):
        raise FileNotFoundError(
            "Directory '{}' does not exist.".format(starting_dir)
        )

    os.chdir(starting_dir)
    files = []
    for pattern in patterns:
        for file_path in glob.glob(pattern, recursive=True):
            if not directories_only or (
                directories_only and os.path.isdir(file_path)
            ):
                files.append(file_path)
    os.chdir(original_working_dir)
    return sorted(files)


def sha256_sum(file_name):
    hash_sha256 = hashlib.sha256()
    with open(file_name, "rb") as file_handle:
        for chunk in iter(lambda: file_handle.read(4096), b""):
            hash_sha256.update(chunk)
    return hash_sha256.hexdigest()


def md5_sum(file_name):
    hash_md5 = hashlib.md5()
    with open(file_name, "rb") as file_handle:
        for chunk in iter(lambda: file_handle.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()


def verify_file_checksum(file_path, expected_checksum, sum_function=sha256_sum):
    downloaded_checksum = sum_function(file_path)
    if downloaded_checksum != expected_checksum:
        raise InvalidChecksumError(
            f"Invalid {sum_function.__name__}\n"
            f"{downloaded_checksum} {os.path.basename(file_path)}\n"
            f"{expected_checksum} (expected)\n\n"
            "Please delete this file and try again:\n"
            f"{file_path}"
        )

    _LOG.debug("  %s:", sum_function.__name__)
    _LOG.debug("  %s %s", downloaded_checksum, os.path.basename(file_path))
    return True


def relative_or_absolute_path(file_string: str):
    """Return a Path relative to os.getcwd(), else an absolute path."""
    file_path = Path(file_string)
    try:
        return file_path.relative_to(os.getcwd())
    except ValueError:
        return file_path.resolve()


def download_to_cache(
    url: str,
    expected_md5sum=None,
    expected_sha256sum=None,
    cache_directory=".cache",
    downloaded_file_name=None,
) -> str:
    """TODO(tonymd) Add docstring."""

    cache_dir = os.path.realpath(
        os.path.expanduser(os.path.expandvars(cache_directory))
    )
    if not downloaded_file_name:
        # Use the last part of the URL as the file name.
        downloaded_file_name = url.split("/")[-1]
    downloaded_file = os.path.join(cache_dir, downloaded_file_name)

    if not os.path.exists(downloaded_file):
        _LOG.info("Downloading: %s", url)
        _LOG.info("Please wait...")
        urllib.request.urlretrieve(url, filename=downloaded_file)

    if os.path.exists(downloaded_file):
        _LOG.info("Downloaded: %s", relative_or_absolute_path(downloaded_file))
        if expected_sha256sum:
            verify_file_checksum(
                downloaded_file, expected_sha256sum, sum_function=sha256_sum
            )
        elif expected_md5sum:
            verify_file_checksum(
                downloaded_file, expected_md5sum, sum_function=md5_sum
            )

    return downloaded_file


def extract_zipfile(archive_file: str, dest_dir: str):
    """Extract a zipfile preseving permissions."""
    destination_path = Path(dest_dir)
    with zipfile.ZipFile(archive_file) as archive:
        for info in archive.infolist():
            archive.extract(info.filename, path=dest_dir)
            permissions = info.external_attr >> 16
            out_path = destination_path / info.filename
            out_path.chmod(permissions)


def extract_tarfile(archive_file: str, dest_dir: str):
    with tarfile.open(archive_file, 'r') as archive:
        archive.extractall(path=dest_dir)


def extract_archive(
    archive_file: str,
    dest_dir: str,
    cache_dir: str,
    remove_single_toplevel_folder=True,
):
    """Extract a tar or zip file.

    Args:
        archive_file (str): Absolute path to the archive file.
        dest_dir (str): Extraction destination directory.
        cache_dir (str): Directory where temp files can be created.
        remove_single_toplevel_folder (bool): If the archive contains only a
            single folder move the contents of that into the destination
            directory.
    """
    # Make a temporary directory to extract files into
    temp_extract_dir = os.path.join(
        cache_dir, "." + os.path.basename(archive_file)
    )
    os.makedirs(temp_extract_dir, exist_ok=True)

    _LOG.info("Extracting: %s", relative_or_absolute_path(archive_file))
    if zipfile.is_zipfile(archive_file):
        extract_zipfile(archive_file, temp_extract_dir)
    elif tarfile.is_tarfile(archive_file):
        extract_tarfile(archive_file, temp_extract_dir)
    else:
        _LOG.error("Unknown archive format: %s", archive_file)
        return sys.exit(1)

    _LOG.info("Installing into: %s", relative_or_absolute_path(dest_dir))
    path_to_extracted_files = temp_extract_dir

    extracted_top_level_files = os.listdir(temp_extract_dir)
    # Check if tarfile has only one folder
    # If yes, make that the new path_to_extracted_files
    if remove_single_toplevel_folder and len(extracted_top_level_files) == 1:
        path_to_extracted_files = os.path.join(
            temp_extract_dir, extracted_top_level_files[0]
        )

    # Move extracted files to dest_dir
    extracted_files = os.listdir(path_to_extracted_files)
    for file_name in extracted_files:
        source_file = os.path.join(path_to_extracted_files, file_name)
        dest_file = os.path.join(dest_dir, file_name)
        shutil.move(source_file, dest_file)

    # rm -rf temp_extract_dir
    shutil.rmtree(temp_extract_dir, ignore_errors=True)

    # Return List of extracted files
    return list(Path(dest_dir).rglob("*"))


def remove_empty_directories(directory):
    """Recursively remove empty directories."""

    for path in sorted(Path(directory).rglob("*"), reverse=True):
        # If broken symlink
        if path.is_symlink() and not path.exists():
            path.unlink()
        # if empty directory
        elif path.is_dir() and len(os.listdir(path)) == 0:
            path.rmdir()


def decode_file_json(file_name):
    """Decode JSON values from a file.

    Does not raise an error if the file cannot be decoded."""

    # Get absolute path to the file.
    file_path = os.path.realpath(
        os.path.expanduser(os.path.expandvars(file_name))
    )

    json_file_options = {}
    try:
        with open(file_path, "r") as jfile:
            json_file_options = json.loads(jfile.read())
    except (FileNotFoundError, json.JSONDecodeError):
        _LOG.warning("Unable to read file '%s'", file_path)

    return json_file_options, file_path


def git_apply_patch(
    root_directory, patch_file, ignore_whitespace=True, unsafe_paths=False
):
    """Use `git apply` to apply a diff file."""

    _LOG.info("Applying Patch: %s", patch_file)
    git_apply_command = ["git", "apply"]
    if ignore_whitespace:
        git_apply_command.append("--ignore-whitespace")
    if unsafe_paths:
        git_apply_command.append("--unsafe-paths")
    git_apply_command += ["--directory", root_directory, patch_file]
    subprocess.run(git_apply_command)
