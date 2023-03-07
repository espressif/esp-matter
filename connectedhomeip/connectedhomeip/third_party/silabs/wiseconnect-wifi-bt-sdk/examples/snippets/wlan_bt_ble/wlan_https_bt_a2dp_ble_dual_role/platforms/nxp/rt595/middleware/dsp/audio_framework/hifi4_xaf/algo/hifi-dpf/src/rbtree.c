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
 * rbtree.c
 *
 * Red-black tree library
 ******************************************************************************/

#include "xf-dp.h"

/*******************************************************************************
 * Macros definitions
 ******************************************************************************/

/* ...node color */
#define RB_RED                          (1)
#define RB_BLK                          (0)

/* ...pointer to parent node */
#define RB_PARENT(tree, node)           ((node)->parent)

/* ...pointer to left child node */
#define RB_LEFT(tree, node)             ((node)->left)

/* ...pointer to right child node */
#define RB_RIGHT(tree, node)            ((node)->right)

/* ...pointer to right child node */
#define RB_COLOR(tree, node)            ((node)->color & 1)

/* ...check if node is black */
#define RB_IS_BLACK(tree, node)         (!((node)->color & RB_RED))

/* ...root node index of the tree - can be simplified? */
#define RB_ROOT(tree)                   RB_LEFT((tree), &(tree)->root)

/* ...empty node */
#define RB_NULL(tree)                   (&(tree)->root)

/*******************************************************************************
 * Helpers
 ******************************************************************************/

#define RB_SET_P(t, n, p)               \
    ({ (n)->parent = (p); })

#define RB_SET_L(t, n, l)               \
    ({ (n)->left = (l); })

#define RB_SET_R(t, n, r)               \
    ({ (n)->right = (r); })

#define RB_SET_C(t, n, c)               \
    RB_SET_C_##c((t), (n))

#define RB_SET_C_RB_BLK(t, n)           \
    ({ (n)->color &= ~1; })

#define RB_SET_C_RB_RED(t, n)           \
    ({ (n)->color |= 1; })

#define RB_SET_P_C(t, n, p, c)          \
    ({ (n)->parent = (p); RB_SET_C_##c(t, n); })

#define RB_SET_P_L(t, n, p, l)          \
    ({ (n)->parent = (p); (n)->left = (l); })

#define RB_SET_P_L_C(t, n, p, l, c)     \
    ({ (n)->parent = (p); (n)->left = (l); RB_SET_C_##c(t, n); })

#define RB_SET_P_R(t, n, p, r)          \
    ({ (n)->parent = (p); (n)->right = (r); })

#define RB_SET_P_R_C(t, n, p, r, c)     \
    ({ (n)->parent = (p); (n)->right = (r); RB_SET_C_##c(t, n); })

#define RB_SET_P_L_R(t, n, p, l, r)     \
    ({ (n)->parent = (p); (n)->left = (l); (n)->right = (r); })

#define RB_SET_P_L_R_C(t, n, p, l, r, c)\
    ({ (n)->parent = (p); (n)->left = (l); (n)->right = (r); RB_SET_C_##c(t, n); })

#define RB_SET_ROOT(t, n)               \
    RB_SET_L((t), &(t)->root, (n))

/*******************************************************************************
 * RB-tree functions
 ******************************************************************************/

/*******************************************************************************
 * rb_init
 *
 * Initialize rb-tree structure
 ******************************************************************************/

void rb_init(rb_tree_t *tree)
{
    /* ...initialize sentinel node of the empty tree */
    RB_SET_P_L_R_C(tree, &tree->root, RB_NULL(tree), RB_NULL(tree), RB_NULL(tree), RB_BLK);
}

/*******************************************************************************
 * rb_insert
 *
 * Insert new item into RB-tree. Returns non-zero node index on success
 ******************************************************************************/

/* ...internal tree balancing function */
static void __rb_insert_balance(rb_tree_t *tree, rb_idx_t n_idx, rb_idx_t p_idx)
{
	rb_idx_t    u_idx, g_idx, t_idx, cl_idx, cr_idx;

rebalance:

	/***************************************************************************
	 * Trivial case #1 - N (red) is a root
	 **************************************************************************/

	if (p_idx == RB_NULL(tree))
    {
		RB_SET_C(tree, n_idx, RB_BLK);
		goto root;
	}

	/***************************************************************************
	 * Trivial case #2 - P is black
	 **************************************************************************/

	if (RB_IS_BLACK(tree, p_idx))
		goto done;

	/***************************************************************************
	 * Complex cases - P is red, N is red
	 **************************************************************************/

	/* ...grandparent must exist and be black */
	g_idx = RB_PARENT(tree, p_idx);
	if (p_idx == RB_LEFT(tree, g_idx))
    {
		/* ...we are left grandchild; get uncle (if it exists) */
		u_idx = RB_RIGHT(tree, g_idx);

		/* ...if U is read, we have conditions of case #3 */
		if (!RB_IS_BLACK(tree, u_idx))
			goto case3;

		/* ...we will need grand-grand-parent later */
		t_idx = RB_PARENT(tree, g_idx);

		/* ...U is black/null; if we are LL grandchild, we have case #5 */
		if (n_idx == RB_LEFT(tree, p_idx))
			goto case5_ll;

		/* ...N is RL grandchild of G; case #4 */
		goto case4_rl;
	}
    else
    {
		/* ...we are right grandchild; get uncle (if it exists) */
		u_idx = RB_LEFT(tree, g_idx);

		/* ...if U is read, we have conditions of case #3 */
		if (!RB_IS_BLACK(tree, u_idx))
			goto case3;

		/* ...we will need grand-grand-parent later */
		t_idx = RB_PARENT(tree, g_idx);

		/* ...U is black/null; if we are RR grandchild, we have case #5 */
		if (n_idx == RB_RIGHT(tree, p_idx))
			goto case5_rr;

		/* ...N is LR grandchild of G; case #4 */
		goto case4_lr;
	}

case4_rl:

	/***************************************************************************
	 * Case #4 - P is red, U is black, N is red RL grandchild of G. We will do
	 * two tree rotations - first the one described in case #4, second is the
	 * one described in case #5 (as have conditions for case #5(LL) with P and
	 * N changing roles
	 **************************************************************************/

	cl_idx = RB_LEFT(tree, n_idx), cr_idx = RB_RIGHT(tree, n_idx);
	RB_SET_P_L_R_C(tree, n_idx, t_idx, p_idx, g_idx, RB_BLK);
	RB_SET_P_R(tree, p_idx, n_idx, cl_idx);
	RB_SET_P(tree, cl_idx, p_idx);
	RB_SET_P_L_C(tree, g_idx, n_idx, cr_idx, RB_RED);
	RB_SET_P(tree, cr_idx, g_idx);

	/* ...new root of subtree is N; adjust T pointer */
	goto case5_xx;

case4_lr:

	/***************************************************************************
	 * Case #4 - P is red, U is black, N is red LR grandchild of G. We will do
	 * two tree rotations - first the one described in case #4, second is the
	 * one described in case #5 (as have conditions for case #5(RR) with P and
	 * N changing roles
	 **************************************************************************/

	cl_idx = RB_LEFT(tree, n_idx), cr_idx = RB_RIGHT(tree, n_idx);
	RB_SET_P_L_R_C(tree, n_idx, t_idx, g_idx, p_idx, RB_BLK);
	RB_SET_P_L(tree, p_idx, n_idx, cr_idx);
	RB_SET_P(tree, cr_idx, p_idx);
	RB_SET_P_R_C(tree, g_idx, n_idx, cl_idx, RB_RED);
	RB_SET_P(tree, cl_idx, g_idx);

	/* ...new root of the subtree is N; adjust T pointer */
	goto case5_xx;

case5_ll:

	/***************************************************************************
	 * Case #5: N is LL grandchild of P; N and P red, G and U black
	 **************************************************************************/

	cr_idx = RB_RIGHT(tree, p_idx);
	RB_SET_P_L_C(tree, g_idx, p_idx, cr_idx, RB_RED);
	RB_SET_P(tree, cr_idx, g_idx);
	RB_SET_P_R_C(tree, p_idx, t_idx, g_idx, RB_BLK);

	/* ...new root of the subtree is P; relabel and adjust T pointer */
	n_idx = p_idx;
	goto case5_xx;

case5_rr:

	/***************************************************************************
	 * Case #5: N is RR grandchild of P; N and P red, G and U black
	 **************************************************************************/

	cl_idx = RB_LEFT(tree, p_idx);
	RB_SET_P_R_C(tree, g_idx, p_idx, cl_idx, RB_RED);
	RB_SET_P(tree, cl_idx, g_idx);
	RB_SET_P_L_C(tree, p_idx, t_idx, g_idx, RB_BLK);

	/* ...new root of the subtree is P; relabel and adjust T pointer */
	n_idx = p_idx;
	goto case5_xx;

case5_xx:

	/* ...N is a (black) root of subtree; check if it is a root of tree as well */
    if (t_idx == RB_NULL(tree))
        goto root;
    else if (g_idx == RB_LEFT(tree, t_idx))
        RB_SET_L(tree, t_idx, n_idx);
    else
        RB_SET_R(tree, t_idx, n_idx);
    
    goto done;

case3:

	/***************************************************************************
	 * Case #3 - P and U are red, G is black
	 **************************************************************************/

	RB_SET_C(tree, p_idx, RB_BLK);
	RB_SET_C(tree, u_idx, RB_BLK);
	RB_SET_C(tree, g_idx, RB_RED);

	/* ...rebalance the tree for a G */
	n_idx = g_idx, p_idx = RB_PARENT(tree, g_idx);
	goto rebalance;

root:
	/* ...adjust root pointer of the tree */
	RB_SET_ROOT(tree, n_idx);

done:
	/* ...tree is balanced */
	return;
}

/* ...high-level API function */
void rb_insert(rb_tree_t *tree, rb_idx_t n_idx, rb_idx_t p_idx)
{
    if (p_idx == RB_NULL(tree))
    {
        /* ...set black root node */
		RB_SET_P_L_R_C(tree, n_idx, p_idx, p_idx, p_idx, RB_BLK);

        /* ...tree consists of the only root node; is balanced */
		RB_SET_ROOT(tree, n_idx);
    }
    else
    {
        /* ...create new node - set parent pointer and paint red */
        RB_SET_P_L_R_C(tree, n_idx, p_idx, RB_NULL(tree), RB_NULL(tree), RB_RED);

        /* ...and rebalance the tree */
        __rb_insert_balance(tree, n_idx, p_idx);
    }
}

/*******************************************************************************
 * rb_delete
 *
 * Remove item from RB-key (by key). Returns zero on success
 ******************************************************************************/

/* ...internal tree balancing function */
static void __rb_delete_rebalance(rb_tree_t *tree, rb_idx_t p_idx)
{
	rb_idx_t    n_idx, s_idx, sl_idx, sr_idx, g_idx, c_idx, cl_idx, cr_idx;

    /* ...initialize rebalancing procedure with null-child of P */
    n_idx = RB_NULL(tree);
    
rebalance:

	/* ...save grand-parent pointer (may be null) */
	g_idx = RB_PARENT(tree, p_idx);

	/***************************************************************************
	 * Check for complex cases
	 **************************************************************************/

	if (n_idx == RB_LEFT(tree, p_idx))
    {
		/* ...N is left child; get sibling (must exist) and its children  */
		s_idx = RB_RIGHT(tree, p_idx);
		sl_idx = RB_LEFT(tree, s_idx);
		sr_idx = RB_RIGHT(tree, s_idx);

		/* ...if S is black, test for conditions 3,4,5,6; otherwise - case 2 */
		if (RB_IS_BLACK(tree, s_idx))
			goto test3_l;
		else
			goto case2_l;
	}
    else
    {
		/* ...N is right child; get sibling (must exist) and its children */
		s_idx = RB_LEFT(tree, p_idx);
		sl_idx = RB_LEFT(tree, s_idx);
		sr_idx = RB_RIGHT(tree, s_idx);

		/* ...if S is black, test for conditions 3,4,5,6; otherwise - case 2 */
		if (RB_IS_BLACK(tree, s_idx))
			goto test3_r;
		else
			goto case2_r;
	}

case2_l:

	/***************************************************************************
	 * Case #2: N is a left child of P; S is red
	 **************************************************************************/

	c_idx = sl_idx;
	RB_SET_P_L_C(tree, s_idx, g_idx, p_idx, RB_BLK);
	RB_SET_P_R_C(tree, p_idx, s_idx, c_idx, RB_RED);
	RB_SET_P(tree, c_idx, p_idx);

	/* ...S is new root of subtree, Sl(C) is new sibling of N; update G */
	goto case2_x;

case2_r:

	/***************************************************************************
	 * Case #2: N is a right child of P; S is red
	 **************************************************************************/

	c_idx = sr_idx;
	RB_SET_P_R_C(tree, s_idx, g_idx, p_idx, RB_BLK);
	RB_SET_P_L_C(tree, p_idx, s_idx, c_idx, RB_RED);
	RB_SET_P(tree, c_idx, p_idx);

	/* ...S is new root of subtree, Sr(C) is new sibling of N; update G */
	goto case2_x;

case2_x:

	/* ...check if S is becoming new (black) root of the tree */
    if (g_idx == RB_NULL(tree))
        RB_SET_ROOT(tree, s_idx);
    else if (p_idx == RB_LEFT(tree, g_idx))
        RB_SET_L(tree, g_idx, s_idx);
    else
        RB_SET_R(tree, g_idx, s_idx);

	/* ...relabel new N's grandparent (now S) as G and new sibling (now C) as S	 */
	g_idx = s_idx, s_idx = c_idx;
	sl_idx = RB_LEFT(tree, s_idx);
	sr_idx = RB_RIGHT(tree, s_idx);

	/* ...N is still one of P's children; select proper side */
	if (n_idx == RB_LEFT(tree, p_idx))
		goto test3_l;
	else
		goto test3_r;

test3_l:

	/***************************************************************************
	 * Test for cases 3,4,5,6; P is any, S is black. N is left child of P
	 **************************************************************************/
    
    if (!RB_IS_BLACK(tree, sr_idx))
		/* ...Sr is red, Sl of any color; conditions for case #6 are met */
		goto case6_l;
    else if (!RB_IS_BLACK(tree, sl_idx))
        /* ...Sr is black and Sl is red; conditions for case #5 are met */
        goto case5_l;
    else if (RB_IS_BLACK(tree, p_idx))
        /* ...Sl and Sr are of the same (black) color and P is black */
        goto case3;
    else
        /* ...Sl and Sr are of the same (black) color and P is red */
        goto case4;

test3_r:

	/***************************************************************************
	 * Test for cases 3,4,5,6; P is any, S is black. N is right child of P
	 **************************************************************************/

    if (!RB_IS_BLACK(tree, sl_idx))
		/* ...Sl is red, Sr of any color; conditions for case #6 are met */
		goto case6_r;
    else if (!RB_IS_BLACK(tree, sr_idx))
        /* ...Sl is black and Sr is red; conditions for case #5 are met */
        goto case5_r;
    else if (RB_IS_BLACK(tree, p_idx))
        /* ...Sl and Sr are of the same (black) color and P is black */
        goto case3;
    else
        /* ...Sl and Sr are of the same (black) color and P is red */
        goto case4;

case3:

	/***************************************************************************
	 * Case #3: N, P, S, Sl and Sr are black
	 **************************************************************************/

	RB_SET_C(tree, s_idx, RB_RED);

	/* ...and rebalance the tree for parent */
	n_idx = p_idx, p_idx = RB_PARENT(tree, p_idx);

	if (p_idx == RB_NULL(tree))
		goto done;
	else
		goto rebalance;

case4:

	/***************************************************************************
	 * Case #4: N and S are black, P is red, Sl and Sr are all black
	 **************************************************************************/

	RB_SET_C(tree, s_idx, RB_RED);
	RB_SET_C(tree, p_idx, RB_BLK);

	goto done;

case5_l:
	/***************************************************************************
	 * Case #5: S is black, Sl is red, Sr is black; N is left child of P. We
	 * have two subsequent transformations (case #5 and case #6) combined
	 **************************************************************************/

	cl_idx = RB_LEFT(tree, sl_idx);
	cr_idx = RB_RIGHT(tree, sl_idx);

	if (RB_IS_BLACK(tree, p_idx))
		RB_SET_P_L_R_C(tree, sl_idx, g_idx, p_idx, s_idx, RB_BLK);
	else
		RB_SET_P_L_R_C(tree, sl_idx, g_idx, p_idx, s_idx, RB_RED);

	RB_SET_P_R_C(tree, p_idx, sl_idx, cl_idx, RB_BLK);
	RB_SET_P(tree, cl_idx, p_idx);
	RB_SET_P_L(tree, s_idx, sl_idx, cr_idx);
	RB_SET_P(tree, cr_idx, s_idx);

	/* ...relabel new root as S (for common processing in case #6) */
	s_idx = sl_idx;
	goto case6_x;

case5_r:
	/***************************************************************************
	 * Case #5: S is black, Sr is red, Sl is black; N is right child of P. We
	 * have two subsequent transformations (case #5 and case #6) combined
	 **************************************************************************/

	cl_idx = RB_LEFT(tree, sr_idx);
	cr_idx = RB_RIGHT(tree, sr_idx);

	if (RB_IS_BLACK(tree, p_idx))
		RB_SET_P_L_R_C(tree, sr_idx, g_idx, s_idx, p_idx, RB_BLK);
	else
		RB_SET_P_L_R_C(tree, sr_idx, g_idx, s_idx, p_idx, RB_RED);

	RB_SET_P_L_C(tree, p_idx, sr_idx, cr_idx, RB_BLK);
	RB_SET_P(tree, cr_idx, p_idx);
	RB_SET_P_R(tree, s_idx, sr_idx, cl_idx);
	RB_SET_P(tree, cl_idx, s_idx);

	/* ...relabel new root as S (for common processing in case #6) */
	s_idx = sr_idx;
	goto case6_x;

case6_l:

	/***************************************************************************
	 * Case #6: S is black, N is the left child of P, Sr is red
	 **************************************************************************/

	if (RB_IS_BLACK(tree, p_idx))
		RB_SET_P_L(tree, s_idx, g_idx, p_idx);
	else
		RB_SET_P_L_C(tree, s_idx, g_idx, p_idx, RB_RED);

	RB_SET_P_R_C(tree, p_idx, s_idx, sl_idx, RB_BLK);
	RB_SET_P(tree, sl_idx, p_idx);
	RB_SET_C(tree, sr_idx, RB_BLK);

	/* ...S is a new root of subtree; update G */
	goto case6_x;

case6_r:

	/***************************************************************************
	 * Case #6: S is black, N is the right child of P, Sl is red
	 **************************************************************************/

	if (RB_IS_BLACK(tree, p_idx))
		RB_SET_P_R(tree, s_idx, g_idx, p_idx);
	else
		RB_SET_P_R_C(tree, s_idx, g_idx, p_idx, RB_RED);

	RB_SET_P_L_C(tree, p_idx, s_idx, sr_idx, RB_BLK);
	RB_SET_P(tree, sr_idx, p_idx);
	RB_SET_C(tree, sl_idx, RB_BLK);

	/* ...S is a new root of subtree; update G */
	goto case6_x;

case6_x:

	/* ...S is a new root of subtree; update G's pointer */
    if (g_idx == RB_NULL(tree))
		RB_SET_ROOT(tree, s_idx);
    else if (p_idx == RB_LEFT(tree, g_idx))
        RB_SET_L(tree, g_idx, s_idx);
    else
        RB_SET_R(tree, g_idx, s_idx);

	/* ...tree is balanced; pass through */

done:

	return;
}

/* ...high-level API function */
rb_idx_t rb_delete(rb_tree_t *tree, rb_idx_t n_idx)
{
	rb_idx_t    p_idx, t_idx, m_idx, c_idx, l_idx, r_idx, k_idx;
    UWORD32         color;
    
    /* ...save parent of element N that we are going to remove */
    p_idx = RB_PARENT(tree, n_idx);

	/* ...get in-order predecessor/successor of n_idx, if possible */
	if ((m_idx = RB_LEFT(tree, n_idx)) != RB_NULL(tree))
    {
		while ((t_idx = RB_RIGHT(tree, m_idx)) != RB_NULL(tree))
			m_idx = t_idx;

        /* ...set the child of in-order predecessor (may be null) */
		c_idx = RB_LEFT(tree, m_idx);   
	}
    else if ((m_idx = RB_RIGHT(tree, n_idx)) != RB_NULL(tree))
    {
		while ((t_idx = RB_LEFT(tree, m_idx)) != RB_NULL(tree))
			m_idx = t_idx;

        /* ...set the child of in-order successor (may be null) */
		c_idx = RB_RIGHT(tree, m_idx);
	}
    else if (p_idx == RB_NULL(tree))
    {
        /* ...tree consists of the only root node N that we are removing */
        RB_SET_ROOT(tree, m_idx);

        /* ..return tree null pointer */
        return m_idx;
    }
    else
    {
        /* ...N is a (non-root) leaf node; M and C are null */
		c_idx = m_idx;

        /* ...save the color of the node we are going to delete */
        color = RB_COLOR(tree, n_idx);
                        
        /* ...set new parent of C */
        t_idx = p_idx;
        
        /* ...pointer that we return as in-order predecessor/successor */
        k_idx = p_idx;
        
        /* ...adjust only parent of the N */
        goto adjust_parent;
    }

    /* ...node that replaces our component is M */
    k_idx = m_idx;
    
	/***************************************************************************
	 * Replace node N with M
	 **************************************************************************/

    /* ...save original color of M (the node that we are deleting) */
    color = RB_COLOR(tree, m_idx);    

    /* ...put M in place of N; get N's children */
    l_idx = RB_LEFT(tree, n_idx);
    r_idx = RB_RIGHT(tree, n_idx);

    /* ...see if M is a child of N */
    if ((t_idx = RB_PARENT(tree, m_idx)) != n_idx)
    {
        /* ...C becomes left or right child of M's original parent T */
        if (c_idx == RB_LEFT(tree, m_idx))
            RB_SET_R(tree, t_idx, c_idx);
        else
            RB_SET_L(tree, t_idx, c_idx);
        
        /* ...adjust C parent pointer (okay if it's null)  */
        RB_SET_P(tree, c_idx, t_idx);

        /* ...set all pointers of node M (it replaces N) */
        RB_SET_P_L_R(tree, m_idx, p_idx, l_idx, r_idx);
        RB_SET_P(tree, l_idx, m_idx);
        RB_SET_P(tree, r_idx, m_idx);
    }
    else
    {
        /* ...M is a left or right child of N; it gets to N's place, and C remains intact */
        if (m_idx == l_idx)
        {
            RB_SET_P_R(tree, m_idx, p_idx, r_idx);
            RB_SET_P(tree, r_idx, m_idx);
        }
        else
        {
            RB_SET_P_L(tree, m_idx, p_idx, l_idx);
            RB_SET_P(tree, l_idx, m_idx);
        }

        /* ...parent of C is still M (we label it as T) */
        t_idx = m_idx;
    }

    /* ...paint M in the same color as N which it replaced */
    if (RB_IS_BLACK(tree, n_idx))
        RB_SET_C(tree, m_idx, RB_BLK);
    else
        RB_SET_C(tree, m_idx, RB_RED);
    
adjust_parent:

    /* ...adjust N's parent node to point to M */
    if (p_idx == RB_NULL(tree))
        RB_SET_ROOT(tree, m_idx);
    else if (n_idx == RB_LEFT(tree, p_idx))
        RB_SET_L(tree, p_idx, m_idx);
    else
        RB_SET_R(tree, p_idx, m_idx);

	/* ...check for a color of deleted item (M or N in case it is a leaf) */
	if (color == RB_BLK)
    {
		if (c_idx == RB_NULL(tree))
            /* ...rebalance the tree for a T node (it is never a null)*/
            __rb_delete_rebalance(tree, t_idx);
		else
            /* ...C node exists and is necessarily red; repaint it black */
            RB_SET_C(tree, c_idx, RB_BLK);
	}

    /* ....return the node K which replaced deleted node N */
    return k_idx;
}

/*******************************************************************************
 * rb_replace
 *
 * Replace the node with the same-key node - adjust tree pointers
 ******************************************************************************/

void rb_replace(rb_tree_t *tree, rb_idx_t n_idx, rb_idx_t t_idx)
{
    rb_idx_t    p_idx, l_idx, r_idx;
    
    /* ...get node pointers */
    p_idx = RB_PARENT(tree, n_idx), l_idx = RB_LEFT(tree, n_idx), r_idx = RB_RIGHT(tree, n_idx);

    /* ...set new node pointers */
    RB_SET_P_L_R(tree, t_idx, p_idx, l_idx, r_idx);
    
    /* ...set node color */
    if (RB_IS_BLACK(tree, n_idx))
        RB_SET_C(tree, t_idx, RB_BLK);
    else
        RB_SET_C(tree, t_idx, RB_RED);

    /* ...update parent node */
    if (p_idx == RB_NULL(tree))
        RB_SET_ROOT(tree, t_idx);
    else if (n_idx == RB_LEFT(tree, p_idx))
        RB_SET_L(tree, p_idx, t_idx);
    else
        RB_SET_R(tree, p_idx, t_idx);
    
    /* ...update children's parent node (okay if null) */
    RB_SET_P(tree, l_idx, t_idx), RB_SET_P(tree, r_idx, t_idx); 
}
