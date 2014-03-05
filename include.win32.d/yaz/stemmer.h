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
 * \file stemmer.h
 * \brief Header for the stemming API.
 *
 * Simple wrapper to hide underlying implementation.
 */

#ifndef YAZ_STEMMER_H
#define YAZ_STEMMER_H

#include <yaz/yconfig.h>
#include <yaz/xmltypes.h>
#include <yaz/icu_I18N.h>
#include <yaz/log.h>

#include <unicode/ustring.h>  /* some more string fcns*/
#include <unicode/uchar.h>    /* char names           */


YAZ_BEGIN_CDECL

typedef struct yaz_stemmer_t *yaz_stemmer_p;

YAZ_EXPORT
yaz_stemmer_p yaz_stemmer_create(const char *locale, const char *rule, UErrorCode *status);

YAZ_EXPORT
yaz_stemmer_p yaz_stemmer_clone(yaz_stemmer_p stemmer);

YAZ_EXPORT
void yaz_stemmer_stem(yaz_stemmer_p stemmer, struct icu_buf_utf16 *dst, struct icu_buf_utf16* src, UErrorCode *status);

YAZ_EXPORT
void yaz_stemmer_destroy(yaz_stemmer_p stemmer);

#endif
