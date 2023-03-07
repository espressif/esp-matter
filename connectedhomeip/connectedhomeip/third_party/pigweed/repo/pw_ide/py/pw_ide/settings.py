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
"""pw_ide settings."""

import enum
from inspect import cleandoc
import glob
import os
from pathlib import Path
from typing import Any, cast, Dict, List, Literal, Optional, Union
import yaml

from pw_cli.yaml_config_loader_mixin import YamlConfigLoaderMixin

PW_IDE_DIR_NAME = '.pw_ide'
PW_IDE_DEFAULT_DIR = (
    Path(os.path.expandvars('$PW_PROJECT_ROOT')) / PW_IDE_DIR_NAME
)

PW_PIGWEED_CIPD_INSTALL_DIR = Path(
    os.path.expandvars('$PW_PIGWEED_CIPD_INSTALL_DIR')
)

PW_ARM_CIPD_INSTALL_DIR = Path(os.path.expandvars('$PW_ARM_CIPD_INSTALL_DIR'))

_DEFAULT_BUILD_DIR_NAME = 'out'
_DEFAULT_BUILD_DIR = (
    Path(os.path.expandvars('$PW_PROJECT_ROOT')) / _DEFAULT_BUILD_DIR_NAME
)

_DEFAULT_COMPDB_PATHS = [_DEFAULT_BUILD_DIR]
_DEFAULT_TARGET_INFERENCE = '?'

SupportedEditorName = Literal['vscode']


class SupportedEditor(enum.Enum):
    VSCODE = 'vscode'


_DEFAULT_SUPPORTED_EDITORS: Dict[SupportedEditorName, bool] = {
    'vscode': True,
}

_DEFAULT_CONFIG: Dict[str, Any] = {
    'clangd_additional_query_drivers': [],
    'build_dir': _DEFAULT_BUILD_DIR,
    'compdb_paths': _DEFAULT_BUILD_DIR_NAME,
    'default_target': None,
    'editors': _DEFAULT_SUPPORTED_EDITORS,
    'setup': ['pw --no-banner ide cpp --gn --set-default --no-override'],
    'targets': [],
    'target_inference': _DEFAULT_TARGET_INFERENCE,
    'working_dir': PW_IDE_DEFAULT_DIR,
}

_DEFAULT_PROJECT_FILE = Path('$PW_PROJECT_ROOT/.pw_ide.yaml')
_DEFAULT_PROJECT_USER_FILE = Path('$PW_PROJECT_ROOT/.pw_ide.user.yaml')
_DEFAULT_USER_FILE = Path('$HOME/.pw_ide.yaml')


class PigweedIdeSettings(YamlConfigLoaderMixin):
    """Pigweed IDE features settings storage class."""

    def __init__(
        self,
        project_file: Union[Path, bool] = _DEFAULT_PROJECT_FILE,
        project_user_file: Union[Path, bool] = _DEFAULT_PROJECT_USER_FILE,
        user_file: Union[Path, bool] = _DEFAULT_USER_FILE,
        default_config: Optional[Dict[str, Any]] = None,
    ) -> None:
        self.config_init(
            config_section_title='pw_ide',
            project_file=project_file,
            project_user_file=project_user_file,
            user_file=user_file,
            default_config=_DEFAULT_CONFIG
            if default_config is None
            else default_config,
            environment_var='PW_IDE_CONFIG_FILE',
        )

    @property
    def working_dir(self) -> Path:
        """Path to the ``pw_ide`` working directory.

        The working directory holds C++ compilation databases and caches, and
        other supporting files. This should not be a directory that's regularly
        deleted or manipulated by other processes (e.g. the GN ``out``
        directory) nor should it be committed to source control.
        """
        return Path(self._config.get('working_dir', PW_IDE_DEFAULT_DIR))

    @property
    def build_dir(self) -> Path:
        """The build system's root output directory.

        We will use this as the output directory when automatically running
        build system commands, and will use it to resolve target names using
        target name inference when processing compilation databases. This can
        be the same build directory used for general-purpose builds, but it
        does not have to be.
        """
        return Path(self._config.get('build_dir', _DEFAULT_BUILD_DIR))

    @property
    def compdb_paths(self) -> str:
        """A path glob to search for compilation databases.

        These paths can be to files or to directories. Paths that are
        directories will be appended with the default file name for
        ``clangd`` compilation databases, ``compile_commands.json``.
        """
        return self._config.get('compdb_paths', _DEFAULT_BUILD_DIR_NAME)

    @property
    def compdb_paths_expanded(self) -> List[Path]:
        return [Path(node) for node in glob.iglob(self.compdb_paths)]

    @property
    def targets(self) -> List[str]:
        """The list of targets that should be enabled for code analysis.

        In this case, "target" is analogous to a GN target, i.e., a particular
        build configuration. By default, all available targets are enabled. By
        adding targets to this list, you can constrain the targets that are
        enabled for code analysis to a subset of those that are available, which
        may be useful if your project has many similar targets that are
        redundant from a code analysis perspective.

        Target names need to match the name of the directory that holds the
        build system artifacts for the target. For example, GN outputs build
        artifacts for the ``pw_strict_host_clang_debug`` target in a directory
        with that name in its output directory. So that becomes the canonical
        name for the target.
        """
        return self._config.get('targets', list())

    @property
    def target_inference(self) -> str:
        """A glob-like string for extracting a target name from an output path.

        Build systems and projects have varying ways of organizing their build
        directory structure. For a given compilation unit, we need to know how
        to extract the build's target name from the build artifact path. A
        simple example:

        .. code-block:: none

           clang++ hello.cc -o host/obj/hello.cc.o

        The top-level directory ``host`` is the target name we want. The same
        compilation unit might be used with another build target:

        .. code-block:: none

           gcc-arm-none-eabi hello.cc -o arm_dev_board/obj/hello.cc.o

        In this case, this compile command is associated with the
        ``arm_dev_board`` target.

        When importing and processing a compilation database, we assume by
        default that for each compile command, the corresponding target name is
        the name of the top level directory within the build directory root
        that contains the build artifact. This is the default behavior for most
        build systems. However, if your project is structured differently, you
        can provide a glob-like string that indicates how to extract the target
        name from build artifact path.

        A ``*`` indicates any directory, and ``?`` indicates the directory that
        has the name of the target. The path is resolved from the build
        directory root, and anything deeper than the target directory is
        ignored. For example, a glob indicating that the directory two levels
        down from the build directory root has the target name would be
        expressed with ``*/*/?``.
        """
        return self._config.get('target_inference', _DEFAULT_TARGET_INFERENCE)

    @property
    def default_target(self) -> Optional[str]:
        """The default target to use when calling ``--set-default``.

        This target will be selected when ``pw ide cpp --set-default`` is
        called. You can define an explicit default target here. If that command
        is invoked without a default target definition, ``pw_ide`` will try to
        infer the best choice of default target. Currently, it selects the
        target with the broadest compilation unit coverage.
        """
        return self._config.get('default_target', None)

    @property
    def setup(self) -> List[str]:
        """A sequence of commands to automate IDE features setup.

        ``pw ide setup`` should do everything necessary to get the project from
        a fresh checkout to a working default IDE experience. This defines the
        list of commands that makes that happen, which will be executed
        sequentially in subprocesses. These commands should be idempotent, so
        that the user can run them at any time to update their IDE features
        configuration without the risk of putting those features in a bad or
        unexpected state.
        """
        return self._config.get('setup', list())

    @property
    def clangd_additional_query_drivers(self) -> List[str]:
        """Additional query driver paths that clangd should use.

        By default, ``pw_ide`` supplies driver paths for the toolchains included
        in Pigweed. If you are using toolchains that are not supplied by
        Pigweed, you should include path globs to your toolchains here. These
        paths will be given higher priority than the Pigweed toolchain paths.
        """
        return self._config.get('clangd_additional_query_drivers', list())

    def clangd_query_drivers(self) -> List[str]:
        return [
            *[str(Path(p)) for p in self.clangd_additional_query_drivers],
            str(PW_PIGWEED_CIPD_INSTALL_DIR / 'bin' / '*'),
            str(PW_ARM_CIPD_INSTALL_DIR / 'bin' / '*'),
        ]

    def clangd_query_driver_str(self) -> str:
        return ','.join(self.clangd_query_drivers())

    @property
    def editors(self) -> Dict[str, bool]:
        """Enable or disable automated support for editors.

        Automatic support for some editors is provided by ``pw_ide``, which is
        accomplished through generating configuration files in your project
        directory. All supported editors are enabled by default, but you can
        disable editors by adding an ``'<editor>': false`` entry.
        """
        return self._config.get('editors', _DEFAULT_SUPPORTED_EDITORS)

    def editor_enabled(self, editor: SupportedEditorName) -> bool:
        """True if the provided editor is enabled in settings.

        This module will integrate the project with all supported editors by
        default. If the project or user want to disable particular editors,
        they can do so in the appropriate settings file.
        """
        return self._config.get('editors', {}).get(editor, False)


def _docstring_set_default(
    obj: Any, default: Any, literal: bool = False
) -> None:
    """Add a default value annotation to a docstring.

    Formatting isn't allowed in docstrings, so by default we can't inject
    variables that we would like to appear in the documentation, like the
    default value of a property. But we can use this function to add it
    separately.
    """
    if obj.__doc__ is not None:
        default = str(default)

        if literal:
            lines = default.splitlines()

            if len(lines) == 0:
                return
            if len(lines) == 1:
                default = f'Default: ``{lines[0]}``'
            else:
                default = 'Default:\n\n.. code-block::\n\n  ' + '\n  '.join(
                    lines
                )

        doc = cast(str, obj.__doc__)
        obj.__doc__ = f'{cleandoc(doc)}\n\n{default}'


_docstring_set_default(
    PigweedIdeSettings.working_dir, PW_IDE_DIR_NAME, literal=True
)
_docstring_set_default(
    PigweedIdeSettings.build_dir, _DEFAULT_BUILD_DIR_NAME, literal=True
)
_docstring_set_default(
    PigweedIdeSettings.compdb_paths,
    _DEFAULT_CONFIG['compdb_paths'],
    literal=True,
)
_docstring_set_default(
    PigweedIdeSettings.targets, _DEFAULT_CONFIG['targets'], literal=True
)
_docstring_set_default(
    PigweedIdeSettings.default_target,
    _DEFAULT_CONFIG['default_target'],
    literal=True,
)
_docstring_set_default(
    PigweedIdeSettings.target_inference,
    _DEFAULT_CONFIG['target_inference'],
    literal=True,
)
_docstring_set_default(
    PigweedIdeSettings.setup, _DEFAULT_CONFIG['setup'], literal=True
)
_docstring_set_default(
    PigweedIdeSettings.clangd_additional_query_drivers,
    _DEFAULT_CONFIG['clangd_additional_query_drivers'],
    literal=True,
)
_docstring_set_default(
    PigweedIdeSettings.editors,
    yaml.dump(_DEFAULT_SUPPORTED_EDITORS),
    literal=True,
)
