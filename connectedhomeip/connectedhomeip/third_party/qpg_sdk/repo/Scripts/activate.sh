#!/bin/bash

SCRIPT_PATH="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
VENV_PATH=$(realpath "${SCRIPT_PATH}/../.python_venv")

/proc/self/exe --version 2>/dev/null | grep -q 'GNU bash' ||  (\
    echo "!!!!! This is a BASH script !!!!!"; \
    echo "The shell you are running is $(/proc/self/exe --version)"; \
    echo "Please start bash first by running the 'bash' command"; \
    echo "Press Ctrl-C to abort"; \
    read -r \
)

log() {
    echo "========= ${1} ============"
}
activate_sh_failure() {
    echo "========= ${1} ============"
    export ACTIVATE_SH_FAILURE=true
}


DEFAULT_TOOLCHAIN_DIR=/opt/TOOL_ARMGCCEMB/gcc-arm-none-eabi-9-2019-q4-major

export PATH=$PATH:$DEFAULT_TOOLCHAIN_DIR/bin:${SCRIPT_PATH}/../Tools/FactoryData
export MAKEFLAGS=-s

if bash "${SCRIPT_PATH}"/bootstrap.sh
then
    export TOOLCHAIN="$DEFAULT_TOOLCHAIN_DIR"
    # shellcheck source=/dev/null
    source "${VENV_PATH}"/bin/activate || activate_sh_failure "$(realpath "${BASH_SOURCE[0]}") FAILED"
else
    activate_sh_failure "$(realpath "${BASH_SOURCE[0]}") FAILED"
fi
