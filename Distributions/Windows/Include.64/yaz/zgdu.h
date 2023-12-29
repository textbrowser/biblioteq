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
 * \file zgdu.h
 * \brief Header for the Z_GDU (HTTP or Z39.50 package)
 */

#ifndef Z_GDU_H
#define Z_GDU_H

#include <yaz/z-core.h>

YAZ_BEGIN_CDECL

typedef struct Z_HTTP_Header Z_HTTP_Header;

struct Z_HTTP_Header {
    char *name;
    char *value;
    Z_HTTP_Header *next;
};

typedef struct {
    char *method;
    char *version;
    char *path;
    Z_HTTP_Header *headers;
    char *content_buf;
    int content_len;
} Z_HTTP_Request;

typedef struct {
    int code;
    char *version;
    Z_HTTP_Header *headers;
    char *content_buf;
    int content_len;
} Z_HTTP_Response;

#define Z_GDU_Z3950         1
#define Z_GDU_HTTP_Request  2
#define Z_GDU_HTTP_Response 3
typedef struct {
    int which;
    union {
        Z_APDU *z3950;
        Z_HTTP_Request *HTTP_Request;
        Z_HTTP_Response *HTTP_Response;
    } u;
} Z_GDU ;
YAZ_EXPORT int z_GDU (ODR o, Z_GDU **p, int opt, const char *name);
YAZ_EXPORT void z_HTTP_header_add(ODR o, Z_HTTP_Header **hp, const char *n,
                                  const char *v);
YAZ_EXPORT void z_HTTP_header_set(ODR o, Z_HTTP_Header **hp, const char *n,
                                  const char *v);
YAZ_EXPORT void z_HTTP_header_add_content_type(ODR o, Z_HTTP_Header **hp,
                                               const char *content_type,
                                               const char *charset);
YAZ_EXPORT void z_HTTP_header_add_basic_auth(ODR o, Z_HTTP_Header **hp,
                                             const char *username,
                                             const char *password);
YAZ_EXPORT const char *z_HTTP_header_lookup(const Z_HTTP_Header *hp,
                                            const char *n);
YAZ_EXPORT const char *z_HTTP_header_remove(Z_HTTP_Header **hp,
                                            const char *n);

YAZ_EXPORT const char *z_HTTP_errmsg(int code);

YAZ_EXPORT Z_GDU *z_get_HTTP_Response(ODR o, int code);
YAZ_EXPORT Z_GDU *z_get_HTTP_Response_details(ODR o, int code,
                                              const char *details);
YAZ_EXPORT Z_GDU *z_get_HTTP_Response_server(ODR o,
                                             int code, const char *details,
                                             const char *server,
                                             const char *server_url);
YAZ_EXPORT Z_GDU *z_get_HTTP_Request(ODR o);
YAZ_EXPORT Z_GDU *z_get_HTTP_Request_host_path(ODR odr,
                                               const char *host,
                                               const char *path);
YAZ_EXPORT Z_GDU *z_get_HTTP_Request_uri(ODR odr, const char *uri,
                                         const char *args,
                                         int use_full_uri);
YAZ_EXPORT int yaz_decode_http_request(ODR o, Z_HTTP_Request **hr_p);
YAZ_EXPORT int yaz_decode_http_response(ODR o, Z_HTTP_Response **hr_p);
YAZ_EXPORT int yaz_encode_http_response(ODR o, Z_HTTP_Response *hr);
YAZ_EXPORT int yaz_encode_http_request(ODR o, Z_HTTP_Request *hr);

YAZ_EXPORT const char *yaz_check_location(ODR odr, const char *uri,
                                    const char *location, int *host_change);
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

