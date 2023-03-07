/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* This file can be loaded in .syscfg files programmatically in order to load
 * the tirtos 7 kernel configuration that used to be provided as default on
 * tirtos 6.
 */

var board = system.deviceData.board.name;

if (system.getRTOS() === "tirtos7") {
    if (board.match(/CC13.1/) || board.match(/CC26.1/)) {
	    const tirtos7_release_syscfg_js = system.getScript("cc13x1_cc26x1/release/tirtos7_release.syscfg.js");
    }
    else if (board.match(/CC13.2/) || board.match(/CC26.2/)) {
	    const tirtos7_release_syscfg_js = system.getScript("cc13x2_cc26x2/release/tirtos7_release.syscfg.js");
    }
    else if (board.match(/CC13.2.7/) || board.match(/CC26.2.7/)) {
	    const tirtos7_release_syscfg_js = system.getScript("cc13x2x7_cc26x2x7/release/tirtos7_release.syscfg");
    }
    else if (board.match(/CC13.4/) || board.match(/CC26.4/) || board.match(/CC26.3/)) {
	    const tirtos7_release_syscfg_js = system.getScript("cc13x4_cc26x4/release/tirtos7_release.syscfg.js");
    }
    else if (board.match(/CC32/)) {
	    const tirtos7_release_syscfg_js = system.getScript("cc32xx/release/tirtos7_release.syscfg.js");
    }
}