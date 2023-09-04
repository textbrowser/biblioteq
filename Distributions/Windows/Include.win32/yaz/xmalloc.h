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
 * \file xmalloc.h
 * \brief Header for memory handling functions.
 *
 * This is a set of wrapper functions for the memory allocation routines
 * from stdlib.h.. Such as malloc, free, realloc etc.
 * These functions calls exit if memory allocation fails.
 */

#ifndef XMALLOC_H
#define XMALLOC_H

#include <stddef.h>

#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** \brief utility macro which calls xrealloc_f */
#define xrealloc(o, x) xrealloc_f(o, x, __FILE__, __LINE__)
/** \brief utility macro which calls malloc_f */
#define xmalloc(x) xmalloc_f(x, __FILE__, __LINE__)
/** \brief utility macro which calls xcalloc_f */
#define xcalloc(x,y) xcalloc_f(x,y, __FILE__, __LINE__)
/** \brief utility macro which calls xfree_f */
#define xfree(x) xfree_f(x, __FILE__, __LINE__)
/** \brief utility macro which calls xstrdup_f */
#define xstrdup(s) xstrdup_f(s, __FILE__, __LINE__)
/** \brief utility macro which calls xstrndup_f */
#define xstrndup(s, n) xstrndup_f(s, n, __FILE__, __LINE__)
/** \brief utility macro which calls malloc_trav_f */
#define xmalloc_trav(s) xmalloc_trav_f(s, __FILE__, __LINE__)

/** \brief realloc
    \param o buffer to be reallocated
    \param size size of buffer to be allocated
    \param file fname location of use
    \param line line location of use
    \returns buffer

    This function is invoked via macro xrealloc in which file and line are set
    automatically.
*/

YAZ_EXPORT void *xrealloc_f(void *o, size_t size, const char *file, int line);
/** \brief malloc
    \param size size of buffer to be allocated
    \param file fname location of use
    \param line line location of use
    \returns buffer

    This function is invoked via macro xmalloc in which file and line are set
    automatically.
*/
YAZ_EXPORT void *xmalloc_f(size_t size, const char *file, int line);

/** \brief calloc
    \param nmemb number of members
    \param size size of member
    \param file fname location of use
    \param line line location of use
    \returns buffer

    This function is invoked via macro xcalloc in which file and line are set
    automatically.
*/
YAZ_EXPORT void *xcalloc_f(size_t nmemb, size_t size,
                           const char *file, int line);
/** \brief strdup
    \param p string to be cloned
    \param file fname location of use
    \param line line location of use
    \returns resulting string

    This function is invoked via macro xstrdup in which file and line are set
    automatically.
*/
YAZ_EXPORT char *xstrdup_f(const char *p, const char *file, int line);

/** \brief strndup
    \param p string to be cloned
    \param n max size of resulting string (excluding 0)
    \param file fname location of use
    \param line line location of use
    \returns resulting string

    This function is invoked via macro xstrndup in which file and line are set
    automatically.
*/
YAZ_EXPORT char *xstrndup_f(const char *p, size_t n,
                            const char *file, int line);

/** \brief free
    \param p string to be freed (might be NULL)
    \param file fname location of use
    \param line line location of use

    This function is invoked via macro xfree in which file and line are set
    automatically.
*/
YAZ_EXPORT void xfree_f(void *p, const char *file, int line);

/** \brief logs all xmalloc buffers
    \param s not used
    \param file fname location of use
    \param line line location of use

    This function is invoked via macro xmalloc_trav in which file and line
    are set automatically. Only if TRACE_XMALLOC > 1 this function
    does any real work!
*/
YAZ_EXPORT void xmalloc_trav_f(const char *s, const char *file, int line);

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

