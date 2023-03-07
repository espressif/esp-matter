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
package iar.rov.server;

/*
 *  ======== SymbolTable.xdc ========
 *  @_nodoc
 */
metaonly module SymbolTable inherits xdc.rov.ISymbolTable {

instance:

    /*
     *  ======== create ========
     *  Create takes the handle to an xdc.rov.ISymbolTable Java object.
     */
    create(any ISymInst);

}
