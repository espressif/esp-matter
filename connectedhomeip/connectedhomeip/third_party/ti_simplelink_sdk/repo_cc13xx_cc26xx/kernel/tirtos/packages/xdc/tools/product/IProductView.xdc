/* 
 *  Copyright (c) 2009 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */
package xdc.tools.product;

/*!
 *  ======== IProductView ========
 *  Define a product as a logical group of RTSC modules
 *
 *  This interface allows a product to define logical groups of RTSC
 *  modules delivered with it. Implementations of this interface are
 *  typically consumed within development environments like IDE's to
 *  render a graphical tree view of the product consisting of these
 *  groups.
 *
 *  This is an experimental interface and is subject to change.
 */ 
metaonly interface IProductView
{
    /*!
     *  ======== ProductElemDesc ======== 
     *  ProductElemDesc structure
     * 
     *  This structure is used to define the logical tree "view" of a
     *  product. This view is composed of elements that are either modules
     *  or groups: modules at the leaves of the tree and groups elsewhere.
     *
     *  @field(elemName)        Name of element. Elements can be either
     *                          groups or modules
     *  @field(moduleName)      In case of a leaf node this field will
     *                          contain the module name. For intermediate
     *                          tree nodes the module name will be set to 
     *                          `null`
     *  @field(iconFileName)    The name of the icon file that is associated
     *                          with this element.  In xdctools 3.23, this path
     *                          must be relative to the package containing the
     *                          `IProductView` implementation.
     *
     *                          In xdctools 3.24.01 (or above), this path may
     *                          be relative or absolute.  If it's relative,
     *                          it's first searched for in the package
     *                          containing the `IProductView` implementation
     *                          module and, if this fails, the relative path
     *                          is searched via `xdc.findFile()`.  If the icon
     *                          is still not found, a default icon will be
     *                          used.
     *  @field(elemArray)       Array used to define children of a node.
     */ 
    struct ProductElemDesc {
        String elemName;             /*! Name of group or module */
        String moduleName;           /*! Name of module */
        String iconFileName;         /*! Icon file name representing element */
        ProductElemDesc elemArray[]; /*! Array of `ProductElemDesc` */
    };

    /*!
     *  ======== homeModule ========
     *  Top-level module for this product
     *
     *  The `homeModule` is a distinguished "top-level" module that is used
     *  by GUI's as a starting point, or "home page", to navigate to the
     *  other content managed by this product.
     */
    config String homeModule;

    /*!
     *  ======== linksToArray ========
     *  Array of IDs of products accessable from this product's home module
     *
     *  This array of product IDs is used to determine a hierarchical
     *  relationship between products and if a product's modules can be
     *  reached from another product's home page. If product A "links to"
     *  product B, then product B's home page can be closed so long as
     *  product A's home page is open.
     */
    config String linksToArray[];
    
    /*!
     *  ======== getProductDescriptor ========
     *  Return this product's tree view
     *
     *  This method is typically invoked by development tools like IDE's 
     *  to get the product view tree structure.
     *
     *  @a(returns)
     *  Returns root node of tree of `{@link #ProductElemDesc}` structures
     */ 
    ProductElemDesc getProductDescriptor();
}
/*
 *  @(#) xdc.tools.product; 1, 0, 0,3; 2-18-2019 11:04:00; /db/ztree/library/trees/xdctools/xdctools-h03/src/
 */

