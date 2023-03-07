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
"""Check various rules for .gitmodules files."""

import dataclasses
import logging
from pathlib import Path
from typing import Callable, Dict, Optional, Sequence
import urllib.parse

from pw_presubmit import (
    git_repo,
    PresubmitContext,
    PresubmitFailure,
    filter_paths,
)

_LOG: logging.Logger = logging.getLogger(__name__)


@dataclasses.dataclass
class Config:
    # Allow direct references to non-Google hosts.
    allow_non_googlesource_hosts: bool = False

    # Allow a specific subset of googlesource.com hosts. If an empty list then
    # all googlesource hosts are permitted.
    allowed_googlesource_hosts: Sequence[str] = ()

    # Require relative URLs, like those that start with "/" or "../".
    require_relative_urls: bool = False

    # Allow "sso://" URLs.
    allow_sso: bool = True

    # Allow use of "git.corp.google.com" URLs.
    allow_git_corp_google_com: bool = True

    # Require a branch for each submodule.
    require_branch: bool = False

    # Arbitrary validator. Gets invoked with the submodule name and a dict of
    # the submodule properties. Should throw exceptions or call ctx.fail to
    # register errors.
    validator: Optional[
        Callable[[PresubmitContext, Path, str, Dict[str, str]], None]
    ] = None


def _parse_gitmodules(path: Path) -> Dict[str, Dict[str, str]]:
    raw_submodules: str = git_repo.git_stdout(
        'config', '--file', path, '--list'
    )
    submodules: Dict[str, Dict[str, str]] = {}
    for line in raw_submodules.splitlines():
        key: str
        value: str
        key, value = line.split('=', 1)
        if not key.startswith('submodule.'):
            raise PresubmitFailure(f'unexpected key {key!r}', path)
        key = key.split('.', 1)[1]

        submodule: str
        param: str
        submodule, param = key.rsplit('.', 1)

        submodules.setdefault(submodule, {})
        submodules[submodule][param] = value

    return submodules


_GERRIT_HOST_SUFFIXES = ('.googlesource.com', '.git.corp.google.com')


def process_gitmodules(ctx: PresubmitContext, config: Config, path: Path):
    """Check if a specific .gitmodules file passes the options in the config."""
    _LOG.debug('Evaluating path %s', path)
    submodules: Dict[str, Dict[str, str]] = _parse_gitmodules(path)

    assert isinstance(config.allowed_googlesource_hosts, (list, tuple))
    for allowed in config.allowed_googlesource_hosts:
        if '.' in allowed or '-review' in allowed:
            raise PresubmitFailure(
                f'invalid googlesource requirement: {allowed}'
            )

    for name, submodule in submodules.items():
        _LOG.debug('======================')
        _LOG.debug('evaluating submodule %s', name)
        _LOG.debug('%r', submodule)

        if config.require_branch:
            _LOG.debug('branch is required')
            if 'branch' not in submodule:
                ctx.fail(
                    f'submodule {name} does not have a branch set but '
                    'branches are required'
                )

        url = submodule['url']

        if config.validator:
            config.validator(ctx, path, name, submodule)

        if url.startswith(('/', '../')):
            _LOG.debug('URL is relative, remaining checks are irrelevant')
            continue

        if config.require_relative_urls:
            _LOG.debug('relative URLs required')
            ctx.fail(
                f'submodule {name} has non-relative url {url!r} but '
                'relative urls are required'
            )
            continue

        parsed = urllib.parse.urlparse(url)

        if not config.allow_sso:
            _LOG.debug('sso not allowed')
            if parsed.scheme in ('sso', 'rpc'):
                ctx.fail(
                    f'submodule {name} has sso/rpc url {url!r} but '
                    'sso/rpc urls are not allowed'
                )
                continue

        if not config.allow_git_corp_google_com:
            _LOG.debug('git.corp.google.com not allowed')
            if '.git.corp.google.com' in parsed.netloc:
                ctx.fail(
                    f'submodule {name} has git.corp.google.com url '
                    f'{url!r} but git.corp.google.com urls are not '
                    'allowed'
                )
                continue

        if not config.allow_non_googlesource_hosts:
            _LOG.debug('non-google hosted repos not allowed')
            if parsed.scheme not in (
                'sso',
                'rpc',
            ) and not parsed.netloc.endswith(_GERRIT_HOST_SUFFIXES):
                ctx.fail(
                    f'submodule {name} has prohibited non-Google url ' f'{url}'
                )
                continue

        if config.allowed_googlesource_hosts:
            _LOG.debug(
                'allowed googlesource hosts: %r',
                config.allowed_googlesource_hosts,
            )
            _LOG.debug('raw url: %s', url)
            host = parsed.netloc
            if host.endswith(_GERRIT_HOST_SUFFIXES) or parsed.scheme in (
                'sso',
                'rpc',
            ):
                for suffix in _GERRIT_HOST_SUFFIXES:
                    host = host.replace(suffix, '')
                _LOG.debug('host: %s', host)
                if host not in config.allowed_googlesource_hosts:
                    ctx.fail(
                        f'submodule {name} is from prohibited Google '
                        f'Gerrit host {parsed.netloc}'
                    )
                    continue


def create(config: Config = Config()):
    """Create a gitmodules presubmit step with a given config."""

    @filter_paths(endswith='.gitmodules')
    def gitmodules(ctx: PresubmitContext):
        """Check various rules for .gitmodules files."""
        for path in ctx.paths:
            process_gitmodules(ctx, config, path)

    return gitmodules
