/* --COPYRIGHT--,EPL
 *  Copyright (c) 2014-2015 Texas Instruments Incorporated
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/

var mi;
/*
 *  ======== instance$meta$init ========
 */
function instance$meta$init(memoryImageInstance)
{
    mi = memoryImageInstance;
}

/*
 *  ======== clearRegisters ========
 */
function clearRegisters()
{
    mi.jclearRegisters();
}

/*
 *  ======== fetchRegisters ========
 */
function fetchRegisters(names)
{
    var jnames = java.lang.reflect.Array.newInstance(java.lang.String,
        names.length);
    for (var i = 0; i < names.length; i++) {
        jnames[i] = names[i];
    }
    mi.jfetchRegisters(jnames);
}

/*
 *  ======== getRegister ========
 */
function getRegister(name)
{
    var buffer = java.lang.reflect.Array.newInstance(java.lang.Integer.TYPE, 1);
    mi.jreadRegister(buffer, name);
    return (buffer[0]);
}

/*
 *  ======== setRegister ========
 */
function setRegister(name, value)
{
    mi.jwriteRegister(value, name);
}

/*
 *  ======== toText ========
 */
function toText()
{
    var res = mi.jreadCallStack();
    /* A string is added because, Hwi expects it, but that assumption should
     * be removed from Hwi.
     */
    res = res + "Unwind halted";
    return (res);
}
