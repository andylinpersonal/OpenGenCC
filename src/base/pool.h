#ifndef OPENGC3_BASE_POOL_H
#define OPENGC3_BASE_POOL_H

#include "mesg.h"
#include "misc.h"

#include <stdio.h>
#include <string.h>


/* safe allocation */


#define _safe_alloc(_void_ptr, _alloc_bytes)                                   \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if ((_void_ptr) != NULL)                                                   \
        CC_ERROR(CC_ERROR_MSG_MEMORY_LEAK);                                    \
                                                                               \
    if (((_void_ptr) = malloc((_alloc_bytes))) == NULL)                        \
        CC_FATAL(CC_FATAL_MSG_MALLOC_FAIL);                                    \
)


#define _safe_free(_void_ptr)                                                  \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if ((_void_ptr) == NULL)                                                   \
        CC_ERROR(CC_ERROR_MSG_DOUBLE_FREE);                                    \
                                                                               \
    free((_void_ptr));                                                         \
    (_void_ptr) = NULL;                                                        \
)



/* cont / iter */


#define _cont_alloc(_cont)  _safe_alloc((_cont), sizeof(*(_cont)))

#define _cont_free(_cont)   _safe_free ((_cont))

#define _iter_alloc(_iter)  _safe_alloc((_iter), sizeof(*(_iter)))

#define _iter_free(_iter)   _safe_free ((_iter))



/* itarr */


#define _itarr_init(_cont, _ll_)                                               \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    _safe_alloc((_cont)->itarr, sizeof(*(_cont)->itarr));                      \
                                                                               \
    for (int _idx = 0; _idx < (int)(ELEMOF(*(_cont)->itarr)); _idx++)          \
        _ll_##_iter_init(&((*(_cont)->itarr)[_idx]), (_cont));                 \
)


#define _itarr_free(_cont)                                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    _safe_free((_cont)->itarr);                                                \
)



/* _it / _co management */


#define _itco_total(_cont, _itco_)                                             \
                                                                               \
        ((_cont)->_itco_##_base + (_cont)->_itco_##_limit)


#ifndef CC_STRICT

#define _it_init(_cont, _items, _iter, _ll_)                                   \
                                                                               \
        __typeof__(**(_cont)->_it) _iter[(_items)];                            \
                                                                               \
        for (int _cnt = 0; _cnt < (_items); _cnt++)                            \
            _ll_##_iter_init(_it_((_cont), _iter, _cnt), (_cont))

#else

#define _it_init(_cont, _items, _base, _ll_)                                   \
                                                                               \
        int _base;                                                             \
                                                                               \
        _itco_init((_cont), (_items), &(_base), _##_ll_##_iter_init, _it)

#endif // CC_STRICT


#define _co_init(_cont, _items, _base, _ll_)                                   \
                                                                               \
        int _base;                                                             \
                                                                               \
        _itco_init((_cont), (_items), &(_base), _##_ll_##_init, _co)


#define _itco_init(_cont, _items, _pbase, _pinit, _itco_)                      \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    _auxr_alloc((_cont), (_items), (_pbase), _itco_);                          \
                                                                               \
    for (int _idx = (*(_pbase)); _idx < (*(_pbase) + (_items)); _idx++)        \
        _pinit((_cont)->_itco_[_idx], (_cont), !((_cont)->_itco_[_idx]));      \
)


#ifndef CC_STRICT

#define _it_clear(_cont, _items)

#else

#define _it_clear(_cont, _items)  _itco_clear(_cont, _items, _it)

#endif // CC_STRICT


#define _co_clear(_cont, _items)                                               \
                                                                               \
        _itco_clear(_cont, _items, _co)


#define _itco_clear(_cont, _items, _itco_)                                     \
                                                                               \
        _auxr_clear(_cont, _items, _itco_)


#define _it_free(_cont)                                                        \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _it_total = _itco_total((_cont), _it);                                 \
                                                                               \
    for (int _idx_it = 0; _idx_it < _it_total; _idx_it++)                      \
        _iter_free((_cont)->_it[_idx_it]);                                     \
                                                                               \
    _auxr_free((_cont), _it);                                                  \
)


#define _co_free(_cont)                                                        \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _co_total = _itco_total((_cont), _co);                                 \
                                                                               \
    for (int _idx_co = 0; _idx_co < _co_total; _idx_co++)                      \
    {                                                                          \
        _it_free   ((_cont)->_co[_idx_co]);                                    \
        _block_free((_cont)->_co[_idx_co]);                                    \
        _cont_free ((_cont)->_co[_idx_co]);                                    \
    }                                                                          \
                                                                               \
    _auxr_free((_cont), _co);                                                  \
)



/* auxr management */


#define _auxr_alloc(_cont, _items, _pbase, _itco_)                             \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if ((_items) > (_cont)->_itco_##_limit && (_items) != 0)                   \
    {                                                                          \
        size_t _size = sizeof(*(_cont)->_itco_);                               \
        int _ttl = _itco_total((_cont), _itco_);                               \
                                                                               \
        void **_tmp = (void*)&((_cont)->_itco_);                               \
        *_tmp = realloc(*_tmp, (_size *                                        \
                               (unsigned)((_cont)->_itco_##_base + (_items))));\
                                                                               \
        memset((_cont)->_itco_ + _ttl, 0,                                      \
               (_size * (unsigned)((_cont)->_itco_##_base + (_items) - _ttl)));\
                                                                               \
        (_cont)->_itco_##_limit = (_items);                                    \
    }                                                                          \
                                                                               \
    *(_pbase) = (_cont)->_itco_##_base;                                        \
                                                                               \
    (_cont)->_itco_##_base  += (_items);                                       \
    (_cont)->_itco_##_limit -= (_items);                                       \
)


#define _auxr_clear(_cont, _items, _itco_)                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    (_cont)->_itco_##_base  -= (_items);                                       \
    (_cont)->_itco_##_limit += (_items);                                       \
)


#define _auxr_free(_cont, _itco_)                                              \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (_itco_total((_cont), _itco_) != 0)                                     \
        _safe_free ((_cont)->_itco_);                                          \
                                                                               \
    (_cont)->_itco_##_base  = 0;                                               \
    (_cont)->_itco_##_limit = 0;                                               \
)



/* node / block management */


#define _node_alloc(_pnode, _cont)                                             \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if ((_cont)->avsp == NULL)                                                 \
    {                                                                          \
        if ((_cont)->vcnt == 0)                                                \
        {                                                                      \
            (_cont)->pblock = (_cont)->pool;                                   \
                                                                               \
            if ((_cont)->pool != NULL && (_cont)->last != 0)                   \
                (_cont)->pool = (_cont)->pool->bnxt;                           \
                                                                               \
            if ((_cont)->last == 0)                                            \
                (_cont)->vcnt = ((_cont)->last  = (_cont)->start);             \
            else                                                               \
                (_cont)->vcnt = ((_cont)->last  < (_cont)->thrsh) ?            \
                                ((_cont)->last *= (_cont)->ratio) :            \
                                ((_cont)->last  = (_cont)->thrsh);             \
                                                                               \
            if ((_cont)->pool == NULL)                                         \
            {                                                                  \
                _safe_alloc((_cont)->pool, (sizeof(*(_cont)->pblock)) +        \
                                           (sizeof(*(_cont)->pblock->nodes)) * \
                                           (size_t)((_cont)->vcnt));           \
                                                                               \
                (_cont)->pool->bprv = (_cont)->pblock;                         \
                (_cont)->pool->ncnt = (_cont)->vcnt;                           \
                                                                               \
                (_cont)->pool->bnxt = NULL;                                    \
                if ((_cont)->pool->bprv != NULL)                               \
                    (_cont)->pool->bprv->bnxt = (_cont)->pool;                 \
            }                                                                  \
        }                                                                      \
                                                                               \
        (_pnode) = &((_cont)->pool->nodes[--(_cont)->vcnt]);                   \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        (_pnode)      = (_cont)->avsp;                                         \
        (_cont)->avsp = (_cont)->avsp->lnk[0];                                 \
    }                                                                          \
)


#define _node_clear(_pnode, _cont)                                             \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    (_pnode)->lnk[0] = (_cont)->avsp;                                          \
    (_cont)->avsp    = (_pnode);                                               \
)


#define _block_free(_cont)                                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    while ((_cont)->pool != NULL)                                              \
    {                                                                          \
        (_cont)->pblock = (_cont)->pool;                                       \
        (_cont)->pool   = (_cont)->pool->bprv;                                 \
        _safe_free((_cont)->pblock);                                           \
    }                                                                          \
)



#endif