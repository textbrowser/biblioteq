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
 * \file soap.h
 * \brief Header for SOAP
 */

#ifndef YAZ_SOAP_H
#define YAZ_SOAP_H

#include <yaz/odr.h>

YAZ_BEGIN_CDECL

typedef struct {
    char *fault_code;
    char *fault_string;
    char *details;
} Z_SOAP_Fault;

typedef struct {
    int no;
    char *ns;
    void *p;
} Z_SOAP_Generic;

#define Z_SOAP_fault 1
#define Z_SOAP_generic 2
#define Z_SOAP_error 3
typedef struct {
    int which;
    union {
        Z_SOAP_Fault   *fault;
        Z_SOAP_Generic *generic;
        Z_SOAP_Fault   *soap_error;
    } u;
    const char *ns;
} Z_SOAP;

typedef int (*Z_SOAP_fun)(ODR o, void * ptr, void **handler_data,
                         void *client_data, const char *ns);
typedef struct {
    char *ns;
    void *client_data;
    Z_SOAP_fun f;
} Z_SOAP_Handler;

YAZ_EXPORT int z_soap_codec(ODR o, Z_SOAP **pp,
                            char **content_buf, int *content_len,
                            Z_SOAP_Handler *handlers);
YAZ_EXPORT int z_soap_codec_enc(ODR o, Z_SOAP **pp,
                                char **content_buf, int *content_len,
                                Z_SOAP_Handler *handlers,
                                const char *encoding);
YAZ_EXPORT int z_soap_codec_enc_xsl(ODR o, Z_SOAP **pp,
                                    char **content_buf, int *content_len,
                                    Z_SOAP_Handler *handlers,
                                    const char *encoding,
                                    const char *stylesheet);

YAZ_EXPORT int z_soap_error(ODR o, Z_SOAP *p,
                            const char *fault_code, const char *fault_string,
                            const char *details);

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

