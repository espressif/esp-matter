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

# Just in case PATH isn't already exported.
export PATH

# Note: Colors are unfortunately duplicated in several places; and removing the
# duplication is not easy. Their locations are:
#
#   - bootstrap.sh
#   - pw_cli/color.py
#   - pw_env_setup/py/pw_env_setup/colors.py
#
# So please keep them matching then modifying them.
pw_none() {
  echo -e "$*"
}

pw_red() {
  echo -e "\033[0;31m$*\033[0m"
}

pw_bold_red() {
  echo -e "\033[1;31m$*\033[0m"
}

pw_yellow() {
  echo -e "\033[0;33m$*\033[0m"
}

pw_bold_yellow() {
  echo -e "\033[1;33m$*\033[0m"
}

pw_green() {
  echo -e "\033[0;32m$*\033[0m"
}

pw_bold_green() {
  echo -e "\033[1;32m$*\033[0m"
}

pw_blue() {
  echo -e "\033[1;34m$*\033[0m"
}

pw_cyan() {
  echo -e "\033[1;36m$*\033[0m"
}

pw_magenta() {
  echo -e "\033[0;35m$*\033[0m"
}

pw_bold_white() {
  echo -e "\033[1;37m$*\033[0m"
}

pw_error() {
  echo -e "\033[1;31m$*\033[0m" >& /dev/stderr
}

pw_error_info() {
  echo -e "\033[0;31m$*\033[0m" >& /dev/stderr
}

pw_eval_sourced() {
  if [ "$1" -eq 0 ]; then
    # TODO(pwbug/354) Remove conditional after all downstream projects have
    # changed to passing in second argument.
    if [ -n "$2" ]; then
      _PW_NAME=$(basename "$2" .sh)
    else
      _PW_NAME=$(basename "$_BOOTSTRAP_PATH" .sh)
    fi
    pw_error "Error: Attempting to $_PW_NAME in a subshell"
    pw_error_info "  Since $_PW_NAME.sh modifies your shell's environment"
    pw_error_info "  variables, it must be sourced rather than executed. In"
    pw_error_info "  particular, 'bash $_PW_NAME.sh' will not work since the "
    pw_error_info "  modified environment will get destroyed at the end of the"
    pw_error_info "  script. Instead, source the script's contents in your"
    pw_error_info "  shell:"
    pw_error_info ""
    pw_error_info "    \$ source $_PW_NAME.sh"
    exit 1
  fi
}

pw_check_root() {
  _PW_ROOT="$1"
  if [[ "$_PW_ROOT" = *" "* ]]; then
    pw_error "Error: The Pigweed path contains spaces\n"
    pw_error_info "  The path '$_PW_ROOT' contains spaces. "
    pw_error_info "  Pigweed's Python environment currently requires Pigweed to"
    pw_error_info "  be at a path without spaces. Please checkout Pigweed in a"
    pw_error_info "  directory without spaces and retry running bootstrap."
    return
  fi
}

pw_get_env_root() {
  # PW_ENVIRONMENT_ROOT allows developers to specify where the environment
  # should be installed. bootstrap.sh scripts should not use that variable to
  # store the result of this function. This separation allows scripts to assume
  # PW_ENVIRONMENT_ROOT came from the developer and not from a previous
  # bootstrap possibly from another workspace.
  if [ -n "$PW_ENVIRONMENT_ROOT" ]; then
    echo "$PW_ENVIRONMENT_ROOT"
    return
  fi

  # Determine project-level root directory.
  if [ -n "$PW_PROJECT_ROOT" ]; then
    _PW_ENV_PREFIX="$PW_PROJECT_ROOT"
  else
    _PW_ENV_PREFIX="$PW_ROOT"
  fi

  # If <root>/environment exists, use it. Otherwise, if <root>/.environment
  # exists, use it. Finally, use <root>/environment.
  _PW_DOTENV="$_PW_ENV_PREFIX/.environment"
  _PW_ENV="$_PW_ENV_PREFIX/environment"

  if [ -d "$_PW_DOTENV" ]; then
    if [ -d "$_PW_ENV" ]; then
      pw_error "Error: both possible environment directories exist."
      pw_error_info "  $_PW_DOTENV"
      pw_error_info "  $_PW_ENV"
      pw_error_info "  If only one of these folders exists it will be used for"
      pw_error_info "  the Pigweed environment. If neither exists"
      pw_error_info "  '<...>/environment' will be used. Since both exist,"
      pw_error_info "  bootstrap doesn't know which to use. Please delete one"
      pw_error_info "  or both and rerun bootstrap."
      exit 1
    fi
  fi

  if [ -d "$_PW_ENV" ]; then
    echo "$_PW_ENV"
  elif [ -d "$_PW_DOTENV" ]; then
    echo "$_PW_DOTENV"
  else
    echo "$_PW_ENV"
  fi
}

# Note: This banner is duplicated in three places; which is a lesser evil than
# the contortions that would be needed to share this snippet across shell,
# batch, and Python. Locations:
#
#   - pw_env_setup/util.sh
#   - pw_cli/branding.py
#   - pw_env_setup/py/pw_env_setup/windows_env_start.py
#
_PW_BANNER=$(cat <<EOF
 ▒█████▄   █▓  ▄███▒  ▒█    ▒█ ░▓████▒ ░▓████▒ ▒▓████▄
  ▒█░  █░ ░█▒ ██▒ ▀█▒ ▒█░ █ ▒█  ▒█   ▀  ▒█   ▀  ▒█  ▀█▌
  ▒█▄▄▄█░ ░█▒ █▓░ ▄▄░ ▒█░ █ ▒█  ▒███    ▒███    ░█   █▌
  ▒█▀     ░█░ ▓█   █▓ ░█░ █ ▒█  ▒█   ▄  ▒█   ▄  ░█  ▄█▌
  ▒█      ░█░ ░▓███▀   ▒█▓▀▓█░ ░▓████▒ ░▓████▒ ▒▓████▀
EOF
)

_pw_banner() {
  if [ -z "$PW_ENVSETUP_QUIET" ] && [ -z "$PW_ENVSETUP_NO_BANNER" ]; then
    pw_magenta "$_PW_BANNER\n"
  fi
}

_PW_BANNER_FUNC="_pw_banner"

_pw_hello() {
  _PW_TEXT="$1"
  if [ -n "$PW_BANNER_FUNC" ]; then
    _PW_BANNER_FUNC="$PW_BANNER_FUNC"
  fi
  if [ -z "$PW_ENVSETUP_QUIET" ]; then
    pw_green "\n  WELCOME TO...\n"
    "$_PW_BANNER_FUNC"
    pw_green "$_PW_TEXT"
  fi
}

pw_deactivate() {
  # Assume PW_ROOT and PW_PROJECT_ROOT have already been set and we need to
  # preserve their values.
  _NEW_PW_ROOT="$PW_ROOT"
  _NEW_PW_PROJECT_ROOT="$PW_PROJECT_ROOT"

  # Find deactivate script, run it, and then delete it. This way if the
  # deactivate script is doing something wrong subsequent bootstraps still
  # have a chance to pass.
  _PW_DEACTIVATE_SH="$_PW_ACTUAL_ENVIRONMENT_ROOT/deactivate.sh"
  if [ -f "$_PW_DEACTIVATE_SH" ]; then
    . "$_PW_DEACTIVATE_SH"
    rm -f "$_PW_DEACTIVATE_SH" &> /dev/null
  fi

  # If there's a _pw_deactivate function run it. Redirect output to /dev/null
  # in case _pw_deactivate doesn't exist. Remove _pw_deactivate when complete.
  if [ -n "$(command -v _pw_deactivate)" ]; then
    _pw_deactivate > /dev/null 2> /dev/null
    unset -f _pw_deactivate
  fi

  # Restore.
  PW_ROOT="$_NEW_PW_ROOT"
  export PW_ROOT
  PW_PROJECT_ROOT="$_NEW_PW_PROJECT_ROOT"
  export PW_PROJECT_ROOT
}

deactivate() {
  pw_deactivate
  unset -f pw_deactivate
  unset -f deactivate
  unset PW_ROOT
  unset PW_PROJECT_ROOT
  unset PW_BRANDING_BANNER
  unset PW_BRANDING_BANNER_COLOR
}

# The next three functions use the following variables.
# * PW_BANNER_FUNC: function to print banner
# * PW_BOOTSTRAP_PYTHON: specific Python interpreter to use for bootstrap
# * PW_ROOT: path to Pigweed root
# * PW_ENVSETUP_QUIET: limit output if "true"
#
# All arguments passed in are passed on to env_setup.py in pw_bootstrap,
# pw_activate takes no arguments, and pw_finalize takes the name of the script
# "bootstrap" or "activate" and the path to the setup script written by
# bootstrap.sh.
pw_bootstrap() {
  _pw_hello "  BOOTSTRAP! Bootstrap may take a few minutes; please be patient.\n"

  local _pw_alias_check=0
  alias python > /dev/null 2> /dev/null || _pw_alias_check=$?
  if [ "$_pw_alias_check" -eq 0 ]; then
    pw_error "Error: 'python' is an alias"
    pw_error_info "The shell has a 'python' alias set. This causes many obscure"
    pw_error_info "Python-related issues both in and out of Pigweed. Please"
    pw_error_info "remove the Python alias from your shell init file or at"
    pw_error_info "least run the following command before bootstrapping"
    pw_error_info "Pigweed."
    pw_error_info
    pw_error_info "  unalias python"
    pw_error_info
    return
  fi

  # Allow forcing a specific version of Python for testing pursposes.
  if [ -n "$PW_BOOTSTRAP_PYTHON" ]; then
    _PW_PYTHON="$PW_BOOTSTRAP_PYTHON"
  elif command -v python3 > /dev/null 2> /dev/null; then
    _PW_PYTHON=python3
  elif command -v python2 > /dev/null 2> /dev/null; then
    _PW_PYTHON=python2
  elif command -v python > /dev/null 2> /dev/null; then
    _PW_PYTHON=python
  else
    pw_error "Error: No system Python present\n"
    pw_error_info "  Pigweed's bootstrap process requires a local system"
    pw_error_info "  Python. Please install Python on your system, add it to "
    pw_error_info "  your PATH and re-try running bootstrap."
    return
  fi

  if [ -n "$_PW_ENV_SETUP" ]; then
    "$_PW_ENV_SETUP" "$@"
    _PW_ENV_SETUP_STATUS="$?"
  else
    "$_PW_PYTHON" "$PW_ROOT/pw_env_setup/py/pw_env_setup/env_setup.py" "$@"
    _PW_ENV_SETUP_STATUS="$?"
  fi

  # Write the directory path at bootstrap time into the directory. This helps
  # us double-check things are still in the same space when calling activate.
  _PW_ENV_ROOT_TXT="$_PW_ACTUAL_ENVIRONMENT_ROOT/env_root.txt"
  echo "$_PW_ACTUAL_ENVIRONMENT_ROOT" > "$_PW_ENV_ROOT_TXT"

  # Create the environment README file. Use quotes to prevent alias expansion.
  "cp" "$PW_ROOT/pw_env_setup/destination.md" "$_PW_ACTUAL_ENVIRONMENT_ROOT/README.md"
}

pw_activate() {
  _pw_hello "  ACTIVATOR! This sets your shell environment variables.\n"
  _PW_ENV_SETUP_STATUS=0
}

pw_finalize() {
  _PW_NAME="$1"
  _PW_SETUP_SH="$2"

  # Check that the environment directory agrees that the path it's at matches
  # where it thinks it should be. If not, bail.
  _PW_ENV_ROOT_TXT="$_PW_ACTUAL_ENVIRONMENT_ROOT/env_root.txt"
  if [ -f "$_PW_ENV_ROOT_TXT" ]; then
    _PW_PREV_ENV_ROOT="$(cat $_PW_ENV_ROOT_TXT)"
    if [ "$_PW_ACTUAL_ENVIRONMENT_ROOT" != "$_PW_PREV_ENV_ROOT" ]; then
      pw_error "Error: Environment directory moved"
      pw_error_info "This Pigweed environment was created at"
      pw_error_info
      pw_error_info "    $_PW_PREV_ENV_ROOT"
      pw_error_info
      pw_error_info "But it is now being activated from"
      pw_error_info
      pw_error_info "    $_PW_ACTUAL_ENVIRONMENT_ROOT"
      pw_error_info
      pw_error_info "This is likely because the checkout moved. After moving "
      pw_error_info "the checkout a full '. ./bootstrap.sh' is required."
      pw_error_info
      _PW_ENV_SETUP_STATUS=1
    fi
  fi

  if [ "$_PW_ENV_SETUP_STATUS" -ne 0 ]; then
     return
  fi

  if [ -f "$_PW_SETUP_SH" ]; then
    . "$_PW_SETUP_SH"

    if [ "$?" -eq 0 ]; then
      if [ "$_PW_NAME" = "bootstrap" ] && [ -z "$PW_ENVSETUP_QUIET" ]; then
        echo "To reactivate this environment in the future, run this in your "
        echo "terminal:"
        echo
        pw_green "  source ./activate.sh"
        echo
        echo "To deactivate this environment, run this:"
        echo
        pw_green "  deactivate"
        echo
      fi
    else
      pw_error "Error during $_PW_NAME--see messages above."
    fi
  else
    pw_error "Error during $_PW_NAME--see messages above."
  fi
}

pw_install_post_checkout_hook() {
  cp "$PW_ROOT/pw_env_setup/post-checkout-hook.sh" "$PW_PROJECT_ROOT/.git/hooks/post-checkout"
}

pw_cleanup() {
  unset _PW_BANNER
  unset _PW_BANNER_FUNC
  unset PW_BANNER_FUNC
  unset _PW_ENV_SETUP
  unset _PW_NAME
  unset _PW_PYTHON
  unset _PW_ENV_ROOT_TXT
  unset _PW_PREV_ENV_ROOT
  unset _PW_SETUP_SH
  unset _PW_DEACTIVATE_SH
  unset _NEW_PW_ROOT
  unset _PW_ENV_SETUP_STATUS
  unset _PW_ENV_PREFIX
  unset _PW_ENV
  unset _PW_DOTENV

  unset -f pw_none
  unset -f pw_red
  unset -f pw_bold_red
  unset -f pw_yellow
  unset -f pw_bold_yellow
  unset -f pw_green
  unset -f pw_bold_green
  unset -f pw_blue
  unset -f pw_cyan
  unset -f pw_magenta
  unset -f pw_bold_white
  unset -f pw_eval_sourced
  unset -f pw_check_root
  unset -f pw_get_env_root
  unset -f _pw_banner
  unset -f pw_bootstrap
  unset -f pw_activate
  unset -f pw_finalize
  unset -f pw_install_post_checkout_hook
  unset -f pw_cleanup
  unset -f _pw_hello
  unset -f pw_error
  unset -f pw_error_info
}
