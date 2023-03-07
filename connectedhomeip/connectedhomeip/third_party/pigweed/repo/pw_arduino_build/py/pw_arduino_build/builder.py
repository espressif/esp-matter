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
"""Extracts build information from Arduino cores."""

import glob
import logging
import os
import platform
import pprint
import re
import sys
import time
from collections import OrderedDict
from pathlib import Path
from typing import List

from pw_arduino_build import file_operations

_LOG = logging.getLogger(__name__)

_pretty_print = pprint.PrettyPrinter(indent=1, width=120).pprint
_pretty_format = pprint.PrettyPrinter(indent=1, width=120).pformat


def arduino_runtime_os_string():
    arduno_platform = {
        "Linux": "linux",
        "Windows": "windows",
        "Darwin": "macosx",
    }
    return arduno_platform[platform.system()]


class ArduinoBuilder:
    """Used to interpret arduino boards.txt and platform.txt files."""

    # pylint: disable=too-many-instance-attributes,too-many-public-methods

    BOARD_MENU_REGEX = re.compile(
        r"^(?P<name>menu\.[^#=]+)=(?P<description>.*)$", re.MULTILINE
    )

    BOARD_NAME_REGEX = re.compile(
        r"^(?P<name>[^\s#\.]+)\.name=(?P<description>.*)$", re.MULTILINE
    )

    VARIABLE_REGEX = re.compile(
        r"^(?P<name>[^\s#=]+)=(?P<value>.*)$", re.MULTILINE
    )

    MENU_OPTION_REGEX = re.compile(
        r"^menu\."  # starts with "menu"
        r"(?P<menu_option_name>[^.]+)\."  # first token after .
        r"(?P<menu_option_value>[^.]+)$"
    )  # second (final) token after .

    TOOL_NAME_REGEX = re.compile(
        r"^tools\." r"(?P<tool_name>[^.]+)\."  # starts with "tools"
    )  # first token after .

    INTERPOLATED_VARIABLE_REGEX = re.compile(r"{[^}]+}", re.MULTILINE)

    OBJCOPY_STEP_NAME_REGEX = re.compile(r"^recipe.objcopy.([^.]+).pattern$")

    def __init__(
        self,
        arduino_path,
        package_name,
        build_path=None,
        project_path=None,
        project_source_path=None,
        library_path=None,
        library_names=None,
        build_project_name=None,
        compiler_path_override=False,
    ):
        self.arduino_path = arduino_path
        self.arduino_package_name = package_name
        self.selected_board = None
        self.build_path = build_path
        self.project_path = project_path
        self.project_source_path = project_source_path
        self.build_project_name = build_project_name
        self.compiler_path_override = compiler_path_override
        self.variant_includes = ""
        self.build_variant_path = False
        self.library_names = library_names
        self.library_path = library_path

        self.compiler_path_override_binaries = []
        if self.compiler_path_override:
            self.compiler_path_override_binaries = file_operations.find_files(
                self.compiler_path_override, "*"
            )

        # Container dicts for boards.txt and platform.txt file data.
        self.board = OrderedDict()
        self.platform = OrderedDict()
        self.menu_options = OrderedDict(
            {"global_options": {}, "default_board_values": {}, "selected": {}}
        )
        self.tools_variables = {}

        # Set and check for valid hardware folder.
        self.hardware_path = os.path.join(self.arduino_path, "hardware")

        if not os.path.exists(self.hardware_path):
            raise FileNotFoundError(
                "Arduino package path '{}' does not exist.".format(
                    self.hardware_path
                )
            )

        # Set and check for valid package name
        self.package_path = os.path.join(
            self.arduino_path, "hardware", package_name
        )
        # {build.arch} is the first folder name of the package (upcased)
        self.build_arch = os.path.split(package_name)[0].upper()

        if not os.path.exists(self.package_path):
            _LOG.error(
                "Error: Arduino package name '%s' does not exist.", package_name
            )
            _LOG.error("Did you mean:\n")
            # TODO(tonymd): On Windows concatenating "/" may not work
            possible_alternatives = [
                d.replace(self.hardware_path + os.sep, "", 1)
                for d in glob.glob(self.hardware_path + "/*/*")
            ]
            _LOG.error("\n".join(possible_alternatives))
            sys.exit(1)

        # Populate library paths.
        if not library_path:
            self.library_path = []
        # Append core libraries directory.
        core_lib_path = Path(self.package_path) / "libraries"
        if core_lib_path.is_dir():
            self.library_path.append(Path(self.package_path) / "libraries")
        if library_path:
            self.library_path = [
                os.path.realpath(os.path.expanduser(os.path.expandvars(l_path)))
                for l_path in library_path
            ]

        # Grab all folder names in the cores directory. These are typically
        # sub-core source files.
        self.sub_core_folders = os.listdir(
            os.path.join(self.package_path, "cores")
        )

        self._find_tools_variables()

        self.boards_txt = os.path.join(self.package_path, "boards.txt")
        self.platform_txt = os.path.join(self.package_path, "platform.txt")

    def select_board(self, board_name, menu_option_overrides=False):
        self.selected_board = board_name

        # Load default menu options for a selected board.
        if not self.selected_board in self.board.keys():
            _LOG.error("Error board: '%s' not supported.", self.selected_board)
            # TODO(tonymd): Print supported boards here
            sys.exit(1)

        # Override default menu options if any are specified.
        if menu_option_overrides:
            for moption in menu_option_overrides:
                if not self.set_menu_option(moption):
                    # TODO(tonymd): Print supported menu options here
                    sys.exit(1)

        self._copy_default_menu_options_to_build_variables()
        self._apply_recipe_overrides()
        self._substitute_variables()

    def set_variables(self, variable_list: List[str]):
        # Convert the string list containing 'name=value' items into a dict
        variable_source = {}
        for var in variable_list:
            var_name, value = var.split("=")
            variable_source[var_name] = value

        # Replace variables in platform
        for var, value in self.platform.items():
            self.platform[var] = self._replace_variables(value, variable_source)

    def _apply_recipe_overrides(self):
        # Override link recipes with per-core exceptions
        # Teensyduino cores
        if self.build_arch == "TEENSY":
            # Change {build.path}/{archive_file}
            # To {archive_file_path} (which should contain the core.a file)
            new_link_line = self.platform["recipe.c.combine.pattern"].replace(
                "{object_files} \"{build.path}/{archive_file}\"",
                "{object_files} {archive_file_path}",
                1,
            )
            # Add the teensy provided toolchain lib folder for link access to
            # libarm_cortexM*_math.a
            new_link_line = new_link_line.replace(
                "\"-L{build.path}\"",
                "\"-L{build.path}\" -L{compiler.path}/arm/arm-none-eabi/lib",
                1,
            )
            self.platform["recipe.c.combine.pattern"] = new_link_line
            # Remove the pre-compiled header include
            self.platform["recipe.cpp.o.pattern"] = self.platform[
                "recipe.cpp.o.pattern"
            ].replace("\"-I{build.path}/pch\"", "", 1)

        # Adafruit-samd core
        # TODO(tonymd): This build_arch may clash with Arduino-SAMD core
        elif self.build_arch == "SAMD":
            new_link_line = self.platform["recipe.c.combine.pattern"].replace(
                "\"{build.path}/{archive_file}\" -Wl,--end-group",
                "{archive_file_path} -Wl,--end-group",
                1,
            )
            self.platform["recipe.c.combine.pattern"] = new_link_line

        # STM32L4 Core:
        # https://github.com/GrumpyOldPizza/arduino-STM32L4
        elif self.build_arch == "STM32L4":
            # TODO(tonymd): {build.path}/{archive_file} for the link step always
            # seems to be core.a (except STM32 core)
            line_to_delete = "-Wl,--start-group \"{build.path}/{archive_file}\""
            new_link_line = self.platform["recipe.c.combine.pattern"].replace(
                line_to_delete, "-Wl,--start-group {archive_file_path}", 1
            )
            self.platform["recipe.c.combine.pattern"] = new_link_line

        # stm32duino core
        elif self.build_arch == "STM32":
            # Must link in SrcWrapper for all projects.
            if not self.library_names:
                self.library_names = []
            self.library_names.append("SrcWrapper")

    def _copy_default_menu_options_to_build_variables(self):
        # Clear existing options
        self.menu_options["selected"] = {}
        # Set default menu options for selected board
        for menu_key, menu_dict in self.menu_options["default_board_values"][
            self.selected_board
        ].items():
            for name, var in self.board[self.selected_board].items():
                starting_key = "{}.{}.".format(menu_key, menu_dict["name"])
                if name.startswith(starting_key):
                    new_var_name = name.replace(starting_key, "", 1)
                    self.menu_options["selected"][new_var_name] = var

    def set_menu_option(self, moption):
        if moption not in self.board[self.selected_board]:
            _LOG.error("Error: '%s' is not a valid menu option.", moption)
            return False

        # Override default menu option with new value.
        menu_match_result = self.MENU_OPTION_REGEX.match(moption)
        if menu_match_result:
            menu_match = menu_match_result.groupdict()
            menu_value = menu_match["menu_option_value"]
            menu_key = "menu.{}".format(menu_match["menu_option_name"])
            self.menu_options["default_board_values"][self.selected_board][
                menu_key
            ]["name"] = menu_value

        # Update build variables
        self._copy_default_menu_options_to_build_variables()
        return True

    def _set_global_arduino_variables(self):
        """Set some global variables defined by the Arduino-IDE.

        See Docs:
        https://arduino.github.io/arduino-cli/platform-specification/#global-predefined-properties
        """

        # TODO(tonymd): Make sure these variables are replaced in recipe lines
        # even if they are None: build_path, project_path, project_source_path,
        # build_project_name
        for current_board in self.board.values():
            if self.build_path:
                current_board["build.path"] = self.build_path
            if self.build_project_name:
                current_board["build.project_name"] = self.build_project_name
                # {archive_file} is the final *.elf
                archive_file = "{}.elf".format(self.build_project_name)
                current_board["archive_file"] = archive_file
                # {archive_file_path} is the final core.a archive
                if self.build_path:
                    current_board["archive_file_path"] = os.path.join(
                        self.build_path, "core.a"
                    )
            if self.project_source_path:
                current_board["build.source.path"] = self.project_source_path

            current_board["extra.time.local"] = str(int(time.time()))
            current_board["runtime.ide.version"] = "10812"
            current_board["runtime.hardware.path"] = self.hardware_path

            # Copy {runtime.tools.TOOL_NAME.path} vars
            self._set_tools_variables(current_board)

            current_board["runtime.platform.path"] = self.package_path
            if self.platform["name"] == "Teensyduino":
                # Teensyduino is installed into the arduino IDE folder
                # rather than ~/.arduino15/packages/
                current_board["runtime.hardware.path"] = os.path.join(
                    self.hardware_path, "teensy"
                )

            current_board["build.system.path"] = os.path.join(
                self.package_path, "system"
            )

            # Set the {build.core.path} variable that pointing to a sub-core
            # folder. For Teensys this is:
            # 'teensy/hardware/teensy/avr/cores/teensy{3,4}'. For other cores
            # it's typically just the 'arduino' folder. For example:
            # 'arduino-samd/hardware/samd/1.8.8/cores/arduino'
            core_path = Path(self.package_path) / "cores"
            core_path /= current_board.get(
                "build.core", self.sub_core_folders[0]
            )
            current_board["build.core.path"] = core_path.as_posix()

            current_board["build.arch"] = self.build_arch

            for name, var in current_board.items():
                current_board[name] = var.replace(
                    "{build.core.path}", core_path.as_posix()
                )

    def load_board_definitions(self):
        """Loads Arduino boards.txt and platform.txt files into dictionaries.

        Populates the following dictionaries:
            self.menu_options
            self.boards
            self.platform
        """
        # Load platform.txt
        with open(self.platform_txt, "r") as pfile:
            platform_file = pfile.read()
            platform_var_matches = self.VARIABLE_REGEX.finditer(platform_file)
            for p_match in [m.groupdict() for m in platform_var_matches]:
                self.platform[p_match["name"]] = p_match["value"]

        # Load boards.txt
        with open(self.boards_txt, "r") as bfile:
            board_file = bfile.read()
            # Get all top-level menu options, e.g. menu.usb=USB Type
            board_menu_matches = self.BOARD_MENU_REGEX.finditer(board_file)
            for menuitem in [m.groupdict() for m in board_menu_matches]:
                self.menu_options["global_options"][menuitem["name"]] = {
                    "description": menuitem["description"]
                }

            # Get all board names, e.g. teensy40.name=Teensy 4.0
            board_name_matches = self.BOARD_NAME_REGEX.finditer(board_file)
            for b_match in [m.groupdict() for m in board_name_matches]:
                self.board[b_match["name"]] = OrderedDict()
                self.menu_options["default_board_values"][
                    b_match["name"]
                ] = OrderedDict()

            # Get all board variables, e.g. teensy40.*
            for current_board_name, current_board in self.board.items():
                board_line_matches = re.finditer(
                    fr"^\s*{current_board_name}\."
                    fr"(?P<key>[^#=]+)=(?P<value>.*)$",
                    board_file,
                    re.MULTILINE,
                )
                for b_match in [m.groupdict() for m in board_line_matches]:
                    # Check if this line is a menu option
                    # (e.g. 'menu.usb.serial') and save as default if it's the
                    # first one seen.
                    ArduinoBuilder.save_default_menu_option(
                        current_board_name,
                        b_match["key"],
                        b_match["value"],
                        self.menu_options,
                    )
                    current_board[b_match["key"]] = b_match["value"].strip()

            self._set_global_arduino_variables()

    @staticmethod
    def save_default_menu_option(current_board_name, key, value, menu_options):
        """Save a given menu option as the default.

        Saves the key and value into menu_options["default_board_values"]
        if it doesn't already exist. Assumes menu options are added in the order
        specified in boards.txt. The first value for a menu key is the default.
        """
        # Check if key is a menu option
        # e.g. menu.usb.serial
        #      menu.usb.serial.build.usbtype
        menu_match_result = re.match(
            r'^menu\.'  # starts with "menu"
            r'(?P<menu_option_name>[^.]+)\.'  # first token after .
            r'(?P<menu_option_value>[^.]+)'  # second token after .
            r'(\.(?P<rest>.+))?',  # optionally any trailing tokens after a .
            key,
        )
        if menu_match_result:
            menu_match = menu_match_result.groupdict()
            current_menu_key = "menu.{}".format(menu_match["menu_option_name"])
            # If this is the first menu option seen for current_board_name, save
            # as the default.
            if (
                current_menu_key
                not in menu_options["default_board_values"][current_board_name]
            ):
                menu_options["default_board_values"][current_board_name][
                    current_menu_key
                ] = {
                    "name": menu_match["menu_option_value"],
                    "description": value,
                }

    def _replace_variables(self, line, variable_lookup_source):
        """Replace {variables} from loaded boards.txt or platform.txt.

        Replace interpolated variables surrounded by curly braces in line with
        definitions from variable_lookup_source.
        """
        new_line = line
        for current_var_match in self.INTERPOLATED_VARIABLE_REGEX.findall(line):
            # {build.flags.c} --> build.flags.c
            current_var = current_var_match.strip("{}")

            # check for matches in board definition
            if current_var in variable_lookup_source:
                replacement = variable_lookup_source.get(current_var, "")
                new_line = new_line.replace(current_var_match, replacement)
        return new_line

    def _find_tools_variables(self):
        # Gather tool directories in order of increasing precedence
        runtime_tool_paths = []

        # Check for tools installed in ~/.arduino15/packages/arduino/tools/
        # TODO(tonymd): Is this Mac & Linux specific?
        runtime_tool_paths += glob.glob(
            os.path.join(
                os.path.realpath(os.path.expanduser(os.path.expandvars("~"))),
                ".arduino15",
                "packages",
                "arduino",
                "tools",
                "*",
            )
        )

        # <ARDUINO_PATH>/tools/<OS_STRING>/<TOOL_NAMES>
        runtime_tool_paths += glob.glob(
            os.path.join(
                self.arduino_path, "tools", arduino_runtime_os_string(), "*"
            )
        )
        # <ARDUINO_PATH>/tools/<TOOL_NAMES>
        # This will grab linux/windows/macosx/share as <TOOL_NAMES>.
        runtime_tool_paths += glob.glob(
            os.path.join(self.arduino_path, "tools", "*")
        )

        # Process package tools after arduino tools.
        # They should overwrite vars & take precedence.

        # <PACKAGE_PATH>/tools/<OS_STRING>/<TOOL_NAMES>
        runtime_tool_paths += glob.glob(
            os.path.join(
                self.package_path, "tools", arduino_runtime_os_string(), "*"
            )
        )
        # <PACKAGE_PATH>/tools/<TOOL_NAMES>
        # This will grab linux/windows/macosx/share as <TOOL_NAMES>.
        runtime_tool_paths += glob.glob(
            os.path.join(self.package_path, "tools", "*")
        )

        for path in runtime_tool_paths:
            # Make sure TOOL_NAME is not an OS string
            if not (
                path.endswith("linux")
                or path.endswith("windows")
                or path.endswith("macosx")
                or path.endswith("share")
            ):
                # TODO(tonymd): Check if a file & do nothing?

                # Check if it's a directory with subdir(s) as a version string
                #   create all 'runtime.tools.{tool_folder}-{version.path}'
                #     (for each version)
                #   create 'runtime.tools.{tool_folder}.path'
                #     (with latest version)
                if os.path.isdir(path):
                    # Grab the tool name (folder) by itself.
                    tool_folder = os.path.basename(path)
                    # Sort so that [-1] is the latest version.
                    version_paths = sorted(glob.glob(os.path.join(path, "*")))
                    # Check if all sub folders start with a version string.
                    if len(version_paths) == sum(
                        bool(re.match(r"^[0-9.]+", os.path.basename(vp)))
                        for vp in version_paths
                    ):
                        for version_path in version_paths:
                            version_string = os.path.basename(version_path)
                            var_name = "runtime.tools.{}-{}.path".format(
                                tool_folder, version_string
                            )
                            self.tools_variables[var_name] = os.path.join(
                                path, version_string
                            )
                        var_name = "runtime.tools.{}.path".format(tool_folder)
                        self.tools_variables[var_name] = os.path.join(
                            path, os.path.basename(version_paths[-1])
                        )
                    # Else set toolpath to path.
                    else:
                        var_name = "runtime.tools.{}.path".format(tool_folder)
                        self.tools_variables[var_name] = path

        _LOG.debug("TOOL VARIABLES: %s", _pretty_format(self.tools_variables))

    # Copy self.tools_variables into destination
    def _set_tools_variables(self, destination):
        for key, value in self.tools_variables.items():
            destination[key] = value

    def _substitute_variables(self):
        """Perform variable substitution in board and platform metadata."""

        # menu -> board
        # Copy selected menu variables into board definiton
        for name, value in self.menu_options["selected"].items():
            self.board[self.selected_board][name] = value

        # board -> board
        # Replace any {vars} in the selected board with values defined within
        # (and from copied in menu options).
        for var, value in self.board[self.selected_board].items():
            self.board[self.selected_board][var] = self._replace_variables(
                value, self.board[self.selected_board]
            )

        # Check for build.variant variable
        # This will be set in selected board after menu options substitution
        build_variant = self.board[self.selected_board].get(
            "build.variant", None
        )
        if build_variant:
            # Set build.variant.path
            bvp = os.path.join(self.package_path, "variants", build_variant)
            self.build_variant_path = bvp
            self.board[self.selected_board]["build.variant.path"] = bvp
            # Add the variant folder as an include directory
            # (used in stm32l4 core)
            self.variant_includes = "-I{}".format(bvp)

        _LOG.debug("PLATFORM INITIAL: %s", _pretty_format(self.platform))

        # board -> platform
        # Replace {vars} in platform from the selected board definition
        for var, value in self.platform.items():
            self.platform[var] = self._replace_variables(
                value, self.board[self.selected_board]
            )

        # platform -> platform
        # Replace any remaining {vars} in platform from platform
        for var, value in self.platform.items():
            self.platform[var] = self._replace_variables(value, self.platform)

        # Repeat platform -> platform for any lingering variables
        # Example: {build.opt.name} in STM32 core
        for var, value in self.platform.items():
            self.platform[var] = self._replace_variables(value, self.platform)

        _LOG.debug("MENU_OPTIONS: %s", _pretty_format(self.menu_options))
        _LOG.debug(
            "SELECTED_BOARD: %s",
            _pretty_format(self.board[self.selected_board]),
        )
        _LOG.debug("PLATFORM: %s", _pretty_format(self.platform))

    def selected_board_spec(self):
        return self.board[self.selected_board]

    def get_menu_options(self):
        all_options = []
        max_string_length = [0, 0]

        for key_name, description in self.board[self.selected_board].items():
            menu_match_result = self.MENU_OPTION_REGEX.match(key_name)
            if menu_match_result:
                menu_match = menu_match_result.groupdict()
                name = "menu.{}.{}".format(
                    menu_match["menu_option_name"],
                    menu_match["menu_option_value"],
                )
                if len(name) > max_string_length[0]:
                    max_string_length[0] = len(name)
                if len(description) > max_string_length[1]:
                    max_string_length[1] = len(description)
                all_options.append((name, description))

        return all_options, max_string_length

    def get_default_menu_options(self):
        default_options = []
        max_string_length = [0, 0]

        for key_name, value in self.menu_options["default_board_values"][
            self.selected_board
        ].items():
            full_key = key_name + "." + value["name"]
            if len(full_key) > max_string_length[0]:
                max_string_length[0] = len(full_key)
            if len(value["description"]) > max_string_length[1]:
                max_string_length[1] = len(value["description"])
            default_options.append((full_key, value["description"]))

        return default_options, max_string_length

    @staticmethod
    def split_binary_from_arguments(compile_line):
        compile_binary = None
        rest_of_line = compile_line

        compile_binary_match = re.search(r'^("[^"]+") ', compile_line)
        if compile_binary_match:
            compile_binary = compile_binary_match[1]
            rest_of_line = compile_line.replace(compile_binary_match[0], "", 1)

        return compile_binary, rest_of_line

    def _strip_includes_source_file_object_file_vars(self, compile_line):
        line = compile_line
        if self.variant_includes:
            line = compile_line.replace(
                "{includes} \"{source_file}\" -o \"{object_file}\"",
                self.variant_includes,
                1,
            )
        else:
            line = compile_line.replace(
                "{includes} \"{source_file}\" -o \"{object_file}\"", "", 1
            )
        return line

    def _get_tool_name(self, line):
        tool_match_result = self.TOOL_NAME_REGEX.match(line)
        if tool_match_result:
            return tool_match_result[1]
        return False

    def get_upload_tool_names(self):
        return [
            self._get_tool_name(t)
            for t in self.platform.keys()
            if self.TOOL_NAME_REGEX.match(t) and 'upload.pattern' in t
        ]

    # TODO(tonymd): Use these getters in _replace_variables() or
    # _substitute_variables()

    def _get_platform_variable(self, variable):
        # TODO(tonymd): Check for '.macos' '.linux' '.windows' in variable name,
        # compare with platform.system() and return that instead.
        return self.platform.get(variable, False)

    def _get_platform_variable_with_substitutions(self, variable, namespace):
        line = self.platform.get(variable, False)
        # Get all unique variables used in this line in line.
        unique_vars = sorted(
            set(self.INTERPOLATED_VARIABLE_REGEX.findall(line))
        )
        # Search for each unique_vars in namespace and global.
        for var in unique_vars:
            v_raw_name = var.strip("{}")

            # Check for namespace.variable
            #   eg: 'tools.stm32CubeProg.cmd'
            possible_var_name = "{}.{}".format(namespace, v_raw_name)
            result = self._get_platform_variable(possible_var_name)
            # Check for os overriden variable
            #   eg:
            #     ('tools.stm32CubeProg.cmd', 'stm32CubeProg.sh'),
            #     ('tools.stm32CubeProg.cmd.windows', 'stm32CubeProg.bat'),
            possible_var_name = "{}.{}.{}".format(
                namespace, v_raw_name, arduino_runtime_os_string()
            )
            os_override_result = self._get_platform_variable(possible_var_name)

            if os_override_result:
                line = line.replace(var, os_override_result)
            elif result:
                line = line.replace(var, result)
            # Check for variable at top level?
            # elif self._get_platform_variable(v_raw_name):
            #     line = line.replace(self._get_platform_variable(v_raw_name),
            #                         result)
        return line

    def get_upload_line(self, tool_name, serial_port=False):
        """TODO(tonymd) Add docstring."""
        # TODO(tonymd): Error if tool_name does not exist
        tool_namespace = "tools.{}".format(tool_name)
        pattern = "tools.{}.upload.pattern".format(tool_name)

        if not self._get_platform_variable(pattern):
            _LOG.error("Error: upload tool '%s' does not exist.", tool_name)
            tools = self.get_upload_tool_names()
            _LOG.error("Valid tools: %s", ", ".join(tools))
            return sys.exit(1)

        line = self._get_platform_variable_with_substitutions(
            pattern, tool_namespace
        )

        # TODO(tonymd): Teensy specific tool overrides.
        if tool_name == "teensyloader":
            # Remove un-necessary lines
            # {serial.port.label} and {serial.port.protocol} are returned by
            # the teensy_ports binary.
            line = line.replace("\"-portlabel={serial.port.label}\"", "", 1)
            line = line.replace(
                "\"-portprotocol={serial.port.protocol}\"", "", 1
            )

            if serial_port == "UNKNOWN" or not serial_port:
                line = line.replace('"-port={serial.port}"', "", 1)
            else:
                line = line.replace("{serial.port}", serial_port, 1)

        return line

    def _get_binary_path(self, variable_pattern):
        compile_line = self.replace_compile_binary_with_override_path(
            self._get_platform_variable(variable_pattern)
        )
        compile_binary, _ = ArduinoBuilder.split_binary_from_arguments(
            compile_line
        )
        return compile_binary

    def get_cc_binary(self):
        return self._get_binary_path("recipe.c.o.pattern")

    def get_cxx_binary(self):
        return self._get_binary_path("recipe.cpp.o.pattern")

    def get_objcopy_binary(self):
        objcopy_step_name = self.get_objcopy_step_names()[0]
        objcopy_binary = self._get_binary_path(objcopy_step_name)
        return objcopy_binary

    def get_ar_binary(self):
        return self._get_binary_path("recipe.ar.pattern")

    def get_size_binary(self):
        return self._get_binary_path("recipe.size.pattern")

    def replace_command_args_with_compiler_override_path(self, compile_line):
        if not self.compiler_path_override:
            return compile_line
        replacement_line = compile_line
        replacement_line_args = compile_line.split()
        for arg in replacement_line_args:
            compile_binary_basename = os.path.basename(arg.strip("\""))
            if compile_binary_basename in self.compiler_path_override_binaries:
                new_compiler = os.path.join(
                    self.compiler_path_override, compile_binary_basename
                )
                replacement_line = replacement_line.replace(
                    arg, new_compiler, 1
                )
        return replacement_line

    def replace_compile_binary_with_override_path(self, compile_line):
        replacement_compile_line = compile_line

        # Change the compiler path if there's an override path set
        if self.compiler_path_override:
            compile_binary, line = ArduinoBuilder.split_binary_from_arguments(
                compile_line
            )
            compile_binary_basename = os.path.basename(
                compile_binary.strip("\"")
            )
            new_compiler = os.path.join(
                self.compiler_path_override, compile_binary_basename
            )
            if platform.system() == "Windows" and not re.match(
                r".*\.exe$", new_compiler, flags=re.IGNORECASE
            ):
                new_compiler += ".exe"

            if os.path.isfile(new_compiler):
                replacement_compile_line = "\"{}\" {}".format(
                    new_compiler, line
                )

        return replacement_compile_line

    def get_c_compile_line(self):
        _LOG.debug(
            "ARDUINO_C_COMPILE: %s",
            _pretty_format(self.platform["recipe.c.o.pattern"]),
        )

        compile_line = self.platform["recipe.c.o.pattern"]
        compile_line = self._strip_includes_source_file_object_file_vars(
            compile_line
        )
        compile_line += " -I{}".format(
            self.board[self.selected_board]["build.core.path"]
        )

        compile_line = self.replace_compile_binary_with_override_path(
            compile_line
        )
        return compile_line

    def get_s_compile_line(self):
        _LOG.debug(
            "ARDUINO_S_COMPILE %s",
            _pretty_format(self.platform["recipe.S.o.pattern"]),
        )

        compile_line = self.platform["recipe.S.o.pattern"]
        compile_line = self._strip_includes_source_file_object_file_vars(
            compile_line
        )
        compile_line += " -I{}".format(
            self.board[self.selected_board]["build.core.path"]
        )

        compile_line = self.replace_compile_binary_with_override_path(
            compile_line
        )
        return compile_line

    def get_ar_compile_line(self):
        _LOG.debug(
            "ARDUINO_AR_COMPILE: %s",
            _pretty_format(self.platform["recipe.ar.pattern"]),
        )

        compile_line = self.platform["recipe.ar.pattern"].replace(
            "\"{object_file}\"", "", 1
        )

        compile_line = self.replace_compile_binary_with_override_path(
            compile_line
        )
        return compile_line

    def get_cpp_compile_line(self):
        _LOG.debug(
            "ARDUINO_CPP_COMPILE: %s",
            _pretty_format(self.platform["recipe.cpp.o.pattern"]),
        )

        compile_line = self.platform["recipe.cpp.o.pattern"]
        compile_line = self._strip_includes_source_file_object_file_vars(
            compile_line
        )
        compile_line += " -I{}".format(
            self.board[self.selected_board]["build.core.path"]
        )

        compile_line = self.replace_compile_binary_with_override_path(
            compile_line
        )
        return compile_line

    def get_link_line(self):
        _LOG.debug(
            "ARDUINO_LINK: %s",
            _pretty_format(self.platform["recipe.c.combine.pattern"]),
        )

        compile_line = self.platform["recipe.c.combine.pattern"]

        compile_line = self.replace_compile_binary_with_override_path(
            compile_line
        )
        return compile_line

    def get_objcopy_step_names(self):
        names = [
            name
            for name, line in self.platform.items()
            if self.OBJCOPY_STEP_NAME_REGEX.match(name)
        ]
        return names

    def get_objcopy_steps(self) -> List[str]:
        lines = [
            line
            for name, line in self.platform.items()
            if self.OBJCOPY_STEP_NAME_REGEX.match(name)
        ]
        lines = [
            self.replace_compile_binary_with_override_path(line)
            for line in lines
        ]
        return lines

    # TODO(tonymd): These recipes are probably run in sorted order
    def get_objcopy(self, suffix):
        # Expected vars:
        # teensy:
        #   recipe.objcopy.eep.pattern
        #   recipe.objcopy.hex.pattern

        pattern = "recipe.objcopy.{}.pattern".format(suffix)
        objcopy_step_names = self.get_objcopy_step_names()

        objcopy_suffixes = [
            m[1]
            for m in [
                self.OBJCOPY_STEP_NAME_REGEX.match(line)
                for line in objcopy_step_names
            ]
            if m
        ]
        if pattern not in objcopy_step_names:
            _LOG.error("Error: objcopy suffix '%s' does not exist.", suffix)
            _LOG.error("Valid suffixes: %s", ", ".join(objcopy_suffixes))
            return sys.exit(1)

        line = self._get_platform_variable(pattern)

        _LOG.debug("ARDUINO_OBJCOPY_%s: %s", suffix, line)

        line = self.replace_compile_binary_with_override_path(line)

        return line

    def get_objcopy_flags(self, suffix):
        # TODO(tonymd): Possibly teensy specific variables.
        flags = ""
        if suffix == "hex":
            flags = self.platform.get("compiler.elf2hex.flags", "")
        elif suffix == "bin":
            flags = self.platform.get("compiler.elf2bin.flags", "")
        elif suffix == "eep":
            flags = self.platform.get("compiler.objcopy.eep.flags", "")
        return flags

    # TODO(tonymd): There are more recipe hooks besides postbuild.
    #   They are run in sorted order.
    # TODO(tonymd): Rename this to get_hooks(hook_name, step).
    # TODO(tonymd): Add a list-hooks and or run-hooks command
    def get_postbuild_line(self, step_number):
        line = self.platform[
            "recipe.hooks.postbuild.{}.pattern".format(step_number)
        ]
        line = self.replace_command_args_with_compiler_override_path(line)
        return line

    def get_prebuild_steps(self) -> List[str]:
        # Teensy core uses recipe.hooks.sketch.prebuild.1.pattern
        # stm32 core uses recipe.hooks.prebuild.1.pattern
        # TODO(tonymd): STM32 core uses recipe.hooks.prebuild.1.pattern.windows
        #   (should override non-windows key)
        lines = [
            line
            for name, line in self.platform.items()
            if re.match(
                r"^recipe.hooks.(?:sketch.)?prebuild.[^.]+.pattern$", name
            )
        ]
        # TODO(tonymd): Write a function to fetch/replace OS specific patterns
        #   (ending in an OS string)
        lines = [
            self.replace_compile_binary_with_override_path(line)
            for line in lines
        ]
        return lines

    def get_postbuild_steps(self) -> List[str]:
        lines = [
            line
            for name, line in self.platform.items()
            if re.match(r"^recipe.hooks.postbuild.[^.]+.pattern$", name)
        ]

        lines = [
            self.replace_command_args_with_compiler_override_path(line)
            for line in lines
        ]
        return lines

    def get_s_flags(self):
        compile_line = self.get_s_compile_line()
        _, compile_line = ArduinoBuilder.split_binary_from_arguments(
            compile_line
        )
        compile_line = compile_line.replace("-c", "", 1)
        return compile_line.strip()

    def get_c_flags(self):
        compile_line = self.get_c_compile_line()
        _, compile_line = ArduinoBuilder.split_binary_from_arguments(
            compile_line
        )
        compile_line = compile_line.replace("-c", "", 1)
        return compile_line.strip()

    def get_cpp_flags(self):
        compile_line = self.get_cpp_compile_line()
        _, compile_line = ArduinoBuilder.split_binary_from_arguments(
            compile_line
        )
        compile_line = compile_line.replace("-c", "", 1)
        return compile_line.strip()

    def get_ar_flags(self):
        compile_line = self.get_ar_compile_line()
        _, compile_line = ArduinoBuilder.split_binary_from_arguments(
            compile_line
        )
        return compile_line.strip()

    def get_ld_flags(self):
        compile_line = self.get_link_line()
        _, compile_line = ArduinoBuilder.split_binary_from_arguments(
            compile_line
        )

        # TODO(tonymd): This is teensy specific
        line_to_delete = (
            "-o \"{build.path}/{build.project_name}.elf\" "
            "{object_files} \"-L{build.path}\""
        )
        if self.build_path:
            line_to_delete = line_to_delete.replace(
                "{build.path}", self.build_path
            )
        if self.build_project_name:
            line_to_delete = line_to_delete.replace(
                "{build.project_name}", self.build_project_name
            )

        compile_line = compile_line.replace(line_to_delete, "", 1)
        libs = re.findall(r'(-l[^ ]+ ?)', compile_line)
        for lib in libs:
            compile_line = compile_line.replace(lib, "", 1)
        libs = [lib.strip() for lib in libs]

        return compile_line.strip()

    def get_ld_libs(self, name_only=False):
        compile_line = self.get_link_line()
        libs = re.findall(r'(?P<arg>-l(?P<name>[^ ]+) ?)', compile_line)
        if name_only:
            libs = [lib_name.strip() for lib_arg, lib_name in libs]
        else:
            libs = [lib_arg.strip() for lib_arg, lib_name in libs]
        return " ".join(libs)

    def library_folders(self):
        """TODO(tonymd) Add docstring."""
        # Arduino library format documentation:
        # https://arduino.github.io/arduino-cli/library-specification/#layout-of-folders-and-files
        # - If src folder exists,
        #   use that as the root include directory -Ilibraries/libname/src
        # - Else lib folder as root include -Ilibraries/libname
        #   (exclude source files in the examples folder in this case)

        if not self.library_names or not self.library_path:
            return []

        folder_patterns = ["*"]
        if self.library_names:
            folder_patterns = self.library_names

        library_folders = OrderedDict()
        for library_dir in self.library_path:
            found_library_names = file_operations.find_files(
                library_dir, folder_patterns, directories_only=True
            )
            _LOG.debug(
                "Found Libraries %s: %s", library_dir, found_library_names
            )
            for lib_name in found_library_names:
                lib_dir = os.path.join(library_dir, lib_name)
                src_dir = os.path.join(lib_dir, "src")
                if os.path.exists(src_dir) and os.path.isdir(src_dir):
                    library_folders[lib_name] = src_dir
                else:
                    library_folders[lib_name] = lib_dir

        return list(library_folders.values())

    def library_include_dirs(self):
        return [Path(lib).as_posix() for lib in self.library_folders()]

    def library_includes(self):
        include_args = []
        library_folders = self.library_folders()
        for lib_dir in library_folders:
            include_args.append("-I{}".format(os.path.relpath(lib_dir)))
        return include_args

    def library_files(self, pattern, only_library_name=None):
        sources = []
        library_folders = self.library_folders()
        if only_library_name:
            library_folders = [
                lf for lf in self.library_folders() if only_library_name in lf
            ]
        for lib_dir in library_folders:
            for file_path in file_operations.find_files(lib_dir, [pattern]):
                if not file_path.startswith("examples"):
                    sources.append((Path(lib_dir) / file_path).as_posix())
        return sources

    def library_c_files(self):
        return self.library_files("**/*.c")

    def library_s_files(self):
        return self.library_files("**/*.S")

    def library_cpp_files(self):
        return self.library_files("**/*.cpp")

    def get_core_path(self):
        return self.board[self.selected_board]["build.core.path"]

    def core_files(self, pattern):
        sources = []
        for file_path in file_operations.find_files(
            self.get_core_path(), [pattern]
        ):
            sources.append(os.path.join(self.get_core_path(), file_path))
        return sources

    def core_c_files(self):
        return self.core_files("**/*.c")

    def core_s_files(self):
        return self.core_files("**/*.S")

    def core_cpp_files(self):
        return self.core_files("**/*.cpp")

    def get_variant_path(self):
        return self.build_variant_path

    def variant_files(self, pattern):
        sources = []
        if self.build_variant_path:
            for file_path in file_operations.find_files(
                self.get_variant_path(), [pattern]
            ):
                sources.append(os.path.join(self.get_variant_path(), file_path))
        return sources

    def variant_c_files(self):
        return self.variant_files("**/*.c")

    def variant_s_files(self):
        return self.variant_files("**/*.S")

    def variant_cpp_files(self):
        return self.variant_files("**/*.cpp")

    def project_files(self, pattern):
        sources = []
        for file_path in file_operations.find_files(
            self.project_path, [pattern]
        ):
            if not file_path.startswith(
                "examples"
            ) and not file_path.startswith("libraries"):
                sources.append(file_path)
        return sources

    def project_c_files(self):
        return self.project_files("**/*.c")

    def project_cpp_files(self):
        return self.project_files("**/*.cpp")

    def project_ino_files(self):
        return self.project_files("**/*.ino")
