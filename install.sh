#!/usr/bin/env bash

set -e

# function to print help message
print_help() {
    echo "Options:"
    echo "  --no-host-tool          Disable installation of host tools"
    echo "  --help                  Display this help message"
}

# Parse command-line arguments
NO_HOST_TOOL=false
while [[ "$#" -gt 0 ]]; do
  case $1 in
         --no-host-tool)
                 NO_HOST_TOOL=true
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

echo ""
echo "Running Matter Setup"
echo ""
cd ${MATTER_PATH}
source ${MATTER_PATH}/scripts/bootstrap.sh
cd ${ESP_MATTER_PATH}

echo ""
echo "Building host tools"
echo ""

if [ $NO_HOST_TOOL = false ]; then
  gn --root="${MATTER_PATH}" gen ${MATTER_PATH}/out/host --args='chip_inet_config_enable_ipv4=false'
  ninja -C ${MATTER_PATH}/out/host chip-cert chip-tool
  echo ""
  echo "Host tools built at: ${MATTER_PATH}/out/host"
fi

echo ""
echo "Exit Matter environment"
echo ""
deactivate

echo ""
echo "Installing python dependencies for Matter"
echo ""
python3 -m pip install -r ${ESP_MATTER_PATH}/requirements.txt

echo "All done! You can now run:"
echo ""
echo "  . ${basedir}/export.sh"
echo ""
