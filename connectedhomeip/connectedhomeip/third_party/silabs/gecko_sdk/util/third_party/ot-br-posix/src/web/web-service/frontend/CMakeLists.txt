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

set(NPM_CSS_DEPENDENCIES
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/angular-material/angular-material.min.css
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/material-design-lite/material.min.css
)
set(NPM_JS_DEPENDENCIES
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/angular-animate/angular-animate.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/angular-aria/angular-aria.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/angular-material/angular-material.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/angular-messages/angular-messages.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/angular/angular.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/d3/d3.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/node_modules/material-design-lite/material.min.js
)

add_custom_target(otbr-web-frontend
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/package.json .
    COMMAND npm install)

install(FILES index.html join.dialog.html
    DESTINATION ${OTBR_WEB_DATADIR}/frontend)

install(DIRECTORY res
    DESTINATION ${OTBR_WEB_DATADIR}/frontend)

install(FILES ${NPM_JS_DEPENDENCIES}
    DESTINATION ${OTBR_WEB_DATADIR}/frontend/res/js)

install(FILES ${NPM_CSS_DEPENDENCIES}
    DESTINATION ${OTBR_WEB_DATADIR}/frontend/res/css)
