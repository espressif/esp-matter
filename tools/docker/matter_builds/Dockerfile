# 67 is the latest tag which updates the docker image for ESP32 platform
# hence pinning version to 67 for building with v1.4-branch.
ARG VERSION=67
FROM ghcr.io/project-chip/chip-build:${VERSION} as build

# Use IDF_CHECKOUT_REF to specify a tag, a branch or a specific commit ID.

ARG IDF_CLONE_URL=https://github.com/espressif/esp-idf.git
ARG IDF_CHECKOUT_REF=v5.1

RUN set -x \
    && mkdir -p /tmp/esp-idf \
    && cd /tmp/esp-idf \
    && git init \
    && git remote add origin $IDF_CLONE_URL \
    && git fetch origin --depth=1 --recurse-submodules ${IDF_CHECKOUT_REF} \
    && git checkout FETCH_HEAD \
    && git submodule update --init --recursive  --depth 1 \
    && : # last line

FROM ghcr.io/project-chip/chip-build:${VERSION}

ENV IDF_PATH=/opt/espressif/esp-idf/
ENV IDF_TOOLS_PATH=/opt/espressif/tools

COPY --from=build /tmp/esp-idf /opt/espressif/esp-idf

# Setup the ESP-IDF
WORKDIR /opt/espressif/esp-idf
RUN set -x \
    && ./install.sh \
    && : # last line

ARG ESP_MATTER_CLONE_URL=https://github.com/espressif/esp-matter.git
ARG ESP_MATTER_CHECKOUT_REF=main

WORKDIR /opt/espressif
ENV ESP_MATTER_PATH=/opt/espressif/esp-matter

RUN set -x \
    && mkdir -p $ESP_MATTER_PATH \
    && cd $ESP_MATTER_PATH \
    && git init \
    && git remote add origin $ESP_MATTER_CLONE_URL \
    && git fetch origin --depth=1 ${ESP_MATTER_CHECKOUT_REF} \
    && git checkout FETCH_HEAD \
    && git submodule update --init --depth 1 \
    && cd ./connectedhomeip/connectedhomeip \
    && ./scripts/checkout_submodules.py --platform esp32 linux --shallow \
    && cd ../.. \
    && . $IDF_PATH/export.sh \
    && ./install.sh \
    && : # last line

RUN set -x \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -fy --no-install-recommends \
    tree \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/ \
    && : # last line

COPY entrypoint.sh /opt/esp/entrypoint.sh
ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]
CMD [ "/bin/bash" ]

WORKDIR /opt/espressif/esp-matter
