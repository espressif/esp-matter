/* --COPYRIGHT--,EPL
 *  Copyright (c) 2012 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/

/*
 *  ======== instance$meta$init ========
 *  Takes a handle to an Elf parser, which serves as the symbol table.
 */

function instance$meta$init(ISymInst)
{
    this.$$bind('$symTab', ISymInst);
}

/*
 *  ======== getSymbolValue ========
 *  This function returns the address of the requested symbol
 */
function getSymbolValue(symbolName)
{
    return (this.$symTab.getSymbolValue(symbolName));
}

/*
 *  ======== lookupDataSymbol ========
 *  This function returns an array of symbols that have the specified value.
 */
function lookupDataSymbol(addr)
{
    return (this.$symTab.lookupDataSymbol(addr));
}

/*
 *  ======== lookupFuncName ========
 *  Lookup the function name(s) at a given address.
 */
function lookupFuncName(addr)
{
    return (this.$symTab.lookupFuncName(addr));
}

/*
 *  ======== getISA ========
 *  This is not used
 */
function getISA()
{
    return (this.$symTab.getISA());
}

/*
 *  ======== getJavaImp ========
 */
function getJavaImp()
{
    return (this.$symTab);
}
