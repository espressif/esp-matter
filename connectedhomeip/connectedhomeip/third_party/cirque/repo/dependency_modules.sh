#!/bin/bash

# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#   Description: Unit tests for cirque core
#


set -x
set -e

SRC_DIR="$(cd "$(dirname "$0")" && pwd -P)"
OPEN_THREAD_DIR="${SRC_DIR}"/openthread
IMAGES=`docker images --filter=reference='*:latest' | awk '{print $1}'`

function install_openthread() {
  if [ ! -d "${OPEN_THREAD_DIR}" ]; then
      wget https://github.com/openthread/openthread/archive/0d14e854398cd9eced31666e48e6ab656e574074.zip
      unzip 0d14e854398cd9eced31666e48e6ab656e574074.zip
      rm 0d14e854398cd9eced31666e48e6ab656e574074.zip
      mv openthread-0d14e854398cd9eced31666e48e6ab656e574074 openthread
  fi
  pushd .
  cd openthread
  ./bootstrap
  make -f examples/Makefile-simulation
  popd
}

function build_wifiap_docker_image() {
  pushd .
  cd "cirque/resources"
  if [[ "${IMAGES}" != *mac80211_ap_image* ]]; then
    docker build -t mac80211_ap_image -f Dockerfile.wifiap .
  fi
  popd
}

function build_generic_node_docker_image() {
  pushd .
  cd "cirque/resources"
  if [[ "${IMAGES}" != *generic_node_image* ]]; then
    docker build -t generic_node_image  -f Dockerfile.generic_node .
  fi
  popd
}

function main() {
  install_openthread
  build_wifiap_docker_image
  build_generic_node_docker_image
}

main
