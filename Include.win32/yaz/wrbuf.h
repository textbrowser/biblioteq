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
 * \file wrbuf.h
 * \brief Header for WRBUF (growing buffer)
 */

#ifndef WRBUF_H
#define WRBUF_H

#include <yaz/xmalloc.h>
#include <yaz/yaz-iconv.h>

YAZ_BEGIN_CDECL

/** \brief string buffer */
typedef struct wrbuf
{
    char *buf;
    size_t pos;
    size_t size;
} wrbuf, *WRBUF;

/** \brief construct WRBUF
    \returns WRBUF
 */
YAZ_EXPORT WRBUF wrbuf_alloc(void);

/** \brief destroy WRBUF and its buffer
    \param b WRBUF

    For YAZ 4.0.2 WRBUF b may be NULL.
 */
YAZ_EXPORT void wrbuf_destroy(WRBUF b);

/** \brief empty WRBUF content (length of buffer set to 0)
    \param b WRBUF
 */
YAZ_EXPORT void wrbuf_rewind(WRBUF b);

/** \brief append constant size buffer to WRBUF
    \param b WRBUF
    \param buf buffer
    \param size size of buffer
 */
YAZ_EXPORT void wrbuf_write(WRBUF b, const char *buf, size_t size);

/** \brief inserts buffer into WRBUF at some position
    \param b WRBUF
    \param pos position (0=beginning)
    \param buf buffer
    \param size size of buffer
 */
YAZ_EXPORT void wrbuf_insert(WRBUF b, size_t pos, const char *buf, size_t size);

/** \brief appends C-string to WRBUF
    \param b WRBUF
    \param buf C-string (0-terminated)
 */
YAZ_EXPORT void wrbuf_puts(WRBUF b, const char *buf);

/** \brief appends C-string to WRBUF - void pointer variant
    \param buf C-string
    \param client_data assumed WRBUF
*/
YAZ_EXPORT void wrbuf_vp_puts(const char *buf, void *client_data);

/** \brief writes buffer of certain size to WRBUF and XML encode (as CDATA)
    \param b WRBUF
    \param cp CDATA
    \param size size of CDATA
 */
YAZ_EXPORT void wrbuf_xmlputs_n(WRBUF b, const char *cp, size_t size);

/** \brief writes C-String to WRBUF and XML encode (as CDATA)
    \param b WRBUF
    \param cp CDATA buffer (0-terminated)
 */
YAZ_EXPORT void wrbuf_xmlputs(WRBUF b, const char *cp);

/** \brief puts buf to WRBUF and replaces a single char
    \param b WRBUF
    \param buf buffer to append (C-string)
    \param from character "from"
    \param to charcter "to"
*/
YAZ_EXPORT void wrbuf_puts_replace_char(WRBUF b, const char *buf,
                                        const char from, const char to);

/** \brief writes C-string to WRBUF and escape non-ASCII characters
    \param b WRBUF
    \param str C-string

    Non-ASCII characters will be presented as \\xDD .
 */
YAZ_EXPORT void wrbuf_puts_escaped(WRBUF b, const char *str);

/** \brief writes buffer to WRBUF and escape non-ASCII characters
    \param b WRBUF
    \param buf buffer
    \param len size of buffer

    Non-ASCII characters will be presented as \\xDD .
 */
YAZ_EXPORT void wrbuf_write_escaped(WRBUF b, const char *buf, size_t len);

/** \brief writes printf result to WRBUF
    \param b WRBUF
    \param fmt printf-like format
 */
YAZ_EXPORT void wrbuf_printf(WRBUF b, const char *fmt, ...)
#ifdef __GNUC__
        __attribute__ ((format (printf, 2, 3)))
#endif
        ;

/** \brief General writer of string using iconv and cdata
    \param b WRBUF
    \param cd iconv handle (0 for no conversion)
    \param buf buffer
    \param size size of buffer
    \param wfunc write handler (that takes WRBUF only)
    \returns -1 if invalid sequence was encountered (truncation in effect)
    \returns 0 if buffer could be converted and written
*/
int wrbuf_iconv_write2(WRBUF b, yaz_iconv_t cd, const char *buf,
                       size_t size,
                       void (*wfunc)(WRBUF, const char *, size_t));

/** \brief writer of string using iconv and cdata

    Obsolete: use wrbuf_iconv_write2 instead.
    \param b WRBUF
    \param cd iconv handle (0 for no conversion)
    \param buf buffer
    \param size size of buffer
    \param cdata non-zero for CDATA; 0 for cdata
    \returns -1 if invalid sequence was encountered (truncation in effect)
    \returns 0 if buffer could be converted and written
*/
int wrbuf_iconv_write_x(WRBUF b, yaz_iconv_t cd, const char *buf,
                        size_t size, int cdata)
#ifdef __GNUC__
    __attribute__ ((deprecated))
#endif
    ;

/** \brief Converts buffer using iconv and appends to WRBUF
    \param b WRBUF
    \param cd iconv handle
    \param buf buffer
    \param size size of buffer
*/
YAZ_EXPORT void wrbuf_iconv_write(WRBUF b, yaz_iconv_t cd, const char *buf,
                                 size_t size);

/** \brief Converts buffer using iconv and appends to WRBUF as XML CDATA
    \param b WRBUF
    \param cd iconv handle
    \param buf buffer
    \param size size of buffer
*/
YAZ_EXPORT void wrbuf_iconv_write_cdata(WRBUF b, yaz_iconv_t cd,
                                       const char *buf, size_t size);

/** \brief iconv converts C-string and appends to WRBUF
    \param b WRBUF
    \param cd iconv handle
    \param str C-string
*/
YAZ_EXPORT void wrbuf_iconv_puts(WRBUF b, yaz_iconv_t cd, const char *str);

/** \brief iconv converts C-string and appends to WRBUF as XML CDATA
    \param b WRBUF
    \param cd iconv handle
    \param str C-string
*/
YAZ_EXPORT void wrbuf_iconv_puts_cdata(WRBUF b, yaz_iconv_t cd,
                                       const char *str);

/** \brief iconv converts character and appends to WRBUF
    \param b WRBUF
    \param cd iconv handle
    \param ch character
*/
YAZ_EXPORT void wrbuf_iconv_putchar(WRBUF b, yaz_iconv_t cd, int ch);

/** \brief iconv reset(flush) to WRBUF
    \param b
    \param cd iconv handle

    This function calls iconv(cd, 0, 0, ..) to make it
    flush any remaining content.
*/
YAZ_EXPORT void wrbuf_iconv_reset(WRBUF b, yaz_iconv_t cd);

/** \brief chips traling blanks away from WRBUF
    \param b WRBUF
*/
YAZ_EXPORT void wrbuf_chop_right(WRBUF b);

/** \brief cut size of WRBUF
    \param b WRBUF
    \param no_to_remove number of bytes to remove
 */
YAZ_EXPORT void wrbuf_cut_right(WRBUF b, size_t no_to_remove);

/** \brief grow WRBUF larger
    \param b WRBUF
    \param minsize make WRBUF at least this size

    This function is normally not used by applications
*/
YAZ_EXPORT int wrbuf_grow(WRBUF b, size_t minsize);

#define wrbuf_len(b) ((b)->pos)
#define wrbuf_buf(b) ((b)->buf)

/** \brief returns WRBUF content as C-string
    \param b WRBUF (may not be NULL)
    \returns C-string
*/
YAZ_EXPORT const char *wrbuf_cstr(WRBUF b);

/** \brief returns WRBUF content as C-string or NULL
    \param b WRBUF
    \returns C-string or NULL

    This function returns NULL if either b is NULL or length of buffer is 0
*/
YAZ_EXPORT
const char *wrbuf_cstr_null(WRBUF b);

#define wrbuf_putc(b, c) \
    ((void) ((b)->pos >= (b)->size ? wrbuf_grow(b, 1) : 0),  \
    (b)->buf[(b)->pos++] = (c))


/** \brief writes JSON text to WRBUF with escaping
    \param b result
    \param str input string to be encoded
*/
YAZ_EXPORT
void wrbuf_json_puts(WRBUF b, const char *str);

/** \brief writes JSON text to WRBUF with escaping
    \param b result
    \param cp char buffer
    \param sz size of char buffer
*/
YAZ_EXPORT
void wrbuf_json_write(WRBUF b, const char *cp, size_t sz);

YAZ_EXPORT
void wrbuf_iconv_json_write(WRBUF b, yaz_iconv_t cd,
                            const char *buf, size_t size);

YAZ_EXPORT
void wrbuf_iconv_json_puts(WRBUF b, yaz_iconv_t cd, const char *strz);

/** \brief writes SHA1 text to WRBUF
    \param b result
    \param cp char buffer
    \param sz size of char buffer
    \param hexit 1=hex mode; 0=binary
    \returns 0 if successful
    \returns -1 on error
*/
YAZ_EXPORT
int wrbuf_sha1_write(WRBUF b, const char *cp, size_t sz, int hexit);

/** \brief writes SHA1 text to WRBUF
    \param b result
    \param cp C-string
    \param hexit 1=hex mode; 0=binary
    \returns 0 if successful
    \returns -1 on error
*/
YAZ_EXPORT
int wrbuf_sha1_puts(WRBUF b, const char *cp, int hexit);

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

