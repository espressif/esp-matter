#!/bin/bash
#
#  Copyright (c) 2018, The OpenThread Authors.
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

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

function parse_args()
{
    while [ $# -gt 0 ]; do
        case $1 in
            --radio-url)
                RADIO_URL="$2"
                shift
                shift
                ;;
            --trel-url)
                TREL_URL="$2"
                shift
                shift
                ;;
            --interface | -I)
                TUN_INTERFACE_NAME=$2
                shift
                shift
                ;;
            --backbone-interface | -B)
                BACKBONE_INTERFACE=$2
                shift
                shift
                ;;
            --nat64-prefix)
                NAT64_PREFIX=$2
                shift
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
}

parse_args "$@"

[ -n "$RADIO_URL" ] || RADIO_URL="spinel+hdlc+uart:///dev/ttyUSB0"
[ -n "$TREL_URL" ] || TREL_URL=""
[ -n "$TUN_INTERFACE_NAME" ] || TUN_INTERFACE_NAME="wpan0"
[ -n "$BACKBONE_INTERFACE" ] || BACKBONE_INTERFACE="eth0"
[ -n "$NAT64_PREFIX" ] || NAT64_PREFIX="64:ff9b::/96"

echo "RADIO_URL:" $RADIO_URL
echo "TREL_URL:" $TREL_URL
echo "TUN_INTERFACE_NAME:" $TUN_INTERFACE_NAME
echo "BACKBONE_INTERFACE: $BACKBONE_INTERFACE"
echo "NAT64_PREFIX:" $NAT64_PREFIX

NAT64_PREFIX=${NAT64_PREFIX/\//\\\/}
TAYGA_CONF=/etc/tayga.conf
BIND_CONF_OPTIONS=/etc/bind/named.conf.options

! test -f $TAYGA_CONF || sed -i "s/^prefix.*$/prefix $NAT64_PREFIX/" $TAYGA_CONF
! test -f $BIND_CONF_OPTIONS || sed -i "s/dns64.*$/dns64 $NAT64_PREFIX {};/" $BIND_CONF_OPTIONS
sed -i "s/$INFRA_IF_NAME/$BACKBONE_INTERFACE/" /etc/sysctl.d/60-otbr-accept-ra.conf

echo "OTBR_AGENT_OPTS=\"-I $TUN_INTERFACE_NAME -B $BACKBONE_INTERFACE -d7 $RADIO_URL $TREL_URL\"" >/etc/default/otbr-agent
echo "OTBR_WEB_OPTS=\"-I $TUN_INTERFACE_NAME -d7 -p 80\"" >/etc/default/otbr-web

/app/script/server

tail -f /var/log/syslog
