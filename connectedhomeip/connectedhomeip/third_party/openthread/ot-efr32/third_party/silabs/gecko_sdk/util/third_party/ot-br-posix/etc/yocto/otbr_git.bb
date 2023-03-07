#
#  Copyright (c) 2021, The OpenThread Authors.
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
# Sample Yocto recipe to build OTBR

DESCRIPTION = "OpenThread Border Router Agent"
HOMEPAGE = "https://github.com/openthread/ot-br-posix"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=87109e44b2fda96a8991f27684a7349c"

SRC_URI = "gitsm://git@github.com/openthread/ot-br-posix.git;branch=main;protocol=ssh"

ICECC_DISABLED = "1"

S = "${WORKDIR}/git"
SRCREV = "${AUTOREV}"
PV_append = "+${SRCPV}"

DEPENDS += "avahi boost dbus iproute2 jsoncpp ncurses"

inherit autotools cmake

EXTRA_OECMAKE += " \
    -GNinja \
    -Werror=n \
    -DBUILD_TESTING=OFF \
    -DCMAKE_BUILD_TYPE="Release" \
    -DOTBR_BACKBONE_ROUTER=OFF \
    -DOTBR_BORDER_ROUTING=ON \
    -DOTBR_COVERAGE=OFF \
    -DOTBR_MDNS=avahi \
    -DOTBR_SRP_ADVERTISING_PROXY=ON \
    -DOTBR_VENDOR_NAME="OpenThread" \
    -DOTBR_PRODUCT_NAME="Border Router" \
    -DOTBR_MESHCOP_SERVICE_INSTANCE_NAME="OpenThread Border Router" \
    -DOT_DUA=OFF \
    -DOT_POSIX_SETTINGS_PATH='"/tmp/"' \
    -DOT_MLR=OFF \
"
