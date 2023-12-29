/* This file is part of the YAZ toolkit.
 * Copyright (C) Index Data.
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Index Data nor the names of its contributors
 *       may be used to endorse or promote products derived from this
 *       software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file shptr.h
 * \brief Shared pointer macros
 */

#ifndef YAZ_SHARED_PTR_H
#define YAZ_SHARED_PTR_H

#include <yaz/xmalloc.h>
#include <yaz/mutex.h>

YAZ_BEGIN_CDECL

#define YAZ_SHPTR_TYPE(type) \
    struct type##_shptr \
    {                          \
    type ptr;                  \
    int ref;                   \
    YAZ_MUTEX mutex;           \
    };  \
    typedef struct type##_shptr *type##_shptr_t;

#define YAZ_SHPTR_INIT(p,n) {                   \
        p = xmalloc(sizeof(*p));                \
        p->ptr = n;                             \
        p->ref = 1;                             \
        p->mutex = 0;  \
        yaz_mutex_create(&p->mutex);            \
    }

#define YAZ_SHPTR_INC(p) {                      \
        yaz_mutex_enter(p->mutex);              \
        p->ref++;                               \
        yaz_mutex_leave(p->mutex);              \
    }

#define YAZ_SHPTR_DEC(p, destroy)  {             \
    yaz_mutex_enter(p->mutex);                   \
    if (--p->ref == 0) {                         \
        yaz_mutex_leave(p->mutex);               \
        destroy(p->ptr);                         \
        yaz_mutex_destroy(&p->mutex);            \
        xfree(p);                                \
        p = 0;                                   \
    } else { \
    yaz_mutex_leave(p->mutex); \
    } \
    }

YAZ_END_CDECL

#endif
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

