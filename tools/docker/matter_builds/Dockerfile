# Minimal Ubuntu-based image for ESP32 Matter builds
FROM ubuntu:24.04

# Build arguments
ARG IDF_CLONE_URL=https://github.com/espressif/esp-idf.git
ARG IDF_CHECKOUT_REF=v5.1

# Environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV IDF_PATH=/opt/espressif/esp-idf
ENV IDF_TOOLS_PATH=/opt/espressif/tools
ENV PATH="${IDF_PATH}/tools:${PATH}"
# Defaulting to 4 as this would be mostly run on resource constrained machines.
ENV NINJA_JOBS=4

# Install minimal dependencies for ESP-IDF and Matter
RUN set -x \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
    # Build essentials (gcc, g++, make, etc.)
    build-essential \
    cmake \
    ninja-build \
    ccache \
    # Autotools (required by Matter bootstrap)
    autoconf \
    automake \
    libtool \
    # Python (ESP-IDF + Matter requirements)
    python3 \
    python3-pip \
    python3-venv \
    python3-dev \
    python-is-python3 \
    # Version control and tools
    git \
    git-lfs \
    wget \
    curl \
    unzip \
    # ESP-IDF requirements
    flex \
    bison \
    gperf \
    libncurses5-dev \
    libffi-dev \
    libssl-dev \
    libusb-1.0-0 \
    dfu-util \
    # Matter/CHIP core requirements
    nodejs \
    pkg-config \
    libglib2.0-dev \
    libdbus-1-dev \
    libavahi-client-dev \
    libavahi-common-dev \
    libgirepository1.0-dev \
    libcairo2-dev \
    libreadline-dev \
    libmbedtls-dev \
    # Additional utilities
    tree \
    # Cleanup apt and unnecessary files
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && rm -rf /usr/share/doc/* /usr/share/man/* /usr/share/info/* \
    && rm -rf /usr/share/locale/* \
    && rm -rf /var/log/* /var/cache/* \
    && git lfs install \
    && : # last line

WORKDIR /opt/espressif
RUN set -x \
    && git clone --depth=1 --branch=${IDF_CHECKOUT_REF} --recurse-submodules --shallow-submodules ${IDF_CLONE_URL} ${IDF_PATH} \
    && cd ${IDF_PATH} \
    && ./install.sh --disable-gdbgui --disable-pytest --disable-ci --disable-docs \
    && git rev-parse --short HEAD > /opt/espressif/esp-idf-commit.txt \
    # remove the archives from the tools path
    && rm -rf ${IDF_TOOLS_PATH}/dist \
    && rm -rf /root/.cache \
    && : # last line

ARG ESP_MATTER_CLONE_URL=https://github.com/espressif/esp-matter.git
ARG ESP_MATTER_CHECKOUT_REF=main
ENV ESP_MATTER_PATH=/opt/espressif/esp-matter

RUN set -x \
    && mkdir -p $ESP_MATTER_PATH \
    && cd $ESP_MATTER_PATH \
    && git init \
    && git remote add origin $ESP_MATTER_CLONE_URL \
    && git fetch origin --depth=1 ${ESP_MATTER_CHECKOUT_REF} \
    && git checkout FETCH_HEAD \
    && git submodule update --init --depth=1 \
    && cd ./connectedhomeip/connectedhomeip \
    && ./scripts/checkout_submodules.py --platform esp32 linux --shallow \
    && git rev-parse --short HEAD > /opt/espressif/connectedhomeip-commit.txt \
    && cd ${ESP_MATTER_PATH} && git rev-parse --short HEAD > /opt/espressif/esp-matter-commit.txt \
    && export IDF_PATH_FORCE=1 && . ${IDF_PATH}/export.sh \
    && cd ${ESP_MATTER_PATH} && ./install.sh --ninja-jobs ${NINJA_JOBS} \
    # Move host tools to /usr/local/bin, strip debug symbols, and remove build directory
    && cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-cert /usr/local/bin/ \
    && cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-tool /usr/local/bin/ \
    && strip /usr/local/bin/chip-cert /usr/local/bin/chip-tool \
    && rm -rf ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out \
    && rm -rf /root/.cache /tmp/* \
    && : # last line

COPY entrypoint.sh /opt/esp/entrypoint.sh
ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]
CMD [ "/bin/bash" ]

WORKDIR /opt/espressif/esp-matter
