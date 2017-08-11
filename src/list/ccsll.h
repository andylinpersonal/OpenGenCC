#ifndef OPENGC3_LIST_CCSLL_H
#define OPENGC3_LIST_CCSLL_H

#include "base.h"
#include "../base/pool.h"
#include "../base/misc.h"
#include "../base/snym.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


/* ccsll create */


#define ccsll(elem_t)                                                          \
                                                                               \
        ccsll_extd(elem_t, 1, NORMAL)

#define ccsll_pckd(elem_t)                                                     \
                                                                               \
        ccsll_extd(elem_t, 1, PACKED)

#define ccsll_extd(elem_t, _n_iter, _ALIGN_)                                   \
                                                                               \
        typedef ccsll_struct_extd(elem_t, _n_iter, _ALIGN_) *CCSLL;  CCSLL

#define ccsll_type(elem_t)                                                     \
                                                                               \
        typedef ccsll_struct_extd(elem_t, 1, NORMAL) *


#define ccsll_struct(elem_t)                                                   \
                                                                               \
        ccsll_struct_extd(elem_t, 1, NORMAL)

#define ccsll_struct_pckd(elem_t)                                              \
                                                                               \
        ccsll_struct_extd(elem_t, 1, PACKED)

#define ccsll_struct_extd(elem_t, _n_iter, _ALIGN_)                            \
                                                                               \
    struct CCSLL_CONT                                                          \
    {                                                                          \
        int size,  last,  vcnt;                   /* size and node record */   \
        int start, ratio, thrsh;                  /* block increment info */   \
                                                                               \
        struct CCSLL_NODE                                                      \
        {   struct CCSLL_NODE *lnk[1];                                         \
            elem_t val;                           /* val with a next link */   \
        }   *avsp, *pnode, head, tail, swap;      /* available space list */   \
                                                                               \
        struct CCSLL_BLCK                                                      \
        {   struct CCSLL_BLCK *bprv, *bnxt;       /* points to prev block */   \
            int ncnt;                             /* the item of the node */   \
            PRAGMA_##_ALIGN_##_BGN                /* packed pragma starts */   \
            struct CCSLL_NODE nodes[];            /* node structure array */   \
            PRAGMA_##_ALIGN_##_END                /* the pragma ends here */   \
        }   *pool, *pblock;                       /* points to 1-st block */   \
                                                                               \
        struct CCSLL_ITER                                                      \
        {   struct CCSLL_PTRS                                                  \
            {   struct CCSLL_NODE *node;                                       \
            }   prev, curr;                       /* points to p./c. node */   \
            struct CCSLL_CONT *ccsll;             /* points to ccsll body */   \
        }   (*itarr)[_n_iter], *_iter, **_it;                                  \
                                                                               \
        struct CCSLL_CONT **_co;                  /* internal use _it _co */   \
                                                                               \
        int _it_base, _it_limit;                                               \
        int _co_base, _co_limit;                                               \
    }



/* ccsll initialize */


#define ccsll_init(_ccsll)                                                     \
                                                                               \
        ccsll_init_extd(_ccsll,     16,      2,  65536)

#define ccsll_init_extd(_ccsll, _start, _ratio, _thrsh)                        \
                                                                               \
        cc_ll_init_extd(_ccsll, _start, _ratio, _thrsh, ccsll)


#define _ccsll_init(_ccsll_dst, _ccsll_src, _alloc)                            \
                                                                               \
        _ccsll_init_extd(_ccsll_dst, -1,     -1,     -1, _alloc)

#define _ccsll_init_extd(_ccsll, _start, _ratio, _thrsh, _alloc)               \
                                                                               \
        _cc_ll_init_extd(_ccsll, _start, _ratio, _thrsh, _alloc, ccsll)


#define _ccsll_init_core(_ccsll)                                               \
                                                                               \
        _cc_ll_init_core(_ccsll, ccsll)


#define _ccsll_init_seed(_ccsll)                                               \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_ccsll)->size = 0,                                                        \
    (_ccsll)->last = (_ccsll)->vcnt = 0,                                       \
                                                                               \
    (_ccsll)->head.NXT = &((_ccsll)->tail),                                    \
    (_ccsll)->tail.NXT = NULL                                                  \
)


#define _ccsll_init_info(_ccsll, _start, _ratio, _thrsh)                       \
                                                                               \
        _cc_ll_init_info(_ccsll, _start, _ratio, _thrsh)


#define ccsll_iter_init(_iter, _ccsll)                                         \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter)->prev.node = NULL,                                                 \
    (_iter)->curr.node = NULL,                                                 \
    (_iter)->ccsll = (_ccsll)                                                  \
)


#define _ccsll_iter_init(_iter, _ccsll, _alloc)                                \
                                                                               \
        _cc_ll_iter_init(_iter, _ccsll, _alloc, ccsll)



/* ccsll destroy */


#define ccsll_free(_ccsll)  cc_ll_free(_ccsll)



/* ccsll access */


#define ccsll_front(_ccsll)  ((_ccsll)->head.NXT->val)



/* ccsll capacity */


#define ccsll_size(_ccsll)   ((_ccsll)->size)

#define ccsll_empty(_ccsll)  ((_ccsll)->head.NXT == &((_ccsll)->tail))



/* ccsll modifiers */


#define ccsll_push_front(_ccsll, _val)                                         \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    ccsll_push_front_alloc((_ccsll));                                          \
                                                                               \
    (_ccsll)->head.NXT->val = (_val);                                          \
)


#define ccsll_push_front_alloc(_ccsll)                                         \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    _node_alloc((_ccsll)->pnode, (_ccsll));                                    \
                                                                               \
    (_ccsll)->pnode->NXT = (_ccsll)->head.NXT;                                 \
    (_ccsll)->head.NXT   = (_ccsll)->pnode;                                    \
                                                                               \
    (_ccsll)->size++;                                                          \
)


#define  ccsll_pop_front(_ccsll)                                               \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccsll_empty((_ccsll)))  break;                                         \
                                                                               \
    (_ccsll)->pnode    = (_ccsll)->head.NXT;                                   \
    (_ccsll)->head.NXT = (_ccsll)->pnode->NXT;                                 \
                                                                               \
    _node_clear((_ccsll)->pnode, (_ccsll));                                    \
                                                                               \
    (_ccsll)->size--;                                                          \
)


#define ccsll_insert(_iter, _val)                                              \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccsll_iter_at_head((_iter)))  break;                                   \
                                                                               \
    _node_alloc((_iter)->ccsll->pnode, (_iter)->ccsll);                        \
                                                                               \
    /* TODO */                                                                 \
                                                                               \
    (_iter)->ccsll->size++;                                                    \
)


#define ccsll_erase(_iter)                                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccsll_iter_at_head((_iter)) || ccsll_iter_at_tail((_iter)))  break;    \
                                                                               \
    /* TODO */                                                                 \
                                                                               \
    _node_clear((_iter)->curr.node, (_iter)->ccsll);                           \
                                                                               \
    (_iter)->ccsll->size--;                                                    \
)


#define ccsll_swap(_ccsll_a, _ccsll_b)                                         \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    XOR2_SWAP((_ccsll_a), (_ccsll_b));                                         \
)


#define ccsll_resize(_ccsll, _items, _val)                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _size = ccsll_size((_ccsll)) - (_items);                               \
                                                                               \
         if (_size > 0)  while(_size--)  ccsll_pop_front ((_ccsll));           \
    else if (_size < 0)  while(_size++)  ccsll_push_front((_ccsll), (_val));   \
)


#define ccsll_clear(_ccsll)                                                    \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    while (!(ccsll_empty((_ccsll))))     ccsll_pop_front((_ccsll));            \
)



/* ccsll operations */


#define ccsll_move(_iter_p, _iter_i)                                           \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (_unlikely((_iter_i)->curr.node == (_iter_p)->curr.node->PRV))  break;  \
                                                                               \
    if (_unlikely(ccsll_iter_at_head((_iter_p)) ||                             \
                  ccsll_iter_at_head((_iter_i)) ||                             \
                  ccsll_iter_at_tail((_iter_i))))  break;                      \
                                                                               \
    /* TODO */                                                                 \
                                                                               \
    (_iter_i)->ccsll->size--;                                                  \
    (_iter_i)->ccsll = (_iter_p)->ccsll;                                       \
    (_iter_i)->ccsll->size++;                                                  \
)


#define ccsll_move_range(_iter_p, _iter_l, _iter_r)                            \
                                                                               \
        ccsll_move_range_extd(_iter_p, _iter_l, _iter_r,    -1)

#define ccsll_move_range_extd(_iter_p, _iter_l, _iter_r, _dist)                \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (_unlikely((_iter_l)->ccsll != (_iter_r)->ccsll))  break;               \
                                                                               \
    if (_unlikely((_iter_p)->ccsll != (_iter_l)->ccsll))                       \
    {                                                                          \
        int _dist_m = (_dist);                                                 \
                                                                               \
        if (_dist_m < 0)                                                       \
            ccsll_iter_distance((_iter_l), (_iter_r), &_dist_m);               \
                                                                               \
        (_iter_p)->ccsll->size += _dist_m;                                     \
        (_iter_l)->ccsll->size -= _dist_m;                                     \
        (_iter_l)->ccsll = (_iter_p)->ccsll;                                   \
    }                                                                          \
                                                                               \
    if (_unlikely((_iter_l)->curr.node == (_iter_r)->curr.node))  break;       \
                                                                               \
    /* TODO */                                                                 \
)


#define  ccsll_merge(_ccsll_d, _ccsll_s)                                       \
                                                                               \
         ccsll_merge_extd(_ccsll_d, _ccsll_s, DLEQ)

#define  ccsll_merge_extd(_ccsll_d, _ccsll_s, _leq)                            \
                                                                               \
         cc_ll_merge_extd(_ccsll_d, _ccsll_s, _leq, ccsll, )

#define _ccsll_merge_extd(_ccsll_d, _iter_l, _iter_m, _iter_r, _leq)           \
                                                                               \
        _cc_ll_merge_extd(_ccsll_d, _iter_l, _iter_m, _iter_r, _leq, ccsll, )


#define  ccsll_merge_range(_iter_l, _iter_m, _iter_r)                          \
                                                                               \
         ccsll_merge_range_extd(_iter_l, _iter_m, _iter_r, DLEQ)

#define  ccsll_merge_range_extd(_iter_l, _iter_m, _iter_r, _leq)               \
                                                                               \
         cc_ll_merge_range_extd(_iter_l, _iter_m, _iter_r, _leq, ccsll, )

#define _ccsll_merge_range_extd(_iter_l, _iter_m, _iter_r, _iter_x, _leq)      \
                                                                               \
        _cc_ll_merge_range_extd(_iter_l, _iter_m, _iter_r, _iter_x, _leq, ccsll)


#define  ccsll_sort(_ccsll)                                                    \
                                                                               \
         ccsll_sort_extd(_ccsll, DLEQ)

#define  ccsll_sort_extd(_ccsll, _leq)                                         \
                                                                               \
         cc_ll_sort_extd(_ccsll, _leq, ccsll, )

#define _ccsll_sort_extd(_ccsll, _carry, _pbuck, _iter_a, _iter_b, _leq)       \
                                                                               \
        _cc_ll_sort_extd(_ccsll, _carry, _pbuck, _iter_a, _iter_b, _leq, ccsll,)


#define ccsll_is_sorted(_ccsll, _ptrue)                                        \
                                                                               \
        ccsll_is_sorted_extd(_ccsll, SLEQ, _ptrue)

#define ccsll_is_sorted_extd(_ccsll, _leq, _ptrue)                             \
                                                                               \
        cc_ll_is_sorted_extd(_ccsll, SLEQ, _ptrue, ccsll)



/* ccsll comparator */


#define ccsll_comp_leq(_iter_a, _iter_b)  (SREF((_iter_a)) <= SREF((_iter_b)))

#define ccsll_comp_geq(_iter_a, _iter_b)  (SREF((_iter_a)) >= SREF((_iter_b)))



/* ccsll iterators */


#define ccsll_iter_copy(_iter_dst, _iter_src)                                  \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    *(_iter_dst) = *(_iter_src)                                                \
)


#define ccsll_iter_head(_iter)                                                 \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter)->prev.node = NULL,                                                 \
    (_iter)->curr.node = &((_iter)->ccsll->head)                               \
)


#define ccsll_iter_begin(_iter)                                                \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter)->prev.node = &((_iter)->ccsll->head),                              \
    (_iter)->curr.node =  ((_iter)->ccsll->head.NXT)                           \
)


#define ccsll_iter_at_head(_iter)   ( (_iter)->curr.node ==                    \
                                    &((_iter)->ccsll->head) )

#define ccsll_iter_at_tail(_iter)   ( (_iter)->curr.node ==                    \
                                    &((_iter)->ccsll->tail) )

#define ccsll_iter_at_begin(_iter)  ( (_iter)->curr.node ==                    \
                                      (_iter)->ccsll->head.NXT )

#define ccsll_iter_at_end(_iter)    ( (_iter)->curr.node->NXT ==               \
                                    &((_iter)->ccsll->tail))


#define ccsll_iter_incr(_iter)                                                 \
(                                                                              \
    (ccsll_iter_at_tail(_iter)) ? (NULL) :                                     \
    ((_iter)->prev.node = (_iter)->curr.node,                                  \
     (_iter)->curr.node = (_iter)->curr.node->NXT)->NXT                        \
)


#define ccsll_iter_advance(_iter, _diff)                                       \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _len = (_diff);                                                        \
                                                                               \
    if (_len > 0)  while (ccsll_iter_incr((_iter)) && --_len);                 \
)


#define ccsll_iter_distance(_iter_a, _iter_b, _pdist)                          \
                                                                               \
        cc_ll_iter_distance(_iter_a, _iter_b, _pdist, ccsll)



/* ccsll traversor */


#define CCSLL_INCR(_iter)  CC_LL_INCR(_iter, ccsll)

#ifndef CC_STRICT

#define CCSLL_INCR_AUTO(_pval, _ccsll)                                         \
                                                                               \
        CCSLL_INCR_EXTD(_pval, _ccsll, (void)0)

#define CCSLL_INCR_EXTD(_pval, _ccsll, ...)                                    \
                                                                               \
        CC_LL_INCR_EXTD(_pval, _ccsll, ccsll, __VA_ARGS__)

#endif // CC_STRICT



#endif