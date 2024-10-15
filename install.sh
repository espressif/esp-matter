#!/usr/bin/env bash

set -e

# function to print help message
print_help() {
    echo "Options:"
    echo "  --no-host-tool          Disable installation of host tools"
    echo "  --no-bootstrap          Disable sourcing connectedhomeip's scripts/bootstrap.sh,"
    echo "                          This can be helpful if there's already present connectedhomeip setup"
    echo "  --help                  Display this help message"
}

echo_log() {
  echo $@
  echo ""
}

# Parse command-line arguments
NO_HOST_TOOL=false
NO_BOOTSTRAP=false

while [[ "$#" -gt 0 ]]; do
  case $1 in
         --no-host-tool)
            NO_HOST_TOOL=true
            ;;
        --no-bootstrap)
            NO_BOOTSTRAP=true
            ;;
        --help)
            print_help
            exit 1
            ;;
         *)
            print_help
            exit 1
            ;;
  esac
  shift
done

basedir=$(dirname "$0")
ESP_MATTER_PATH=$(cd "${basedir}"; pwd)
MATTER_PATH=${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip
export ZAP_INSTALL_PATH=${MATTER_PATH}/.environment/cipd/packages/zap


if [ $NO_BOOTSTRAP = false ]; then
  echo_log "Running Matter Setup"
  cd ${MATTER_PATH}
  source ${MATTER_PATH}/scripts/bootstrap.sh
  cd ${ESP_MATTER_PATH}
else
  echo_log "Skipping Matter Setup"
fi

if [ $NO_HOST_TOOL = false ]; then
  echo_log "Building host tools"
  gn --root="${MATTER_PATH}" gen ${MATTER_PATH}/out/host --args='chip_inet_config_enable_ipv4=false'
  ninja -C ${MATTER_PATH}/out/host chip-cert chip-tool
  echo_log "Host tools built at: ${MATTER_PATH}/out/host"
else
  echo_log "Skip building host tools"
fi

if [ $NO_BOOTSTRAP = false ]; then
  echo_log "Exit Matter environment"
  deactivate
fi

echo_log "Installing python dependencies for Matter"

# Install python dependencies based on idf version
if [[ $(git -C $IDF_PATH describe) == v4.4* ]]; then
  echo_log "Installing requirements from requirements_idf_v4.4.txt"
  python3 -m pip install -r ${ESP_MATTER_PATH}/requirements_idf_v4.4.txt >/dev/null
else
  echo_log "Installing requirements from requirements.txt"
  python3 -m pip install -r ${ESP_MATTER_PATH}/requirements.txt > /dev/null
fi

echo_log "All done! You can now run:"
echo_log "  . ${basedir}/export.sh"
