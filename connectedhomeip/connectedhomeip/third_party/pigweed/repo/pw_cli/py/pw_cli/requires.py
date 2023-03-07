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
"""Create transitive CLs for requirements on internal Gerrits.

This is only intended to be used by Googlers.

If the current CL needs to be tested alongside internal-project:1234 on an
internal project, but "internal-project" is something that can't be referenced
publicly, this automates creation of a CL on the pigweed-internal Gerrit that
references internal-project:1234 so the current commit effectively has a
requirement on internal-project:1234.

For more see http://go/pigweed-ci-cq-intro.
"""

import argparse
import json
import logging
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import uuid

HELPER_GERRIT = 'pigweed-internal'
HELPER_PROJECT = 'requires-helper'
HELPER_REPO = 'sso://{}/{}'.format(HELPER_GERRIT, HELPER_PROJECT)

# Pass checks that look for "DO NOT ..." and block submission.
_DNS = ' '.join(
    (
        'DO',
        'NOT',
        'SUBMIT',
    )
)

# Subset of the output from pushing to Gerrit.
DEFAULT_OUTPUT = f'''
remote:
remote:   https://{HELPER_GERRIT}-review.git.corp.google.com/c/{HELPER_PROJECT}/+/123456789 {_DNS} [NEW]
remote:
'''.strip()

_LOG = logging.getLogger(__name__)


def parse_args() -> argparse.Namespace:
    """Creates an argument parser and parses arguments."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'requirements',
        nargs='+',
        help='Requirements to be added ("<gerrit-name>:<cl-number>").',
    )
    parser.add_argument(
        '--no-push',
        dest='push',
        action='store_false',
        help=argparse.SUPPRESS,  # This option is only for debugging.
    )

    return parser.parse_args()


def _run_command(*args, **kwargs):
    kwargs.setdefault('capture_output', True)
    _LOG.debug('%s', args)
    _LOG.debug('%s', kwargs)
    res = subprocess.run(*args, **kwargs)
    _LOG.debug('%s', res.stdout)
    _LOG.debug('%s', res.stderr)
    res.check_returncode()
    return res


def check_status() -> bool:
    res = subprocess.run(['git', 'status'], capture_output=True)
    if res.returncode:
        _LOG.error('repository not clean, commit to suppress this warning')
        return False
    return True


def clone(requires_dir: Path) -> None:
    _LOG.info('cloning helper repository into %s', requires_dir)
    _run_command(['git', 'clone', HELPER_REPO, '.'], cwd=requires_dir)


def create_commit(requires_dir: Path, requirements) -> None:
    """Create a commit in the local tree with the given requirements."""
    change_id = str(uuid.uuid4()).replace('-', '00')
    _LOG.debug('change_id %s', change_id)

    reqs = []
    for req in requirements:
        gerrit_name, number = req.split(':', 1)
        reqs.append({'gerrit_name': gerrit_name, 'number': number})

    path = requires_dir / 'patches.json'
    _LOG.debug('path %s', path)
    with open(path, 'w') as outs:
        json.dump(reqs, outs)

    _run_command(['git', 'add', path], cwd=requires_dir)

    commit_message = [
        f'{_DNS} {change_id[0:10]}\n\n',
        '',
        f'Change-Id: I{change_id}',
    ]
    for req in requirements:
        commit_message.append(f'Requires: {req}')

    _LOG.debug('message %s', commit_message)
    _run_command(
        ['git', 'commit', '-m', '\n'.join(commit_message)],
        cwd=requires_dir,
    )

    # Not strictly necessary, only used for logging.
    _run_command(['git', 'show'], cwd=requires_dir)


def push_commit(requires_dir: Path, push=True) -> str:
    output = DEFAULT_OUTPUT
    if push:
        res = _run_command(
            ['git', 'push', HELPER_REPO, '+HEAD:refs/for/main'],
            cwd=requires_dir,
        )
        output = res.stderr.decode()

    _LOG.debug('output: %s', output)
    regex = re.compile(
        f'^\\s*remote:\\s*'
        f'https://{HELPER_GERRIT}-review.(?:git.corp.google|googlesource).com/'
        f'c/{HELPER_PROJECT}/\\+/(?P<num>\\d+)\\s+',
        re.MULTILINE,
    )
    _LOG.debug('regex %r', regex)
    match = regex.search(output)
    if not match:
        raise ValueError(f"invalid output from 'git push': {output}")
    change_num = match.group('num')
    _LOG.info('created %s change %s', HELPER_PROJECT, change_num)
    return f'{HELPER_GERRIT}:{change_num}'


def amend_existing_change(change: str) -> None:
    res = _run_command(['git', 'log', '-1', '--pretty=%B'])
    original = res.stdout.rstrip().decode()

    addition = f'Requires: {change}'
    _LOG.info('adding "%s" to current commit message', addition)
    message = '\n'.join((original, addition))
    _run_command(['git', 'commit', '--amend', '--message', message])


def run(requirements, push=True) -> int:
    """Entry point for requires."""

    if not check_status():
        return -1

    # Create directory for checking out helper repository.
    with tempfile.TemporaryDirectory() as requires_dir_str:
        requires_dir = Path(requires_dir_str)
        # Clone into helper repository.
        clone(requires_dir)
        # Make commit with requirements from command line.
        create_commit(requires_dir, requirements)
        # Push that commit and save its number.
        change = push_commit(requires_dir, push=push)
    # Add dependency on newly pushed commit on current commit.
    amend_existing_change(change)

    return 0


def main() -> int:
    return run(**vars(parse_args()))


if __name__ == '__main__':
    try:
        # If pw_cli is available, use it to initialize logs.
        from pw_cli import log

        log.install(logging.INFO)
    except ImportError:
        # If pw_cli isn't available, display log messages like a simple print.
        logging.basicConfig(format='%(message)s', level=logging.INFO)

    sys.exit(main())
