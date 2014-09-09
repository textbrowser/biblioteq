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
 * \file
 * \brief Header for Nibble Memory functions
 *
 * This is a simple and fairly wasteful little module for nibble memory
 * allocation. Eventually we'll put in something better.
 */
#ifndef NMEM_H
#define NMEM_H

#include <stddef.h>
#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** \brief NMEM handle (an opaque pointer to memory) */
typedef struct nmem_control *NMEM;

/** \brief Set to 1 if YAZ BER integer is 64-bit ; 0 otherwise */
#ifndef NMEM_64
#define NMEM_64 1
#endif

#if NMEM_64

#ifdef _MSC_VER
/* Visual Studio. 6.0 and later supports this */
typedef __int64 nmem_int_t;
#define NMEM_INT_PRINTF "%I64d"
#else
/* C99 */
typedef long long int nmem_int_t;
#define NMEM_INT_PRINTF "%lld"
#endif

#else
/** \brief BER/utility integer (32-bit on most platforms) */
typedef int nmem_int_t;
/** \brief printf format for nmem_int_t type */
#define NMEM_INT_PRINTF "%d"
#endif

/** \brief BER/utility boolean */
typedef int nmem_bool_t;

/** \brief releases memory associaged with an NMEM handle
    \param n NMEM handle
*/
YAZ_EXPORT void nmem_reset(NMEM n);

/** \brief returns size in bytes of memory for NMEM handle
    \returns number of bytes
 */
YAZ_EXPORT size_t nmem_total(NMEM n);

/** \brief allocates string on NMEM handle (similar strdup)
    \param mem HNEM handle
    \param src string
    \returns duplicated string
 */
YAZ_EXPORT char *nmem_strdup(NMEM mem, const char *src);
/** \brief allocates string on NMEM handle - allows NULL ptr buffer
    \param mem HNEM handle
    \param src string
    \returns duplicated string or NULL if src was NULL
 */
YAZ_EXPORT char *nmem_strdup_null(NMEM mem, const char *src);

/** \brief allocates string of certain size on NMEM handle
    \param mem NMEM handle
    \param src string
    \param n size of string
    \returns duplicated string (0 terminated)
 */
YAZ_EXPORT char *nmem_strdupn(NMEM mem, const char *src, size_t n);

/** \brief allocates sub strings out of string using certain delimitors
    \param nmem NMEM handle
    \param delim delimitor chars (splits on each char in there)
    \param dstr string to be split
    \param darray result string array for each sub string
    \param num number of result strings
*/
YAZ_EXPORT void nmem_strsplit(NMEM nmem, const char *delim,
                              const char *dstr,
                              char ***darray, int *num);

/** \brief allocates sub strings out of string using certain delimitors
    \param nmem NMEM handle
    \param delim delimitor chars (splits on each char in there)
    \param dstr string to be split
    \param darray result string array for each sub string
    \param num number of result strings
    \param collapse 1=collapse multiple delims to one; 0=no collapse
*/
YAZ_EXPORT void nmem_strsplitx(NMEM nmem, const char *delim,
                               const char *dstr,
                               char ***darray, int *num,
                               int collapse);

/** \brief splits string into sub strings delimited by blanks
    \param nmem NMEM handle
    \param dstr string to be split
    \param darray result string array for each sub string
    \param num number of result strings
*/
YAZ_EXPORT void nmem_strsplit_blank(NMEM nmem, const char *dstr,
                                    char ***darray, int *num);


/** \brief allocates sub strings out of string using certain delimitors
    \param nmem NMEM handle
    \param delim delimitor chars (splits on each char in there)
    \param dstr string to be split
    \param darray result string array for each sub string
    \param num number of result strings
    \param collapse 1=collapse multiple delims to one; 0=no collapse
    \param escape_char != 0, an escape char (could be \)
*/
YAZ_EXPORT void nmem_strsplit_escape(NMEM nmem, const char *delim,
                                     const char *dstr,
                                     char ***darray, int *num, int collapse,
                                     int escape_char);

/** \brief allocates sub strings out of string using certain delimitors
    \param nmem NMEM handle
    \param delim delimitor chars (splits on each char in there)
    \param dstr string to be split
    \param darray result string array for each sub string
    \param num number of result strings
    \param collapse 1=collapse multiple delims to one; 0=no collapse
    \param escape_char != 0, an escape char (could be \)
    \param subst_escape 1=substitute escapes, 0 leave as is
*/
YAZ_EXPORT void nmem_strsplit_escape2(NMEM nmem, const char *delim,
                                      const char *dstr,
                                      char ***darray, int *num, int collapse,
                                      int escape_char, int subst_escape);

/** \brief allocates and sets integer for NMEM
    \param nmem NMEM handle
    \param v integer value
    \returns pointer to created integer
*/
YAZ_EXPORT nmem_int_t *nmem_intdup(NMEM nmem, nmem_int_t v);

/** \brief allocates and sets boolean for NMEM
    \param nmem NMEM handle
    \param v value (0=false, != 0 true)
    \returns pointer to created boolean
*/
YAZ_EXPORT nmem_bool_t *nmem_booldup(NMEM nmem, nmem_bool_t v);

/** \brief transfers memory from one NMEM handle to another
    \param src source NMEM handle
    \param dst destination NMEM handle
 */
YAZ_EXPORT void nmem_transfer(NMEM dst, NMEM src);

/** \brief returns new NMEM handle
    \returns NMEM handle
 */
YAZ_EXPORT NMEM nmem_create(void);

/** \brief destroys NMEM handle and memory associated with it
    \param n NMEM handle
 */
YAZ_EXPORT void nmem_destroy(NMEM n);

/** \brief allocates memory block on NMEM handle
    \param n NMEM handle
    \param size number of bytes to be allocated
    \returns pointer to allocated memory
 */
YAZ_EXPORT void *nmem_malloc(NMEM n, size_t size);

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

