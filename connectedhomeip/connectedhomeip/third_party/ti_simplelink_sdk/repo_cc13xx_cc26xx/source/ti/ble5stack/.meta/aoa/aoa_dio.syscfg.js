/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 *  ======== aoa_dio.syscfg.js ========
 */

"use strict";

let config = [
    {
     name         : 'dio',
     displayName  : 'DIO',
     default: -1,
     hidden: false,
     options: [
        {name: -1 , displayName: "None",},
        {name: 0},
        {name: 1},
        {name: 2},
        {name: 3},
        {name: 4},
        {name: 5},
        {name: 6},
        {name: 7},
        {name: 8},
        {name: 9},
        {name: 10},
        {name: 11},
        {name: 12},
        {name: 13},
        {name: 14},
        {name: 15},
        {name: 16},
        {name: 17},
        {name: 18},
        {name: 19},
        {name: 20},
        {name: 21},
        {name: 22},
        {name: 23},
        {name: 24},
        {name: 25},
        {name: 26},
        {name: 27},
        {name: 28},
        {name: 29},
        {name: 30}
    ]
    }
]

/*
 *  ======== exports ========
 *  Export the Antenna Settings Configuration
 */
exports = {
    name                 : "dio",
    displayName          : "DIO_",
    maxInstances         : 7905,  // Max of 255 antenna table entry and max of 31 DIOs for ecach entry
    config               : config,
};
