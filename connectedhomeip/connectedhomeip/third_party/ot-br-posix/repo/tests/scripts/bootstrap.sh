#!/bin/bash
#
#  Copyright (c) 2017, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

set -euxo pipefail

TOOLS_HOME="$HOME"/.cache/tools
[[ -d $TOOLS_HOME ]] || mkdir -p "$TOOLS_HOME"

disable_install_recommends()
{
    OTBR_APT_CONF_FILE=/etc/apt/apt.conf

    if [[ -f ${OTBR_APT_CONF_FILE} ]] && grep Install-Recommends "${OTBR_APT_CONF_FILE}"; then
        return 0
    fi

    sudo tee -a /etc/apt/apt.conf <<EOF
APT::Get::Install-Recommends "false";
APT::Get::Install-Suggests "false";
EOF
}

install_common_dependencies()
{
    # Common dependencies
    sudo apt-get install --no-install-recommends -y \
        libdbus-1-dev \
        ninja-build \
        doxygen \
        expect \
        net-tools \
        libboost-dev \
        libboost-filesystem-dev \
        libboost-system-dev \
        libavahi-common-dev \
        libavahi-client-dev \
        libreadline-dev \
        libncurses-dev \
        libjsoncpp-dev \
        coreutils
}

install_openthread_binraries()
{
    pip3 install -U cmake
    cd third_party/openthread/repo
    mkdir -p build && cd build

    cmake .. -GNinja -DOT_PLATFORM=simulation -DOT_FULL_LOGS=1 -DOT_COMMISSIONER=ON -DOT_JOINER=ON
    ninja
    sudo ninja install

    sudo apt-get install --no-install-recommends -y socat
}

configure_network()
{
    echo 0 | sudo tee /proc/sys/net/ipv6/conf/all/disable_ipv6
    echo 1 | sudo tee /proc/sys/net/ipv6/conf/all/forwarding
    echo 1 | sudo tee /proc/sys/net/ipv4/conf/all/forwarding
}

case "$(uname)" in
    "Linux")
        disable_install_recommends
        sudo apt-get update
        install_common_dependencies

        if [ "$BUILD_TARGET" == script-check ] || [ "$BUILD_TARGET" == docker-check ]; then
            install_openthread_binraries
            configure_network
            exit 0
        fi

        if [ "$BUILD_TARGET" == otbr-dbus-check ]; then
            install_openthread_binraries
            configure_network
            install_common_dependencies
            exit 0
        fi

        if [ "$BUILD_TARGET" == check ] || [ "$BUILD_TARGET" == meshcop ]; then
            install_openthread_binraries
            sudo apt-get install --no-install-recommends -y avahi-daemon avahi-utils cpputest
            configure_network
        fi

        if [ "$BUILD_TARGET" == android-check ]; then
            sudo apt-get install --no-install-recommends -y wget unzip libexpat1-dev gcc-multilib g++-multilib
            (
                cd "$HOME"
                wget -nv https://dl.google.com/android/repository/android-ndk-r17c-linux-x86_64.zip
                unzip android-ndk-r17c-linux-x86_64.zip >/dev/null
                mv android-ndk-r17c ndk-bundle
            )
            exit 0
        fi

        if [ "$BUILD_TARGET" == scan-build ]; then
            pip3 install -U cmake
            sudo apt-get install --no-install-recommends -y clang clang-tools
        fi

        if [ "$BUILD_TARGET" == pretty-check ]; then
            sudo apt-get install -y clang-format-9 shellcheck
            sudo snap install shfmt
        fi

        if [ "${OTBR_MDNS-}" == 'mDNSResponder' ]; then
            SOURCE_NAME=mDNSResponder-1310.80.1
            wget https://opensource.apple.com/tarballs/mDNSResponder/$SOURCE_NAME.tar.gz \
                && tar xvf $SOURCE_NAME.tar.gz \
                && cd $SOURCE_NAME/Clients \
                && sed -i '/#include <ctype.h>/a #include <stdarg.h>' dns-sd.c \
                && sed -i '/#include <ctype.h>/a #include <sys/param.h>' dns-sd.c \
                && cd ../mDNSPosix \
                && make os=linux && sudo make install os=linux
        fi

        # Enable IPv6
        if [ "$BUILD_TARGET" == check ]; then
            echo 0 | sudo tee /proc/sys/net/ipv6/conf/all/disable_ipv6
        fi

        # Allow access syslog file for unit test
        if [ "$BUILD_TARGET" == check ]; then
            sudo chmod a+r /var/log/syslog
        fi

        # Prepare Raspbian image
        if [ "$BUILD_TARGET" == raspbian-gcc ]; then
            sudo apt-get install --no-install-recommends --allow-unauthenticated -y qemu qemu-user-static binfmt-support parted

            (mkdir -p docker-rpi-emu \
                && cd docker-rpi-emu \
                && (git --git-dir=.git rev-parse --is-inside-work-tree || git --git-dir=.git init .) \
                && git fetch --depth 1 https://github.com/ryankurte/docker-rpi-emu.git master \
                && git checkout FETCH_HEAD)

            pip3 install git-archive-all

            IMAGE_NAME=$(basename "${IMAGE_URL}" .zip)
            IMAGE_FILE="$IMAGE_NAME".img
            [ -f "$TOOLS_HOME"/images/"$IMAGE_FILE" ] || {
                # unit MB
                EXPAND_SIZE=1024

                [ -d "$TOOLS_HOME"/images ] || mkdir -p "$TOOLS_HOME"/images

                [[ -f "$IMAGE_NAME".zip ]] || curl -LO "$IMAGE_URL"

                unzip "$IMAGE_NAME".zip -d /tmp

                (cd /tmp \
                    && dd if=/dev/zero bs=1048576 count="$EXPAND_SIZE" >>"$IMAGE_FILE" \
                    && mv "$IMAGE_FILE" "$TOOLS_HOME"/images/"$IMAGE_FILE")

                (cd docker-rpi-emu/scripts \
                    && sudo ./expand.sh "$TOOLS_HOME"/images/"$IMAGE_FILE" "$EXPAND_SIZE")
            }
        fi
        ;;

    "Darwin")
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
        ;;

    *)
        echo "Unknown os type"
        exit 1
        ;;
esac
