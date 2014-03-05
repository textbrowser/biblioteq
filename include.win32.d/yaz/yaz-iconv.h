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
 * \file yaz-iconv.h
 * \brief Header for YAZ iconv interface
 */

#ifndef YAZ_ICONV_H
#define YAZ_ICONV_H

#include <stddef.h>
#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** \brief yaz_iconv handle (similar to iconv_t) */
typedef struct yaz_iconv_struct *yaz_iconv_t;

/** \brief error code: unknown */
#define YAZ_ICONV_UNKNOWN 1
/** \brief error code: Not sufficient room for output buffer */
#define YAZ_ICONV_E2BIG 2
/** \brief error code: Invalid sequence */
#define YAZ_ICONV_EILSEQ 3
/** \brief error code: An incomplete multibyte sequence is in input buffer */
#define YAZ_ICONV_EINVAL 4

/** \brief just like iconv_open(3) */
YAZ_EXPORT yaz_iconv_t yaz_iconv_open(const char *tocode, const char *fromcode);
/** \brief just like iconv(3) */
YAZ_EXPORT size_t yaz_iconv(yaz_iconv_t cd, char **inbuf, size_t *inbytesleft,
                            char **outbuf, size_t *outbytesleft);
/** \brief returns last error - like errno for iconv(3) */
YAZ_EXPORT int yaz_iconv_error(yaz_iconv_t cd);

/** \brief just like iconv_close(3) */
YAZ_EXPORT int yaz_iconv_close(yaz_iconv_t cd);

/** \brief tests whether conversion is handled by YAZ' iconv or system iconv */
YAZ_EXPORT int yaz_iconv_isbuiltin(yaz_iconv_t cd);

YAZ_EXPORT unsigned long yaz_read_UTF8_char(const unsigned char *inp,
                                            size_t inbytesleft,
                                            size_t *no_read,
                                            int *error);

/** \brief encodes UTF-8 sequence
    \param x the UNICODE value
    \param outbuf output buffer pointer, updated if conversion is successful
    \param outbytesleft length of buffer, updated if conversino is successful
    \param error pointer to error code if error occurs
    \retval 0 if successful
    \retval -1 for error
*/
YAZ_EXPORT size_t yaz_write_UTF8_char(unsigned long x,
                                      char **outbuf, size_t *outbytesleft,
                                      int *error);

/* ctype.h macros ASCII based. That do not depend on LANG, nor require
   unsigned int as argument */
#define yaz_isdigit(x) ((x) >= '0' && (x) <= '9')
#define yaz_isspace(x) strchr(" \f\r\n\t\v", x)
#define yaz_toupper(x) ((x) + ('A' - 'a'))
#define yaz_isupper(x) ((x) >= 'A' && (x) <= 'Z')
#define yaz_tolower(x) ((x) + ('a' - 'A'))
#define yaz_islower(x) ((x) >= 'a' && (x) <= 'z')

/** \brief check whether string apppers to be UTF-8 encoded
    \param cstr string to check
    \retval 1 OK (appears to be UTF-8)
    \retval 0 definitely not UTF-8
*/
YAZ_EXPORT int yaz_utf8_check(const char *cstr);

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

