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
"""Generate and serialize update bundles."""

import argparse
import logging
import os
from pathlib import Path
import shutil
from typing import Dict, Iterable, Optional, Tuple

from pw_software_update import metadata
from pw_software_update.tuf_pb2 import SignedRootMetadata, SignedTargetsMetadata
from pw_software_update.update_bundle_pb2 import UpdateBundle

_LOG = logging.getLogger(__package__)


def targets_from_directory(
    root_dir: Path,
    exclude: Iterable[Path] = tuple(),
    remap_paths: Optional[Dict[Path, str]] = None,
) -> Dict[str, Path]:
    """Given a directory on dist, generate a dict of target names to files.

    Args:
      root_dir: Directory to crawl for targets.
      exclude: Paths relative to root_dir to exclude as targets.
      remap_paths: Custom target names to use for targets.

    Each file in the input directory will be read in as a target file, unless
    its path (relative to the TUF repo root) is among the excludes.

    Default behavior is to treat root_dir-relative paths as the strings to use
    as targets file names, but remapping can be used to change a target file
    name to any string. If some remappings are provided but a file is found that
    does not have a remapping, a warning will be logged. If a remap is declared
    for a file that does not exist, FileNotFoundError will be raised.
    """
    if not root_dir.is_dir():
        raise ValueError(
            f'Cannot generate TUF targets from {root_dir}; not a directory.'
        )
    targets = {}
    for path in root_dir.glob('**/*'):
        if path.is_dir():
            continue
        rel_path = path.relative_to(root_dir)
        if rel_path in exclude:
            continue
        target_name = str(rel_path.as_posix())
        if remap_paths:
            if rel_path in remap_paths:
                target_name = remap_paths[rel_path]
            else:
                _LOG.warning('Some remaps defined, but not "%s"', target_name)
        targets[target_name] = path

    if remap_paths is not None:
        for original_path, new_target_file_name in remap_paths.items():
            if new_target_file_name not in targets:
                raise FileNotFoundError(
                    f'Unable to remap "{original_path}" to'
                    f' "{new_target_file_name}"; file not found in root dir.'
                )

    return targets


def gen_empty_update_bundle(
    targets_metadata_version: int = metadata.DEFAULT_METADATA_VERSION,
) -> UpdateBundle:
    """Generates an empty bundle

    Given an optional target metadata version, generates an empty bundle.

    Args:
      targets_metadata_version: default set to 1

    Returns:
      UpdateBundle: empty bundle
    """

    targets_metadata = metadata.gen_targets_metadata(
        target_payloads={}, version=targets_metadata_version
    )
    unsigned_targets_metadata = SignedTargetsMetadata(
        serialized_targets_metadata=targets_metadata.SerializeToString()
    )

    return UpdateBundle(
        root_metadata=None,
        targets_metadata=dict(targets=unsigned_targets_metadata),
        target_payloads=None,
    )


def gen_unsigned_update_bundle(
    targets: Dict[Path, str],
    persist: Optional[Path] = None,
    targets_metadata_version: int = metadata.DEFAULT_METADATA_VERSION,
    root_metadata: Optional[SignedRootMetadata] = None,
) -> UpdateBundle:
    """Given a set of targets, generates an unsigned UpdateBundle.

    Args:
      targets: A dict mapping payload Paths to their target names.
      persist: If not None, persist the raw TUF repository to this directory.
      targets_metadata_version: version number for the targets metadata.
      root_metadata: Optional signed Root metadata.

    The input targets will be treated as an ephemeral TUF repository for the
    purposes of building an UpdateBundle instance. This approach differs
    slightly from the normal concept of a TUF repository, which is typically a
    directory on disk. For ease in debugging raw repository contents, the
    `persist` argument can be supplied. If a persist Path is supplied, the TUF
    repository will be persisted to disk at that location.

    NOTE: If path separator characters (like '/') are used in target names, then
    persisting the repository to disk via the 'persist' argument will create the
    corresponding directory structure.

    NOTE: If a root metadata is included, the client is expected to first
    upgrade its on-device trusted root metadata before verifying the rest of
    the bundle.
    """
    if persist:
        if persist.exists() and not persist.is_dir():
            raise ValueError(
                f'TUF repo cannot be persisted to "{persist}";'
                ' file exists and is not a directory.'
            )
        if persist.exists():
            shutil.rmtree(persist)

        os.makedirs(persist)

    target_payloads = {}
    for path, target_name in targets.items():
        target_payloads[target_name] = path.read_bytes()
        if persist:
            target_persist_path = persist / target_name
            os.makedirs(target_persist_path.parent, exist_ok=True)
            shutil.copy(path, target_persist_path)

    targets_metadata = metadata.gen_targets_metadata(
        target_payloads, version=targets_metadata_version
    )
    unsigned_targets_metadata = SignedTargetsMetadata(
        serialized_targets_metadata=targets_metadata.SerializeToString()
    )

    return UpdateBundle(
        root_metadata=root_metadata,
        targets_metadata=dict(targets=unsigned_targets_metadata),
        target_payloads=target_payloads,
    )


def parse_target_arg(target_arg: str) -> Tuple[Path, str]:
    """Parse an individual target string passed in to the --targets argument.

    Target strings take the following form:
      "FILE_PATH > TARGET_NAME"

    For example:
      "fw_images/main_image.bin > main"
    """
    try:
        file_path_str, target_name = target_arg.split('>')
        return Path(file_path_str.strip()), target_name.strip()
    except ValueError as err:
        raise ValueError(
            'Targets must be strings of the form:\n'
            '  "FILE_PATH > TARGET_NAME"'
        ) from err


def parse_args() -> argparse.Namespace:
    """Parse CLI arguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '-t',
        '--targets',
        type=str,
        nargs='+',
        required=True,
        help='Strings defining targets to bundle',
    )
    parser.add_argument(
        '-o',
        '--out',
        type=Path,
        required=True,
        help='Output path for serialized UpdateBundle',
    )
    parser.add_argument(
        '--persist',
        type=Path,
        default=None,
        help=(
            'If provided, TUF repo will be persisted to disk'
            ' at this path for debugging'
        ),
    )
    parser.add_argument(
        '--targets-metadata-version',
        type=int,
        default=metadata.DEFAULT_METADATA_VERSION,
        help='Version number for the targets metadata',
    )
    parser.add_argument(
        '--targets-metadata-version-file',
        type=Path,
        default=None,
        help='Read version number string from this file. When '
        'provided, content of this file supersede '
        '--targets-metadata-version',
    )
    parser.add_argument(
        '--signed-root-metadata',
        type=Path,
        default=None,
        help='Path to the signed Root metadata',
    )
    return parser.parse_args()


def main(
    targets: Iterable[str],
    out: Path,
    persist: Optional[Path] = None,
    targets_metadata_version: int = metadata.DEFAULT_METADATA_VERSION,
    targets_metadata_version_file: Optional[Path] = None,
    signed_root_metadata: Optional[Path] = None,
) -> None:
    """Generates an UpdateBundle and serializes it to disk."""
    target_dict = {}
    for target_arg in targets:
        path, target_name = parse_target_arg(target_arg)
        target_dict[path] = target_name

    root_metadata = None
    if signed_root_metadata:
        root_metadata = SignedRootMetadata.FromString(
            signed_root_metadata.read_bytes()
        )

    if targets_metadata_version_file:
        with targets_metadata_version_file.open() as version_file:
            targets_metadata_version = int(version_file.read().strip())

    bundle = gen_unsigned_update_bundle(
        target_dict, persist, targets_metadata_version, root_metadata
    )

    out.write_bytes(bundle.SerializeToString())


if __name__ == '__main__':
    logging.basicConfig()
    main(**vars(parse_args()))
