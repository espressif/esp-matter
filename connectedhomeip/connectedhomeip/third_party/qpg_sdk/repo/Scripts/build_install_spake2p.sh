#!/bin/bash

# Prerequisites for matter bootstrap.sh,
# install packages from connectedhomeip/integrations/docker/images/chip-build/Dockerfile
DEBIAN_FRONTEND=noninteractive sudo apt install -y \
    autoconf \
    automake \
    bison \
    bridge-utils \
    ccache \
    clang \
    clang-format \
    clang-tidy \
    curl \
    flex \
    g++ \
    git \
    gperf \
    iproute2 \
    jq \
    lcov \
    libavahi-client-dev \
    libavahi-common-dev \
    libcairo2-dev \
    libdbus-1-dev \
    libdbus-glib-1-dev \
    libdmalloc-dev \
    libgif-dev \
    libglib2.0-dev \
    libical-dev \
    libjpeg-dev \
    libmbedtls-dev \
    libncurses5-dev \
    libncursesw5-dev \
    libnl-3-dev \
    libnl-route-3-dev \
    libnspr4-dev \
    libpango1.0-dev \
    libpixman-1-dev \
    libreadline-dev \
    libssl-dev \
    libtool \
    libudev-dev \
    libusb-1.0-0 \
    libusb-dev \
    libxml2-dev \
    make \
    net-tools \
    ninja-build \
    openjdk-8-jdk \
    pkg-config \
    python3.9 \
    python3.9-dev \
    python3.9-venv \
    rsync \
    shellcheck \
    strace \
    systemd \
    udev \
    unzip \
    wget \
    git-lfs \
    zlib1g-dev

# limit chip activate.sh modifications to subshell
(
    # shellcheck source=/dev/null
    . scripts/bootstrap.sh || (echo spake2p build: chip bootstrap failed; exit 1)
    # shellcheck source=/dev/null
    . scripts/activate.sh|| (echo spake2p build: chip activate failed; exit 1)
    cd src/tools/spake2p || (echo spake2p build: chdir to spake2p directory failed; exit 1)
    log "building spake2p"
    gn gen out || (echo spake2p build: gn gen failed; exit 1)
    ninja -C out || (echo spake2p build: ninja build failed; exit 1)
    sudo cp out/spake2p /usr/bin || (echo installing spake2p failed; exit 1)
)
