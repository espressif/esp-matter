#!/bin/bash

CONTAINER_NAME=${CONTAINER_NAME:-"siliconlabsinc/multiprotocol"}

CPCD_SECURITY_ENABLED=true
CPCD_DEFAULT_BINDING_KEY_FILE=/etc/binding-key.key
CPCD_BINDING_KEY_FILE=$CPCD_DEFAULT_BINDING_KEY_FILE

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            echo "Usage: $0 [options]"
            echo -e "\t-s\tStop running the multiprotocol container"
            echo -e "\t-c\tMount the supplied cpcd.conf file and start the multiprotocol container"
            echo -e "\t-k\tMount the supplied cpcd binding key file and start the multiprotocol container"
            echo -e "\t-K\tGenerate a cpcd binding key file and place at $CPCD_DEFAULT_BINDING_KEY_FILE"
            echo -d "\t-X\tDisable cpcd security"
            echo -e "\t-o\tOpen a bash shell in the running multiprotocol container"
            echo -e "\t-O\tStart ot-cli application in the running multiprotocol container"
            echo -e "\t-T\tStart OTBR and ot-ctl application in the running multiprotocol container"
            echo -e "\t-Z\tStart Zigbeed and Z3GatewayHost application in the running multiprotocol container"
            echo -e "\t-C\tStart Z3GatewayCpc Host application in the running multiprotocol container"
            echo -e "\t-L\tStart Bluetooth and bluetoothctl application in the running multiprotocol container"
            echo "By default, this script will start the multiprotocol container"
            exit
            ;;
        -s|--stop)
            docker stop -t 0 multiprotocol
            exit
            ;;
        -c|--cpcd-conf)
            shift
            CPCD_CONFIG_FILE=$(realpath $1)
            shift
            ;;
        -k|--cpcd-binding-key-file)
            shift
            CPCD_BINDING_KEY_FILE=$(realpath $1)
            shift
            ;;
        -K|--generate-cpcd-binding-key)
            GENERATE_CPCD_BINDING_KEY=true
            shift
            ;;
        -X|--disable-cpcd-security)
            CPCD_SECURITY_ENABLED=false
            echo "Running container without mounting a cpcd binding key file."
            echo "Make sure the RCP image is compiled with SL_CPC_SECURITY_ENABLED defined to 0"
            echo "and that disable_encryption is set to true in cpcd.conf."
            shift
            ;;
        -l|--log)
            docker exec -it multiprotocol journalctl -fexu cpcd
            exit
            ;;
        -o|--open)
            docker exec -it multiprotocol /bin/bash
            exit
            ;;
        -O|--ot-cli)
            docker exec -it multiprotocol /usr/local/bin/ot-cli 'spinel+cpc://cpcd_0?iid=2'
            exit
            ;;
        -T|--ot-ctl)
            echo "Cleaning up stale OTBR firewall rules. Ignore errors..."
            docker exec -it multiprotocol ip6tables -D FORWARD 1
            docker exec -it multiprotocol ip6tables -F
            docker exec -it multiprotocol ip6tables -X OTBR_FORWARD_INGRESS
            echo "Starting OTBR..."
            docker exec -it multiprotocol systemctl start otbr
            sleep 5
            echo "Starting ot-ctl..."
            echo "(If errors persist, run 'journalctl -fex' inside container for logs.)"
            echo "Press ENTER for prompt..."
            echo
            while 
                docker exec -it multiprotocol ot-ctl
                [[ $? -eq 1 ]]
            do
                sleep 1
            done
            exit
            ;;
        -Z|--zigbee-host)
            echo "Starting zigbeed..."
            docker exec -it multiprotocol systemctl start zigbeed
            sleep 5
            echo "Checking zigbeed status..."
            while
                docker exec -it multiprotocol systemctl status zigbeed | grep 'RCP version'
                [[ $? -ne 0 ]]
            do
                sleep 3
                echo "Failed to start zigbeed, restarting..."
                echo "(If errors persist, run 'journalctl -fex' inside container for logs.)"
                docker exec -it multiprotocol systemctl restart zigbeed
            done
            echo "Starting Z3Gateway..."
            docker exec -it multiprotocol /usr/local/bin/Z3Gateway -p ttyZigbeeNCP
            exit
            ;;
        -C|--zigbee-cpc-host)
            docker exec -it multiprotocol /usr/local/bin/Z3GatewayCpc
            exit
            ;;    
        -L|--bluetoothctl)
            # stop bluetoothd on the host
            sudo service bluetooth stop
            # disable bluetoothd on the host
            sudo systemctl mask bluetooth.service
            docker exec -it multiprotocol systemctl start hciattach
            sleep 5
            docker exec -it multiprotocol bluetoothctl
            exit
            ;;
        *)
            echo "Unrecognized option '$1'"
            exit
            ;;
    esac
done

docker pull ${CONTAINER_NAME}
docker stop -t 0 multiprotocol # stop container if it is running

RUN_ARGS="--rm --name multiprotocol" # Clean up after run
if [ -e "$CPCD_CONFIG_FILE" ]; then
    echo "Using host's cpcd config file: $CPCD_CONFIG_FILE"
    RUN_ARGS+=" -v $CPCD_CONFIG_FILE:/usr/local/etc/cpcd.conf:ro"
fi
RUN_ARGS+=" -v /tmp/multiprotocol-container/log:/var/log/" # Add in logging folder
RUN_ARGS+=" -v /accept_silabs_msla" # Accept the MSLA for Zigbeed
RUN_ARGS+=" --privileged --cap-add SYS_ADMIN" # Add in security permissions
RUN_ARGS+=" --cap-add=SYS_ADMIN --cap-add=NET_ADMIN --net=host" # Add bluetooth
RUN_ARGS+=" -d" # Start as a daemon

if [ "$GENERATE_CPCD_BINDING_KEY" = true ]; then
    echo "Attempting to generate cpcd binding key file and place at $CPCD_DEFAULT_BINDING_KEY_FILE"
    echo "Starting container"
    docker run ${RUN_ARGS} ${CONTAINER_NAME}
    sleep 3
    echo "Stopping cpcd service"
    docker exec multiprotocol systemctl stop cpcd
    # Temporarily using /var/log is just an easy way to move the file outside the container
    docker exec multiprotocol rm /var/log/binding-key.key
    echo "Generating cpcd keys"
    docker exec multiprotocol cpcd --bind ecdh --key /var/log/binding-key.key
    if [ -s /tmp/multiprotocol-container/log/binding-key.key ]; then
        sudo mv /tmp/multiprotocol-container/log/binding-key.key ${CPCD_DEFAULT_BINDING_KEY_FILE}
        echo "Generated cpcd key at $CPCD_DEFAULT_BINDING_KEY_FILE"
        echo "Now running this script (without the -K argument) will"
        echo "automatically mount the binding key file into the container."
    else
        echo "Failed to generate the binding key file."
        echo "This can happen if the RCP is still bound to the host."
        echo "Consult the CPCd readme.md for more instructions on binding."
    fi
    docker stop -t 0 multiprotocol
    echo "Container stopped"
    exit
fi

if [ "$CPCD_SECURITY_ENABLED" = true ]; then
    if [ -e "$CPCD_BINDING_KEY_FILE" ]; then
        RUN_ARGS+=" -v $CPCD_BINDING_KEY_FILE:$CPCD_DEFAULT_BINDING_KEY_FILE"
    else
        echo ""
        echo "No CPCd binding key file found at $CPCD_BINDING_KEY_FILE."
        echo ""
        echo "Use the -K argument to generate a binding key file and move it to"
        echo "$CPCD_DEFAULT_BINDING_KEY_FILE outside the container. After that,"
        echo "running this script will automatically mount the binding key file"
        echo "into the container."
        echo ""
        echo "If security is disabled on the RCP and the host, use the -X"
        echo "argument to run without mounting a binding key file."
        echo "" 
        echo "Consult the CPCd readme.md for more instructions on binding."
        exit
    fi
fi

echo "Running 'docker run ${RUN_ARGS} ${CONTAINER_NAME}'"
docker run ${RUN_ARGS} ${CONTAINER_NAME}
