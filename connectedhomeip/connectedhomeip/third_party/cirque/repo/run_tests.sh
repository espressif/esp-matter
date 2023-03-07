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

#   Description: unit and integration tests for cirque
#   Usage: sudo ./run_tests.sh


SRC_DIR="$(cd "$(dirname "$0")" && pwd -P)"
OPENTHREAD_DIR="${SRC_DIR}"/openthread
VENV="${SRC_DIR}"/venv-test

function flask_clean() {
  flask_pid=`ps aux | grep "[f]lask" | awk '{print $2}' | sort -k2 -rn`
  if [ ! -z "${flask_pid}" ]; then
    for pid in "${flask_pid}"; do
      kill -9 $pid >/dev/null 2>&1
    done
  fi
}

function grpc_clean() {
  grpc_pid=`ps aux | grep "[c]irque.grpc.service" | awk '{print $2}'`
  if [ ! -z "${grpc_pid}" ]; then
    for pid in "${grpc_pid}"; do
      kill -9 $pid >/dev/null 2>&1
    done
  fi
}

function socat_clean() {
  socat_pid=`ps aux | grep "[s]ocat" | awk '{print $2}'`
  if [ ! -z "${socat_pid}" ]; then
    for pid in "${socat_pid}"; do
      kill -9 $pid >/dev/null 2>&1
    done
  fi
}

function virtual_thread_clean() {
  vthread_pid=`ps aux | grep "[o]t-ncp-ftd" | awk '{print $2}'`
  if [ ! -z "${vthread_pid}" ]; then
    for pid in "${vthread_pid}"; do
      kill -9 $pid >/dev/null 2>&1
    done
  fi
}

function create_virtual_environment() {
  echo "creating python3 virtual environment for tests..."
  python3 -m venv "${VENV}"
  source "${VENV}"/bin/activate
  pip install --upgrade pip
  pip install --upgrade setuptools
  pip install wheel
  deactivate
}

function install_cirque_to_venv() {
  source "${VENV}"/bin/activate
  echo "installing cirque into virtual environment..."
  pip install .
  deactivate
}

function run_flask_service() {
  pip install -r requirements.txt
  FLASK_APP='cirque/restservice/service.py' \
    PATH="${PATH}":"${OPENTHREAD_DIR}"/output/x86_64-unknown-linux-gnu/bin/ \
    python3 -m flask run
}

function run_flask_virtual_home_test() {
  set -e
  echo "running flask virtual home test.."
  source "${VENV}"/bin/activate
  run_flask_service >/dev/null 2>&1 &
  sleep 10
  python3 examples/test_flask_virtual_home.py
  deactivate
}

function run_flask_clean() {
  echo "done flask test, clean up.."
  flask_clean
  socat_clean
  virtual_thread_clean
}

function run_grpc_service() {
  current_dirname=${SRC_DIR##*/}
  pushd .
  cd ..
  PATH="${PATH}":"${OPENTHREAD_DIR}"/output/x86_64-unknown-linux-gnu/bin/ \
    "${VENV}"/bin/python3 -m cirque.grpc.service
}

function run_grpc_virtual_home_test() {
  set -e
  echo "running gprc virtual home test.."
  source "${VENV}"/bin/activate
  run_grpc_service >/dev/null 2>&1 &
  sleep 10
  python3 examples/test_grpc_virtual_home.py
  deactivate
}

function run_grpc_clean() {
  echo "done grpc test, clean up.."
  grpc_clean
  socat_clean
  virtual_thread_clean
}

function run_unit_tests() {
  set -e
  echo "running unit tests.."
  source "${VENV}"/bin/activate
  export PATH="${PATH}":"${OPENTHREAD_DIR}"/output/x86_64-unknown-linux-gnu/bin/
  python3 cirque/capabilities/test/test_mount_capability.py
  python3 cirque/capabilities/test/test_trafficcontrol_capability.py
  python3 cirque/capabilities/test/test_xvnc_capability.py
  # python3 cirque/capabilities/test/test_wifi_capability.py
  # python3 cirque/home/test/test_home.py
  deactivate
}

function main() {
  if [ ! -d "${VENV}" ]; then
    ./dependency_modules.sh
  fi
  create_virtual_environment
  install_cirque_to_venv
  run_unit_tests
  testexit=$?
  #sleep 5
  # run_flask_virtual_home_test
  # testexit=$?
  # run_flask_clean
  # if [[ $testexit -gt 0 ]]; then
  #  exit 1
  #fi
  #sleep 5
  #run_grpc_virtual_home_test
  #run_grpc_clean
  if [[ $testexit -gt 0 ]]; then
    exit 1
  fi
}

main
