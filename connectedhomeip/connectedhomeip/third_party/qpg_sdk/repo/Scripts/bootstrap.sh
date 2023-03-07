#!/bin/bash

set -ex

SCRIPT_PATH="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
VENV_PATH=$(realpath "${SCRIPT_PATH}/../.python_venv")
QMATTER_ROOT_PATH=$(realpath "${SCRIPT_PATH}/..")

trap 'on_error $? $LINENO' ERR

on_error() {
  echo "!!! $SCRIPT_PATH failed with error $1 on line $2"
  exit "$1"
}

DEFAULT_TOOLCHAIN_DIR=/opt/TOOL_ARMGCCEMB/gcc-arm-none-eabi-9-2019-q4-major

export PATH=$PATH:$DEFAULT_TOOLCHAIN_DIR/bin:${SCRIPT_PATH}/../Tools/FactoryData
export MAKEFLAGS=-s

/proc/self/exe --version 2>/dev/null | grep -q 'GNU bash' ||  (\
    echo "!!!!! This is a BASH script !!!!!"; \
    echo "The shell you are running is $(/proc/self/exe --version)"; \
    echo "Please start bash first by running the 'bash' command"; \
    echo "Press Ctrl-C to abort"; \
    read -r \
)

log() {
    echo "$(realpath "${BASH_SOURCE[0]}") ========= ${1} ============"
}
bootstrap_sh_failure() {
    echo "========= ${1} ============"
}
check_installed_dependency ()
{
    $1 --version foo >/dev/null 2>&1 || {
        echo >&2 "$1 not installed. installing........"
        return 1
    }
    return 0
}

install_node_npm ()
{
    sudo apt-get update

    ### Node.js v16 ###
    curl -fsSL https://deb.nodesource.com/setup_16.x | sudo bash -

    sudo apt install -y nodejs
    # the 'installed-check' package checks package.json is fulfilled
    if ! npm list installed-check &>/dev/null; then
        npm install installed-check
    fi

    if ! ./node_modules/.bin/installed-check -c &>/dev/null; then
        npm install
    fi
}

install_zap_dependencies ()
{
    sudo apt-get update
    sudo apt-get install -y clang-format npm

    sudo apt-get install -y --fix-missing libpixman-1-dev libcairo-dev libsdl-pango-dev libjpeg-dev libgif-dev

    curl -fsSL https://deb.nodesource.com/setup_16.x | sudo bash -

    sudo apt install -y nodejs

    if ! npm list installed-check &>/dev/null; then
        npm install installed-check
    fi

    if ! ./node_modules/.bin/installed-check -c &>/dev/null; then
        npm install
    fi
}

install_arm_gcc_emb ()
{
    wget -P /tmp --progress=dot:giga https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
    sudo mkdir -p /opt/TOOL_ARMGCCEMB
    sudo tar -xf /tmp/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2 -C /opt/TOOL_ARMGCCEMB
    rm /tmp/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
}

install_gn ()
{
    git clone https://gn.googlesource.com/gn /tmp/gn
    python3 /tmp/gn/build/gen.py --out-path=/tmp/gn/out
    ninja -C /tmp/gn/out
    sudo cp /tmp/gn/out/gn /usr/local/bin/gn
    sudo chmod +x /usr/local/bin/gn
}

setup_venv ()
{
    sudo apt install -y python3.9

    python3.9 -m venv --help >/dev/null 2>&1 || sudo apt-get install -y python3.9-venv
    python3.9 -m ensurepip --help >/dev/null 2>&1 || sudo apt-get install -y python3.9-venv

    if [[ ! -d ${VENV_PATH} ]]; then
        mkdir -p "${VENV_PATH}"
    fi
    python3.9 -m venv "${VENV_PATH}"
    export VENV_PATH
    # shellcheck source=/dev/null
    source "${VENV_PATH}"/bin/activate
    log "$(python -V)"
    # Install additional modules
    pip3 install dataclasses intelhex click ecdsa cryptography
}

setup_submodules ()
{
    # QMatter '-libs' variant lacks the matter submodule to avoid
    # a recursive dependency in project-chip/connectedhomeip
    # For Qorvo-internal CI testing, we add this at validation time
    if test -e "${SCRIPT_PATH}/git_add_submodules.sh"  && test ! -e "${QMATTER_ROOT_PATH}/Components/Thirdparty/Matter/repo/.gitmodules"
    then
        log "Adding submodules to allow package validation"
        # shellcheck source=/dev/null
        source "${SCRIPT_PATH}/git_add_submodules.sh"
    fi

    log "Updating submodules"
    git submodule update --init --depth=1 Components/ThirdParty/Matter/repo

    cd Components/ThirdParty/Matter/repo || (bootstrap_sh_failure "chdir to matter repo failed"; exit 1)
    # TODO: use Components/Thirdparty/Matter/repo/scripts/checkout_submodules.py --platform qpg
    for module_path in  \
        third_party/mbedtls \
        third_party/nlassert \
        third_party/nlio \
        third_party/nlunit-test \
        third_party/freertos \
        third_party/lwip \
        third_party/openthread \
        third_party/pigweed \
        third_party/qpg_sdk
    do
        git submodule update --init --depth=1 -- "${module_path}"
    done
    log "submodules successfully initialized"
}

install_spake2p ()
{
    log "Installing spake2p build requirements"
    cd "${QMATTER_ROOT_PATH}/Components/ThirdParty/Matter/repo" || (echo chdir to matter repo failed; exit 1)

    # shellcheck source=/dev/null
    source "${SCRIPT_PATH}/build_install_spake2p.sh"
}

command -v sudo || (
    echo "Please enter your root password to install sudo."
    su -c 'apt-get update; apt-get install -y sudo'
)
command sudo apt-get update

for tool_name in  \
    git \
    clang \
    make \
    ninja \
    curl \
    wget
do
    command -v "$tool_name" || sudo apt-get install -y "${tool_name}" || sudo apt-get install -y "${tool_name}-build"
done

if ! check_installed_dependency node; then
    install_node_npm
fi

if check_installed_dependency arm-none-eabi-gcc
then
    if ! arm-none-eabi-gcc --version | grep -F "9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]" >/dev/null
    then
        echo "Invalid armgcc version detected"
        exit 1
    fi
else
    install_arm_gcc_emb
fi

if ! check_installed_dependency gn; then
    install_gn
fi

if ! check_installed_dependency npm; then
    install_zap_dependencies
fi

setup_venv

setup_submodules

if test ! -e /usr/bin/spake2p
then
    install_spake2p
fi
