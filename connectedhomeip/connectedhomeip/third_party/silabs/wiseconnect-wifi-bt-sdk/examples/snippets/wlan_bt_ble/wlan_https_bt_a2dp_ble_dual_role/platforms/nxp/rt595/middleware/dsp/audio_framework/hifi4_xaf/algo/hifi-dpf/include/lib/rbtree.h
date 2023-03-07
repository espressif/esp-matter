/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
/*******************************************************************************
 * rbtree.h
 *
 * Generic implmentation of red-black trees
 *******************************************************************************/

#ifndef __RBTREE_H
#define __RBTREE_H

/*******************************************************************************
 * Red-black tree node definition
 ******************************************************************************/

/* ...reference to rb-tree node */
typedef struct rb_node  *rb_idx_t;

/* ...rb-tree node */
typedef struct rb_node
{
    /* ...pointers to parent and two children */
    rb_idx_t            parent, left, right;
    
    /* ...node color (least-significant-bit only) */
    UWORD32                 color;

}   rb_node_t;

/* ...rb-tree data */
typedef struct rb_tree_t
{
    /* ...tree sentinel node */
    rb_node_t           root;

}   rb_tree_t;    

/*******************************************************************************
 * Helpers
 ******************************************************************************/

/* ...left child accessor */
static inline rb_idx_t rb_left(rb_tree_t *tree, rb_idx_t n_idx)
{
    return n_idx->left;
}

/* ...right child accessor */
static inline rb_idx_t rb_right(rb_tree_t *tree, rb_idx_t n_idx)
{
    return n_idx->right;
}

/* ...parent node accessor */
static inline rb_idx_t rb_parent(rb_tree_t *tree, rb_idx_t n_idx)
{
    return n_idx->parent;
}

/* ...tree root node accessor */
static inline rb_idx_t rb_root(rb_tree_t *tree)
{
    return rb_left(tree, &tree->root);
}

/* ...tree data pointer accessor */
static inline rb_idx_t rb_cache(rb_tree_t *tree)
{
    return rb_right(tree, &tree->root);
}

/* ...tree null node */
static inline rb_idx_t rb_null(rb_tree_t *tree)
{
    return &tree->root;
}

/* ...get user-bits stored in node color */
static inline UWORD32 rb_node_data(rb_tree_t *tree, rb_idx_t n_idx)
{
    return (n_idx->color >> 1);
}

/* ...left child assignment */
static inline void rb_set_left(rb_tree_t *tree, rb_idx_t n_idx, rb_node_t *child)
{
    n_idx->left = child;
}

/* ...right child assignment */
static inline void rb_set_right(rb_tree_t *tree, rb_idx_t n_idx, rb_node_t *child)
{
    n_idx->right = child;
}

/* ...cache tree client index */
static inline void rb_set_cache(rb_tree_t *tree, rb_idx_t c_idx)
{
    tree->root.right = c_idx;
}

/* ...get user-bits stored in node color */
static inline void rb_set_node_data(rb_tree_t *tree, rb_idx_t n_idx, UWORD32 data)
{
    n_idx->color = (n_idx->color & 0x1) | (data << 1);
}

/*******************************************************************************
 * API functions
 ******************************************************************************/

/* ...initialize rb-tree */
extern void     rb_init(rb_tree_t *tree);

/* ...insert node into tree as a child of p */
extern void     rb_insert(rb_tree_t *tree, rb_idx_t n_idx, rb_idx_t p_idx);

/* ...replace the node with same-key value and fixup tree pointers */
extern void     rb_replace(rb_tree_t *tree, rb_idx_t n_idx, rb_idx_t t_idx);

/* ...delete node from the tree and return its in-order predecessor/successor */
extern rb_idx_t rb_delete(rb_tree_t *tree, rb_idx_t n_idx);

/* ...first in-order item in the tree */
extern rb_idx_t rb_first(rb_tree_t *tree);

/* ...last in-order item in the tree */
extern rb_idx_t rb_last(rb_tree_t *tree);

/* ...forward tree iterator */
extern rb_idx_t rb_next(rb_tree_t *tree, rb_idx_t n_idx);

/* ...backward tree iterator */
extern rb_idx_t rb_prev(rb_tree_t *tree, rb_idx_t n_idx);

#endif  /* __RBTREE_H */
