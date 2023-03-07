/*
 * Copyright (c) 2012-2017, Texas Instruments Incorporated
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
/*
 *  ======== Queue.xs ========
 *
 */

var Queue = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    Queue = this;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Queue.common$.namedInstance = false;

    xdc.useModule("ti.sysbios.hal.Hwi");
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    if (obj.elem.next === undefined) {  /* workaround for RTSC bug 321911 */
        obj.elem.next = obj.elem.prev = obj.elem;
    }
}

/*
 *  ======== putMeta ========
 */
function putMeta(elem)
{
    var obj = this.$object;

    if (obj.elem.next === undefined) {  /* workaround for RTSC bug 321911 */
        elemClearMeta(obj.elem);
    }   

    elem.next = obj.elem;
    elem.prev = obj.elem.prev;
    obj.elem.prev.next = elem;
    obj.elem.prev = elem;
}

/*
 *  ======== elemClearMeta ========
 */
function elemClearMeta(elem)
{
    elem.next = elem.prev = elem;
}

/*
 *  ======== headMeta ========
 */
function headMeta()
{
    var obj = this.$object;
    if (obj.elem.next != obj.elem) {
        return (obj.elem.next);
    }

    return null;
}

/*
 *  ======== nextMeta ========
 */
function nextMeta(elem)
{
    var obj = this.$object;
    if (elem.next != obj.elem) {
        return elem.next;
    }

    return null;
}

/*
 *  ======== insertMeta ========
 */
function insertMeta(qelem, elem)
{
    var prev;

    prev = qelem.prev;
    elem.next = qelem;
    elem.prev = prev;
    prev.next = elem;
    qelem.prev = elem;
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    if (Queue.common$.namedInstance == true) {
        Queue.$logError("The Queue module does not support namedInstances.", 
                Queue.common$, Queue.common$.namedInstance);
    }
}

/*
 *  ======== viewCheckForNullObject ========
 *  Returns true if the object is all zeros.
 */
function viewCheckForNullObject(mod, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var objSize = mod.Instance_State.$sizeof();

    /* skip uninitialized objects */
    try {
        var objArray = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt8',
                                    isScalar: true},
                                    Number(obj.$addr),
                                    objSize,
                                    true);
    }
    catch(e) {
        print(e.toString());
    }

    for (var i = 0; i < objSize; i++) {
        if (objArray[i] != 0) return (false);
    }

    return (true);
}

/*
 *  ======== instance$view$init ========
 */
function viewInitInstance(view, obj)
{
    var Queue = xdc.useModule('ti.sysbios.knl.Queue');
    
    if (viewCheckForNullObject(Queue, obj)) {
        view.label = "Uninitialized Queue object";
        return;
    }

    view.label = obj.$label;
    
    /* 
     * To check for loops, store each address as a key in a map,
     * then with each new address check first if it is already in
     * the map.
     */
    var addrs = {};
    
    var e = obj.elem;
    while (Number(e.next) != Number(obj.$addr)) {
        //print("e.$addr: " + e.$addr + " e.next: " + e.next);
        
        /* Before fetching the next element, verify we're not in a loop. */
        if (e.next in addrs) {
            view.$status["elems"] = "Queue contains loop. Element " 
                + e.$addr + " points to element " + e.next;
            break;
        }
        
        /* Fetch the next element and add it to the array of elements. */
        e = Program.fetchStruct(Queue.Elem$fetchDesc, e.next);
        view.elems.$add(e.$addr);
        
        /* 
         * Add the address to a map so we can check for loops. 
         * The value 'true' is just a placeholder to make sure the address
         * is in the map.
         */
        addrs[e.$addr] = true;
    }
}
