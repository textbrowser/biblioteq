/* This file is part of the YAZ toolkit.
 * Copyright (C) 1995-2012 Index Data.
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
 * \file url.h
 * \brief Fetch URL utility
 */

#ifndef YAZ_URL_H
#define YAZ_URL_H

#include <yaz/zgdu.h>

YAZ_BEGIN_CDECL

/** \brief handle for our URL fetcher */
typedef struct yaz_url *yaz_url_t;

/** \brief creates a URL fetcher handle
    \returns handle
*/
YAZ_EXPORT yaz_url_t yaz_url_create(void);

/** \brief destroys a URL fetcher
    \param p handle
    
    Note: OK to pass NULL as p
*/
YAZ_EXPORT void yaz_url_destroy(yaz_url_t p);

/** \brief sets proxy for URL fetcher
    \param p handle
    \param proxy proxy address , e.g "localhost:3128"

    Passing a proxy of NULL disables proxy use.
*/
YAZ_EXPORT void yaz_url_set_proxy(yaz_url_t p, const char *proxy);

/** \brief executes the actual HTTP request (including redirects, etc)
    \param p handle
    \param uri URL
    \param method HTTP method
    \param headers HTTP headers to be used (NULL for no custom headers)
    \param buf content buffer for HTTP request, NULL for empty content
    \param len content length for HTTP request
    \returns HTTP response; NULL on ERROR.
*/
YAZ_EXPORT Z_HTTP_Response *yaz_url_exec(yaz_url_t p, const char *uri,
                                         const char *method,
                                         Z_HTTP_Header *headers,
                                         const char *buf, size_t len);
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

