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
 * \file matchstr.h
 * \brief Header for YAZ iconv interface
 */

#ifndef YAZ_MATCH_STR_H
#define YAZ_MATCH_STR_H

#include <stddef.h>
#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** \brief match strings - independent of case and '-'
    \param s1 first string
    \param s2 second string (May include wildcard ? and .)
    \retval 0 strings are similar
    \retval !=0 strings are different
*/
YAZ_EXPORT int yaz_matchstr(const char *s1, const char *s2);

/** \brief match a and b with some delimitor for b
    \param a first second
    \param b second string
    \param b_del delimitor for b
    \retval 0 strings are similar
    \retval !=0 strings are different
*/
YAZ_EXPORT int yaz_strcmp_del(const char *a, const char *b, const char *b_del);


/** \brief compares two buffers of different size
    \param a first buffer
    \param b second buffer
    \param len_a length of first buffer
    \retval len_b length of second buffer
    \retval 0 buffers are equal
    \retval >0 a > b
    \retval <0 a < b
*/
YAZ_EXPORT int yaz_memcmp(const void *a, const void *b,
                          size_t len_a, size_t len_b);

/** \brief ala strncasecmp - no locale
    \param s1 first buffer
    \param s2 second buffer
    \param n number of bytes to compare
    \retval 0 buffers are equal
    \retval >0 a > b
    \retval <0 a < b
*/
YAZ_EXPORT int yaz_strncasecmp(const char *s1, const char *s2, size_t n);

/** \brief ala strcasecmp - no locale
    \param s1 first buffer
    \param s2 second buffer
    \retval 0 buffers are equal
    \retval >0 a > b
    \retval <0 a < b
*/
YAZ_EXPORT int yaz_strcasecmp(const char *s1, const char *s2);

/** \brief strcmp, null may be passed
    \param s1 first buffer or NULL
    \param s2 second buffer or NULL
    \retval 0 buffers are equal
    \retval >0 a > b
    \retval <0 a < b
*/
YAZ_EXPORT int yaz_strcmp_null(const char *s1, const char *s2);

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

