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
"""pw_ide CLI command handlers."""

import logging
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
from typing import cast, Callable, List, Optional, Set, Tuple, Union

from pw_cli.color import colors

from pw_ide.cpp import (
    ClangdSettings,
    compdb_generate_file_path,
    CppCompilationDatabase,
    CppCompilationDatabasesMap,
    CppIdeFeaturesState,
    delete_compilation_databases,
    delete_compilation_database_caches,
    MAX_COMMANDS_TARGET_FILENAME,
)

from pw_ide.exceptions import (
    BadCompDbException,
    InvalidTargetException,
    MissingCompDbException,
)

from pw_ide.python import PythonPaths

from pw_ide.settings import (
    PigweedIdeSettings,
    SupportedEditor,
    SupportedEditorName,
)

from pw_ide import vscode
from pw_ide.vscode import VscSettingsManager, VscSettingsType


def _no_color(msg: str) -> str:
    return msg


def _split_lines(msg: Union[str, List[str]]) -> Tuple[str, List[str]]:
    """Turn a list of strings into a tuple of the first and list of rest."""
    if isinstance(msg, str):
        return (msg, [])

    return (msg[0], msg[1:])


class StatusReporter:
    """Print user-friendly status reports to the terminal for CLI tools.

    The output of ``demo()`` looks something like this, but more colorful:

    .. code-block:: none

       • FYI, here's some information:
         Lorem ipsum dolor sit amet, consectetur adipiscing elit.
         Donec condimentum metus molestie metus maximus ultricies ac id dolor.
       ✓ This is okay, no changes needed.
       ✓ We changed some things successfully!
       ⚠ Uh oh, you might want to be aware of this.
       ❌ This is bad! Things might be broken!

    You can instead redirect these lines to logs without formatting by
    substituting ``LoggingStatusReporter``. Consumers of this should be
    designed to take any subclass and not make assumptions about where the
    output will go. But the reason you would choose this over plain logging is
    because you want to support pretty-printing to the terminal.

    This is also "themable" in the sense that you can subclass this, override
    the methods with whatever formatting you want, and supply the subclass to
    anything that expects an instance of this.

    Key:

    - info: Plain ol' informational status.
    - ok: Something was checked and it was okay.
    - new: Something needed to be changed/updated and it was successfully.
    - wrn: Warning, non-critical.
    - err: Error, critical.

    This doesn't expose the %-style string formatting that is used in idiomatic
    Python logging, but this shouldn't be used for performance-critical logging
    situations anyway.
    """

    def _report(  # pylint: disable=no-self-use
        self,
        msg: Union[str, List[str]],
        color: Callable[[str], str],
        char: str,
        func: Callable,
        silent: bool,
    ) -> None:
        """Actually print/log/whatever the status lines."""
        first_line, rest_lines = _split_lines(msg)
        first_line = color(f'{char} {first_line}')
        spaces = ' ' * len(char)
        rest_lines = [color(f'{spaces} {line}') for line in rest_lines]

        if not silent:
            for line in [first_line, *rest_lines]:
                func(line)

    def demo(self):
        """Run this to see what your status reporter output looks like."""
        self.info(
            [
                'FYI, here\'s some information:',
                'Lorem ipsum dolor sit amet, consectetur adipiscing elit.',
                'Donec condimentum metus molestie metus maximus ultricies '
                'ac id dolor.',
            ]
        )
        self.ok('This is okay, no changes needed.')
        self.new('We changed some things successfully!')
        self.wrn('Uh oh, you might want to be aware of this.')
        self.err('This is bad! Things might be broken!')

    def info(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, _no_color, '\u2022', print, silent)

    def ok(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, colors().blue, '\u2713', print, silent)

    def new(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, colors().green, '\u2713', print, silent)

    def wrn(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, colors().yellow, '\u26A0', print, silent)

    def err(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, colors().red, '\u274C', print, silent)


class LoggingStatusReporter(StatusReporter):
    """Print status lines to logs instead of to the terminal."""

    def __init__(self, logger: logging.Logger) -> None:
        self.logger = logger
        super().__init__()

    def _report(
        self,
        msg: Union[str, List[str]],
        color: Callable[[str], str],
        char: str,
        func: Callable,
        silent: bool,
    ) -> None:
        first_line, rest_lines = _split_lines(msg)

        if not silent:
            for line in [first_line, *rest_lines]:
                func(line)

    def info(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, _no_color, '', self.logger.info, silent)

    def ok(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, _no_color, '', self.logger.info, silent)

    def new(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, _no_color, '', self.logger.info, silent)

    def wrn(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, _no_color, '', self.logger.warning, silent)

    def err(self, msg: Union[str, List[str]], silent: bool = False) -> None:
        self._report(msg, _no_color, '', self.logger.error, silent)


def _make_working_dir(
    reporter: StatusReporter, settings: PigweedIdeSettings, quiet: bool = False
) -> None:
    if not settings.working_dir.exists():
        settings.working_dir.mkdir()
        reporter.new(
            'Initialized the Pigweed IDE working directory at '
            f'{settings.working_dir}'
        )
    else:
        if not quiet:
            reporter.ok(
                'Pigweed IDE working directory already present at '
                f'{settings.working_dir}'
            )


def _report_unrecognized_editor(reporter: StatusReporter, editor: str) -> None:
    supported_editors = ', '.join(sorted([ed.value for ed in SupportedEditor]))
    reporter.wrn(f'Unrecognized editor: {editor}')
    reporter.wrn('This may not be an automatically-supported editor.')
    reporter.wrn(f'Automatically-supported editors: {supported_editors}')


def cmd_clear(
    compdb: bool,
    cache: bool,
    editor: Optional[SupportedEditorName],
    editor_backups: Optional[SupportedEditorName],
    silent: bool = False,
    reporter: StatusReporter = StatusReporter(),
    pw_ide_settings: PigweedIdeSettings = PigweedIdeSettings(),
) -> None:
    """Clear components of the IDE features.

    In contrast to the ``reset`` subcommand, ``clear`` allows you to specify
    components to delete. You will not need this command under normal
    circumstances.
    """
    if compdb:
        delete_compilation_databases(pw_ide_settings)
        reporter.wrn('Cleared compilation databases', silent)

    if cache:
        delete_compilation_database_caches(pw_ide_settings)
        reporter.wrn('Cleared compilation database caches', silent)

    if editor is not None:
        try:
            validated_editor = SupportedEditor(editor)
        except ValueError:
            _report_unrecognized_editor(reporter, cast(str, editor))
            sys.exit(1)

        if validated_editor == SupportedEditor.VSCODE:
            vsc_settings_manager = VscSettingsManager(pw_ide_settings)
            vsc_settings_manager.delete_all_active_settings()

        reporter.wrn(
            f'Cleared active settings for {validated_editor.value}', silent
        )

    if editor_backups is not None:
        try:
            validated_editor = SupportedEditor(editor_backups)
        except ValueError:
            _report_unrecognized_editor(reporter, cast(str, editor))
            sys.exit(1)

        if validated_editor == SupportedEditor.VSCODE:
            vsc_settings_manager = VscSettingsManager(pw_ide_settings)
            vsc_settings_manager.delete_all_backups()

        reporter.wrn(
            f'Cleared backup settings for {validated_editor.value}',
            silent=silent,
        )


def cmd_reset(
    hard: bool = False,
    reporter: StatusReporter = StatusReporter(),
    pw_ide_settings: PigweedIdeSettings = PigweedIdeSettings(),
) -> None:
    """Reset IDE settings.

    This will clear your .pw_ide working directory and active settings for
    supported editors, restoring your repository to a pre-"pw ide setup" state.
    Any clangd caches in the working directory will not be removed, so that they
    don't need to be generated again later. All backed up supported editor
    settings will also be left in place.

    Adding the --hard flag will completely delete the .pw_ide directory and all
    supported editor backup settings, restoring your repository to a
    pre-`pw ide setup` state.

    This command does not affect this project's pw_ide and editor settings or
    your own pw_ide and editor override settings.
    """
    delete_compilation_databases(pw_ide_settings)
    vsc_settings_manager = VscSettingsManager(pw_ide_settings)
    vsc_settings_manager.delete_all_active_settings()

    if hard:
        try:
            shutil.rmtree(pw_ide_settings.working_dir)
        except FileNotFoundError:
            pass

        vsc_settings_manager.delete_all_backups()

    reporter.wrn('Pigweed IDE settings were reset!')


def cmd_setup(
    reporter: StatusReporter = StatusReporter(),
    pw_ide_settings: PigweedIdeSettings = PigweedIdeSettings(),
) -> None:
    """Set up or update your Pigweed project IDE features.

    This will automatically set up your development environment with all the
    features that Pigweed IDE supports, with sensible defaults.

    At minimum, this command will create the .pw_ide working directory and
    create settings files for all supported editors. Projects can define
    additional setup steps in .pw_ide.yaml.

    When new IDE features are introduced in the future (either by Pigweed or
    your downstream project), you can re-run this command to set up the new
    features. It will not overwrite or break any of your existing configuration.
    """
    _make_working_dir(reporter, pw_ide_settings)

    if pw_ide_settings.editor_enabled('vscode'):
        cmd_vscode(no_override=True)

    for command in pw_ide_settings.setup:
        subprocess.run(shlex.split(command))


def cmd_vscode(
    include: Optional[List[VscSettingsType]] = None,
    exclude: Optional[List[VscSettingsType]] = None,
    no_override: bool = False,
    reporter: StatusReporter = StatusReporter(),
    pw_ide_settings: PigweedIdeSettings = PigweedIdeSettings(),
) -> None:
    """Configure support for Visual Studio Code.

    This will replace your current Visual Studio Code (VSC) settings for this
    project (in ``.vscode/settings.json``, etc.) with the following sets of
    settings, in order:

    - The Pigweed default settings
    - Your project's settings, if any (in ``.vscode/pw_project_settings.json``)
    - Your personal settings, if any (in ``.vscode/pw_user_settings.json``)

    In other words, settings files lower on the list can override settings
    defined by those higher on the list. Settings defined in the sources above
    are not active in VSC until they are merged and output to the current
    settings file by running:

    .. code-block:: bash

       pw ide vscode

    Refer to the Visual Studio Code documentation for more information about
    these settings: https://code.visualstudio.com/docs/getstarted/settings

    This command also manages VSC tasks (``.vscode/tasks.json``) and extensions
    (``.vscode/extensions.json``). You can explicitly control which of these
    settings types ("settings", "tasks", and "extensions") is modified by
    this command by using the ``--include`` or ``--exclude`` options.

    Your current VSC settings will never change unless you run ``pw ide``
    commands. Since the current VSC settings are an artifact built from the
    three settings files described above, you should avoid manually editing
    that file; it will be replaced the next time you run ``pw ide vscode``. A
    backup of your previous settings file will be made, and you can diff it
    against the new file to see what changed.

    These commands will never modify your VSC user settings, which are
    stored outside of the project repository and apply globally to all VSC
    instances.

    The settings files are system-specific and shouldn't be checked into the
    repository, except for the project settings (those with ``pw_project_``),
    which can be used to define consistent settings for everyone working on the
    project.

    Note that support for VSC can be disabled at the project level or the user
    level by adding the following to .pw_ide.yaml or .pw_ide.user.yaml
    respectively:

    .. code-block:: yaml

        editors:
          vscode: false

    Likewise, it can be enabled by setting that value to true. It is enabled by
    default.
    """
    if not pw_ide_settings.editor_enabled('vscode'):
        reporter.wrn('Visual Studio Code support is disabled in settings!')
        sys.exit(1)

    if not vscode.DEFAULT_SETTINGS_PATH.exists():
        vscode.DEFAULT_SETTINGS_PATH.mkdir()

    vsc_manager = VscSettingsManager(pw_ide_settings)

    if include is None:
        include_set = set(VscSettingsType.all())
    else:
        include_set = set(include)

    if exclude is None:
        exclude_set: Set[VscSettingsType] = set()
    else:
        exclude_set = set(exclude)

    types_to_update = cast(
        List[VscSettingsType], tuple(include_set - exclude_set)
    )

    for settings_type in types_to_update:
        active_settings_existed = vsc_manager.active(settings_type).is_present()

        if no_override and active_settings_existed:
            reporter.ok(
                f'Visual Studio Code active {settings_type.value} '
                'already present; will not overwrite'
            )

        else:
            with vsc_manager.active(settings_type).modify(
                reinit=True
            ) as active_settings:
                vsc_manager.default(settings_type).sync_to(active_settings)
                vsc_manager.project(settings_type).sync_to(active_settings)
                vsc_manager.user(settings_type).sync_to(active_settings)

            verb = 'Updated' if active_settings_existed else 'Created'
            reporter.new(
                f'{verb} Visual Studio Code active ' f'{settings_type.value}'
            )


# TODO(chadnorvell): Break up this function.
# The linting errors are a nuisance but they're beginning to have a point.
def cmd_cpp(  # pylint: disable=too-many-arguments, too-many-locals, too-many-branches, too-many-statements
    should_list_targets: bool,
    should_get_target: bool,
    target_to_set: Optional[str],
    compdb_file_paths: Optional[List[Path]],
    build_dir: Optional[Path],
    use_default_target: bool = False,
    should_run_ninja: bool = False,
    should_run_gn: bool = False,
    override_current_target: bool = True,
    clangd_command: bool = False,
    clangd_command_system: Optional[str] = None,
    reporter: StatusReporter = StatusReporter(),
    pw_ide_settings: PigweedIdeSettings = PigweedIdeSettings(),
) -> None:
    """Configure C/C++ code intelligence support.

    Code intelligence can be provided by clangd or other language servers that
    use the clangd compilation database format, defined at:
    https://clang.llvm.org/docs/JSONCompilationDatabase.html

    This command helps you use clangd with Pigweed projects, which use multiple
    toolchains within a distinct environment, and often define multiple targets.
    This means compilation units are likely have multiple compile commands, and
    clangd is not equipped to deal with this out of the box. We handle this by:

    - Processing the compilation database produced the build system into
      multiple internally-consistent compilation databases, one for each target
      (where a "target" is a particular build for a particular system using a
      particular toolchain).

    - Providing commands to select which target you want to use for code
      analysis.

    Refer to the Pigweed documentation or your build system's documentation to
    learn how to produce a clangd compilation database. Once you have one, run
    this command to process it (or provide a glob to process multiple):

    .. code-block:: bash

        pw ide cpp --process {path to compile_commands.json}

    If you're using GN to generate the compilation database, you can do that and
    process it in a single command:

    .. code-block:: bash

       pw ide cpp --gn

    You can do the same for a Ninja build (whether it was generated by GN or
    another way):

    .. code-block:: bash

       pw ide cpp --ninja

    You can now examine the targets that are available to you:

    .. code-block:: bash

        pw ide cpp --list

    ... and select the target you want to use:

    .. code-block:: bash

        pw ide cpp --set host_clang

    As long as your editor or language server plugin is properly configured, you
    will now get code intelligence features relevant to that particular target.

    You can see what target is selected by running:

    .. code-block:: bash

        pw ide cpp

    Whenever you switch to a target you haven't used before, clangd will need to
    index the build, which may take several minutes. These indexes are cached,
    so you can switch between targets without re-indexing each time.

    If your build configuration changes significantly (e.g. you add a new file
    to the project), you will need to re-process the compilation database for
    that change to be recognized. Your target selection will not change, and
    your index will only need to be incrementally updated.

    You can generate the clangd command your editor needs to run with:

    .. code-block:: bash

        pw ide cpp --clangd-command

    If your editor uses JSON for configuration, you can export the same command
    in that format:

    .. code-block:: bash

        pw ide cpp --clangd-command-for json
    """
    _make_working_dir(reporter, pw_ide_settings, quiet=True)

    # If true, no arguments were provided so we do the default behavior.
    default = True

    build_dir = (
        build_dir if build_dir is not None else pw_ide_settings.build_dir
    )

    if compdb_file_paths is not None:
        should_process = True

        if len(compdb_file_paths) == 0:
            compdb_file_paths = pw_ide_settings.compdb_paths_expanded
    else:
        should_process = False
        # This simplifies typing in the rest of this method. We rely on
        # `should_process` instead of the status of this variable.
        compdb_file_paths = []

    # Order of operations matters from here on. It should be possible to run
    # a build system command to generate a compilation database, then process
    # the compilation database, then successfully set the target in a single
    # command.

    # Use Ninja to generate the initial compile_commands.json
    if should_run_ninja:
        default = False

        ninja_commands = ['ninja', '-t', 'compdb']
        reporter.info(f'Running Ninja: {" ".join(ninja_commands)}')

        output_compdb_file_path = build_dir / compdb_generate_file_path()

        try:
            # Ninja writes to STDOUT, so we capture to a file.
            with open(output_compdb_file_path, 'w') as compdb_file:
                result = subprocess.run(
                    ninja_commands,
                    cwd=build_dir,
                    stdout=compdb_file,
                    stderr=subprocess.PIPE,
                )
        except FileNotFoundError:
            reporter.err(f'Could not open path! {str(output_compdb_file_path)}')

        if result.returncode == 0:
            reporter.info('Ran Ninja successfully!')
            should_process = True
            compdb_file_paths.append(output_compdb_file_path)
        else:
            reporter.err('Something went wrong!')
            # Convert from bytes and remove trailing newline
            err = result.stderr.decode().split('\n')[:-1]

            for line in err:
                reporter.err(line)

            sys.exit(1)

    # Use GN to generate the initial compile_commands.json
    if should_run_gn:
        default = False

        gn_commands = ['gn', 'gen', str(build_dir), '--export-compile-commands']

        try:
            with open(build_dir / 'args.gn') as args_file:
                gn_args = [
                    line
                    for line in args_file.readlines()
                    if not line.startswith('#')
                ]
        except FileNotFoundError:
            gn_args = []

        gn_args_string = 'none' if len(gn_args) == 0 else ', '.join(gn_args)

        reporter.info(
            [f'Running GN: {" ".join(gn_commands)} (args: {gn_args_string})']
        )

        result = subprocess.run(gn_commands, capture_output=True)
        gn_status_lines = ['Ran GN successfully!']

        if result.returncode == 0:
            # Convert from bytes and remove trailing newline
            out = result.stdout.decode().split('\n')[:-1]

            for line in out:
                gn_status_lines.append(line)

            reporter.info(gn_status_lines)
            should_process = True
            output_compdb_file_path = build_dir / compdb_generate_file_path()
            compdb_file_paths.append(output_compdb_file_path)
        else:
            reporter.err('Something went wrong!')
            # Convert from bytes and remove trailing newline
            err = result.stderr.decode().split('\n')[:-1]

            for line in err:
                reporter.err(line)

            sys.exit(1)

    if should_process:
        default = False
        prev_targets = len(CppIdeFeaturesState(pw_ide_settings))
        compdb_databases: List[CppCompilationDatabasesMap] = []
        last_processed_path = Path()

        for compdb_file_path in compdb_file_paths:
            # If the path is a dir, append the default compile commands
            # file name.
            if compdb_file_path.is_dir():
                compdb_file_path /= compdb_generate_file_path()

            try:
                compdb_databases.append(
                    CppCompilationDatabase.load(
                        Path(compdb_file_path), build_dir
                    ).process(
                        settings=pw_ide_settings,
                        path_globs=pw_ide_settings.clangd_query_drivers(),
                    )
                )
            except MissingCompDbException:
                reporter.err(f'File not found: {str(compdb_file_path)}')

                if '*' in str(compdb_file_path):
                    reporter.wrn(
                        'It looks like you provided a glob that '
                        'did not match any files.'
                    )

                sys.exit(1)
            # TODO(chadnorvell): Recover more gracefully from errors.
            except BadCompDbException:
                reporter.err(
                    'File does not match compilation database format: '
                    f'{str(compdb_file_path)}'
                )
                sys.exit(1)

            last_processed_path = compdb_file_path

        if len(compdb_databases) == 0:
            reporter.err(
                'No compilation databases found in: '
                f'{str(compdb_file_paths)}'
            )
            sys.exit(1)

        try:
            CppCompilationDatabasesMap.merge(*compdb_databases).write()
        except TypeError:
            reporter.err('Could not serialize file to JSON!')

        total_targets = len(CppIdeFeaturesState(pw_ide_settings))
        new_targets = total_targets - prev_targets

        if len(compdb_file_paths) == 1:
            processed_text = str(last_processed_path)
        else:
            processed_text = f'{len(compdb_file_paths)} compilation databases'

        reporter.new(
            [
                f'Processed {processed_text} '
                f'to {pw_ide_settings.working_dir}',
                f'{total_targets} targets are now available '
                f'({new_targets} are new)',
            ]
        )

    if use_default_target:
        defined_default = pw_ide_settings.default_target
        max_commands_target: Optional[str] = None

        try:
            with open(
                pw_ide_settings.working_dir / MAX_COMMANDS_TARGET_FILENAME
            ) as max_commands_target_file:
                max_commands_target = max_commands_target_file.readline()
        except FileNotFoundError:
            pass

        if defined_default is None and max_commands_target is None:
            reporter.err('Can\'t use default target because none is defined!')
            reporter.wrn('Have you processed a compilation database yet?')
            sys.exit(1)

        target_to_set = (
            defined_default
            if defined_default is not None
            else max_commands_target
        )

    if target_to_set is not None:
        default = False

        # Always set the target if it's not already set, but if it is,
        # respect the --no-override flag.
        should_set_target = (
            CppIdeFeaturesState(pw_ide_settings).current_target is None
            or override_current_target
        )

        if should_set_target:
            try:
                CppIdeFeaturesState(
                    pw_ide_settings
                ).current_target = target_to_set
            except InvalidTargetException:
                reporter.err(
                    [
                        f'Invalid target! {target_to_set} not among the '
                        'defined targets.',
                        'Check .pw_ide.yaml or .pw_ide.user.yaml for defined '
                        'targets.',
                    ]
                )
                sys.exit(1)
            except MissingCompDbException:
                reporter.err(
                    [
                        f'File not found for target! {target_to_set}',
                        'Did you run pw ide cpp --process '
                        '{path to compile_commands.json}?',
                    ]
                )
                sys.exit(1)

            reporter.new(
                'Set C/C++ language server analysis target to: '
                f'{target_to_set}'
            )
        else:
            reporter.ok(
                'Target already is set and will not be overridden: '
                f'{CppIdeFeaturesState(pw_ide_settings).current_target}'
            )

    if clangd_command:
        default = False
        reporter.info(
            [
                'Command to run clangd with Pigweed paths:',
                ClangdSettings(pw_ide_settings).command(),
            ]
        )

    if clangd_command_system is not None:
        default = False
        reporter.info(
            [
                'Command to run clangd with Pigweed paths for '
                f'{clangd_command_system}:',
                ClangdSettings(pw_ide_settings).command(clangd_command_system),
            ]
        )

    if should_list_targets:
        default = False
        targets_list_status = [
            'C/C++ targets available for language server analysis:'
        ]

        for target in sorted(
            CppIdeFeaturesState(pw_ide_settings).enabled_available_targets
        ):
            targets_list_status.append(f'\t{target}')

        reporter.info(targets_list_status)

    if should_get_target or default:
        reporter.info(
            'Current C/C++ language server analysis target: '
            f'{CppIdeFeaturesState(pw_ide_settings).current_target}'
        )


def cmd_python(
    should_print_venv: bool, reporter: StatusReporter = StatusReporter()
) -> None:
    """Configure Python code intelligence support.

    You can generate the path to the Python virtual environment interpreter that
    your editor/language server should use with:

    .. code-block:: bash

       pw ide python --venv
    """
    # If true, no arguments were provided and we should do the default
    # behavior.
    default = True

    if should_print_venv or default:
        reporter.info(
            [
                'Location of the Pigweed Python virtual environment:',
                PythonPaths().interpreter,
            ]
        )
