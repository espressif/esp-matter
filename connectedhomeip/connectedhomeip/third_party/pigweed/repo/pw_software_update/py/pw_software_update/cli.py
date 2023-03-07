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
"""
Software update related operations.

Learn more at: pigweed.dev/pw_software_update

"""

import argparse
import os
import sys
from pathlib import Path

from pw_software_update import (
    dev_sign,
    keys,
    metadata,
    root_metadata,
    update_bundle,
)
from pw_software_update.tuf_pb2 import (
    RootMetadata,
    SignedRootMetadata,
    TargetsMetadata,
)
from pw_software_update.update_bundle_pb2 import UpdateBundle


def inspect_bundle_handler(arg) -> None:
    """Prints bundle contents."""

    try:
        bundle = UpdateBundle.FromString(arg.pathname.read_bytes())
        signed_targets_metadata = bundle.targets_metadata['targets']
        targets_metadata = TargetsMetadata().FromString(
            signed_targets_metadata.serialized_targets_metadata
        )
        print('Targets Metadata:')
        print('=================')
        print(targets_metadata)

        print('\nTarget Files:')
        print('=============')
        for i, (name, contents) in enumerate(bundle.target_payloads.items()):
            print(f'{i+1} of {len(bundle.target_payloads)}:')
            print(f'  filename: {name}')
            print(f'  length: {len(contents)}')

            first_32_bytes = contents[:32]
            print(f'  ascii contents(first 32 bytes): {first_32_bytes!r}')
            print(f'  hex contents(first 32 bytes): {first_32_bytes.hex()}\n')

        signed_root_metadata = bundle.root_metadata
        deserialized_root_metadata = RootMetadata.FromString(
            signed_root_metadata.serialized_root_metadata
        )
        print('\nRoot Metadata:')
        print('==============')
        print(deserialized_root_metadata)

    except IOError as error:
        print(error)


def _new_inspect_bundle_parser(subparsers) -> None:
    """Parser to handle inspect-bundle subcommand"""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    inspect_bundle_parser = subparsers.add_parser(
        'inspect-bundle',
        description='Outputs contents of bundle',
        formatter_class=formatter_class,
        help="",
    )

    inspect_bundle_parser.set_defaults(func=inspect_bundle_handler)
    inspect_bundle_parser.add_argument(
        'pathname', type=Path, help='Path to bundle'
    )


def sign_bundle_handler(arg) -> None:
    """Handles signing of a bundle"""

    try:
        signed_bundle = dev_sign.sign_update_bundle(
            UpdateBundle.FromString(arg.bundle.read_bytes()),
            arg.key.read_bytes(),
        )
        arg.bundle.write_bytes(signed_bundle.SerializeToString())

    except IOError as error:
        print(error)


def _new_sign_bundle_parser(subparsers) -> None:
    """Parser for sign-bundle subcommand"""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    sign_bundle_parser = subparsers.add_parser(
        'sign-bundle',
        description='Sign an existing bundle using a development key',
        formatter_class=formatter_class,
        help="",
    )

    sign_bundle_parser.set_defaults(func=sign_bundle_handler)
    required_arguments = sign_bundle_parser.add_argument_group(
        'required arguments'
    )
    required_arguments.add_argument(
        '--bundle',
        help='Bundle to be signed',
        metavar='BUNDLE',
        required=True,
        type=Path,
    )
    required_arguments.add_argument(
        '--key',
        help='Bundle signing key',
        metavar='KEY',
        required=True,
        type=Path,
    )


def add_file_to_bundle(
    bundle: UpdateBundle, file_name: str, file_contents: bytes
) -> UpdateBundle:
    """Adds a target file represented by file_name and file_contents to an
    existing UpdateBundle -- bundle and returns the updated UpdateBundle object.
    """

    if not file_name in bundle.target_payloads:
        bundle.target_payloads[file_name] = file_contents
    else:
        raise Exception(f'File name {file_name} already exists in bundle')

    signed_targets_metadata = bundle.targets_metadata['targets']
    targets_metadata = TargetsMetadata().FromString(
        signed_targets_metadata.serialized_targets_metadata
    )

    matching_file_names = list(
        filter(
            lambda name: name.file_name == file_name,
            targets_metadata.target_files,
        )
    )

    target_file = metadata.gen_target_file(file_name, file_contents)

    if not matching_file_names:
        targets_metadata.target_files.append(target_file)
    else:
        raise Exception(f'File name {file_name} already exists in bundle')

    bundle.targets_metadata[
        'targets'
    ].serialized_targets_metadata = targets_metadata.SerializeToString()

    return bundle


def add_file_to_bundle_handler(arg) -> None:
    """Add a new file to an existing bundle. Updates the targets metadata
    and errors out if the file already exists.
    """

    try:
        if not arg.new_name:
            file_name = os.path.splitext(os.path.basename(arg.file))[0]
        else:
            file_name = arg.new_name

        bundle = UpdateBundle().FromString(arg.bundle.read_bytes())
        updated_bundle = add_file_to_bundle(
            bundle=bundle,
            file_name=file_name,
            file_contents=arg.file.read_bytes(),
        )

        arg.bundle.write_bytes(updated_bundle.SerializeToString())

    except (IOError) as error:
        print(error)


def _new_add_file_to_bundle_parser(subparsers) -> None:
    """Parser for adding file to bundle subcommand"""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    add_file_to_bundle_parser = subparsers.add_parser(
        'add-file-to-bundle',
        description='Add a file to an existing bundle',
        formatter_class=formatter_class,
        help="",
    )
    add_file_to_bundle_parser.set_defaults(func=add_file_to_bundle_handler)
    required_arguments = add_file_to_bundle_parser.add_argument_group(
        'required arguments'
    )
    required_arguments.add_argument(
        '--bundle',
        help='Path to an existing bundle',
        metavar='BUNDLE',
        required=True,
        type=Path,
    )
    required_arguments.add_argument(
        '--file',
        help='Path to a target file',
        metavar='FILE_PATH',
        required=True,
        type=Path,
    )
    required_arguments.add_argument(
        '--new-name',
        help='Optional new name for target',
        metavar='NEW_NAME',
        required=False,
        type=str,
    )


def add_root_metadata_to_bundle_handler(arg) -> None:
    """Handles appending of root metadata to a bundle"""

    try:
        bundle = UpdateBundle().FromString(arg.bundle.read_bytes())
        bundle.root_metadata.CopyFrom(
            SignedRootMetadata().FromString(
                arg.append_root_metadata.read_bytes()
            )
        )
        arg.bundle.write_bytes(bundle.SerializeToString())

    except IOError as error:
        print(error)


def _new_add_root_metadata_to_bundle_parser(subparsers) -> None:
    """Parser for subcommand adding root metadata to bundle"""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    add_root_metadata_to_bundle_parser = subparsers.add_parser(
        'add-root-metadata-to-bundle',
        description='Add root metadata to a bundle',
        formatter_class=formatter_class,
        help="",
    )
    add_root_metadata_to_bundle_parser.set_defaults(
        func=add_root_metadata_to_bundle_handler
    )
    required_arguments = add_root_metadata_to_bundle_parser.add_argument_group(
        'required arguments'
    )
    required_arguments.add_argument(
        '--append-root-metadata',
        help='Path to root metadata',
        metavar='ROOT_METADATA',
        required=True,
        type=Path,
    )
    required_arguments.add_argument(
        '--bundle',
        help='Path to bundle',
        metavar='BUNDLE',
        required=True,
        type=Path,
    )


def create_empty_bundle_handler(arg) -> None:
    """Handles the creation of an empty bundle and writes it to disc."""

    try:
        bundle = update_bundle.gen_empty_update_bundle(
            arg.target_metadata_version
        )
        arg.pathname.write_bytes(bundle.SerializeToString())

    except IOError as error:
        print(error)


def _new_create_empty_bundle_parser(subparsers) -> None:
    """Parser for creation of an empty bundle."""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    create_empty_bundle_parser = subparsers.add_parser(
        'create-empty-bundle',
        description='Creation of an empty bundle',
        formatter_class=formatter_class,
        help="",
    )
    create_empty_bundle_parser.set_defaults(func=create_empty_bundle_handler)
    create_empty_bundle_parser.add_argument(
        'pathname', type=Path, help='Path to newly created empty bundle'
    )
    create_empty_bundle_parser.add_argument(
        '--target-metadata-version',
        help='Version number for targets metadata; Defaults to 1',
        metavar='VERSION',
        type=int,
        default=1,
        required=False,
    )


def inspect_root_metadata_handler(arg) -> None:
    """Prints root metadata contents as defined by "RootMetadata" message
    structure in tuf.proto as well as the number of identified signatures.
    """

    try:
        signed_root_metadata = SignedRootMetadata.FromString(
            arg.pathname.read_bytes()
        )

        deserialized_root_metadata = RootMetadata.FromString(
            signed_root_metadata.serialized_root_metadata
        )
        print(deserialized_root_metadata)

        print(
            'Number of signatures found:', len(signed_root_metadata.signatures)
        )

    except IOError as error:
        print(error)


def _new_inspect_root_metadata_parser(subparsers) -> None:
    """Parser to handle inspect-root-metadata subcommand"""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    inspect_root_metadata_parser = subparsers.add_parser(
        'inspect-root-metadata',
        description='Outputs contents of root metadata',
        formatter_class=formatter_class,
        help="",
    )

    inspect_root_metadata_parser.set_defaults(
        func=inspect_root_metadata_handler
    )
    inspect_root_metadata_parser.add_argument(
        'pathname', type=Path, help='Path to root metadata'
    )


def sign_root_metadata_handler(arg) -> None:
    """Handler for signing of root metadata"""

    try:
        signed_root_metadata = dev_sign.sign_root_metadata(
            SignedRootMetadata.FromString(arg.root_metadata.read_bytes()),
            arg.root_key.read_bytes(),
        )
        arg.root_metadata.write_bytes(signed_root_metadata.SerializeToString())

    except IOError as error:
        print(error)


def _new_sign_root_metadata_parser(subparsers) -> None:
    """Parser to handle sign-root-metadata subcommand"""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    sign_root_metadata_parser = subparsers.add_parser(
        'sign-root-metadata',
        description='Signing of root metadata',
        formatter_class=formatter_class,
        help="",
    )

    sign_root_metadata_parser.set_defaults(func=sign_root_metadata_handler)
    required_arguments = sign_root_metadata_parser.add_argument_group(
        'required arguments'
    )
    required_arguments.add_argument(
        '--root-metadata',
        help='Root metadata to be signed',
        metavar='ROOT_METADATA',
        required=True,
        type=Path,
    )
    required_arguments.add_argument(
        '--root-key',
        help='Root signing key',
        metavar='ROOT_KEY',
        required=True,
        type=Path,
    )


def create_root_metadata_handler(arg) -> None:
    """Handler function for creation of root metadata."""

    try:
        root_metadata.main(
            arg.out, arg.append_root_key, arg.append_targets_key, arg.version
        )

        # TODO(eashansingh): Print message that allows user
        # to visualize root metadata with
        # `pw update inspect-root-metadata` command

    except IOError as error:
        print(error)


def _new_create_root_metadata_parser(subparsers) -> None:
    """Parser to handle create-root-metadata subcommand."""

    formatter_class = lambda prog: argparse.HelpFormatter(
        prog, max_help_position=100, width=200
    )
    create_root_metadata_parser = subparsers.add_parser(
        'create-root-metadata',
        description='Creation of root metadata',
        formatter_class=formatter_class,
        help='',
    )
    create_root_metadata_parser.set_defaults(func=create_root_metadata_handler)
    create_root_metadata_parser.add_argument(
        '--version',
        help='Canonical version number for rollback checks; Defaults to 1',
        type=int,
        default=1,
        required=False,
    )

    required_arguments = create_root_metadata_parser.add_argument_group(
        'required arguments'
    )
    required_arguments.add_argument(
        '--append-root-key',
        help='Path to root key',
        metavar='ROOT_KEY',
        required=True,
        action='append',
        type=Path,
    )
    required_arguments.add_argument(
        '--append-targets-key',
        help='Path to targets key',
        metavar='TARGETS_KEY',
        required=True,
        action='append',
        type=Path,
    )
    required_arguments.add_argument(
        '-o', '--out', help='Path to output file', required=True, type=Path
    )


def generate_key_handler(arg) -> None:
    """Handler function for key generation"""

    try:
        keys.gen_ecdsa_keypair(arg.pathname)
        print('Private key: ' + str(arg.pathname))
        print('Public key: ' + str(arg.pathname) + '.pub')

    except IOError as error:
        print(error)


def _new_generate_key_parser(subparsers) -> None:
    """Parser to handle key generation subcommand."""

    generate_key_parser = subparsers.add_parser(
        'generate-key',
        description=(
            'Generates an ecdsa-sha2-nistp256 signing key pair '
            '(private + public)'
        ),
        help='',
    )
    generate_key_parser.set_defaults(func=generate_key_handler)
    generate_key_parser.add_argument(
        'pathname', type=Path, help='Path to generated key pair'
    )


def _parse_args() -> argparse.Namespace:
    parser_root = argparse.ArgumentParser(
        description='Software update related operations.',
        epilog='Learn more at: pigweed.dev/pw_software_update',
    )
    parser_root.set_defaults(
        func=lambda *_args, **_kwargs: parser_root.print_help()
    )

    subparsers = parser_root.add_subparsers()

    # Key generation related parsers
    _new_generate_key_parser(subparsers)

    # Root metadata related parsers
    _new_create_root_metadata_parser(subparsers)
    _new_sign_root_metadata_parser(subparsers)
    _new_inspect_root_metadata_parser(subparsers)

    # Bundle related parsers
    _new_create_empty_bundle_parser(subparsers)
    _new_add_root_metadata_to_bundle_parser(subparsers)
    _new_add_file_to_bundle_parser(subparsers)
    _new_sign_bundle_parser(subparsers)
    _new_inspect_bundle_parser(subparsers)

    return parser_root.parse_args()


def _dispatch_command(args) -> None:
    args.func(args)


def main() -> int:
    """Software update command-line interface(WIP)."""
    _dispatch_command(_parse_args())
    return 0


if __name__ == '__main__':
    sys.exit(main())
