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
"""Utilities for managing modules."""

import argparse

import pw_module.check
import pw_module.create


def main() -> None:
    """Entrypoint for the `pw module` plugin."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.set_defaults(func=lambda **_kwargs: parser.print_help())

    subparsers = parser.add_subparsers(title='subcommands')

    pw_module.check.register_subcommand(
        subparsers.add_parser('check', help=pw_module.check.__doc__)
    )
    pw_module.create.register_subcommand(
        subparsers.add_parser('create', help=pw_module.create.__doc__)
    )

    args = {**vars(parser.parse_args())}
    func = args['func']
    del args['func']
    func(**args)


if __name__ == '__main__':
    main()
